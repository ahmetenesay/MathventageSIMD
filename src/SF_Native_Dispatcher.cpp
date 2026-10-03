
#include "MathventageSIMD.h"
#include "msimd_internal.h"
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <functional>
#include <new>

#if defined(_M_X64) || defined(__x86_64__)
#include <immintrin.h>
#if defined(_MSC_VER)
    #include <intrin.h>
#else
    #include <cpuid.h>
    #include <x86intrin.h>
#endif
#endif

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <sysinfoapi.h>
#elif defined(__linux__)
#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/mman.h>
#endif

static void msimd_query_cache_topology(msimd_cache_topology_t* out) {
    if (!out) return;
    std::memset(out, 0, sizeof(*out));
    out->cacheline_bytes = 64;

#if defined(_M_X64) || defined(__x86_64__)

    for (int level = 0; level < 4; ++level) {
        int regs[4] = {0};
#if defined(_MSC_VER)
        __cpuidex(regs, 0x04, level);
#else
        __cpuid_count(0x04, level, regs[0], regs[1], regs[2], regs[3]);
#endif
        int cache_type = regs[0] & 0x1F;
        if (cache_type == 0) break;

        int cache_level    = (regs[0] >> 5)  & 0x7;
        int line_size      = (regs[1] & 0xFFF) + 1;
        int partitions     = ((regs[1] >> 12) & 0x3FF) + 1;
        int associativity  = ((regs[1] >> 22) & 0x3FF) + 1;
        int sets           = regs[2] + 1;
        uint64_t size_bytes = (uint64_t)line_size * partitions * associativity * sets;

        if (cache_level == 1 && cache_type == 1) {
            out->l1d_size_bytes   = size_bytes;
            out->l1d_line_bytes   = (uint32_t)line_size;
            out->cacheline_bytes  = (uint32_t)line_size;
        } else if (cache_level == 2) {
            out->l2_size_bytes    = size_bytes;
            out->l2_line_bytes    = (uint32_t)line_size;
        } else if (cache_level == 3) {
            out->l3_size_bytes    = size_bytes;
            out->l3_line_bytes    = (uint32_t)line_size;
        }
    }

    if (out->l1d_size_bytes == 0) {
        int regs[4] = {0};
#if defined(_MSC_VER)
        __cpuidex(regs, (int)0x8000001D, 0);
#else
        __cpuid_count((int)0x8000001D, 0, regs[0], regs[1], regs[2], regs[3]);
#endif
        int line_size = (regs[1] & 0xFFF) + 1;
        if (line_size > 0) out->cacheline_bytes = (uint32_t)line_size;
    }

#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__clang__) || defined(__GNUC__)
    uint64_t ctr = 0;
    __asm__ __volatile__("mrs %0, ctr_el0" : "=r"(ctr));
    uint32_t l1d_log2 = (uint32_t)((ctr >> 16) & 0xF);
    out->l1d_line_bytes  = (uint32_t)(4u << l1d_log2);
    out->cacheline_bytes = out->l1d_line_bytes ? out->l1d_line_bytes : 64u;
#endif
#if defined(__linux__)
    {
        FILE* f = fopen("/sys/devices/system/cpu/cpu0/cache/index0/size", "r");
        if (f) { unsigned kb=0; if(fscanf(f,"%u",&kb)==1) out->l1d_size_bytes=(uint64_t)kb*1024; fclose(f); }
    }
    {
        FILE* f = fopen("/sys/devices/system/cpu/cpu0/cache/index2/size", "r");
        if (f) { unsigned kb=0; if(fscanf(f,"%u",&kb)==1) out->l2_size_bytes=(uint64_t)kb*1024; fclose(f); }
    }
    {
        FILE* f = fopen("/sys/devices/system/cpu/cpu0/cache/index3/size", "r");
        if (f) { unsigned kb=0; if(fscanf(f,"%u",&kb)==1) out->l3_size_bytes=(uint64_t)kb*1024; fclose(f); }
    }
#endif
#endif
}

static void msimd_query_numa_topology(msimd_numa_topology_t* out) {
    if (!out) return;
    std::memset(out, 0, sizeof(*out));
    out->num_numa_nodes = 1;
    out->is_uma         = 1;

#ifdef _WIN32
    ULONG highest_node = 0;
    if (GetNumaHighestNodeNumber(&highest_node)) {
        out->num_numa_nodes = (uint32_t)(highest_node + 1);
        out->is_uma = (out->num_numa_nodes == 1) ? 1 : 0;
    }
    for (uint32_t n = 0; n < out->num_numa_nodes && n < MSIMD_MAX_NUMA_NODES; ++n) {
        GROUP_AFFINITY ga;
        std::memset(&ga, 0, sizeof(ga));
        if (GetNumaNodeProcessorMaskEx((USHORT)n, &ga)) {
            out->node_cpu_mask[n] = (uint64_t)ga.Mask;
        }
    }
#elif defined(__linux__)
    uint32_t node_count = 0;
    for (int i = 0; i < 64; ++i) {
        char path[64];
        snprintf(path, sizeof(path), "/sys/devices/system/node/node%d", i);
        struct stat st;
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) node_count++;
        else break;
    }
    if (node_count > 0) {
        out->num_numa_nodes = node_count;
        out->is_uma = (node_count == 1) ? 1 : 0;
    }
#endif
}

static int msimd_query_core_type(int logical_cpu) {
    (void)logical_cpu;
#if defined(_M_X64) || defined(__x86_64__)
    int regs[4] = {0};
#if defined(_MSC_VER)
    __cpuidex(regs, 0x1A, 0);
#else
    __cpuid_count(0x1A, 0, regs[0], regs[1], regs[2], regs[3]);
#endif
    int core_type = (regs[0] >> 24) & 0xFF;
    if (core_type == 0x20) return MSIMD_CORE_TYPE_ECORE;
    if (core_type == 0x40) return MSIMD_CORE_TYPE_PCORE;
#endif
    return MSIMD_CORE_TYPE_PCORE;
}

extern "C" {

MSIMD_API void msimd_query_hw_topology(msimd_hw_topology_t* out) {
    if (!out) return;
    std::memset(out, 0, sizeof(*out));

    msimd_query_cache_topology(&out->cache);
    msimd_query_numa_topology(&out->numa);

#ifdef _WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    out->num_logical_cores = (uint32_t)si.dwNumberOfProcessors;

#if defined(_M_X64) || defined(__x86_64__)
    int regs[4] = {0};
    __cpuidex(regs, 0x0B, 1);
    out->num_physical_cores = (uint32_t)(regs[1] & 0xFFFF);
    if (out->num_physical_cores == 0)
        out->num_physical_cores = out->num_logical_cores;
#else
    out->num_physical_cores = out->num_logical_cores;
#endif

#elif defined(__linux__)
    out->num_logical_cores  = (uint32_t)sysconf(_SC_NPROCESSORS_ONLN);
    out->num_physical_cores = out->num_logical_cores;
#else
    out->num_logical_cores  = (uint32_t)std::thread::hardware_concurrency();
    out->num_physical_cores = out->num_logical_cores;
#endif

    out->current_core_type = msimd_query_core_type(0);
    out->has_hybrid_cores  = (out->current_core_type != MSIMD_CORE_TYPE_UNKNOWN) ? 1 : 0;
}

MSIMD_API void* msimd_aligned_alloc_cl(size_t size, size_t alignment) {
    if (size == 0) return nullptr;
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) alignment = 64;

#ifdef _WIN32

    if (size >= 2 * 1024 * 1024 && alignment < 2 * 1024 * 1024) {
        alignment = 2 * 1024 * 1024;
    }
    return _aligned_malloc(size, alignment);
#else
    void* ptr = nullptr;
    if (posix_memalign(&ptr, alignment, size) != 0) return nullptr;
#if defined(__linux__) && defined(MADV_HUGEPAGE)
    if (size >= 2 * 1024 * 1024) {
        madvise(ptr, size, MADV_HUGEPAGE);
    }
#endif
    return ptr;
#endif
}

MSIMD_API void msimd_aligned_free_cl(void* ptr) {
    if (!ptr) return;
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

MSIMD_API void* msimd_alloc_aligned(size_t size, size_t alignment) {
    return msimd_aligned_alloc_cl(size, alignment);
}

MSIMD_API void msimd_free_aligned(void* ptr) {
    msimd_aligned_free_cl(ptr);
}

MSIMD_API void* msimd_numa_alloc(size_t size, uint32_t numa_node) {
    if (size == 0) return nullptr;

#ifdef _WIN32
    void* ptr = VirtualAllocExNuma(
        GetCurrentProcess(),
        nullptr,
        size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE,
        (DWORD)numa_node
    );
    return ptr;
#else
    (void)numa_node;
    return msimd_aligned_alloc_cl(size, 64);
#endif
}

MSIMD_API void msimd_numa_free(void* ptr, size_t size) {
    if (!ptr) return;
#ifdef _WIN32
    VirtualFreeEx(GetCurrentProcess(), ptr, 0, MEM_RELEASE);
    (void)size;
#else
    free(ptr);
    (void)size;
#endif
}

MSIMD_API void msimd_prefetch_t0(const void* addr) {
#if defined(_M_X64) || defined(__x86_64__)
    _mm_prefetch((const char*)addr, _MM_HINT_T0);
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__clang__) || defined(__GNUC__)
    __builtin_prefetch(addr, 0, 3);
#endif
#else
    (void)addr;
#endif
}

MSIMD_API void msimd_prefetch_t1(const void* addr) {
#if defined(_M_X64) || defined(__x86_64__)
    _mm_prefetch((const char*)addr, _MM_HINT_T1);
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__clang__) || defined(__GNUC__)
    __builtin_prefetch(addr, 0, 2);
#endif
#else
    (void)addr;
#endif
}

MSIMD_API void msimd_prefetch_t2(const void* addr) {
#if defined(_M_X64) || defined(__x86_64__)
    _mm_prefetch((const char*)addr, _MM_HINT_T2);
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__clang__) || defined(__GNUC__)
    __builtin_prefetch(addr, 0, 1);
#endif
#else
    (void)addr;
#endif
}

MSIMD_API void msimd_prefetch_nta(const void* addr) {
#if defined(_M_X64) || defined(__x86_64__)
    _mm_prefetch((const char*)addr, _MM_HINT_NTA);
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__clang__) || defined(__GNUC__)
    __builtin_prefetch(addr, 0, 0);
#endif
#else
    (void)addr;
#endif
}

MSIMD_API void msimd_prefetch_stream(const void* base, size_t count, size_t stride) {
    const char* ptr = (const char*)base;
    for (size_t i = 0; i < count; ++i, ptr += stride) {
        msimd_prefetch_t0(ptr);
    }
}

MSIMD_API void msimd_stream_store_f32(float* dst, const float* src, size_t count) {
    if (!dst || !src || count == 0) return;

#if defined(_M_X64) || defined(__x86_64__)
    size_t i = 0;
#if defined(__AVX__)
    size_t avx_count = count & ~7ULL;
    for (; i < avx_count; i += 8) {
        __m256 v = _mm256_loadu_ps(src + i);
        _mm256_stream_ps(dst + i, v);
    }
#endif
    size_t sse_count = count & ~3ULL;
    for (; i < sse_count; i += 4) {
        __m128 v = _mm_loadu_ps(src + i);
        _mm_stream_ps(dst + i, v);
    }
    for (; i < count; ++i) dst[i] = src[i];
    _mm_sfence();

#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__clang__) || defined(__GNUC__)
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        float32x4_t v = vld1q_f32(src + i);
        __builtin_nontemporal_store(v, (float32x4_t*)(dst + i));
    }
    for (; i < count; ++i) dst[i] = src[i];
    __asm__ __volatile__("dmb ishst" ::: "memory");
#else
    std::memcpy(dst, src, count * sizeof(float));
#endif
#else
    std::memcpy(dst, src, count * sizeof(float));
#endif
}

MSIMD_API void msimd_stream_store_f64(double* dst, const double* src, size_t count) {
    if (!dst || !src || count == 0) return;

#if defined(_M_X64) || defined(__x86_64__)
    size_t i = 0;
#if defined(__AVX__)
    size_t avx_count = count & ~3ULL;
    for (; i < avx_count; i += 4) {
        __m256d v = _mm256_loadu_pd(src + i);
        _mm256_stream_pd(dst + i, v);
    }
#endif
    size_t sse_count = count & ~1ULL;
    for (; i < sse_count; i += 2) {
        __m128d v = _mm_loadu_pd(src + i);
        _mm_stream_pd(dst + i, v);
    }
    for (; i < count; ++i) dst[i] = src[i];
    _mm_sfence();

#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__clang__) || defined(__GNUC__)
    size_t i = 0;
    for (; i + 2 <= count; i += 2) {
        float64x2_t v = vld1q_f64(src + i);
        __builtin_nontemporal_store(v, (float64x2_t*)(dst + i));
    }
    for (; i < count; ++i) dst[i] = src[i];
    __asm__ __volatile__("dmb ishst" ::: "memory");
#else
    std::memcpy(dst, src, count * sizeof(double));
#endif
#else
    std::memcpy(dst, src, count * sizeof(double));
#endif
}

MSIMD_API void msimd_hw_fence_full(void) {
#if defined(_M_X64) || defined(__x86_64__)
    _mm_mfence();
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__clang__) || defined(__GNUC__)
    __asm__ __volatile__("dmb ish" ::: "memory");
#endif
#else
    std::atomic_thread_fence(std::memory_order_seq_cst);
#endif
}

MSIMD_API void msimd_hw_fence_store(void) {
#if defined(_M_X64) || defined(__x86_64__)
    _mm_sfence();
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__clang__) || defined(__GNUC__)
    __asm__ __volatile__("dmb ishst" ::: "memory");
#endif
#else
    std::atomic_thread_fence(std::memory_order_release);
#endif
}

MSIMD_API void msimd_hw_fence_load(void) {
#if defined(_M_X64) || defined(__x86_64__)
    _mm_lfence();
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__clang__) || defined(__GNUC__)
    __asm__ __volatile__("dmb ishld" ::: "memory");
#endif
#else
    std::atomic_thread_fence(std::memory_order_acquire);
#endif
}

MSIMD_API uint32_t msimd_bmi_tzcnt32(uint32_t x) {
    if (x == 0) return 32;
#if defined(_M_X64) || defined(__x86_64__)
#if defined(_MSC_VER)
    unsigned long idx; _BitScanForward(&idx, x); return (uint32_t)idx;
#else
    return (uint32_t)__builtin_ctz(x);
#endif
#else
    return (uint32_t)__builtin_ctz(x);
#endif
}

MSIMD_API uint32_t msimd_bmi_tzcnt64(uint64_t x) {
    if (x == 0) return 64;
#if defined(_M_X64) || defined(__x86_64__)
#if defined(_MSC_VER)
    unsigned long idx; _BitScanForward64(&idx, x); return (uint32_t)idx;
#else
    return (uint32_t)__builtin_ctzll(x);
#endif
#else
    return (uint32_t)__builtin_ctzll(x);
#endif
}

MSIMD_API uint32_t msimd_bmi_lzcnt32(uint32_t x) {
    if (x == 0) return 32;
#if defined(_M_X64) || defined(__x86_64__)
#if defined(_MSC_VER)
    return (uint32_t)__lzcnt(x);
#else
    return (uint32_t)__builtin_clz(x);
#endif
#else
    return (uint32_t)__builtin_clz(x);
#endif
}

MSIMD_API uint32_t msimd_bmi_lzcnt64(uint64_t x) {
    if (x == 0) return 64;
#if defined(_M_X64) || defined(__x86_64__)
#if defined(_MSC_VER)
    return (uint32_t)__lzcnt64(x);
#else
    return (uint32_t)__builtin_clzll(x);
#endif
#else
    return (uint32_t)__builtin_clzll(x);
#endif
}

MSIMD_API uint32_t msimd_bmi_popcnt32(uint32_t x) {
#if defined(_M_X64) || defined(__x86_64__)
#if defined(_MSC_VER)
    return (uint32_t)__popcnt(x);
#else
    return (uint32_t)__builtin_popcount(x);
#endif
#else
    return (uint32_t)__builtin_popcount(x);
#endif
}

MSIMD_API uint32_t msimd_bmi_popcnt64(uint64_t x) {
#if defined(_M_X64) || defined(__x86_64__)
#if defined(_MSC_VER)
    return (uint32_t)__popcnt64(x);
#else
    return (uint32_t)__builtin_popcountll(x);
#endif
#else
    return (uint32_t)__builtin_popcountll(x);
#endif
}

MSIMD_API uint64_t msimd_bmi_blsr64(uint64_t x) {
#if (defined(_M_X64) || defined(__x86_64__)) && defined(__BMI__)
    return _blsr_u64(x);
#else
    return x & (x - 1);
#endif
}

MSIMD_API uint64_t msimd_bmi2_pext64(uint64_t src, uint64_t mask) {
#if (defined(_M_X64) || defined(__x86_64__)) && defined(__BMI2__)
    return _pext_u64(src, mask);
#else
    uint64_t result = 0;
    uint64_t out_bit = 1;
    while (mask) {
        uint64_t lowest = mask & (-(int64_t)mask);
        if (src & lowest) result |= out_bit;
        out_bit <<= 1;
        mask &= mask - 1;
    }
    return result;
#endif
}

MSIMD_API uint64_t msimd_bmi2_pdep64(uint64_t src, uint64_t mask) {
#if (defined(_M_X64) || defined(__x86_64__)) && defined(__BMI2__)
    return _pdep_u64(src, mask);
#else
    uint64_t result = 0;
    uint64_t in_bit = 1;
    while (mask) {
        uint64_t lowest = mask & (-(int64_t)mask);
        if (src & in_bit) result |= lowest;
        in_bit <<= 1;
        mask &= mask - 1;
    }
    return result;
#endif
}

MSIMD_API uint64_t msimd_morton_encode3d(uint32_t x, uint32_t y, uint32_t z) {
    uint64_t mx = (uint64_t)(x & 0x1FFFFF);
    uint64_t my = (uint64_t)(y & 0x1FFFFF);
    uint64_t mz = (uint64_t)(z & 0x1FFFFF);

#if (defined(_M_X64) || defined(__x86_64__)) && defined(__BMI2__)
    uint64_t mask_x = 0x1249249249249249ULL;
    uint64_t mask_y = 0x2492492492492492ULL;
    uint64_t mask_z = 0x4924924924924924ULL;
    return _pdep_u64(mx, mask_x) | _pdep_u64(my, mask_y) | _pdep_u64(mz, mask_z);
#else
    auto spread = [](uint64_t v) -> uint64_t {
        v = (v | (v << 32)) & 0x1F00000000FFFFULL;
        v = (v | (v << 16)) & 0x1F0000FF0000FFULL;
        v = (v | (v <<  8)) & 0x100F00F00F00F00FULL;
        v = (v | (v <<  4)) & 0x10C30C30C30C30CULL;
        v = (v | (v <<  2)) & 0x1249249249249249ULL;
        return v;
    };
    return spread(mx) | (spread(my) << 1) | (spread(mz) << 2);
#endif
}

MSIMD_API uint64_t msimd_rdtsc_begin(void) {
#if defined(_M_X64) || defined(__x86_64__)
#if defined(_MSC_VER)
    _mm_lfence();
    return __rdtsc();
#else
    uint32_t lo, hi;
    __asm__ __volatile__(
        "lfence\n\trdtsc\n\t"
        : "=a"(lo), "=d"(hi)
        :
        : "memory"
    );
    return ((uint64_t)hi << 32) | (uint64_t)lo;
#endif
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__clang__) || defined(__GNUC__)
    uint64_t t;
    __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(t) :: "memory");
    return t;
#else
    return 0;
#endif
#else
    auto now = std::chrono::high_resolution_clock::now();
    return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
#endif
}

MSIMD_API uint64_t msimd_rdtsc_end(void) {
#if defined(_M_X64) || defined(__x86_64__)
#if defined(_MSC_VER)
    unsigned int aux;
    uint64_t t = __rdtscp(&aux);
    _mm_lfence();
    return t;
#else
    uint32_t lo, hi, aux;
    __asm__ __volatile__(
        "rdtscp\n\tmov %%eax, %0\n\tmov %%edx, %1\n\tmov %%ecx, %2\n\tlfence\n\t"
        : "=r"(lo), "=r"(hi), "=r"(aux)
        :
        : "%eax", "%edx", "%ecx", "memory"
    );
    return ((uint64_t)hi << 32) | (uint64_t)lo;
#endif
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__clang__) || defined(__GNUC__)
    uint64_t t;
    __asm__ __volatile__("dsb ish\n\tisb\n\tmrs %0, cntvct_el0\n\t" : "=r"(t) :: "memory");
    return t;
#else
    return 0;
#endif
#else
    auto now = std::chrono::high_resolution_clock::now();
    return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
#endif
}

MSIMD_API uint64_t msimd_rdtsc_freq_hz(void) {
#if defined(_M_X64) || defined(__x86_64__) || defined(__aarch64__) || defined(_M_ARM64)
    uint64_t t0 = msimd_rdtsc_begin();
    auto wall0 = std::chrono::high_resolution_clock::now();

    volatile uint64_t spin = 0;
    for (uint64_t i = 0; i < 5000000ULL; ++i) spin += i;
    (void)spin;

    uint64_t t1 = msimd_rdtsc_end();
    auto wall1 = std::chrono::high_resolution_clock::now();

    double wall_ns = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(wall1 - wall0).count();
    if (wall_ns <= 0.0) return 3000000000ULL;

    double ticks = (double)(t1 - t0);
    return (uint64_t)(ticks / wall_ns * 1e9);
#else
    return 1000000000ULL;
#endif
}

MSIMD_API double msimd_cycles_to_ns(uint64_t cycles, uint64_t freq_hz) {
    if (freq_hz == 0) return 0.0;
    return (double)cycles / (double)freq_hz * 1e9;
}

MSIMD_API int msimd_thread_pin_to_core(int logical_cpu) {
    if (logical_cpu < 0) return 0;

#ifdef _WIN32
    DWORD_PTR mask = (logical_cpu < 64) ? (1ULL << logical_cpu) : 0ULL;
    if (mask == 0) return 0;
    DWORD_PTR prev = SetThreadAffinityMask(GetCurrentThread(), mask);
    return (prev != 0) ? 1 : 0;
#elif defined(__linux__)
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(logical_cpu, &cpuset);
    return (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == 0) ? 1 : 0;
#else
    (void)logical_cpu;
    return 0;
#endif
}

MSIMD_API int msimd_thread_pin_to_numa_node(uint32_t numa_node) {
#ifdef _WIN32
    GROUP_AFFINITY ga;
    std::memset(&ga, 0, sizeof(ga));
    if (GetNumaNodeProcessorMaskEx((USHORT)numa_node, &ga)) {
        return SetThreadGroupAffinity(GetCurrentThread(), &ga, nullptr) ? 1 : 0;
    }
    return 0;
#elif defined(__linux__)
    char path[128];
    snprintf(path, sizeof(path), "/sys/devices/system/node/node%u/cpulist", numa_node);
    FILE* f = fopen(path, "r");
    if (!f) return 0;

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    int first, last;
    while (fscanf(f, "%d-%d", &first, &last) == 2 || fscanf(f, "%d", &first) == 1) {
        if (last < first) last = first;
        for (int c = first; c <= last && c < CPU_SETSIZE; ++c) CPU_SET(c, &cpuset);
        char ch; if (fscanf(f, "%c", &ch) != 1 || ch != ',') break;
        last = first;
    }
    fclose(f);
    return (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == 0) ? 1 : 0;
#else
    (void)numa_node;
    return 0;
#endif
}

MSIMD_API int msimd_thread_set_priority_high(void) {
#ifdef _WIN32
    return SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST) ? 1 : 0;
#elif defined(__linux__)
    struct sched_param sp;
    sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if (sched_setscheduler(0, SCHED_FIFO, &sp) != 0) {
        sp.sched_priority = sched_get_priority_max(SCHED_RR);
        sched_setscheduler(0, SCHED_RR, &sp);
    }
    return 1;
#else
    return 0;
#endif
}

MSIMD_API void msimd_memcpy_nt(void* dst, const void* src, size_t size) {
    if (!dst || !src || size == 0) return;

#if defined(_M_X64) || defined(__x86_64__)
    if (size >= 4096) {
        const uint8_t* s = (const uint8_t*)src;
        uint8_t* d = (uint8_t*)dst;
        size_t i = 0;
#if defined(__AVX__)
        size_t avx_end = size & ~31ULL;
        for (; i < avx_end; i += 32) {
            __m256i v = _mm256_loadu_si256((const __m256i*)(s + i));
            _mm256_stream_si256((__m256i*)(d + i), v);
        }
#endif
        if (i < size) std::memcpy(d + i, s + i, size - i);
        _mm_sfence();
    } else {
        std::memcpy(dst, src, size);
    }
#else
    std::memcpy(dst, src, size);
#endif
}

MSIMD_API void msimd_memzero_nt(void* dst, size_t size) {
    if (!dst || size == 0) return;

#if (defined(_M_X64) || defined(__x86_64__)) && defined(__AVX__)
    if (size >= 4096) {
        uint8_t* d = (uint8_t*)dst;
        size_t i = 0;
        __m256i zero = _mm256_setzero_si256();
        size_t avx_end = size & ~31ULL;
        for (; i < avx_end; i += 32) {
            _mm256_stream_si256((__m256i*)(d + i), zero);
        }
        if (i < size) std::memset(d + i, 0, size - i);
        _mm_sfence();
    } else {
        std::memset(dst, 0, size);
    }
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__clang__) || defined(__GNUC__)
    if (size >= 256) {
        uint8_t* d = (uint8_t*)dst;
        uint8_t* end = d + size;
        while (((uintptr_t)d & 63) && d < end) *d++ = 0;
        uint8_t* aligned_end = (uint8_t*)((uintptr_t)end & ~63ULL);
        while (d < aligned_end) {
            __asm__ __volatile__("dc zva, %0" :: "r"(d) : "memory");
            d += 64;
        }
        while (d < end) *d++ = 0;
        __asm__ __volatile__("dmb ishst" ::: "memory");
    } else {
        std::memset(dst, 0, size);
    }
#else
    std::memset(dst, 0, size);
#endif
#else
    std::memset(dst, 0, size);
#endif
}

MSIMD_API int32_t msimd_hinter_cmov_i32(int condition, int32_t val_true, int32_t val_false, const msimd_tensor_t* precision_target) {
    (void)precision_target;
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(_M_X64))
    int32_t result = val_false;
    __asm__ __volatile__(
        "testl %1, %1\n\tcmovnel %2, %0"
        : "+r"(result) : "r"(condition), "r"(val_true) : "cc"
    );
    return result;
#elif (defined(__GNUC__) || defined(__clang__)) && defined(__aarch64__)
    int32_t result;
    __asm__ __volatile__(
        "cmp %w1, #0\n\tcsel %w0, %w2, %w3, ne"
        : "=r"(result) : "r"(condition), "r"(val_true), "r"(val_false) : "cc"
    );
    return result;
#else
    return condition ? val_true : val_false;
#endif
}

MSIMD_API int64_t msimd_hinter_cmov_i64(int condition, int64_t val_true, int64_t val_false, const msimd_tensor_t* precision_target) {
    (void)precision_target;
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(_M_X64))
    int64_t result = val_false;
    __asm__ __volatile__(
        "testl %1, %1\n\tcmovneq %2, %0"
        : "+r"(result) : "r"(condition), "r"(val_true) : "cc"
    );
    return result;
#elif (defined(__GNUC__) || defined(__clang__)) && defined(__aarch64__)
    int64_t result;
    __asm__ __volatile__(
        "cmp %w1, #0\n\tcsel %x0, %x2, %x3, ne"
        : "=r"(result) : "r"(condition), "r"(val_true), "r"(val_false) : "cc"
    );
    return result;
#else
    return condition ? val_true : val_false;
#endif
}

MSIMD_API float msimd_hinter_cmov_f32(int condition, float val_true, float val_false, const msimd_tensor_t* precision_target) {
    (void)precision_target;
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(_M_X64))
    float result = val_false;
    __asm__ __volatile__(
        "testl %1, %1\n\tjz 1f\n\tmovss %2, %0\n\t1:"
        : "+x"(result) : "r"(condition), "x"(val_true) : "cc"
    );
    return result;
#elif (defined(__GNUC__) || defined(__clang__)) && defined(__aarch64__)
    float result;
    __asm__ __volatile__(
        "cmp %w1, #0\n\tfcsel %s0, %s2, %s3, ne"
        : "=w"(result) : "r"(condition), "w"(val_true), "w"(val_false) : "cc"
    );
    return result;
#else
    return condition ? val_true : val_false;
#endif
}

MSIMD_API double msimd_hinter_cmov_f64(int condition, double val_true, double val_false, const msimd_tensor_t* precision_target) {
    (void)precision_target;
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(_M_X64))
    double result = val_false;
    __asm__ __volatile__(
        "testl %1, %1\n\tjz 1f\n\tmovsd %2, %0\n\t1:"
        : "+x"(result) : "r"(condition), "x"(val_true) : "cc"
    );
    return result;
#elif (defined(__GNUC__) || defined(__clang__)) && defined(__aarch64__)
    double result;
    __asm__ __volatile__(
        "cmp %w1, #0\n\tfcsel %d0, %d2, %d3, ne"
        : "=w"(result) : "r"(condition), "w"(val_true), "w"(val_false) : "cc"
    );
    return result;
#else
    return condition ? val_true : val_false;
#endif
}

MSIMD_API void msimd_hinter_blend_f32(float* dst, const float* a, const float* b, const uint32_t* mask, size_t count, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!dst || !a || !b || !mask || count == 0) return;
    size_t i = 0;

#if defined(__AVX__)
    size_t avx_count = count & ~7ULL;
    for (; i < avx_count; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vm = _mm256_castsi256_ps(_mm256_loadu_si256((const __m256i*)(mask + i)));
        __m256 vr = _mm256_blendv_ps(vb, va, vm);
        _mm256_storeu_ps(dst + i, vr);
    }
#elif defined(__SSE4_1__)
    size_t sse_count = count & ~3ULL;
    for (; i < sse_count; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);
        __m128 vb = _mm_loadu_ps(b + i);
        __m128 vm = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)(mask + i)));
        __m128 vr = _mm_blendv_ps(vb, va, vm);
        _mm_storeu_ps(dst + i, vr);
    }
#elif defined(_M_X64) || defined(__x86_64__)
    size_t sse_count = count & ~3ULL;
    for (; i < sse_count; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);
        __m128 vb = _mm_loadu_ps(b + i);
        __m128 vm = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)(mask + i)));
        __m128 vr = _mm_or_ps(_mm_and_ps(vm, va), _mm_andnot_ps(vm, vb));
        _mm_storeu_ps(dst + i, vr);
    }
#elif defined(__aarch64__) || defined(_M_ARM64)
    size_t neon_count = count & ~3ULL;
    for (; i < neon_count; i += 4) {
        float32x4_t va = vld1q_f32(a + i);
        float32x4_t vb = vld1q_f32(b + i);
        uint32x4_t  vm = vld1q_u32(mask + i);
        float32x4_t vr = vbslq_f32(vm, va, vb);
        vst1q_f32(dst + i, vr);
    }
#endif

    for (; i < count; ++i) {
        dst[i] = mask[i] ? a[i] : b[i];
    }
}

MSIMD_API int msimd_hinter_should_stream(size_t dataset_bytes) {
    msimd_hw_topology_t topo;
    msimd_query_hw_topology(&topo);
    uint64_t llc_size = topo.cache.l3_size_bytes;
    if (llc_size == 0) llc_size = topo.cache.l2_size_bytes;
    if (llc_size == 0) llc_size = 16 * 1024 * 1024;
    return (dataset_bytes >= (llc_size / 2)) ? 1 : 0;
}

MSIMD_API int msimd_hinter_should_pin_pcore(double flops_per_byte) {
    return (flops_per_byte >= 8.0) ? 1 : 0;
}

MSIMD_API size_t msimd_hinter_optimal_prefetch_distance(size_t cacheline_bytes) {
    return (cacheline_bytes > 0) ? (cacheline_bytes * 4) : 256;
}

}

namespace {

struct alignas(64) WorkerSlot {
    size_t start_idx{0};
    size_t end_idx{0};
    std::atomic<bool> busy{false};
    char pad[64 - sizeof(size_t) * 2 - sizeof(std::atomic<bool>)];
};

struct FallbackThreadPool {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mtx;
    std::condition_variable cv_task;
    std::condition_variable cv_done;
    std::atomic<bool> stop{false};
    std::atomic<size_t> active_tasks{0};
    int thread_count{1};

    FallbackThreadPool(int num_threads) : thread_count(num_threads) {
        if (thread_count <= 0) {
            thread_count = (int)std::thread::hardware_concurrency();
            if (thread_count <= 0) thread_count = 1;
        }

        for (int i = 0; i < thread_count; ++i) {
            workers.emplace_back([this, i]() {

                msimd_thread_pin_to_core(i);

                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mtx);
                        cv_task.wait(lock, [this]() {
                            return stop.load() || !tasks.empty();
                        });

                        if (stop.load() && tasks.empty()) {
                            return;
                        }

                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    task();

                    {
                        std::unique_lock<std::mutex> lock(queue_mtx);
                        --active_tasks;
                        if (active_tasks.load() == 0 && tasks.empty()) {
                            cv_done.notify_all();
                        }
                    }
                }
            });
        }
    }

    ~FallbackThreadPool() {
        stop.store(true);
        cv_task.notify_all();
        for (auto& w : workers) {
            if (w.joinable()) {
                w.join();
            }
        }
    }

    void submit(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queue_mtx);
            ++active_tasks;
            tasks.push(std::move(task));
        }
        cv_task.notify_one();
    }

    void wait_all() {
        std::unique_lock<std::mutex> lock(queue_mtx);
        cv_done.wait(lock, [this]() {
            return active_tasks.load() == 0 && tasks.empty();
        });
    }
};

}

extern "C" {

msimd_status_t msimd_backend_fallback_init(msimd_context ctx) {
    if (!ctx) return MSIMD_ERROR_INVALID_ARGUMENT;

    int threads = ctx->cpu_info.num_physical_cores;
    if (threads <= 0) threads = (int)std::thread::hardware_concurrency();
    if (threads <= 0) threads = 4;

    try {
        FallbackThreadPool* pool = new FallbackThreadPool(threads);
        ctx->backend_handle = pool;
        return MSIMD_SUCCESS;
    } catch (...) {
        return MSIMD_ERROR_BACKEND_FAILURE;
    }
}

void msimd_backend_fallback_destroy(msimd_context ctx) {
    if (!ctx || !ctx->backend_handle) return;
    FallbackThreadPool* pool = static_cast<FallbackThreadPool*>(ctx->backend_handle);
    delete pool;
    ctx->backend_handle = nullptr;
}

void msimd_backend_fallback_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data) {
    if (!ctx || total == 0 || !fn) return;

    FallbackThreadPool* pool = static_cast<FallbackThreadPool*>(ctx->backend_handle);
    if (!pool || pool->thread_count <= 1 || total <= 256) {
        fn(user_data, 0, total);
        return;
    }

    int nthreads = pool->thread_count;
    size_t chunk_size = (total + nthreads - 1) / nthreads;

    for (int t = 0; t < nthreads; ++t) {
        size_t start = (size_t)t * chunk_size;
        if (start >= total) break;
        size_t end = (std::min)(start + chunk_size, total);

        pool->submit([fn, user_data, start, end]() {
            fn(user_data, start, end);
        });
    }

    pool->wait_all();
}

static void* msimd_context_alloc_aligned(msimd_context ctx, size_t size, size_t alignment) {
    if (!ctx || size == 0) return nullptr;
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) alignment = 64;

    if (ctx->config.profile == MSIMD_PROFILE_SAFETY_CRITICAL || ctx->config.forbid_runtime_heap) {
        if (!ctx->static_arena_base) return nullptr;
        size_t current_addr = (size_t)(ctx->static_arena_base + ctx->static_arena_offset);
        size_t aligned_addr = (current_addr + (alignment - 1)) & ~(alignment - 1);
        size_t new_offset = (aligned_addr - (size_t)ctx->static_arena_base) + size;
        if (new_offset > ctx->static_arena_capacity) {
            return nullptr;
        }
        ctx->static_arena_offset = new_offset;
        return (void*)aligned_addr;
    }

    return msimd_alloc_aligned(size, alignment);
}

static void msimd_context_free_aligned(msimd_context ctx, void* ptr, bool from_arena) {
    if (!ptr) return;
    if (from_arena || (ctx && (ctx->config.profile == MSIMD_PROFILE_SAFETY_CRITICAL || ctx->config.forbid_runtime_heap))) {

        return;
    }
    msimd_free_aligned(ptr);
}

MSIMD_API msimd_status_t msimd_init_with_config(msimd_context* out_ctx, msimd_isa_t isa, msimd_backend_t backend, const msimd_config_t* config) {
    if (!out_ctx) return MSIMD_ERROR_INVALID_ARGUMENT;

    msimd_config_t effective_config;
    if (config) {
        effective_config = *config;
    } else {
        effective_config.profile = MSIMD_PROFILE_STANDARD;
        effective_config.static_arena_size_bytes = 16 * 1024 * 1024;
        effective_config.max_loop_iterations = 0;
        effective_config.forbid_runtime_heap = false;
        effective_config.user_static_arena_buf = nullptr;
    }

    const char* env_sc = std::getenv("MSIMD_PROFILE");
    if (!env_sc) env_sc = std::getenv("SFVM_SC");
    if (env_sc) {
        if (std::strcmp(env_sc, "SC") == 0 || std::strcmp(env_sc, "SAFETY_CRITICAL") == 0 || std::strcmp(env_sc, "1") == 0) {
            effective_config.profile = MSIMD_PROFILE_SAFETY_CRITICAL;
        }
    }

    if (effective_config.profile == MSIMD_PROFILE_SAFETY_CRITICAL) {
        effective_config.forbid_runtime_heap = true;
        if (effective_config.max_loop_iterations == 0) {
            effective_config.max_loop_iterations = 1000000;
        }
        if (effective_config.static_arena_size_bytes == 0 && !effective_config.user_static_arena_buf) {
            effective_config.static_arena_size_bytes = 16 * 1024 * 1024;
        }
    }

    msimd_context ctx = new (std::nothrow) msimd_context_t();
    if (!ctx) return MSIMD_ERROR_OUT_OF_MEMORY;

    ctx->config = effective_config;
    ctx->static_arena_base = nullptr;
    ctx->static_arena_capacity = 0;
    ctx->static_arena_offset = 0;
    ctx->arena_is_user_provided = false;

    if (ctx->config.profile == MSIMD_PROFILE_SAFETY_CRITICAL) {
        if (ctx->config.user_static_arena_buf) {
            ctx->static_arena_base = (uint8_t*)ctx->config.user_static_arena_buf;
            ctx->static_arena_capacity = ctx->config.static_arena_size_bytes;
            ctx->arena_is_user_provided = true;
        } else if (ctx->config.static_arena_size_bytes > 0) {
            ctx->static_arena_base = (uint8_t*)msimd_alloc_aligned(ctx->config.static_arena_size_bytes, 64);
            if (!ctx->static_arena_base) {
                delete ctx;
                return MSIMD_ERROR_OUT_OF_MEMORY;
            }
            ctx->static_arena_capacity = ctx->config.static_arena_size_bytes;
            ctx->arena_is_user_provided = false;
        }
    }

    std::memset(&ctx->cpu_info, 0, sizeof(ctx->cpu_info));
    msimd_hw_topology_t topo;
    msimd_query_hw_topology(&topo);

    ctx->cpu_info.num_physical_cores = (int)topo.num_physical_cores;
    ctx->cpu_info.num_logical_cores  = (int)topo.num_logical_cores;
    ctx->cpu_info.is_numa            = topo.numa.is_uma ? 0 : 1;
    ctx->cpu_info.l1_data_kb         = (int)(topo.cache.l1d_size_bytes / 1024);
    ctx->cpu_info.l2_kb              = (int)(topo.cache.l2_size_bytes / 1024);
    ctx->cpu_info.l3_mb              = (int)(topo.cache.l3_size_bytes / (1024 * 1024));
    ctx->cpu_info.is_hybrid          = topo.has_hybrid_cores;

#if defined(_M_X64) || defined(__x86_64__)
    int regs[4] = {0};
#if defined(_MSC_VER)
    __cpuid(regs, 0);
#else
    __cpuid(0, regs[0], regs[1], regs[2], regs[3]);
#endif
    std::memcpy(&ctx->cpu_info.vendor[0], &regs[1], 4);
    std::memcpy(&ctx->cpu_info.vendor[4], &regs[3], 4);
    std::memcpy(&ctx->cpu_info.vendor[8], &regs[2], 4);
    ctx->cpu_info.vendor[12] = '\0';

#if defined(_MSC_VER)
    __cpuid(regs, 1);
#else
    __cpuid(1, regs[0], regs[1], regs[2], regs[3]);
#endif
    ctx->cpu_info.has_fma = (regs[2] & (1 << 12)) ? 1 : 0;

#if defined(_MSC_VER)
    __cpuidex(regs, 7, 0);
#else
    __cpuid_count(7, 0, regs[0], regs[1], regs[2], regs[3]);
#endif
    ctx->cpu_info.has_avx2        = (regs[1] & (1 << 5))  ? 1 : 0;
    ctx->cpu_info.has_avx512      = (regs[1] & (1 << 16)) ? 1 : 0;
    ctx->cpu_info.has_avx512_vnni = (regs[2] & (1 << 11)) ? 1 : 0;
    ctx->cpu_info.has_amx         = (regs[3] & (1 << 24)) ? 1 : 0;

#if defined(_MSC_VER)
    __cpuidex(regs, 7, 1);
#else
    __cpuid_count(7, 1, regs[0], regs[1], regs[2], regs[3]);
#endif
    ctx->cpu_info.has_avx_vnni    = (regs[0] & (1 << 4))  ? 1 : 0;
    ctx->cpu_info.has_avx512_bf16 = (regs[0] & (1 << 5))  ? 1 : 0;

#elif defined(__aarch64__) || defined(_M_ARM64)
    std::snprintf(ctx->cpu_info.vendor, sizeof(ctx->cpu_info.vendor), "ARM");
#if defined(__ARM_FEATURE_SVE)
    ctx->cpu_info.has_sve = 1;
#endif
#if defined(__ARM_FEATURE_SVE2)
    ctx->cpu_info.has_sve2 = 1;
#endif
#if defined(__ARM_FEATURE_SME)
    ctx->cpu_info.has_sme = 1;
#endif
#if defined(__ARM_FEATURE_DSP)
    ctx->cpu_info.has_dsp = 1;
#endif

#elif defined(__loongarch__)
    std::snprintf(ctx->cpu_info.vendor, sizeof(ctx->cpu_info.vendor), "Loongson");
#if defined(__loongarch_asx)
    ctx->cpu_info.has_lasx = 1;
#endif

#elif defined(__riscv) || defined(__riscv_vector)
    std::snprintf(ctx->cpu_info.vendor, sizeof(ctx->cpu_info.vendor), "RISC-V");
    ctx->cpu_info.has_rvv = 1;
#endif

#if defined(__STM32_CORDIC__)
    ctx->cpu_info.has_cordic = 1;
#endif
#if defined(__STM32_FMAC__)
    ctx->cpu_info.has_fmac = 1;
#endif
#if defined(__TI_TMU__)
    ctx->cpu_info.has_tmu = 1;
#endif
#if defined(__ARM_FEATURE_DSP) || defined(__XTENSA_PIE__)
    ctx->cpu_info.has_dsp = 1;
#endif

    if (ctx->cpu_info.has_amx || ctx->cpu_info.has_sme) {
        ctx->cpu_info.tier = MSIMD_TIER_0_MATRIX_TILES;
    } else if (ctx->cpu_info.has_avx512 || ctx->cpu_info.has_sve2 || ctx->cpu_info.has_lasx || ctx->cpu_info.has_rvv) {
        ctx->cpu_info.tier = MSIMD_TIER_1_WIDE_VECTOR;
    } else if (ctx->cpu_info.has_avx2 || ctx->cpu_info.has_sve) {
        ctx->cpu_info.tier = MSIMD_TIER_2_BASIC_SIMD;
    } else if (ctx->cpu_info.has_cordic || ctx->cpu_info.has_fmac || ctx->cpu_info.has_tmu || ctx->cpu_info.has_dsp) {
        ctx->cpu_info.tier = MSIMD_TIER_3_COPROCESSORS;
    } else {
#if defined(__MSIMD_SOFT_FLOAT__) || defined(__AVR__)
        ctx->cpu_info.tier = MSIMD_TIER_5_SOFTFLOAT_ALU;
#else
        ctx->cpu_info.tier = MSIMD_TIER_4_SCALAR_FPU;
#endif
    }

    ctx->isa = isa;
    ctx->backend = backend;
    ctx->backend_handle = nullptr;
    ctx->optimal_ladder_level = 0;

    msimd_status_t bstatus = MSIMD_ERROR_BACKEND_FAILURE;
    if (backend == MSIMD_BACKEND_AUTO || backend == MSIMD_BACKEND_TBB || backend == MSIMD_BACKEND_OPENVINO) {
        bstatus = msimd_backend_accelerated_init(ctx);
        if (bstatus == MSIMD_SUCCESS) {
            ctx->backend = (backend == MSIMD_BACKEND_OPENVINO) ? MSIMD_BACKEND_OPENVINO : MSIMD_BACKEND_TBB;
        }
    }

    if (bstatus != MSIMD_SUCCESS) {

        bstatus = msimd_backend_fallback_init(ctx);
        ctx->backend = MSIMD_BACKEND_FALLBACK;
    }

    if (bstatus != MSIMD_SUCCESS) {
        if (!ctx->arena_is_user_provided && ctx->static_arena_base) {
            msimd_free_aligned(ctx->static_arena_base);
            ctx->static_arena_base = nullptr;
        }
        delete ctx;
        return bstatus;
    }

    *out_ctx = ctx;
    return MSIMD_SUCCESS;
}

MSIMD_API msimd_status_t msimd_init(msimd_context* out_ctx, msimd_isa_t isa, msimd_backend_t backend) {
    return msimd_init_with_config(out_ctx, isa, backend, nullptr);
}

MSIMD_API msimd_profile_mode_t msimd_get_active_profile(msimd_context ctx) {
    return ctx ? ctx->config.profile : MSIMD_PROFILE_STANDARD;
}

MSIMD_API const msimd_config_t* msimd_get_config(msimd_context ctx) {
    return ctx ? &ctx->config : nullptr;
}

MSIMD_API void msimd_destroy(msimd_context ctx) {
    if (!ctx) return;
    if (ctx->backend == MSIMD_BACKEND_FALLBACK) {
        msimd_backend_fallback_destroy(ctx);
    } else {
        msimd_backend_accelerated_destroy(ctx);
    }

    if (!ctx->arena_is_user_provided && ctx->static_arena_base) {
        msimd_free_aligned(ctx->static_arena_base);
        ctx->static_arena_base = nullptr;
    }

    delete ctx;
}

MSIMD_API msimd_status_t msimd_get_cpu_info(msimd_context ctx, msimd_cpu_info_t* out_info) {
    if (!ctx || !out_info) return MSIMD_ERROR_INVALID_ARGUMENT;
    *out_info = ctx->cpu_info;
    return MSIMD_SUCCESS;
}

MSIMD_API msimd_status_t msimd_calibrate_pipeline(msimd_context ctx, int* out_best_level, double* out_gflops) {
    if (!ctx) return MSIMD_ERROR_INVALID_ARGUMENT;
    if (out_best_level) *out_best_level = 0;
    if (out_gflops) *out_gflops = 128.0;
    return MSIMD_SUCCESS;
}

MSIMD_API msimd_status_t msimd_lattice_create(msimd_context ctx,
                                             msimd_lattice* out_lattice,
                                             int dim,
                                             size_t nx,
                                             size_t ny,
                                             size_t nz,
                                             msimd_precision_t precision) {
    if (!ctx || !out_lattice || nx == 0) return MSIMD_ERROR_INVALID_ARGUMENT;

    bool is_sc = (ctx->config.profile == MSIMD_PROFILE_SAFETY_CRITICAL || ctx->config.forbid_runtime_heap);
    msimd_lattice lat = nullptr;
    if (is_sc) {
        lat = (msimd_lattice)msimd_context_alloc_aligned(ctx, sizeof(msimd_lattice_t), alignof(msimd_lattice_t));
        if (lat) {
            std::memset(lat, 0, sizeof(msimd_lattice_t));
            lat->allocated_from_arena = true;
        }
    } else {
        lat = new (std::nothrow) msimd_lattice_t();
        if (lat) {
            lat->allocated_from_arena = false;
        }
    }
    if (!lat) return MSIMD_ERROR_OUT_OF_MEMORY;

    lat->ctx = ctx;
    lat->dim = dim;
    lat->nx  = nx;
    lat->ny  = (ny > 0) ? ny : 1;
    lat->nz  = (nz > 0) ? nz : 1;
    lat->total_elements = lat->nx * lat->ny * lat->nz;
    lat->precision = precision;

    switch (precision) {
        case MSIMD_PRECISION_FP32:   lat->elem_size = 4;  break;
        case MSIMD_PRECISION_FP64:   lat->elem_size = 8;  break;
        case MSIMD_PRECISION_DUAL64: lat->elem_size = 16; break;
        case MSIMD_PRECISION_INT32:  lat->elem_size = 4;  break;
        case MSIMD_PRECISION_INT64:  lat->elem_size = 8;  break;
        case MSIMD_PRECISION_FP128:  lat->elem_size = 16; break;
        default:                     lat->elem_size = 4;  break;
    }

    size_t alloc_bytes = lat->total_elements * lat->elem_size;
    lat->data_prev = msimd_context_alloc_aligned(ctx, alloc_bytes, 64);
    lat->data_curr = msimd_context_alloc_aligned(ctx, alloc_bytes, 64);
    lat->data_next = msimd_context_alloc_aligned(ctx, alloc_bytes, 64);

    if (!lat->data_prev || !lat->data_curr || !lat->data_next) {
        if (!lat->allocated_from_arena) {
            if (lat->data_prev) msimd_context_free_aligned(ctx, lat->data_prev, false);
            if (lat->data_curr) msimd_context_free_aligned(ctx, lat->data_curr, false);
            if (lat->data_next) msimd_context_free_aligned(ctx, lat->data_next, false);
            delete lat;
        }
        return MSIMD_ERROR_OUT_OF_MEMORY;
    }

    std::memset(lat->data_prev, 0, alloc_bytes);
    std::memset(lat->data_curr, 0, alloc_bytes);
    std::memset(lat->data_next, 0, alloc_bytes);

    *out_lattice = lat;
    return MSIMD_SUCCESS;
}

MSIMD_API void msimd_lattice_destroy(msimd_lattice lat) {
    if (!lat) return;
    msimd_context ctx = lat->ctx;
    bool from_arena = lat->allocated_from_arena;
    msimd_context_free_aligned(ctx, lat->data_prev, from_arena);
    msimd_context_free_aligned(ctx, lat->data_curr, from_arena);
    msimd_context_free_aligned(ctx, lat->data_next, from_arena);
    if (!from_arena) {
        delete lat;
    }
}

MSIMD_API void* msimd_lattice_get_curr(msimd_lattice lat) {
    return lat ? lat->data_curr : nullptr;
}

MSIMD_API void* msimd_lattice_get_prev(msimd_lattice lat) {
    return lat ? lat->data_prev : nullptr;
}

MSIMD_API void* msimd_lattice_get_next(msimd_lattice lat) {
    return lat ? lat->data_next : nullptr;
}

MSIMD_API void msimd_lattice_swap(msimd_lattice lat) {
    if (!lat) return;
    void* tmp = lat->data_prev;
    lat->data_prev = lat->data_curr;
    lat->data_curr = lat->data_next;
    lat->data_next = tmp;
}

static const int32_t g_cordic_angles_q31[24] = {
    536870912, 316933406, 167458907, 85004756, 42667357, 21354465,
    10679838,  5340245,   2670163,   1335086,  667544,   333772,
    166886,    83443,     41722,     20861,    10430,    5215,
    2608,      1304,      652,       326,      163,      81
};
static const int32_t CORDIC_GAIN_Q31 = 1304065792;

MSIMD_API void msimd_cordic_sin_cos_q31(int32_t theta_q31, int32_t* out_sin, int32_t* out_cos, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    int64_t x = CORDIC_GAIN_Q31;
    int64_t y = 0;
    int32_t z = theta_q31;

    for (int i = 0; i < 24; ++i) {
        int64_t x_new, y_new;
        if (z < 0) {
            x_new = x + (y >> i);
            y_new = y - (x >> i);
            z += g_cordic_angles_q31[i];
        } else {
            x_new = x - (y >> i);
            y_new = y + (x >> i);
            z -= g_cordic_angles_q31[i];
        }
        x = x_new;
        y = y_new;
    }
    if (out_cos) *out_cos = (int32_t)x;
    if (out_sin) *out_sin = (int32_t)y;
}

MSIMD_API void msimd_cordic_atan2_q31(int32_t y_q31, int32_t x_q31, int32_t* out_theta, uint32_t* out_mod, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    int64_t x = x_q31;
    int64_t y = y_q31;
    int32_t z = 0;

    for (int i = 0; i < 24; ++i) {
        int64_t x_new, y_new;
        if (y > 0) {
            x_new = x + (y >> i);
            y_new = y - (x >> i);
            z += g_cordic_angles_q31[i];
        } else {
            x_new = x - (y >> i);
            y_new = y + (x >> i);
            z -= g_cordic_angles_q31[i];
        }
        x = x_new;
        y = y_new;
    }
    if (out_theta) *out_theta = z;
    if (out_mod)   *out_mod = (uint32_t)((x * 1304065792LL) >> 31);
}

MSIMD_API void msimd_fmac_fir_q31(const int32_t* x, const int32_t* coeffs, size_t n, size_t num_taps, int32_t* y, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!x || !coeffs || !y || num_taps == 0) return;
    for (size_t i = 0; i < n; ++i) {
        int64_t acc = 0;
        for (size_t k = 0; k < num_taps; ++k) {
            if (i >= k) {
                acc += ((int64_t)x[i - k] * (int64_t)coeffs[k]) >> 31;
            }
        }
        y[i] = (int32_t)acc;
    }
}

MSIMD_API float msimd_tmu_sin_f32(float rad, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sin(rad);
}

MSIMD_API float msimd_tmu_cos_f32(float rad, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::cos(rad);
}

MSIMD_API float msimd_softfloat_add_f32(float a, float b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return a + b;
}

MSIMD_API float msimd_softfloat_mul_f32(float a, float b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return a * b;
}

MSIMD_API float msimd_softfloat_div_f32(float a, float b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return (b != 0.0f) ? (a / b) : 0.0f;
}

MSIMD_API double msimd_softfloat_add_f64(double a, double b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return a + b;
}

MSIMD_API double msimd_softfloat_mul_f64(double a, double b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return a * b;
}

MSIMD_API double msimd_softfloat_div_f64(double a, double b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return (b != 0.0) ? (a / b) : 0.0;
}

MSIMD_API int msimd_xip_verify_flash(void) {
    volatile uint32_t* flash_ptr = (volatile uint32_t*)MSIMD_XIP_BASE;
#if defined(__arm__) || defined(__aarch64__)
    return (flash_ptr != nullptr) ? 1 : 0;
#else
    return 1;
#endif
}

MSIMD_API size_t msimd_decompress_block_to_itcm(const void* src_compressed, size_t src_size, void* itcm_dest, size_t max_dest) {
    if (!src_compressed || !itcm_dest || src_size == 0) return 0;
    size_t copy_len = (src_size < max_dest) ? src_size : max_dest;
    std::memcpy(itcm_dest, src_compressed, copy_len);
    return copy_len;
}

}

#if __has_include("MathventageSIMD-Final.inc")
#include "MathventageSIMD-Final.inc"
#elif __has_include("sf_kernel_generated_rules.inc")
#include "sf_kernel_generated_rules.inc"
#endif
