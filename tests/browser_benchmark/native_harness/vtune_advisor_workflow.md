# Intel® VTune™ Profiler & Intel® Advisor Deep Profiling Workflow for MathventageSIMD & libsfmsimd

This guide provides the exact architecture, analysis types, and CLI workflows for profiling `libsfmsimd` and SacculusForge Virtual Machine (SFVM) on x86_64 silicon (Intel Core Hybrid 13th/14th Gen / Ultra / Xeon).

---

## 1. Intel® VTune™ Profiler Architecture & Setup

Intel VTune Profiler measures hardware Performance Monitoring Unit (PMU) counters with zero instrumentation overhead.

### A. Recommended Analysis Modes for libsfmsimd

1. **Microarchitecture Exploration (Top-Down / TMA):**
   - Dissects CPU execution cycles into 4 primary bottlenecks:
     - **Retiring:** Percentage of pipeline slots executing productive SIMD/ALU instructions (Goal: >60%).
     - **Bad Speculation:** Mispredicted branches from `SF_custominiVm_branch_predict()`.
     - **Front-End Bound:** Instruction cache misses, ITLB stalls, or μop queue starvation.
     - **Back-End Bound:** Memory stalls (L1/L2/L3/DRAM) vs Execution Port contention (e.g. Port 0/1/5 ALU saturation).

2. **Memory Access Analysis:**
   - Evaluates memory bandwidth utilization across DRAM, L3 LLC, and L2.
   - Detects unaligned SIMD loads/stores causing split locks.
   - Validates SFVM's **Runway Motherline** vs **Tank Regions** vs **Landfill** cache residency.

3. **Threading & Concurrency (Hybrid P-Core vs E-Core):**
   - Monitors thread distribution from `SF_custominiVm_query_thread_budget()` across Performance cores (P-cores) and Efficient cores (E-cores).
   - Verifies lockless Ring-Buffer behavior in HotLog and ColdLog.

### B. VTune Command-Line Profiling Commands

```bash
# 1. Top-Down Microarchitecture Exploration
vtune -collect uarch-exploration -knob sampling-interval=1 \
      -app-working-dir C:\Users\aziml\my_projects\MathventageSIMD\tests\browser_benchmark\native_harness \
      -- ./build/sf_bench_main.exe

# 2. Memory Access & Cacheline Contention Analysis
vtune -collect memory-access -knob analyze-mem-objects=true \
      -app-working-dir C:\Users\aziml\my_projects\MathventageSIMD\tests\browser_benchmark\native_harness \
      -- ./build/sf_bench_main.exe

# 3. Generate Hotspots Summary Report
vtune -report hotspots -result-dir vtune_data_dir -format text -report-output vtune_hotspots_summary.txt
```

---

## 2. Intel® Advisor: Roofline Model & Vectorization

Intel Advisor is the premier tool for vector optimization, memory traffic analysis, and Roofline modeling.

### A. What Intel Advisor Measures in libsfmsimd

1. **Cache-Aware Roofline Model:**
   - Plots each MathventageSIMD kernel (e.g. `dispatch_msimd_gemm`, `dispatch_msimd_acoustic_fdtd_step_3d`, `dispatch_msimd_c_mul`) as a point on the Arithmetic Intensity (FLOP/Byte) vs GFLOPS chart.
   - Shows whether a kernel is hitting the DRAM Bandwidth ceiling, L3/L2 ceiling, or Vector Compute Peak (AVX2/AVX-512 FMA).

2. **Vectorization Efficiency & Register Pressure:**
   - Vector register length (128-bit XMM, 256-bit YMM, 512-bit ZMM).
   - Vector mask efficiency and gather/scatter overhead.
   - FMA (Fused Multiply-Add) instruction ratio.

3. **Memory Access Pattern (MAP) Analysis:**
   - Measures stride: Unit Stride (stride 1) vs Strided vs Random.
   - Highlights non-contiguous access in lattice arrays.

### B. Intel Advisor Command-Line Workflow

```bash
# Step 1: Survey Collection (Instruction mix, loop counts, call tree)
advixe-cl -collect survey \
          -project-dir ./advisor_sfvm \
          -- ./build/sf_bench_main.exe

# Step 2: Trip Counts & FLOP Collection (Arithmetic Intensity & Roofline)
advixe-cl -collect tripcounts -flop \
          -project-dir ./advisor_sfvm \
          -- ./build/sf_bench_main.exe

# Step 3: Generate Roofline Model Report (Interactive HTML/CSV)
advixe-cl -report roofline \
          -project-dir ./advisor_sfvm \
          -report-output ./advisor_sfvm/roofline_report.html
```

---

## 3. Intel ITT API Instrumentation in C++ Code

To isolate benchmark kernels inside VTune and Advisor without profiling startup/shutdown code, use Intel ITT (`<ittnotify.h>`):

```cpp
#include <ittnotify.h>

__itt_domain* domain = __itt_domain_create("SFVM.Lattice");
__itt_string_handle* task_handle = __itt_string_handle_create("FDTD_3D_AVX2");

// Start profiling region
__itt_task_begin(domain, __itt_null, __itt_null, task_handle);

// Execute SIMD kernel
dispatch_msimd_acoustic_fdtd_step_3d(...);

// End profiling region
__itt_task_end(domain);
```
