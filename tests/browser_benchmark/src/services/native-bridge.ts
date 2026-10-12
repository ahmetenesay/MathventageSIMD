import {
  LiveTelemetryFrame,
  BenchmarkConfig,
  HardwareCeilings,
  RooflineKernelPoint,
  LatencySample
} from '../types/telemetry';

export type FrameCallback = (frame: LiveTelemetryFrame) => void;
export type ConnectionStatus = 'CONNECTED_NATIVE' | 'CONNECTING' | 'SIMULATION_MODE' | 'DISCONNECTED';

export class NativeBridgeService {
  private ws: WebSocket | null = null;
  private wsUrl: string = 'ws://localhost:9099/stream';
  private httpUrl: string = 'http://localhost:9099/telemetry';
  private frameCallbacks: Set<FrameCallback> = new Set();
  private statusCallbacks: Set<(status: ConnectionStatus) => void> = new Set();
  private status: ConnectionStatus = 'DISCONNECTED';
  private simulationInterval: number | null = null;
  private httpPollingInterval: number | null = null;
  private autoReconnectTimer: number | null = null;

  // Active Simulated State for realistic fallback
  private simFrameIndex = 0;
  private simEggCount = 42;
  private simFledglingCount = 180;
  private simPitstopCount = 3;
  private simFallineBoxes = 64;
  private simTankBytes = 64 * 1024 * 1024;
  private simLandfillBytes = 12 * 1024 * 1024;
  private simTotalAlloc = 128 * 1024 * 1024;
  private simRmrpReclaimed = 8 * 1024 * 1024;
  private activeConfig: BenchmarkConfig = {
    kernel: 'dense_gemm_f32',
    workloadSize: 65536,
    targetIsa: 'AVX2',
    profile: 'NORMAX',
    allowAluDispatch: true,
    rmrpMethod: 'PLUMP',
    runMode: 'CONTINUOUS'
  };

  public readonly hardwareCeilings: HardwareCeilings = {
    cpuModel: 'Intel Core / Hybrid Architecture (x86_64)',
    tier: 'ORTA_ORTA',
    mhz: 4200,
    pCores: 6,
    eCores: 8,
    l1dBandwidthGBs: 3400.0,
    l2BandwidthGBs: 1850.0,
    l3BandwidthGBs: 580.0,
    dramBandwidthGBs: 76.8,
    scalarPeakGflops: 67.2,
    ssePeakGflops: 268.8,
    avx2PeakGflops: 1075.2,
    avx512PeakGflops: 2150.4,
    npuPeakGflops: 4800.0
  };

  constructor() {
    this.connect();
  }

  public connect(): void {
    if (this.ws && (this.ws.readyState === WebSocket.OPEN || this.ws.readyState === WebSocket.CONNECTING)) {
      return;
    }

    this.setStatus('CONNECTING');

    try {
      this.ws = new WebSocket(this.wsUrl);

      this.ws.onopen = () => {
        this.setStatus('CONNECTED_NATIVE');
        this.stopSimulation();
        this.stopHttpPolling();
      };

      this.ws.onmessage = (event) => {
        try {
          const frame: LiveTelemetryFrame = JSON.parse(event.data);
          this.notifyFrame(frame);
        } catch (e) {
          console.error('Failed to parse telemetry frame from native daemon:', e);
        }
      };

      this.ws.onerror = () => {
        // WebSocket connection failed; immediately attempt HTTP polling fallback
        this.startHttpPolling();
      };

      this.ws.onclose = () => {
        if (this.status === 'CONNECTED_NATIVE') {
          this.startHttpPolling();
        }
        this.scheduleReconnect();
      };
    } catch {
      this.startHttpPolling();
      this.scheduleReconnect();
    }
  }

  private startHttpPolling(): void {
    if (this.httpPollingInterval !== null) return;

    this.httpPollingInterval = window.setInterval(async () => {
      // If WebSocket reconnected in the meantime, stop polling
      if (this.ws && this.ws.readyState === WebSocket.OPEN) {
        this.stopHttpPolling();
        return;
      }

      try {
        const res = await fetch(this.httpUrl, { cache: 'no-store' });
        if (res.ok) {
          const frame: LiveTelemetryFrame = await res.json();
          this.setStatus('CONNECTED_NATIVE');
          this.stopSimulation();
          this.notifyFrame(frame);
          return;
        }
      } catch {
        // Bridge daemon not running at all
      }

      // If HTTP polling also failed, drop to high-fidelity simulation
      if (this.status !== 'SIMULATION_MODE') {
        this.startSimulation();
      }
    }, 150);
  }

  private stopHttpPolling(): void {
    if (this.httpPollingInterval !== null) {
      clearInterval(this.httpPollingInterval);
      this.httpPollingInterval = null;
    }
  }

  private scheduleReconnect(): void {
    if (this.autoReconnectTimer !== null) return;
    this.autoReconnectTimer = window.setTimeout(() => {
      this.autoReconnectTimer = null;
      if (!this.ws || this.ws.readyState === WebSocket.CLOSED) {
        this.connect();
      }
    }, 3000);
  }

  public setStatus(newStatus: ConnectionStatus): void {
    if (this.status === newStatus) return;
    this.status = newStatus;
    for (const cb of this.statusCallbacks) {
      cb(newStatus);
    }
  }

  public getStatus(): ConnectionStatus {
    return this.status;
  }

  public onFrame(callback: FrameCallback): () => void {
    this.frameCallbacks.add(callback);
    return () => this.frameCallbacks.delete(callback);
  }

  public onStatusChange(callback: (status: ConnectionStatus) => void): () => void {
    this.statusCallbacks.add(callback);
    callback(this.status);
    return () => this.statusCallbacks.delete(callback);
  }

  public async sendConfig(config: BenchmarkConfig): Promise<void> {
    this.activeConfig = { ...config };
    const payload = JSON.stringify({ type: 'CONFIGURE', config });

    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(payload);
    } else {
      try {
        await fetch('http://localhost:9099', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: payload
        });
      } catch {
        // Simulation mode active
      }
    }
  }

  public async triggerRMRP(method: 'PLUMP' | 'SEQUENTIAL'): Promise<void> {
    const payload = JSON.stringify({ type: 'TRIGGER_RMRP', method });
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(payload);
    } else {
      try {
        await fetch('http://localhost:9099', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: payload
        });
      } catch {
        // Fallback simulated RMRP effect
        const reclaimed = Math.floor(Math.random() * 4 + 2) * 1024 * 1024;
        this.simRmrpReclaimed += reclaimed;
        this.simFallineBoxes = Math.max(8, this.simFallineBoxes - 20);
        this.simTotalAlloc = Math.max(16 * 1024 * 1024, this.simTotalAlloc - reclaimed);
      }
    }
  }

  public async triggerPitstopExcise(): Promise<void> {
    const payload = JSON.stringify({ type: 'TRIGGER_PITSTOP' });
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(payload);
    } else {
      try {
        await fetch('http://localhost:9099', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: payload
        });
      } catch {
        if (this.simEggCount > 0) {
          this.simEggCount--;
          this.simPitstopCount++;
          setTimeout(() => {
            this.simPitstopCount = Math.max(0, this.simPitstopCount - 1);
            this.simFledglingCount++;
          }, 1200);
        }
      }
    }
  }

  public async triggerSelfHealCheck(): Promise<void> {
    const payload = JSON.stringify({ type: 'TRIGGER_SELF_HEAL' });
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(payload);
    } else {
      try {
        await fetch('http://localhost:9099', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: payload
        });
      } catch {
        // Simulation mode active
      }
    }
  }

  private notifyFrame(frame: LiveTelemetryFrame): void {
    for (const cb of this.frameCallbacks) {
      cb(frame);
    }
  }

  // --- High-Fidelity Realistic Simulation Engine ---
  private startSimulation(): void {
    this.setStatus('SIMULATION_MODE');
    if (this.simulationInterval !== null) return;

    this.simulationInterval = window.setInterval(() => {
      this.generateSimulatedFrame();
    }, 100); // 10 updates per second (100ms)
  }

  private stopSimulation(): void {
    if (this.simulationInterval !== null) {
      clearInterval(this.simulationInterval);
      this.simulationInterval = null;
    }
  }

  private generateSimulatedFrame(): void {
    this.simFrameIndex++;
    const now = performance.now();

    // Workload-dependent math calculations
    let baseGflops = 240;
    let arithmeticIntensity = 4.0; // FLOPs/Byte default

    switch (this.activeConfig.targetIsa) {
      case 'AVX512':
        baseGflops = 1420;
        break;
      case 'AVX2':
        baseGflops = 720;
        break;
      case 'SSE4_2':
        baseGflops = 210;
        break;
      case 'SCALAR':
        baseGflops = 48;
        break;
      case 'OPENVINO_NPU':
        baseGflops = 2650;
        break;
      case 'WASM_SIMD128':
        baseGflops = 160;
        break;
    }

    if (this.activeConfig.kernel === 'dense_gemm_f32') {
      arithmeticIntensity = 16.0; // Highly compute-bound
      baseGflops *= 1.25;
    } else if (this.activeConfig.kernel === 'vector_fma') {
      arithmeticIntensity = 2.0; // Memory bandwidth bound
      baseGflops *= 0.65;
    } else if (this.activeConfig.kernel === 'lattice_fdtd_3d') {
      arithmeticIntensity = 3.5;
      baseGflops *= 0.85;
    } else if (this.activeConfig.kernel === 'cordic_sin_cos') {
      arithmeticIntensity = 8.0;
      baseGflops *= 0.95;
    } else if (this.activeConfig.kernel === 'neumaier_sum') {
      arithmeticIntensity = 1.0;
      baseGflops *= 0.55;
    }

    // Dynamic jitter / variation simulating CPU cache noise and frequency boost
    const jitter = (Math.sin(this.simFrameIndex * 0.25) * 0.08) + ((Math.random() - 0.5) * 0.04);
    const currentGflops = Math.max(10, baseGflops * (1 + jitter));
    const throughputGBs = (currentGflops / arithmeticIntensity);
    const estimatedIPC = Math.min(3.8, (currentGflops / (this.hardwareCeilings.mhz * 0.001 * 16)) * 2.2);

    // Evolve memory topography
    if (this.simFrameIndex % 6 === 0) {
      if (Math.random() > 0.4) {
        this.simEggCount += Math.floor(Math.random() * 4);
      }
      if (this.simEggCount > 10 && Math.random() > 0.5) {
        const moved = Math.min(this.simEggCount, Math.floor(Math.random() * 3 + 1));
        this.simEggCount -= moved;
        this.simFledglingCount += moved;
      }
      if (Math.random() > 0.8) {
        this.simFallineBoxes += Math.floor(Math.random() * 5);
      }
    }

    // Generate recent latency samples with micro-jitter
    const recentLatencies: LatencySample[] = [];
    const baseLatencyNs = (this.activeConfig.workloadSize / (currentGflops * 1e9)) * 1e9 * 1000;
    const cpuCyclesPerNs = this.hardwareCeilings.mhz / 1000.0;

    for (let i = 0; i < 8; ++i) {
      const latNoise = (Math.random() - 0.48) * 0.15;
      const ns = Math.max(12, baseLatencyNs * (1 + latNoise));
      recentLatencies.push({
        timestamp: now - (8 - i) * 12,
        cycles: Math.floor(ns * cpuCyclesPerNs),
        nanoseconds: Math.round(ns),
        kernelName: this.activeConfig.kernel,
        isa: this.activeConfig.targetIsa
      });
    }

    // Roofline points
    const rooflinePoints: RooflineKernelPoint[] = [
      {
        id: 'dense_gemm_f32',
        name: 'Dense GEMM FP32',
        arithmeticIntensity: 18.5,
        performanceGflops: this.activeConfig.targetIsa === 'AVX512' ? 1680 : 890,
        isa: this.activeConfig.targetIsa,
        boundType: 'compute_bound',
        timestamp: now
      },
      {
        id: 'lattice_fdtd_3d',
        name: 'Lattice FDTD 3D',
        arithmeticIntensity: 3.2,
        performanceGflops: 340,
        isa: 'AVX2',
        boundType: 'memory_bound',
        timestamp: now
      },
      {
        id: 'vector_fma',
        name: 'Vector FMA (Stream)',
        arithmeticIntensity: 1.5,
        performanceGflops: 115,
        isa: 'AVX2',
        boundType: 'memory_bound',
        timestamp: now
      },
      {
        id: 'cordic_sin_cos',
        name: 'CORDIC Fixed Q31',
        arithmeticIntensity: 7.8,
        performanceGflops: 620,
        isa: 'AVX2',
        boundType: 'compute_bound',
        timestamp: now
      },
      {
        id: 'neumaier_sum',
        name: 'Neumaier Sum f32',
        arithmeticIntensity: 0.8,
        performanceGflops: 65,
        isa: 'AVX2',
        boundType: 'memory_bound',
        timestamp: now
      }
    ];

    const frame: LiveTelemetryFrame = {
      timestamp: now,
      frameIndex: this.simFrameIndex,
      cpuMhz: this.hardwareCeilings.mhz + Math.floor(Math.sin(this.simFrameIndex * 0.1) * 150),
      topography: {
        eggCount: this.simEggCount,
        fledglingCount: this.simFledglingCount,
        pitstopCount: this.simPitstopCount,
        fallineEmptyBoxes: this.simFallineBoxes,
        tankRegionsBytes: this.simTankBytes,
        landfillEvacuatedBytes: this.simLandfillBytes,
        puzzleArenaActiveTests: 2,
        totalAllocatedBytes: this.simTotalAlloc,
        rmrpReclaimedBytes: this.simRmrpReclaimed
      },
      metrics: {
        currentGflops: Math.round(currentGflops * 10) / 10,
        peakGflops: Math.round(baseGflops * 1.35 * 10) / 10,
        currentThroughputGBs: Math.round(throughputGBs * 10) / 10,
        estimatedIPC: Math.round(estimatedIPC * 100) / 100,
        totalOperations: this.simFrameIndex * this.activeConfig.workloadSize,
        dispatchedAluCount: Math.floor(this.simFrameIndex * 3.4),
        speedupVsScalar: Math.round((currentGflops / 48) * 10) / 10,
        speedupVsWasm: Math.round((currentGflops / 160) * 10) / 10,
        activeKernel: this.activeConfig.kernel,
        activeProfile: this.activeConfig.profile,
        activeIsa: this.activeConfig.targetIsa
      },
      recentLatencies,
      rooflinePoints,
      selfHealStats: {
        corruptionsHealed: 14,
        brooksCyclesBroken: 4,
        codeErrorsCaptured: 2,
        bitcodeExceptionsShielded: 3
      },
      fourPointStatus: {
        safepointPolls: this.simFrameIndex * 24,
        controlpointChecks: this.simFrameIndex * 12,
        hookpointEnters: this.simFrameIndex * 8,
        hookpointExits: this.simFrameIndex * 8,
        pausepointPolls: this.simFrameIndex * 4,
        pausepointArmed: false
      }
    };

    this.notifyFrame(frame);
  }
}

export const nativeBridge = new NativeBridgeService();
