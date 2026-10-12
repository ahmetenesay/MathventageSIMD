# MathventageSIMD & SacculusForge SFVM Browser Benchmark Suite

A high-performance, dark/cyberpunk/HPC benchmark dashboard built with **TypeScript + Vite**, visualizing silicon exploitation metrics, live RDTSC hardware cycle telemetry, multi-generational memory topography, and cache-aware Roofline models.

---

## 🚀 Quick Start

### 1. Launch the Browser Benchmark UI
```bash
cd tests/browser_benchmark
npm run dev
```
Open [http://localhost:5173](http://localhost:5173) in your browser.

- Out-of-the-box, the dashboard runs in **High-Fidelity Simulation Mode**, generating realistic silicon performance numbers and dynamic SFVM memory topography updates.
- Click **"⚡ Run In-Browser Wasm SIMD Benchmark"** to execute real client-side SIMD / TypedArray math kernels directly inside your browser and compare them against native execution!

### 2. Launch the Local Telemetry Bridge Daemon (Optional)
To feed live RDTSC hardware counters from `libsfmsimd`:
```bash
node bridge_daemon.cjs
```
The browser dashboard will automatically detect the bridge and switch to `NATIVE LIBWS BRIDGE (ACTIVE)`.

---

## 📊 Dashboard Modules

1. **Tensor & Compute Throughput Gauges:**
   - Real-time GFLOPS / TFLOPS compute meter.
   - Memory Bandwidth (GB/s) gauge with DRAM / L3 / L2 / L1 utilization ceilings.
   - Pipeline IPC (Instructions per Cycle) and ALU Queue Dispatch counter.
   - Real-time speedup multipliers vs Scalar baseline and vs in-browser WebAssembly SIMD.

2. **Cache-Aware Roofline Model Chart:**
   - HTML5 Canvas log-log plot (Arithmetic Intensity [FLOP/Byte] vs Attainable GFLOPS).
   - Dynamic memory bandwidth diagonals for DRAM (76.8 GB/s), L3 LLC (580 GB/s), and L1d Cache (3400 GB/s).
   - Horizontal vector execution ceilings: Scalar FP32 (67 GF), AVX2 256-bit FMA (1075 GF), AVX-512 512-bit FMA (2150 GF), and OpenVINO/NPU Peak (4800 GF).
   - Real-time kernel placement showing whether operations are **Memory-Bound** or **Compute-Bound**.

3. **Cycle-Accurate RDTSC Latency Histogram:**
   - Real-time distribution with p50, p90, p95, p99, and p99.9 percentile markers.
   - Microsecond & nanosecond hardware jitter telemetry.

4. **SFVM Memory Topography Visualizer:**
   - **Runway (Young & Nursery Domain):**
     - **Motherline:** Live tensor allocation partition into **Egg Zone** (newborn unoptimized candidates) and **Fledgling Zone** (promoted objects running verified ISA intrinsics).
     - **Falline:** Segregated Empty Box chain for zero-syscall LIFO/FIFO memory reuse.
   - **Pitstop:** In-place excised tensor mutation and ISA auto-tuning (e.g. testing AVX512_BF16 vs AVX2) without heap compaction.
   - **Tank Regions (Humongous Domain):** Large-page contiguous memory allocations bypassing generational compaction.
   - **Landfill (Mature Domain):** Evacuated mature objects undergoing reclamation or pagefile swap.
   - **Yapbozcu Puzzle Arena:** Isolated auto-tuning sandbox running concurrent vector kernel shootouts.

5. **Live 4-Point Topology & Self-Healing Telemetry:**
   - Tracks SFVM's 4 execution points: **Safepoints**, **Controlpoints**, **Hookpoints (prologue/epilogue)**, and **Pausepoint/Interruptpoint preemption gate**.
   - Monitors sfGC Self-Healing counters: repaired corruptions, broken cyclic Brooks forwarding pointers, and shielded bitcode exceptions.

---

## 🏛 Architectural Answers to Design Questions

### 1. Does the browser communicate with `libsfmsimd` and run benchmarks?
- **Sandbox Reality**: A web browser (Chrome/Firefox/Edge) is sandboxed and cannot directly `dlopen()` or `LoadLibrary()` native Win32 `.dll` binaries (`libsfmsimd.dll`) or execute Ring-0/RDTSC instructions directly. Furthermore, browser `performance.now()` is deliberately jittered and quantized to 5-20 microseconds for Spectre mitigation.
- **The Solution**: A **Dual-Mode Telemetry Architecture**:
  1. **Native RDTSC Daemon Bridge**: A lightweight native daemon links to `libsfmsimd.dll`, executes benchmarks using hardware serialized `__rdtscp` fences and thread pinning (`SF_custominiVm_bind_calling_thread`), and streams JSON telemetry over `ws://localhost:9099/stream` to the browser.
  2. **In-Browser WebAssembly SIMD Comparator**: The browser runs Wasm SIMD128 kernels (GEMM, FMA, Neumaier sum, Lattice Laplacian) client-side. This allows direct side-by-side comparison between **Browser Wasm SIMD** and **Native libsfmsimd AVX2/AVX-512**, dramatically highlighting native silicon exploitation!

### 2. Benchmark Language: C++ or C?
- **Recommendation: C++20 for the Harness, C-ABI for Kernel Invocations.**
  - **Why C++20 for Harness**:
    - **RAII Memory Management**: Deterministic cleanup of huge tensor slabs, Tank regions, and aligned memory buffers.
    - **Chrono & Atomics**: Clean standard primitives (`std::chrono::high_resolution_clock`, `std::jthread`, `std::atomic`).
    - **Templated Sweeps**: Clean parametrization over tensor shapes, types (`f32`, `f64`, `bf16`), and strides without code duplication.
    - **Integration with `SF::` C++ Facade**: Direct access to `SF::EntangleScope`, `SF::GroupCompute`, and `SF::HardwareBudget`.
  - **Why C for Invocation**: The actual calls into `libsfmsimd` utilize the pure `extern "C"` ABI functions (`msimd_init`, `dispatch_msimd_*`, `SF_custominiVm_*`). This eliminates C++ virtual method overhead and name mangling issues, ensuring zero overhead.

### 3. Should we use Intel Advisor & VTune?
- **Recommendation: ABSOLUTELY YES.**
  - **Intel VTune Profiler**:
    - **Top-Down Microarchitecture Exploration (TMA)**: Detects whether bottlenecks are Front-End Bound, Bad Speculation, Memory Bound (L1/L2/L3/DRAM), or Core Bound (Port ALU saturation).
    - **Memory Access Profiling**: Verifies 64-byte cacheline alignment, non-temporal streaming stores (`SF_custominiVm_dram_stream_store`), and SRAM vs DRAM volatility.
    - **Hybrid P/E-Core Threading**: Analyzes thread scheduling between Performance and Efficient cores.
  - **Intel Advisor**:
    - **Cache-Aware Roofline Model**: Measures true Arithmetic Intensity (FLOP/Byte) and attainable GFLOPS on your CPU silicon.
    - **Vectorization Efficiency**: Inspects register occupancy (XMM 128 vs YMM 256 vs ZMM 512), vector loop trip counts, and FMA utilization.
    - **Memory Access Patterns (MAP)**: Detects strided or indirect access penalties in 3D lattice simulations.
  - Full CLI instructions and workflow guides are available in [`native_harness/vtune_advisor_workflow.md`](file:///C:/Users/aziml/my_projects/MathventageSIMD/tests/browser_benchmark/native_harness/vtune_advisor_workflow.md).
