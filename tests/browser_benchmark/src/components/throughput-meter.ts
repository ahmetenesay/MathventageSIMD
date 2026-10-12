import { BenchmarkMetrics } from '../types/telemetry';

export class ThroughputMeterComponent {
  private container: HTMLElement;

  constructor(container: HTMLElement) {
    this.container = container;
    this.renderInitialSkeleton();
  }

  private renderInitialSkeleton(): void {
    this.container.innerHTML = `
      <div class="meter-grid">
        <div class="meter-card neon-card-cyan">
          <div class="meter-header">
            <span class="meter-title">COMPUTE THROUGHPUT</span>
            <span class="meter-badge" id="meter-isa-badge">AVX2</span>
          </div>
          <div class="meter-main-val">
            <span class="big-num" id="val-gflops">0.0</span>
            <span class="unit">GFLOPS</span>
          </div>
          <div class="meter-sub-bar">
            <div class="bar-fill" id="bar-gflops" style="width: 0%"></div>
          </div>
          <div class="meter-footer">
            <span>Peak: <strong id="val-peak-gflops">0.0</strong> GF</span>
            <span class="speedup-tag" id="val-speedup-scalar">1.0x vs Scalar</span>
          </div>
        </div>

        <div class="meter-card neon-card-blue">
          <div class="meter-header">
            <span class="meter-title">MEMORY BANDWIDTH</span>
            <span class="meter-badge">DRAM / LLC</span>
          </div>
          <div class="meter-main-val">
            <span class="big-num" id="val-bandwidth">0.0</span>
            <span class="unit">GB/s</span>
          </div>
          <div class="meter-sub-bar">
            <div class="bar-fill blue" id="bar-bandwidth" style="width: 0%"></div>
          </div>
          <div class="meter-footer">
            <span>Utilization: <strong id="val-bw-util">0%</strong></span>
            <span class="speedup-tag wasm-tag" id="val-speedup-wasm">3.8x vs Wasm</span>
          </div>
        </div>

        <div class="meter-card neon-card-green">
          <div class="meter-header">
            <span class="meter-title">ESTIMATED IPC</span>
            <span class="meter-badge">PIPELINE</span>
          </div>
          <div class="meter-main-val">
            <span class="big-num green" id="val-ipc">0.00</span>
            <span class="unit">IPC</span>
          </div>
          <div class="meter-sub-bar">
            <div class="bar-fill green" id="bar-ipc" style="width: 0%"></div>
          </div>
          <div class="meter-footer">
            <span>ALU Dispatches: <strong id="val-alu-disp">0</strong></span>
            <span class="status-tag active">STALL-FREE</span>
          </div>
        </div>
      </div>
    `;
  }

  public update(metrics: BenchmarkMetrics): void {
    const elGflops = document.getElementById('val-gflops');
    const elPeak = document.getElementById('val-peak-gflops');
    const elBarGflops = document.getElementById('bar-gflops');
    const elSpeedScalar = document.getElementById('val-speedup-scalar');
    const elIsaBadge = document.getElementById('meter-isa-badge');

    const elBw = document.getElementById('val-bandwidth');
    const elBarBw = document.getElementById('bar-bandwidth');
    const elBwUtil = document.getElementById('val-bw-util');
    const elSpeedWasm = document.getElementById('val-speedup-wasm');

    const elIpc = document.getElementById('val-ipc');
    const elBarIpc = document.getElementById('bar-ipc');
    const elAluDisp = document.getElementById('val-alu-disp');

    if (elGflops) elGflops.textContent = metrics.currentGflops.toLocaleString();
    if (elPeak) elPeak.textContent = metrics.peakGflops.toLocaleString();
    if (elBarGflops) {
      const pct = Math.min(100, (metrics.currentGflops / (metrics.peakGflops || 1000)) * 100);
      elBarGflops.style.width = `${pct}%`;
    }
    if (elSpeedScalar) elSpeedScalar.textContent = `${metrics.speedupVsScalar}x vs Scalar`;
    if (elIsaBadge) elIsaBadge.textContent = metrics.activeIsa;

    if (elBw) elBw.textContent = metrics.currentThroughputGBs.toLocaleString();
    if (elBarBw) {
      const pct = Math.min(100, (metrics.currentThroughputGBs / 76.8) * 100);
      elBarBw.style.width = `${pct}%`;
    }
    if (elBwUtil) {
      const pct = Math.round(Math.min(100, (metrics.currentThroughputGBs / 76.8) * 100));
      elBwUtil.textContent = `${pct}%`;
    }
    if (elSpeedWasm) elSpeedWasm.textContent = `${metrics.speedupVsWasm}x vs Wasm SIMD`;

    if (elIpc) elIpc.textContent = metrics.estimatedIPC.toFixed(2);
    if (elBarIpc) {
      const pct = Math.min(100, (metrics.estimatedIPC / 4.0) * 100);
      elBarIpc.style.width = `${pct}%`;
    }
    if (elAluDisp) elAluDisp.textContent = metrics.dispatchedAluCount.toLocaleString();
  }
}
