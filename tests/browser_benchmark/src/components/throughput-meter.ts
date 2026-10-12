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
        <!-- 1. Compute Throughput -->
        <div class="meter-card">
          <div class="meter-header">
            <span class="meter-title">COMPUTE THROUGHPUT</span>
            <span class="meter-badge" id="meter-isa-badge">AVX2</span>
          </div>
          <div class="meter-main-val">
            <span class="big-num mint" id="val-gflops">0.0</span>
            <span class="unit">GFLOPS</span>
          </div>
          <div class="meter-sub-bar">
            <div class="bar-fill mint" id="bar-gflops" style="width: 0%"></div>
          </div>
          <div class="meter-footer">
            <span>Peak: <strong id="val-peak-gflops" style="color: #cbd5e1">0.0</strong> GF</span>
            <span class="speedup-tag" id="val-speedup-scalar">1.0x vs Scalar</span>
          </div>
        </div>

        <!-- 2. Memory Bandwidth -->
        <div class="meter-card">
          <div class="meter-header">
            <span class="meter-title">MEMORY BANDWIDTH</span>
            <span class="meter-badge">DRAM / LLC</span>
          </div>
          <div class="meter-main-val">
            <span class="big-num sky" id="val-bandwidth">0.0</span>
            <span class="unit">GB/s</span>
          </div>
          <div class="meter-sub-bar">
            <div class="bar-fill sky" id="bar-bandwidth" style="width: 0%"></div>
          </div>
          <div class="meter-footer">
            <span>Utilization: <strong id="val-bw-util" style="color: #cbd5e1">0%</strong></span>
            <span class="speedup-tag" id="val-speedup-wasm">3.8x vs Wasm</span>
          </div>
        </div>

        <!-- 3. Estimated IPC -->
        <div class="meter-card">
          <div class="meter-header">
            <span class="meter-title">ESTIMATED IPC</span>
            <span class="meter-badge">UOPS PIPELINE</span>
          </div>
          <div class="meter-main-val">
            <span class="big-num gold" id="val-ipc">0.00</span>
            <span class="unit">IPC</span>
          </div>
          <div class="meter-sub-bar">
            <div class="bar-fill gold" id="bar-ipc" style="width: 0%"></div>
          </div>
          <div class="meter-footer">
            <span>Dispatched ALUs: <strong id="val-alu-count" style="color: #cbd5e1">0</strong></span>
            <span style="color: #88d49e; font-weight: 600">Port Contention Balanced</span>
          </div>
        </div>

        <!-- 4. Runway Active Tensors -->
        <div class="meter-card">
          <div class="meter-header">
            <span class="meter-title">RUNWAY TOPOLOGY</span>
            <span class="meter-badge">SFVM RUNWAY</span>
          </div>
          <div class="meter-main-val">
            <span class="big-num lavender" id="val-tensors-total">0</span>
            <span class="unit">OBJS</span>
          </div>
          <div class="meter-sub-bar">
            <div class="bar-fill lavender" id="bar-tensors" style="width: 0%"></div>
          </div>
          <div class="meter-footer">
            <span>Egg: <strong id="val-egg-ratio" style="color: #f6d585">0</strong> | Fledgling: <strong id="val-fled-ratio" style="color: #88d49e">0</strong></span>
            <span id="val-pitstop-active" style="color: #e0a96d; font-weight: 600">0 in Pitstop</span>
          </div>
        </div>
      </div>
    `;
  }

  public update(metrics: BenchmarkMetrics, eggCount = 0, fledCount = 0, pitCount = 0): void {
    const elGflops = document.getElementById('val-gflops');
    const elPeak = document.getElementById('val-peak-gflops');
    const barGflops = document.getElementById('bar-gflops');
    const elBw = document.getElementById('val-bandwidth');
    const barBw = document.getElementById('bar-bandwidth');
    const elBwUtil = document.getElementById('val-bw-util');
    const elIpc = document.getElementById('val-ipc');
    const barIpc = document.getElementById('bar-ipc');
    const elAlu = document.getElementById('val-alu-count');
    const elSpeedupScalar = document.getElementById('val-speedup-scalar');
    const elSpeedupWasm = document.getElementById('val-speedup-wasm');
    const elIsaBadge = document.getElementById('meter-isa-badge');
    const elTensors = document.getElementById('val-tensors-total');
    const elEggRatio = document.getElementById('val-egg-ratio');
    const elFledRatio = document.getElementById('val-fled-ratio');
    const elPitActive = document.getElementById('val-pitstop-active');
    const barTensors = document.getElementById('bar-tensors');

    if (elGflops) elGflops.textContent = metrics.currentGflops.toFixed(1);
    if (elPeak) elPeak.textContent = metrics.peakGflops.toFixed(1);
    if (barGflops) {
      const pct = Math.min(100, Math.round((metrics.currentGflops / Math.max(1, metrics.peakGflops)) * 100));
      barGflops.style.width = `${pct}%`;
    }

    if (elBw) elBw.textContent = metrics.currentThroughputGBs.toFixed(1);
    if (barBw) {
      const pct = Math.min(100, Math.round((metrics.currentThroughputGBs / 76.8) * 100));
      barBw.style.width = `${pct}%`;
      if (elBwUtil) elBwUtil.textContent = `${pct}%`;
    }

    if (elIpc) elIpc.textContent = metrics.estimatedIPC.toFixed(2);
    if (barIpc) {
      const pct = Math.min(100, Math.round((metrics.estimatedIPC / 4.0) * 100));
      barIpc.style.width = `${pct}%`;
    }

    if (elAlu) elAlu.textContent = metrics.dispatchedAluCount.toLocaleString();
    if (elSpeedupScalar) elSpeedupScalar.textContent = `${metrics.speedupVsScalar.toFixed(1)}x vs Scalar`;
    if (elSpeedupWasm) elSpeedupWasm.textContent = `${metrics.speedupVsWasm.toFixed(1)}x vs Wasm`;
    if (elIsaBadge) elIsaBadge.textContent = metrics.activeIsa;

    const totalObjs = eggCount + fledCount + pitCount;
    if (elTensors) elTensors.textContent = totalObjs.toString();
    if (elEggRatio) elEggRatio.textContent = eggCount.toString();
    if (elFledRatio) elFledRatio.textContent = fledCount.toString();
    if (elPitActive) elPitActive.textContent = `${pitCount} in Pitstop`;
    if (barTensors) {
      const pct = Math.min(100, Math.round((fledCount / Math.max(1, totalObjs)) * 100));
      barTensors.style.width = `${pct}%`;
    }
  }
}
