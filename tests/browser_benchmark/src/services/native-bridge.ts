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
  private scriptResultCallbacks: Set<(result: any) => void> = new Set();
  private status: ConnectionStatus = 'DISCONNECTED';
  private simulationInterval: number | null = null;
  private httpPollingInterval: number | null = null;
  private autoReconnectTimer: number | null = null;

  // Active Simulated State for realistic fallback
  private simFrameIndex = 0;
  private simEggCount = 0;
  private simFledglingCount = 0;
  private simPitstopCount = 0;
  private simFallineBoxes = 0;
  private simTankBytes = 0;
  private simLandfillBytes = 0;
  private simTotalAlloc = 0;
  private simRmrpReclaimed = 0;
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
          if (msg && msg.type === 'SCRIPT_RESULT') {
            if (msg.frame) {
              this.notifyFrame(msg.frame);
            }
            this.notifyScriptResult(msg);
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

      const frame: LiveTelemetryFrame = {
        timestamp: Date.now(),
        frameIndex: this.simFrameIndex,
        cpuMhz: 4200,
        topography: {
          eggCount: this.simEggCount,
          fledglingCount: this.simFledglingCount,
          pitstopCount: this.simPitstopCount,
          fallineEmptyBoxes: this.simFallineBoxes,
          tankRegionsBytes: this.simTankBytes,
          landfillEvacuatedBytes: this.simLandfillBytes,
          puzzleArenaActiveTests: 0,
          totalAllocatedBytes: this.simTotalAlloc,
          rmrpReclaimedBytes: this.simRmrpReclaimed
        },
        metrics: {
          currentGflops: 0,
          peakGflops: 0,
          currentThroughputGBs: 0,
          estimatedIPC: 0,
          totalOperations: 0,
          dispatchedAluCount: 0,
          speedupVsScalar: 0,
          speedupVsWasm: 0,
          activeKernel: 'none',
          activeProfile: this.activeConfig.profile,
          activeIsa: this.activeConfig.targetIsa
        },
        recentLatencies: [],
        rooflinePoints: [],
        selfHealStats: {
          corruptionsHealed: 0,
          brooksCyclesBroken: 0,
          codeErrorsCaptured: 0,
          bitcodeExceptionsShielded: 0
        },
        fourPointStatus: {
          safepointPolls: 0,
          controlpointChecks: 0,
          hookpointEnters: 0,
          hookpointExits: 0,
          pausepointPolls: 0,
          pausepointArmed: false
        },
        system: {
          timestamp: Date.now(),
          cpu: {
            totalLoad: 35,
            cores: [45, 60, 30, 25, 40, 35, 20, 18, 12, 10, 50, 62, 58, 48],
            count: 14
          },
          gpu: [
            { name: 'NVIDIA GeForce RTX 3060', load: 8, vramUsedMB: 1840, vramTotalMB: 12288 }
          ],
          memory: {
            totalMB: 32104,
            usedMB: 20152,
            freeMB: 11952,
            percent: 62,
            sfvmOnHeapMB: 128
          },
          disks: [
            { fs: 'C:', sizeGB: 931, usedGB: 908, freeGB: 23, usePercent: 97 },
            { fs: 'E:', sizeGB: 699, usedGB: 675, freeGB: 24, usePercent: 96 }
          ],
          network: {
            iface: 'Ethernet',
            rxSecKB: 45,
            txSecKB: 18
          }
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

  public onScriptResult(cb: (result: any) => void): () => void {
    this.scriptResultCallbacks.add(cb);
    return () => this.scriptResultCallbacks.delete(cb);
  }

  private notifyScriptResult(result: any): void {
    for (const cb of this.scriptResultCallbacks) {
      cb(result);
    }
  }

  public executeScript(sourceCode: string): Promise<any> {
    const payload = JSON.stringify({
      type: 'EXECUTE_SCRIPT',
      script: sourceCode
    });

    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(payload);
      return Promise.resolve({ sent: true, method: 'WS' });
    }

    return fetch('http://localhost:9099/script/run', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ script: sourceCode })
    })
      .then(res => res.json())
      .then(data => {
        if (data && data.frame) {
          this.notifyFrame(data.frame);
        }
        this.notifyScriptResult(data);
        return data;
      })
      .catch(() => ({ success: false, error: 'Connection failed' }));
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
