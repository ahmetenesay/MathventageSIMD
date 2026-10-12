import {
  LiveTelemetryFrame,
  BenchmarkConfig,
  HardwareCeilings
} from '../types/telemetry';
import { TS_BenchmarkRunner, TS_BenchmarkResult } from './native-api';
import { LogEntry } from '../components/terminal-console';

export type FrameCallback = (frame: LiveTelemetryFrame) => void;
export type ConnectionStatus = 'CONNECTED_NATIVE' | 'CONNECTING' | 'SIMULATION_MODE' | 'DISCONNECTED';

export class NativeBridgeService {
  private ws: WebSocket | null = null;
  private wsUrl: string = 'ws://localhost:9099/stream';
  private httpUrl: string = 'http://localhost:9099/telemetry';
  private frameCallbacks: Set<FrameCallback> = new Set();
  private statusCallbacks: Set<(status: ConnectionStatus) => void> = new Set();
  private logCallbacks: Set<(entry: LogEntry) => void> = new Set();
  private status: ConnectionStatus = 'DISCONNECTED';
  private simulationInterval: number | null = null;
  private httpPollingInterval: number | null = null;
  private autoReconnectTimer: number | null = null;

  // Active Simulated State for realistic fallback
  private simFrameIndex = 0;
  private simEggCount = 38;
  private simFledglingCount = 172;
  private simPitstopCount = 2;
  private simFallineBoxes = 64;
  private simTankBytes = 64 * 1024 * 1024;
  private simLandfillBytes = 16 * 1024 * 1024;
  private simTotalAlloc = 128 * 1024 * 1024;
  private simRmrpReclaimed = 12 * 1024 * 1024;
  private activeConfig: BenchmarkConfig = {
    kernel: 'msimd_mat4f_mul',
    workloadSize: 65536,
    targetIsa: 'AVX2',
    profile: 'NORMAX',
    executionFocus: 'AUTO',
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
          const msg = JSON.parse(event.data);
          if (msg && msg.type === 'NATIVE_LOG' && msg.entry) {
            this.notifyLog(msg.entry);
            return;
          }
          this.notifyFrame(msg as LiveTelemetryFrame);
        } catch (e) {
          console.error('Failed to parse telemetry frame from native daemon:', e);
        }
      };

      this.ws.onerror = () => {
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
    }
  }

  private scheduleReconnect(): void {
    if (this.autoReconnectTimer !== null) return;
    this.autoReconnectTimer = window.setTimeout(() => {
      this.autoReconnectTimer = null;
      if (this.status !== 'CONNECTED_NATIVE') {
        this.connect();
      }
    }, 4000);
  }

  private startHttpPolling(): void {
    if (this.httpPollingInterval !== null) return;

    const poll = async () => {
      try {
        const resp = await fetch(this.httpUrl, { cache: 'no-store' });
        if (resp.ok) {
          const frame: LiveTelemetryFrame = await resp.json();
          this.setStatus('CONNECTED_NATIVE');
          this.stopSimulation();
          this.notifyFrame(frame);
          return;
        }
      } catch {}

      if (this.status !== 'SIMULATION_MODE') {
        this.setStatus('SIMULATION_MODE');
        this.startSimulation();
      }
    };

    poll();
    this.httpPollingInterval = window.setInterval(poll, 150);
  }

  private stopHttpPolling(): void {
    if (this.httpPollingInterval !== null) {
      clearInterval(this.httpPollingInterval);
      this.httpPollingInterval = null;
    }
  }

  private startSimulation(): void {
    if (this.simulationInterval !== null) return;

    this.simulationInterval = window.setInterval(() => {
      this.simFrameIndex++;

      if (this.simFrameIndex % 6 === 0) {
        if (this.simEggCount > 10) {
          this.simEggCount -= 2;
          this.simFledglingCount += 2;
        } else {
          this.simEggCount += 16;
        }
      }

      let baseGflops = 890.4;
      let arithmeticIntensity = 18.5;
      if (this.activeConfig.targetIsa === 'AVX512') baseGflops = 1520.0;
      if (this.activeConfig.targetIsa === 'SSE4_2') baseGflops = 240.0;
      if (this.activeConfig.targetIsa === 'SCALAR') baseGflops = 54.0;
      if (this.activeConfig.targetIsa === 'WASM_SIMD128') baseGflops = 180.0;

      if (this.activeConfig.kernel === 'msimd_wave_supersaw_7') {
        arithmeticIntensity = 6.2;
        baseGflops = (this.activeConfig.targetIsa === 'AVX512' ? 520.0 : 380.0);
      } else if (this.activeConfig.kernel === 'msimd_cull_distance_lod_f32') {
        arithmeticIntensity = 1.8;
        baseGflops = 140.0;
      }

      const jitter = Math.sin(this.simFrameIndex * 0.25) * 0.05;
      const currentGflops = Math.max(10, Math.round((baseGflops * (1 + jitter)) * 10) / 10);
      const throughputGBs = Math.round((currentGflops / arithmeticIntensity) * 10) / 10;
      const ipc = Math.min(3.8, Math.round(((currentGflops / (4200 * 0.001 * 16)) * 2.1) * 100) / 100);

      const frame: LiveTelemetryFrame = {
        timestamp: Date.now(),
        frameIndex: this.simFrameIndex,
        cpuMhz: 4200 + Math.floor(Math.sin(this.simFrameIndex * 0.2) * 120),
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
          currentGflops,
          peakGflops: Math.round(baseGflops * 1.35 * 10) / 10,
          currentThroughputGBs: throughputGBs,
          estimatedIPC: ipc,
          totalOperations: this.simFrameIndex * this.activeConfig.workloadSize,
          dispatchedAluCount: Math.floor(this.simFrameIndex * 3.8),
          speedupVsScalar: Math.round((currentGflops / 54.0) * 10) / 10,
          speedupVsWasm: Math.round((currentGflops / 180.0) * 10) / 10,
          activeKernel: this.activeConfig.kernel,
          activeProfile: this.activeConfig.profile,
          activeIsa: this.activeConfig.targetIsa
        },
        recentLatencies: [
          {
            timestamp: Date.now(),
            cycles: 1420 + Math.floor(Math.random() * 80),
            nanoseconds: 338 + Math.floor(Math.random() * 20),
            kernelName: this.activeConfig.kernel,
            isa: this.activeConfig.targetIsa
          }
        ],
        rooflinePoints: [
          {
            id: 'msimd_mat4f_mul',
            name: 'Dense Mat4x4 Multiply',
            arithmeticIntensity: 18.5,
            performanceGflops: this.activeConfig.targetIsa === 'AVX512' ? 1520.0 : 890.4,
            isa: this.activeConfig.targetIsa,
            boundType: 'compute_bound',
            timestamp: Date.now()
          },
          {
            id: 'msimd_cull_distance_lod_f32',
            name: 'Visibility Distance LOD',
            arithmeticIntensity: 1.8,
            performanceGflops: 120.0,
            isa: this.activeConfig.targetIsa,
            boundType: 'memory_bound',
            timestamp: Date.now()
          },
          {
            id: 'msimd_wave_supersaw_7',
            name: 'Wave Supersaw-7',
            arithmeticIntensity: 6.2,
            performanceGflops: 380.0,
            isa: this.activeConfig.targetIsa,
            boundType: 'compute_bound',
            timestamp: Date.now()
          }
        ],
        selfHealStats: {
          corruptionsHealed: 22,
          brooksCyclesBroken: 6,
          codeErrorsCaptured: 2,
          bitcodeExceptionsShielded: 4
        },
        fourPointStatus: {
          safepointPolls: 1420,
          controlpointChecks: 710,
          hookpointEnters: 355,
          hookpointExits: 355,
          pausepointPolls: 88,
          pausepointArmed: false
        }
      };

      this.notifyFrame(frame);
    }, 100);
  }

  private stopSimulation(): void {
    if (this.simulationInterval !== null) {
      clearInterval(this.simulationInterval);
      this.simulationInterval = null;
    }
  }

  private setStatus(newStatus: ConnectionStatus): void {
    if (this.status !== newStatus) {
      this.status = newStatus;
      for (const cb of this.statusCallbacks) {
        cb(this.status);
      }
    }
  }

  public getStatus(): ConnectionStatus {
    return this.status;
  }

  public onFrame(cb: FrameCallback): () => void {
    this.frameCallbacks.add(cb);
    return () => this.frameCallbacks.delete(cb);
  }

  public onLog(cb: (entry: LogEntry) => void): () => void {
    this.logCallbacks.add(cb);
    return () => this.logCallbacks.delete(cb);
  }

  public onStatusChange(cb: (status: ConnectionStatus) => void): () => void {
    this.statusCallbacks.add(cb);
    cb(this.status);
    return () => this.statusCallbacks.delete(cb);
  }

  private notifyFrame(frame: LiveTelemetryFrame): void {
    for (const cb of this.frameCallbacks) {
      cb(frame);
    }
  }

  private notifyLog(entry: LogEntry): void {
    for (const cb of this.logCallbacks) {
      cb(entry);
    }
  }

  public sendConfig(config: BenchmarkConfig): void {
    this.activeConfig = { ...config };
    const payload = JSON.stringify({ type: 'CONFIGURE', config });

    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(payload);
    } else {
      fetch(this.httpUrl, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: payload
      }).catch(() => {});
    }
  }

  public triggerNativeBenchmark(kernel?: string, count?: number): void {
    const payload = JSON.stringify({
      type: 'TRIGGER_BENCHMARK',
      kernel: kernel || this.activeConfig.kernel,
      count: count || this.activeConfig.workloadSize
    });

    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(payload);
    } else {
      fetch(this.httpUrl, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: payload
      }).catch(() => {});
    }
  }

  public runTsBenchmark(kernel?: string, count?: number): TS_BenchmarkResult {
    const k = kernel || this.activeConfig.kernel;
    const c = count || this.activeConfig.workloadSize;
    const result = TS_BenchmarkRunner.runBenchmark(k, c);
    
    // Also trigger native bridge pass for comparison
    this.triggerNativeBenchmark(k, c);
    return result;
  }

  public triggerPitstopExcise(): void {
    const payload = JSON.stringify({ type: 'TRIGGER_PITSTOP' });
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(payload);
    } else {
      if (this.simEggCount > 0) {
        this.simEggCount--;
        this.simPitstopCount++;
        setTimeout(() => {
          this.simPitstopCount = Math.max(0, this.simPitstopCount - 1);
          this.simFledglingCount++;
        }, 700);
      }
    }
  }

  public triggerRmrpCompaction(): void {
    const payload = JSON.stringify({ type: 'TRIGGER_RMRP' });
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(payload);
    } else {
      this.simRmrpReclaimed += 4 * 1024 * 1024;
    }
  }

  public triggerSelfHeal(): void {
    const payload = JSON.stringify({ type: 'TRIGGER_SELF_HEAL' });
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(payload);
    }
  }
}

export const nativeBridge = new NativeBridgeService();
