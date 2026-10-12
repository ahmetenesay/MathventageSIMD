import { BenchmarkConfig, SIMDTarget, VMProfile } from '../types/telemetry';
import { nativeBridge } from '../services/native-bridge';
import { wasmBenchmarkRunner, WasmBenchmarkResult } from '../services/wasm-runner';

export class BenchmarkControllerComponent {
  private container: HTMLElement;
  private currentConfig: BenchmarkConfig = {
    kernel: 'dense_gemm_f32',
    workloadSize: 65536,
    targetIsa: 'AVX2',
    profile: 'NORMAX',
    allowAluDispatch: true,
    rmrpMethod: 'PLUMP',
    runMode: 'CONTINUOUS'
  };

  private onWasmResultCallback: ((result: WasmBenchmarkResult) => void) | null = null;

  constructor(container: HTMLElement) {
    this.container = container;
    this.renderSkeleton();
    this.attachEventListeners();
  }

  public onWasmResult(cb: (result: WasmBenchmarkResult) => void): void {
    this.onWasmResultCallback = cb;
  }

  private renderSkeleton(): void {
    this.container.innerHTML = `
      <div class="controller-card">
        <div class="ctrl-header">
          <span class="ctrl-title">BENCHMARK EXECUTION MATRIX</span>
          <span class="ctrl-sub">Dynamic Kernel & Silicon Control</span>
        </div>

        <div class="ctrl-grid">
          <!-- Kernel Selector -->
          <div class="ctrl-field">
            <label class="ctrl-label">Mathematical Kernel</label>
            <select class="ctrl-select" id="ctrl-kernel">
              <option value="dense_gemm_f32" selected>Dense GEMM Matrix Multiply (FP32)</option>
              <option value="lattice_fdtd_3d">Lattice FDTD 3D Wave Mechanics</option>
              <option value="vector_fma">Vector FMA Stream (a*b+c)</option>
              <option value="cordic_sin_cos">CORDIC Trigonometric (Fixed-Point Q31)</option>
              <option value="neumaier_sum">Neumaier Exact Compensated Sum</option>
            </select>
          </div>

          <!-- Workload Size -->
          <div class="ctrl-field">
            <label class="ctrl-label">Workload Dimension (N)</label>
            <select class="ctrl-select" id="ctrl-size">
              <option value="4096">4,096 Elements (L1d Resident)</option>
              <option value="65536" selected>65,536 Elements (L2 Resident)</option>
              <option value="262144">262,144 Elements (L3 LLC Resident)</option>
              <option value="1048576">1,048,576 Elements (DRAM Streaming)</option>
              <option value="4194304">4,194,304 Elements (Humongous Tank)</option>
            </select>
          </div>

          <!-- Target ISA -->
          <div class="ctrl-field">
            <label class="ctrl-label">Target Vector ISA</label>
            <select class="ctrl-select" id="ctrl-isa">
              <option value="AVX512">AVX-512 (512-bit ZMM)</option>
              <option value="AVX2" selected>AVX2 + FMA (256-bit YMM)</option>
              <option value="SSE4_2">SSE4.2 (128-bit XMM)</option>
              <option value="SCALAR">Scalar FP32 Baseline</option>
              <option value="OPENVINO_NPU">OpenVINO / NPU Offload</option>
              <option value="WASM_SIMD128">Wasm SIMD128 Client-Side</option>
            </select>
          </div>

          <!-- Profile -->
          <div class="ctrl-field">
            <label class="ctrl-label">SFVM Operational Profile</label>
            <select class="ctrl-select" id="ctrl-profile">
              <option value="NORMAX" selected>NORMAX (HPC High-Throughput)</option>
              <option value="SSIOLSC">SSIOLSC (Safety-Critical / ASIL-D)</option>
            </select>
          </div>
        </div>

        <div class="ctrl-actions-row">
          <button class="btn btn-primary" id="btn-wasm-run">
            ⚡ Run In-Browser Wasm SIMD Benchmark
          </button>
          <button class="btn btn-secondary" id="btn-pitstop-excise">
            ⚙ Trigger Pitstop Excise Test
          </button>
          <button class="btn btn-warning" id="btn-rmrp-plump">
            🧹 Run RMRP Compaction (Plump)
          </button>
          <button class="btn btn-outline" id="btn-self-heal">
            🛡 Run Self-Heal Sweep
          </button>
        </div>

        <div class="wasm-result-banner" id="wasm-result-banner" style="display: none;">
          <div class="wasm-banner-title">In-Browser Wasm Benchmark Finished</div>
          <div class="wasm-banner-content" id="wasm-banner-content"></div>
        </div>
      </div>
    `;
  }

  private attachEventListeners(): void {
    const selKernel = document.getElementById('ctrl-kernel') as HTMLSelectElement;
    const selSize = document.getElementById('ctrl-size') as HTMLSelectElement;
    const selIsa = document.getElementById('ctrl-isa') as HTMLSelectElement;
    const selProfile = document.getElementById('ctrl-profile') as HTMLSelectElement;

    const btnWasm = document.getElementById('btn-wasm-run');
    const btnPitstop = document.getElementById('btn-pitstop-excise');
    const btnRmrp = document.getElementById('btn-rmrp-plump');
    const btnSelfHeal = document.getElementById('btn-self-heal');

    const updateConfig = () => {
      if (selKernel) this.currentConfig.kernel = selKernel.value as any;
      if (selSize) this.currentConfig.workloadSize = parseInt(selSize.value, 10);
      if (selIsa) this.currentConfig.targetIsa = selIsa.value as SIMDTarget;
      if (selProfile) this.currentConfig.profile = selProfile.value as VMProfile;

      nativeBridge.sendConfig(this.currentConfig);
    };

    if (selKernel) selKernel.addEventListener('change', updateConfig);
    if (selSize) selSize.addEventListener('change', updateConfig);
    if (selIsa) selIsa.addEventListener('change', updateConfig);
    if (selProfile) selProfile.addEventListener('change', updateConfig);

    if (btnPitstop) {
      btnPitstop.addEventListener('click', () => {
        nativeBridge.triggerPitstopExcise();
      });
    }

    if (btnRmrp) {
      btnRmrp.addEventListener('click', () => {
        nativeBridge.triggerRMRP('PLUMP');
      });
    }

    if (btnSelfHeal) {
      btnSelfHeal.addEventListener('click', () => {
        nativeBridge.triggerSelfHealCheck();
      });
    }

    if (btnWasm) {
      btnWasm.addEventListener('click', async () => {
        btnWasm.setAttribute('disabled', 'true');
        btnWasm.textContent = '⏳ Executing In-Browser Wasm SIMD...';

        try {
          const res = await wasmBenchmarkRunner.runBenchmark(
            this.currentConfig.kernel as any,
            this.currentConfig.workloadSize
          );

          if (this.onWasmResultCallback) {
            this.onWasmResultCallback(res);
          }

          const banner = document.getElementById('wasm-result-banner');
          const content = document.getElementById('wasm-banner-content');
          if (banner && content) {
            banner.style.display = 'block';
            content.innerHTML = `
              <strong>${res.kernelName}</strong>: Completed in <code>${res.elapsedMs.toFixed(2)} ms</code> |
              Throughput: <code class="cyan">${res.gflops} GFLOPS</code> |
              Memory: <code class="green">${res.bandwidthGBs} GB/s</code> |
              Mode: <code>${res.simdType}</code>
            `;
          }
        } finally {
          btnWasm.removeAttribute('disabled');
          btnWasm.textContent = '⚡ Run In-Browser Wasm SIMD Benchmark';
        }
      });
    }
  }
}
