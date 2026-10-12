import { LiveTelemetryFrame } from '../types/telemetry';

export class TelemetryStreamComponent {
  private container: HTMLElement;

  constructor(container: HTMLElement) {
    this.container = container;
    this.renderInitialSkeleton();
  }

  private renderInitialSkeleton(): void {
    this.container.innerHTML = `
      <div class="telemetry-card">
        <div class="ctrl-header">
          <span class="ctrl-title">EXECUTION TOPOLOGY & 4-POINT SAFETY GATES</span>
          <span class="ctrl-sub" id="telemetry-backend-status">Direct Hardware RDTSC Telemetry</span>
        </div>

        <!-- 4 Execution Topology Gates -->
        <div class="four-point-grid">
          <div class="gate-box">
            <span class="gate-name">SAFEPOINT POLLS</span>
            <span class="gate-count" id="gate-safepoint">0</span>
            <span class="gate-sub">sf_gc_safepoint_poll (Lock-free thread sync)</span>
          </div>

          <div class="gate-box">
            <span class="gate-name">HOOKPOINT PROBES</span>
            <span class="gate-count" id="gate-hookpoint" style="color: #d4a373">0</span>
            <span class="gate-sub">sf_vm_hookpoint_trigger (8-slot SIMD audit)</span>
          </div>

          <div class="gate-box">
            <span class="gate-name">CONTROLPOINT CHECKS</span>
            <span class="gate-count" id="gate-controlpoint" style="color: #88d49e">0</span>
            <span class="gate-sub">sf_gc_controlpoint_check (Brooks forwarding audit)</span>
          </div>

          <div class="gate-box">
            <span class="gate-name">PAUSEPOINTS POLLS</span>
            <span class="gate-count" id="gate-pausepoint" style="color: #7eb0d5">0</span>
            <span class="gate-sub">sf_gc_pausepoint_poll (Clean task preemption)</span>
          </div>
        </div>

        <!-- Self-Healing Engine Status Banner -->
        <div class="self-heal-banner">
          <div>
            <strong style="color: #d4a373">SFVM AUTONOMOUS SELF-HEALING ENGINE</strong>
            <div style="font-size: 10px; color: var(--text-muted); margin-top: 2px;">
              Active in-memory memory corruption, cycle-breaking, and bitcode exception recovery
            </div>
          </div>
          <div class="sh-stat-group">
            <div class="sh-stat">
              <span>Corruptions Repaired:</span>
              <span class="sh-val" id="sh-repaired">0</span>
            </div>
            <div class="sh-stat">
              <span>Brooks Cycles Broken:</span>
              <span class="sh-val" id="sh-cycles" style="color: #88d49e">0</span>
            </div>
            <div class="sh-stat">
              <span>Bitcode Protected:</span>
              <span class="sh-val" id="sh-bitcode" style="color: #7eb0d5">0</span>
            </div>
          </div>
        </div>
      </div>
    `;
  }

  public update(frame: LiveTelemetryFrame): void {
    const elSafe = document.getElementById('gate-safepoint');
    const elHook = document.getElementById('gate-hookpoint');
    const elCtrl = document.getElementById('gate-controlpoint');
    const elPause = document.getElementById('gate-pausepoint');

    const elRepaired = document.getElementById('sh-repaired');
    const elCycles = document.getElementById('sh-cycles');
    const elBitcode = document.getElementById('sh-bitcode');
    const elBackend = document.getElementById('telemetry-backend-status');

    if (elSafe) elSafe.textContent = frame.fourPointStatus.safepointPolls.toLocaleString();
    if (elHook) elHook.textContent = frame.fourPointStatus.hookpointEnters.toLocaleString();
    if (elCtrl) elCtrl.textContent = frame.fourPointStatus.controlpointChecks.toLocaleString();
    if (elPause) elPause.textContent = frame.fourPointStatus.pausepointPolls.toLocaleString();

    if (elRepaired) elRepaired.textContent = frame.selfHealStats.corruptionsHealed.toString();
    if (elCycles) elCycles.textContent = frame.selfHealStats.brooksCyclesBroken.toString();
    if (elBitcode) elBitcode.textContent = frame.selfHealStats.bitcodeExceptionsShielded.toString();

    if (elBackend && frame.nativeBackend) {
      elBackend.textContent = frame.nativeBackend.isNative
        ? `Native C-ABI Active (${frame.nativeBackend.abiInterface})`
        : 'High-Fidelity Simulation Stream';
    }
  }

  public updateStatus(status: string): void {
    const elBackend = document.getElementById('telemetry-backend-status');
    const elConnBadge = document.getElementById('header-conn-badge');
    if (elBackend) {
      elBackend.textContent = status === 'CONNECTED_NATIVE'
        ? 'Native C-ABI Active (64-bit Direct Symbol Dispatch)'
        : (status === 'CONNECTING' ? 'Connecting to local bridge...' : 'High-Fidelity Simulation Stream');
    }
    if (elConnBadge) {
      elConnBadge.textContent = status === 'CONNECTED_NATIVE' ? '● NATIVE C-ABI CONNECTED' : '○ SIMULATION MODE';
      elConnBadge.className = 'badge-pill ' + (status === 'CONNECTED_NATIVE' ? 'status-native' : 'status-sim');
    }
  }

  public appendLog(msg: string): void {
    console.log(`[TelemetryLog] ${msg}`);
  }
}
