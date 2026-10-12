import { LiveTelemetryFrame } from '../types/telemetry';
import { ConnectionStatus } from '../services/native-bridge';

export class TelemetryStreamComponent {
  private container: HTMLElement;
  private logTerminal: HTMLElement | null = null;
  private logLines: string[] = [];
  private readonly maxLogLines = 40;

  constructor(container: HTMLElement) {
    this.container = container;
    this.renderSkeleton();
  }

  private renderSkeleton(): void {
    this.container.innerHTML = `
      <div class="stream-panel">
        <div class="stream-header">
          <div class="header-left">
            <span class="stream-title">RDTSC HARDWARE TELEMETRY & 4-POINT TOPOLOGY</span>
            <div class="status-indicator-badge" id="conn-status-badge">
              <span class="dot"></span>
              <span id="conn-status-text">INITIALIZING</span>
            </div>
          </div>
          <div class="cpu-stat" id="stream-cpu-clock">CPU: 4200 MHz</div>
        </div>

        <div class="stream-grid">
          <!-- 4-Point Topology Status Cards -->
          <div class="four-point-box">
            <div class="fp-title">4-Point Execution Topology</div>
            <div class="fp-items">
              <div class="fp-item">
                <span class="fp-lbl">Safepoints:</span>
                <span class="fp-val" id="fp-safepoint">0</span>
              </div>
              <div class="fp-item">
                <span class="fp-lbl">Controlpoints:</span>
                <span class="fp-val" id="fp-controlpoint">0</span>
              </div>
              <div class="fp-item">
                <span class="fp-lbl">Hookpoints:</span>
                <span class="fp-val" id="fp-hookpoint">0 / 0</span>
              </div>
              <div class="fp-item">
                <span class="fp-lbl">Pausepoints:</span>
                <span class="fp-val" id="fp-pausepoint">0</span>
              </div>
            </div>

            <div class="self-heal-box">
              <div class="sh-title">sfGC Self-Healing Engine</div>
              <div class="sh-items">
                <div>Healed: <strong class="green" id="sh-healed">0</strong></div>
                <div>Brooks Broken: <strong class="cyan" id="sh-brooks">0</strong></div>
                <div>Shielded: <strong class="yellow" id="sh-shielded">0</strong></div>
              </div>
            </div>
          </div>

          <!-- Scrolling Telemetry Log Stream -->
          <div class="log-stream-terminal" id="log-terminal">
            <div class="terminal-line system-line">[SYSTEM] MathventageSIMD Browser Telemetry Client Initialized</div>
          </div>
        </div>
      </div>
    `;

    this.logTerminal = document.getElementById('log-terminal');
  }

  public updateStatus(status: ConnectionStatus): void {
    const elBadge = document.getElementById('conn-status-badge');
    const elText = document.getElementById('conn-status-text');
    if (!elBadge || !elText) return;

    elBadge.className = 'status-indicator-badge ' + status.toLowerCase();
    switch (status) {
      case 'CONNECTED_NATIVE':
        elText.textContent = 'NATIVE LIBWS BRIDGE (ACTIVE)';
        break;
      case 'SIMULATION_MODE':
        elText.textContent = 'HIGH-FIDELITY SIMULATION (OFFLINE)';
        break;
      case 'CONNECTING':
        elText.textContent = 'CONNECTING TO BRIDGE...';
        break;
      case 'DISCONNECTED':
        elText.textContent = 'DISCONNECTED';
        break;
    }
  }

  public update(frame: LiveTelemetryFrame): void {
    const elCpu = document.getElementById('stream-cpu-clock');
    if (elCpu) elCpu.textContent = `CPU: ${frame.cpuMhz} MHz`;

    const elSafe = document.getElementById('fp-safepoint');
    const elCtrl = document.getElementById('fp-controlpoint');
    const elHook = document.getElementById('fp-hookpoint');
    const elPause = document.getElementById('fp-pausepoint');

    if (elSafe) elSafe.textContent = frame.fourPointStatus.safepointPolls.toLocaleString();
    if (elCtrl) elCtrl.textContent = frame.fourPointStatus.controlpointChecks.toLocaleString();
    if (elHook) {
      elHook.textContent = `${frame.fourPointStatus.hookpointEnters.toLocaleString()} / ${frame.fourPointStatus.hookpointExits.toLocaleString()}`;
    }
    if (elPause) elPause.textContent = frame.fourPointStatus.pausepointPolls.toLocaleString();

    const elHealed = document.getElementById('sh-healed');
    const elBrooks = document.getElementById('sh-brooks');
    const elShielded = document.getElementById('sh-shielded');

    if (elHealed) elHealed.textContent = frame.selfHealStats.corruptionsHealed.toString();
    if (elBrooks) elBrooks.textContent = frame.selfHealStats.brooksCyclesBroken.toString();
    if (elShielded) elShielded.textContent = frame.selfHealStats.bitcodeExceptionsShielded.toString();

    // Log recent latencies
    if (frame.recentLatencies && frame.recentLatencies.length > 0) {
      const sample = frame.recentLatencies[frame.recentLatencies.length - 1];
      const logLine = `[RDTSC] Frame #${frame.frameIndex} | Kernel: ${sample.kernelName} | ISA: ${sample.isa} | Lat: ${sample.nanoseconds}ns (${sample.cycles} cycles) | GFLOPS: ${frame.metrics.currentGflops}`;
      this.appendLog(logLine);
    }
  }

  public appendLog(line: string): void {
    this.logLines.push(line);
    if (this.logLines.length > this.maxLogLines) {
      this.logLines.shift();
    }

    if (this.logTerminal) {
      let html = '';
      for (const l of this.logLines) {
        html += `<div class="terminal-line">${l}</div>`;
      }
      this.logTerminal.innerHTML = html;
      this.logTerminal.scrollTop = this.logTerminal.scrollHeight;
    }
  }
}
