import { BenchmarkConfig, SIMDTarget } from '../types/telemetry';
import { nativeBridge } from '../services/native-bridge';
import { TS_BenchmarkResult } from '../services/native-api';

export class BenchmarkControllerComponent {
  private container: HTMLElement;
  private currentConfig: BenchmarkConfig = {
    kernel: 'msimd_mat4f_mul',
    workloadSize: 65536,
    targetIsa: 'AVX2',
    profile: 'NORMAX',
    executionFocus: 'AUTO',
    allowAluDispatch: true,
    rmrpMethod: 'PLUMP',
    runMode: 'CONTINUOUS'
  };

  constructor(container: HTMLElement) {
    this.container = container;
    this.renderSkeleton();
    this.attachEventListeners();
  }

  private renderSkeleton(): void {
    this.container.innerHTML = `
      <div class="controller-card">
        <div class="ctrl-header">
          <span class="ctrl-title">BENCHMARK EXECUTION MATRIX & HARDWARE CONTROLLER</span>
          <span class="ctrl-sub">TypeScript Engine & C-ABI Dynamic Steering</span>
        </div>

        <div class="ctrl-grid">
          <!-- Kernel Selector -->
          <div class="ctrl-field">
            <label class="ctrl-label">Mathematical Kernel</label>
            <select class="ctrl-select" id="ctrl-kernel">
              <option value="msimd_mat4f_mul" selected>msimd_mat4f_mul (Dense 4x4 Matrix Multiply FP32)</option>
              <option value="msimd_mat4d_mul">msimd_mat4d_mul (Dense 4x4 Matrix Multiply FP64)</option>
              <option value="msimd_cull_distance_lod_f32">msimd_cull_distance_lod_f32 (Frustum & Distance LOD Culling)</option>
              <option value="msimd_wave_supersaw_7">msimd_wave_supersaw_7 (Analytical Supersaw-7 Waveform)</option>
              <option value="msimd_neumaier_sum_f32">msimd_neumaier_sum_f32 (Neumaier Compensated Exact Sum)</option>
            </select>
          </div>

          <!-- Workload Size -->
          <div class="ctrl-field">
            <label class="ctrl-label">Workload Dimension (N)</label>
            <select class="ctrl-select" id="ctrl-size">
              <option value="4096">4,096 Elements (L1d Cache Resident)</option>
              <option value="65536" selected>65,536 Elements (L2 Cache Resident)</option>
              <option value="262144">262,144 Elements (L3 LLC Cache Resident)</option>
              <option value="1048576">1,048,576 Elements (DRAM Streaming)</option>
              <option value="4194304">4,194,304 Elements (Humongous Tank)</option>
            </select>
          </div>

          <!-- Target ISA -->
          <div class="ctrl-field">
            <label class="ctrl-label">Vector ISA Steering</label>
            <select class="ctrl-select" id="ctrl-isa">
              <option value="AVX10_2">AVX10.2 / Converged 512 (Level 17)</option>
              <option value="AVX512">AVX-512 Skylake-X / Zen 4 / Zen 5 (Level 13)</option>
              <option value="AVX2" selected>AVX2 + FMA 256-bit (Level 8)</option>
              <option value="SSE4_2">SSE4.2 128-bit (Level 4)</option>
              <option value="SCALAR">Scalar Portable Baseline (Level 0)</option>
              <option value="WASM_SIMD128">Wasm SIMD128 Client-Side</option>
            </select>
          </div>

          <!-- Execution Focus -->
          <div class="ctrl-field">
            <label class="ctrl-label">Execution Focus Scheduler</label>
            <select class="ctrl-select" id="ctrl-focus">
              <option value="AUTO" selected>AUTO (Hardware-Adaptive Nexus)</option>
              <option value="SCP">SCP (Single-Core Turbo P-Core Pinning)</option>
              <option value="MCP">MCP (Multi-Core NUMA Parallel Sweeps)</option>
              <option value="HYBRID_DUAL">HYBRID_DUAL (Asymmetric P+E Task Splitting)</option>
            </select>
          </div>
        </div>

        <div class="ctrl-actions-row">
          <button class="btn btn-ts" id="btn-ts-bench">
            ⚡ Run TypeScript Benchmark Pass
          </button>
          <button class="btn btn-primary" id="btn-native-bench">
            🚀 Trigger Native C-ABI Benchmark
          </button>
          <button class="btn btn-secondary" id="btn-pitstop-excise">
            ⚙ Trigger Pitstop In-Place ISA Migration
          </button>
          <button class="btn btn-warning" id="btn-rmrp-plump">
            🧹 Run RMRP Compaction (Plump)
          </button>
          <button class="btn btn-outline" id="btn-self-heal">
            🛡 Run Self-Heal Sweep
          </button>
        </div>

        <div class="bench-result-banner" id="bench-result-banner" style="display: none;">
          <div class="bench-banner-title" id="bench-banner-title">TypeScript Benchmark Result</div>
          <div class="bench-banner-content" id="bench-banner-content"></div>
        </div>
      </div>
    `;
  }

  private attachEventListeners(): void {
    const selKernel = document.getElementById('ctrl-kernel') as HTMLSelectElement;
    const selSize = document.getElementById('ctrl-size') as HTMLSelectElement;
    const selIsa = document.getElementById('ctrl-isa') as HTMLSelectElement;
    const selFocus = document.getElementById('ctrl-focus') as HTMLSelectElement;

    const btnTs = document.getElementById('btn-ts-bench');
    const btnNative = document.getElementById('btn-native-bench');
    const btnPitstop = document.getElementById('btn-pitstop-excise');
    const btnRmrp = document.getElementById('btn-rmrp-plump');
    const btnSelfHeal = document.getElementById('btn-self-heal');

    const updateConfig = () => {
      if (selKernel) this.currentConfig.kernel = selKernel.value;
      if (selSize) this.currentConfig.workloadSize = parseInt(selSize.value, 10);
      if (selIsa) this.currentConfig.targetIsa = selIsa.value as SIMDTarget;
      if (selFocus) this.currentConfig.executionFocus = selFocus.value as any;

      nativeBridge.sendConfig(this.currentConfig);
    };

    if (selKernel) selKernel.addEventListener('change', updateConfig);
    if (selSize) selSize.addEventListener('change', updateConfig);
    if (selIsa) selIsa.addEventListener('change', updateConfig);
    if (selFocus) selFocus.addEventListener('change', updateConfig);

    if (btnTs) {
      btnTs.addEventListener('click', () => {
        const res: TS_BenchmarkResult = nativeBridge.runTsBenchmark(
          this.currentConfig.kernel,
          this.currentConfig.workloadSize
        );
        this.displayResult(
          'TypeScript Benchmark Pass Completed',
          `Kernel: <strong>${res.kernelName}</strong> | Operations: <strong>${res.operations.toLocaleString()}</strong> | Latency: <strong>${res.latencyNs} ns/op</strong> | Throughput: <strong style="color: #88d49e">${res.gflops} GFLOPS</strong> (${res.throughputGBs} GB/s) in ${res.totalTimeMs} ms`
        );
      });
    }

    if (btnNative) {
      btnNative.addEventListener('click', () => {
        nativeBridge.triggerNativeBenchmark(this.currentConfig.kernel, this.currentConfig.workloadSize);
        this.displayResult(
          'Native C-ABI Direct Kernel Dispatch Dispatched',
          `Triggered <strong>${this.currentConfig.kernel}</strong> (${this.currentConfig.workloadSize.toLocaleString()} elements) via <strong>libsfmsimd.dll</strong> native C ABI exports.`
        );
      });
    }

    if (btnPitstop) {
      btnPitstop.addEventListener('click', () => {
        nativeBridge.triggerPitstopExcise();
      });
    }

    if (btnRmrp) {
      btnRmrp.addEventListener('click', () => {
        nativeBridge.triggerRmrpCompaction();
      });
    }

    if (btnSelfHeal) {
      btnSelfHeal.addEventListener('click', () => {
        nativeBridge.triggerSelfHeal();
      });
    }
  }

  private displayResult(title: string, content: string): void {
    const banner = document.getElementById('bench-result-banner');
    const elTitle = document.getElementById('bench-banner-title');
    const elContent = document.getElementById('bench-banner-content');
    if (banner && elTitle && elContent) {
      elTitle.textContent = title;
      elContent.innerHTML = content;
      banner.style.display = 'flex';
      setTimeout(() => {
        banner.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
      }, 50);
    }
  }
}
