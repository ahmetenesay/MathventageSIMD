/**
 * =========================================================================
 * MathventageSIMD & SacculusForge SFVM Native Benchmark Harness
 * =========================================================================
 * File: bench_main.cpp
 * Language: C++20 with C-ABI Direct Kernel Dispatch
 *
 * Implements cycle-accurate hardware benchmarking via serialized RDTSC fences,
 * thread-affinity pinning, memory topography zone queries, and Intel ITT API markers.
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <cstdlib>
#include <malloc.h>

#ifdef _WIN32
    #include <windows.h>
    #include <intrin.h>
#else
    #include <x86intrin.h>
#endif

#include "SacculusForge.h"
#include "MathventageSIMD.h"

// =========================================================================
// INTEL ITT PROFILING HOOKS (VTune & Intel Advisor Integration)
// =========================================================================
#if defined(USE_INTEL_ITT) && __has_include(<ittnotify.h>)
    #include <ittnotify.h>
    static __itt_domain* g_itt_domain = __itt_domain_create("SFVM.Benchmark");
    #define ITT_TASK_BEGIN(name) __itt_task_begin(g_itt_domain, __itt_null, __itt_null, __itt_string_handle_create(name))
    #define ITT_TASK_END()       __itt_task_end(g_itt_domain)
#else
    #define ITT_TASK_BEGIN(name) ((void)0)
    #define ITT_TASK_END()       ((void)0)
#endif

// =========================================================================
// HARDWARE CYCLE TIMERS (Serialized RDTSCP)
// =========================================================================
static inline uint64_t rdtsc_start() {
#if defined(_MSC_VER) || defined(__clang__)
    _mm_lfence();
    return __rdtsc();
#else
    uint32_t lo, hi;
    __asm__ __volatile__("lfence; rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
#endif
}

static inline uint64_t rdtsc_end() {
#if defined(_MSC_VER) || defined(__clang__)
    unsigned int aux;
    uint64_t t = __rdtscp(&aux);
    _mm_lfence();
    return t;
#else
    uint32_t lo, hi, aux;
    __asm__ __volatile__("rdtscp; lfence" : "=a"(lo), "=d"(hi), "=c"(aux));
    return ((uint64_t)hi << 32) | lo;
#endif
}

// =========================================================================
// BENCHMARK RESULT STRUCTURE
// =========================================================================
struct BenchmarkResult {
    const char* kernel_name;
    const char* isa_name;
    size_t elements;
    uint64_t min_cycles;
    uint64_t p50_cycles;
    uint64_t p95_cycles;
    uint64_t p99_cycles;
    double avg_ns;
    double gflops;
    double bandwidth_gbs;
    double arithmetic_intensity; // FLOP / Byte
};

// =========================================================================
// RUNWAY & TENSOR ALLOCATION BENCHMARKS
// =========================================================================
void run_memory_topography_benchmark() {
    std::cout << "\n=======================================================\n";
    std::cout << "  [SFVM] Memory Topography & Pitstop Lifecycle Benchmark\n";
    std::cout << "=======================================================\n";

    size_t egg0 = 0, fled0 = 0, pit0 = 0;
    SF_custominiVm_runway_query_zones(&egg0, &fled0, &pit0);
    std::cout << "Initial Zones: Egg=" << egg0 << ", Fledgling=" << fled0 << ", Pitstop=" << pit0 << "\n";

    constexpr size_t N = 100;
    std::vector<SF_custominiVm_tensor_t> tensors(N);
    int32_t shape[] = {256, 256}; // 64K FP32 elements = 256 KB each

    uint64_t t0 = rdtsc_start();
    for (size_t i = 0; i < N; ++i) {
        tensors[i] = SF_custominiVm_tensor_alloc(shape, 2, SF_DTYPE_F32);
        SF_custominiVm_tensor_set_function_id(&tensors[i], 101);
    }
    uint64_t alloc_cycles = rdtsc_end() - t0;

    size_t egg1 = 0, fled1 = 0, pit1 = 0;
    SF_custominiVm_runway_query_zones(&egg1, &fled1, &pit1);
    std::cout << "Post-Alloc (100 tensors): Egg=" << egg1 << ", Fledgling=" << fled1 << "\n";
    std::cout << "Average Alloc Cost: " << (alloc_cycles / N) << " cycles/tensor\n";

    // Exercise Pitstop on tensor[0]
    t0 = rdtsc_start();
    int pit_rc = SF_custominiVm_pitstop_test_cycle(tensors[0].data, "AVX512_FMA", 2400);
    uint64_t pit_cycles = rdtsc_end() - t0;

    size_t egg2 = 0, fled2 = 0, pit2 = 0;
    SF_custominiVm_runway_query_zones(&egg2, &fled2, &pit2);
    std::cout << "Post-Pitstop Excise & Return (RC=" << pit_rc << "): Egg=" << egg2
              << ", Fledgling=" << fled2 << " (Took " << pit_cycles << " cycles)\n";

    // Trigger RMRP on-heap compaction
    t0 = rdtsc_start();
    size_t reclaimed = SF_custominiVm_run_rmrp();
    uint64_t rmrp_cycles = rdtsc_end() - t0;
    std::cout << "RMRP Compaction Pass: Reclaimed " << reclaimed << " bytes in " << rmrp_cycles << " cycles\n";

    // Free tensors
    for (size_t i = 0; i < N; ++i) {
        SF_custominiVm_tensor_free(&tensors[i]);
    }
}

// =========================================================================
// COMPUTE KERNEL BENCHMARKS (GEMM, FMA, CORDIC)
// =========================================================================
BenchmarkResult benchmark_fma_stream(size_t n, size_t iterations) {
    float* a = (float*)_aligned_malloc(n * sizeof(float), 64);
    float* b = (float*)_aligned_malloc(n * sizeof(float), 64);
    float* c = (float*)_aligned_malloc(n * sizeof(float), 64);

    if (!a || !b || !c) {
        if (a) _aligned_free(a);
        if (b) _aligned_free(b);
        if (c) _aligned_free(c);
        return {"Vector FMA Stream", "AVX2/AVX-512", n, 0, 0, 0, 0, 0, 0, 0, 0};
    }

    for (size_t i = 0; i < n; ++i) {
        a[i] = 1.0001f + (float)(i % 100) * 0.0001f;
        b[i] = 1.0002f;
        c[i] = 0.0005f;
    }

    std::vector<uint64_t> cycle_samples;
    cycle_samples.reserve(iterations);

    ITT_TASK_BEGIN("VectorFMA_Stream");

    for (size_t it = 0; it < iterations; ++it) {
        uint64_t t0 = rdtsc_start();
        
        // Simulating SIMD FMA kernel pass with unrolling
        #pragma omp simd
        for (size_t i = 0; i < n; ++i) {
            a[i] = a[i] * b[i] + c[i];
        }

        uint64_t t1 = rdtsc_end();
        cycle_samples.push_back(t1 - t0);
    }

    ITT_TASK_END();

    std::sort(cycle_samples.begin(), cycle_samples.end());
    uint64_t min_c = cycle_samples.front();
    uint64_t p50_c = cycle_samples[iterations * 50 / 100];
    uint64_t p95_c = cycle_samples[iterations * 95 / 100];
    uint64_t p99_c = cycle_samples[iterations * 99 / 100];

    // Estimate ns based on ~4.0 GHz clock
    double cpu_ghz = 4.0;
    double avg_ns = (p50_c / cpu_ghz);
    double flops = 2.0 * n;
    double gflops = (flops / (avg_ns * 1e-9)) / 1e9;
    double bytes_transferred = (3.0 * sizeof(float)) * n;
    double bw_gbs = (bytes_transferred / (avg_ns * 1e-9)) / 1e9;

    _aligned_free(a);
    _aligned_free(b);
    _aligned_free(c);

    return {
        "Vector FMA Stream",
        "AVX2/AVX-512",
        n,
        min_c,
        p50_c,
        p95_c,
        p99_c,
        avg_ns,
        gflops,
        bw_gbs,
        flops / bytes_transferred // Arithmetic Intensity (FLOP/Byte)
    };
}

int main(int argc, char** argv) {
    std::cout << "===============================================================\n";
    std::cout << "   MathventageSIMD & SacculusForge SFVM Native Benchmark Suit   \n";
    std::cout << "===============================================================\n" << std::endl;

    // 1. Boot SFVM Engine in NORMAX Profile
    int setup_rc = SF_custominiVm_setup_start(nullptr);
    if (setup_rc != 0) {
        std::cerr << "[-] SFVM setup failed with code: " << setup_rc << "\n";
        return 1;
    }
    SF_custominiVm_start();
    std::cout << "[+] SFVM Engine Booted Successfully." << std::endl;

    // 2. Query Silicon & Hardware Tier Topology
    SF_custominiVm_hardware_tier_t tier = SF_custominiVm_query_hardware_tier();
    const char* tier_name = SF_custominiVm_query_hardware_tier_name();
    uint32_t gc_th = 0, puz_th = 0, vm_th = 0;
    SF_custominiVm_query_thread_budget(&gc_th, &puz_th, &vm_th);

    std::cout << "[+] Hardware Tier Detected: [" << (int)tier << "] " << (tier_name ? tier_name : "Unknown") << "\n";
    std::cout << "[+] Adaptive Thread Budget: GC=" << gc_th << ", Puzzle=" << puz_th << ", VM=" << vm_th << std::endl;

    // 3. Bind Calling Thread to Primary Execution Core for Jitter Elimination
    SF_custominiVm_bind_calling_thread(SF_CORE_TIER_PERFORMANCE);

    // 4. Memory Topography Benchmarks
    run_memory_topography_benchmark();

    // 5. Execution Kernel Benchmarks
    std::cout << "\n=======================================================\n";
    std::cout << "  [HPC] Microarchitecture Vector Kernel Benchmarks     \n";
    std::cout << "=======================================================\n" << std::endl;

    std::vector<size_t> sizes = {4096, 65536, 262144, 1048576};
    for (size_t sz : sizes) {
        auto res = benchmark_fma_stream(sz, 100);
        std::cout << std::left << std::setw(20) << res.kernel_name
                  << " | N=" << std::setw(8) << res.elements
                  << " | p50=" << std::setw(7) << res.p50_cycles << " cyc"
                  << " | " << std::setw(6) << std::fixed << std::setprecision(1) << res.gflops << " GFLOPS"
                  << " | " << std::setw(6) << std::setprecision(1) << res.bandwidth_gbs << " GB/s"
                  << " | AI=" << std::setprecision(2) << res.arithmetic_intensity << " FLOP/B" << std::endl;
    }

    std::cout << "\n[+] Benchmark Suite Completed Successfully." << std::endl;
    std::cout << "[+] Shutting down SFVM..." << std::endl;
    SF_custominiVm_shutdown();
    std::cout << std::flush;
    std::fflush(stdout);
#ifdef _WIN32
    TerminateProcess(GetCurrentProcess(), 0);
#endif
    return 0;
}
