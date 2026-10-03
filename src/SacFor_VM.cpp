#include "MathventageSIMD.h"
#include "SacculusForge.h"

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <new>
#include <functional>
#include <vector>
#include <string>

#if defined(_M_X64) || defined(__x86_64__) || defined(_M_IX86) || defined(__i386__)
  #include <immintrin.h>
  #include <xmmintrin.h>
  #include <emmintrin.h>
  #include <pmmintrin.h>
  #include <tmmintrin.h>
  #include <smmintrin.h>
  #include <nmmintrin.h>
  #include <wmmintrin.h>
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(__ARM_NEON)
  #include <arm_neon.h>
  #if defined(__ARM_FEATURE_SVE)
    #include <arm_sve.h>
  #endif
#elif defined(__riscv) && defined(__riscv_vector)
  #include <riscv_vector.h>
#elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__)
  #include <altivec.h>
#elif defined(__s390x__) || defined(__zarch__)
  #include <vecintrin.h>
#endif

#if defined(__clang__)
    #define SF_DISABLE_OPT_BEGIN _Pragma("clang optimize off")
    #define SF_DISABLE_OPT_END   _Pragma("clang optimize on")
#elif defined(__GNUC__) || defined(__GNUG__)
    #define SF_DISABLE_OPT_BEGIN _Pragma("GCC push_options") _Pragma("GCC optimize(\"O0\")")
    #define SF_DISABLE_OPT_END   _Pragma("GCC pop_options")
#elif defined(_MSC_VER)
    #define SF_DISABLE_OPT_BEGIN __pragma(optimize("", off))
    #define SF_DISABLE_OPT_END   __pragma(optimize("", on))
#else
    #define SF_DISABLE_OPT_BEGIN
    #define SF_DISABLE_OPT_END
#endif

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <sys/mman.h>
#  include <unistd.h>
#  include <time.h>
#  include <fcntl.h>
#  if defined(__linux__) && (defined(__aarch64__) || defined(__arm__))
#    include <sys/auxv.h>
#    include <asm/hwcap.h>
#  endif
#  if defined(__APPLE__)
#    include <sys/types.h>
#    include <sys/sysctl.h>
#  endif
#endif

#if defined(_M_X64) || defined(__x86_64__)
#  include <immintrin.h>
#  if defined(_MSC_VER) || defined(__clang__)
#    include <intrin.h>
#  else
#    include <cpuid.h>
#  endif
#elif defined(__aarch64__) || defined(_M_ARM64)
#  include <arm_neon.h>
#elif defined(__arm__) || defined(_M_ARM)
#  include <arm_neon.h>
#endif

static inline void sf_get_cpuid(int info[4], int leaf, int subleaf = 0) {
#if defined(_MSC_VER) || (defined(__clang__) && defined(_WIN32))
    __cpuidex(info, leaf, subleaf);
#elif defined(__GNUC__) || defined(__clang__)
    __cpuid_count(leaf, subleaf, info[0], info[1], info[2], info[3]);
#else
    info[0] = info[1] = info[2] = info[3] = 0;
#endif
}

#if defined(__APPLE__) && (defined(__aarch64__) || defined(__arm64__))
#  define SACFOR_CACHE_LINE       128
#else
#  define SACFOR_CACHE_LINE       64
#endif

#define SACFOR_ALIGN_CL         alignas(SACFOR_CACHE_LINE)

#ifndef SACFOR_MAX_CORES
#  define SACFOR_MAX_CORES        256u
#endif

#ifndef SACFOR_CORE_WORDS
#  define SACFOR_CORE_WORDS       4u
#endif

#define SACFOR_MAX_INSTANCES    65535u

#define SACFOR_HOTLOG_SIZE      (1u << 20)

#define SACFOR_COLDLOG_SIZE     (1u << 14)

#define SACFOR_ROOM_COUNT       3

#define SACFOR_REGION_COUNT     4

#define SACFOR_GEN_YOUNG_MAX    3

#define SACFOR_GEN_ADOL_MAX     10

#define SACFOR_OVERHEAD_DEMAND  1000ULL

#define SACFOR_OVERHEAD_MIDDLE  50000ULL

#define SACFOR_HEAT_HOT         0.75f

#define SACFOR_HEAT_COLD        0.10f

#define SACFOR_HEAT_DECAY       0.97f

#define SACFOR_HEAT_GAIN        1.0f

#define SACFOR_IDLE_YOUNG_NS    (2ULL  * 1000000000ULL)

#define SACFOR_IDLE_ADOL_NS     (10ULL * 1000000000ULL)

#define SACFOR_IDLE_SENIOR_NS   (60ULL * 1000000000ULL)

#define SACFOR_GC_INTERVAL_NS   1000000ULL

#define SACFOR_HOTLOG_REFRESH_NS 1000000ULL

#define SACFOR_LANDFILL_CRIT    0.85f

#define SACFOR_LANDFILL_INIT    (32u * 1024u * 1024u)

#define SACFOR_LANDFILL_SCALE   2u

#define SF_PRESSURE_MILD          0.60f

#define SF_PRESSURE_HIGH          0.80f

#define SF_PRESSURE_CRITICAL      0.92f

#define SF_FALLINE_COMPACT_RATIO  0.30f

#define SF_REHAB_SCAN_INTERVAL    50u

#define SF_HOTFIELD_COMPACT_SWEEPS 20u

#define SF_MATURED_FLUSH_RATIO    SACFOR_LANDFILL_CRIT

#define SF_REHAB_WARN_RATIO       0.70f

static inline uint64_t sacculus_now_ns(void) {
#ifdef _WIN32
    LARGE_INTEGER f, t;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t);
    return (uint64_t)(t.QuadPart * 1000000000LL / f.QuadPart);
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
#endif
}

static inline uint64_t sacculus_rdtsc(void) {
#if defined(_M_X64) || defined(__x86_64__)
#  if defined(_MSC_VER) || defined(__clang__)
    return __rdtsc();
#  else
    uint32_t lo, hi;
    __asm__ __volatile__("lfence\nrdtsc" : "=a"(lo), "=d"(hi) :: "memory");
    return ((uint64_t)hi << 32) | lo;
#  endif
#elif defined(__aarch64__) || defined(_M_ARM64)
#  if defined(_MSC_VER)
    return (uint64_t)_ReadStatusReg(ARM64_CNTVCT);
#  else
    uint64_t t;
    __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(t) :: "memory");
    return t;
#  endif
#else
    return sacculus_now_ns();
#endif
}

static inline void sf_cpu_pause(void) {
#if defined(_M_X64) || defined(__x86_64__)
    _mm_pause();
#elif defined(__aarch64__) || defined(_M_ARM64)
#  if defined(_MSC_VER)
    __yield();
#  else
    __asm__ __volatile__("yield" ::: "memory");
#  endif
#elif defined(__arm__) || defined(_M_ARM)
#  if defined(_MSC_VER)
    __yield();
#  else
    __asm__ __volatile__("yield" ::: "memory");
#  endif
#else
    std::this_thread::yield();
#endif
}

static inline void sf_sync_store_fence(void) {
#if defined(_M_X64) || defined(__x86_64__)
    _mm_sfence();
#elif defined(__aarch64__)
    __asm__ __volatile__("dmb ishst" ::: "memory");
#else
    std::atomic_thread_fence(std::memory_order_release);
#endif
}

static inline void sf_sync_load_fence(void) {
#if defined(_M_X64) || defined(__x86_64__)
    _mm_lfence();
#elif defined(__aarch64__)
    __asm__ __volatile__("dmb ishld" ::: "memory");
#else
    std::atomic_thread_fence(std::memory_order_acquire);
#endif
}

static inline void sf_sync_full_fence(void) {
#if defined(_M_X64) || defined(__x86_64__)
    _mm_mfence();
#elif defined(__aarch64__)
    __asm__ __volatile__("dmb ish" ::: "memory");
#else
    std::atomic_thread_fence(std::memory_order_seq_cst);
#endif
}

static inline void sf_sync_flush_icache(void* addr, size_t bytes) {
#ifdef _WIN32
    FlushInstructionCache(GetCurrentProcess(), addr, bytes);
#elif defined(__clang__) || defined(__GNUC__)
    __builtin___clear_cache((char*)addr, (char*)addr + bytes);
#endif
}

static inline void sf_pretouch_memory(void* base, size_t size, size_t page_stride = 4096) {
    if (!base || size == 0) return;
    volatile uint8_t* p = static_cast<volatile uint8_t*>(base);
    for (size_t off = 0; off < size; off += page_stride) {
        p[off] = 0;
    }
}

struct SACFOR_ALIGN_CL sf_ticket_lock_t {
    alignas(64) std::atomic<uint32_t> next_ticket{0};
    alignas(64) std::atomic<uint32_t> now_serving{0};

    void lock(void) {
        uint32_t my_ticket = next_ticket.fetch_add(1, std::memory_order_relaxed);
        while (now_serving.load(std::memory_order_acquire) != my_ticket) {
            sf_cpu_pause();
        }
    }

    void unlock(void) {
        now_serving.fetch_add(1, std::memory_order_release);
    }
};

struct sf_ticket_guard_t {
    sf_ticket_lock_t& l;
    explicit sf_ticket_guard_t(sf_ticket_lock_t& lock_ref) : l(lock_ref) { l.lock(); }
    ~sf_ticket_guard_t() { l.unlock(); }
};

enum class sf_critical_simd_e : uint8_t {
    AUTO  = 0,
    ALLOW = 1,
    DENY  = 2
};

static std::atomic<uint8_t> g_sf_critical_simd_mode{static_cast<uint8_t>(sf_critical_simd_e::AUTO)};

SF_DISABLE_OPT_BEGIN
static inline void sf_critical_clear_memory(void* dst, size_t bytes) {
    if (!dst || bytes == 0) return;
    uint8_t m = g_sf_critical_simd_mode.load(std::memory_order_relaxed);
    if (m == static_cast<uint8_t>(sf_critical_simd_e::DENY)) {

        memset(dst, 0, bytes);
        return;
    }
#if defined(_M_X64) || defined(__x86_64__)

    uint8_t* p = static_cast<uint8_t*>(dst);
    size_t rem = bytes;
    __m128i z = _mm_setzero_si128();
    while (rem >= 16) {
        _mm_storeu_si128(reinterpret_cast<__m128i*>(p), z);
        p += 16;
        rem -= 16;
    }
    while (rem > 0) {
        *p++ = 0;
        --rem;
    }
#elif defined(__aarch64__)
    uint8_t* p = static_cast<uint8_t*>(dst);
    size_t rem = bytes;
    uint8x16_t z = vdupq_n_u8(0);
    while (rem >= 16) {
        vst1q_u8(p, z);
        p += 16;
        rem -= 16;
    }
    while (rem > 0) {
        *p++ = 0;
        --rem;
    }
#else
    memset(dst, 0, bytes);
#endif
}
SF_DISABLE_OPT_END

static inline void* sf_os_reserve_memory(size_t bytes) {
#ifdef _WIN32
    return VirtualAlloc(nullptr, bytes, MEM_RESERVE, PAGE_READWRITE);
#else
    void* p = mmap(nullptr, bytes, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (p == MAP_FAILED) ? nullptr : p;
#endif
}

static inline bool sf_os_commit_memory(void* addr, size_t bytes) {
    if (!addr || bytes == 0) return false;
#ifdef _WIN32
    return VirtualAlloc(addr, bytes, MEM_COMMIT, PAGE_READWRITE) != nullptr;
#else
    return mprotect(addr, bytes, PROT_READ | PROT_WRITE) == 0;
#endif
}

static inline void sf_os_decommit_memory(void* addr, size_t bytes) {
    if (!addr || bytes == 0) return;
#ifdef _WIN32
    VirtualFree(addr, bytes, MEM_DECOMMIT);
#elif defined(__linux__)
    madvise(addr, bytes, MADV_DONTNEED);
#else
    madvise(addr, bytes, MADV_FREE);
#endif
}

static inline void sf_os_release_memory(void* addr, size_t bytes = 0) {
    if (!addr) return;
#ifdef _WIN32
    (void)bytes;
    VirtualFree(addr, 0, MEM_RELEASE);
#else
    munmap(addr, bytes);
#endif
}

struct sf_anonymous_pagefile_t {
    void*  mapped_base     = nullptr;
    size_t mapped_capacity = 0;
#ifdef _WIN32
    HANDLE hMap = NULL;
#endif

    bool init(size_t capacity_bytes) {
        mapped_capacity = (capacity_bytes + 4095ULL) & ~4095ULL;
#ifdef _WIN32
        hMap = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                                  (DWORD)((uint64_t)mapped_capacity >> 32),
                                  (DWORD)(mapped_capacity & 0xFFFFFFFF), NULL);
        if (!hMap) return false;
        mapped_base = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, mapped_capacity);
        return mapped_base != nullptr;
#else
        mapped_base = mmap(nullptr, mapped_capacity, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        return (mapped_base != MAP_FAILED) && (mapped_base != nullptr);
#endif
    }

    void destroy() {
        if (!mapped_base) return;
#ifdef _WIN32
        UnmapViewOfFile(mapped_base);
        if (hMap) CloseHandle(hMap);
        hMap = NULL;
#else
        munmap(mapped_base, mapped_capacity);
#endif
        mapped_base = nullptr;
        mapped_capacity = 0;
    }
};

#define SF_HEAP_MAGIC 0x53464850U

struct SACFOR_ALIGN_CL sf_heap_superblock_t {
    uint32_t magic;
    uint32_t version;
    size_t   total_reserved_bytes;
    size_t   total_committed_bytes;
    size_t   page_size;
    size_t   alignment;

    size_t   slabs_offset;
    size_t   slabs_capacity;
    size_t   bump_arena_offset;
    size_t   bump_arena_capacity;
    size_t   los_offset;
    size_t   los_capacity;

    size_t   puzzle_arena_offset;
    size_t   puzzle_arena_capacity;

    std::atomic<size_t> allocated_bytes{0};
    std::atomic<size_t> rebated_bytes{0};
    std::atomic<uint32_t> rebate_cycle_count{0};

    void*    next_chained_block{nullptr};
    uint8_t  _pad[4];
};

static inline void sacculus_core_bitmap_set(std::atomic<uint64_t> bm[SACFOR_CORE_WORDS],
                                             uint16_t core) {
    if (core >= SACFOR_MAX_CORES) return;
    bm[core / 64].fetch_or(1ULL << (core % 64), std::memory_order_relaxed);
}

static inline void sacculus_core_bitmap_set(uint64_t bm[SACFOR_CORE_WORDS],
                                             uint16_t core) {
    if (core >= SACFOR_MAX_CORES) return;
    bm[core / 64] |= (1ULL << (core % 64));
}

static inline int sacculus_core_bitmap_test(const std::atomic<uint64_t> bm[SACFOR_CORE_WORDS],
                                             uint16_t core) {
    if (core >= SACFOR_MAX_CORES) return 0;
    return (int)((bm[core / 64].load(std::memory_order_relaxed) >> (core % 64)) & 1u);
}

static inline int sacculus_core_bitmap_test(const uint64_t bm[SACFOR_CORE_WORDS],
                                             uint16_t core) {
    if (core >= SACFOR_MAX_CORES) return 0;
    return (int)((bm[core / 64] >> (core % 64)) & 1u);
}

static inline uint32_t sacculus_core_bitmap_count(
        const std::atomic<uint64_t> bm[SACFOR_CORE_WORDS]) {
    uint32_t n = 0;
    for (uint32_t i = 0; i < SACFOR_CORE_WORDS; ++i) {
        uint64_t w = bm[i].load(std::memory_order_relaxed);
#ifdef __POPCNT__
        n += (uint32_t)__builtin_popcountll(w);
#else
        while (w) { n++; w &= w - 1; }
#endif
    }
    return n;
}

static inline uint32_t sacculus_core_bitmap_count(
        const uint64_t bm[SACFOR_CORE_WORDS]) {
    uint32_t n = 0;
    for (uint32_t i = 0; i < SACFOR_CORE_WORDS; ++i) {
#ifdef __POPCNT__
        n += (uint32_t)__builtin_popcountll(bm[i]);
#else
        uint64_t x = bm[i];
        while (x) { n++; x &= x - 1; }
#endif
    }
    return n;
}

enum class sf_gc_mode_e : uint8_t {
    CYCLES = 0,
    SIMD   = 1
};

enum class sf_snatch_rebate_mode_e : uint8_t {
    AUTO  = 0,
    RAPID = 1,
    LAZY  = 2,
    GYC   = 3
};

enum class sf_gc_heuristic_e : uint8_t {
    ADAPTIVE     = 0,
    AGGRESSIVE   = 1,
    BUREAUCRATIC = 2,
    DISABLED     = 3
};

enum class sf_scgc_type_e : uint8_t {
    DEFAULT      = 0,
    PRIMITIVE_RW = 1
};

struct sf_vm_string_args_t {
    char                    gc_mode_raw[32]           = "CYCLES";
    char                    critical_simd_raw[32]     = "Auto";
    char                    snatch_rebate_raw[32]     = "Auto";
    char                    freelist_policy_raw[32]   = "LIFO";
    char                    gc_heuristic_raw[32]      = "Adaptive";
    sf_gc_mode_e            gc_mode                   = sf_gc_mode_e::CYCLES;
    sf_critical_simd_e      critical_simd_mode        = sf_critical_simd_e::AUTO;
    sf_snatch_rebate_mode_e snatch_rebate_mode        = sf_snatch_rebate_mode_e::AUTO;
    sf_gc_heuristic_e       gc_heuristic              = sf_gc_heuristic_e::ADAPTIVE;
    sf_scgc_type_e          scgc_type                 = sf_scgc_type_e::DEFAULT;
    bool                    critical_simd_explicit    = false;
    bool                    enable_huge_pages         = false;
    bool                    always_pretouch           = false;
    bool                    always_focus_compactizing = false;
    bool                    no_sfgc_overwrite_mode    = false;
};

struct sf_vm_integer_args_t {
    size_t   runway_handsel_bytes     = 1024ULL * 1024 * 1024;
    size_t   landfill_handsel_bytes   = 285ULL * 1024 * 1024;
    size_t   slab_chunk_size_bytes    = 2ULL * 1024 * 1024;
    size_t   pressure_threshold_bytes = 256ULL * 1024 * 1024;
    size_t   code_arena_bytes         = 64ULL * 1024 * 1024;
    uint64_t wakes_gc_interval_ns     = 1000000ULL;
    uint64_t line_compaction_freq_ns  = 350ULL * 1000000ULL;
    uint32_t in_heap_forwarding_rate  = 7;
    uint32_t freelist_recycle_policy  = 1;
};

struct sf_vm_float_args_t {
    float    pressure_mild_ratio      = 0.60f;
    float    pressure_high_ratio      = 0.80f;
    float    pressure_critical_ratio  = 0.92f;
    float    landfill_critical_ratio  = 0.85f;
    float    falline_compact_ratio    = 0.30f;
    float    heat_decay_factor        = 0.97f;
    float    heat_gain_weight         = 1.00f;
};

struct sf_vm_config_t {
    sf_vm_string_args_t  strings;
    sf_vm_integer_args_t integers;
    sf_vm_float_args_t   floats;
    msimd_profile_mode_t profile                      = MSIMD_PROFILE_STANDARD;
    bool                 is_safety_critical           = false;
    bool                 forbid_runtime_heap          = false;
    bool                 disable_jit_deopt            = false;
    uint32_t             max_loop_iterations          = 0;
    bool                 has_flag_conflict            = false;
    char                 conflict_msg[192]            = {0};
};

static uint64_t parse_size_with_unit(const char* str) {
    if (!str || !*str) return 0;
    char* end = nullptr;
    double val = std::strtod(str, &end);
    if (val < 0) val = 0;
    if (end) {
        while (*end == ' ') end++;
        if (_stricmp(end, "gb") == 0 || _stricmp(end, "g") == 0 ||
            _stricmp(end, "tb") == 0 || _stricmp(end, "t") == 0) {
            return (uint64_t)(val * 1024.0 * 1024.0 * 1024.0);
        }
        if (_stricmp(end, "mb") == 0 || _stricmp(end, "m") == 0) {
            return (uint64_t)(val * 1024.0 * 1024.0);
        }
        if (_stricmp(end, "kb") == 0 || _stricmp(end, "k") == 0) {
            return (uint64_t)(val * 1024.0);
        }
        if (_stricmp(end, "b") == 0) {
            return (uint64_t)val;
        }
    }
    return (uint64_t)val;
}

static uint64_t parse_time_with_unit_to_ns(const char* str) {
    if (!str || !*str) return 0;
    char* end = nullptr;
    double val = std::strtod(str, &end);
    if (val < 0) val = 0;
    if (end) {
        while (*end == ' ') end++;
        if (_stricmp(end, "s") == 0 || _stricmp(end, "sec") == 0 ||
            _stricmp(end, "m") == 0 || _stricmp(end, "min") == 0) {
            return (uint64_t)(val * 1000000000.0);
        }
        if (_stricmp(end, "ms") == 0) {
            return (uint64_t)(val * 1000000.0);
        }
        if (_stricmp(end, "us") == 0) {
            return (uint64_t)(val * 1000.0);
        }
        if (_stricmp(end, "ns") == 0) {
            return (uint64_t)val;
        }
    }
    return (uint64_t)(val * 1000000.0);
}

static inline void sf_upclocker_set_boost(int enable);

static void parse_single_sf_flag(sf_vm_config_t& cfg, const char* flag_str) {

    if (!flag_str) return;
    const char* prefix = "-#SF|";
    const char* pos = strstr(flag_str, prefix);
    if (!pos) return;
    pos += strlen(prefix);

    const char* colon = strchr(pos, ':');
    if (!colon) return;

    char key[64] = {0};
    char val[64] = {0};

    size_t klen = (size_t)(colon - pos);
    if (klen >= sizeof(key)) klen = sizeof(key) - 1;
    strncpy(key, pos, klen);
    key[klen] = '\0';

    const char* val_start = colon + 1;
    size_t vlen = 0;
    while (val_start[vlen] && val_start[vlen] != ' ' && val_start[vlen] != ',' && val_start[vlen] != ';') {
        vlen++;
    }
    if (vlen >= sizeof(val)) vlen = sizeof(val) - 1;
    strncpy(val, val_start, vlen);
    val[vlen] = '\0';

    if (_stricmp(key, "GC_Mode") == 0) {
        strncpy(cfg.strings.gc_mode_raw, val, sizeof(cfg.strings.gc_mode_raw) - 1);
        if (_stricmp(val, "SIMD") == 0) cfg.strings.gc_mode = sf_gc_mode_e::SIMD;
        else cfg.strings.gc_mode = sf_gc_mode_e::CYCLES;
    } else if (_stricmp(key, "Use_Critical_SIMDly_Operations") == 0 ||
               _stricmp(key, "Use_Critical_SIMD_Operations") == 0 ||
               _stricmp(key, "Critical_SIMDly_Operations") == 0 ||
               _stricmp(key, "Critical_SIMD_Operations") == 0) {
        cfg.strings.critical_simd_explicit = true;
        strncpy(cfg.strings.critical_simd_raw, val, sizeof(cfg.strings.critical_simd_raw) - 1);
        if (_stricmp(val, "Allow") == 0 || _stricmp(val, "Allowed") == 0 || _stricmp(val, "1") == 0 || _stricmp(val, "True") == 0) {
            cfg.strings.critical_simd_mode = sf_critical_simd_e::ALLOW;
        } else if (_stricmp(val, "Deny") == 0 || _stricmp(val, "Denied") == 0 || _stricmp(val, "0") == 0 || _stricmp(val, "False") == 0) {
            cfg.strings.critical_simd_mode = sf_critical_simd_e::DENY;
        } else {
            cfg.strings.critical_simd_mode = sf_critical_simd_e::AUTO;
        }
        g_sf_critical_simd_mode.store(static_cast<uint8_t>(cfg.strings.critical_simd_mode), std::memory_order_relaxed);
    } else if (_stricmp(key, "Snatch_and_Rebate_Mode") == 0 || _stricmp(key, "Snatch_And_Rebate_Mode") == 0) {
        strncpy(cfg.strings.snatch_rebate_raw, val, sizeof(cfg.strings.snatch_rebate_raw) - 1);
        if (_stricmp(val, "Rapid") == 0) {
            cfg.strings.snatch_rebate_mode = sf_snatch_rebate_mode_e::RAPID;
        } else if (_stricmp(val, "Lazy") == 0) {
            cfg.strings.snatch_rebate_mode = sf_snatch_rebate_mode_e::LAZY;
        } else if (_stricmp(val, "GYC") == 0 || strstr(val, "generous") != nullptr || strstr(val, "cautious") != nullptr) {
            cfg.strings.snatch_rebate_mode = sf_snatch_rebate_mode_e::GYC;
        } else {
            cfg.strings.snatch_rebate_mode = sf_snatch_rebate_mode_e::AUTO;
        }
    } else if (_stricmp(key, "GC_Heuristic") == 0 || _stricmp(key, "GC_Heuristics") == 0 || _stricmp(key, "Heuristic") == 0) {
        strncpy(cfg.strings.gc_heuristic_raw, val, sizeof(cfg.strings.gc_heuristic_raw) - 1);
        if (_stricmp(val, "Aggressive") == 0 || _stricmp(val, "Agresif") == 0) {
            cfg.strings.gc_heuristic = sf_gc_heuristic_e::AGGRESSIVE;
        } else if (_stricmp(val, "Bureaucratic") == 0 || _stricmp(val, "Burokratik") == 0 || _stricmp(val, "BÃ¼rokratik") == 0) {
            cfg.strings.gc_heuristic = sf_gc_heuristic_e::BUREAUCRATIC;
        } else if (_stricmp(val, "Disabled") == 0 || _stricmp(val, "None") == 0 || _stricmp(val, "Zero_GC") == 0 ||
                   _stricmp(val, "No_sfGC") == 0 || _stricmp(val, "No_GC") == 0 || _stricmp(val, "Overwrite") == 0) {
            cfg.strings.gc_heuristic = sf_gc_heuristic_e::DISABLED;
            cfg.strings.no_sfgc_overwrite_mode = true;
        } else {
            cfg.strings.gc_heuristic = sf_gc_heuristic_e::ADAPTIVE;
        }
    } else if (_stricmp(key, "FreeList_Recycle_Policy") == 0) {
        strncpy(cfg.strings.freelist_policy_raw, val, sizeof(cfg.strings.freelist_policy_raw) - 1);
        cfg.integers.freelist_recycle_policy = (uint32_t)std::strtoul(val, nullptr, 10);
    } else if (_stricmp(key, "Enable_Huge_Pages") == 0) {
        cfg.strings.enable_huge_pages = (_stricmp(val, "1") == 0 || _stricmp(val, "true") == 0);
    } else if (_stricmp(key, "AlwaysPreTouch") == 0 ||
               _stricmp(key, "Always_Pre_Touch") == 0 ||
               _stricmp(key, "Always_Pretouch") == 0 ||
               _stricmp(key, "Alwayfpretouch") == 0 ||
               _stricmp(key, "Alwayf_Pretouch") == 0) {
        cfg.strings.always_pretouch = (_stricmp(val, "1") == 0 ||
                                       _stricmp(val, "true") == 0 ||
                                       _stricmp(val, "yes") == 0 ||
                                       _stricmp(val, "enable") == 0 ||
                                       _stricmp(val, "enabled") == 0);
    } else if (_stricmp(key, "Upclocker") == 0 || _stricmp(key, "Upclock") == 0) {
        if (_stricmp(val, "Allow") == 0 || _stricmp(val, "Max") == 0 || _stricmp(val, "Boost") == 0 || _stricmp(val, "1") == 0 || _stricmp(val, "True") == 0) {
            sf_upclocker_set_boost(1);
        } else {
            sf_upclocker_set_boost(0);
        }
    } else if (_stricmp(key, "Always_Focus_Compactizing") == 0 ||
               _stricmp(key, "AlwaysFocusCompactizing") == 0 ||
               _stricmp(key, "Always_Compactize") == 0 ||
               _stricmp(key, "Always_Compact") == 0) {
        cfg.strings.always_focus_compactizing = (_stricmp(val, "1") == 0 ||
                                                 _stricmp(val, "true") == 0 ||
                                                 _stricmp(val, "yes") == 0 ||
                                                 _stricmp(val, "enable") == 0 ||
                                                 _stricmp(val, "enabled") == 0);
    } else if (_stricmp(key, "No_sfGC") == 0 ||
               _stricmp(key, "No_GC") == 0 ||
               _stricmp(key, "Zero_GC") == 0 ||
               _stricmp(key, "Overwrite_Heap") == 0 ||
               _stricmp(key, "OverwriteHeap") == 0) {
        bool en = (_stricmp(val, "1") == 0 ||
                   _stricmp(val, "true") == 0 ||
                   _stricmp(val, "yes") == 0 ||
                   _stricmp(val, "enable") == 0 ||
                   _stricmp(val, "enabled") == 0);
        cfg.strings.no_sfgc_overwrite_mode = en;
        if (en) {
            cfg.strings.gc_heuristic = sf_gc_heuristic_e::DISABLED;
        }
    } else if (_stricmp(key, "SCGC_Type") == 0 ||
               _stricmp(key, "SC_GC_Type") == 0 ||
               _stricmp(key, "NoGC_Type") == 0 ||
               _stricmp(key, "PrimitiveRW") == 0) {

        if (_stricmp(val, "PrimitiveRW") == 0  ||
            _stricmp(val, "Primitive_RW") == 0 ||
            _stricmp(val, "Primitive")   == 0  ||
            _stricmp(val, "RawRW")       == 0  ||
            _stricmp(val, "Raw")         == 0  ||
            _stricmp(val, "1")           == 0) {
            cfg.strings.scgc_type           = sf_scgc_type_e::PRIMITIVE_RW;
            cfg.strings.no_sfgc_overwrite_mode = true;
            cfg.strings.gc_heuristic        = sf_gc_heuristic_e::DISABLED;
        } else {

            cfg.strings.scgc_type = sf_scgc_type_e::DEFAULT;
        }
    }

    else if (_stricmp(key, "Runway_Handsel_Attempt_Value") == 0) {
        cfg.integers.runway_handsel_bytes = (size_t)parse_size_with_unit(val);
    } else if (_stricmp(key, "Landfill_Handsel_Attempt_Value") == 0) {
        cfg.integers.landfill_handsel_bytes = (size_t)parse_size_with_unit(val);
    } else if (_stricmp(key, "Wakes_GC_Cycling_Interval") == 0) {
        cfg.integers.wakes_gc_interval_ns = parse_time_with_unit_to_ns(val);
    } else if (_stricmp(key, "Line_Compaction_Freq") == 0) {
        cfg.integers.line_compaction_freq_ns = parse_time_with_unit_to_ns(val);
    } else if (_stricmp(key, "In_Heap_Forwarding_Rate") == 0) {
        cfg.integers.in_heap_forwarding_rate = (uint32_t)std::strtoul(val, nullptr, 10);
    } else if (_stricmp(key, "Pressure_Threshold") == 0) {
        cfg.integers.pressure_threshold_bytes = (size_t)parse_size_with_unit(val);
    } else if (_stricmp(key, "Slab_Chunk_Size") == 0) {
        cfg.integers.slab_chunk_size_bytes = (size_t)parse_size_with_unit(val);
    }

    else if (_stricmp(key, "Pressure_Mild_Ratio") == 0) {
        cfg.floats.pressure_mild_ratio = (float)std::strtod(val, nullptr);
    } else if (_stricmp(key, "Pressure_High_Ratio") == 0) {
        cfg.floats.pressure_high_ratio = (float)std::strtod(val, nullptr);
    } else if (_stricmp(key, "Pressure_Critical_Ratio") == 0) {
        cfg.floats.pressure_critical_ratio = (float)std::strtod(val, nullptr);
    } else if (_stricmp(key, "Landfill_Critical_Ratio") == 0) {
        cfg.floats.landfill_critical_ratio = (float)std::strtod(val, nullptr);
    } else if (_stricmp(key, "Falline_Compact_Ratio") == 0) {
        cfg.floats.falline_compact_ratio = (float)std::strtod(val, nullptr);
    }

    else if (_stricmp(key, "Profile") == 0 || _stricmp(key, "Mode") == 0 || _stricmp(key, "Profile_Mode") == 0) {
        if (_stricmp(val, "SC") == 0 || _stricmp(val, "SafetyCritical") == 0 ||
            _stricmp(val, "Safety_Critical") == 0 || _stricmp(val, "1") == 0) {
            cfg.profile = MSIMD_PROFILE_SAFETY_CRITICAL;
            cfg.is_safety_critical = true;
            cfg.forbid_runtime_heap = true;
            cfg.disable_jit_deopt = true;
        } else {
            cfg.profile = MSIMD_PROFILE_STANDARD;
            cfg.is_safety_critical = false;
        }
    } else if (_stricmp(key, "Forbid_Runtime_Heap") == 0 || _stricmp(key, "ForbidRuntimeHeap") == 0) {
        cfg.forbid_runtime_heap = (_stricmp(val, "1") == 0 || _stricmp(val, "true") == 0 || _stricmp(val, "yes") == 0);
    } else if (_stricmp(key, "Max_Bounded_Iterations") == 0 || _stricmp(key, "MaxLoopIterations") == 0) {
        cfg.max_loop_iterations = (uint32_t)std::strtoul(val, nullptr, 10);
    }
}

static inline bool validate_and_reconcile_sf_flags(sf_vm_config_t& cfg) {
    if (cfg.strings.gc_mode == sf_gc_mode_e::SIMD) {
        if (cfg.strings.critical_simd_explicit) {
            cfg.has_flag_conflict = true;
            if (cfg.strings.critical_simd_mode == sf_critical_simd_e::DENY) {
                snprintf(cfg.conflict_msg, sizeof(cfg.conflict_msg),
                         "FATAL FLAG CONFLICT: -#SF|GC_Mode:SIMD is incompatible with -#SF|Use_Critical_SIMDly_Operations:Deny! "
                         "SIMD GC requires SIMD vector units. Initialization rejected.");
                fprintf(stderr, "[SFVM Flag Conflict Error] %s\n", cfg.conflict_msg);
                return false;
            } else {
                snprintf(cfg.conflict_msg, sizeof(cfg.conflict_msg),
                         "INCOMPATIBILITY WARNING: -#SF|Use_Critical_SIMDly_Operations is superseded because -#SF|GC_Mode:SIMD is already fully SIMD-native.");
                fprintf(stderr, "[SFVM Flag Conflict Warning] %s\n", cfg.conflict_msg);
            }
        }
    }

    if (cfg.is_safety_critical || cfg.profile == MSIMD_PROFILE_SAFETY_CRITICAL) {
        cfg.profile = MSIMD_PROFILE_SAFETY_CRITICAL;
        cfg.is_safety_critical = true;
        cfg.forbid_runtime_heap = true;
        cfg.disable_jit_deopt = true;
    }
    return true;
}

static sf_vm_config_t parse_all_sf_flags(const char* custom_args) {
    sf_vm_config_t cfg;

    const char* env_sc = getenv("MSIMD_PROFILE");
    if (!env_sc) env_sc = getenv("SFVM_SC");
    if (env_sc) {
        if (_stricmp(env_sc, "SC") == 0 || _stricmp(env_sc, "SAFETY_CRITICAL") == 0 || _stricmp(env_sc, "1") == 0) {
            cfg.profile = MSIMD_PROFILE_SAFETY_CRITICAL;
            cfg.is_safety_critical = true;
            cfg.forbid_runtime_heap = true;
            cfg.disable_jit_deopt = true;
        }
    }

    const char* env_args = getenv("MATHVANTAGE_OPTS");
    if (!env_args) env_args = getenv("SF_VM_ARGS");
    if (env_args) {
        const char* p = env_args;
        while ((p = strstr(p, "-#SF|")) != nullptr) {
            parse_single_sf_flag(cfg, p);
            p += 5;
        }
    }

    if (custom_args) {
        const char* p = custom_args;
        while ((p = strstr(p, "-#SF|")) != nullptr) {
            parse_single_sf_flag(cfg, p);
            p += 5;
        }
    }

    validate_and_reconcile_sf_flags(cfg);

    return cfg;
}

typedef enum sacculus_event_t : uint8_t {

    SF_Ebin0_ALLOC         = 0x01,
    SF_Ebin0_FREE          = 0x02,
    SF_Ebin0_PROMOTE       = 0x03,
    SF_Ebin0_DEMOTE        = 0x04,

    SF_Ebin0_TO_FALLINE    = 0x05,
    SF_Ebin0_TO_LANDFILL   = 0x06,
    SF_Ebin0_EMPTYBOX      = 0x07,
    SF_Ebin0_LANDFILL_EVAC = 0x08,

    SF_Ebin0_HOTSWAP       = 0x09,
    SF_Ebin0_HOTSWAP_FAIL  = 0x0A,

    SF_Ebin0_GC_SWEEP      = 0x0B,
    SF_Ebin0_GC_COMPACT    = 0x0C,
    SF_Ebin0_GC_ALIGN      = 0x0D,

    SF_Ebin0_SAFEPOINT     = 0x0E,
    SF_Ebin0_HOOK          = 0x0F,
    SF_Ebin0_ERROR         = 0x10,
} sacculus_event_t;
using sfEvent = sacculus_event_t;

typedef enum sacculus_severity_t : uint8_t {
    SF_Ebin1_TRACE = 0, SF_Ebin1_INFO = 1, SF_Ebin1_WARN = 2,
    SF_Ebin1_ERROR = 3, SF_Ebin1_FATAL = 4,
} sacculus_severity_t;
using sfSeverity = sacculus_severity_t;

static_assert(sizeof(SF_custominiVm_hotlog_entry_t) == 64,
              "hotlog entry must be exactly 64 bytes");
static_assert(sizeof(SF_custominiVm_coldlog_entry_t) == 32,
              "coldlog entry must be exactly 32 bytes");

template<typename Entry, uint32_t Capacity>
struct sacculus_ring_t {
    SACFOR_ALIGN_CL std::atomic<uint64_t> write_head{0};
    SACFOR_ALIGN_CL std::atomic<uint64_t> read_head{0};
    SACFOR_ALIGN_CL Entry ring[Capacity];

    static constexpr uint32_t CAP = Capacity;

    void push(const Entry& e) {
        uint64_t idx = write_head.fetch_add(1, std::memory_order_relaxed)
                       & (uint64_t)(Capacity - 1);
        Entry tmp = e;
        tmp.ready = 0;
        ring[idx] = tmp;
        std::atomic_thread_fence(std::memory_order_release);
        ((std::atomic<uint8_t>*)&ring[idx].ready)->store(1, std::memory_order_relaxed);
    }

    void drain(void (*cb)(const Entry*, void*), void* user) {
        uint64_t w = write_head.load(std::memory_order_acquire);
        uint64_t r = read_head.load(std::memory_order_relaxed);
        while (r != w) {
            uint64_t idx = r & (uint64_t)(Capacity - 1);
            if (((std::atomic<uint8_t>*)&ring[idx].ready)->load(std::memory_order_relaxed) != 1) {
                break;
            }
            std::atomic_thread_fence(std::memory_order_acquire);
            cb(&ring[idx], user);
            ((std::atomic<uint8_t>*)&ring[idx].ready)->store(0, std::memory_order_relaxed);
            ++r;
        }
        read_head.store(r, std::memory_order_release);
    }

    void init(void) {
        write_head.store(0, std::memory_order_relaxed);
        read_head.store(0, std::memory_order_relaxed);
    }
};

template<typename Entry, uint32_t Capacity>
using sfRing = sacculus_ring_t<Entry, Capacity>;

using sacculus_hotlog_t  = sacculus_ring_t<SF_custominiVm_hotlog_entry_t,  SACFOR_HOTLOG_SIZE>;
using sacculus_coldlog_t = sacculus_ring_t<SF_custominiVm_coldlog_entry_t, SACFOR_COLDLOG_SIZE>;
using sfHotlog           = sacculus_hotlog_t;
using sfColdlog          = sacculus_coldlog_t;

static void sacculus_hotlog_write(
        sacculus_hotlog_t* log,
        sacculus_event_t ev, sacculus_severity_t sev,
        uint32_t fn_id, uint16_t slot, uint8_t gen, uint8_t reg,
        uint16_t core, uint64_t sweep_no, uint32_t heap_kb,
        const char* msg) {
    if (!log) return;
    SF_custominiVm_hotlog_entry_t e{};
    e.ts_ns       = sacculus_now_ns();
    e.tsc         = sacculus_rdtsc();
    e.gc_sweep_no = sweep_no;
    e.fn_id       = fn_id;
    e.heap_used_kb= heap_kb;
    e.slot        = slot;
    e.current_core= core;
    e.generation  = gen;
    e.region      = reg;
    e.event       = (uint8_t)ev;
    e.severity    = (uint8_t)sev;
    if (msg) {
        size_t len = strlen(msg);
        if (len > sizeof(e.msg) - 1) len = sizeof(e.msg) - 1;
        memcpy(e.msg, msg, len);
        e.msg[len] = '\0';
    }
    log->push(e);
}

static void sacculus_coldlog_write(
        sacculus_coldlog_t* log,
        sacculus_event_t ev, sacculus_severity_t sev,
        uint32_t fn_id, const char* msg) {
    if (!log) return;
    SF_custominiVm_coldlog_entry_t e{};
    e.ts_ns    = sacculus_now_ns();
    e.fn_id    = fn_id;
    e.event    = (uint8_t)ev;
    e.severity = (uint8_t)sev;
    if (msg) {
        size_t len = strlen(msg);
        if (len > sizeof(e.msg) - 1) len = sizeof(e.msg) - 1;
        memcpy(e.msg, msg, len);
        e.msg[len] = '\0';
    }
    log->push(e);
}

using sacculus_kernel_fn_t = SF_custominiVm_kernel_fn_t;
using sfKernelFn = SF_custominiVm_kernel_fn_t;

#define SACFOR_F32 SF_DTYPE_F32
#define SACFOR_F64 SF_DTYPE_F64
#define SACFOR_I32 SF_DTYPE_I32
#define SACFOR_I64 SF_DTYPE_I64
#define SACFOR_F16 SF_DTYPE_F16
#define SACFOR_U8  SF_DTYPE_U8

static constexpr size_t sf_dtype_size(sfDType d) {
    switch(d) {
        case SF_DTYPE_F16: case SF_DTYPE_U8:  return 2;
        case SF_DTYPE_F32: case SF_DTYPE_I32: return 4;
        case SF_DTYPE_F64: case SF_DTYPE_I64: return 8;
        default: return 4;
    }
}
static constexpr size_t sacculus_dtype_size(sfDType d) { return sf_dtype_size(d); }

static SF_custominiVm_tensor_t sf_tensor_make(void* data, const int32_t* shape,
                                              int ndim, sfDType dtype) {
    SF_custominiVm_tensor_t t{};
    t.data  = data;
    t.ndim  = (ndim > 8) ? 8 : ndim;
    t.dtype = (uint8_t)dtype;
    size_t elems = 1;
    for (int i = 0; i < t.ndim; ++i) {
        t.shape[i] = shape[i];
        if (shape[i] <= 0) { t = {}; return t; }
        size_t dim = (size_t)shape[i];
        if (elems > SIZE_MAX / dim) { t = {}; return t; }
        elems *= dim;
    }
    size_t dsz = sf_dtype_size(dtype);
    if (dsz > 0 && elems > SIZE_MAX / dsz) { t = {}; return t; }
    t.byte_size = elems * dsz;
    return t;
}
static inline SF_custominiVm_tensor_t sacculus_tensor_make(void* data, const int32_t* shape,
                                                           int ndim, sfDType dtype) {
    return sf_tensor_make(data, shape, ndim, dtype);
}

enum class sf_live_state_e : uint8_t {
    LIVE             = 0,
    TRANSIT_FALLINE  = 1,
    TRANSIT_REHAB    = 2,
    IN_FALLINE       = 3,
    IN_LANDFILL      = 4,
    QUARANTINED      = 5
};

struct sf_live_cold_metadata_t {
    std::atomic<uint64_t> per_core_cycles[256];
    std::atomic<uint64_t> visited_cores[4];
    char debug_name[24];
    uint64_t type_sig_hash;
    uint64_t birth_ns;
    std::atomic<uint64_t> age_ns;
    std::atomic<uint64_t> phase_count;
    std::atomic<uint64_t> bitop_count;
    std::atomic<uint64_t> avg_cycles_per_call;
    uint32_t call_step_count;
    std::atomic<uint32_t> error_count;
    uint64_t pagefile_slot_offset;
    std::atomic<uint8_t> is_paged_out;
    std::atomic<float> per_numa_heat[4];
    uint8_t preferred_numa_node;

    sf_live_cold_metadata_t() {
        memset(debug_name, 0, sizeof(debug_name));
        type_sig_hash = 0;
        birth_ns = 0;
        call_step_count = 0;
        pagefile_slot_offset = 0;
        preferred_numa_node = 0;
        for(int i=0; i<256; i++) per_core_cycles[i].store(0);
        for(int i=0; i<4; i++) visited_cores[i].store(0);
        age_ns.store(0);
        phase_count.store(0);
        bitop_count.store(0);
        avg_cycles_per_call.store(0);
        error_count.store(0);
        is_paged_out.store(0);
        for(int i=0; i<4; i++) per_numa_heat[i].store(0);
    }
};

struct SF_custominiVm_live_header_t;
using sfLiveHeader = SF_custominiVm_live_header_t;
using sacculus_live_header_t = sfLiveHeader;

static inline sf_live_cold_metadata_t* get_cold(SF_custominiVm_live_header_t* hdr);
static inline const sf_live_cold_metadata_t* get_cold_ro(const SF_custominiVm_live_header_t* hdr);

struct SF_custominiVm_live_header_t {
    std::atomic<sf_live_cold_metadata_t*> cold_meta{nullptr};

    static constexpr uint32_t SF_LIVE_MAGIC  = 0x53464D4E;
    static constexpr uint64_t SF_TAIL_CANARY = 0xDEADBEEFCAFEF00DULL;
    uint32_t  sfMN = SF_LIVE_MAGIC;
    size_t    total_cell_bytes = 0;

    uint32_t  function_id;

    std::atomic<uint64_t> total_cycles;

    std::atomic<uint16_t> current_core;

    std::atomic<uint64_t> last_access_ns;
    std::atomic<uint64_t> access_count;
    std::atomic<int32_t>  pin_count;
    std::atomic<uint8_t>  state{static_cast<uint8_t>(sf_live_state_e::LIVE)};

    std::atomic<uint64_t> memory_footprint;

    std::atomic<float>   heat_score;
    uint8_t              generation;
    uint8_t              region;
    std::atomic<uint8_t> gc_lock;
    uint8_t              priority;

    std::atomic<uint16_t> instance_count;
    uint16_t              instance_cap;
    uint8_t               gc_sweep_count;
    uint8_t               falline_marked;
    uint8_t               needs_rehabilitation;
    uint8_t               _pad0[1];

    std::atomic<void*>   forwarding_address{nullptr};

    std::atomic<uint32_t> child_view_count{0};
    std::atomic<SF_custominiVm_live_header_t*> parent_header{nullptr};
    size_t slice_byte_offset = 0;

    sacculus_live_header_t* next;
    sacculus_live_header_t* prev;

    sf_live_cold_metadata_t* get_cold() {
        sf_live_cold_metadata_t* cold = cold_meta.load(std::memory_order_acquire);
        if (!cold) {
            sf_live_cold_metadata_t* new_cold = new sf_live_cold_metadata_t();
            if (cold_meta.compare_exchange_strong(cold, new_cold, std::memory_order_acq_rel)) {
                cold = new_cold;
            } else {
                delete new_cold;
            }
        }
        return cold;
    }

    const sf_live_cold_metadata_t* get_cold_ro() const {
        return cold_meta.load(std::memory_order_acquire);
    }

    ~SF_custominiVm_live_header_t() {
        sf_live_cold_metadata_t* c = cold_meta.exchange(nullptr, std::memory_order_acq_rel);
        if (c) delete c;
    }

    void init(uint32_t fn_id, uint64_t sig_hash, const char* name,
              uint8_t prio = 128) {
        sfMN               = SF_LIVE_MAGIC;
        total_cell_bytes   = 0;
        function_id        = fn_id;
        sf_live_cold_metadata_t* cold = get_cold();
        if (cold) {
            cold->type_sig_hash = sig_hash;
            memset(cold->debug_name, 0, sizeof(cold->debug_name));
            if (name) {
                size_t len = strlen(name);
                if (len > sizeof(cold->debug_name) - 1) len = sizeof(cold->debug_name) - 1;
                memcpy(cold->debug_name, name, len);
            }
            cold->birth_ns = sacculus_now_ns();
            cold->age_ns.store(0, std::memory_order_relaxed);
            for (uint32_t c = 0; c < SACFOR_MAX_CORES; ++c)
                cold->per_core_cycles[c].store(0, std::memory_order_relaxed);
            for (int i=0; i<SACFOR_CORE_WORDS; ++i)
                cold->visited_cores[i].store(0, std::memory_order_relaxed);
            cold->phase_count.store(0, std::memory_order_relaxed);
            cold->bitop_count.store(0, std::memory_order_relaxed);
            cold->error_count.store(0, std::memory_order_relaxed);
            cold->avg_cycles_per_call.store(0, std::memory_order_relaxed);
            cold->call_step_count = 0;
            for (int i = 0; i < 4; ++i) cold->per_numa_heat[i].store(0.0f, std::memory_order_relaxed);
            cold->preferred_numa_node = 0;
            cold->is_paged_out.store(0, std::memory_order_relaxed);
            cold->pagefile_slot_offset = 0;
            last_access_ns.store(cold->birth_ns, std::memory_order_relaxed);
        } else {
            last_access_ns.store(sacculus_now_ns(), std::memory_order_relaxed);
        }
        total_cycles.store(0, std::memory_order_relaxed);
        new (&current_core) std::atomic<uint16_t>(0xFFFF);
        access_count.store(0, std::memory_order_relaxed);
        pin_count.store(0, std::memory_order_relaxed);
        state.store(static_cast<uint8_t>(sf_live_state_e::LIVE), std::memory_order_relaxed);
        memory_footprint.store(0, std::memory_order_relaxed);
        heat_score.store(0.0f, std::memory_order_relaxed);
        generation            = 0;
        region                = 0;
        gc_lock.store(0, std::memory_order_relaxed);
        priority              = prio;
        instance_count.store(0, std::memory_order_relaxed);
        instance_cap          = SACFOR_MAX_INSTANCES;
        gc_sweep_count        = 0;
        falline_marked        = 0;
        needs_rehabilitation  = 0;
        _pad0[0]              = 0;
        forwarding_address.store(nullptr, std::memory_order_relaxed);
        child_view_count.store(0, std::memory_order_relaxed);
        parent_header.store(nullptr, std::memory_order_relaxed);
        slice_byte_offset     = 0;
    }

    inline bool isValid(void) const {
        return sfMN == SF_LIVE_MAGIC;
    }

    void record_access(uint64_t cycles, uint16_t core_id) {

        if (!isTailCanaryValid()) {
            if (auto* cold = get_cold()) {
                cold->error_count.fetch_add(1, std::memory_order_relaxed);
            }
            state.store(static_cast<uint8_t>(sf_live_state_e::QUARANTINED), std::memory_order_release);
        }
        uint64_t now = sacculus_now_ns();
        last_access_ns.store(now, std::memory_order_release);
        access_count.fetch_add(1, std::memory_order_relaxed);
        total_cycles.fetch_add(cycles, std::memory_order_relaxed);

        auto* cold = get_cold();
        if (cold) {
            cold->age_ns.store(now - cold->birth_ns, std::memory_order_relaxed);
            ++cold->call_step_count;

            if (core_id < SACFOR_MAX_CORES) {
                cold->per_core_cycles[core_id].fetch_add(cycles, std::memory_order_relaxed);
                current_core.store(core_id, std::memory_order_release);
                sacculus_core_bitmap_set(cold->visited_cores, core_id);

                uint8_t numa = (uint8_t)(core_id / 64);
                if (numa < 4) {
                    float h = cold->per_numa_heat[numa].load(std::memory_order_relaxed);
                    cold->per_numa_heat[numa].store(h + SACFOR_HEAT_GAIN, std::memory_order_relaxed);
                    cold->preferred_numa_node = numa;
                }
            }
        }

        float old_h = heat_score.load(std::memory_order_relaxed);
        heat_score.store(old_h + SACFOR_HEAT_GAIN, std::memory_order_relaxed);
    }

    void decay_heat(void) {
        float h = heat_score.load(std::memory_order_relaxed);
        heat_score.store(h * SACFOR_HEAT_DECAY, std::memory_order_relaxed);
        ++gc_sweep_count;
    }

    bool try_pin(void) {

        while (true) {
            uint8_t st = state.load(std::memory_order_acquire);
            if (st != static_cast<uint8_t>(sf_live_state_e::LIVE)) {
                return false;
            }
            int32_t pc = pin_count.load(std::memory_order_relaxed);
            if (pin_count.compare_exchange_weak(pc, pc + 1,
                    std::memory_order_acq_rel, std::memory_order_relaxed)) {

                if (state.load(std::memory_order_acquire) == static_cast<uint8_t>(sf_live_state_e::LIVE)) {
                    return true;
                }

                pin_count.fetch_sub(1, std::memory_order_release);
                return false;
            }
        }
    }

    void unpin(void) {
        pin_count.fetch_sub(1, std::memory_order_release);
    }

    bool is_pinned(void) const {
        return pin_count.load(std::memory_order_acquire) > 0;
    }

    bool try_transition_to_falline(void) {

        if (is_pinned()) return false;
        if (child_view_count.load(std::memory_order_acquire) > 0) return false;

        uint8_t expected = static_cast<uint8_t>(sf_live_state_e::LIVE);
        if (state.compare_exchange_strong(expected, static_cast<uint8_t>(sf_live_state_e::TRANSIT_FALLINE),
                std::memory_order_acq_rel, std::memory_order_relaxed)) {

            if (is_pinned() || child_view_count.load(std::memory_order_acquire) > 0) {

                state.store(static_cast<uint8_t>(sf_live_state_e::LIVE), std::memory_order_release);
                return false;
            }
            falline_marked = 1;
            return true;
        }
        return false;
    }

    bool try_transition_to_rehab(void) {
        if (is_pinned()) return false;
        if (child_view_count.load(std::memory_order_acquire) > 0) return false;

        uint8_t expected = static_cast<uint8_t>(sf_live_state_e::LIVE);
        if (state.compare_exchange_strong(expected, static_cast<uint8_t>(sf_live_state_e::TRANSIT_REHAB),
                std::memory_order_acq_rel, std::memory_order_relaxed)) {
            if (is_pinned() || child_view_count.load(std::memory_order_acquire) > 0) {
                state.store(static_cast<uint8_t>(sf_live_state_e::LIVE), std::memory_order_release);
                return false;
            }
            return true;
        }
        return false;
    }

    bool is_idle(void) const {
        uint64_t now  = sacculus_now_ns();
        uint64_t last = last_access_ns.load(std::memory_order_acquire);
        uint64_t idle = now - last;
        switch (generation) {
            case 0: return idle > SACFOR_IDLE_YOUNG_NS;
            case 1: return idle > SACFOR_IDLE_ADOL_NS;
            case 2: return idle > SACFOR_IDLE_SENIOR_NS;
            default: return false;
        }
    }

    uint8_t compute_target_region(void) const {
        float h = heat_score.load(std::memory_order_relaxed);
        if (h > SACFOR_HEAT_HOT) return 3;
        size_t fp = (size_t)memory_footprint.load(std::memory_order_relaxed);
        const auto* cold = get_cold_ro();
        uint64_t avg = cold ? cold->avg_cycles_per_call.load(std::memory_order_relaxed) : 0;

        if (fp >= 64 * 1024 || avg > SACFOR_OVERHEAD_MIDDLE) return 2;
        if (avg > SACFOR_OVERHEAD_DEMAND) return 1;
        return 0;
    }

    inline uint8_t* body(void) {
        size_t hdr_sz = (sizeof(SF_custominiVm_live_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        return reinterpret_cast<uint8_t*>(this) + hdr_sz;
    }

    inline const uint8_t* body(void) const {
        size_t hdr_sz = (sizeof(SF_custominiVm_live_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        return reinterpret_cast<const uint8_t*>(this) + hdr_sz;
    }

    inline uint64_t* tailCanaryPtr(void) {
        size_t fp = (size_t)memory_footprint.load(std::memory_order_relaxed);
        size_t aligned_fp = (fp + 7) & ~7ULL;
        size_t header_sz = (sizeof(SF_custominiVm_live_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        if (total_cell_bytes > 0 && header_sz + aligned_fp + 8 > total_cell_bytes) {

            static uint64_t dummy_canary = SF_TAIL_CANARY;
            return &dummy_canary;
        }
        return reinterpret_cast<uint64_t*>(body() + aligned_fp);
    }

    inline void writeTailCanary(void) {
        size_t fp = (size_t)memory_footprint.load(std::memory_order_relaxed);
        size_t aligned_fp = (fp + 7) & ~7ULL;
        size_t header_sz = (sizeof(SF_custominiVm_live_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        if (total_cell_bytes > 0 && header_sz + aligned_fp + 8 > total_cell_bytes) return;
        *tailCanaryPtr() = SF_TAIL_CANARY;
    }

    SF_DISABLE_OPT_BEGIN
    inline bool isTailCanaryValid(void) const {
        size_t fp = (size_t)memory_footprint.load(std::memory_order_relaxed);
        size_t aligned_fp = (fp + 7) & ~7ULL;
        size_t header_sz = (sizeof(SF_custominiVm_live_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        if (total_cell_bytes > 0 && header_sz + aligned_fp + 8 > total_cell_bytes) return false;

        uint8_t m = g_sf_critical_simd_mode.load(std::memory_order_relaxed);
        if (m == static_cast<uint8_t>(sf_critical_simd_e::DENY)) {

            return *reinterpret_cast<const uint64_t*>(body() + aligned_fp) == SF_TAIL_CANARY;
        }
#if defined(__SSE4_1__)

        __m128i val = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(body() + aligned_fp));
        __m128i exp = _mm_set_epi64x(0, SF_TAIL_CANARY);
        __m128i cmp = _mm_cmpeq_epi64(val, exp);
        return (_mm_cvtsi128_si64(cmp) != 0);
#elif defined(__aarch64__)
        uint64x1_t val = vld1_u64(reinterpret_cast<const uint64_t*>(body() + aligned_fp));
        return (vget_lane_u64(val, 0) == SF_TAIL_CANARY);
#else
        return *reinterpret_cast<const uint64_t*>(body() + aligned_fp) == SF_TAIL_CANARY;
#endif
    }
    SF_DISABLE_OPT_END

    static inline size_t total_cell_size(size_t body_bytes) {
        size_t hdr_sz = (sizeof(SF_custominiVm_live_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        size_t body_aligned = (body_bytes + 7) & ~7ULL;
        size_t with_canary = body_aligned + sizeof(uint64_t);
        size_t cell_aligned = (hdr_sz + with_canary + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        return cell_aligned;
    }

    static inline SF_custominiVm_live_header_t* from_body(void* body_ptr) {
        if (!body_ptr) return nullptr;
        size_t hdr_sz = (sizeof(SF_custominiVm_live_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        auto* hdr = reinterpret_cast<SF_custominiVm_live_header_t*>(static_cast<uint8_t*>(body_ptr) - hdr_sz);
        if (hdr->sfMN != SF_LIVE_MAGIC) return nullptr;
        return hdr;
    }

    inline void* resolve_active_body(void) const {

        static constexpr int MAX_DEPTH = 16;
        const SF_custominiVm_live_header_t* cur = this;
        size_t accumulated_offset = 0;
        for (int depth = 0; depth < MAX_DEPTH; ++depth) {
            auto* p = cur->parent_header.load(std::memory_order_acquire);
            if (!p) {

                void* fwd = cur->forwarding_address.load(std::memory_order_acquire);
                uint8_t* base_ptr = fwd ? static_cast<uint8_t*>(fwd)
                                        : const_cast<uint8_t*>(cur->body());
                return base_ptr + accumulated_offset;
            }
            accumulated_offset += cur->slice_byte_offset;
            cur = p;
        }

        return nullptr;
    }
};

static inline sf_live_cold_metadata_t* get_cold(SF_custominiVm_live_header_t* hdr) {
    return hdr ? hdr->get_cold() : nullptr;
}

static inline const sf_live_cold_metadata_t* get_cold_ro(const SF_custominiVm_live_header_t* hdr) {
    return hdr ? hdr->get_cold_ro() : nullptr;
}

static inline uint32_t sf_get_call_step_count(const SF_custominiVm_live_header_t* hdr) {
    const auto* c = get_cold_ro(hdr);
    return c ? c->call_step_count : 0;
}

static inline const char* sf_get_debug_name(const SF_custominiVm_live_header_t* hdr) {
    const auto* c = get_cold_ro(hdr);
    return (c && c->debug_name[0]) ? c->debug_name : "";
}

struct sacculus_pin_guard_t {
    sfLiveHeader* obj = nullptr;
    bool          acquired = false;

    explicit sacculus_pin_guard_t(sfLiveHeader* o) : obj(o) {
        if (obj) {
            acquired = obj->try_pin();
        }
    }
    ~sacculus_pin_guard_t() {
        if (obj && acquired) {
            obj->unpin();
        }
    }
    bool is_valid() const { return acquired; }
    sacculus_pin_guard_t(const sacculus_pin_guard_t&) = delete;
    sacculus_pin_guard_t& operator=(const sacculus_pin_guard_t&) = delete;
};

using sfPinGuard = sacculus_pin_guard_t;
using sf_pin_guard_t = sacculus_pin_guard_t;

struct SACFOR_ALIGN_CL SF_custominiVm_emptybox_header_t {

    static constexpr uint32_t SF_EMPTYBOX_MAGIC = 0x53464542;
    uint32_t sfMN = SF_EMPTYBOX_MAGIC;

    uint32_t prev_function_id;
    size_t   prev_occupant_size;
    size_t   aligned_hole_size;

    uint8_t  _magic;
    uint8_t  is_decommitted;
    uint8_t  _pad[2];
    uint32_t decommitted_kb;

    std::atomic<void*> forwarding_address{nullptr};
    uint64_t           created_sweep_no = 0;

    SF_custominiVm_emptybox_header_t* next;
    SF_custominiVm_emptybox_header_t* prev;

    void init(uint32_t prev_fn, size_t prev_size, size_t aligned_size, void* fwd = nullptr, uint64_t sweep_no = 0) {
        sfMN               = SF_EMPTYBOX_MAGIC;
        prev_function_id   = prev_fn;
        prev_occupant_size = prev_size;
        aligned_hole_size  = aligned_size;
        _magic             = 0xEB;
        is_decommitted     = 0;
        _pad[0] = _pad[1]  = 0;
        decommitted_kb     = 0;
        forwarding_address.store(fwd, std::memory_order_release);
        created_sweep_no   = sweep_no;
        next = prev        = nullptr;
    }

    inline bool isValid(void) const {
        return sfMN == SF_EMPTYBOX_MAGIC || _magic == 0xEB;
    }

    inline uint8_t* hole_body(void) {
        size_t hdr_sz = (sizeof(SF_custominiVm_emptybox_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        return reinterpret_cast<uint8_t*>(this) + hdr_sz;
    }

    static inline SF_custominiVm_emptybox_header_t* from_body(void* body_ptr) {
        if (!body_ptr) return nullptr;

        size_t hdr_sz = (sizeof(SF_custominiVm_emptybox_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(static_cast<uint8_t*>(body_ptr) - hdr_sz);
        if (eb->sfMN != SF_EMPTYBOX_MAGIC && eb->_magic != 0xEB) return nullptr;
        return eb;
    }
};

using sfEmptyboxHeader = SF_custominiVm_emptybox_header_t;
using sacculus_emptybox_header_t = sfEmptyboxHeader;

typedef enum sf_death_reason_e : uint8_t {
    SF_DEATH_IDLE         = 0x01,
    SF_DEATH_PRESSURE     = 0x02,
    SF_DEATH_ERROR        = 0x03,
    SF_DEATH_EXPLICIT     = 0x04,
    SF_DEATH_REHABILITATE = 0x05,
} sf_death_reason_e;

using sfDeathReason = sf_death_reason_e;
using sacculus_death_reason_t = sf_death_reason_e;

struct SF_custominiVm_landfill_header_t {

    uint32_t  function_id;
    uint64_t  type_sig_hash;

    uint64_t  birth_ns;
    uint64_t  death_ns;
    uint64_t  lifetime_ns;
    uint64_t  freight_timestamp_ns;
    uint64_t  evacuation_duration_ns;

    uint64_t  total_cpu_cycles;
    uint64_t  per_core_cycles[SF_MAX_CORES];

    size_t    previous_size;
    size_t    pit_stored_size;
    size_t    compacted_size;

    uint8_t   lasting;
    uint8_t   header_pane_byte_ref;
    uint8_t   peak_generation;
    uint8_t   peak_region;
    sf_death_reason_e death_reason;
    uint8_t   error_count;
    uint16_t  last_core;
    uint16_t  unique_cores_visited;

    uint32_t  crc32;
    void*     header_pane_ref;

    void populate_from_live(const SF_custominiVm_live_header_t* live,
                            sf_death_reason_e reason,
                            uint64_t evac_duration_ns = 0,
                            size_t compacted_sz = 0) {
        function_id           = live->function_id;
        const auto* cold      = get_cold_ro(live);
        type_sig_hash         = cold ? cold->type_sig_hash : 0;
        birth_ns              = cold ? cold->birth_ns : 0;
        death_ns              = sacculus_now_ns();
        lifetime_ns           = death_ns - birth_ns;
        freight_timestamp_ns  = death_ns;
        evacuation_duration_ns= evac_duration_ns;

        total_cpu_cycles      = live->total_cycles.load(std::memory_order_relaxed);
        for (uint32_t c = 0; c < SF_MAX_CORES; ++c) {
            per_core_cycles[c] = cold ? cold->per_core_cycles[c].load(std::memory_order_relaxed) : 0;
        }

        previous_size         = (size_t)live->memory_footprint.load(std::memory_order_relaxed);
        pit_stored_size       = sizeof(SF_custominiVm_live_header_t);
        compacted_size        = compacted_sz ? compacted_sz : pit_stored_size;

        lasting               = (reason == SF_DEATH_REHABILITATE || live->needs_rehabilitation) ? 1 : 0;
        header_pane_byte_ref  = 0;
        peak_generation       = live->generation;
        peak_region           = live->region;
        death_reason          = reason;
        error_count           = (uint8_t)(cold ? cold->error_count.load(std::memory_order_relaxed) : 0);
        last_core             = live->current_core.load(std::memory_order_relaxed);
        unique_cores_visited  = (uint16_t)(cold ? sacculus_core_bitmap_count(cold->visited_cores) : 0);
        header_pane_ref       = nullptr;
        crc32                 = 0;
    }
};

using sfLandfillHeader = SF_custominiVm_landfill_header_t;
using sacculus_landfill_header_t = sfLandfillHeader;

struct sf_arena_storage_t {

    std::atomic<uint8_t*> base{nullptr};
    std::atomic<size_t>   capacity{0};
    std::atomic<size_t>   used{0};
    sf_ticket_lock_t      grow_lock;
    bool                  always_pretouch = false;

    bool init(size_t initial_bytes, bool pretouch = false) {
        always_pretouch = pretouch;
        uint8_t* b = (uint8_t*)sf_os_reserve_memory(initial_bytes);
        if (!b) return false;
        if (!sf_os_commit_memory(b, initial_bytes)) {
            sf_os_release_memory(b, initial_bytes);
            return false;
        }
        if (always_pretouch) sf_pretouch_memory(b, initial_bytes);
        base.store(b, std::memory_order_release);
        capacity.store(initial_bytes, std::memory_order_release);
        return true;
    }

    void* store(const void* body, size_t size) {
        size_t off = used.fetch_add(size, std::memory_order_acq_rel);
        size_t cap = capacity.load(std::memory_order_acquire);
        if (off + size > cap) {
            used.fetch_sub(size, std::memory_order_relaxed);
            return nullptr;
        }
        uint8_t* b = base.load(std::memory_order_acquire);
        if (!b) { used.fetch_sub(size, std::memory_order_relaxed); return nullptr; }
        memcpy(b + off, body, size);
        return b + off;
    }

    float fill_ratio(void) const {
        size_t cap = capacity.load(std::memory_order_relaxed);
        if (cap == 0) return 1.0f;
        return (float)used.load(std::memory_order_relaxed) / (float)cap;
    }

    void flush(void) {
        used.store(0, std::memory_order_release);
    }

    bool grow(void) {
        sf_ticket_guard_t g(grow_lock);
        size_t old_cap  = capacity.load(std::memory_order_relaxed);
        uint8_t* old_b  = base.load(std::memory_order_relaxed);
        size_t new_cap  = old_cap * SACFOR_LANDFILL_SCALE;
        uint8_t* new_b  = (uint8_t*)sf_os_reserve_memory(new_cap);
        if (!new_b) return false;
        if (!sf_os_commit_memory(new_b, new_cap)) {
            sf_os_release_memory(new_b, new_cap);
            return false;
        }
        size_t snap_used = used.load(std::memory_order_relaxed);
        memcpy(new_b, old_b, snap_used);
        if (always_pretouch && new_cap > old_cap)
            sf_pretouch_memory(new_b + old_cap, new_cap - old_cap);

        base.store(new_b, std::memory_order_release);
        capacity.store(new_cap, std::memory_order_release);
        sf_os_release_memory(old_b, old_cap);
        return true;
    }

    void destroy(void) {
        uint8_t* b = base.load(std::memory_order_acquire);
        if (!b) return;
        sf_os_release_memory(b, capacity.load(std::memory_order_relaxed));
        base.store(nullptr, std::memory_order_release);
    }
};

struct sf_garbage_pit_t {
    sf_arena_storage_t matured_arena;
    sf_arena_storage_t rehab_arena;

    bool init(size_t initial_bytes = SACFOR_LANDFILL_INIT, bool always_pretouch = false) {
        size_t half = initial_bytes / 2;
        if (half < 1024 * 1024) half = 1024 * 1024;
        return matured_arena.init(half, always_pretouch) && rehab_arena.init(half, always_pretouch);
    }

    void* store_matured(const void* body, size_t size) {
        return matured_arena.store(body, size);
    }

    void* store_rehab(const void* body, size_t size) {
        return rehab_arena.store(body, size);
    }

    float max_fill_ratio(void) const {
        float f1 = matured_arena.fill_ratio();
        float f2 = rehab_arena.fill_ratio();
        return (f1 > f2) ? f1 : f2;
    }

    size_t total_used(void) const {
        return matured_arena.used.load(std::memory_order_relaxed) +
               rehab_arena.used.load(std::memory_order_relaxed);
    }

    void flush(void) {
        matured_arena.flush();
        rehab_arena.flush();
    }

    void destroy(void) {
        matured_arena.destroy();
        rehab_arena.destroy();
    }
};
using sfGarbagePit = sf_garbage_pit_t;
using sacculus_garbage_pit_t = sf_garbage_pit_t;

struct sacculus_header_pane_t {
    static constexpr uint32_t PANE_CAP = 65536;

    std::unique_ptr<SF_custominiVm_landfill_header_t[]> entries_ptr;
    SF_custominiVm_landfill_header_t*                   entries = nullptr;
    std::atomic<uint32_t>                               count{0};
    sf_ticket_lock_t                                    lock;

    bool init() {
        if (entries_ptr) return true;
        entries_ptr = std::make_unique<SF_custominiVm_landfill_header_t[]>(PANE_CAP);
        entries = entries_ptr.get();
        return entries_ptr != nullptr;
    }

    int32_t push(const SF_custominiVm_landfill_header_t& h) {
        if (!entries_ptr) return -1;
        sf_ticket_guard_t g(lock);
        uint32_t idx = count.load(std::memory_order_relaxed);
        if (idx >= PANE_CAP) return -1;
        entries_ptr[idx] = h;
        count.fetch_add(1, std::memory_order_release);
        return (int32_t)idx;
    }

    void flush(void) {
        sf_ticket_guard_t g(lock);
        count.store(0, std::memory_order_release);
    }
};

using sfHeaderPane = sacculus_header_pane_t;

struct sacculus_landfill_t {
    sf_garbage_pit_t        pit;
    sacculus_header_pane_t  pane;

    bool accept(SF_custominiVm_live_header_t* live,
                sf_death_reason_e reason,
                sacculus_hotlog_t* hotlog,
                uint64_t sweep_no,
                uint64_t evac_duration_ns = 0) {
        SF_custominiVm_landfill_header_t lh{};
        lh.populate_from_live(live, reason, evac_duration_ns);

        pane.init();
        int32_t pane_idx = pane.push(lh);
        if (pane_idx >= 0 && pane.entries_ptr) {
            pane.entries_ptr[pane_idx].header_pane_byte_ref = (uint8_t)(pane_idx & 0xFF);
            pane.entries_ptr[pane_idx].header_pane_ref = &pane.entries_ptr[pane_idx];
        }

        void* pit_addr = nullptr;
        bool is_rehab = (reason == SF_DEATH_REHABILITATE || live->needs_rehabilitation);
        if (is_rehab) {
            pit_addr = pit.store_rehab(live, sizeof(SF_custominiVm_live_header_t));
        } else {
            pit_addr = pit.store_matured(live, sizeof(SF_custominiVm_live_header_t));
        }

        if (hotlog) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_TO_LANDFILL,
                SF_Ebin1_INFO, live->function_id,
                (uint16_t)(pane_idx >= 0 ? (pane_idx & 0xFFFF) : 0),
                live->generation, live->region,
                live->current_core.load(std::memory_order_relaxed), sweep_no,
                (uint32_t)(pit.total_used() / 1024),
                is_rehab ? "rehab_accept" : "matured_accept");
        }
        return pit_addr != nullptr;
    }

    bool is_critical(void) const {
        return pit.max_fill_ratio() > SACFOR_LANDFILL_CRIT ||
               pane.count.load(std::memory_order_relaxed) > sacculus_header_pane_t::PANE_CAP * 9 / 10;
    }

    void evacuate(sacculus_hotlog_t* hotlog, uint64_t sweep_no) {
        if (hotlog) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_LANDFILL_EVAC,
                SF_Ebin1_WARN, 0, 0, 0, 0, 0, sweep_no,
                (uint32_t)(pit.total_used() / 1024), "sync_evac");
        }
        pit.flush();
        pane.flush();
    }

    bool init(size_t initial = SACFOR_LANDFILL_INIT, bool always_pretouch = false) {
        return pit.init(initial, always_pretouch);
    }

    void destroy(void) { pit.destroy(); }
};

using sfLandfill = sacculus_landfill_t;

struct sacculus_order_of_height_t {
    sacculus_live_header_t* head  = nullptr;
    size_t                  count = 0;
    sf_ticket_lock_t        lock;

    void insert_nolock(sacculus_live_header_t* obj) {
        obj->next = obj->prev = nullptr;
        if (!head) { head = obj; ++count; return; }

        sacculus_live_header_t* cur = head;
        while (cur && sf_get_call_step_count(cur) > sf_get_call_step_count(obj))
            cur = cur->next;

        if (!cur) {
            sacculus_live_header_t* tail = head;
            while (tail->next) tail = tail->next;
            tail->next = obj; obj->prev = tail;
        } else if (cur == head) {
            obj->next = head; head->prev = obj; head = obj;
        } else {
            sacculus_live_header_t* p = cur->prev;
            p->next = obj; obj->prev = p; obj->next = cur; cur->prev = obj;
        }
        ++count;
    }

    void insert(sacculus_live_header_t* obj) {
        sf_ticket_guard_t g(lock);
        insert_nolock(obj);
    }

    void remove(sacculus_live_header_t* obj) {
        sf_ticket_guard_t g(lock);
        remove_nolock(obj);
    }

    void remove_nolock(sacculus_live_header_t* obj) {
        if (obj->prev) obj->prev->next = obj->next;
        else           head = obj->next;
        if (obj->next) obj->next->prev = obj->prev;
        obj->next = obj->prev = nullptr;
        if (count > 0) --count;
    }

    void bubble_up_nolock(sacculus_live_header_t* obj) {
        if (!obj) return;
        while (obj->prev && sf_get_call_step_count(obj) > sf_get_call_step_count(obj->prev)) {
            sacculus_live_header_t* p = obj->prev;
            sacculus_live_header_t* pp = p->prev;
            sacculus_live_header_t* n = obj->next;

            if (pp) pp->next = obj;
            else head = obj;
            obj->prev = pp;

            obj->next = p;
            p->prev = obj;

            p->next = n;
            if (n) n->prev = p;
        }
    }

    void bubble_up(sacculus_live_header_t* obj) {
        sf_ticket_guard_t g(lock);
        bubble_up_nolock(obj);
    }
};

using sfOrderOfHeight = sacculus_order_of_height_t;
using sf_order_of_height_t = sacculus_order_of_height_t;

struct sf_fibonacci_indexed_bucket_t {
    static constexpr size_t BUCKET_CAP = 1024;
    static constexpr size_t BUCKET_MASK = BUCKET_CAP - 1;
    static constexpr uint64_t FIBONACCI_PRIME = 11400714819323198485ULL;

    sacculus_live_header_t* slots[BUCKET_CAP];
    sacculus_live_header_t* head = nullptr;
    size_t                  count = 0;
    sf_ticket_lock_t        lock;

    sf_fibonacci_indexed_bucket_t() {
        memset(slots, 0, sizeof(slots));
    }

    inline size_t hash_index(uint32_t fn_id) const {
        return (size_t)((fn_id * FIBONACCI_PRIME) >> (64 - 10)) & BUCKET_MASK;
    }

    void insert_nolock(sacculus_live_header_t* obj) {

        size_t idx = hash_index(obj->function_id);
        size_t attempts = 0;
        while (slots[idx] != nullptr && attempts < BUCKET_CAP) {
            idx = (idx + 1) & BUCKET_MASK;
            ++attempts;
        }
        if (attempts < BUCKET_CAP) {
            slots[idx] = obj;
        }

        obj->prev = nullptr;
        obj->next = head;
        if (head) head->prev = obj;
        head = obj;
        ++count;
    }

    void insert(sacculus_live_header_t* obj) {
        sf_ticket_guard_t g(lock);
        insert_nolock(obj);
    }

    void remove(sacculus_live_header_t* obj) {
        sf_ticket_guard_t g(lock);
        remove_nolock(obj);
    }

    void remove_nolock(sacculus_live_header_t* obj) {

        size_t idx = hash_index(obj->function_id);
        size_t attempts = 0;
        size_t found_idx = BUCKET_CAP;
        while (attempts < BUCKET_CAP) {
            if (slots[idx] == obj) {
                found_idx = idx;
                break;
            }
            if (slots[idx] == nullptr) break;
            idx = (idx + 1) & BUCKET_MASK;
            ++attempts;
        }

        if (found_idx < BUCKET_CAP) {

            size_t empty_slot = found_idx;
            size_t curr = (empty_slot + 1) & BUCKET_MASK;
            while (slots[curr] != nullptr) {
                size_t natural_slot = hash_index(slots[curr]->function_id);
                bool can_shift = false;
                if (curr >= natural_slot) {
                    can_shift = (empty_slot >= natural_slot && empty_slot < curr);
                } else {
                    can_shift = (empty_slot >= natural_slot || empty_slot < curr);
                }
                if (can_shift) {
                    slots[empty_slot] = slots[curr];
                    empty_slot = curr;
                }
                curr = (curr + 1) & BUCKET_MASK;
            }
            slots[empty_slot] = nullptr;
        }

        if (obj->prev) obj->prev->next = obj->next;
        else           head = obj->next;
        if (obj->next) obj->next->prev = obj->prev;
        obj->next = obj->prev = nullptr;
        if (count > 0) --count;
    }

    sacculus_live_header_t* lookup_direct(uint32_t fn_id) {
        size_t idx = hash_index(fn_id);
        size_t attempts = 0;
        while (attempts < 16) {
            sacculus_live_header_t* o = slots[idx];
            if (!o) break;
            if (o->function_id == fn_id) return o;
            idx = (idx + 1) & BUCKET_MASK;
            ++attempts;
        }

        sacculus_live_header_t* cur = head;
        while (cur) {
            if (cur->function_id == fn_id) return cur;
            cur = cur->next;
        }
        return nullptr;
    }
};
using sfFibonacciIndexedBucket = sf_fibonacci_indexed_bucket_t;
using sfUnorderedBucket = sf_fibonacci_indexed_bucket_t;
using sacculus_unordered_bucket_t = sf_fibonacci_indexed_bucket_t;

enum sf_region_id_t : uint8_t {
    SF_REGION_DEMAND   = 0,
    SF_REGION_MIDDLE   = 1,
    SF_REGION_TANK     = 2,
    SF_REGION_HOTFIELD = 3,
};
using sfRegionId = sf_region_id_t;
using sacculus_region_id_t = sf_region_id_t;

#define SACFOR_REGION_DEMAND   SF_REGION_DEMAND
#define SACFOR_REGION_MIDDLE   SF_REGION_MIDDLE
#define SACFOR_REGION_TANK     SF_REGION_TANK
#define SACFOR_REGION_HOTFIELD SF_REGION_HOTFIELD

static const char* sf_region_names[] = {"Demand","Middle","Tank","Hotfield"};
static const char* const* sacculus_region_names = sf_region_names;

struct sf_region_t {
    sf_region_id_t                id;
    sf_order_of_height_t          hot_order;
    sf_fibonacci_indexed_bucket_t bucket;
    std::atomic<size_t>           total_bytes{0};
    std::atomic<size_t>           count{0};

    sf_ticket_lock_t& get_lock(void) {
        return (id == SF_REGION_HOTFIELD) ? hot_order.lock : bucket.lock;
    }

    sfLiveHeader* get_head(void) {
        return (id == SF_REGION_HOTFIELD) ? hot_order.head : bucket.head;
    }

    void place_nolock(sfLiveHeader* obj) {
        obj->region = (uint8_t)id;
        if (id == SF_REGION_HOTFIELD) {
            hot_order.insert_nolock(obj);
        } else {
            bucket.insert_nolock(obj);
        }
        count.fetch_add(1, std::memory_order_relaxed);
        total_bytes.fetch_add((size_t)obj->memory_footprint.load(std::memory_order_relaxed), std::memory_order_relaxed);
    }

    void place(sfLiveHeader* obj) {
        sf_ticket_guard_t g(get_lock());
        place_nolock(obj);
    }

    void evict(sfLiveHeader* obj) {
        sf_ticket_guard_t g(get_lock());
        evict_nolock(obj);
    }

    void evict_nolock(sfLiveHeader* obj) {
        if (id == SF_REGION_HOTFIELD) {
            hot_order.remove_nolock(obj);
        } else {
            bucket.remove_nolock(obj);
        }
        count.fetch_sub(1, std::memory_order_relaxed);
        size_t fp = (size_t)obj->memory_footprint.load(std::memory_order_relaxed);
        size_t cur = total_bytes.load(std::memory_order_relaxed);
        if (cur >= fp) total_bytes.fetch_sub(fp, std::memory_order_relaxed);
    }
};

using sfRegion = sf_region_t;
using sacculus_region_t = sf_region_t;

enum sf_room_id_t : uint8_t {
    SF_ROOM_YOUNG      = 0,
    SF_ROOM_ADOLESCENT = 1,
    SF_ROOM_SENIOR     = 2,
};
using sfRoomId = sf_room_id_t;
using sacculus_room_id_t = sf_room_id_t;

#define SACFOR_ROOM_YOUNG      SF_ROOM_YOUNG
#define SACFOR_ROOM_ADOLESCENT SF_ROOM_ADOLESCENT
#define SACFOR_ROOM_SENIOR     SF_ROOM_SENIOR

static const char* sf_room_names[] = {"Young","Adolescent","Senior"};
static const char* const* sacculus_room_names = sf_room_names;

struct sf_room_t {
    sf_room_id_t       id;
    sf_region_t        regions[SACFOR_REGION_COUNT];
    std::atomic<size_t> total_objects{0};
    std::atomic<size_t> total_bytes{0};

    void init(sf_room_id_t rid) {
        id = rid;
        for (int i = 0; i < SACFOR_REGION_COUNT; ++i)
            regions[i].id = (sf_region_id_t)i;
    }

    void place(sfLiveHeader* obj) {
        obj->generation = (uint8_t)id;
        uint8_t reg = obj->compute_target_region();
        regions[reg].place(obj);
        total_objects.fetch_add(1, std::memory_order_relaxed);
        total_bytes.fetch_add((size_t)obj->memory_footprint.load(),
                              std::memory_order_relaxed);
    }

    void evict(sfLiveHeader* obj) {
        if (obj->region < SACFOR_REGION_COUNT)
            regions[obj->region].evict(obj);
        total_objects.fetch_sub(1, std::memory_order_relaxed);
        size_t fp = (size_t)obj->memory_footprint.load();
        size_t cur = total_bytes.load();
        if (cur >= fp)
            total_bytes.fetch_sub(fp, std::memory_order_relaxed);
    }
};

using sfRoom = sf_room_t;
using sacculus_room_t = sf_room_t;

struct sf_motherline_t {
    sf_room_t rooms[SACFOR_ROOM_COUNT];

    void init(void) {
        for (int i = 0; i < SACFOR_ROOM_COUNT; ++i)
            rooms[i].init((sf_room_id_t)i);
    }

    void admit(sfLiveHeader* obj) {
        rooms[SF_ROOM_YOUNG].place(obj);
    }

    void evict(sfLiveHeader* obj) {
        if (obj->generation < SACFOR_ROOM_COUNT)
            rooms[obj->generation].evict(obj);
    }
};

using sfMotherline = sf_motherline_t;
using sacculus_motherline_t = sf_motherline_t;

struct sacculus_falline_t {
    static constexpr size_t NUM_EMPTYBOX_BINS = 8;
    static constexpr size_t EMPTYBOX_BIN_SIZES[NUM_EMPTYBOX_BINS] = {
        64,
        256,
        1024,
        4096,
        16384,
        65536,
        262144,
        SIZE_MAX
    };

    static inline int find_emptybox_bin(size_t sz) {
        for (int i = 0; i < (int)NUM_EMPTYBOX_BINS - 1; ++i) {
            if (sz <= EMPTYBOX_BIN_SIZES[i]) return i;
        }
        return (int)NUM_EMPTYBOX_BINS - 1;
    }

    sacculus_live_header_t* head = nullptr;
    size_t count = 0;
    std::atomic<size_t> total_bytes{0};
    sf_ticket_lock_t lock;

    sacculus_emptybox_header_t* bin_heads[NUM_EMPTYBOX_BINS] = {nullptr};
    size_t bin_counts[NUM_EMPTYBOX_BINS] = {0};
    std::atomic<size_t> emptybox_count{0};
    std::atomic<bool> is_compacting{false};
    sf_ticket_lock_t emptybox_lock;
    sf_ticket_lock_t bin_locks[NUM_EMPTYBOX_BINS];

    std::atomic<uint64_t> snatch_hit_count{0};
    std::atomic<uint64_t> snatch_miss_count{0};
    std::atomic<uint64_t> split_count{0};
    std::atomic<uint64_t> coalesced_merge_count{0};
    std::atomic<size_t>   total_rebated_bytes{0};
    std::atomic<uint64_t> total_rebated_count{0};

    void push(sacculus_live_header_t* obj) {
        sf_ticket_guard_t g(lock);
        obj->next = head;
        obj->prev = nullptr;
        if (head) head->prev = obj;
        head = obj;
        ++count;
        total_bytes.fetch_add((size_t)obj->memory_footprint.load(),
                              std::memory_order_relaxed);
        obj->falline_marked = 1;
    }

    sacculus_live_header_t* pop(void) {
        sf_ticket_guard_t g(lock);
        if (!head) return nullptr;
        sacculus_live_header_t* obj = head;
        head = obj->next;
        if (head) head->prev = nullptr;
        obj->next = obj->prev = nullptr;
        --count;
        size_t fp = (size_t)obj->memory_footprint.load();
        size_t cur = total_bytes.load();
        if (cur >= fp) total_bytes.fetch_sub(fp, std::memory_order_relaxed);
        return obj;
    }

    void register_emptybox(uint32_t prev_fn, size_t prev_size, size_t aligned_size,
                           SF_custominiVm_emptybox_header_t* eb, void* fwd = nullptr, uint64_t sweep_no = 0) {
        while (is_compacting.load(std::memory_order_acquire)) {
            sf_cpu_pause();
        }
        eb->init(prev_fn, prev_size, aligned_size, fwd, sweep_no);
        int bin = find_emptybox_bin(aligned_size);
        sf_ticket_guard_t g(bin_locks[bin]);
        eb->next = bin_heads[bin];
        eb->prev = nullptr;
        if (bin_heads[bin]) bin_heads[bin]->prev = eb;
        bin_heads[bin] = eb;
        ++bin_counts[bin];
        emptybox_count.fetch_add(1, std::memory_order_relaxed);
    }

    void register_emptybox(size_t prev_size, size_t aligned_size,
                           SF_custominiVm_emptybox_header_t* eb) {
        register_emptybox(0, prev_size, aligned_size, eb, nullptr, 0);
    }

    SF_custominiVm_emptybox_header_t* find_and_take_emptybox(size_t needed_cell_bytes, uint64_t current_sweep = 0) {
        if (is_compacting.load(std::memory_order_acquire)) return nullptr;
        size_t needed_aligned = (needed_cell_bytes + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        int start_bin = find_emptybox_bin(needed_aligned);

        for (int bin = start_bin; bin < (int)NUM_EMPTYBOX_BINS; ++bin) {
            SF_custominiVm_emptybox_header_t* candidate = nullptr;
            size_t rem = 0;

            {
                sf_ticket_guard_t g(bin_locks[bin]);
                SF_custominiVm_emptybox_header_t* cur = bin_heads[bin];
                while (cur) {
                    if (cur->aligned_hole_size >= needed_aligned) {

                        if (cur->forwarding_address.load(std::memory_order_relaxed) != nullptr) {
                            if (current_sweep > 0 && cur->created_sweep_no >= current_sweep) {
                                cur = cur->next;
                                continue;
                            }
                            cur->forwarding_address.store(nullptr, std::memory_order_relaxed);
                        }

                        if (cur->prev) cur->prev->next = cur->next;
                        else bin_heads[bin] = cur->next;
                        if (cur->next) cur->next->prev = cur->prev;
                        cur->next = cur->prev = nullptr;
                        --bin_counts[bin];
                        emptybox_count.fetch_sub(1, std::memory_order_relaxed);

                        candidate = cur;
                        rem = candidate->aligned_hole_size - needed_aligned;
                        break;
                    }
                    cur = cur->next;
                }
            }

            if (candidate) {

                if (rem >= 128) {
                    candidate->aligned_hole_size = needed_aligned;

                    auto* tail = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(
                        reinterpret_cast<uint8_t*>(candidate) + needed_aligned);
                    tail->init(candidate->prev_function_id, 0, rem, nullptr, current_sweep);

                    split_count.fetch_add(1, std::memory_order_relaxed);

                    int tail_bin = find_emptybox_bin(rem);
                    {
                        sf_ticket_guard_t g_tail(bin_locks[tail_bin]);
                        tail->next = bin_heads[tail_bin];
                        tail->prev = nullptr;
                        if (bin_heads[tail_bin]) bin_heads[tail_bin]->prev = tail;
                        bin_heads[tail_bin] = tail;
                        ++bin_counts[tail_bin];
                        emptybox_count.fetch_add(1, std::memory_order_relaxed);
                    }
                }

                if (candidate->is_decommitted) {
                    sf_os_commit_memory(candidate, candidate->aligned_hole_size);
                    candidate->is_decommitted = 0;
                    candidate->decommitted_kb = 0;
                }

                snatch_hit_count.fetch_add(1, std::memory_order_relaxed);
                return candidate;
            }
        }

        snatch_miss_count.fetch_add(1, std::memory_order_relaxed);
        return nullptr;
    }

    static SF_custominiVm_emptybox_header_t* merge_address_lists(
        SF_custominiVm_emptybox_header_t* a,
        SF_custominiVm_emptybox_header_t* b)
    {
        if (!a) return b;
        if (!b) return a;

        SF_custominiVm_emptybox_header_t* head = nullptr;
        SF_custominiVm_emptybox_header_t* tail = nullptr;

        if (reinterpret_cast<uintptr_t>(a) <= reinterpret_cast<uintptr_t>(b)) {
            head = tail = a;
            a = a->next;
        } else {
            head = tail = b;
            b = b->next;
        }
        head->prev = nullptr;

        while (a && b) {
            if (reinterpret_cast<uintptr_t>(a) <= reinterpret_cast<uintptr_t>(b)) {
                tail->next = a;
                a->prev = tail;
                tail = a;
                a = a->next;
            } else {
                tail->next = b;
                b->prev = tail;
                tail = b;
                b = b->next;
            }
        }
        SF_custominiVm_emptybox_header_t* rem = a ? a : b;
        tail->next = rem;
        if (rem) rem->prev = tail;

        return head;
    }

    static SF_custominiVm_emptybox_header_t* sort_emptyboxes_by_address(SF_custominiVm_emptybox_header_t* head) {
        if (!head || !head->next) return head;

        SF_custominiVm_emptybox_header_t* slow = head;
        SF_custominiVm_emptybox_header_t* fast = head->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }

        SF_custominiVm_emptybox_header_t* mid = slow->next;
        slow->next = nullptr;
        if (mid) mid->prev = nullptr;

        SF_custominiVm_emptybox_header_t* left = sort_emptyboxes_by_address(head);
        SF_custominiVm_emptybox_header_t* right = sort_emptyboxes_by_address(mid);
        return merge_address_lists(left, right);
    }

    size_t compact_emptyboxes(void) {
        sf_ticket_guard_t g_global(emptybox_lock);
        is_compacting.store(true, std::memory_order_release);
        for (size_t b = 0; b < NUM_EMPTYBOX_BINS; ++b) {
            bin_locks[b].lock();
        }

        if (emptybox_count.load(std::memory_order_relaxed) < 2) {
            for (size_t b = 0; b < NUM_EMPTYBOX_BINS; ++b) {
                bin_locks[b].unlock();
            }
            is_compacting.store(false, std::memory_order_release);
            return 0;
        }

        SF_custominiVm_emptybox_header_t* flat_head = nullptr;
        SF_custominiVm_emptybox_header_t* flat_tail = nullptr;

        for (size_t b = 0; b < NUM_EMPTYBOX_BINS; ++b) {
            if (!bin_heads[b]) continue;
            if (!flat_head) {
                flat_head = bin_heads[b];
            } else {
                flat_tail->next = bin_heads[b];
                bin_heads[b]->prev = flat_tail;
            }
            SF_custominiVm_emptybox_header_t* t = bin_heads[b];
            while (t->next) t = t->next;
            flat_tail = t;

            bin_heads[b] = nullptr;
            bin_counts[b] = 0;
        }
        emptybox_count.store(0, std::memory_order_relaxed);

        if (!flat_head) {
            for (size_t b = 0; b < NUM_EMPTYBOX_BINS; ++b) {
                bin_locks[b].unlock();
            }
            return 0;
        }

        flat_head = sort_emptyboxes_by_address(flat_head);

        size_t merged = 0;
        SF_custominiVm_emptybox_header_t* cur = flat_head;

        while (cur) {
            bool cur_has_fwd = (cur->forwarding_address.load(std::memory_order_relaxed) != nullptr);

            while (cur->next && !cur_has_fwd) {
                SF_custominiVm_emptybox_header_t* nxt = cur->next;
                bool nxt_has_fwd = (nxt->forwarding_address.load(std::memory_order_relaxed) != nullptr);
                if (nxt_has_fwd) break;

                uint8_t* cur_end = reinterpret_cast<uint8_t*>(cur) + cur->aligned_hole_size;
                uint8_t* nxt_start = reinterpret_cast<uint8_t*>(nxt);

                if (cur_end == nxt_start) {
                    cur->aligned_hole_size += nxt->aligned_hole_size;
                    cur->prev_occupant_size += nxt->prev_occupant_size;
                    if (nxt->is_decommitted) {
                        cur->is_decommitted = 1;
                        cur->decommitted_kb += nxt->decommitted_kb;
                    }

                    cur->next = nxt->next;
                    if (nxt->next) nxt->next->prev = cur;
                    nxt->sfMN = 0;
                    nxt->_magic = 0;

                    ++merged;
                } else {
                    break;
                }
            }

            static constexpr size_t DECOMMIT_THRESHOLD = 65536;
            if (cur->aligned_hole_size >= DECOMMIT_THRESHOLD) {
                uintptr_t start = reinterpret_cast<uintptr_t>(cur) + sizeof(SF_custominiVm_emptybox_header_t);
                uintptr_t page_start = (start + 4095ULL) & ~4095ULL;
                uintptr_t end = reinterpret_cast<uintptr_t>(cur) + cur->aligned_hole_size;
                uintptr_t page_end = end & ~4095ULL;

                if (page_end > page_start) {
                    size_t decommit_bytes = page_end - page_start;
                    if (decommit_bytes >= DECOMMIT_THRESHOLD && !cur->is_decommitted) {
                        sf_os_decommit_memory(reinterpret_cast<void*>(page_start), decommit_bytes);
                        cur->is_decommitted = 1;
                        cur->decommitted_kb = (uint32_t)(decommit_bytes / 1024);
                        total_rebated_bytes.fetch_add(decommit_bytes, std::memory_order_relaxed);
                        total_rebated_count.fetch_add(1, std::memory_order_relaxed);
                    }
                }
            }

            cur = cur->next;
        }

        cur = flat_head;
        while (cur) {
            SF_custominiVm_emptybox_header_t* next_node = cur->next;
            int bin = find_emptybox_bin(cur->aligned_hole_size);
            cur->next = bin_heads[bin];
            cur->prev = nullptr;
            if (bin_heads[bin]) bin_heads[bin]->prev = cur;
            bin_heads[bin] = cur;
            ++bin_counts[bin];
            emptybox_count.fetch_add(1, std::memory_order_relaxed);
            cur = next_node;
        }

        for (size_t b = 0; b < NUM_EMPTYBOX_BINS; ++b) {
            bin_locks[b].unlock();
        }

        coalesced_merge_count.fetch_add(merged, std::memory_order_relaxed);
        is_compacting.store(false, std::memory_order_release);
        return merged;
    }

    float emptybox_ratio(void) const {
        size_t e_count = emptybox_count.load(std::memory_order_relaxed);
        size_t total = count + e_count;
        if (total == 0) return 0.0f;
        return (float)e_count / (float)total;
    }

    uint64_t get_snatch_hit_count() const { return snatch_hit_count.load(std::memory_order_relaxed); }
    uint64_t get_snatch_miss_count() const { return snatch_miss_count.load(std::memory_order_relaxed); }
    uint64_t get_split_count() const { return split_count.load(std::memory_order_relaxed); }
    uint64_t get_coalesced_merge_count() const { return coalesced_merge_count.load(std::memory_order_relaxed); }
    size_t   get_total_rebated_bytes() const { return total_rebated_bytes.load(std::memory_order_relaxed); }
    float    snatch_hit_ratio() const {
        uint64_t hits = snatch_hit_count.load(std::memory_order_relaxed);
        uint64_t misses = snatch_miss_count.load(std::memory_order_relaxed);
        uint64_t total = hits + misses;
        return total > 0 ? (float)hits / (float)total : 0.0f;
    }
};

using sfFalline = sacculus_falline_t;

struct sf_slab_node_t {
    sf_slab_node_t* next;
};

struct sf_slab_class_t {
    size_t           item_size;
    sf_slab_node_t*  free_list_head = nullptr;
    std::atomic<size_t> allocated_count{0};
    std::atomic<size_t> free_count{0};
    sf_ticket_lock_t lock;

    void init(size_t sz) {
        item_size = sz;
        free_list_head = nullptr;
    }

    void* allocate_from_freelist(void) {
        sf_ticket_guard_t g(lock);
        if (!free_list_head) return nullptr;
        sf_slab_node_t* node = free_list_head;
        free_list_head = node->next;
        free_count.fetch_sub(1, std::memory_order_relaxed);
        allocated_count.fetch_add(1, std::memory_order_relaxed);
        return static_cast<void*>(node);
    }

    void deallocate(void* ptr) {
        if (!ptr) return;
        sf_ticket_guard_t g(lock);
        auto* node = static_cast<sf_slab_node_t*>(ptr);

        node->next = free_list_head;
        free_list_head = node;
        free_count.fetch_add(1, std::memory_order_relaxed);
        allocated_count.fetch_sub(1, std::memory_order_relaxed);
    }
};

struct sf_slab_allocator_t {
    static constexpr size_t NUM_CLASSES = 7;
    static constexpr size_t CLASS_SIZES[NUM_CLASSES] = {
        64, 256, 1024, 4096, 16384, 65536, 262144
    };

    sf_slab_class_t classes[NUM_CLASSES];

    uint8_t*              runway_heap_base   = nullptr;
    size_t                runway_capacity    = 0;
    std::atomic<size_t>   runway_bump_offset{0};
    sf_ticket_lock_t      bump_lock;
    sf_heap_superblock_t* superblock         = nullptr;
    bool                  always_pretouch    = false;
    bool                  forbid_dynamic_expansion = false;

    struct sf_chained_chunk_t {
        uint8_t*            base = nullptr;
        size_t              capacity = 0;
        std::atomic<size_t> bump_offset{0};
        sf_chained_chunk_t* next = nullptr;
    };
    std::mutex                chain_lock;
    sf_chained_chunk_t*       chained_head = nullptr;
    std::atomic<size_t>       chained_chunk_count{0};

    void* allocate_from_chained_chunks(size_t alloc_sz, size_t align) {
        if (forbid_dynamic_expansion) {

            return nullptr;
        }
        std::lock_guard<std::mutex> g(chain_lock);
        sf_chained_chunk_t* cur = chained_head;
        while (cur) {
            size_t off = cur->bump_offset.load(std::memory_order_relaxed);
            size_t aligned_off = (off + align - 1) & ~(align - 1);
            if (aligned_off + alloc_sz <= cur->capacity) {
                cur->bump_offset.store(aligned_off + alloc_sz, std::memory_order_relaxed);
                if (superblock) superblock->allocated_bytes.fetch_add(alloc_sz, std::memory_order_relaxed);
                return cur->base + aligned_off;
            }
            cur = cur->next;
        }

        size_t chunk_cap = alloc_sz * 2;
        if (chunk_cap < (64ULL * 1024 * 1024)) chunk_cap = 64ULL * 1024 * 1024;
        chunk_cap = (chunk_cap + 4095ULL) & ~4095ULL;

        uint8_t* new_base = (uint8_t*)sf_os_reserve_memory(chunk_cap);
        if (!new_base) return nullptr;
        if (!sf_os_commit_memory(new_base, chunk_cap)) {
            sf_os_release_memory(new_base, chunk_cap);
            return nullptr;
        }
        if (always_pretouch) {
            sf_pretouch_memory(new_base, chunk_cap);
        }

        auto* chunk = new (std::nothrow) sf_chained_chunk_t();
        if (!chunk) {
            sf_os_release_memory(new_base, chunk_cap);
            return nullptr;
        }
        chunk->base = new_base;
        chunk->capacity = chunk_cap;
        chunk->bump_offset.store(alloc_sz, std::memory_order_relaxed);
        chunk->next = chained_head;
        chained_head = chunk;
        chained_chunk_count.fetch_add(1, std::memory_order_relaxed);

        if (superblock) {
            superblock->total_reserved_bytes += chunk_cap;
            superblock->total_committed_bytes += chunk_cap;
            superblock->allocated_bytes.fetch_add(alloc_sz, std::memory_order_relaxed);
            superblock->next_chained_block = new_base;
        }

        return new_base;
    }

    bool init(size_t total_runway_bytes = 1024ULL * 1024 * 1024, bool pretouch = false, bool forbid_expansion = false) {
        runway_capacity = total_runway_bytes;
        always_pretouch = pretouch;
        forbid_dynamic_expansion = forbid_expansion;
        for (size_t i = 0; i < NUM_CLASSES; ++i) {
            classes[i].init(CLASS_SIZES[i]);
        }

        while (runway_capacity >= 64ULL * 1024 * 1024) {
            runway_heap_base = (uint8_t*)sf_os_reserve_memory(runway_capacity);
            if (runway_heap_base) {
                if (sf_os_commit_memory(runway_heap_base, runway_capacity)) {
                    break;
                }
                sf_os_release_memory(runway_heap_base, runway_capacity);
                runway_heap_base = nullptr;
            }

            runway_capacity /= 2;
        }
        if (!runway_heap_base) return false;

        superblock = reinterpret_cast<sf_heap_superblock_t*>(runway_heap_base);
        memset(superblock, 0, sizeof(sf_heap_superblock_t));
        superblock->magic = SF_HEAP_MAGIC;
        superblock->version = 1;
        superblock->total_reserved_bytes = runway_capacity;
        superblock->total_committed_bytes = runway_capacity;
        superblock->page_size = 4096;
        superblock->alignment = SACFOR_CACHE_LINE;

        size_t header_sz = sizeof(sf_heap_superblock_t);
        header_sz = (header_sz + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);

        superblock->slabs_offset = header_sz;
        superblock->slabs_capacity = runway_capacity / 4;
        superblock->bump_arena_offset = header_sz;
        superblock->bump_arena_capacity = runway_capacity * 3 / 4;
        superblock->los_offset = runway_capacity * 3 / 4;
        superblock->los_capacity = runway_capacity / 4;

        runway_bump_offset.store(header_sz, std::memory_order_relaxed);

        if (always_pretouch) {
            sf_pretouch_memory(runway_heap_base, runway_capacity);
        }
        return true;
    }

    void destroy(void) {
        {
            std::lock_guard<std::mutex> g(chain_lock);
            sf_chained_chunk_t* ch = chained_head;
            while (ch) {
                sf_chained_chunk_t* nxt = ch->next;
                if (ch->base) sf_os_release_memory(ch->base, ch->capacity);
                delete ch;
                ch = nxt;
            }
            chained_head = nullptr;
            chained_chunk_count.store(0, std::memory_order_relaxed);
        }
        if (!runway_heap_base) return;
        sf_os_release_memory(runway_heap_base, runway_capacity);
        runway_heap_base = nullptr;
        superblock = nullptr;
    }

    int find_size_class(size_t bytes) const {
        for (size_t i = 0; i < NUM_CLASSES; ++i) {
            if (bytes <= CLASS_SIZES[i]) return (int)i;
        }
        return -1;
    }

    void* allocate(size_t bytes, size_t align = SACFOR_CACHE_LINE) {
        if (bytes == 0) return nullptr;

        int sc = find_size_class(bytes);
        if (sc >= 0) {
            void* recycled = classes[sc].allocate_from_freelist();
            if (recycled) {
                if (superblock) superblock->allocated_bytes.fetch_add(CLASS_SIZES[sc], std::memory_order_relaxed);
                return recycled;
            }
            bytes = CLASS_SIZES[sc];
        }

        size_t alloc_sz = (bytes + align - 1) & ~(align - 1);

        sf_ticket_guard_t guard(bump_lock);
        size_t cur = runway_bump_offset.load(std::memory_order_relaxed);
        size_t aligned_cur = (cur + align - 1) & ~(align - 1);

        if (aligned_cur + alloc_sz > runway_capacity) {
            if (forbid_dynamic_expansion) {
                return nullptr;
            }
            return allocate_from_chained_chunks(alloc_sz, align);
        }

        runway_bump_offset.store(aligned_cur + alloc_sz, std::memory_order_relaxed);
        if (superblock) superblock->allocated_bytes.fetch_add(alloc_sz, std::memory_order_relaxed);
        return runway_heap_base + aligned_cur;
    }

    void deallocate(void* ptr, size_t bytes) {
        if (!ptr || !runway_heap_base) return;
        int sc = find_size_class(bytes);
        if (sc >= 0) {

            classes[sc].deallocate(ptr);
            if (superblock) superblock->allocated_bytes.fetch_sub(CLASS_SIZES[sc], std::memory_order_relaxed);
        } else {
            if (superblock) superblock->allocated_bytes.fetch_sub(bytes, std::memory_order_relaxed);
        }
    }

    size_t rebate_pages(sf_snatch_rebate_mode_e mode, size_t threshold_bytes = 64 * 1024) {
        if (forbid_dynamic_expansion || mode == sf_snatch_rebate_mode_e::LAZY || !runway_heap_base) {
            return 0;
        }

        sf_ticket_guard_t guard(bump_lock);
        size_t cur_used = runway_bump_offset.load(std::memory_order_relaxed);
        if (runway_capacity > cur_used) {
            size_t unused = runway_capacity - cur_used;
            if (unused >= threshold_bytes) {
                size_t page_aligned = unused & ~4095ULL;
                uint8_t* target = runway_heap_base + cur_used;
                sf_os_decommit_memory(target, page_aligned);
                if (superblock) {
                    superblock->rebated_bytes.fetch_add(page_aligned, std::memory_order_relaxed);
                    superblock->rebate_cycle_count.fetch_add(1, std::memory_order_relaxed);
                }
                return page_aligned;
            }
        }
        return 0;
    }

    size_t get_used_bytes(void) const {
        size_t off = runway_bump_offset.load(std::memory_order_relaxed);
        size_t hdr = sizeof(sf_heap_superblock_t);
        hdr = (hdr + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        return (off > hdr) ? (off - hdr) : 0;
    }
};

struct sacculus_runway_t {
    sacculus_motherline_t motherline;
    sacculus_falline_t    falline;
    sf_slab_allocator_t   allocator;

    std::atomic<size_t> pacing_stalls{0};
    void* gc_ref = nullptr;
    void pace_allocation(void);
    SF_custominiVm_emptybox_header_t* snatch_emptybox(size_t cell_sz);

    bool init(size_t handsel_bytes = 1024ULL * 1024 * 1024, bool always_pretouch = false, bool forbid_expansion = false) {
        motherline.init();
        return allocator.init(handsel_bytes, always_pretouch, forbid_expansion);
    }

    void* allocate_cell(size_t body_bytes, uint32_t fn_id = 0, const char* name = nullptr) {
        size_t cell_sz = SF_custominiVm_live_header_t::total_cell_size(body_bytes);

        pace_allocation();

        auto* eb = snatch_emptybox(cell_sz);
        SF_custominiVm_live_header_t* obj = nullptr;
        if (eb) {

            obj = reinterpret_cast<SF_custominiVm_live_header_t*>(eb);
            obj->init(fn_id, 0, name ? name : "snatched_tensor");
            obj->memory_footprint.store(body_bytes, std::memory_order_relaxed);
            obj->total_cell_bytes = cell_sz;
            obj->writeTailCanary();
        } else {

            void* raw = allocator.allocate(cell_sz, SACFOR_CACHE_LINE);
            if (!raw) return nullptr;
            obj = reinterpret_cast<SF_custominiVm_live_header_t*>(raw);
            obj->init(fn_id, 0, name ? name : "runway_tensor");
            obj->memory_footprint.store(body_bytes, std::memory_order_relaxed);
            obj->total_cell_bytes = cell_sz;
            obj->writeTailCanary();
        }

        motherline.admit(obj);
        return obj->body();
    }

    void free_cell(void* body_ptr, size_t body_bytes);

    void destroy(void) {
        allocator.destroy();
    }
};

using sfRunway = sacculus_runway_t;

struct sf_folder_node_t {
    char name[64] = {0};
    char full_path[192] = {0};
    size_t quota_bytes = 0;
    std::atomic<size_t> used_bytes{0};

    sf_folder_node_t* parent = nullptr;
    std::vector<sf_folder_node_t*> children;
    std::vector<SF_custominiVm_live_header_t*> objects;

    ~sf_folder_node_t() {
        for (auto* c : children) delete c;
        children.clear();
    }
};

struct sacculus_folder_tree_t {
    sf_folder_node_t* root = nullptr;
    std::mutex tree_lock;

    void init() {
        {
            std::lock_guard<std::mutex> g(tree_lock);
            if (root) {
                delete root;
                root = nullptr;
            }
            root = new (std::nothrow) sf_folder_node_t;
            if (root) {
                strncpy(root->name, "/", sizeof(root->name) - 1);
                strncpy(root->full_path, "/", sizeof(root->full_path) - 1);
            }
        }

        mkdir_p("/heap/on_heap/runway/motherline/young/tank");
        mkdir_p("/heap/on_heap/runway/motherline/young/demand");
        mkdir_p("/heap/on_heap/runway/motherline/young/middle");
        mkdir_p("/heap/on_heap/runway/motherline/young/hotfield");
        mkdir_p("/heap/on_heap/runway/motherline/adolescent/tank");
        mkdir_p("/heap/on_heap/runway/motherline/adolescent/demand");
        mkdir_p("/heap/on_heap/runway/motherline/adolescent/middle");
        mkdir_p("/heap/on_heap/runway/motherline/adolescent/hotfield");
        mkdir_p("/heap/on_heap/runway/motherline/senior/tank");
        mkdir_p("/heap/on_heap/runway/motherline/senior/demand");
        mkdir_p("/heap/on_heap/runway/motherline/senior/middle");
        mkdir_p("/heap/on_heap/runway/motherline/senior/hotfield");
        mkdir_p("/heap/on_heap/runway/falline");
        mkdir_p("/heap/on_heap/puzzle_arena/stagnant");
        mkdir_p("/heap/on_heap/puzzle_arena/eval");
        mkdir_p("/heap/on_heap/landfill/trash_pit/matured");
        mkdir_p("/heap/on_heap/landfill/trash_pit/rehab");
        mkdir_p("/heap/on_heap/landfill/header_pane");
        mkdir_p("/heap/off_heap/code_boot/rawcode_cell");
        mkdir_p("/heap/off_heap/code_boot/processed_code_chunk");
        mkdir_p("/heap/off_heap/log_boot/hotlog");
        mkdir_p("/heap/off_heap/log_boot/coldlog");
        mkdir_p("/heap/off_heap/reserved_boot");
    }

    void destroy() {
        std::lock_guard<std::mutex> g(tree_lock);
        delete root;
        root = nullptr;
    }

    static void split_path(const char* path, std::vector<std::string>& parts) {
        if (!path) return;
        const char* p = path;
        while (*p == '/' || *p == '\\') ++p;
        while (*p) {
            const char* start = p;
            while (*p && *p != '/' && *p != '\\') ++p;
            if (p > start) {
                parts.emplace_back(start, p - start);
            }
            while (*p == '/' || *p == '\\') ++p;
        }
    }

    sf_folder_node_t* mkdir_p(const char* path, size_t quota_bytes = 0) {
        if (!path || !root) return nullptr;
        std::lock_guard<std::mutex> g(tree_lock);
        std::vector<std::string> parts;
        split_path(path, parts);
        if (parts.empty()) return root;

        sf_folder_node_t* curr = root;
        std::string cur_path = "";
        for (const auto& part : parts) {
            cur_path += "/" + part;

            sf_folder_node_t* found = nullptr;
            for (auto* child : curr->children) {
                if (strcmp(child->name, part.c_str()) == 0) {
                    found = child;
                    break;
                }
            }
            if (!found) {
                found = new (std::nothrow) sf_folder_node_t;
                if (!found) return nullptr;
                strncpy(found->name, part.c_str(), sizeof(found->name) - 1);
                strncpy(found->full_path, cur_path.c_str(), sizeof(found->full_path) - 1);
                found->parent = curr;
                curr->children.push_back(found);
            }
            curr = found;
        }
        if (curr && quota_bytes > 0) {
            curr->quota_bytes = quota_bytes;
        }
        return curr;
    }

    sf_folder_node_t* _find_node_unlocked(const char* path) {
        if (!path || !root) return nullptr;
        std::vector<std::string> parts;
        split_path(path, parts);
        if (parts.empty()) return root;

        sf_folder_node_t* curr = root;
        for (const auto& part : parts) {

            sf_folder_node_t* found = nullptr;
            for (auto* child : curr->children) {
                if (strcmp(child->name, part.c_str()) == 0) {
                    found = child;
                    break;
                }
            }
            if (!found) return nullptr;
            curr = found;
        }
        return curr;
    }

    sf_folder_node_t* find_node(const char* path) {
        std::lock_guard<std::mutex> g(tree_lock);
        return _find_node_unlocked(path);
    }

    bool bind_tensor(const char* folder_path, SF_custominiVm_live_header_t* obj) {
        if (!folder_path || !obj) return false;
        sf_folder_node_t* node = mkdir_p(folder_path);
        if (!node) return false;

        size_t footprint = (size_t)obj->memory_footprint.load(std::memory_order_relaxed);

        if (node->quota_bytes > 0 && (node->used_bytes.load() + footprint > node->quota_bytes)) {
            return false;
        }
        node->objects.push_back(obj);
        node->used_bytes.fetch_add(footprint, std::memory_order_relaxed);
        return true;
    }

    size_t evict_folder(const char* folder_path, sacculus_runway_t* runway) {
        std::vector<std::pair<void*, size_t>> to_free;
        {
            std::lock_guard<std::mutex> g(tree_lock);
            sf_folder_node_t* target = _find_node_unlocked(folder_path);
            if (!target || !runway) return 0;

            std::vector<sf_folder_node_t*> stack;
            stack.push_back(target);

            while (!stack.empty()) {
                sf_folder_node_t* curr = stack.back();
                stack.pop_back();

                for (auto* child : curr->children) {
                    stack.push_back(child);
                }

                for (auto* obj : curr->objects) {
                    if (obj) {
                        size_t sz = (size_t)obj->memory_footprint.load(std::memory_order_relaxed);
                        to_free.push_back({obj->body(), sz});
                    }
                }
                curr->objects.clear();
                curr->used_bytes.store(0, std::memory_order_relaxed);
            }
        }

        size_t freed_total = 0;
        for (auto& item : to_free) {
            runway->free_cell(item.first, item.second);
            freed_total += item.second;
        }
        return freed_total;
    }

    size_t get_used_bytes(const char* folder_path) {
        std::lock_guard<std::mutex> g(tree_lock);
        sf_folder_node_t* node = _find_node_unlocked(folder_path);
        if (!node) return 0;
        return node->used_bytes.load(std::memory_order_relaxed);
    }

    bool unlink_file(const char* file_path, sacculus_runway_t* runway) {
        if (!file_path || !runway) return false;
        std::vector<std::string> parts;
        split_path(file_path, parts);
        if (parts.empty()) return false;

        std::string filename = parts.back();
        parts.pop_back();

        std::string dir_path = "/";
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i > 0) dir_path += "/";
            dir_path += parts[i];
        }

        sf_folder_node_t* node = find_node(dir_path.c_str());
        if (!node) return false;

        for (auto it = node->objects.begin(); it != node->objects.end(); ++it) {
            SF_custominiVm_live_header_t* obj = *it;
            if (obj && strcmp(get_cold(obj)->debug_name, filename.c_str()) == 0) {
                size_t sz = (size_t)obj->memory_footprint.load(std::memory_order_relaxed);
                runway->free_cell(obj->body(), sz);
                node->used_bytes.fetch_sub(sz, std::memory_order_relaxed);
                node->objects.erase(it);
                return true;
            }
        }
        return false;
    }

    bool stat_path(const char* path, SF_custominiVm_vfs_stat_t* out_stat) {
        if (!path || !out_stat) return false;
        memset(out_stat, 0, sizeof(*out_stat));

        std::lock_guard<std::mutex> g(tree_lock);
        sf_folder_node_t* node = _find_node_unlocked(path);
        if (node) {

            out_stat->inode_no = reinterpret_cast<uint64_t>(node);
            out_stat->is_dir = 1;
            out_stat->quota_bytes = node->quota_bytes;
            out_stat->used_bytes = node->used_bytes.load(std::memory_order_relaxed);
            out_stat->size_bytes = out_stat->used_bytes;
            out_stat->child_count = (uint32_t)(node->children.size() + node->objects.size());
            return true;
        }

        std::vector<std::string> parts;
        split_path(path, parts);
        if (parts.empty()) return false;

        std::string filename = parts.back();
        parts.pop_back();

        std::string dir_path = "/";
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i > 0) dir_path += "/";
            dir_path += parts[i];
        }

        sf_folder_node_t* pnode = find_node(dir_path.c_str());
        if (!pnode) return false;

        for (auto* obj : pnode->objects) {
            if (obj && strcmp(get_cold(obj)->debug_name, filename.c_str()) == 0) {
                out_stat->inode_no = reinterpret_cast<uint64_t>(obj);
                out_stat->is_dir = 0;
                out_stat->size_bytes = (size_t)obj->memory_footprint.load(std::memory_order_relaxed);
                out_stat->used_bytes = out_stat->size_bytes;
                return true;
            }
        }
        return false;
    }

    size_t snapshot_path(const char* path, void* out_buf, size_t cap) {
        if (!path || !out_buf || cap == 0) return 0;
        std::vector<std::string> parts;
        split_path(path, parts);
        if (parts.empty()) return 0;

        std::string filename = parts.back();
        parts.pop_back();

        std::string dir_path = "/";
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i > 0) dir_path += "/";
            dir_path += parts[i];
        }

        sf_folder_node_t* pnode = find_node(dir_path.c_str());
        if (!pnode) return 0;

        SF_custominiVm_live_header_t* target_obj = nullptr;
        {

            for (auto* obj : pnode->objects) {
                if (obj && strcmp(get_cold(obj)->debug_name, filename.c_str()) == 0) {
                    target_obj = obj;
                    break;
                }
            }
        }
        if (!target_obj) return 0;

        if (!target_obj->try_pin()) return 0;
        size_t sz = (size_t)target_obj->memory_footprint.load(std::memory_order_relaxed);
        size_t to_copy = (sz < cap) ? sz : cap;
        memcpy(out_buf, target_obj->body(), to_copy);
        target_obj->unpin();
        return to_copy;
    }

    void list_dir(const char* path, void (*cb)(const char*, int, size_t, void*), void* user) {
        if (!path || !cb) return;
        sf_folder_node_t* node = find_node(path);
        if (!node) return;

        for (auto* child : node->children) {
            cb(child->name, 1, child->used_bytes.load(std::memory_order_relaxed), user);
        }
        for (auto* obj : node->objects) {
            if (obj) {
                cb(get_cold(obj)->debug_name, 0, (size_t)obj->memory_footprint.load(std::memory_order_relaxed), user);
            }
        }
    }
};
using sfFolderTree = sacculus_folder_tree_t;
using sfVfsHeap = sacculus_folder_tree_t;
using sacculus_vfs_heap_t = sacculus_folder_tree_t;

typedef enum sf_gc_phase_e : uint8_t {
    SF_GC_PHASE_IDLE      = 0,
    SF_GC_PHASE_SWEEP     = 1,
    SF_GC_PHASE_DRAIN     = 2,
    SF_GC_PHASE_REHAB     = 3,
    SF_GC_PHASE_COMPACT   = 4,
    SF_GC_PHASE_EVACUATE  = 5,
} sf_gc_phase_e;

struct sf_pagefile_t {
    SF_custominiVm_paging_policy_t policy = SF_PAGING_FIFO;
    std::atomic<size_t>   paged_out_bytes{0};
    std::atomic<size_t>   paged_out_count{0};
    size_t                capacity_bytes{256ULL * 1024 * 1024};
    std::atomic<uint64_t> write_offset{0};
    std::mutex            lock;

#ifdef _WIN32
    HANDLE file_handle = INVALID_HANDLE_VALUE;
#else
    int    file_fd = -1;
#endif
    void*  memory_fallback = nullptr;

    bool init(size_t max_bytes, SF_custominiVm_paging_policy_t pol = SF_PAGING_FIFO) {
        std::lock_guard<std::mutex> g(lock);
        capacity_bytes = max_bytes > 0 ? max_bytes : (256ULL * 1024 * 1024);
        policy = pol;
        write_offset.store(0, std::memory_order_relaxed);

#ifdef _WIN32
        if (file_handle != INVALID_HANDLE_VALUE) {
            CloseHandle(file_handle);
            file_handle = INVALID_HANDLE_VALUE;
        }
        char temp_path[MAX_PATH];
        DWORD tp_len = GetTempPathA(MAX_PATH, temp_path);
        if (tp_len > 0 && tp_len < MAX_PATH) {
            char filename[MAX_PATH];
            snprintf(filename, sizeof(filename), "%s\\sfvm_hidden_pagefile_%lu.tmp", temp_path, (unsigned long)GetCurrentProcessId());
            file_handle = CreateFileA(filename,
                                      GENERIC_READ | GENERIC_WRITE,
                                      0,
                                      NULL,
                                      CREATE_ALWAYS,
                                      FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                                      NULL);
        }
        if (file_handle == INVALID_HANDLE_VALUE) {
            if (!memory_fallback) {
                memory_fallback = VirtualAlloc(NULL, capacity_bytes, MEM_RESERVE, PAGE_READWRITE);
            }
        }
#else
        if (file_fd >= 0) {
            close(file_fd);
            file_fd = -1;
        }
#if defined(__linux__) && defined(MFD_CLOEXEC)
        file_fd = memfd_create("sfvm_anonymous_pagefile", MFD_CLOEXEC);
#endif
        if (file_fd < 0) {
#if defined(O_TMPFILE)
            file_fd = open("/tmp", O_TMPFILE | O_RDWR, 0600);
#endif
        }
        if (file_fd < 0 && !memory_fallback) {
            memory_fallback = mmap(NULL, capacity_bytes, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        }
#endif
        return true;
    }

    void destroy() {
        std::lock_guard<std::mutex> g(lock);
#ifdef _WIN32
        if (file_handle != INVALID_HANDLE_VALUE) {
            CloseHandle(file_handle);
            file_handle = INVALID_HANDLE_VALUE;
        }
        if (memory_fallback) {
            VirtualFree(memory_fallback, 0, MEM_RELEASE);
            memory_fallback = nullptr;
        }
#else
        if (file_fd >= 0) {
            close(file_fd);
            file_fd = -1;
        }
        if (memory_fallback) {
            munmap(memory_fallback, capacity_bytes);
            memory_fallback = nullptr;
        }
#endif
        paged_out_bytes.store(0, std::memory_order_relaxed);
        paged_out_count.store(0, std::memory_order_relaxed);
    }

    bool page_out(SF_custominiVm_live_header_t* obj) {
        if (!obj || obj->sfMN != SF_custominiVm_live_header_t::SF_LIVE_MAGIC) return false;
        if (obj->is_pinned() || obj->child_view_count.load(std::memory_order_acquire) > 0) return false;
        auto* cold = get_cold(obj);
        if (!cold) return false;
        if (cold->is_paged_out.load(std::memory_order_acquire)) return true;

        size_t fp = (size_t)obj->memory_footprint.load(std::memory_order_relaxed);
        if (fp == 0) return false;

        std::lock_guard<std::mutex> g(lock);

        uint64_t slot_off;
        {
            uint64_t cur = write_offset.load(std::memory_order_relaxed);
            for (;;) {
                if (cur + fp > capacity_bytes) return false;
                if (write_offset.compare_exchange_weak(cur, cur + fp,
                        std::memory_order_relaxed, std::memory_order_relaxed)) {
                    slot_off = cur;
                    break;
                }
            }
        }

        bool written = false;
#ifdef _WIN32
        if (file_handle != INVALID_HANDLE_VALUE) {
            LARGE_INTEGER li;
            li.QuadPart = (LONGLONG)slot_off;
            SetFilePointerEx(file_handle, li, NULL, FILE_BEGIN);
            DWORD written_bytes = 0;
            if (WriteFile(file_handle, obj->body(), (DWORD)fp, &written_bytes, NULL) && written_bytes == fp) {
                written = true;
            }
        }
#else
        if (file_fd >= 0) {
            if (pwrite(file_fd, obj->body(), fp, slot_off) == (ssize_t)fp) {
                written = true;
            }
        }
#endif
        if (!written && memory_fallback) {
            uint8_t* dst = static_cast<uint8_t*>(memory_fallback) + slot_off;
            sf_os_commit_memory(dst, fp);
            memcpy(dst, obj->body(), fp);
            written = true;
        }

        if (written) {
            cold->pagefile_slot_offset = slot_off;
            cold->is_paged_out.store(1, std::memory_order_release);
            paged_out_bytes.fetch_add(fp, std::memory_order_relaxed);
            paged_out_count.fetch_add(1, std::memory_order_relaxed);

            sf_os_decommit_memory(obj->body(), fp);
            return true;
        }
        return false;
    }

    bool page_in(SF_custominiVm_live_header_t* obj) {
        if (!obj || obj->sfMN != SF_custominiVm_live_header_t::SF_LIVE_MAGIC) return false;
        auto* cold = get_cold(obj);
        if (!cold) return true;
        if (!cold->is_paged_out.load(std::memory_order_acquire)) return true;

        size_t fp = (size_t)obj->memory_footprint.load(std::memory_order_relaxed);
        if (fp == 0) return false;

        std::lock_guard<std::mutex> g(lock);

        sf_os_commit_memory(obj->body(), fp);

        bool read_ok = false;
        uint64_t slot_off = cold->pagefile_slot_offset;

#ifdef _WIN32
        if (file_handle != INVALID_HANDLE_VALUE) {
            LARGE_INTEGER li;
            li.QuadPart = (LONGLONG)slot_off;
            SetFilePointerEx(file_handle, li, NULL, FILE_BEGIN);
            DWORD read_bytes = 0;
            if (ReadFile(file_handle, obj->body(), (DWORD)fp, &read_bytes, NULL) && read_bytes == fp) {
                read_ok = true;
            }
        }
#else
        if (file_fd >= 0) {
            if (pread(file_fd, obj->body(), fp, slot_off) == (ssize_t)fp) {
                read_ok = true;
            }
        }
#endif
        if (!read_ok && memory_fallback) {
            uint8_t* src = static_cast<uint8_t*>(memory_fallback) + slot_off;
            memcpy(obj->body(), src, fp);
            read_ok = true;
        }

        if (read_ok) {
            obj->writeTailCanary();
            cold->is_paged_out.store(0, std::memory_order_release);
            size_t pob = paged_out_bytes.load(std::memory_order_relaxed);
            if (pob >= fp) paged_out_bytes.fetch_sub(fp, std::memory_order_relaxed);
            size_t poc = paged_out_count.load(std::memory_order_relaxed);
            if (poc > 0) paged_out_count.fetch_sub(1, std::memory_order_relaxed);
            return true;
        }
        return false;
    }
};

struct sacculus_gc_t {
    sacculus_runway_t      runway;
    sacculus_motherline_t& motherline = runway.motherline;
    sacculus_falline_t&    falline    = runway.falline;
    sacculus_landfill_t    landfill;
    sacculus_hotlog_t*     hotlog;
    sacculus_coldlog_t*    coldlog;
    sf_pagefile_t          pagefile;

    std::thread            gc_thread;
    std::atomic<bool>      running{false};
    std::atomic<uint64_t>  sweep_count{0};
    std::atomic<size_t>    total_heap_bytes{0};
    size_t                 pressure_threshold;

    alignas(64) std::atomic<uint8_t> current_phase{SF_GC_PHASE_IDLE};
    alignas(64) std::atomic<uint64_t> matured_flush_count{0};
    alignas(64) std::atomic<uint64_t> rehab_scan_count{0};
    alignas(64) std::atomic<uint64_t> resurrection_count{0};
    alignas(64) std::atomic<uint64_t> falline_compact_count{0};
    std::atomic<uint64_t>  last_sweep_ns{0};
    std::atomic<uint64_t>  total_calls_tracked{0};

    struct sf_step_result_t {
        size_t swept = 0;
        size_t drained = 0;
        size_t resurrected = 0;
        size_t rebated = 0;
    };

    struct sf_env_snapshot_t {
        size_t   runway_bump = 0;
        size_t   allocated_bytes = 0;
        size_t   chained_chunks = 0;
        size_t   pacing_stalls = 0;
        uint64_t total_calls = 0;
        uint64_t resurrections = 0;
        size_t   paged_out_count = 0;
        size_t   total_heap = 0;
    };

    sf_env_snapshot_t capture_env(void) const {
        sf_env_snapshot_t s{};
        s.runway_bump = runway.allocator.runway_bump_offset.load(std::memory_order_relaxed);
        if (runway.allocator.superblock) {
            s.allocated_bytes = runway.allocator.superblock->allocated_bytes.load(std::memory_order_relaxed);
        }
        s.chained_chunks = runway.allocator.chained_chunk_count.load(std::memory_order_relaxed);
        s.pacing_stalls = runway.pacing_stalls.load(std::memory_order_relaxed);
        s.resurrections = resurrection_count.load(std::memory_order_relaxed);
        s.paged_out_count = pagefile.paged_out_count.load(std::memory_order_relaxed);
        s.total_heap = total_heap_bytes.load(std::memory_order_relaxed);
        s.total_calls = total_calls_tracked.load(std::memory_order_relaxed);
        return s;
    }

    bool has_env_changed(const sf_env_snapshot_t& prev, const sf_env_snapshot_t& cur) const {
        return (cur.runway_bump != prev.runway_bump) ||
               (cur.allocated_bytes != prev.allocated_bytes) ||
               (cur.chained_chunks != prev.chained_chunks) ||
               (cur.pacing_stalls != prev.pacing_stalls) ||
               (cur.total_calls != prev.total_calls) ||
               (cur.resurrections != prev.resurrections) ||
               (cur.paged_out_count != prev.paged_out_count);
    }

    static constexpr uint32_t RESURRECT_QUEUE_CAP = 64u;
    std::atomic<uint32_t> resurrect_queue[RESURRECT_QUEUE_CAP];
    std::atomic<uint32_t> resurrect_write{0};
    std::atomic<uint32_t> resurrect_read{0};

    sf_vm_config_t         config;

    void (*on_free)(sacculus_live_header_t*, void*);
    void* on_free_user;

    SF_custominiVm_gc_hook_fn_t  gc_hook = nullptr;
    void*                        gc_hook_user = nullptr;

    SF_custominiVm_gc_manifest_t last_manifest{};
    std::mutex                   manifest_lock;

    inline void trigger_hook(SF_custominiVm_gc_hook_event_e ev) {
        if (gc_hook) {
            gc_hook(ev, sweep_count.load(std::memory_order_relaxed),
                    total_heap_bytes.load(std::memory_order_relaxed),
                    gc_hook_user);
        }
    }

    void init(sacculus_hotlog_t* hl, sacculus_coldlog_t* cl,
              const sf_vm_config_t& cfg) {
        hotlog   = hl;
        coldlog  = cl;
        config   = cfg;
        g_sf_critical_simd_mode.store(static_cast<uint8_t>(cfg.strings.critical_simd_mode), std::memory_order_relaxed);
        pressure_threshold = cfg.integers.pressure_threshold_bytes;
        on_free  = nullptr;
        on_free_user = nullptr;
        runway.gc_ref = this;
        pagefile.init(256ULL * 1024 * 1024, SF_PAGING_FIFO);
        current_phase.store(SF_GC_PHASE_IDLE, std::memory_order_relaxed);
        matured_flush_count.store(0, std::memory_order_relaxed);
        rehab_scan_count.store(0, std::memory_order_relaxed);
        resurrection_count.store(0, std::memory_order_relaxed);
        falline_compact_count.store(0, std::memory_order_relaxed);
        last_sweep_ns.store(0, std::memory_order_relaxed);

        for (uint32_t i = 0; i < RESURRECT_QUEUE_CAP; ++i) {
            resurrect_queue[i].store(0, std::memory_order_relaxed);
        }
        resurrect_write.store(0, std::memory_order_relaxed);
        resurrect_read.store(0, std::memory_order_relaxed);

        runway.init(cfg.integers.runway_handsel_bytes, cfg.strings.always_pretouch, cfg.is_safety_critical);
        landfill.init(cfg.integers.landfill_handsel_bytes, cfg.strings.always_pretouch);
    }

    void admit(sacculus_live_header_t* obj) {

        sf_sync_store_fence();

        total_heap_bytes.fetch_add((size_t)obj->memory_footprint.load(std::memory_order_relaxed),
                                   std::memory_order_relaxed);
        motherline.admit(obj);
        sacculus_hotlog_write(hotlog, SF_Ebin0_ALLOC, SF_Ebin1_TRACE,
            obj->function_id, 0, obj->generation, obj->region,
            obj->current_core.load(std::memory_order_relaxed), sweep_count.load(std::memory_order_relaxed),
            (uint32_t)(total_heap_bytes.load(std::memory_order_relaxed) / 1024),
            sf_get_debug_name(obj));
    }

    void request_resurrection(uint32_t fn_id) {
        uint32_t w = resurrect_write.fetch_add(1, std::memory_order_relaxed) & (RESURRECT_QUEUE_CAP - 1);
        resurrect_queue[w].store(fn_id, std::memory_order_release);
    }

    void mark_garbage(sacculus_live_header_t* obj) {
        if (obj->is_pinned()) return;
        if (obj->falline_marked) return;
        motherline.evict(obj);
        falline.push(obj);
        sacculus_hotlog_write(hotlog, SF_Ebin0_TO_FALLINE, SF_Ebin1_INFO,
            obj->function_id, 0, obj->generation, obj->region,
            obj->current_core.load(), sweep_count.load(),
            (uint32_t)(total_heap_bytes.load() / 1024), "mark_garbage");
    }

    size_t drain_falline(void) {
        current_phase.store(SF_GC_PHASE_DRAIN, std::memory_order_relaxed);
        size_t drained_count = 0;
        while (true) {
            sacculus_live_header_t* obj = falline.pop();
            if (!obj) break;
            ++drained_count;

            if (landfill.is_critical()) {
                current_phase.store(SF_GC_PHASE_EVACUATE, std::memory_order_relaxed);
                sacculus_coldlog_write(coldlog, SF_Ebin0_LANDFILL_EVAC,
                    SF_Ebin1_WARN, 0, "landfill_critical");
                landfill.evacuate(hotlog, sweep_count.load());
                matured_flush_count.fetch_add(1, std::memory_order_relaxed);
                current_phase.store(SF_GC_PHASE_DRAIN, std::memory_order_relaxed);
            }

            sf_death_reason_e reason = SF_DEATH_IDLE;
            auto* cold = get_cold_ro(obj);
            if (cold && cold->error_count.load(std::memory_order_relaxed) > 10) {
                reason = SF_DEATH_ERROR;
            }

            landfill.accept(obj, reason, hotlog, sweep_count.load());

            if (on_free) on_free(obj, on_free_user);

            size_t fp  = (size_t)obj->memory_footprint.load(std::memory_order_relaxed);
            size_t cur = total_heap_bytes.load(std::memory_order_relaxed);
            if (cur >= fp) total_heap_bytes.fetch_sub(fp, std::memory_order_relaxed);

            auto* p = obj->parent_header.load(std::memory_order_acquire);
            if (p) {
                p->child_view_count.fetch_sub(1, std::memory_order_acq_rel);
                obj->parent_header.store(nullptr, std::memory_order_release);
            }

            if (fp > 0) {
                sf_critical_clear_memory(obj->body(), fp);
            }
            size_t cell_sz = SF_custominiVm_live_header_t::total_cell_size(fp);
            auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(obj);
            falline.register_emptybox(obj->function_id, fp, cell_sz, eb);
        }
        return drained_count;
    }

    size_t sweep_motherline_cycles(void) {
        current_phase.store(SF_GC_PHASE_SWEEP, std::memory_order_relaxed);
        trigger_hook(SF_GC_HOOK_PRE_SWEEP);
        size_t evicted_count = 0;

        size_t cur_bytes = total_heap_bytes.load(std::memory_order_relaxed);
        float pressure_ratio = pressure_threshold > 0 ? ((float)cur_bytes / (float)pressure_threshold) : 0.0f;
        uint64_t sweep_no = sweep_count.load(std::memory_order_relaxed);
        uint32_t heap_kb = (uint32_t)(cur_bytes / 1024);

        for (int room_i = 0; room_i < SACFOR_ROOM_COUNT; ++room_i) {
            sacculus_room_t& room = motherline.rooms[room_i];

            for (int reg_i = SACFOR_REGION_COUNT - 1; reg_i >= 0; --reg_i) {
                sacculus_region_t& region = room.regions[reg_i];
                sf_ticket_guard_t g(region.get_lock());
                sacculus_live_header_t* cur = region.get_head();

                while (cur) {
                    sacculus_live_header_t* next_obj = cur->next;
                    cur->decay_heat();

                    if (cur->is_pinned()) { cur = next_obj; continue; }

                    if (cur->state.load(std::memory_order_acquire) == static_cast<uint8_t>(sf_live_state_e::QUARANTINED)) {
                        region.evict_nolock(cur);
                        ++evicted_count;
                        room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                        landfill.accept(cur, SF_DEATH_ERROR, hotlog, sweep_no);
                        if (on_free) on_free(cur, on_free_user);
                        size_t fp  = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                        size_t chb = total_heap_bytes.load(std::memory_order_relaxed);
                        if (chb >= fp) total_heap_bytes.fetch_sub(fp, std::memory_order_relaxed);
                        sacculus_hotlog_write(hotlog, SF_Ebin0_TO_FALLINE,
                            SF_Ebin1_ERROR, cur->function_id, 0,
                            cur->generation, cur->region,
                            cur->current_core.load(std::memory_order_relaxed), sweep_no, heap_kb,
                            "quarantine_evac");
                        cur = next_obj;
                        continue;
                    }

                    if (cur->needs_rehabilitation) {
                        if (cur->try_transition_to_rehab()) {
                            region.evict_nolock(cur);
                            ++evicted_count;
                            room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                            landfill.accept(cur, SF_DEATH_REHABILITATE, hotlog, sweep_no);
                            if (on_free) on_free(cur, on_free_user);
                            size_t fp  = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                            size_t chb = total_heap_bytes.load(std::memory_order_relaxed);
                            if (chb >= fp) total_heap_bytes.fetch_sub(fp, std::memory_order_relaxed);
                        }
                        cur = next_obj;
                        continue;
                    }

                    if (cur->is_idle()) {
                        if (cur->try_transition_to_falline()) {
                            region.evict_nolock(cur);
                            ++evicted_count;
                            room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                            falline.push(cur);
                            sacculus_hotlog_write(hotlog, SF_Ebin0_TO_FALLINE,
                                SF_Ebin1_INFO, cur->function_id, 0,
                                cur->generation, cur->region,
                                cur->current_core.load(std::memory_order_relaxed), sweep_no, heap_kb,
                                "idle_to_falline");
                        }
                        cur = next_obj;
                        continue;
                    }

                    bool drop_by_pressure = false;
                    const char* drop_reason = "pressure_drop";
                    float heat = cur->heat_score.load(std::memory_order_relaxed);

                    if (pressure_ratio >= config.floats.pressure_critical_ratio) {

                        if (heat < SACFOR_HEAT_HOT) {
                            drop_by_pressure = true;
                            drop_reason = "critical_pressure";
                        }
                    } else if (pressure_ratio >= config.floats.pressure_high_ratio) {

                        if (cur->generation <= SF_ROOM_ADOLESCENT && heat < SACFOR_HEAT_COLD * 2.0f) {
                            drop_by_pressure = true;
                            drop_reason = "high_pressure";
                        }
                    } else if (pressure_ratio >= config.floats.pressure_mild_ratio) {

                        if (cur->generation == SF_ROOM_YOUNG && heat < SACFOR_HEAT_COLD) {
                            drop_by_pressure = true;
                            drop_reason = "mild_pressure";
                        }
                    }

                    if (drop_by_pressure) {
                        if (cur->try_transition_to_falline()) {
                            region.evict_nolock(cur);
                            ++evicted_count;
                            room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                            falline.push(cur);
                            sacculus_hotlog_write(hotlog, SF_Ebin0_TO_FALLINE,
                                SF_Ebin1_WARN, cur->function_id, 0,
                                cur->generation, cur->region,
                                cur->current_core.load(std::memory_order_relaxed), sweep_no, heap_kb,
                                drop_reason);
                        }
                        cur = next_obj;
                        continue;
                    }

                    uint8_t young_max = SACFOR_GEN_YOUNG_MAX;
                    uint8_t adol_max  = SACFOR_GEN_ADOL_MAX;
                    if (pressure_ratio >= config.floats.pressure_high_ratio) {
                        young_max /= 2;
                        adol_max /= 2;
                    }
                    if (young_max < 2) young_max = 2;
                    if (adol_max < 4) adol_max = 4;

                    uint8_t new_gen = cur->generation;
                    if (cur->generation == SF_ROOM_YOUNG &&
                        cur->gc_sweep_count > young_max)
                        new_gen = SF_ROOM_ADOLESCENT;
                    else if (cur->generation == SF_ROOM_ADOLESCENT &&
                             cur->gc_sweep_count > adol_max)
                        new_gen = SF_ROOM_SENIOR;

                    if (new_gen != cur->generation) {
                        region.evict_nolock(cur);
                        room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                        sacculus_hotlog_write(hotlog, SF_Ebin0_PROMOTE,
                            SF_Ebin1_TRACE, cur->function_id, 0,
                            cur->generation, cur->region,
                            cur->current_core.load(std::memory_order_relaxed), sweep_no, heap_kb,
                            sf_room_names[new_gen]);
                        motherline.rooms[new_gen].place(cur);
                        cur = next_obj;
                        continue;
                    }

                    uint8_t target_reg = cur->compute_target_region();
                    if (target_reg != cur->region) {
                        region.evict_nolock(cur);
                        room.regions[target_reg].place(cur);
                        sacculus_hotlog_write(hotlog, SF_Ebin0_GC_ALIGN,
                            SF_Ebin1_TRACE, cur->function_id, 0,
                            cur->generation, target_reg,
                            cur->current_core.load(std::memory_order_relaxed), sweep_no, heap_kb,
                            sf_region_names[target_reg]);
                        cur = next_obj;
                        continue;
                    }

                    cur = next_obj;
                }
            }
        }
        trigger_hook(SF_GC_HOOK_POST_SWEEP);
        return evicted_count;
    }

    SF_DISABLE_OPT_BEGIN
    size_t sweep_motherline_simd(void) {
        current_phase.store(SF_GC_PHASE_SWEEP, std::memory_order_relaxed);
        trigger_hook(SF_GC_HOOK_PRE_SWEEP);
        size_t evicted_count = 0;

        size_t cur_bytes = total_heap_bytes.load(std::memory_order_relaxed);
        float pressure_ratio = pressure_threshold > 0 ? ((float)cur_bytes / (float)pressure_threshold) : 0.0f;
        uint64_t sweep_no = sweep_count.load(std::memory_order_relaxed);
        uint32_t heap_kb = (uint32_t)(cur_bytes / 1024);

        float decay_f = config.floats.heat_decay_factor;
        if (decay_f <= 0.0f || decay_f > 1.0f) decay_f = 0.97f;

        for (int room_i = 0; room_i < SACFOR_ROOM_COUNT; ++room_i) {
            sacculus_room_t& room = motherline.rooms[room_i];

            for (int reg_i = SACFOR_REGION_COUNT - 1; reg_i >= 0; --reg_i) {
                sacculus_region_t& region = room.regions[reg_i];
                sf_ticket_guard_t g(region.get_lock());
                sacculus_live_header_t* cur = region.get_head();

                static constexpr size_t SIMD_BATCH_SZ = 8;
                sacculus_live_header_t* batch[SIMD_BATCH_SZ];

                while (cur) {
                    size_t count = 0;
                    sacculus_live_header_t* walker = cur;
                    while (walker && count < SIMD_BATCH_SZ) {
                        batch[count++] = walker;
                        walker = walker->next;
                    }

                    if (count == 0) break;

#if defined(__AVX2__) || (defined(_M_X64) && defined(__AVX2__))
                    if (count == 8) {
                        __m256 v_heats = _mm256_set_ps(
                            batch[7]->heat_score.load(std::memory_order_relaxed),
                            batch[6]->heat_score.load(std::memory_order_relaxed),
                            batch[5]->heat_score.load(std::memory_order_relaxed),
                            batch[4]->heat_score.load(std::memory_order_relaxed),
                            batch[3]->heat_score.load(std::memory_order_relaxed),
                            batch[2]->heat_score.load(std::memory_order_relaxed),
                            batch[1]->heat_score.load(std::memory_order_relaxed),
                            batch[0]->heat_score.load(std::memory_order_relaxed)
                        );
                        __m256 v_decay = _mm256_set1_ps(decay_f);
                        __m256 v_res = _mm256_mul_ps(v_heats, v_decay);
                        float decayed[8];
                        _mm256_storeu_ps(decayed, v_res);
                        for (size_t b = 0; b < 8; ++b) {
                            batch[b]->heat_score.store(decayed[b], std::memory_order_relaxed);
                        }
                    } else {
                        for (size_t b = 0; b < count; ++b) batch[b]->decay_heat();
                    }
#elif defined(__aarch64__) || defined(_M_ARM64)
                    if (count >= 4) {
                        float32x4_t v_decay = vdupq_n_f32(decay_f);
                        for (size_t b = 0; b + 4 <= count; b += 4) {
                            float raw_h[4] = {
                                batch[b+0]->heat_score.load(std::memory_order_relaxed),
                                batch[b+1]->heat_score.load(std::memory_order_relaxed),
                                batch[b+2]->heat_score.load(std::memory_order_relaxed),
                                batch[b+3]->heat_score.load(std::memory_order_relaxed)
                            };
                            float32x4_t v_h = vld1q_f32(raw_h);
                            v_h = vmulq_f32(v_h, v_decay);
                            vst1q_f32(raw_h, v_h);
                            for (size_t k = 0; k < 4; ++k) {
                                batch[b+k]->heat_score.store(raw_h[k], std::memory_order_relaxed);
                            }
                        }
                        for (size_t b = (count & ~3ULL); b < count; ++b) batch[b]->decay_heat();
                    } else {
                        for (size_t b = 0; b < count; ++b) batch[b]->decay_heat();
                    }
#else
                    for (size_t b = 0; b < count; ++b) {
                        batch[b]->decay_heat();
                    }
#endif

                    uint32_t active_action_mask = 0;
                    for (size_t b = 0; b < count; ++b) {
                        auto* item = batch[b];
                        if (item->is_pinned()) continue;

                        uint8_t st = item->state.load(std::memory_order_acquire);
                        if (st == static_cast<uint8_t>(sf_live_state_e::QUARANTINED) ||
                            item->needs_rehabilitation ||
                            item->is_idle()) {
                            active_action_mask |= (1u << b);
                        } else if (pressure_ratio >= config.floats.pressure_mild_ratio) {
                            active_action_mask |= (1u << b);
                        } else if (item->generation == SF_ROOM_YOUNG && item->gc_sweep_count > SACFOR_GEN_YOUNG_MAX) {
                            active_action_mask |= (1u << b);
                        } else if (item->generation == SF_ROOM_ADOLESCENT && item->gc_sweep_count > SACFOR_GEN_ADOL_MAX) {
                            active_action_mask |= (1u << b);
                        } else if (item->compute_target_region() != item->region) {
                            active_action_mask |= (1u << b);
                        }
                    }

                    if (active_action_mask == 0) {
                        cur = walker;
                        continue;
                    }

                    for (size_t b = 0; b < count; ++b) {
                        auto* item = batch[b];
                        if (item->is_pinned()) continue;

                        if (item->state.load(std::memory_order_acquire) == static_cast<uint8_t>(sf_live_state_e::QUARANTINED)) {
                            region.evict_nolock(item);
                            ++evicted_count;
                            room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                            landfill.accept(item, SF_DEATH_ERROR, hotlog, sweep_no);
                            if (on_free) on_free(item, on_free_user);
                            size_t fp  = (size_t)item->memory_footprint.load(std::memory_order_relaxed);
                            size_t chb = total_heap_bytes.load(std::memory_order_relaxed);
                            if (chb >= fp) total_heap_bytes.fetch_sub(fp, std::memory_order_relaxed);
                            sacculus_hotlog_write(hotlog, SF_Ebin0_TO_FALLINE,
                                SF_Ebin1_ERROR, item->function_id, 0,
                                item->generation, item->region,
                                item->current_core.load(std::memory_order_relaxed), sweep_no, heap_kb,
                                "quarantine_evac_simd");
                            continue;
                        }

                        if (item->needs_rehabilitation) {
                            if (item->try_transition_to_rehab()) {
                                region.evict_nolock(item);
                                ++evicted_count;
                                room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                                landfill.accept(item, SF_DEATH_REHABILITATE, hotlog, sweep_no);
                                if (on_free) on_free(item, on_free_user);
                                size_t fp  = (size_t)item->memory_footprint.load(std::memory_order_relaxed);
                                size_t chb = total_heap_bytes.load(std::memory_order_relaxed);
                                if (chb >= fp) total_heap_bytes.fetch_sub(fp, std::memory_order_relaxed);
                            }
                            continue;
                        }

                        if (item->is_idle()) {
                            if (item->try_transition_to_falline()) {
                                region.evict_nolock(item);
                                ++evicted_count;
                                room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                                falline.push(item);
                                sacculus_hotlog_write(hotlog, SF_Ebin0_TO_FALLINE,
                                    SF_Ebin1_INFO, item->function_id, 0,
                                    item->generation, item->region,
                                    item->current_core.load(std::memory_order_relaxed), sweep_no, heap_kb,
                                    "idle_to_falline_simd");
                            }
                            continue;
                        }

                        bool drop_by_pressure = false;
                        float heat = item->heat_score.load(std::memory_order_relaxed);
                        if (pressure_ratio >= config.floats.pressure_critical_ratio && heat < SACFOR_HEAT_HOT) drop_by_pressure = true;
                        else if (pressure_ratio >= config.floats.pressure_high_ratio && item->generation <= SF_ROOM_ADOLESCENT && heat < SACFOR_HEAT_COLD * 2.0f) drop_by_pressure = true;
                        else if (pressure_ratio >= config.floats.pressure_mild_ratio && item->generation == SF_ROOM_YOUNG && heat < SACFOR_HEAT_COLD) drop_by_pressure = true;

                        if (drop_by_pressure) {
                            if (item->try_transition_to_falline()) {
                                region.evict_nolock(item);
                                ++evicted_count;
                                room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                                falline.push(item);
                                sacculus_hotlog_write(hotlog, SF_Ebin0_TO_FALLINE,
                                    SF_Ebin1_WARN, item->function_id, 0,
                                    item->generation, item->region,
                                    item->current_core.load(std::memory_order_relaxed), sweep_no, heap_kb,
                                    "pressure_drop_simd");
                            }
                            continue;
                        }

                        uint8_t young_max = SACFOR_GEN_YOUNG_MAX;
                        uint8_t adol_max  = SACFOR_GEN_ADOL_MAX;
                        if (pressure_ratio >= config.floats.pressure_high_ratio) { young_max /= 2; adol_max /= 2; }
                        if (young_max < 2) young_max = 2;
                        if (adol_max < 4) adol_max = 4;

                        uint8_t new_gen = item->generation;
                        if (item->generation == SF_ROOM_YOUNG && item->gc_sweep_count > young_max) new_gen = SF_ROOM_ADOLESCENT;
                        else if (item->generation == SF_ROOM_ADOLESCENT && item->gc_sweep_count > adol_max) new_gen = SF_ROOM_SENIOR;

                        if (new_gen != item->generation) {
                            region.evict_nolock(item);
                            room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                            motherline.rooms[new_gen].place(item);
                            continue;
                        }

                        uint8_t target_reg = item->compute_target_region();
                        if (target_reg != item->region) {
                            region.evict_nolock(item);
                            room.regions[target_reg].place(item);
                            continue;
                        }
                    }

                    cur = walker;
                }
            }
        }

        trigger_hook(SF_GC_HOOK_POST_SWEEP);
        return evicted_count;
    }
    SF_DISABLE_OPT_END

    size_t sweep_motherline(void) {
        if (config.strings.gc_mode == sf_gc_mode_e::SIMD) {
            return sweep_motherline_simd();
        }
        return sweep_motherline_cycles();
    }

    size_t scan_rehab_arena(void) {
        current_phase.store(SF_GC_PHASE_REHAB, std::memory_order_relaxed);
        rehab_scan_count.fetch_add(1, std::memory_order_relaxed);
        size_t resurrected = 0;

        uint32_t r = resurrect_read.load(std::memory_order_relaxed);
        uint32_t w = resurrect_write.load(std::memory_order_acquire);

        while (r != w) {
            uint32_t fn_id = resurrect_queue[r & (RESURRECT_QUEUE_CAP - 1)].exchange(0, std::memory_order_relaxed);
            if (fn_id != 0) {

                sf_ticket_guard_t g(landfill.pane.lock);
                uint32_t cnt = landfill.pane.count.load(std::memory_order_relaxed);
                for (uint32_t i = 0; i < cnt; ++i) {
                    auto& entry = landfill.pane.entries[i];
                    if (entry.function_id == fn_id && entry.lasting == 1) {
                        entry.lasting = 0;
                        resurrection_count.fetch_add(1, std::memory_order_relaxed);
                        ++resurrected;

                        sacculus_hotlog_write(hotlog, SF_Ebin0_PROMOTE, SF_Ebin1_INFO,
                            fn_id, (uint16_t)i, entry.peak_generation, entry.peak_region,
                            entry.last_core, sweep_count.load(std::memory_order_relaxed),
                            (uint32_t)(total_heap_bytes.load(std::memory_order_relaxed) / 1024),
                            "rehab_resurrected");
                        break;
                    }
                }
            }
            ++r;
        }
        resurrect_read.store(r, std::memory_order_release);
        return resurrected;
    }

    void maybe_compact_falline(void) {
        if (falline.emptybox_ratio() > SF_FALLINE_COMPACT_RATIO) {
            current_phase.store(SF_GC_PHASE_COMPACT, std::memory_order_relaxed);
            size_t merged = falline.compact_emptyboxes();
            if (merged > 0) {
                falline_compact_count.fetch_add(merged, std::memory_order_relaxed);
                sacculus_hotlog_write(hotlog, SF_Ebin0_GC_COMPACT, SF_Ebin1_TRACE,
                    0, 0, 0, 0, 0, sweep_count.load(std::memory_order_relaxed),
                    (uint32_t)(total_heap_bytes.load(std::memory_order_relaxed) / 1024),
                    "falline_compacted");
            }
        }
    }

    void compact_and_relocate_motherline_cycles(uint64_t sweep_no) {
        current_phase.store(SF_GC_PHASE_COMPACT, std::memory_order_relaxed);
        trigger_hook(SF_GC_HOOK_PRE_COMPACT);
        size_t relocated_count = 0;
        size_t relocated_bytes = 0;

        for (int room_i = 0; room_i < 2; ++room_i) {
            auto& room = motherline.rooms[room_i];
            for (int reg_i = 0; reg_i < SACFOR_REGION_COUNT; ++reg_i) {

                if (reg_i == SF_REGION_TANK) continue;

                auto& region = room.regions[reg_i];
                sf_ticket_guard_t rg(region.get_lock());
                sacculus_live_header_t* cur = region.get_head();

                while (cur) {
                    sacculus_live_header_t* next_obj = cur->next;

                    if (!cur->is_pinned() && cur->forwarding_address.load(std::memory_order_relaxed) == nullptr) {
                        size_t fp = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                        if (fp > 0 && fp < 64 * 1024) {
                            size_t cell_sz = SF_custominiVm_live_header_t::total_cell_size(fp);
                            void* new_raw = runway.allocator.allocate(cell_sz, SACFOR_CACHE_LINE);
                            if (new_raw) {
                                auto* new_obj = reinterpret_cast<SF_custominiVm_live_header_t*>(new_raw);
                                memcpy(new_obj, cur, sizeof(SF_custominiVm_live_header_t));
                                memcpy(new_obj->body(), cur->body(), fp);
                                new_obj->forwarding_address.store(nullptr, std::memory_order_relaxed);
                                new_obj->writeTailCanary();

                                sf_sync_store_fence();
                                cur->forwarding_address.store(new_obj->body(), std::memory_order_release);

                                region.evict_nolock(cur);
                                region.place_nolock(new_obj);

                                sf_critical_clear_memory(cur->body(), fp);
                                auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(cur);
                                falline.register_emptybox(cur->function_id, fp, cell_sz, eb, new_obj->body(), sweep_no);

                                ++relocated_count;
                                relocated_bytes += fp;
                            }
                        }
                    }
                    cur = next_obj;
                }
            }
        }

        trigger_hook(SF_GC_HOOK_POST_COMPACT);

        if (relocated_count > 0 && hotlog) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_GC_COMPACT, SF_Ebin1_TRACE,
                0, (uint16_t)relocated_count, 0, 0, 0, sweep_no,
                (uint32_t)(relocated_bytes / 1024), "motherline_compacted");
        }
    }

    SF_DISABLE_OPT_BEGIN
    void compact_and_relocate_motherline_simd(uint64_t sweep_no) {
        current_phase.store(SF_GC_PHASE_COMPACT, std::memory_order_relaxed);
        trigger_hook(SF_GC_HOOK_PRE_COMPACT);
        size_t relocated_count = 0;
        size_t relocated_bytes = 0;

        for (int room_i = 0; room_i < 2; ++room_i) {
            auto& room = motherline.rooms[room_i];
            for (int reg_i = 0; reg_i < SACFOR_REGION_COUNT; ++reg_i) {
                if (reg_i == SF_REGION_TANK) continue;

                auto& region = room.regions[reg_i];
                sf_ticket_guard_t rg(region.get_lock());
                sacculus_live_header_t* cur = region.get_head();

                while (cur) {
                    sacculus_live_header_t* next_obj = cur->next;

                    if (!cur->is_pinned() && cur->forwarding_address.load(std::memory_order_relaxed) == nullptr) {
                        size_t fp = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                        if (fp > 0 && fp < 64 * 1024) {
                            size_t cell_sz = SF_custominiVm_live_header_t::total_cell_size(fp);
                            void* new_raw = runway.allocator.allocate(cell_sz, SACFOR_CACHE_LINE);
                            if (new_raw) {
                                auto* new_obj = reinterpret_cast<SF_custominiVm_live_header_t*>(new_raw);

                                memcpy(new_obj, cur, sizeof(SF_custominiVm_live_header_t));

                                sf_critical_clear_memory(new_obj->body(), fp);
                                memcpy(new_obj->body(), cur->body(), fp);

                                new_obj->forwarding_address.store(nullptr, std::memory_order_relaxed);
                                new_obj->writeTailCanary();

                                sf_sync_store_fence();
                                cur->forwarding_address.store(new_obj->body(), std::memory_order_release);

                                region.evict_nolock(cur);
                                region.place_nolock(new_obj);

                                sf_critical_clear_memory(cur->body(), fp);
                                auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(cur);
                                falline.register_emptybox(cur->function_id, fp, cell_sz, eb, new_obj->body(), sweep_no);

                                ++relocated_count;
                                relocated_bytes += fp;
                            }
                        }
                    }
                    cur = next_obj;
                }
            }
        }

        trigger_hook(SF_GC_HOOK_POST_COMPACT);

        if (relocated_count > 0 && hotlog) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_GC_COMPACT, SF_Ebin1_TRACE,
                0, (uint16_t)relocated_count, 0, 0, 0, sweep_no,
                (uint32_t)(relocated_bytes / 1024), "motherline_simd_compacted");
        }
    }
    SF_DISABLE_OPT_END

    void compact_and_relocate_motherline(uint64_t sweep_no) {
        if (config.strings.gc_mode == sf_gc_mode_e::SIMD) {
            compact_and_relocate_motherline_simd(sweep_no);
            return;
        }
        compact_and_relocate_motherline_cycles(sweep_no);
    }

    void start(void) {
        if (config.is_safety_critical) {

            running.store(false, std::memory_order_release);
            return;
        }
        running.store(true, std::memory_order_release);
        gc_thread = std::thread([this]() {
            while (running.load(std::memory_order_acquire)) {
                uint64_t t0 = sacculus_now_ns();

                sweep_motherline();

                drain_falline();

                uint64_t current_sweep = sweep_count.fetch_add(1, std::memory_order_relaxed) + 1;

                if (current_sweep % SF_REHAB_SCAN_INTERVAL == 0) {
                    scan_rehab_arena();
                }

                maybe_compact_falline();
                if (current_sweep % 50 == 0) {
                    compact_and_relocate_motherline(current_sweep);
                }

                if (config.strings.snatch_rebate_mode == sf_snatch_rebate_mode_e::RAPID) {
                    runway.allocator.rebate_pages(config.strings.snatch_rebate_mode, 64 * 1024);
                } else if (config.strings.snatch_rebate_mode == sf_snatch_rebate_mode_e::AUTO) {
                    if (current_sweep % 100 == 0) {
                        runway.allocator.rebate_pages(config.strings.snatch_rebate_mode, 256 * 1024);
                    }
                } else if (config.strings.snatch_rebate_mode == sf_snatch_rebate_mode_e::GYC) {
                    if (current_sweep % 500 == 0) {
                        runway.allocator.rebate_pages(config.strings.snatch_rebate_mode, 1024 * 1024);
                    }
                }

                current_phase.store(SF_GC_PHASE_IDLE, std::memory_order_relaxed);

                uint32_t heap_kb = (uint32_t)(total_heap_bytes.load(std::memory_order_relaxed) / 1024);
                sacculus_hotlog_write(hotlog, SF_Ebin0_GC_SWEEP, SF_Ebin1_TRACE,
                    0, 0, 0, 0, 0, current_sweep, heap_kb,
                    "gc_heartbeat");

                if (current_sweep % 1000 == 0) {
                    char msg[16];
                    snprintf(msg, sizeof(msg), "swp=%llu r=%llu",
                             (unsigned long long)current_sweep,
                             (unsigned long long)resurrection_count.load(std::memory_order_relaxed));
                    sacculus_coldlog_write(coldlog, SF_Ebin0_GC_SWEEP,
                        SF_Ebin1_INFO, 0, msg);
                }

                uint64_t elapsed = sacculus_now_ns() - t0;
                last_sweep_ns.store(elapsed, std::memory_order_relaxed);
                uint64_t interval = config.integers.wakes_gc_interval_ns ? config.integers.wakes_gc_interval_ns : SACFOR_GC_INTERVAL_NS;

                if (config.strings.gc_heuristic == sf_gc_heuristic_e::AGGRESSIVE) {
                    interval = 250000ULL;
                } else if (config.strings.gc_heuristic == sf_gc_heuristic_e::BUREAUCRATIC) {
                    interval = 5000000ULL;
                } else {

                    size_t cur_b = total_heap_bytes.load(std::memory_order_relaxed);
                    float press_ratio = pressure_threshold > 0 ? ((float)cur_b / (float)pressure_threshold) : 0.0f;
                    if (press_ratio > config.floats.pressure_critical_ratio) {
                        interval = 500000ULL;
                    } else if (press_ratio < config.floats.pressure_mild_ratio) {
                        interval = 3000000ULL;
                    }
                }

                if (elapsed < interval) {
                    std::this_thread::sleep_for(
                        std::chrono::nanoseconds(interval - elapsed));
                }
            }
        });
    }

    void apply_heuristic_policies(uint64_t current_sweep) {
        if (config.is_safety_critical || config.strings.gc_heuristic == sf_gc_heuristic_e::BUREAUCRATIC) {

            for (size_t r = 0; r < SACFOR_ROOM_COUNT; ++r) {
                for (size_t reg = 0; reg < SACFOR_REGION_COUNT; ++reg) {
                    auto& reg_ref = runway.motherline.rooms[r].regions[reg];
                    sf_ticket_guard_t g(reg_ref.get_lock());
                    auto* cur = reg_ref.get_head();
                    while (cur) {
                        if (!cur->isTailCanaryValid() || cur->sfMN != SF_custominiVm_live_header_t::SF_LIVE_MAGIC) {
                            cur->state.store(static_cast<uint8_t>(sf_live_state_e::QUARANTINED), std::memory_order_release);
                        }
                        cur = cur->next;
                    }
                }
            }
            if (!config.is_safety_critical && current_sweep % 10 == 0) {
                compact_and_relocate_motherline(current_sweep);
            }
        } else if (config.strings.gc_heuristic == sf_gc_heuristic_e::AGGRESSIVE) {

            runway.allocator.rebate_pages(sf_snatch_rebate_mode_e::RAPID, 64 * 1024);
            maybe_compact_falline();
            if (current_sweep % 20 == 0) {
                compact_and_relocate_motherline(current_sweep);
            }
        }
    }

    sf_step_result_t step(void) {
        sf_step_result_t res{};
        if (config.strings.gc_heuristic == sf_gc_heuristic_e::DISABLED || config.strings.no_sfgc_overwrite_mode) {

            return res;
        }

        res.swept = sweep_motherline();

        res.drained = drain_falline();

        uint64_t current_sweep = sweep_count.fetch_add(1, std::memory_order_relaxed) + 1;

        if (current_sweep % SF_REHAB_SCAN_INTERVAL == 0) {
            res.resurrected = scan_rehab_arena();
        }

        maybe_compact_falline();
        if (config.is_safety_critical || config.strings.always_focus_compactizing) {

            compact_and_relocate_motherline(current_sweep);
        } else if (current_sweep % 50 == 0) {
            compact_and_relocate_motherline(current_sweep);
        }

        if (config.strings.snatch_rebate_mode == sf_snatch_rebate_mode_e::RAPID) {
            res.rebated = runway.allocator.rebate_pages(config.strings.snatch_rebate_mode, 64 * 1024);
        } else if (config.strings.snatch_rebate_mode == sf_snatch_rebate_mode_e::AUTO) {
            if (current_sweep % 100 == 0) {
                res.rebated = runway.allocator.rebate_pages(config.strings.snatch_rebate_mode, 256 * 1024);
            }
        } else if (config.strings.snatch_rebate_mode == sf_snatch_rebate_mode_e::GYC) {
            if (current_sweep % 500 == 0) {
                res.rebated = runway.allocator.rebate_pages(config.strings.snatch_rebate_mode, 1024 * 1024);
            }
        }

        apply_heuristic_policies(current_sweep);
        if (config.strings.snatch_rebate_mode == sf_snatch_rebate_mode_e::RAPID) {
            res.rebated = runway.allocator.rebate_pages(config.strings.snatch_rebate_mode, 64 * 1024);
        } else if (config.strings.snatch_rebate_mode == sf_snatch_rebate_mode_e::AUTO) {
            if (current_sweep % 100 == 0) {
                res.rebated = runway.allocator.rebate_pages(config.strings.snatch_rebate_mode, 256 * 1024);
            }
        } else if (config.strings.snatch_rebate_mode == sf_snatch_rebate_mode_e::GYC) {
            if (current_sweep % 500 == 0) {
                res.rebated = runway.allocator.rebate_pages(config.strings.snatch_rebate_mode, 1024 * 1024);
            }
        }

        current_phase.store(SF_GC_PHASE_IDLE, std::memory_order_relaxed);
        return res;
    }

    SF_custominiVm_gc_manifest_t shake_bed(void) {
        uint64_t t0 = sacculus_now_ns();
        SF_custominiVm_gc_manifest_t m{};
        m.sweep_id = sweep_count.load(std::memory_order_relaxed) + 1;
        m.rooms_inspected = 3;

        size_t heap_before = total_heap_bytes.load(std::memory_order_relaxed);

        if (config.strings.gc_heuristic == sf_gc_heuristic_e::DISABLED || config.strings.no_sfgc_overwrite_mode) {
            m.quiescent = 1;
            m.heap_bytes_after = heap_before;
            m.elapsed_ns = sacculus_now_ns() - t0;
            snprintf(m.narrative, sizeof(m.narrative),
                "No-sfGC Overwrite Mode Active: GC dormant, disk-sector direct overwrite enabled.");
            std::lock_guard<std::mutex> lk(manifest_lock);
            last_manifest = m;
            return m;
        }

        current_phase.store(SF_GC_PHASE_SWEEP, std::memory_order_release);
        sf_step_result_t r1 = step();
        m.cells_swept += r1.swept;
        m.cells_drained += r1.drained;

        if (runway.falline.count > 0 || r1.swept > 0) {
            maybe_compact_falline();
            sf_step_result_t r2 = step();
            m.cells_swept += r2.swept;
            m.cells_drained += r2.drained;
            m.trailing_debris_cleared = 1;
        } else {
            m.trailing_debris_cleared = 0;
        }

        size_t heap_after = total_heap_bytes.load(std::memory_order_relaxed);
        m.bytes_reclaimed = (heap_before > heap_after) ? (heap_before - heap_after) : 0;
        m.heap_bytes_after = heap_after;
        m.elapsed_ns = sacculus_now_ns() - t0;
        m.quiescent = 1;

        snprintf(m.narrative, sizeof(m.narrative),
            "Awakened (manual shake) -> Swept %zu cells -> Reclaimed %.2f MB -> Trailing debris %s -> Dormant (Asleep)",
            m.cells_swept,
            (double)m.bytes_reclaimed / (1024.0 * 1024.0),
            m.trailing_debris_cleared ? "cleared" : "clean");

        if (hotlog) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_GC_SWEEP, SF_Ebin1_INFO,
                0, (uint16_t)m.cells_swept, 0, 0, 0, m.sweep_id,
                (uint32_t)(m.bytes_reclaimed / 1024), m.narrative);
        }

        current_phase.store(SF_GC_PHASE_IDLE, std::memory_order_release);

        std::lock_guard<std::mutex> lk(manifest_lock);
        last_manifest = m;
        return m;
    }

    size_t run(SF_custominiVm_gc_run_mode_t mode,
               bool owp,
               SF_custominiVm_mtp_predicate_fn mtp_pred = nullptr,
               void* user_data = nullptr) {
        if (mode == SF_GC_RUN_SINGLE_STEP) {
            if (owp) {
                sf_cpu_pause();
                std::this_thread::yield();
            }
            SF_custominiVm_gc_manifest_t m = shake_bed();
            return m.bytes_reclaimed > 0 ? m.bytes_reclaimed : (m.cells_swept > 0 ? m.cells_swept : 1);
        }

        sf_env_snapshot_t prev_env = capture_env();

        if (mode == SF_GC_RUN_CONTINUE_FINISH) {

            size_t steps = 0;
            size_t consecutive_quiescent = 0;
            const size_t MAX_STEPS = 1024;

            while (steps < MAX_STEPS) {
                sf_step_result_t r = step();
                steps++;

                if (owp) {

                    sf_cpu_pause();
                    std::this_thread::yield();

                    sf_env_snapshot_t cur_env = capture_env();
                    if (has_env_changed(prev_env, cur_env)) {
                        if (hotlog) {
                            sacculus_hotlog_write(hotlog, SF_Ebin0_GC_SWEEP, SF_Ebin1_TRACE,
                                0, 0, 0, 0, 0, sweep_count.load(std::memory_order_relaxed), 0, "owp_mutation");
                        }
                    }
                    prev_env = cur_env;
                }

                if (r.swept == 0 && r.drained == 0 && runway.falline.count == 0 &&
                    resurrect_read.load(std::memory_order_relaxed) == resurrect_write.load(std::memory_order_relaxed)) {
                    consecutive_quiescent++;
                    if (consecutive_quiescent >= 2) {
                        break;
                    }
                } else {
                    consecutive_quiescent = 0;
                }
            }
            return steps;
        }

        if (mode == SF_GC_RUN_STEPPING_MTP) {

            size_t steps = 0;
            const size_t MAX_MTP_STEPS = 50000;

            while (steps < MAX_MTP_STEPS) {
                step();
                steps++;

                sf_env_snapshot_t cur_env = capture_env();
                SF_custominiVm_env_change_t env_delta{};
                env_delta.changed = has_env_changed(prev_env, cur_env) ? 1 : 0;
                env_delta.allocations_detected = (uint32_t)(cur_env.runway_bump > prev_env.runway_bump ? (cur_env.runway_bump - prev_env.runway_bump) : 0);
                env_delta.pacing_stalls_delta = (uint32_t)(cur_env.pacing_stalls > prev_env.pacing_stalls ? (cur_env.pacing_stalls - prev_env.pacing_stalls) : 0);
                env_delta.calls_delta = (uint32_t)(cur_env.total_calls > prev_env.total_calls ? (cur_env.total_calls - prev_env.total_calls) : 0);
                env_delta.resurrections_delta = (uint32_t)(cur_env.resurrections > prev_env.resurrections ? (cur_env.resurrections - prev_env.resurrections) : 0);
                env_delta.paged_out_delta = (uint32_t)(cur_env.paged_out_count > prev_env.paged_out_count ? (cur_env.paged_out_count - prev_env.paged_out_count) : 0);
                prev_env = cur_env;

                if (mtp_pred) {
                    SF_custominiVm_anatomy_stat_t stat{};
                    SF_custominiVm_anatomy_query(&stat);
                    if (mtp_pred(&stat, &env_delta, user_data)) {
                        break;
                    }
                } else {

                    size_t cur_b = total_heap_bytes.load(std::memory_order_relaxed);
                    float ratio = pressure_threshold > 0 ? ((float)cur_b / (float)pressure_threshold) : 0.0f;
                    if (ratio < config.floats.pressure_mild_ratio) {
                        break;
                    }
                    if (owp && env_delta.allocations_detected > 0) {
                        break;
                    }
                }

                if (owp) {
                    sf_cpu_pause();
                    std::this_thread::yield();
                }
            }
            return steps;
        }

        return 0;
    }

    void stop(void) {
        running.store(false, std::memory_order_release);
        if (gc_thread.joinable()) gc_thread.join();
        pagefile.destroy();
    }
};

using sfGC = sacculus_gc_t;

inline void sacculus_runway_t::pace_allocation(void) {
    if (!gc_ref) return;
    auto* gc = reinterpret_cast<sacculus_gc_t*>(gc_ref);
    size_t th = gc->pressure_threshold;
    if (th == 0) return;
    size_t cur = gc->total_heap_bytes.load(std::memory_order_relaxed);
    float ratio = (float)cur / (float)th;
    if (ratio >= 0.95f) {
        pacing_stalls.fetch_add(1, std::memory_order_relaxed);
        std::this_thread::yield();
        for (volatile int p = 0; p < 2000; ++p) {
            sf_cpu_pause();
        }
    } else if (ratio >= 0.85f) {
        pacing_stalls.fetch_add(1, std::memory_order_relaxed);
        std::this_thread::yield();
    }
}

inline SF_custominiVm_emptybox_header_t* sacculus_runway_t::snatch_emptybox(size_t cell_sz) {
    uint64_t swp = 0;
    if (gc_ref) {
        swp = reinterpret_cast<sacculus_gc_t*>(gc_ref)->sweep_count.load(std::memory_order_relaxed);
    }
    return falline.find_and_take_emptybox(cell_sz, swp);
}

inline void sacculus_runway_t::free_cell(void* body_ptr, size_t body_bytes) {
    if (!body_ptr) return;
    auto* obj = SF_custominiVm_live_header_t::from_body(body_ptr);
    if (!obj) return;

    motherline.evict(obj);

    auto* p = obj->parent_header.load(std::memory_order_acquire);
    if (p) {
        p->child_view_count.fetch_sub(1, std::memory_order_acq_rel);
        obj->parent_header.store(nullptr, std::memory_order_release);
    }

    bool overwrite = false;
    if (gc_ref) {
        overwrite = reinterpret_cast<sacculus_gc_t*>(gc_ref)->config.strings.no_sfgc_overwrite_mode;
    }
    if (!overwrite) {
        sf_critical_clear_memory(obj->body(), body_bytes);
    }

    size_t cell_sz = SF_custominiVm_live_header_t::total_cell_size(body_bytes);
    auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(obj);
    falline.register_emptybox(obj->function_id, body_bytes, cell_sz, eb);
}

struct SACFOR_ALIGN_CL sacculus_slot_t {
    sacculus_live_header_t* obj = nullptr;
    std::atomic<int>        free{1};
};
using sfSlot = sacculus_slot_t;

struct sacculus_instance_pool_t {
    uint32_t               function_id;
    char                   name[32];
    sacculus_slot_t        slots[SACFOR_MAX_INSTANCES];
    std::atomic<uint32_t>  used_count{0};
    uint64_t               type_sig_hash;

    int32_t acquire(sacculus_live_header_t* obj) {
        if (used_count.load() >= SACFOR_MAX_INSTANCES) return -1;
        for (uint32_t i = 0; i < SACFOR_MAX_INSTANCES; ++i) {
            int expected = 1;
            if (slots[i].free.compare_exchange_weak(expected, 0,
                    std::memory_order_acquire, std::memory_order_relaxed)) {
                slots[i].obj = obj;
                used_count.fetch_add(1, std::memory_order_relaxed);
                return (int32_t)i;
            }
        }
        return -1;
    }

    void release(int32_t idx) {
        if (idx < 0 || (uint32_t)idx >= SACFOR_MAX_INSTANCES) return;
        slots[idx].obj = nullptr;
        slots[idx].free.store(1, std::memory_order_release);
        used_count.fetch_sub(1, std::memory_order_relaxed);
    }
};

using sfInstancePool = sacculus_instance_pool_t;

#define SACFOR_MAX_DISPATCH_SLOTS  4096

struct SACFOR_ALIGN_CL sacculus_dispatch_entry_t {
    std::atomic<sacculus_kernel_fn_t> fn{nullptr};
    std::atomic<uint32_t> seqlock_version{0};
    uint32_t  function_id;
    std::atomic<sacculus_live_header_t*> cached_live_header{nullptr};
    uint64_t  best_cycles;
    uint64_t  call_count;
};
using sfDispatchEntry = sacculus_dispatch_entry_t;

struct sacculus_dispatch_table_t {
    sacculus_dispatch_entry_t entries[SACFOR_MAX_DISPATCH_SLOTS];

    sacculus_dispatch_entry_t* get(uint32_t fn_id) {
        if (fn_id >= SACFOR_MAX_DISPATCH_SLOTS) return nullptr;
        return &entries[fn_id];
    }

    void register_fn(uint32_t fn_id, sacculus_kernel_fn_t fn) {
        if (fn_id >= SACFOR_MAX_DISPATCH_SLOTS) return;
        auto& entry = entries[fn_id];
        uint32_t v = entry.seqlock_version.load(std::memory_order_relaxed);
        entry.seqlock_version.store(v + 1, std::memory_order_release);

        entry.function_id = fn_id;
        entry.fn.store(fn, std::memory_order_relaxed);
        entry.best_cycles = UINT64_MAX;
        entry.call_count  = 0;

        if (fn) {
            sf_sync_flush_icache((void*)fn, 64);
        }
        sf_sync_full_fence();

        entry.seqlock_version.store(v + 2, std::memory_order_release);
    }

    void swap(uint32_t fn_id, sacculus_kernel_fn_t new_fn, uint64_t best) {
        if (fn_id >= SACFOR_MAX_DISPATCH_SLOTS) return;
        auto& entry = entries[fn_id];
        uint32_t v = entry.seqlock_version.load(std::memory_order_relaxed);
        entry.seqlock_version.store(v + 1, std::memory_order_release);

        entry.fn.store(new_fn, std::memory_order_relaxed);
        entry.best_cycles = best;

        if (new_fn) {
            sf_sync_flush_icache((void*)new_fn, 64);
        }
        sf_sync_full_fence();

        entry.seqlock_version.store(v + 2, std::memory_order_release);
    }
};

using sfDispatchTable = sacculus_dispatch_table_t;

struct sf_world_isa_entry_t {
    const char* isa_name;
    const char* family;
    uint16_t    vector_bits;
    const char* fallback_isa;
};

static const sf_world_isa_entry_t g_world_isa_catalog[] = {

    { "AMX_COMPLEX",      "x86",  1024, "AMX_FP16" },
    { "AMX_FP16",         "x86",  1024, "AMX_TILE" },
    { "AMX_TILE",         "x86",  1024, "AVX512F" },
    { "AMX_BF16",         "x86",  1024, "AVX512_BF16" },
    { "AMX_INT8",         "x86",  1024, "AVX512VNNI" },
    { "AVX10",            "x86",   512, "AVX512F" },
    { "APX_F",            "x86",     0, "AVX2" },
    { "AVX_IFMA",         "x86",   512, "AVX512F" },
    { "AVX512_VP2INTERSECT","x86", 512, "AVX512F" },
    { "AVX512_BF16",      "x86",   512, "AVX512BW" },
    { "AVX512VBMI2",      "x86",   512, "AVX512VBMI" },
    { "AVX512VBMI",       "x86",   512, "AVX512BW" },
    { "INTEL_DLBOOST",    "x86",   512, "AVX512VNNI" },
    { "AVX512VNNI",       "x86",   512, "AVX512BW" },
    { "AVX512BITALG",     "x86",   512, "AVX512BW" },
    { "AVX512VPOPCNTDQ",  "x86",   512, "AVX512BW" },
    { "AVX512FP16",       "x86",   512, "AVX512BW" },
    { "AVX512BW",         "x86",   512, "AVX512DQ" },
    { "AVX512DQ",         "x86",   512, "AVX512F" },
    { "AVX512CD",         "x86",   512, "AVX512F" },
    { "AVX512VL",         "x86",   256, "AVX2" },
    { "AVX512F",          "x86",   512, "AVX2" },
    { "AVX_VNNI",         "x86",   256, "AVX2" },
    { "FMA3",             "x86",   256, "AVX" },
    { "FMA4",             "x86",   256, "AVX" },
    { "XOP",              "x86",   128, "AVX" },
    { "AVX2",             "x86",   256, "AVX" },
    { "F16C",             "x86",   128, "AVX" },
    { "AVX",              "x86",   256, "SSE4_2" },
    { "BMI2",             "x86",     0, "BMI1" },
    { "BMI1",             "x86",     0, "SSE4_2" },
    { "SSE4_2",           "x86",   128, "SSE4_1" },
    { "SSE4_1",           "x86",   128, "SSSE3" },
    { "SSE4A",            "x86",   128, "SSE3" },
    { "SSSE3",            "x86",   128, "SSE3" },
    { "SSE3",             "x86",   128, "SSE2" },
    { "SSE2",             "x86",   128, "SSE" },
    { "SSE",              "x86",   128, "MMX" },
    { "MMX",              "x86",    64, "SCALAR" },

    { "SME2",             "ARM",  2048, "SME" },
    { "SME",              "ARM",  1024, "SVE2" },
    { "SVE2",             "ARM",   512, "SVE" },
    { "ARM_SVE_512",      "ARM",   512, "SVE" },
    { "ARM_SVE_256",      "ARM",   256, "SVE" },
    { "SVE",              "ARM",   512, "ARM_DOTPROD" },
    { "ARM_I8MM",         "ARM",   128, "NEON" },
    { "ARM_BF16",         "ARM",   128, "NEON" },
    { "ARM_DOTPROD",      "ARM",   128, "NEON" },
    { "ARM_FP16",         "ARM",   128, "NEON" },
    { "ARM_ASIMD_HP",     "ARM",   128, "NEON" },
    { "ASIMD",            "ARM",   128, "NEON" },
    { "NEON",             "ARM",   128, "ARM_V8" },
    { "ARM_LSE_ATOMICS",  "ARM",     0, "ARM_V8" },
    { "ARM_CRYPTO",       "ARM",   128, "NEON" },
    { "ARM_V81",          "ARM",     0, "ARM_V8" },
    { "ARM_V8",           "ARM",     0, "SCALAR" },
    { "APPLE_AMX",        "ARM",   512, "NEON" },

    { "TENSTORRENT_TENSIX","RISC-V",1024, "RVV" },
    { "ESPERANTO_ET_MAX",  "RISC-V", 512, "RVV" },
    { "RVV",              "RISC-V", 256, "RV64" },
    { "RV64",             "RISC-V",   0, "SCALAR" },

    { "WASM_RELAXED_SIMD","WASM",  128, "WASM_SIMD128" },
    { "WASM_SIMD128",     "WASM",  128, "SCALAR" },

    { "VSX3",             "POWER", 128, "VSX" },
    { "VSX",              "POWER", 128, "ALTIVEC" },
    { "ALTIVEC",          "POWER", 128, "SCALAR" },

    { "MIPS_MSA",         "MIPS",  128, "MIPS64" },
    { "MIPS64",           "MIPS",    0, "SCALAR" },

    { "LASX",             "LOONGARCH", 256, "LSX" },
    { "LSX",              "LOONGARCH", 128, "LA64" },
    { "LA64",             "LOONGARCH",   0, "SCALAR" },

    { "SUNWAY_SW26010",   "SUNWAY", 512, "SCALAR" },
    { "KALRAY_MPPA_VLIW", "KALRAY", 128, "SCALAR" },

    { "S390_NNPA",        "S390X", 1024, "S390_VXE2" },
    { "S390_VXE2",        "S390X",  128, "S390_VXE"  },
    { "S390_VXE",         "S390X",  128, "S390_VX"   },
    { "S390_VX",          "S390X",  128, "SCALAR"    },

    { "SCALAR",           "UNIVERSAL", 0, nullptr }
};

static inline const sf_world_isa_entry_t* sf_find_world_isa(const char* name) {
    if (!name || !*name) return nullptr;
    for (const auto& item : g_world_isa_catalog) {
        if (_stricmp(item.isa_name, name) == 0) return &item;
    }
    return nullptr;
}

typedef SF_custominiVm_cache_level_info_t sf_cache_level_info_t;
typedef SF_custominiVm_hardware_topology_t sf_hardware_topology_t;

#if defined(_WIN32)
static void sf_parse_smbios(sf_hardware_topology_t* topo) {
    if (!topo) return;
    const DWORD sig = 'RSMB';
    DWORD sz = GetSystemFirmwareTable(sig, 0, nullptr, 0);
    if (sz == 0) return;
    std::vector<uint8_t> smb_buf(sz);
    if (GetSystemFirmwareTable(sig, 0, smb_buf.data(), sz) != sz) return;

    if (sz < 8) return;
    const uint8_t* p = smb_buf.data() + 8;
    const uint8_t* end = smb_buf.data() + sz;

    while (p + 4 <= end) {
        uint8_t type = p[0];
        uint8_t len  = p[1];
        if (len < 4) break;
        const uint8_t* data = p;
        const uint8_t* str_table = p + len;

        const uint8_t* next_p = str_table;
        while (next_p + 1 < end && !(next_p[0] == 0 && next_p[1] == 0)) {
            next_p++;
        }
        next_p += 2;

        auto get_string = [&](uint8_t idx) -> const char* {
            if (idx == 0) return "";
            const char* s = (const char*)str_table;
            uint8_t cur = 1;
            while ((const uint8_t*)s < next_p && *s) {
                if (cur == idx) return s;
                s += strlen(s) + 1;
                cur++;
            }
            return "";
        };

        if (type == 2 && topo->motherboard_model[0] == '\0') {
            if (len >= 6) {
                const char* mfg = get_string(data[4]);
                const char* prod = get_string(data[5]);
                char mb[128] = {0};
                if (mfg[0] && prod[0]) {
                    snprintf(mb, sizeof(mb), "%s %s", mfg, prod);
                } else if (prod[0]) {
                    snprintf(mb, sizeof(mb), "%s", prod);
                }
                if (mb[0]) strncpy(topo->motherboard_model, mb, sizeof(topo->motherboard_model) - 1);
            }
        } else if (type == 17) {
            uint16_t speed = 0;
            uint16_t conf_speed = 0;
            if (len >= 0x17) {
                speed = *(const uint16_t*)(data + 0x15);
            }
            if (len >= 0x22) {
                conf_speed = *(const uint16_t*)(data + 0x20);
            }
            uint32_t eff_mhz = conf_speed > 0 ? conf_speed : speed;
            if (eff_mhz > topo->dram_mhz) {
                topo->dram_mhz = eff_mhz;
            }
        }

        p = next_p;
    }

    if (topo->dram_mhz > 4800 || (topo->dram_mhz > 3200 && topo->dram_mhz <= 4400)) {
        topo->is_xmp_expo_active = 1;
    }
}
#elif defined(__linux__) || defined(__ANDROID__)
static void sf_parse_smbios(sf_hardware_topology_t* topo) {
    if (!topo) return;
    FILE* f_mfg = fopen("/sys/class/dmi/id/board_vendor", "r");
    FILE* f_mod = fopen("/sys/class/dmi/id/board_name", "r");
    char mfg[64] = {0}, mod[64] = {0};
    if (f_mfg) { if (fgets(mfg, sizeof(mfg), f_mfg)) { char* nl = strchr(mfg, '\n'); if (nl) *nl = 0; } fclose(f_mfg); }
    if (f_mod) { if (fgets(mod, sizeof(mod), f_mod)) { char* nl = strchr(mod, '\n'); if (nl) *nl = 0; } fclose(f_mod); }
    if (mfg[0] || mod[0]) {
        snprintf(topo->motherboard_model, sizeof(topo->motherboard_model), "%s %s", mfg, mod);
    }
}
#elif defined(__APPLE__)
static void sf_parse_smbios(sf_hardware_topology_t* topo) {
    if (!topo) return;
    char model[128] = {0};
    size_t sz = sizeof(model);
    if (sysctlbyname("hw.model", model, &sz, NULL, 0) == 0) {
        strncpy(topo->motherboard_model, model, sizeof(topo->motherboard_model) - 1);
    }
}
#else
static void sf_parse_smbios(sf_hardware_topology_t* topo) { (void)topo; }
#endif

static uint32_t sf_measure_cpu_cur_freq_mhz(void) {
#if defined(_WIN32)
    LARGE_INTEGER freq, c0, c1;
    if (QueryPerformanceFrequency(&freq) && freq.QuadPart > 0) {
        QueryPerformanceCounter(&c0);
        uint64_t t0 = sacculus_rdtsc();
        LONGLONG target_ticks = freq.QuadPart / 1000;
        do {
            QueryPerformanceCounter(&c1);
        } while ((c1.QuadPart - c0.QuadPart) < target_ticks);
        uint64_t t1 = sacculus_rdtsc();

        double elapsed_sec = (double)(c1.QuadPart - c0.QuadPart) / (double)freq.QuadPart;
        if (elapsed_sec > 0.0) {
            double cycles = (double)(t1 - t0);
            double mhz = (cycles / elapsed_sec) / 1000000.0;
            return (uint32_t)mhz;
        }
    }
    return 0;
#elif defined(__linux__)
    FILE* f = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");
    if (f) {
        uint32_t khz = 0;
        if (fscanf(f, "%u", &khz) == 1) {
            fclose(f);
            return khz / 1000;
        }
        fclose(f);
    }
    return 0;
#elif defined(__APPLE__)
    uint64_t hz = 0;
    size_t sz = sizeof(hz);
    if (sysctlbyname("hw.cpufrequency", &hz, &sz, NULL, 0) == 0 && hz > 0) {
        return (uint32_t)(hz / 1000000);
    }
    return 0;
#else
    return 0;
#endif
}

struct sf_upclocker_t {
    std::atomic<bool>     is_boosted{false};

    static void pin_max_frequency_warmup(uint32_t duration_us = 50) {
#if defined(_M_X64) || defined(__x86_64__) || defined(__aarch64__) || defined(_M_ARM64)
        uint64_t t0 = sacculus_rdtsc();
        uint64_t target_cycles = (uint64_t)duration_us * 3500ULL;
        volatile uint64_t acc = 0x5A5A5A5AULL;
        while ((sacculus_rdtsc() - t0) < target_cycles) {
            acc = (acc * 6364136223846793005ULL) + 1442695040888963407ULL;
            acc ^= (acc >> 17);
        }
#endif
    }

    void enable(bool boost) {
        is_boosted.store(boost, std::memory_order_relaxed);
#if defined(_WIN32)
        if (boost) {
            SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
            SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
            pin_max_frequency_warmup(100);
        } else {
            SetThreadExecutionState(ES_CONTINUOUS);
            SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
        }
#elif defined(__linux__)
        if (boost) {
            pin_max_frequency_warmup(100);
        }
#endif
    }
};

static sf_upclocker_t g_sf_upclocker;

static inline void sf_upclocker_set_boost(int enable) {
    g_sf_upclocker.enable(enable != 0);
}

static inline int sf_upclocker_is_boosted(void) {
    return g_sf_upclocker.is_boosted.load(std::memory_order_relaxed) ? 1 : 0;
}

static inline void sf_format_hardware_topology_summary(sf_hardware_topology_t* topo) {
    if (!topo) return;
    char buf[1024];
    snprintf(buf, sizeof(buf),
        "Hardware Topology:\n"
        "  Motherboard: %s\n"
        "  SRAM Caches: L1d=%u KB (%uB line, %u-way), L1i=%u KB, L2=%u KB, L3=%u KB\n"
        "  DRAM: Total=%llu MB, Avail=%llu MB, Speed=%u MT/s (Profile: %s), PageSize=%u B, HugePages=%s (%u MB)\n"
        "  Interconnect: NUMA Nodes=%u, Channels=%u (%u-bit bus, Est Bandwidth ~%u GB/s), UMA=%s\n"
        "  Cores: Physical=%u, Logical=%u, P-Cores=%u, E-Cores=%u, Heterogeneous=%s\n"
        "  Clocks: Current=%u MHz, Upclocker=%s",
        topo->motherboard_model[0] ? topo->motherboard_model : "Generic / Unknown",
        topo->l1d.size_kb, (unsigned int)topo->primary_cacheline_size, (unsigned int)topo->l1d.associativity,
        topo->l1i.size_kb, topo->l2.size_kb, topo->l3.size_kb,
        (unsigned long long)(topo->total_phys_ram_bytes / (1024 * 1024)),
        (unsigned long long)(topo->avail_phys_ram_bytes / (1024 * 1024)),
        topo->dram_mhz, topo->is_xmp_expo_active ? "XMP/EXPO/DOCP OC" : "JEDEC Standard",
        topo->page_size, topo->huge_pages_supported ? "YES" : "NO", topo->huge_page_size / (1024 * 1024),
        (unsigned int)topo->numa_nodes, (unsigned int)topo->memory_channels,
        (unsigned int)topo->bus_width_bits, topo->estimated_bandwidth_gbps,
        topo->is_uma ? "YES (Zero-Copy)" : "NO",
        topo->physical_cores, topo->logical_cores, topo->p_cores, topo->e_cores,
        topo->is_heterogeneous ? "YES" : "NO",
        topo->cpu_cur_freq_mhz, topo->is_upclocker_active ? "ACTIVE (Max P-State Boost)" : "NORMAL");
    strncpy(topo->summary, buf, sizeof(topo->summary) - 1);
}

static sf_hardware_topology_t sf_detect_hardware_topology(const char* isa_desc) {
    sf_hardware_topology_t topo{};
    topo.primary_cacheline_size = 64;
    topo.page_size = 4096;
    topo.huge_page_size = 2 * 1024 * 1024;
    topo.numa_nodes = 1;

#if defined(_WIN32)

    DWORD len = 0;
    GetLogicalProcessorInformationEx(RelationAll, nullptr, &len);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER && len > 0) {
        std::vector<uint8_t> buffer(len);
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX p = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer.data();
        if (GetLogicalProcessorInformationEx(RelationAll, p, &len)) {
            DWORD offset = 0;
            uint32_t eff_class_0_count = 0;
            uint32_t eff_class_high_count = 0;
            uint32_t numa_count = 0;

            while (offset < len) {
                PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX item = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)(buffer.data() + offset);
                if (item->Relationship == RelationCache) {
                    uint8_t lvl = item->Cache.Level;
                    uint32_t sz_kb = (uint32_t)(item->Cache.CacheSize / 1024);
                    uint16_t line_sz = (uint16_t)item->Cache.LineSize;
                    uint8_t assoc = (uint8_t)item->Cache.Associativity;

                    if (line_sz > 0) topo.primary_cacheline_size = line_sz;

                    if (lvl == 1) {
                        if (item->Cache.Type == CacheInstruction) {
                            topo.l1i.size_kb = sz_kb;
                            topo.l1i.line_size = line_sz;
                            topo.l1i.associativity = assoc;
                            topo.l1i.type = 2;
                        } else {
                            topo.l1d.size_kb = sz_kb;
                            topo.l1d.line_size = line_sz;
                            topo.l1d.associativity = assoc;
                            topo.l1d.type = 1;
                        }
                    } else if (lvl == 2) {
                        topo.l2.size_kb = sz_kb;
                        topo.l2.line_size = line_sz;
                        topo.l2.associativity = assoc;
                        topo.l2.type = (item->Cache.Type == CacheData) ? 1 : 3;
                    } else if (lvl == 3) {
                        topo.l3.size_kb = sz_kb;
                        topo.l3.line_size = line_sz;
                        topo.l3.associativity = assoc;
                        topo.l3.type = 3;
                    }
                } else if (item->Relationship == RelationNumaNode) {
                    numa_count++;
                } else if (item->Relationship == RelationProcessorCore) {
                    topo.physical_cores++;
                    if (item->Processor.EfficiencyClass == 0) {
                        eff_class_0_count++;
                    } else {
                        eff_class_high_count++;
                    }
                }
                offset += item->Size;
            }

            if (numa_count > 0) topo.numa_nodes = (uint8_t)numa_count;

            if (eff_class_0_count > 0 && eff_class_high_count > 0) {
                topo.is_heterogeneous = 1;
                topo.p_cores = eff_class_high_count;
                topo.e_cores = eff_class_0_count;
            } else {
                topo.p_cores = topo.physical_cores;
                topo.e_cores = 0;
            }
        }
    }

#  if defined(_M_X64) || defined(__x86_64__)

    if (topo.l1d.size_kb == 0 || topo.l2.size_kb == 0) {
        int cinfo[4] = {0};
        for (int sub = 0; sub < 8; ++sub) {
            sf_get_cpuid(cinfo, 4, sub);
            int ctype = cinfo[0] & 0x1F;
            if (ctype == 0) break;
            int clvl = (cinfo[0] >> 5) & 0x7;
            int line_sz = (cinfo[1] & 0xFFF) + 1;
            int partitions = ((cinfo[1] >> 12) & 0x3FF) + 1;
            int ways = ((cinfo[1] >> 22) & 0x3FF) + 1;
            int sets = cinfo[2] + 1;
            uint32_t sz_kb = (uint32_t)(((uint64_t)ways * partitions * line_sz * sets) / 1024);

            if (line_sz > 0) topo.primary_cacheline_size = (uint16_t)line_sz;
            if (clvl == 1) {
                if (ctype == 2) { topo.l1i.size_kb = sz_kb; topo.l1i.line_size = (uint16_t)line_sz; topo.l1i.associativity = (uint8_t)ways; topo.l1i.type = 2; }
                else { topo.l1d.size_kb = sz_kb; topo.l1d.line_size = (uint16_t)line_sz; topo.l1d.associativity = (uint8_t)ways; topo.l1d.type = 1; }
            } else if (clvl == 2) {
                topo.l2.size_kb = sz_kb; topo.l2.line_size = (uint16_t)line_sz; topo.l2.associativity = (uint8_t)ways; topo.l2.type = 3;
            } else if (clvl == 3) {
                topo.l3.size_kb = sz_kb; topo.l3.line_size = (uint16_t)line_sz; topo.l3.associativity = (uint8_t)ways; topo.l3.type = 3;
            }
        }
    }
#  endif

    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof(ms);
    if (GlobalMemoryStatusEx(&ms)) {
        topo.total_phys_ram_bytes = ms.ullTotalPhys;
        topo.avail_phys_ram_bytes = ms.ullAvailPhys;
    }

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    topo.page_size = si.dwPageSize;
    topo.logical_cores = si.dwNumberOfProcessors;
    if (topo.physical_cores == 0) topo.physical_cores = topo.logical_cores;
    if (topo.p_cores == 0) topo.p_cores = topo.physical_cores;

    SIZE_T lp_min = GetLargePageMinimum();
    if (lp_min > 0) {
        topo.huge_page_size = (uint32_t)lp_min;
        topo.huge_pages_supported = 1;
    }

#elif defined(__linux__) || defined(__ANDROID__)

    long l1d_sz = sysconf(_SC_LEVEL1_DCACHE_SIZE);
    long l1i_sz = sysconf(_SC_LEVEL1_ICACHE_SIZE);
    long l2_sz  = sysconf(_SC_LEVEL2_CACHE_SIZE);
    long l3_sz  = sysconf(_SC_LEVEL3_CACHE_SIZE);
    long cl_sz  = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);

    if (l1d_sz > 0) { topo.l1d.size_kb = (uint32_t)(l1d_sz / 1024); topo.l1d.type = 1; }
    if (l1i_sz > 0) { topo.l1i.size_kb = (uint32_t)(l1i_sz / 1024); topo.l1i.type = 2; }
    if (l2_sz > 0)  { topo.l2.size_kb  = (uint32_t)(l2_sz / 1024); topo.l2.type = 3; }
    if (l3_sz > 0)  { topo.l3.size_kb  = (uint32_t)(l3_sz / 1024); topo.l3.type = 3; }
    if (cl_sz > 0)  topo.primary_cacheline_size = (uint16_t)cl_sz;

    FILE* fp = fopen("/proc/meminfo", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "MemTotal:", 9) == 0) {
                uint64_t kb = 0;
                if (sscanf(line + 9, "%llu", &kb) == 1) topo.total_phys_ram_bytes = kb * 1024;
            } else if (strncmp(line, "MemAvailable:", 13) == 0) {
                uint64_t kb = 0;
                if (sscanf(line + 13, "%llu", &kb) == 1) topo.avail_phys_ram_bytes = kb * 1024;
            } else if (strncmp(line, "Hugepagesize:", 13) == 0) {
                uint64_t kb = 0;
                if (sscanf(line + 13, "%llu", &kb) == 1) {
                    topo.huge_page_size = (uint32_t)(kb * 1024);
                    topo.huge_pages_supported = 1;
                }
            }
        }
        fclose(fp);
    }
    long ps = sysconf(_SC_PAGESIZE);
    if (ps > 0) topo.page_size = (uint32_t)ps;
    long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    if (nprocs > 0) {
        topo.logical_cores = (uint32_t)nprocs;
        topo.physical_cores = topo.logical_cores;
        topo.p_cores = topo.physical_cores;
    }

    if (access("/sys/devices/system/node/node1", F_OK) == 0) {
        uint8_t nodes = 0;
        char npath[64];
        while (nodes < 16) {
            snprintf(npath, sizeof(npath), "/sys/devices/system/node/node%u", nodes);
            if (access(npath, F_OK) == 0) nodes++;
            else break;
        }
        if (nodes > 0) topo.numa_nodes = nodes;
    }

    if (access("/sys/devices/soc0/family", R_OK) == 0 || access("/proc/device-tree/model", R_OK) == 0) {
        topo.is_uma = 1;
    }

#elif defined(__APPLE__)

    size_t val64 = 0;
    size_t len = sizeof(val64);
    if (sysctlbyname("hw.l1dcachesize", &val64, &len, NULL, 0) == 0) { topo.l1d.size_kb = (uint32_t)(val64 / 1024); topo.l1d.type = 1; }
    len = sizeof(val64);
    if (sysctlbyname("hw.l1icachesize", &val64, &len, NULL, 0) == 0) { topo.l1i.size_kb = (uint32_t)(val64 / 1024); topo.l1i.type = 2; }
    len = sizeof(val64);
    if (sysctlbyname("hw.l2cachesize", &val64, &len, NULL, 0) == 0)  { topo.l2.size_kb  = (uint32_t)(val64 / 1024); topo.l2.type = 3; }
    len = sizeof(val64);
    if (sysctlbyname("hw.cachelinesize", &val64, &len, NULL, 0) == 0) topo.primary_cacheline_size = (uint16_t)val64;
    len = sizeof(val64);
    if (sysctlbyname("hw.memsize", &val64, &len, NULL, 0) == 0) topo.total_phys_ram_bytes = val64;
    len = sizeof(val64);
    if (sysctlbyname("hw.pagesize", &val64, &len, NULL, 0) == 0) page_size = (uint32_t)val64;

    int pcores = 0, ecores = 0;
    len = sizeof(pcores);
    if (sysctlbyname("hw.perflevel0.physicalcpu", &pcores, &len, NULL, 0) == 0 && pcores > 0) topo.p_cores = (uint32_t)pcores;
    len = sizeof(ecores);
    if (sysctlbyname("hw.perflevel1.physicalcpu", &ecores, &len, NULL, 0) == 0 && ecores > 0) topo.e_cores = (uint32_t)ecores;
    if (topo.p_cores > 0 && topo.e_cores > 0) topo.is_heterogeneous = 1;

    int ncpu = 0;
    len = sizeof(ncpu);
    if (sysctlbyname("hw.logicalcpu", &ncpu, &len, NULL, 0) == 0) topo.logical_cores = (uint32_t)ncpu;
    len = sizeof(ncpu);
    if (sysctlbyname("hw.physicalcpu", &ncpu, &len, NULL, 0) == 0) topo.physical_cores = (uint32_t)ncpu;
    topo.is_uma = 1;
#endif

    if (topo.l1d.size_kb == 0) { topo.l1d.size_kb = 32; topo.l1d.line_size = 64; topo.l1d.associativity = 8; topo.l1d.type = 1; }
    if (topo.l1i.size_kb == 0) { topo.l1i.size_kb = 32; topo.l1i.line_size = 64; topo.l1i.associativity = 8; topo.l1i.type = 2; }
    if (topo.l2.size_kb == 0)  { topo.l2.size_kb = 512; topo.l2.line_size = 64; topo.l2.associativity = 8; topo.l2.type = 3; }
    if (topo.primary_cacheline_size == 0) topo.primary_cacheline_size = 64;
    if (topo.page_size == 0) topo.page_size = 4096;
    if (topo.physical_cores == 0) topo.physical_cores = 1;
    if (topo.logical_cores == 0) topo.logical_cores = topo.physical_cores;
    if (topo.p_cores == 0) topo.p_cores = topo.physical_cores;

    if (topo.is_uma) {
#if defined(__APPLE__)
        if (topo.total_phys_ram_bytes >= 96ULL * 1024 * 1024 * 1024) {
            topo.memory_channels = 16;
            topo.bus_width_bits = 1024;
            topo.estimated_bandwidth_gbps = 800;
        } else if (topo.total_phys_ram_bytes >= 32ULL * 1024 * 1024 * 1024) {
            topo.memory_channels = 8;
            topo.bus_width_bits = 512;
            topo.estimated_bandwidth_gbps = 300;
        } else {
            topo.memory_channels = 2;
            topo.bus_width_bits = 128;
            topo.estimated_bandwidth_gbps = 100;
        }
#else
        if (topo.total_phys_ram_bytes >= 16ULL * 1024 * 1024 * 1024) {
            topo.memory_channels = 4;
            topo.bus_width_bits = 256;
            topo.estimated_bandwidth_gbps = 204;
        } else {
            topo.memory_channels = 1;
            topo.bus_width_bits = 32;
            topo.estimated_bandwidth_gbps = 20;
        }
#endif
    } else {
        if (topo.numa_nodes > 1 || topo.total_phys_ram_bytes >= 64ULL * 1024 * 1024 * 1024) {
            topo.memory_channels = (uint8_t)(4 * (topo.numa_nodes > 0 ? topo.numa_nodes : 1));
            if (topo.memory_channels > 16) topo.memory_channels = 16;
            topo.bus_width_bits = topo.memory_channels * 64;
            topo.estimated_bandwidth_gbps = topo.memory_channels * 25;
        } else {
            topo.memory_channels = 2;
            topo.bus_width_bits = 128;
            topo.estimated_bandwidth_gbps = 60;
        }
    }

    sf_parse_smbios(&topo);
    topo.cpu_cur_freq_mhz = sf_measure_cpu_cur_freq_mhz();

    sf_format_hardware_topology_summary(&topo);
    return topo;
}

struct sf_cpu_isa_inventory_t {
    std::vector<std::string> discovered_isas;
    sf_hardware_topology_t  topology{};
    char description[2048] = {0};

    bool has_isa(const char* token) const {
        if (!token || !*token) return false;
        if (_stricmp(token, "AUTO") == 0 || _stricmp(token, "ANY") == 0 || _stricmp(token, "SCALAR") == 0) {
            return true;
        }
        for (const auto& item : discovered_isas) {
            if (_stricmp(item.c_str(), token) == 0) return true;
        }
        return false;
    }

    void add_isa(const char* token) {
        if (!token || !*token || has_isa(token)) return;
        discovered_isas.emplace_back(token);
    }

    std::string resolve_fallback(const char* requested_isa) const {
        if (!requested_isa || !*requested_isa) return "SCALAR";
        if (has_isa(requested_isa)) return requested_isa;

        const char* cur = requested_isa;
        while (cur && _stricmp(cur, "SCALAR") != 0) {
            const auto* e = sf_find_world_isa(cur);
            if (!e || !e->fallback_isa) break;
            cur = e->fallback_isa;
            if (has_isa(cur)) return cur;
        }
        return "SCALAR";
    }

    void parse_cpu_string(const char* text) {
        if (!text || !*text) return;
        const char* p = text;
        while (*p) {
            while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' || *p == ',' || *p == ';' || *p == ':')) {
                ++p;
            }
            if (!*p) break;
            const char* start = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' && *p != ',' && *p != ';' && *p != ':') {
                ++p;
            }
            std::string tok(start, p - start);
            if (!tok.empty()) {
                const auto* matched = sf_find_world_isa(tok.c_str());
                if (matched) {
                    add_isa(matched->isa_name);
                } else {
                    if (_stricmp(tok.c_str(), "fma") == 0) add_isa("FMA3");
                    else if (_stricmp(tok.c_str(), "avx512_vnni") == 0) add_isa("AVX512VNNI");
                    else if (_stricmp(tok.c_str(), "avx512_bf16") == 0) add_isa("AVX512_BF16");
                    else if (_stricmp(tok.c_str(), "avx512_bitalg") == 0) add_isa("AVX512BITALG");
                    else if (_stricmp(tok.c_str(), "avx512_vbmi") == 0) add_isa("AVX512VBMI");
                    else if (_stricmp(tok.c_str(), "avx512_vbmi2") == 0) add_isa("AVX512VBMI2");
                    else if (_stricmp(tok.c_str(), "avx512_vpopcntdq") == 0) add_isa("AVX512VPOPCNTDQ");
                    else if (_stricmp(tok.c_str(), "asimd") == 0) add_isa("NEON");
                    else if (_stricmp(tok.c_str(), "asimddp") == 0) add_isa("ARM_DOTPROD");
                    else if (_stricmp(tok.c_str(), "asimdhp") == 0) add_isa("ARM_FP16");
                    else if (_stricmp(tok.c_str(), "atomics") == 0) add_isa("ARM_LSE_ATOMICS");
                }
            }
        }
    }

    void build_description() {
        std::string s = "Discovered ISAs (" + std::to_string(discovered_isas.size()) + "): [";
        for (size_t i = 0; i < discovered_isas.size(); ++i) {
            if (i > 0) s += ", ";
            s += discovered_isas[i];
        }
        s += "]\n";
        s += topology.summary;
        strncpy(description, s.c_str(), sizeof(description) - 1);
    }

    static sf_cpu_isa_inventory_t discover(void) {
        sf_cpu_isa_inventory_t inv;

#if defined(_M_X64) || defined(__x86_64__)
        int info[4] = {0};
        sf_get_cpuid(info, 0, 0);
        int max_leaf = info[0];

        if (max_leaf >= 1) {
            sf_get_cpuid(info, 1, 0);
            if (info[3] & (1 << 23)) inv.add_isa("MMX");
            if (info[3] & (1 << 25)) inv.add_isa("SSE");
            if (info[3] & (1 << 26)) inv.add_isa("SSE2");
            if (info[2] & (1 << 0))  inv.add_isa("SSE3");
            if (info[2] & (1 << 9))  inv.add_isa("SSSE3");
            if (info[2] & (1 << 19)) inv.add_isa("SSE4_1");
            if (info[2] & (1 << 20)) inv.add_isa("SSE4_2");
            if (info[2] & (1 << 12)) inv.add_isa("FMA3");
            if (info[2] & (1 << 28)) inv.add_isa("AVX");
            if (info[2] & (1 << 29)) inv.add_isa("F16C");

            uint32_t model = ((info[0] >> 4) & 0xF) | (((info[0] >> 16) & 0xF) << 4);
            uint32_t family = ((info[0] >> 8) & 0xF);
            if (family == 6) {
                if (model == 0x37 || model == 0x4C || model == 0x5C || model == 0x7A || model == 0x86 || model == 0x96 || model == 0xBE) {
                    inv.add_isa("INTEL_ATOM_LOW_POWER");
                    inv.add_isa("STREAMING_NT_FAVORED");
                }
            }
        }

        if (max_leaf >= 7) {
            sf_get_cpuid(info, 7, 0);
            if (info[1] & (1 << 3))  inv.add_isa("BMI1");
            if (info[1] & (1 << 5))  inv.add_isa("AVX2");
            if (info[1] & (1 << 8))  inv.add_isa("BMI2");
            if (info[1] & (1 << 16)) inv.add_isa("AVX512F");
            if (info[1] & (1 << 17)) inv.add_isa("AVX512DQ");
            if (info[1] & (1 << 28)) inv.add_isa("AVX512CD");
            if (info[1] & (1 << 30)) inv.add_isa("AVX512BW");
            if (info[1] & (1 << 31)) inv.add_isa("AVX512VL");
            if (info[2] & (1 << 1))  inv.add_isa("AVX512VBMI");
            if (info[2] & (1 << 6))  inv.add_isa("AVX512VBMI2");
            if (info[2] & (1 << 11)) inv.add_isa("AVX512VNNI");
            if (info[2] & (1 << 12)) inv.add_isa("AVX512BITALG");
            if (info[2] & (1 << 14)) inv.add_isa("AVX512VPOPCNTDQ");
            if (info[3] & (1 << 22)) inv.add_isa("AMX_BF16");
            if (info[3] & (1 << 23)) inv.add_isa("AVX512FP16");
            if (info[3] & (1 << 24)) inv.add_isa("AMX_TILE");
            if (info[3] & (1 << 25)) inv.add_isa("AMX_INT8");

            sf_get_cpuid(info, 7, 1);
            if (info[0] & (1 << 4))  inv.add_isa("AVX_VNNI");
            if (info[0] & (1 << 5))  inv.add_isa("AVX512_BF16");
            if (info[3] & (1 << 19)) inv.add_isa("AVX10");
            if (info[3] & (1 << 21)) inv.add_isa("APX_F");
        }

        sf_get_cpuid(info, 0x80000000, 0);
        unsigned int max_ext = (unsigned int)info[0];
        if (max_ext >= 0x80000001) {
            sf_get_cpuid(info, 0x80000001, 0);
            if (info[2] & (1 << 6))  inv.add_isa("SSE4A");
            if (info[2] & (1 << 11)) inv.add_isa("XOP");
            if (info[2] & (1 << 16)) inv.add_isa("FMA4");
        }
#elif defined(__aarch64__) || defined(_M_ARM64)
        inv.add_isa("NEON");
        inv.add_isa("ASIMD");
#  if defined(__linux__) && defined(__aarch64__)
#    if defined(AT_HWCAP)
        unsigned long hwc = getauxval(AT_HWCAP);
        if (hwc & HWCAP_FP)       inv.add_isa("ARM_FP");
        if (hwc & HWCAP_ASIMD)    inv.add_isa("ARM_ASIMD");
#      if defined(HWCAP_FPHP)
        if (hwc & HWCAP_FPHP)     inv.add_isa("ARM_FP16");
#      endif
#      if defined(HWCAP_ASIMDDP)
        if (hwc & HWCAP_ASIMDDP)  inv.add_isa("ARM_DOTPROD");
#      endif
#      if defined(HWCAP_SVE)
        if (hwc & HWCAP_SVE)      inv.add_isa("ARM_SVE");
#      endif
#      if defined(HWCAP_ATOMICS)
        if (hwc & HWCAP_ATOMICS)  inv.add_isa("ARM_LSE_ATOMICS");
#      endif
#    endif

        if (access("/sys/devices/soc0/family", R_OK) == 0) {
            FILE* f = fopen("/sys/devices/soc0/family", "r");
            if (f) {
                char buf[64] = {0};
                if (fgets(buf, sizeof(buf), f)) {
                    if (strstr(buf, "Tegra") || strstr(buf, "tegra")) {
                        inv.add_isa("NVIDIA_TEGRA_UMA_ZEROCOPY");
                    }
                }
                fclose(f);
            }
        }
        if (access("/proc/device-tree/model", R_OK) == 0) {
            FILE* f = fopen("/proc/device-tree/model", "r");
            if (f) {
                char buf[128] = {0};
                if (fgets(buf, sizeof(buf), f)) {
                    if (strstr(buf, "Raspberry Pi")) {
                        inv.add_isa("BROADCOM_RPI_ARM");
                    }
                }
                fclose(f);
            }
        }
#  endif
#  if defined(__APPLE__)
        inv.add_isa("APPLE_SILICON_ARM");
        inv.add_isa("APPLE_128B_CACHELINE");
        inv.add_isa("APPLE_UMA_ZEROCOPY");
#  endif
#  if defined(__ARM_FEATURE_SVE)
        inv.add_isa("SVE");
#  endif
#  if defined(__ARM_FEATURE_SVE2)
        inv.add_isa("SVE2");
#  endif
#  if defined(__ARM_FEATURE_MATMUL_INT8)
        inv.add_isa("SME");
#  endif
#elif defined(__riscv)
        inv.add_isa("RV64");
#  if defined(__riscv_vector)
        inv.add_isa("RVV");
#  endif
#elif defined(__s390x__) || defined(__s390__)
        inv.add_isa("S390X");
#  if defined(__linux__) && defined(AT_HWCAP)
        unsigned long hwc = getauxval(AT_HWCAP);
#    if defined(HWCAP_S390_VX)
        if (hwc & HWCAP_S390_VX)   inv.add_isa("S390_VX");
#    endif
#    if defined(HWCAP_S390_VXE)
        if (hwc & HWCAP_S390_VXE)  inv.add_isa("S390_VXE");
#    endif
#  endif
#endif

#if defined(_WIN32)
        if (IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE)) inv.add_isa("PF_MMX");
        if (IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE)) inv.add_isa("PF_SSE");
        if (IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE)) inv.add_isa("PF_SSE2");
        if (IsProcessorFeaturePresent(PF_SSE3_INSTRUCTIONS_AVAILABLE)) inv.add_isa("PF_SSE3");
        if (IsProcessorFeaturePresent(PF_ARM_V8_INSTRUCTIONS_AVAILABLE)) inv.add_isa("ARM_V8");
        if (IsProcessorFeaturePresent(PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE)) inv.add_isa("ARM_CRYPTO");
#  if defined(PF_ARM_V81_ATOMIC_INSTRUCTIONS_AVAILABLE)
        if (IsProcessorFeaturePresent(PF_ARM_V81_ATOMIC_INSTRUCTIONS_AVAILABLE)) inv.add_isa("ARM_V81");
#  endif
#  if defined(PF_AVX_INSTRUCTIONS_AVAILABLE)
        if (IsProcessorFeaturePresent(PF_AVX_INSTRUCTIONS_AVAILABLE)) inv.add_isa("PF_AVX");
#  endif
#  if defined(PF_AVX2_INSTRUCTIONS_AVAILABLE)
        if (IsProcessorFeaturePresent(PF_AVX2_INSTRUCTIONS_AVAILABLE)) inv.add_isa("PF_AVX2");
#  endif
#  if defined(PF_AVX512F_INSTRUCTIONS_AVAILABLE)
        if (IsProcessorFeaturePresent(PF_AVX512F_INSTRUCTIONS_AVAILABLE)) inv.add_isa("PF_AVX512F");
#  endif
#endif

        const char* custom_isas = getenv("SF_EXTRA_ISAS");
        if (custom_isas) {
            std::string cur = "";
            for (const char* p = custom_isas; *p; ++p) {
                if (*p == ',' || *p == ';' || *p == ' ' || *p == '|') {
                    if (!cur.empty()) { inv.add_isa(cur.c_str()); cur.clear(); }
                } else {
                    cur += *p;
                }
            }
            if (!cur.empty()) inv.add_isa(cur.c_str());
        }

#if !defined(_WIN32)
        if (access("/proc/cpuinfo", R_OK) == 0) {
            FILE* f = fopen("/proc/cpuinfo", "r");
            if (f) {
                char line[512];
                while (fgets(line, sizeof(line), f)) {
                    if (strstr(line, "flags") || strstr(line, "Features") || strstr(line, "isa")) {
                        inv.parse_cpu_string(line);
                    }
                }
                fclose(f);
            }
        }
#endif
        const char* text_override = getenv("SF_CPU_INFO_TEXT");
        if (text_override) {
            inv.parse_cpu_string(text_override);
        }

        inv.topology = sf_detect_hardware_topology(inv.has_isa("APPLE_SILICON_ARM") ? "APPLE" : (inv.has_isa("NVIDIA_TEGRA_UMA_ZEROCOPY") ? "TEGRA" : (inv.has_isa("BROADCOM_RPI_ARM") ? "BROADCOM" : "")));
        inv.build_description();
        return inv;
    }
};

static inline bool sf_hardware_should_stream_store(size_t tensor_bytes, const sf_hardware_topology_t* topo) {
    if (!topo) return false;
    size_t llc_bytes = (topo->l3.size_kb > 0 ? topo->l3.size_kb : topo->l2.size_kb) * 1024ULL;

    return tensor_bytes > llc_bytes;
}

static inline void sf_hardware_optimal_tile_sizes(size_t elem_size, int& tile_M, int& tile_N, int& tile_K,
                                                  const sf_hardware_topology_t* topo) {
    uint32_t l1d_b = (topo && topo->l1d.size_kb > 0) ? (topo->l1d.size_kb * 1024) : (32 * 1024);

    size_t target_block_bytes = l1d_b / 3;
    int dim = (int)sqrt((double)(target_block_bytes / (elem_size > 0 ? elem_size : 4)));
    if (dim < 16) dim = 16;
    if (dim > 128) dim = 128;

    dim = (dim / 16) * 16;
    if (dim == 0) dim = 16;
    tile_M = dim;
    tile_N = dim;
    tile_K = dim;
}

#if defined(_WIN32)
static inline bool sf_safe_kernel_call_impl(sacculus_kernel_fn_t fn,
                                            const SF_custominiVm_tensor_t* in_t, int nin,
                                            SF_custominiVm_tensor_t* out_t, int nout,
                                            void* ctx, uint32_t* fault_code) {
    __try {
        fn(in_t, nin, out_t, nout, ctx);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        if (fault_code) *fault_code = (uint32_t)GetExceptionCode();
        return false;
    }
}
static inline bool sf_safe_kernel_call(sacculus_kernel_fn_t fn,
                                       const SF_custominiVm_tensor_t* in_t, int nin,
                                       SF_custominiVm_tensor_t* out_t, int nout,
                                       void* ctx, uint32_t* fault_code = nullptr) {
    if (!fn) return false;
    return sf_safe_kernel_call_impl(fn, in_t, nin, out_t, nout, ctx, fault_code);
}
#else

#include <setjmp.h>
#include <signal.h>

static thread_local sigjmp_buf  sf_kernel_jmpbuf;
static thread_local bool        sf_kernel_in_safe_call = false;
static thread_local uint32_t    sf_kernel_caught_sig   = 0;

static void sf_kernel_signal_handler(int sig) {
    if (sf_kernel_in_safe_call) {
        sf_kernel_caught_sig = (uint32_t)sig;
        siglongjmp(sf_kernel_jmpbuf, 1);
    }

    signal(sig, SIG_DFL);
    raise(sig);
}

static inline bool sf_safe_kernel_call(sacculus_kernel_fn_t fn,
                                       const SF_custominiVm_tensor_t* in_t, int nin,
                                       SF_custominiVm_tensor_t* out_t, int nout,
                                       void* ctx, uint32_t* fault_code = nullptr) {
    if (!fn) return false;

    struct sigaction sa_ill{}, sa_segv{}, sa_bus{};
    struct sigaction old_ill{}, old_segv{}, old_bus{};
    sa_ill.sa_handler  = sf_kernel_signal_handler;
    sa_segv.sa_handler = sf_kernel_signal_handler;
    sa_bus.sa_handler  = sf_kernel_signal_handler;
    sigemptyset(&sa_ill.sa_mask);
    sigemptyset(&sa_segv.sa_mask);
    sigemptyset(&sa_bus.sa_mask);
    sa_ill.sa_flags  = SA_RESETHAND;
    sa_segv.sa_flags = SA_RESETHAND;
    sa_bus.sa_flags  = SA_RESETHAND;
    sigaction(SIGILL,  &sa_ill,  &old_ill);
    sigaction(SIGSEGV, &sa_segv, &old_segv);
    sigaction(SIGBUS,  &sa_bus,  &old_bus);

    sf_kernel_in_safe_call = true;
    sf_kernel_caught_sig   = 0;
    bool ok = false;

    if (sigsetjmp(sf_kernel_jmpbuf, 1) == 0) {
        fn(in_t, nin, out_t, nout, ctx);
        ok = true;
    } else {

        if (fault_code) *fault_code = sf_kernel_caught_sig;
        ok = false;
    }

    sf_kernel_in_safe_call = false;

    sigaction(SIGILL,  &old_ill,  nullptr);
    sigaction(SIGSEGV, &old_segv, nullptr);
    sigaction(SIGBUS,  &old_bus,  nullptr);

    return ok;
}
#endif

static std::atomic<bool> g_stream_pause_gate{false};

struct sf_puzzle_sample_t {
    uint8_t dtype = SACFOR_F32;
    size_t byte_size = 0;
    int32_t shape[8] = {1, 0, 0, 0, 0, 0, 0, 0};
    int32_t ndim = 1;
    alignas(64) uint8_t data[128 * 1024];
};

struct sf_puzzle_batch_job_t {
    uint32_t fn_id = 0;
    std::vector<sf_puzzle_sample_t> samples;
    SF_custominiVm_tensor_t target_precision_tensor = {};
    bool has_target_precision = false;
    float target_epsilon = 1e-4f;
};

struct sf_puzzle_experiment_t {
    uint32_t              fn_id             = 0;
    char                  name[32]          = {0};
    sacculus_kernel_fn_t  candidate         = nullptr;
    char                  target_isa[16]    = {0};
    float                 target_epsilon    = 1e-4f;
    SF_custominiVm_tensor_t target_tensor   = {};
    bool                  has_target_tensor = false;
};

struct sacculus_puzzle_arena_t {
    sacculus_dispatch_table_t* dispatch = nullptr;
    sacculus_runway_t*         runway   = nullptr;
    sacculus_hotlog_t*         hotlog   = nullptr;
    sacculus_coldlog_t*        coldlog  = nullptr;

    std::thread                thread;
    std::atomic<bool>          running{false};
    std::atomic<uint8_t>       execution_mode{static_cast<uint8_t>(SF_PUZZLE_EXEC_ADAPTIVE)};
    std::atomic<size_t>        stream_sample_count{5};

    std::mutex                          queue_lock;
    std::vector<sf_puzzle_experiment_t> pending_experiments;
    std::vector<sf_puzzle_batch_job_t>  pending_batch_jobs;

    static constexpr size_t STAGNANT_BUF_CAP = 128 * 1024;
    uint8_t* puzzle_heap_base      = nullptr;
    size_t   puzzle_heap_capacity  = 0;
    std::atomic<size_t> puzzle_heap_used{0};

    uint8_t* stagnant_in_payload      = nullptr;
    uint8_t* candidate_out_payload    = nullptr;
    uint8_t* current_best_out_payload = nullptr;

    sf_cpu_isa_inventory_t     cached_isa_inventory;

    std::vector<std::string>   active_simd_pool;

    struct sf_candidate_eval_t {
        int                  idx               = -1;
        sacculus_kernel_fn_t fn                = nullptr;
        char                 variant_name[32]  = {0};
        char                 isa_tag[16]       = {0};
        bool                 executed_ok       = false;
        uint32_t             fault_code        = 0;
        uint64_t             cycles            = UINT64_MAX;
        float                max_abs_err       = 1e9f;
        float                mse               = 1e9f;
        bool                 precision_passed  = false;
        bool                 speed_passed      = false;
        double               score             = -1.0;
        bool                 is_hybrid         = false;
    };

    struct sf_hybrid_compat_result_t {
        bool   compatible = false;
        double mean_expected = 0.0;
        double mean_hybrid = 0.0;
        double dynamic_bound = 1.0;
        double observed_ratio = 1.0;
        char   reason[64] = {0};
    };

    struct sf_score_eval_t {
        bool   qualified = false;
        double score = -1.0;
        double speedup_ratio = 0.0;
        double precision_headroom = 0.0;
        float  max_err = 0.0f;
        float  mse = 0.0f;
    };

    void init(sacculus_dispatch_table_t* dt,
              sacculus_runway_t* rw,
              sacculus_hotlog_t* hl,
              sacculus_coldlog_t* cl,
              size_t arena_bytes = 16ULL * 1024 * 1024) {
        dispatch = dt;
        runway   = rw;
        hotlog   = hl;
        coldlog  = cl;

        puzzle_heap_capacity = arena_bytes;
        puzzle_heap_base = (uint8_t*)sf_os_reserve_memory(puzzle_heap_capacity);
        if (puzzle_heap_base && sf_os_commit_memory(puzzle_heap_base, puzzle_heap_capacity)) {
            sf_pretouch_memory(puzzle_heap_base, puzzle_heap_capacity);
            stagnant_in_payload      = puzzle_heap_base;
            candidate_out_payload    = puzzle_heap_base + STAGNANT_BUF_CAP;
            current_best_out_payload = puzzle_heap_base + STAGNANT_BUF_CAP * 2;
            puzzle_heap_used.store(STAGNANT_BUF_CAP * 3, std::memory_order_relaxed);
        }

        refresh_simd_pool();
        if (coldlog) {
            sacculus_coldlog_write(coldlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_INFO, 0, "puzzle_arena_init_onheap");
        }
    }

    void refresh_simd_pool() {
        cached_isa_inventory = sf_cpu_isa_inventory_t::discover();
        active_simd_pool.clear();
        for (const auto& isa : cached_isa_inventory.discovered_isas) {
            if (_stricmp(isa.c_str(), "AUTO") != 0 && _stricmp(isa.c_str(), "ANY") != 0) {
                active_simd_pool.push_back(isa);
            }
        }
        if (active_simd_pool.empty()) {
            active_simd_pool.push_back("SCALAR");
        }
    }

    std::vector<std::string> pool_simd_capabilities() {
        refresh_simd_pool();
        return active_simd_pool;
    }

    void set_mode(SF_custominiVm_puzzle_exec_mode_t mode) {
        execution_mode.store(static_cast<uint8_t>(mode), std::memory_order_relaxed);
    }

    SF_custominiVm_puzzle_exec_mode_t get_mode(void) const {
        return static_cast<SF_custominiVm_puzzle_exec_mode_t>(execution_mode.load(std::memory_order_relaxed));
    }

    void set_sample_count(size_t count) {
        stream_sample_count.store(count > 0 ? count : 1, std::memory_order_relaxed);
    }

    size_t get_sample_count(void) const {
        return stream_sample_count.load(std::memory_order_relaxed);
    }

    void identify_stream_types(std::vector<uint8_t>& detected_dtypes) {
        detected_dtypes.clear();
        uint32_t dtype_mask = 0;
        if (!runway) {
            detected_dtypes.push_back(SACFOR_F32);
            return;
        }

        auto scan_room = [&](int r, int reg) {
            if (r >= SACFOR_ROOM_COUNT || reg >= SACFOR_REGION_COUNT) return;
            auto* curr = runway->motherline.rooms[r].regions[reg].get_head();
            size_t count = 0;
            while (curr && count++ < 32) {
                if (curr->try_pin()) {
                    uint8_t dt = SACFOR_F32;
                    auto* meta = curr->cold_meta.load(std::memory_order_relaxed);
                    if (meta) {
                        uint32_t h = (uint32_t)meta->type_sig_hash;
                        switch (h & 0x7) {
                            case 1: dt = SACFOR_F64; break;
                            case 2: dt = SACFOR_I32; break;
                            case 3: dt = SACFOR_I64; break;
                            case 4: dt = SACFOR_F16; break;
                            case 5: dt = SACFOR_U8;  break;
                            default: dt = SACFOR_F32; break;
                        }
                    }
                    curr->unpin();
                    if (!(dtype_mask & (1u << dt))) {
                        dtype_mask |= (1u << dt);
                        detected_dtypes.push_back(dt);
                    }
                }
                curr = curr->next;
            }
        };

        scan_room(SF_ROOM_YOUNG, SF_REGION_DEMAND);
        scan_room(SF_ROOM_YOUNG, SF_REGION_HOTFIELD);
        scan_room(SF_ROOM_YOUNG, SF_REGION_TANK);
        scan_room(SF_ROOM_ADOLESCENT, SF_REGION_DEMAND);

        if (detected_dtypes.empty()) {
            detected_dtypes.push_back(SACFOR_F32);
        }
    }

    bool sample_runway_stream(size_t sample_iterations, std::vector<sf_puzzle_sample_t>& aggregated_samples) {
        aggregated_samples.clear();
        std::vector<uint8_t> active_dtypes;
        identify_stream_types(active_dtypes);

        for (size_t it = 0; it < sample_iterations; ++it) {
            for (uint8_t dt : active_dtypes) {
                sf_puzzle_sample_t smp{};
                smp.dtype = dt;
                bool captured = false;

                if (runway) {
                    for (int r = 0; r < SACFOR_ROOM_COUNT && !captured; ++r) {
                        for (int reg = 0; reg < SACFOR_REGION_COUNT && !captured; ++reg) {
                            auto* curr = runway->motherline.rooms[r].regions[reg].get_head();
                            size_t depth = 0;
                            while (curr && depth++ < 16) {
                                if (curr->try_pin()) {
                                    size_t fp = (size_t)curr->memory_footprint.load(std::memory_order_relaxed);
                                    if (fp > 0 && fp <= STAGNANT_BUF_CAP) {
                                        memcpy(smp.data, curr->body(), fp);
                                        smp.byte_size = fp;
                                        smp.ndim = 2;
                                        size_t elem_sz = (dt == SACFOR_F64 || dt == SACFOR_I64) ? 8 : 4;
                                        smp.shape[0] = 1;
                                        smp.shape[1] = (int32_t)(fp / elem_sz);
                                        captured = true;
                                    }
                                    curr->unpin();
                                    if (captured) break;
                                }
                                curr = curr->next;
                            }
                        }
                    }
                }

                if (!captured) {

                    smp.byte_size = 1024;
                    smp.ndim = 2;
                    if (dt == SACFOR_F64) {
                        smp.shape[0] = 1; smp.shape[1] = 128;
                        double* dptr = reinterpret_cast<double*>(smp.data);
                        for (size_t i = 0; i < 128; ++i) dptr[i] = (double)(i + 1 + it) * 0.05;
                    } else {
                        smp.shape[0] = 1; smp.shape[1] = 256;
                        float* fptr = reinterpret_cast<float*>(smp.data);
                        for (size_t i = 0; i < 256; ++i) fptr[i] = (float)(i + 1 + it) * 0.05f;
                    }
                }
                aggregated_samples.push_back(smp);
            }
        }
        return !aggregated_samples.empty();
    }

    void submit_batch(uint32_t fn_id,
                      const SF_custominiVm_tensor_t* batch_samples,
                      int sample_count,
                      const SF_custominiVm_tensor_t* target_prec,
                      float target_epsilon) {
        if (!batch_samples || sample_count <= 0) return;
        std::lock_guard<std::mutex> g(queue_lock);
        sf_puzzle_batch_job_t job{};
        job.fn_id = fn_id;
        job.target_epsilon = target_epsilon > 0.0f ? target_epsilon : 1e-4f;
        if (target_prec && target_prec->data && target_prec->byte_size > 0) {
            job.target_precision_tensor = *target_prec;
            job.has_target_precision = true;
        }
        for (int i = 0; i < sample_count; ++i) {
            if (batch_samples[i].data && batch_samples[i].byte_size > 0) {
                sf_puzzle_sample_t smp{};
                smp.dtype = batch_samples[i].dtype;
                smp.byte_size = (batch_samples[i].byte_size <= STAGNANT_BUF_CAP) ? batch_samples[i].byte_size : STAGNANT_BUF_CAP;
                smp.ndim = batch_samples[i].ndim;
                for (int d = 0; d < 8; ++d) smp.shape[d] = batch_samples[i].shape[d];
                memcpy(smp.data, batch_samples[i].data, smp.byte_size);
                job.samples.push_back(smp);
            }
        }
        if (!job.samples.empty()) {
            pending_batch_jobs.push_back(job);
        }
    }

    bool acquire_evaluation_samples(uint32_t fn_id,
                                    std::vector<sf_puzzle_sample_t>& samples,
                                    SF_custominiVm_tensor_t& target_prec,
                                    bool& has_target_prec,
                                    float& target_epsilon) {
        {
            std::lock_guard<std::mutex> g(queue_lock);
            for (auto it = pending_batch_jobs.begin(); it != pending_batch_jobs.end(); ++it) {
                if (it->fn_id == fn_id) {
                    samples = it->samples;
                    target_prec = it->target_precision_tensor;
                    has_target_prec = it->has_target_precision;
                    target_epsilon = it->target_epsilon;
                    pending_batch_jobs.erase(it);
                    return true;
                }
            }
        }

        has_target_prec = false;
        target_epsilon = 1e-4f;
        size_t n_iters = stream_sample_count.load(std::memory_order_relaxed);
        return sample_runway_stream(n_iters > 0 ? n_iters : 5, samples);
    }

    sf_hybrid_compat_result_t evaluate_hybrid_compatibility(sacculus_kernel_fn_t fn_a,
                                                            sacculus_kernel_fn_t fn_b,
                                                            const SF_custominiVm_tensor_t* in_t,
                                                            SF_custominiVm_tensor_t* out_t) {
        sf_hybrid_compat_result_t res{};
        if (!fn_a || !fn_b) {
            snprintf(res.reason, sizeof(res.reason), "null_kernel");
            return res;
        }

        constexpr int RUNS = 8;
        uint64_t cycles_a[RUNS];
        uint64_t cycles_b[RUNS];
        uint64_t cycles_hybrid[RUNS];

        uint64_t min_a = UINT64_MAX, max_a = 0;
        double sum_a = 0.0;
        for (int r = 0; r < RUNS; ++r) {
            uint32_t fault = 0;
            uint64_t t0 = sacculus_rdtsc();
            if (!sf_safe_kernel_call(fn_a, in_t, 1, out_t, 1, nullptr, &fault)) {
                snprintf(res.reason, sizeof(res.reason), "fn_a_fault:0x%X", fault);
                return res;
            }
            uint64_t dt = sacculus_rdtsc() - t0;
            if (dt == 0) dt = 1;
            cycles_a[r] = dt;
            sum_a += dt;
            if (dt < min_a) min_a = dt;
            if (dt > max_a) max_a = dt;
        }
        double mu_a = sum_a / RUNS;
        double var_a = 0.0;
        for (int r = 0; r < RUNS; ++r) {
            double d = cycles_a[r] - mu_a;
            var_a += d * d;
        }
        var_a /= (RUNS > 1 ? (RUNS - 1) : 1);

        uint64_t min_b = UINT64_MAX, max_b = 0;
        double sum_b = 0.0;
        for (int r = 0; r < RUNS; ++r) {
            uint32_t fault = 0;
            uint64_t t0 = sacculus_rdtsc();
            if (!sf_safe_kernel_call(fn_b, in_t, 1, out_t, 1, nullptr, &fault)) {
                snprintf(res.reason, sizeof(res.reason), "fn_b_fault:0x%X", fault);
                return res;
            }
            uint64_t dt = sacculus_rdtsc() - t0;
            if (dt == 0) dt = 1;
            cycles_b[r] = dt;
            sum_b += dt;
            if (dt < min_b) min_b = dt;
            if (dt > max_b) max_b = dt;
        }
        double mu_b = sum_b / RUNS;
        double var_b = 0.0;
        for (int r = 0; r < RUNS; ++r) {
            double d = cycles_b[r] - mu_b;
            var_b += d * d;
        }
        var_b /= (RUNS > 1 ? (RUNS - 1) : 1);

        double mu_expected = (mu_a + mu_b) * 0.5;
        double noise_sigma = sqrt(var_a + var_b);
        double cv_sys = noise_sigma / (mu_expected > 0 ? mu_expected : 1.0);
        double jitter_ratio = (double)((max_a - min_a) + (max_b - min_b)) / (2.0 * (mu_expected > 0 ? mu_expected : 1.0));

        double theta_dynamic = 1.0 + cv_sys + jitter_ratio;

        double sum_hyb = 0.0;
        for (int r = 0; r < RUNS; ++r) {
            uint32_t fault = 0;
            uint64_t t0 = sacculus_rdtsc();
            bool ok_a = sf_safe_kernel_call(fn_a, in_t, 1, out_t, 1, nullptr, &fault);
            bool ok_b = ok_a && sf_safe_kernel_call(fn_b, in_t, 1, out_t, 1, nullptr, &fault);
            uint64_t dt = sacculus_rdtsc() - t0;
            if (!ok_b) {
                snprintf(res.reason, sizeof(res.reason), "hybrid_fault:0x%X", fault);
                return res;
            }
            cycles_hybrid[r] = dt;
            sum_hyb += dt;
        }
        double mu_hybrid = (sum_hyb / RUNS) * 0.5;
        double observed_ratio = mu_hybrid / (mu_expected > 0 ? mu_expected : 1.0);

        res.mean_expected = mu_expected;
        res.mean_hybrid = mu_hybrid;
        res.dynamic_bound = theta_dynamic;
        res.observed_ratio = observed_ratio;

        if (observed_ratio <= theta_dynamic) {
            res.compatible = true;
            snprintf(res.reason, sizeof(res.reason), "dynamically_compatible(ratio=%.3f<=bound=%.3f)", observed_ratio, theta_dynamic);
        } else {
            res.compatible = false;
            snprintf(res.reason, sizeof(res.reason), "stall_incompatible(ratio=%.3f>bound=%.3f)", observed_ratio, theta_dynamic);
        }
        return res;
    }

    sf_score_eval_t evaluate_score(uint64_t cand_cycles,
                                   uint64_t cur_cycles,
                                   float max_abs_err,
                                   float mse,
                                   float target_epsilon) {
        sf_score_eval_t res{};
        res.max_err = max_abs_err;
        res.mse = mse;

        bool speed_ok = (cand_cycles < cur_cycles);
        bool precision_ok = (max_abs_err <= target_epsilon) || (mse <= target_epsilon);

        if (!speed_ok || !precision_ok) {
            res.qualified = false;
            res.score = -1.0;
            return res;
        }

        double speedup = (double)(cur_cycles - cand_cycles) / (double)(cur_cycles > 0 ? cur_cycles : 1);
        double prec_margin = (double)(target_epsilon - mse) / (double)(target_epsilon > 0 ? target_epsilon : 1e-4f);
        if (prec_margin < 0.0) prec_margin = 0.0;

        res.speedup_ratio = speedup;
        res.precision_headroom = prec_margin;
        res.score = 0.6 * speedup + 0.4 * prec_margin;
        res.qualified = true;
        return res;
    }

    void redirect_stream(uint32_t fn_id, sacculus_kernel_fn_t new_fn, uint64_t new_cycles, const char* name, double score) {
        if (!dispatch || !new_fn) return;

        g_stream_pause_gate.store(true, std::memory_order_release);

        dispatch->swap(fn_id, new_fn, new_cycles);

        g_stream_pause_gate.store(false, std::memory_order_release);

        sacculus_hotlog_write(hotlog, SF_Ebin0_HOTSWAP, SF_Ebin1_INFO,
            fn_id, (uint16_t)(new_cycles & 0xFFFF), 0, 0, 0, 0,
            (uint32_t)(score * 1000.0), name ? name : "puzzle_win");

        if (coldlog) {
            char msg[64];
            snprintf(msg, sizeof(msg), "redirect:%s cyc=%llu sc=%.3f", name ? name : "simd", (unsigned long long)new_cycles, score);
            sacculus_coldlog_write(coldlog, SF_Ebin0_HOTSWAP, SF_Ebin1_INFO, fn_id, msg);
        }
    }

    void submit(uint32_t fn_id, sacculus_kernel_fn_t candidate, const char* name,
                const char* target_isa = "AVX2", float target_epsilon = 1e-4f,
                const SF_custominiVm_tensor_t* target_tensor = nullptr) {
        std::lock_guard<std::mutex> g(queue_lock);
        sf_puzzle_experiment_t exp{};
        exp.fn_id = fn_id;
        exp.candidate = candidate;
        if (name) {
            strncpy(exp.name, name, sizeof(exp.name) - 1);
        } else {
            strncpy(exp.name, "candidate_isa", sizeof(exp.name) - 1);
        }
        if (target_isa) {
            strncpy(exp.target_isa, target_isa, sizeof(exp.target_isa) - 1);
        } else {
            strncpy(exp.target_isa, "AUTO", sizeof(exp.target_isa) - 1);
        }
        exp.target_epsilon = target_epsilon > 0.0f ? target_epsilon : 1e-4f;
        if (target_tensor && target_tensor->data && target_tensor->byte_size > 0) {
            exp.target_tensor = *target_tensor;
            exp.has_target_tensor = true;
        }
        pending_experiments.push_back(exp);
    }

    void run_puzzle_suite_parallel(uint32_t fn_id, const SF_custominiVm_isa_candidate_t* candidates, int n_candidates,
                                   float target_epsilon, const SF_custominiVm_tensor_t* target_tensor) {
        if (!dispatch || !candidates || n_candidates <= 0) return;
        auto* entry = dispatch->get(fn_id);
        if (!entry || !entry->fn.load()) return;

        std::vector<sf_puzzle_sample_t> samples;
        SF_custominiVm_tensor_t target_prec{};
        bool has_target_prec = false;
        float final_epsilon = target_epsilon;
        if (target_tensor && target_tensor->data && target_tensor->byte_size > 0) {
            target_prec = *target_tensor;
            has_target_prec = true;
        }

        if (!acquire_evaluation_samples(fn_id, samples, target_prec, has_target_prec, final_epsilon)) {
            return;
        }
        if (samples.empty()) return;

        uint64_t cur_cycles_total = 0;
        auto current_fn = entry->fn.load(std::memory_order_acquire);
        for (const auto& smp : samples) {
            SF_custominiVm_tensor_t in_t{};
            in_t.data = const_cast<uint8_t*>(smp.data);
            in_t.ndim = smp.ndim;
            for (int d = 0; d < 8; ++d) in_t.shape[d] = smp.shape[d];
            in_t.dtype = smp.dtype;
            in_t.byte_size = smp.byte_size;

            SF_custominiVm_tensor_t out_t = in_t;
            out_t.data = current_best_out_payload;
            memset(current_best_out_payload, 0, smp.byte_size);

            uint32_t cur_fault = 0;
            uint64_t t0 = sacculus_rdtsc();
            bool ok = sf_safe_kernel_call(current_fn, &in_t, 1, &out_t, 1, nullptr, &cur_fault);
            uint64_t dt = sacculus_rdtsc() - t0;
            if (!ok) return;
            cur_cycles_total += (dt > 0 ? dt : 1);
        }
        uint64_t current_cycles = cur_cycles_total / samples.size();
        if (current_cycles == 0) current_cycles = 1;

        const float* ref_ptr = (has_target_prec && target_prec.data)
                             ? reinterpret_cast<const float*>(target_prec.data)
                             : reinterpret_cast<const float*>(current_best_out_payload);

        std::vector<sf_candidate_eval_t> evals(n_candidates);
        std::vector<std::thread> workers;
        workers.reserve(n_candidates);

        for (int i = 0; i < n_candidates; ++i) {
            evals[i].idx = i;
            evals[i].fn = candidates[i].fn;
            if (candidates[i].variant_name) {
                strncpy(evals[i].variant_name, candidates[i].variant_name, sizeof(evals[i].variant_name) - 1);
            } else {
                strncpy(evals[i].variant_name, "suite_variant", sizeof(evals[i].variant_name) - 1);
            }
            if (candidates[i].isa_tag) {
                strncpy(evals[i].isa_tag, candidates[i].isa_tag, sizeof(evals[i].isa_tag) - 1);
            } else {
                strncpy(evals[i].isa_tag, "AUTO", sizeof(evals[i].isa_tag) - 1);
            }

            if (!candidates[i].fn) continue;
            if (!cached_isa_inventory.has_isa(evals[i].isa_tag)) {
                continue;
            }

            workers.emplace_back([this, &evals, i, &samples, current_cycles, ref_ptr, final_epsilon]() {
                alignas(64) uint8_t local_cand_out[STAGNANT_BUF_CAP];
                uint64_t cand_cycles_total = 0;
                float max_err = 0.0f;
                float sum_sq = 0.0f;
                size_t total_elems_tested = 0;
                bool all_ok = true;
                uint32_t cand_fault = 0;

                for (const auto& smp : samples) {
                    memset(local_cand_out, 0, smp.byte_size);
                    SF_custominiVm_tensor_t in_t{};
                    in_t.data = const_cast<uint8_t*>(smp.data);
                    in_t.ndim = smp.ndim;
                    for (int d = 0; d < 8; ++d) in_t.shape[d] = smp.shape[d];
                    in_t.dtype = smp.dtype;
                    in_t.byte_size = smp.byte_size;

                    SF_custominiVm_tensor_t cand_out_t = in_t;
                    cand_out_t.data = local_cand_out;

                    uint64_t t1 = sacculus_rdtsc();
                    bool ok = sf_safe_kernel_call(evals[i].fn, &in_t, 1, &cand_out_t, 1, nullptr, &cand_fault);
                    uint64_t dt = sacculus_rdtsc() - t1;
                    if (!ok) {
                        all_ok = false;
                        break;
                    }
                    cand_cycles_total += dt;

                    size_t n_elems = smp.byte_size / sizeof(float);
                    const float* cand_ptr = reinterpret_cast<const float*>(local_cand_out);
                    for (size_t k = 0; k < n_elems; ++k) {
                        float diff = fabsf(cand_ptr[k] - ref_ptr[k]);
                        if (diff > max_err) max_err = diff;
                        sum_sq += diff * diff;
                    }
                    total_elems_tested += n_elems;
                }

                evals[i].executed_ok = all_ok;
                evals[i].fault_code = cand_fault;
                if (!all_ok) return;

                uint64_t cand_cycles = cand_cycles_total / samples.size();
                evals[i].cycles = cand_cycles;
                float mse = sum_sq / (float)(total_elems_tested > 0 ? total_elems_tested : 1);
                evals[i].max_abs_err = max_err;
                evals[i].mse = mse;

                auto sc = evaluate_score(cand_cycles, current_cycles, max_err, mse, final_epsilon);
                evals[i].precision_passed = (max_err <= final_epsilon) || (mse <= final_epsilon);
                evals[i].speed_passed = (cand_cycles < current_cycles);
                evals[i].score = sc.score;
            });
        }

        for (auto& w : workers) {
            if (w.joinable()) w.join();
        }

        int best_idx = -1;
        double best_score = -1.0;

        for (int i = 0; i < n_candidates; ++i) {
            if (evals[i].executed_ok && evals[i].score > best_score) {
                best_score = evals[i].score;
                best_idx = i;
            }
        }

        if (n_candidates >= 2) {
            int cand_a = 0, cand_b = 1;
            if (candidates[cand_a].fn && candidates[cand_b].fn) {
                SF_custominiVm_tensor_t test_in{};
                test_in.data = const_cast<uint8_t*>(samples[0].data);
                test_in.ndim = samples[0].ndim;
                for (int d = 0; d < 8; ++d) test_in.shape[d] = samples[0].shape[d];
                test_in.dtype = samples[0].dtype;
                test_in.byte_size = samples[0].byte_size;

                SF_custominiVm_tensor_t test_out = test_in;
                test_out.data = candidate_out_payload;

                auto compat = evaluate_hybrid_compatibility(candidates[cand_a].fn, candidates[cand_b].fn, &test_in, &test_out);
                if (compat.compatible) {
                    uint64_t hyb_cycles = (uint64_t)compat.mean_hybrid;
                    auto hyb_score = evaluate_score(hyb_cycles, current_cycles, evals[cand_a].max_abs_err, evals[cand_a].mse, final_epsilon);
                    if (hyb_score.qualified && hyb_score.score > best_score) {
                        best_score = hyb_score.score;
                        best_idx = cand_a;
                    }
                }
            }
        }

        if (best_idx >= 0 && best_score > 0.0) {

            redirect_stream(fn_id, evals[best_idx].fn, evals[best_idx].cycles, evals[best_idx].variant_name, best_score);
        }
    }

    void submit_suite(uint32_t fn_id, const SF_custominiVm_isa_candidate_t* candidates, int n_candidates,
                      float target_epsilon = 1e-4f, const SF_custominiVm_tensor_t* target_tensor = nullptr) {
        if (!candidates || n_candidates <= 0) return;

        uint8_t mode = execution_mode.load(std::memory_order_relaxed);
        bool run_parallel = false;
        if (mode == static_cast<uint8_t>(SF_PUZZLE_EXEC_PARALLEL)) {
            run_parallel = true;
        } else if (mode == static_cast<uint8_t>(SF_PUZZLE_EXEC_ADAPTIVE)) {
            unsigned int hw_cores = std::thread::hardware_concurrency();
            if (hw_cores >= 4 && n_candidates > 1) {
                run_parallel = true;
            }
        }

        if (run_parallel) {
            run_puzzle_suite_parallel(fn_id, candidates, n_candidates, target_epsilon, target_tensor);
        } else {
            for (int i = 0; i < n_candidates; ++i) {
                const auto& c = candidates[i];
                if (c.fn) {
                    submit(fn_id, c.fn, c.variant_name ? c.variant_name : "suite_variant",
                           c.isa_tag ? c.isa_tag : "AUTO", target_epsilon, target_tensor);
                }
            }
        }
    }

    void run_puzzle_experiment(const sf_puzzle_experiment_t& exp) {
        if (!dispatch) return;
        auto* entry = dispatch->get(exp.fn_id);
        if (!entry || !entry->fn.load() || !exp.candidate) {
            return;
        }

        if (!cached_isa_inventory.has_isa(exp.target_isa)) {
            if (coldlog) {
                sacculus_coldlog_write(coldlog, SF_Ebin0_HOTSWAP_FAIL, SF_Ebin1_TRACE, exp.fn_id, "isa_not_on_cpu");
            }
            return;
        }

        std::vector<sf_puzzle_sample_t> samples;
        SF_custominiVm_tensor_t target_prec{};
        bool has_target_prec = false;
        float target_eps = exp.target_epsilon;
        if (exp.has_target_tensor && exp.target_tensor.data && exp.target_tensor.byte_size > 0) {
            target_prec = exp.target_tensor;
            has_target_prec = true;
        }

        if (!acquire_evaluation_samples(exp.fn_id, samples, target_prec, has_target_prec, target_eps)) {
            return;
        }
        if (samples.empty()) return;

        uint64_t cur_cycles_total = 0;
        auto current_fn = entry->fn.load(std::memory_order_acquire);
        for (const auto& smp : samples) {
            SF_custominiVm_tensor_t in_t{};
            in_t.data = const_cast<uint8_t*>(smp.data);
            in_t.ndim = smp.ndim;
            for (int d = 0; d < 8; ++d) in_t.shape[d] = smp.shape[d];
            in_t.dtype = smp.dtype;
            in_t.byte_size = smp.byte_size;

            SF_custominiVm_tensor_t current_out_t = in_t;
            current_out_t.data = current_best_out_payload;
            memset(current_best_out_payload, 0, smp.byte_size);

            uint32_t cur_fault = 0;
            uint64_t t0 = sacculus_rdtsc();
            bool cur_ok = sf_safe_kernel_call(current_fn, &in_t, 1, &current_out_t, 1, nullptr, &cur_fault);
            uint64_t dt = sacculus_rdtsc() - t0;
            if (!cur_ok) return;
            cur_cycles_total += (dt > 0 ? dt : 1);
        }
        uint64_t current_cycles = cur_cycles_total / samples.size();
        if (current_cycles == 0) current_cycles = 1;

        uint64_t cand_cycles_total = 0;
        float max_abs_err = 0.0f;
        float sum_sq_err = 0.0f;
        size_t total_elems = 0;
        const float* ref_ptr = (has_target_prec && target_prec.data)
                             ? reinterpret_cast<const float*>(target_prec.data)
                             : reinterpret_cast<const float*>(current_best_out_payload);

        for (const auto& smp : samples) {
            SF_custominiVm_tensor_t in_t{};
            in_t.data = const_cast<uint8_t*>(smp.data);
            in_t.ndim = smp.ndim;
            for (int d = 0; d < 8; ++d) in_t.shape[d] = smp.shape[d];
            in_t.dtype = smp.dtype;
            in_t.byte_size = smp.byte_size;

            SF_custominiVm_tensor_t candidate_out_t = in_t;
            candidate_out_t.data = candidate_out_payload;
            memset(candidate_out_payload, 0, smp.byte_size);

            uint32_t cand_fault = 0;
            uint64_t t1 = sacculus_rdtsc();
            bool cand_ok = sf_safe_kernel_call(exp.candidate, &in_t, 1, &candidate_out_t, 1, nullptr, &cand_fault);
            uint64_t dt = sacculus_rdtsc() - t1;

            if (!cand_ok) {
                sacculus_hotlog_write(hotlog, SF_Ebin0_HOTSWAP_FAIL, SF_Ebin1_WARN,
                    exp.fn_id, 0, 0, 0, 0, 0, cand_fault, "fault_isolated");
                if (coldlog) {
                    char msg[32];
                    snprintf(msg, sizeof(msg), "fault:0x%08X", cand_fault);
                    sacculus_coldlog_write(coldlog, SF_Ebin0_HOTSWAP_FAIL, SF_Ebin1_WARN, exp.fn_id, msg);
                }
                return;
            }
            cand_cycles_total += dt;

            size_t n_elems = smp.byte_size / sizeof(float);
            const float* cand_ptr = reinterpret_cast<const float*>(candidate_out_payload);
            for (size_t i = 0; i < n_elems; ++i) {
                float diff = fabsf(cand_ptr[i] - ref_ptr[i]);
                if (diff > max_abs_err) max_abs_err = diff;
                sum_sq_err += diff * diff;
            }
            total_elems += n_elems;
        }

        uint64_t candidate_cycles = cand_cycles_total / samples.size();
        float mse = sum_sq_err / (float)(total_elems > 0 ? total_elems : 1);

        auto score_eval = evaluate_score(candidate_cycles, current_cycles, max_abs_err, mse, target_eps);

        if (score_eval.qualified && score_eval.score > 0.0) {

            redirect_stream(exp.fn_id, exp.candidate, candidate_cycles, exp.name, score_eval.score);
        } else {

            const char* reason = (candidate_cycles >= current_cycles) ? "fail_speed" : "fail_precision";
            sacculus_hotlog_write(hotlog, SF_Ebin0_HOTSWAP_FAIL, SF_Ebin1_TRACE,
                exp.fn_id, 0, 0, 0, 0, 0, (uint32_t)(max_abs_err * 10000.0f), reason);
        }
    }

    void run_puzzle_batch_job(const sf_puzzle_batch_job_t& job) {
        if (!dispatch || job.samples.empty()) return;
        auto* entry = dispatch->get(job.fn_id);
        if (!entry || !entry->fn.load()) return;

        auto capabilities = pool_simd_capabilities();
        if (capabilities.empty()) return;

        auto cur_fn = entry->fn.load(std::memory_order_acquire);
        if (!cur_fn) return;

        sf_puzzle_experiment_t exp{};
        exp.fn_id = job.fn_id;
        exp.candidate = cur_fn;
        strncpy(exp.name, "batch_job_stream", sizeof(exp.name) - 1);
        strncpy(exp.target_isa, capabilities[0].c_str(), sizeof(exp.target_isa) - 1);
        exp.target_epsilon = job.target_epsilon;
        exp.target_tensor = job.target_precision_tensor;
        exp.has_target_tensor = job.has_target_precision;

        run_puzzle_experiment(exp);
    }

    void start(void) {
        running.store(true, std::memory_order_release);
        thread = std::thread([this]() {
            while (running.load(std::memory_order_acquire)) {
                sf_puzzle_experiment_t exp{};
                sf_puzzle_batch_job_t batch_job{};
                bool has_batch = false;
                bool has_exp = false;

                {
                    std::lock_guard<std::mutex> g(queue_lock);
                    if (!pending_batch_jobs.empty()) {
                        batch_job = pending_batch_jobs.front();
                        pending_batch_jobs.erase(pending_batch_jobs.begin());
                        has_batch = true;
                    } else if (!pending_experiments.empty()) {
                        exp = pending_experiments.front();
                        pending_experiments.erase(pending_experiments.begin());
                        has_exp = true;
                    }
                }

                if (has_batch) {
                    run_puzzle_batch_job(batch_job);
                } else if (has_exp) {
                    run_puzzle_experiment(exp);
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        });
    }

    void stop(void) {
        running.store(false, std::memory_order_release);
        if (thread.joinable()) thread.join();
        std::lock_guard<std::mutex> g(queue_lock);
        pending_experiments.clear();
        pending_batch_jobs.clear();
    }

    void destroy(void) {
        stop();
        if (puzzle_heap_base) {
            sf_os_release_memory(puzzle_heap_base, puzzle_heap_capacity);
            puzzle_heap_base = nullptr;
            puzzle_heap_capacity = 0;
            puzzle_heap_used.store(0, std::memory_order_relaxed);
            stagnant_in_payload = nullptr;
            candidate_out_payload = nullptr;
            current_best_out_payload = nullptr;
        }
    }
};

using sfPuzzleArena = sacculus_puzzle_arena_t;
using sfSandbox = sacculus_puzzle_arena_t;
using sacculus_sandbox_t = sacculus_puzzle_arena_t;

struct sacculus_rawcode_cell_t {
    uint8_t* base                = nullptr;
    size_t   capacity            = 0;
    size_t   decompressed_size   = 0;
    std::atomic<bool> is_sealed_immutable{false};
    sf_ticket_lock_t lock;

    bool init(size_t cap = 32UL * 1024 * 1024) {
        capacity = (cap + 4095ULL) & ~4095ULL;
#ifdef _WIN32
        base = (uint8_t*)VirtualAlloc(nullptr, capacity, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
        base = (uint8_t*)mmap(nullptr, capacity, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (base == MAP_FAILED) base = nullptr;
#endif
        return base != nullptr;
    }

    bool load_decompressed(const uint8_t* bitcode_data, size_t sz) {
        sf_ticket_guard_t g(lock);
        if (!base || is_sealed_immutable) return false;
        if (sz > capacity) return false;
        memcpy(base, bitcode_data, sz);
        decompressed_size = sz;
        return true;
    }

    bool seal_immutable(void) {
        sf_ticket_guard_t g(lock);
        if (!base || is_sealed_immutable || decompressed_size == 0) return false;
        size_t protect_size = (decompressed_size + 4095ULL) & ~4095ULL;
        if (protect_size > capacity) protect_size = capacity;
#ifdef _WIN32
        DWORD old_protect = 0;
        BOOL ok = VirtualProtect(base, protect_size, PAGE_READONLY, &old_protect);
        if (!ok) return false;
#else
        if (mprotect(base, protect_size, PROT_READ) != 0) return false;
#endif
        is_sealed_immutable.store(true, std::memory_order_release);
        return true;
    }

    void destroy(void) {
        if (!base) return;
#ifdef _WIN32
        VirtualFree(base, 0, MEM_RELEASE);
#else
        munmap(base, capacity);
#endif
        base = nullptr;
        capacity = 0;
        decompressed_size = 0;
        is_sealed_immutable = false;
    }
};

struct sacculus_processed_chunk_t {
    uint8_t* exec_base           = nullptr;
    uint8_t* staging_base        = nullptr;
    size_t   capacity            = 0;
    std::atomic<size_t> used{0};
    std::atomic<bool> is_sealed_immutable{false};
    sf_ticket_lock_t lock;

    bool init(size_t cap = 64UL * 1024 * 1024) {
        capacity = (cap + 4095ULL) & ~4095ULL;
#ifdef _WIN32

        exec_base    = (uint8_t*)VirtualAlloc(nullptr, capacity, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        staging_base = (uint8_t*)VirtualAlloc(nullptr, capacity, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else

        exec_base    = (uint8_t*)mmap(nullptr, capacity, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (exec_base == MAP_FAILED) exec_base = nullptr;
        staging_base = (uint8_t*)mmap(nullptr, capacity, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (staging_base == MAP_FAILED) staging_base = nullptr;
#endif
        return (exec_base != nullptr && staging_base != nullptr);
    }

    uint8_t* stage_alloc(size_t bytes, size_t align = 64) {
        size_t cur = used.load(std::memory_order_relaxed);
        size_t aligned = (cur + align - 1) & ~(align - 1);
        if (aligned + bytes > capacity) return nullptr;
        if (used.compare_exchange_strong(cur, aligned + bytes, std::memory_order_acq_rel, std::memory_order_relaxed)) {
            return staging_base + aligned;
        }
        return nullptr;
    }

    bool commit_and_seal(size_t committed_bytes) {
        sf_ticket_guard_t g(lock);
        if (!exec_base || !staging_base) return false;
        size_t cur_used = used.load(std::memory_order_acquire);
        if (committed_bytes > cur_used) committed_bytes = cur_used;
        if (committed_bytes == 0) return true;

        memcpy(exec_base, staging_base, committed_bytes);

        size_t protect_size = (committed_bytes + 4095ULL) & ~4095ULL;
        if (protect_size > capacity) protect_size = capacity;
#ifdef _WIN32
        DWORD old_prot = 0;
        VirtualProtect(exec_base, protect_size, PAGE_EXECUTE_READ, &old_prot);
        FlushInstructionCache(GetCurrentProcess(), exec_base, committed_bytes);
#else
        mprotect(exec_base, protect_size, PROT_READ | PROT_EXEC);
        __builtin___clear_cache((char*)exec_base, (char*)exec_base + committed_bytes);
#endif
        is_sealed_immutable.store(true, std::memory_order_release);
        return true;
    }

    void destroy(void) {
        if (exec_base) {
            sf_os_release_memory(exec_base, capacity);
            exec_base = nullptr;
        }
        if (staging_base) {
            sf_os_release_memory(staging_base, capacity);
            staging_base = nullptr;
        }
        capacity = 0;
        used.store(0, std::memory_order_release);
        is_sealed_immutable = false;
    }
};

struct sacculus_code_heap_t {
    sacculus_rawcode_cell_t    rawcode_cell;
    sacculus_processed_chunk_t processed_chunk;

    bool init(size_t rawcode_mb = 32, size_t processed_mb = 64) {
        if (!rawcode_cell.init(rawcode_mb * 1024 * 1024)) return false;
        if (!processed_chunk.init(processed_mb * 1024 * 1024)) {
            rawcode_cell.destroy();
            return false;
        }
        return true;
    }

    void destroy(void) {
        rawcode_cell.destroy();
        processed_chunk.destroy();
    }
};

struct sacculus_code_arena_t {
    sacculus_processed_chunk_t* chunk_ref = nullptr;
    uint8_t* base  = nullptr;
    size_t   size  = 0;

    bool init(size_t sz = 64UL * 1024 * 1024, bool always_pretouch = false) {
        (void)always_pretouch;
        size = sz;
        return true;
    }

    uint8_t* alloc(size_t bytes, size_t align = 64) {
        if (chunk_ref) return chunk_ref->stage_alloc(bytes, align);
        return nullptr;
    }

    void destroy(void) {
        base = nullptr;
        size = 0;
    }
};
using sfCodeArena = sacculus_code_arena_t;

#define SF_MATHVENTORY_MAGIC 0x43424653

#pragma pack(push, 1)
struct sf_mathventory_container_header_t {
    uint32_t magic;
    uint32_t version;
    uint64_t uncompressed_size;
    uint64_t compressed_size;
    uint32_t compression_codec;
    uint32_t crc32;
    char     build_tag[32];
};
#pragma pack(pop)

static const uint8_t g_embedded_mathventory_bc_zstd[] = {
    'S', 'F', 'B', 'C',
    0x00, 0x00, 0x01, 0x00,
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    'M','a','t','h','v','e','n','t','a','g','e','S','I','M','D','-','B','i','t','c','o','d','e','\0',0,0,0,0,0,0,0,0
};

static inline bool sf_decompress_mathventory(
    const uint8_t* src, size_t src_len,
    uint8_t* dst, size_t dst_cap,
    size_t* out_decomp_len)
{
    if (!src || !dst || src_len < sizeof(sf_mathventory_container_header_t)) return false;
    const auto* hdr = reinterpret_cast<const sf_mathventory_container_header_t*>(src);
    if (hdr->magic != SF_MATHVENTORY_MAGIC) {
        if (src_len <= dst_cap) {
            memcpy(dst, src, src_len);
            if (out_decomp_len) *out_decomp_len = src_len;
            return true;
        }
        return false;
    }

    size_t uncompressed = (size_t)hdr->uncompressed_size;
    if (uncompressed > dst_cap) return false;

    const uint8_t* payload = src + sizeof(sf_mathventory_container_header_t);
    size_t payload_len = src_len - sizeof(sf_mathventory_container_header_t);
    size_t copy_len = (payload_len < uncompressed) ? payload_len : uncompressed;
    memcpy(dst, payload, copy_len);
    if (out_decomp_len) *out_decomp_len = copy_len;
    return true;
}

enum sf_jit_tier_level_e : uint8_t {
    SF_JIT_TIER_NONE   = 0,
    SF_JIT_TIER_VAC    = 1,
    SF_JIT_TIER_MIRC   = 2,
    SF_JIT_TIER_PUZZLE = 3
};

struct sacculus_tiered_jit_state_t {
    static constexpr size_t MAX_FNS = 2048;
    static constexpr uint64_t VAC_TO_MIRC_THRESHOLD = 50;

    std::atomic<uint8_t>  fn_tier[MAX_FNS];
    std::atomic<uint64_t> fn_invocations[MAX_FNS];
    std::atomic<uint32_t> deopt_count{0};
    std::atomic<uint32_t> mirc_compiled_count{0};

    sacculus_tiered_jit_state_t() {
        for (size_t i = 0; i < MAX_FNS; ++i) {
            fn_tier[i].store(SF_JIT_TIER_NONE, std::memory_order_relaxed);
            fn_invocations[i].store(0, std::memory_order_relaxed);
        }
    }

    void record_call(uint32_t fn_id) {
        if (fn_id < MAX_FNS) {
            fn_invocations[fn_id].fetch_add(1, std::memory_order_relaxed);
        }
    }

    bool should_promote_to_mirc(uint32_t fn_id) const {
        if (fn_id >= MAX_FNS) return false;
        return (fn_tier[fn_id].load(std::memory_order_relaxed) == SF_JIT_TIER_VAC &&
                fn_invocations[fn_id].load(std::memory_order_relaxed) >= VAC_TO_MIRC_THRESHOLD);
    }

    void set_tier(uint32_t fn_id, sf_jit_tier_level_e tier) {
        if (fn_id < MAX_FNS) {
            fn_tier[fn_id].store(tier, std::memory_order_release);
            if (tier == SF_JIT_TIER_MIRC) {
                mirc_compiled_count.fetch_add(1, std::memory_order_relaxed);
            }
        }
    }

    void hardware_deopt(uint32_t fn_id) {
        if (fn_id < MAX_FNS) {
            fn_tier[fn_id].store(SF_JIT_TIER_VAC, std::memory_order_release);
            deopt_count.fetch_add(1, std::memory_order_relaxed);
        }
    }
};

extern "C" {
MSIMD_API void sf_gc_safepoint_poll(void);
MSIMD_API int  sf_gc_controlpoint_check(size_t allocated_bytes);
MSIMD_API void sf_vm_hookpoint_trigger(int event_id, uint64_t arg1, uint64_t arg2);
MSIMD_API int  sf_vm_interruptpoint_poll(void);
}

#ifdef SACFOR_HAS_LLVM
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Support/MemoryBuffer.h>
using namespace llvm;
using namespace llvm::orc;

class SfvmSafepointPass : public llvm::PassInfoMixin<SfvmSafepointPass> {
public:
    static void inject_loop_safepoints(llvm::Loop* L, llvm::FunctionCallee poll_fn, llvm::FunctionCallee cp_fn, llvm::FunctionCallee irq_fn) {
        if (!L) return;
        llvm::BasicBlock* header = L->getHeader();
        if (header) {
            llvm::IRBuilder<> builder(&*header->getFirstInsertionPt());
            builder.CreateCall(poll_fn);
            builder.CreateCall(cp_fn);
        }

        for (llvm::BasicBlock* bb : L->blocks()) {
            if (bb != header && !bb->empty()) {
                llvm::Instruction* term = bb->getTerminator();
                if (term) {
                    llvm::IRBuilder<> b_term(term);
                    b_term.CreateCall(irq_fn);
                }
                break;
            }
        }
        for (llvm::Loop* sub_loop : *L) {
            inject_loop_safepoints(sub_loop, poll_fn, cp_fn, irq_fn);
        }
    }

    static void inject_into_module(llvm::Module& M) {
        llvm::LLVMContext& ctx = M.getContext();

        llvm::FunctionType* poll_fn_type = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), false);
        llvm::FunctionCallee poll_callee = M.getOrInsertFunction("sf_gc_safepoint_poll", poll_fn_type);

        llvm::FunctionType* cp_fn_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), false);
        llvm::FunctionCallee cp_callee = M.getOrInsertFunction("sf_gc_controlpoint_check", cp_fn_type);

        std::vector<llvm::Type*> hook_args = {
            llvm::Type::getInt32Ty(ctx),
            llvm::Type::getInt64Ty(ctx),
            llvm::Type::getInt64Ty(ctx)
        };
        llvm::FunctionType* hook_fn_type = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), hook_args, false);
        llvm::FunctionCallee hook_callee = M.getOrInsertFunction("sf_vm_hookpoint_trigger", hook_fn_type);

        llvm::FunctionType* irq_fn_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), false);
        llvm::FunctionCallee irq_callee = M.getOrInsertFunction("sf_vm_interruptpoint_poll", irq_fn_type);

        for (llvm::Function& F : M) {
            if (F.isDeclaration() || F.isIntrinsic()) continue;

            if (!F.empty()) {
                llvm::BasicBlock& entry = F.getEntryBlock();
                llvm::IRBuilder<> builder(&*entry.getFirstInsertionPt());
                builder.CreateCall(poll_callee);
                llvm::Value* ev_enter = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1);
                llvm::Value* zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
                builder.CreateCall(hook_callee, {ev_enter, zero, zero});
            }

            for (llvm::BasicBlock& bb : F) {
                llvm::Instruction* term = bb.getTerminator();
                if (term && llvm::isa<llvm::ReturnInst>(term)) {
                    llvm::IRBuilder<> b_ret(term);
                    llvm::Value* ev_exit = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 2);
                    llvm::Value* zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
                    b_ret.CreateCall(hook_callee, {ev_exit, zero, zero});
                }
            }

            llvm::DominatorTree DT(F);
            llvm::LoopInfo LI(DT);
            for (llvm::Loop* L : LI) {
                inject_loop_safepoints(L, poll_callee, cp_callee, irq_callee);
            }
        }
    }
};
#endif

struct sacculus_forge_jit_t {
    sacculus_code_heap_t*      code_heap = nullptr;
    sacculus_dispatch_table_t* dispatch  = nullptr;
    sacculus_hotlog_t*         hotlog    = nullptr;
    bool                       ready     = false;

#ifdef SACFOR_HAS_LLVM
    std::unique_ptr<ExecutionSession>          es;
    std::unique_ptr<DataLayout>                dl;
    std::unique_ptr<MangleAndInterner>         mangle;
    JITDylib*                                  main_jd = nullptr;
    std::unique_ptr<RTDyldObjectLinkingLayer>  object_layer;
    std::unique_ptr<IRCompileLayer>            compile_layer;
    std::unique_ptr<LLVMContext>               ctx;
#endif

    void init(sacculus_code_heap_t* heap, sacculus_dispatch_table_t* dt, sacculus_hotlog_t* hl) {
        code_heap = heap; dispatch = dt; hotlog = hl;
#ifdef SACFOR_HAS_LLVM
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();

        auto jtmb_or = JITTargetMachineBuilder::detectHost();
        if (!jtmb_or) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_ERROR, SF_Ebin1_ERROR,
                0, 0, 0, 0, 0, 0, 0, "orc_host_detect_fail");
            return;
        }

        auto dl_or = jtmb_or->getDefaultDataLayoutForTarget();
        if (!dl_or) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_ERROR, SF_Ebin1_ERROR,
                0, 0, 0, 0, 0, 0, 0, "orc_datalayout_fail");
            return;
        }

        auto epc_or = SelfExecutorProcessControl::Create();
        if (!epc_or) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_ERROR, SF_Ebin1_ERROR,
                0, 0, 0, 0, 0, 0, 0, "orc_epc_fail");
            return;
        }

        es = std::make_unique<ExecutionSession>(std::move(*epc_or));
        dl = std::make_unique<DataLayout>(std::move(*dl_or));
        mangle = std::make_unique<MangleAndInterner>(*es, *dl);
        main_jd = &es->createBareJITDylib("main");

        object_layer = std::make_unique<RTDyldObjectLinkingLayer>(
            *es,
            []() { return std::make_unique<SectionMemoryManager>(); }
        );

        compile_layer = std::make_unique<IRCompileLayer>(
            *es,
            *object_layer,
            std::make_unique<ConcurrentIRCompiler>(std::move(*jtmb_or))
        );

        auto gen_or = DynamicLibrarySearchGenerator::GetForCurrentProcess(dl->getGlobalPrefix());
        if (gen_or) {
            main_jd->addGenerator(std::move(*gen_or));
        }

        SymbolMap syms;
        syms[(*mangle)("sf_gc_safepoint_poll")]        = JITEvaluatedSymbol::fromPointer(&sf_gc_safepoint_poll);
        syms[(*mangle)("sf_gc_controlpoint_check")]     = JITEvaluatedSymbol::fromPointer(&sf_gc_controlpoint_check);
        syms[(*mangle)("sf_vm_hookpoint_trigger")]      = JITEvaluatedSymbol::fromPointer(&sf_vm_hookpoint_trigger);
        syms[(*mangle)("sf_vm_interruptpoint_poll")]    = JITEvaluatedSymbol::fromPointer(&sf_vm_interruptpoint_poll);
        auto def_err = main_jd->define(absoluteSymbols(syms));
        if (def_err) {
            llvm::consumeError(std::move(def_err));
        }

        ctx   = std::make_unique<LLVMContext>();
        ready = true;
        sacculus_hotlog_write(hotlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_INFO,
            0, 0, 0, 0, 0, 0, 0, "pure_orc_v2_ready");
#else
        sacculus_hotlog_write(hotlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_WARN,
            0, 0, 0, 0, 0, 0, 0, "jit_stub_only");
        ready = false;
#endif
    }

    void destroy(void) {
#ifdef SACFOR_HAS_LLVM
        if (es) {
            auto err = es->endSession();
            if (err) llvm::consumeError(std::move(err));
            es.reset();
        }
        compile_layer.reset();
        object_layer.reset();
        mangle.reset();
        dl.reset();
        ctx.reset();
        ready = false;
#endif
    }

    bool load_bitcode(const uint8_t* blob, size_t sz, uint32_t fn_id_base) {
#ifdef SACFOR_HAS_LLVM
        if (!ready) return false;
        auto buf = MemoryBuffer::getMemBuffer(
            StringRef((const char*)blob, sz), "", false);
        auto mod_or = parseBitcodeFile(*buf, *ctx);
        if (!mod_or) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_ERROR, SF_Ebin1_ERROR,
                fn_id_base, 0, 0, 0, 0, 0, 0, "bc_parse_fail");
            return false;
        }

        SfvmSafepointPass::inject_into_module(**mod_or);

        auto err = compile_layer->add(
            *main_jd,
            ThreadSafeModule(std::move(*mod_or),
                             std::make_unique<LLVMContext>()));
        if (err) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_ERROR, SF_Ebin1_ERROR,
                fn_id_base, 0, 0, 0, 0, 0, 0, "orc_compile_fail");
            return false;
        }
        sacculus_hotlog_write(hotlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_INFO,
            fn_id_base, 0, 0, 0, 0, 0, 0, "orc_module_ok");
        return true;
#else
        (void)blob; (void)sz; (void)fn_id_base; return false;
#endif
    }

    bool resolve(const char* sym, uint32_t fn_id) {
#ifdef SACFOR_HAS_LLVM
        if (!ready) return false;
        auto s = es->lookup({main_jd}, (*mangle)(sym));
        if (!s) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_ERROR, SF_Ebin1_WARN,
                fn_id, 0, 0, 0, 0, 0, 0, sym);
            return false;
        }
        dispatch->register_fn(fn_id, (sacculus_kernel_fn_t)s->getValue());
        return true;
#else
        (void)sym; (void)fn_id; return false;
#endif
    }
};

using sfForgeJit = sacculus_forge_jit_t;

enum sf_vm_state_e : uint8_t {
    SF_VM_STATE_UNINITIALIZED = 0,
    SF_VM_STATE_SETUP_READY   = 1,
    SF_VM_STATE_RUNNING       = 2,
    SF_VM_STATE_SHUTDOWN      = 3
};

struct sacculus_forge_t {

    sacculus_hotlog_t*  hotlog  = nullptr;
    sacculus_coldlog_t* coldlog = nullptr;

    sacculus_gc_t               gc;
    sacculus_code_heap_t        code_heap;
    sacculus_code_arena_t       code_arena;
    sacculus_dispatch_table_t   dispatch;
    sacculus_sandbox_t          sandbox;
    sacculus_forge_jit_t        jit;
    sacculus_folder_tree_t      folder_tree;
    sacculus_tiered_jit_state_t tiered_jit;

    std::atomic<uint8_t>     vm_state{SF_VM_STATE_UNINITIALIZED};
    std::mutex               transition_lock;
    sf_vm_config_t           config;

    bool setup_start(const sf_vm_config_t& cfg) {
        uint8_t cur_st = vm_state.load(std::memory_order_relaxed);
        if (cur_st == SF_VM_STATE_SETUP_READY || cur_st == SF_VM_STATE_RUNNING) return true;

        sf_vm_config_t validated_cfg = cfg;
        if (!validate_and_reconcile_sf_flags(validated_cfg)) {
            fprintf(stderr, "[SFVM Forge Error] Aborting setup due to fatal flag conflict: %s\n", validated_cfg.conflict_msg);
            return false;
        }
        config = validated_cfg;
        g_sf_critical_simd_mode.store(static_cast<uint8_t>(validated_cfg.strings.critical_simd_mode), std::memory_order_relaxed);

        hotlog  = new (std::nothrow) sacculus_hotlog_t;
        coldlog = new (std::nothrow) sacculus_coldlog_t;
        if (!hotlog || !coldlog) return false;
        hotlog->init();
        coldlog->init();

        if (validated_cfg.has_flag_conflict) {
            sacculus_coldlog_write(coldlog, SF_Ebin0_ERROR, SF_Ebin1_WARN, 0, "flag_conflict");
        }

        folder_tree.init();

        size_t rawcode_mb   = 32;
        size_t processed_mb = cfg.integers.code_arena_bytes / (1024 * 1024);
        if (processed_mb == 0) processed_mb = 64;

        if (!code_heap.init(rawcode_mb, processed_mb)) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_ERROR, SF_Ebin1_FATAL, 0, 0, 0, 0, 0, 0, 0, "code_heap_fail");
            return false;
        }

        code_arena.chunk_ref = &code_heap.processed_chunk;
        code_arena.base      = code_heap.processed_chunk.exec_base;
        code_arena.size      = code_heap.processed_chunk.capacity;

        gc.init(hotlog, coldlog, cfg);

        sandbox.init(&dispatch, &gc.runway, hotlog, coldlog);

        jit.init(&code_heap, &dispatch, hotlog);

        sacculus_hotlog_write(hotlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_INFO, 0, 0, 0, 0, 0, 0, 0, "sfvm_setup_ready");
        sacculus_coldlog_write(coldlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_INFO, 0, "setup_ready");

        vm_state.store(SF_VM_STATE_SETUP_READY, std::memory_order_release);
        return true;
    }

    bool vm_start(void) {
        uint8_t cur_st = vm_state.load(std::memory_order_relaxed);
        if (cur_st == SF_VM_STATE_RUNNING) return true;

        if (cur_st != SF_VM_STATE_SETUP_READY) {
            sf_vm_config_t def_cfg = parse_all_sf_flags(nullptr);
            if (!setup_start(def_cfg)) return false;
        }

        size_t decomp_len = 0;
        bool dec_ok = sf_decompress_mathventory(
            g_embedded_mathventory_bc_zstd,
            sizeof(g_embedded_mathventory_bc_zstd),
            code_heap.rawcode_cell.base,
            code_heap.rawcode_cell.capacity,
            &decomp_len
        );

        if (dec_ok) {
            code_heap.rawcode_cell.decompressed_size = decomp_len;

            code_heap.rawcode_cell.seal_immutable();
            sacculus_hotlog_write(hotlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_INFO,
                0, 0, 0, 0, 0, 0, (uint32_t)(decomp_len / 1024), "rawcode_sealed_immutable");
        }

        gc.start();
        if (!config.is_safety_critical) {
            sandbox.start();
        }

        if (code_heap.rawcode_cell.decompressed_size > 0) {
            jit.load_bitcode(code_heap.rawcode_cell.base, code_heap.rawcode_cell.decompressed_size, 0);
        }

        size_t used_code = code_heap.processed_chunk.used.load(std::memory_order_relaxed);
        if (used_code > 0) {
            code_heap.processed_chunk.commit_and_seal(used_code);
        }

        sacculus_hotlog_write(hotlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_INFO, 0, 0, 0, 0, 0, 0, 0, "sfvm_vm_running");
        sacculus_coldlog_write(coldlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_INFO, 0, "vm_running");

        vm_state.store(SF_VM_STATE_RUNNING, std::memory_order_release);
        return true;
    }

    bool init(const sf_vm_config_t& cfg) {
        if (!setup_start(cfg)) return false;
        return vm_start();
    }

    bool init(size_t code_arena_mb = 64, size_t pressure_mb = 256, const char* custom_flags = nullptr) {
        sf_vm_config_t cfg = parse_all_sf_flags(custom_flags);
        if (code_arena_mb > 0) cfg.integers.code_arena_bytes = code_arena_mb * 1024 * 1024;
        if (pressure_mb > 0)   cfg.integers.pressure_threshold_bytes = pressure_mb * 1024 * 1024;
        return init(cfg);
    }

    void shutdown(void) {
        uint8_t cur_st = vm_state.load(std::memory_order_relaxed);
        if (cur_st == SF_VM_STATE_UNINITIALIZED || cur_st == SF_VM_STATE_SHUTDOWN) return;

        gc.stop();
        sandbox.destroy();
        folder_tree.evict_folder("/", &gc.runway);
        folder_tree.destroy();
        code_heap.destroy();
        code_arena.destroy();
        gc.runway.destroy();
        gc.landfill.destroy();

        sacculus_hotlog_write(hotlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_INFO, 0, 0, 0, 0, 0, 0, 0, "sf_forge_shutdown");
        vm_state.store(SF_VM_STATE_SHUTDOWN, std::memory_order_release);

        delete hotlog;  hotlog  = nullptr;
        delete coldlog; coldlog = nullptr;
    }
};

using sfForge = sacculus_forge_t;

static sfForge g_forge;

extern "C" {
MSIMD_API void sf_gc_safepoint_poll(void) {
    if (g_forge.config.is_safety_critical) {
        return;
    }
    if (g_forge.gc.current_phase.load(std::memory_order_relaxed) >= SF_GC_PHASE_DRAIN) {
        sf_cpu_pause();
        std::this_thread::yield();
    }
}

MSIMD_API int sf_gc_controlpoint_check(size_t allocated_bytes) {
    (void)allocated_bytes;
    if (g_forge.config.is_safety_critical) {
        return 0;
    }
    size_t used = g_forge.gc.total_heap_bytes.load(std::memory_order_relaxed);
    size_t thresh = g_forge.gc.pressure_threshold;
    if (thresh > 0 && used > (thresh * 85 / 100)) {
        g_forge.gc.runway.pace_allocation();
        return 1;
    }
    return 0;
}

MSIMD_API void sf_vm_hookpoint_trigger(int event_id, uint64_t arg1, uint64_t arg2) {

    SF_custominiVm_gc_hook_event_e ev = (event_id == 1) ? SF_GC_HOOK_PRE_SWEEP :
                                        (event_id == 2) ? SF_GC_HOOK_POST_SWEEP :
                                        SF_GC_HOOK_PRE_SWEEP;
    g_forge.gc.trigger_hook(ev);
    if (g_forge.hotlog) {
        sacculus_hotlog_write(g_forge.hotlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_TRACE,
            (uint32_t)arg1, (uint16_t)event_id, 0, 0, 0, 0, (uint32_t)arg2, "sfvm_hookpoint");
    }
}

MSIMD_API int sf_vm_interruptpoint_poll(void) {

    if (g_forge.gc.current_phase.load(std::memory_order_relaxed) >= SF_GC_PHASE_DRAIN) {
        sf_cpu_pause();
        std::this_thread::yield();
        return 1;
    }
    if (g_sf_critical_simd_mode.load(std::memory_order_relaxed) == 0) {

        sf_cpu_pause();
    }
    return 0;
}
}

extern "C" {

MSIMD_API int SF_custominiVm_setup_start(const void* custom_cfg) {
    if (custom_cfg) {
        const auto* cfg = reinterpret_cast<const sf_vm_config_t*>(custom_cfg);
        return g_forge.setup_start(*cfg) ? 0 : -1;
    }
    sf_vm_config_t def_cfg = parse_all_sf_flags(nullptr);
    return g_forge.setup_start(def_cfg) ? 0 : -1;
}

MSIMD_API int SF_custominiVm_start(void) {
    return g_forge.vm_start() ? 0 : -1;
}

MSIMD_API int SF_custominiVm_is_setup(void) {
    uint8_t st = g_forge.vm_state.load(std::memory_order_relaxed);
    return (st == SF_VM_STATE_SETUP_READY || st == SF_VM_STATE_RUNNING) ? 1 : 0;
}

MSIMD_API int SF_custominiVm_is_started(void) {
    return (g_forge.vm_state.load(std::memory_order_relaxed) == SF_VM_STATE_RUNNING) ? 1 : 0;
}

MSIMD_API void SF_custominiVm_code_heap_query(SF_custominiVm_code_heap_stat_t* out_stat) {
    if (!out_stat) return;
    memset(out_stat, 0, sizeof(*out_stat));
    out_stat->rawcode_capacity           = g_forge.code_heap.rawcode_cell.capacity;
    out_stat->rawcode_used_bytes         = g_forge.code_heap.rawcode_cell.decompressed_size;
    out_stat->rawcode_is_immutable       = g_forge.code_heap.rawcode_cell.is_sealed_immutable ? 1 : 0;
    out_stat->processed_chunk_capacity   = g_forge.code_heap.processed_chunk.capacity;
    out_stat->processed_chunk_used_bytes = g_forge.code_heap.processed_chunk.used.load(std::memory_order_relaxed);
    out_stat->processed_chunk_is_sealed  = g_forge.code_heap.processed_chunk.is_sealed_immutable ? 1 : 0;
    out_stat->active_jit_tier            = (uint32_t)SF_JIT_TIER_MIRC;
    out_stat->compiled_kernel_count      = g_forge.tiered_jit.mirc_compiled_count.load(std::memory_order_relaxed);
    out_stat->deopt_event_count          = g_forge.tiered_jit.deopt_count.load(std::memory_order_relaxed);
}

MSIMD_API int SF_custominiVm_init(size_t code_arena_mb, size_t pressure_mb) {
    return g_forge.init(
        code_arena_mb ? code_arena_mb : 64u,
        pressure_mb   ? pressure_mb   : 256u
    ) ? 0 : -1;
}

MSIMD_API int SF_custominiVm_init_with_flags(const char* flags_str) {
    sf_vm_config_t cfg = parse_all_sf_flags(flags_str);
    if (!validate_and_reconcile_sf_flags(cfg)) {
        return (int)MSIMD_ERROR_FLAG_CONFLICT;
    }
    return g_forge.init(cfg) ? 0 : -1;
}

MSIMD_API msimd_profile_mode_t SF_custominiVm_get_profile(void) {
    return g_forge.config.profile;
}

MSIMD_API int SF_custominiVm_is_safety_critical(void) {
    return g_forge.config.is_safety_critical ? 1 : 0;
}

MSIMD_API void SF_custominiVm_shutdown(void) {
    g_forge.shutdown();
}

MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_tensor_from(void* data, const int32_t* shape,
                                                          int ndim, int dtype) {
    return sf_tensor_make(data, shape, ndim, (sfDType)dtype);
}

MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_tensor_alloc(const int32_t* shape, int ndim, int dtype) {
    SF_custominiVm_tensor_t t{};
    if (ndim <= 0 || !shape) return t;
    t.ndim = (ndim > 8) ? 8 : ndim;
    t.dtype = (uint8_t)dtype;
    size_t elems = 1;
    for (int i = 0; i < t.ndim; ++i) {
        t.shape[i] = shape[i];
        if (shape[i] <= 0) return t;
        size_t dim = (size_t)shape[i];
        if (elems > SIZE_MAX / dim) return t;
        elems *= dim;
    }
    size_t dsz = sf_dtype_size((sfDType)dtype);
    if (dsz > 0 && elems > SIZE_MAX / dsz) return t;
    t.byte_size = elems * dsz;
    t.data = g_forge.gc.runway.allocate_cell(t.byte_size);
    if (t.data && !g_forge.gc.config.strings.no_sfgc_overwrite_mode) {
        sf_critical_clear_memory(t.data, t.byte_size);
    }
    return t;
}

MSIMD_API void SF_custominiVm_tensor_free(SF_custominiVm_tensor_t* tensor) {
    if (!tensor || !tensor->data) return;
    g_forge.gc.runway.free_cell(tensor->data, tensor->byte_size);
    tensor->data = nullptr;
    tensor->byte_size = 0;
}

MSIMD_API void* SF_custominiVm_heap_alloc(size_t bytes, size_t alignment) {

    if (alignment < 1) alignment = 64;

    if (alignment & (alignment - 1)) {
        alignment = 1;
        while (alignment < 64) alignment <<= 1;
    }
    if (g_forge.gc.config.strings.no_sfgc_overwrite_mode) {
        return SF_primitiva_alloc(bytes, alignment);
    }
    return g_forge.gc.runway.allocate_cell(bytes);
}

MSIMD_API void SF_custominiVm_heap_free(void* ptr, size_t bytes) {
    g_forge.gc.runway.free_cell(ptr, bytes);
}

MSIMD_API size_t SF_custominiVm_heap_allocated_bytes(void) {
    return g_forge.gc.runway.allocator.get_used_bytes();
}

MSIMD_API void SF_custominiVm_register(uint32_t fn_id, SF_custominiVm_kernel_fn_t fn) {
    g_forge.dispatch.register_fn(fn_id, fn);
}

MSIMD_API int SF_custominiVm_bind(const char* name, uint32_t fn_id,
                                const SF_custominiVm_tensor_t* ex, int n) {
    (void)name; (void)ex; (void)n;
    return (g_forge.dispatch.get(fn_id)) ? 0 : -1;
}

MSIMD_API int SF_custominiVm_call(uint32_t fn_id,
                                const SF_custominiVm_tensor_t* inputs,  int n_in,
                                SF_custominiVm_tensor_t*       outputs, int n_out,
                                void* user_data) {
    auto* e = g_forge.dispatch.get(fn_id);
    if (!e) return (int)MSIMD_ERROR_INVALID_ARGUMENT;

    while (g_stream_pause_gate.load(std::memory_order_acquire)) {
#if defined(_M_X64) || defined(__x86_64__)
        _mm_pause();
#endif
    }

    sfKernelFn fn = nullptr;
    uint32_t v1 = 0, v2 = 0;
    do {
        v1 = e->seqlock_version.load(std::memory_order_acquire);
        fn = e->fn.load(std::memory_order_relaxed);
        v2 = e->seqlock_version.load(std::memory_order_acquire);
    } while ((v1 & 1) != 0 || v1 != v2);

    if (!fn) return (int)MSIMD_ERROR_NOT_IMPLEMENTED;

    if (inputs && n_in > 0) {
        for (int i = 0; i < n_in; ++i) {
            auto* obj = SF_custominiVm_live_header_t::from_body(inputs[i].data);
            if (obj && obj->state.load(std::memory_order_acquire) == static_cast<uint8_t>(sf_live_state_e::QUARANTINED)) {
                return (int)MSIMD_ERROR_HARDWARE_FAULT;
            }
        }
    }

    if (g_forge.config.is_safety_critical) {
        if (inputs && n_in > 0) {
            for (int i = 0; i < n_in; ++i) {
                if (!inputs[i].data) return (int)MSIMD_ERROR_INVALID_ARGUMENT;
                size_t elems = 1;
                for (int d = 0; d < inputs[i].ndim; ++d) {
                    elems *= (size_t)(inputs[i].shape[d] > 0 ? inputs[i].shape[d] : 1);
                }
                if (g_forge.config.max_loop_iterations > 0 && elems > g_forge.config.max_loop_iterations) {

                    return (int)MSIMD_ERROR_INVALID_ARGUMENT;
                }
            }
        }
        if (outputs && n_out > 0) {
            for (int i = 0; i < n_out; ++i) {
                if (!outputs[i].data) return (int)MSIMD_ERROR_INVALID_ARGUMENT;
            }
        }
    }

    SF_custominiVm_tensor_t resolved_inputs[16];
    SF_custominiVm_tensor_t resolved_outputs[16];
    const SF_custominiVm_tensor_t* call_inputs = inputs;
    SF_custominiVm_tensor_t* call_outputs = outputs;

    if (inputs && n_in > 0 && n_in <= 16) {
        for (int i = 0; i < n_in; ++i) {
            resolved_inputs[i] = inputs[i];
            void* orig_ptr = inputs[i].data;
            resolved_inputs[i].data = SF_custominiVm_tensor_resolve_ptr(&inputs[i]);
            if (resolved_inputs[i].data != orig_ptr) {
                resolved_inputs[i].status |= SF_TENSOR_STATUS_FORWARDED;
            }
            auto* obj = SF_custominiVm_live_header_t::from_body(resolved_inputs[i].data);
            if (obj) {
                if (obj->parent_header.load(std::memory_order_relaxed)) {
                    resolved_inputs[i].status |= SF_TENSOR_STATUS_VIEW_SLICE;
                }
            }
        }
        call_inputs = resolved_inputs;
    }
    if (outputs && n_out > 0 && n_out <= 16) {
        for (int i = 0; i < n_out; ++i) {
            resolved_outputs[i] = outputs[i];
            void* orig_ptr = outputs[i].data;
            resolved_outputs[i].data = SF_custominiVm_tensor_resolve_ptr(&outputs[i]);
            if (resolved_outputs[i].data != orig_ptr) {
                resolved_outputs[i].status |= SF_TENSOR_STATUS_FORWARDED;
                outputs[i].data = resolved_outputs[i].data;
                outputs[i].status |= SF_TENSOR_STATUS_FORWARDED;
            }
        }
        call_outputs = resolved_outputs;
    }

    uint64_t t0 = sacculus_rdtsc();
    fn(call_inputs, n_in, call_outputs, n_out, user_data);
    uint64_t delta_cycles = sacculus_rdtsc() - t0;
    g_forge.gc.total_calls_tracked.fetch_add(1, std::memory_order_relaxed);

    sf_sync_store_fence();

    if (outputs && n_out > 0) {
        for (int i = 0; i < n_out; ++i) {
            auto* obj = SF_custominiVm_live_header_t::from_body(call_outputs[i].data);
            if (obj) {
                if (!obj->isTailCanaryValid()) {
                    obj->state.store(static_cast<uint8_t>(sf_live_state_e::QUARANTINED), std::memory_order_release);
                    outputs[i].status |= SF_TENSOR_STATUS_QUARANTINED;
                } else {
                    outputs[i].status |= SF_TENSOR_STATUS_OK;
                }
            }
        }
    }

    e->call_count++;
    if (delta_cycles < e->best_cycles) e->best_cycles = delta_cycles;

    sfLiveHeader* live = e->cached_live_header.load(std::memory_order_relaxed);
    if (live) {
        sfPinGuard guard(live);
        if (guard.is_valid()) {
#ifdef _WIN32
            uint16_t cur_core = (uint16_t)GetCurrentProcessorNumber();
#else
            uint16_t cur_core = 0;
#endif
            live->record_access(delta_cycles, cur_core);

            if (live->region == 3 && live->generation < SACFOR_ROOM_COUNT && (get_cold(live)->call_step_count & 0x3F) == 0) {
                g_forge.gc.motherline.rooms[live->generation].regions[3].hot_order.bubble_up(live);
            }
        } else {

            g_forge.gc.request_resurrection(fn_id);
        }
    } else {

        g_forge.gc.request_resurrection(fn_id);
    }

    return (int)MSIMD_SUCCESS;
}

MSIMD_API void SF_custominiVm_hotlog_drain(
        void (*cb)(const SF_custominiVm_hotlog_entry_t*, void*), void* user) {
    if (g_forge.hotlog) g_forge.hotlog->drain(cb, user);
}

MSIMD_API void SF_custominiVm_coldlog_drain(
        void (*cb)(const SF_custominiVm_coldlog_entry_t*, void*), void* user) {
    if (g_forge.coldlog) g_forge.coldlog->drain(cb, user);
}

MSIMD_API void SF_custominiVm_gc_admit(SF_custominiVm_live_header_t* obj) {
    g_forge.gc.admit(obj);
}

MSIMD_API void SF_custominiVm_gc_touch(SF_custominiVm_live_header_t* obj,
                                    uint64_t cycles, uint16_t core_id) {
    if (obj) obj->record_access(cycles, core_id);
}

MSIMD_API void SF_custominiVm_gc_set_on_free(
        void (*cb)(SF_custominiVm_live_header_t*, void*), void* user) {
    g_forge.gc.on_free      = cb;
    g_forge.gc.on_free_user = user;
}

MSIMD_API size_t SF_custominiVm_gc_run(SF_custominiVm_gc_run_mode_t mode,
                                    int owp,
                                    SF_custominiVm_mtp_predicate_fn mtp_pred,
                                    void* user_data) {
    return g_forge.gc.run(mode, owp != 0, mtp_pred, user_data);
}

MSIMD_API void SF_custominiVm_gc_collect(void) {
    g_forge.gc.run(SF_GC_RUN_SINGLE_STEP, false, nullptr, nullptr);
}

MSIMD_API SF_custominiVm_gc_manifest_t SF_custominiVm_gc_shake_bed(void) {
    return g_forge.gc.shake_bed();
}

MSIMD_API void SF_custominiVm_gc_get_last_manifest(SF_custominiVm_gc_manifest_t* out_manifest) {
    if (!out_manifest) return;
    std::lock_guard<std::mutex> lk(g_forge.gc.manifest_lock);
    *out_manifest = g_forge.gc.last_manifest;
}

MSIMD_API void SF_custominiVm_gc_set_heuristic(SF_custominiVm_gc_heuristic_t heuristic) {
    if (g_forge.config.is_safety_critical) {
        if (heuristic == SF_GC_HEURISTIC_DISABLED) {

            g_forge.gc.config.strings.gc_heuristic = sf_gc_heuristic_e::DISABLED;
            g_forge.gc.config.strings.no_sfgc_overwrite_mode = true;
            return;
        }

        g_forge.gc.config.strings.gc_heuristic = sf_gc_heuristic_e::BUREAUCRATIC;
        g_forge.gc.config.strings.no_sfgc_overwrite_mode = false;
        return;
    }
    g_forge.gc.config.strings.gc_heuristic = static_cast<sf_gc_heuristic_e>(heuristic);
    g_forge.gc.config.strings.no_sfgc_overwrite_mode = (heuristic == SF_GC_HEURISTIC_DISABLED);
}

MSIMD_API SF_custominiVm_gc_heuristic_t SF_custominiVm_gc_get_heuristic(void) {
    return static_cast<SF_custominiVm_gc_heuristic_t>(g_forge.gc.config.strings.gc_heuristic);
}

MSIMD_API void SF_custominiVm_gc_set_no_sfgc(int enabled) {
    g_forge.gc.config.strings.no_sfgc_overwrite_mode = (enabled != 0);
    if (enabled) {
        g_forge.gc.config.strings.gc_heuristic = sf_gc_heuristic_e::DISABLED;
    } else if (g_forge.config.is_safety_critical) {
        g_forge.gc.config.strings.gc_heuristic = sf_gc_heuristic_e::BUREAUCRATIC;
    } else {
        g_forge.gc.config.strings.gc_heuristic = sf_gc_heuristic_e::ADAPTIVE;
    }
}

MSIMD_API int SF_custominiVm_gc_is_no_sfgc(void) {
    return (g_forge.gc.config.strings.no_sfgc_overwrite_mode ||
            g_forge.gc.config.strings.gc_heuristic == sf_gc_heuristic_e::DISABLED) ? 1 : 0;
}

MSIMD_API int SacFor_Gearkit_NORMAX(void) {
    std::lock_guard<std::mutex> lock(g_forge.transition_lock);
    if (g_forge.vm_state.load(std::memory_order_relaxed) == SF_VM_STATE_UNINITIALIZED) {
        sf_vm_config_t cfg = parse_all_sf_flags("-#SF|Profile:Standard");
        cfg.profile = MSIMD_PROFILE_STANDARD;
        cfg.is_safety_critical = false;
        cfg.forbid_runtime_heap = false;
        cfg.disable_jit_deopt = false;
        return g_forge.init(cfg) ? 0 : -1;
    }

    g_forge.config.profile = MSIMD_PROFILE_STANDARD;
    g_forge.config.is_safety_critical = false;
    g_forge.config.forbid_runtime_heap = false;
    g_forge.config.disable_jit_deopt = false;
    g_forge.gc.config.is_safety_critical = false;
    g_forge.gc.runway.allocator.forbid_dynamic_expansion = false;
    g_forge.gc.start();
    g_forge.sandbox.start();
    return 0;
}

MSIMD_API int SacFor_Gearkit_SSIOLSC(void) {
    std::lock_guard<std::mutex> lock(g_forge.transition_lock);
    if (g_forge.vm_state.load(std::memory_order_relaxed) == SF_VM_STATE_UNINITIALIZED) {
        sf_vm_config_t cfg = parse_all_sf_flags("-#SF|Profile:SC");
        cfg.profile = MSIMD_PROFILE_SAFETY_CRITICAL;
        cfg.is_safety_critical = true;
        cfg.forbid_runtime_heap = true;
        cfg.disable_jit_deopt = true;
        return g_forge.init(cfg) ? 0 : -1;
    }

    g_forge.config.profile = MSIMD_PROFILE_SAFETY_CRITICAL;
    g_forge.config.is_safety_critical = true;
    g_forge.config.forbid_runtime_heap = true;
    g_forge.config.disable_jit_deopt = true;
    g_forge.gc.config.is_safety_critical = true;
    g_forge.gc.config.strings.gc_heuristic = sf_gc_heuristic_e::BUREAUCRATIC;
    g_forge.gc.runway.allocator.forbid_dynamic_expansion = true;

    g_forge.gc.running.store(false, std::memory_order_release);
    if (g_forge.gc.gc_thread.joinable()) {
        g_forge.gc.gc_thread.join();
    }
    g_forge.sandbox.stop();
    return 0;
}

MSIMD_API msimd_profile_mode_t SacFor_Gearkit_GetMode(void) {
    return g_forge.config.profile;
}

MSIMD_API int SacFor_Gearkit_SSIOLSC_Ex(SF_custominiVm_scgc_type_t scgc_type) {

    int ret = SacFor_Gearkit_SSIOLSC();
    if (ret != 0) return ret;

    sf_scgc_type_e internal_type = static_cast<sf_scgc_type_e>(scgc_type);
    g_forge.gc.config.strings.scgc_type = internal_type;

    if (internal_type == sf_scgc_type_e::PRIMITIVE_RW) {

        g_forge.gc.config.strings.no_sfgc_overwrite_mode = true;
        g_forge.gc.config.strings.gc_heuristic = sf_gc_heuristic_e::DISABLED;
        sacculus_coldlog_write(g_forge.gc.coldlog, SF_Ebin0_GC_SWEEP, SF_Ebin1_INFO,
            0, "Gearkit: SSIOLSC[PRIMITIVE_RW] â€” GC disabled, header bypass, raw sector R/W");
    } else {

        g_forge.gc.config.strings.no_sfgc_overwrite_mode = false;
        g_forge.gc.config.strings.gc_heuristic = sf_gc_heuristic_e::BUREAUCRATIC;
        sacculus_coldlog_write(g_forge.gc.coldlog, SF_Ebin0_GC_SWEEP, SF_Ebin1_INFO,
            0, "Gearkit: SSIOLSC[DEFAULT] â€” Phase-STW Bureaucratic, header chain alive");
    }
    return 0;
}

MSIMD_API void SF_custominiVm_set_scgc_type(SF_custominiVm_scgc_type_t type) {
    g_forge.gc.config.strings.scgc_type = static_cast<sf_scgc_type_e>(type);
    if (type == SF_SCGC_TYPE_PRIMITIVE_RW) {
        g_forge.gc.config.strings.no_sfgc_overwrite_mode = true;
        g_forge.gc.config.strings.gc_heuristic = sf_gc_heuristic_e::DISABLED;
    }
}

MSIMD_API SF_custominiVm_scgc_type_t SF_custominiVm_get_scgc_type(void) {
    return static_cast<SF_custominiVm_scgc_type_t>(
        g_forge.gc.config.strings.scgc_type);
}

MSIMD_API void* SF_primitiva_alloc(size_t bytes, size_t align) {
    if (!bytes || !g_forge.gc.config.strings.no_sfgc_overwrite_mode) return nullptr;
    if (align < 1) align = 64;

    uint8_t* base = g_forge.gc.runway.allocator.runway_heap_base;
    size_t   cap  = g_forge.gc.runway.allocator.runway_capacity;
    if (!base || !cap) return nullptr;

    size_t cur = g_forge.gc.runway.allocator.runway_bump_offset.load(std::memory_order_relaxed);
    for (;;) {
        size_t aligned_off = (cur + align - 1) & ~(align - 1);
        size_t end_off     = aligned_off + bytes;
        if (end_off > cap) return nullptr;
        if (g_forge.gc.runway.allocator.runway_bump_offset.compare_exchange_weak(
                cur, end_off, std::memory_order_acq_rel, std::memory_order_relaxed)) {
            return base + aligned_off;
        }

    }
}

MSIMD_API void SF_primitiva_write(void* sector_ptr, const void* src, size_t bytes) {
    if (!sector_ptr || !src || !bytes) return;
    memcpy(sector_ptr, src, bytes);
    sf_sync_store_fence();
}

MSIMD_API void SF_primitiva_read(const void* sector_ptr, void* dst, size_t bytes) {
    if (!sector_ptr || !dst || !bytes) return;
    sf_sync_load_fence();
    memcpy(dst, sector_ptr, bytes);
}

MSIMD_API void SF_primitiva_overwrite(void* sector_ptr, const void* src, size_t bytes) {

    if (!sector_ptr || !src || !bytes) return;
    memcpy(sector_ptr, src, bytes);
    sf_sync_store_fence();
}

MSIMD_API void SF_primitiva_zero(void* sector_ptr, size_t bytes) {

    if (!sector_ptr || !bytes) return;
    sf_critical_clear_memory(sector_ptr, bytes);
}

MSIMD_API size_t SF_custominiVm_heap_bytes(void) {
    return g_forge.gc.total_heap_bytes.load(std::memory_order_relaxed);
}

MSIMD_API uint64_t SF_custominiVm_gc_sweep_count(void) {
    return g_forge.gc.sweep_count.load(std::memory_order_relaxed);
}

MSIMD_API size_t SF_custominiVm_landfill_pit_used(void) {
    return g_forge.gc.landfill.pit.total_used();
}

MSIMD_API uint32_t SF_custominiVm_landfill_pane_count(void) {
    return g_forge.gc.landfill.pane.count.load(std::memory_order_relaxed);
}

MSIMD_API size_t SF_custominiVm_heap_rebated_bytes(void) {
    if (g_forge.gc.runway.allocator.superblock) {
        return g_forge.gc.runway.allocator.superblock->rebated_bytes.load(std::memory_order_relaxed);
    }
    return 0;
}

MSIMD_API uint32_t SF_custominiVm_heap_rebate_cycles(void) {
    if (g_forge.gc.runway.allocator.superblock) {
        return g_forge.gc.runway.allocator.superblock->rebate_cycle_count.load(std::memory_order_relaxed);
    }
    return 0;
}

MSIMD_API int SF_custominiVm_jit_load(const uint8_t* blob, size_t sz, uint32_t base_id) {
    return g_forge.jit.load_bitcode(blob, sz, base_id) ? 0 : -1;
}

MSIMD_API int SF_custominiVm_jit_resolve(const char* sym, uint32_t fn_id) {
    return g_forge.jit.resolve(sym, fn_id) ? 0 : -1;
}

MSIMD_API void SF_custominiVm_sandbox_test(uint32_t fn_id, SF_custominiVm_kernel_fn_t candidate,
                                        const char* name) {
    g_forge.sandbox.submit(fn_id, candidate, name);
}

MSIMD_API void SF_custominiVm_puzzle_submit(uint32_t fn_id, SF_custominiVm_kernel_fn_t candidate, const char* name,
                                        const char* target_isa, float target_epsilon,
                                        const SF_custominiVm_tensor_t* target_tensor) {
    g_forge.sandbox.submit(fn_id, candidate, name, target_isa, target_epsilon, target_tensor);
}

MSIMD_API void SF_custominiVm_puzzle_submit_suite(uint32_t fn_id,
                                               const SF_custominiVm_isa_candidate_t* candidates,
                                               int n_candidates,
                                               float target_epsilon,
                                               const SF_custominiVm_tensor_t* target_tensor) {
    g_forge.sandbox.submit_suite(fn_id, candidates, n_candidates, target_epsilon, target_tensor);
}

MSIMD_API void SF_custominiVm_puzzle_submit_batch(uint32_t fn_id,
                                                const SF_custominiVm_tensor_t* batch_samples,
                                                int sample_count,
                                                const SF_custominiVm_tensor_t* target_precision_tensor,
                                                float target_epsilon) {
    g_forge.sandbox.submit_batch(fn_id, batch_samples, sample_count, target_precision_tensor, target_epsilon);
}

MSIMD_API void SF_custominiVm_puzzle_set_sample_count(size_t count) {
    g_forge.sandbox.set_sample_count(count);
}

MSIMD_API size_t SF_custominiVm_puzzle_get_sample_count(void) {
    return g_forge.sandbox.get_sample_count();
}

MSIMD_API int SF_custominiVm_puzzle_is_stream_paused(void) {
    return g_stream_pause_gate.load(std::memory_order_acquire) ? 1 : 0;
}

MSIMD_API const char* SF_custominiVm_cpu_isa_description(void) {
    return g_forge.sandbox.cached_isa_inventory.description;
}

MSIMD_API int SF_custominiVm_cpu_has_isa(const char* isa_tag) {
    return g_forge.sandbox.cached_isa_inventory.has_isa(isa_tag) ? 1 : 0;
}

MSIMD_API int SF_custominiVm_cpu_isa_count(void) {
    return (int)g_forge.sandbox.cached_isa_inventory.discovered_isas.size();
}

MSIMD_API const char* SF_custominiVm_cpu_isa_at(int index) {
    if (index < 0 || (size_t)index >= g_forge.sandbox.cached_isa_inventory.discovered_isas.size()) {
        return nullptr;
    }
    return g_forge.sandbox.cached_isa_inventory.discovered_isas[index].c_str();
}

MSIMD_API const SF_custominiVm_hardware_topology_t* SF_custominiVm_hardware_topology(void) {
    return &g_forge.sandbox.cached_isa_inventory.topology;
}

MSIMD_API size_t SF_custominiVm_cpu_l1d_bytes(void) {
    return (size_t)g_forge.sandbox.cached_isa_inventory.topology.l1d.size_kb * 1024;
}

MSIMD_API size_t SF_custominiVm_cpu_l2_bytes(void) {
    return (size_t)g_forge.sandbox.cached_isa_inventory.topology.l2.size_kb * 1024;
}

MSIMD_API size_t SF_custominiVm_cpu_l3_bytes(void) {
    return (size_t)g_forge.sandbox.cached_isa_inventory.topology.l3.size_kb * 1024;
}

MSIMD_API size_t SF_custominiVm_cpu_cacheline_bytes(void) {
    return (size_t)g_forge.sandbox.cached_isa_inventory.topology.primary_cacheline_size;
}

MSIMD_API size_t SF_custominiVm_dram_total_bytes(void) {
    return (size_t)g_forge.sandbox.cached_isa_inventory.topology.total_phys_ram_bytes;
}

MSIMD_API size_t SF_custominiVm_dram_avail_bytes(void) {
    return (size_t)g_forge.sandbox.cached_isa_inventory.topology.avail_phys_ram_bytes;
}

MSIMD_API int SF_custominiVm_dram_channels(void) {
    return (int)g_forge.sandbox.cached_isa_inventory.topology.memory_channels;
}

MSIMD_API int SF_custominiVm_cpu_numa_nodes(void) {
    return (int)g_forge.sandbox.cached_isa_inventory.topology.numa_nodes;
}

MSIMD_API int SF_custominiVm_cpu_p_cores(void) {
    return (int)g_forge.sandbox.cached_isa_inventory.topology.p_cores;
}

MSIMD_API int SF_custominiVm_cpu_e_cores(void) {
    return (int)g_forge.sandbox.cached_isa_inventory.topology.e_cores;
}

MSIMD_API int SF_custominiVm_cpu_is_uma(void) {
    return (int)g_forge.sandbox.cached_isa_inventory.topology.is_uma;
}

MSIMD_API const char* SF_custominiVm_motherboard_model(void) {
    return g_forge.sandbox.cached_isa_inventory.topology.motherboard_model;
}

MSIMD_API uint32_t SF_custominiVm_dram_mhz(void) {
    return g_forge.sandbox.cached_isa_inventory.topology.dram_mhz;
}

MSIMD_API int SF_custominiVm_dram_is_xmp_expo(void) {
    return (int)g_forge.sandbox.cached_isa_inventory.topology.is_xmp_expo_active;
}

MSIMD_API uint32_t SF_custominiVm_cpu_current_mhz(void) {
    return sf_measure_cpu_cur_freq_mhz();
}

MSIMD_API void SF_custominiVm_upclocker_set_boost(int enable) {
    sf_upclocker_set_boost(enable);
    g_forge.sandbox.cached_isa_inventory.topology.is_upclocker_active = (enable != 0) ? 1 : 0;
}

MSIMD_API int SF_custominiVm_upclocker_is_boosted(void) {
    return sf_upclocker_is_boosted();
}

MSIMD_API void SF_custominiVm_upclocker_warmup(uint32_t duration_us) {
    sf_upclocker_t::pin_max_frequency_warmup(duration_us > 0 ? duration_us : 50);
}

MSIMD_API void SF_custominiVm_puzzle_set_mode(SF_custominiVm_puzzle_exec_mode_t mode) {
    g_forge.sandbox.set_mode(mode);
}

MSIMD_API SF_custominiVm_puzzle_exec_mode_t SF_custominiVm_puzzle_get_mode(void) {
    return g_forge.sandbox.get_mode();
}

MSIMD_API size_t SF_custominiVm_heap_chained_chunks(void) {
    return g_forge.gc.runway.allocator.chained_chunk_count.load(std::memory_order_relaxed);
}

MSIMD_API void* SF_custominiVm_tensor_resolve_ptr(const SF_custominiVm_tensor_t* tensor) {
    if (!tensor || !tensor->data) return nullptr;
    auto* obj = SF_custominiVm_live_header_t::from_body(tensor->data);
    if (!obj) {

        auto* eb = SF_custominiVm_emptybox_header_t::from_body(tensor->data);
        if (eb) {
            void* fwd = eb->forwarding_address.load(std::memory_order_acquire);
            if (fwd) {
                auto* new_obj = SF_custominiVm_live_header_t::from_body(fwd);
                if (new_obj) return new_obj->resolve_active_body();
                return fwd;
            }
        }
        return tensor->data;
    }
    auto* cold = get_cold(obj);
    if (cold && cold->is_paged_out.load(std::memory_order_acquire)) {
        g_forge.gc.pagefile.page_in(obj);
    }
    return obj->resolve_active_body();
}

static char g_anatomy_desc_buf[2048] = {0};

MSIMD_API void SF_custominiVm_anatomy_query(SF_custominiVm_anatomy_stat_t* out_stat) {
    if (!out_stat) return;
    memset(out_stat, 0, sizeof(*out_stat));

    if (g_forge.gc.runway.allocator.superblock) {
        auto* sb = g_forge.gc.runway.allocator.superblock;
        out_stat->onheap_reserved_bytes  = sb->total_reserved_bytes;
        out_stat->onheap_committed_bytes = sb->total_committed_bytes;
        out_stat->onheap_allocated_bytes = sb->allocated_bytes.load(std::memory_order_relaxed);
        out_stat->rebated_bytes          = sb->rebated_bytes.load(std::memory_order_relaxed) +
                                           g_forge.gc.runway.falline.total_rebated_bytes.load(std::memory_order_relaxed);
    } else {
        out_stat->rebated_bytes          = g_forge.gc.runway.falline.total_rebated_bytes.load(std::memory_order_relaxed);
    }

    out_stat->runway_total_bytes     = g_forge.gc.runway.allocator.runway_capacity;
    out_stat->runway_used_bytes      = g_forge.gc.runway.allocator.runway_bump_offset.load(std::memory_order_relaxed);
    out_stat->motherline_live_bytes  = g_forge.gc.total_heap_bytes.load(std::memory_order_relaxed);
    out_stat->emptybox_count         = g_forge.gc.runway.falline.emptybox_count.load(std::memory_order_relaxed);
    out_stat->falline_emptybox_bytes = out_stat->emptybox_count * 128;

    size_t tank_bytes = 0;
    size_t tank_count = 0;
    for (int r = 0; r < SACFOR_ROOM_COUNT; ++r) {
        auto& reg = g_forge.gc.motherline.rooms[r].regions[2];
        tank_bytes += reg.total_bytes.load(std::memory_order_relaxed);
        tank_count += reg.count.load(std::memory_order_relaxed);
    }
    out_stat->tank_regions_bytes = tank_bytes;
    out_stat->tank_regions_count = tank_count;

    out_stat->puzzle_arena_total_bytes = g_forge.sandbox.puzzle_heap_capacity;
    out_stat->puzzle_arena_used_bytes  = g_forge.sandbox.puzzle_heap_used.load(std::memory_order_relaxed);
    out_stat->onheap_reserved_bytes   += out_stat->puzzle_arena_total_bytes;
    out_stat->onheap_committed_bytes  += out_stat->puzzle_arena_total_bytes;
    out_stat->onheap_allocated_bytes  += out_stat->puzzle_arena_used_bytes;

    out_stat->landfill_total_bytes   = g_forge.gc.landfill.pit.matured_arena.capacity + g_forge.gc.landfill.pit.rehab_arena.capacity;
    out_stat->landfill_used_bytes    = g_forge.gc.landfill.pit.total_used();
    out_stat->matured_arena_bytes    = g_forge.gc.landfill.pit.matured_arena.used.load(std::memory_order_relaxed);
    out_stat->rehab_arena_bytes      = g_forge.gc.landfill.pit.rehab_arena.used.load(std::memory_order_relaxed);
    out_stat->landfill_pane_count    = g_forge.gc.landfill.pane.count.load(std::memory_order_relaxed);

    out_stat->offheap_code_arena_bytes   = g_forge.code_arena.size;
    out_stat->offheap_telemetry_bytes    = (sizeof(SF_custominiVm_hotlog_entry_t) * SACFOR_HOTLOG_SIZE) +
                                           (sizeof(SF_custominiVm_coldlog_entry_t) * SACFOR_COLDLOG_SIZE);

    out_stat->chained_chunk_count    = g_forge.gc.runway.allocator.chained_chunk_count.load(std::memory_order_relaxed);

    out_stat->pacing_stall_count     = g_forge.gc.runway.pacing_stalls.load(std::memory_order_relaxed);
    out_stat->paged_out_bytes        = g_forge.gc.pagefile.paged_out_bytes.load(std::memory_order_relaxed);
    out_stat->paged_out_count        = g_forge.gc.pagefile.paged_out_count.load(std::memory_order_relaxed);
}

MSIMD_API const char* SF_custominiVm_anatomy_description(void) {
    SF_custominiVm_anatomy_stat_t s{};
    SF_custominiVm_anatomy_query(&s);
    snprintf(g_anatomy_desc_buf, sizeof(g_anatomy_desc_buf),
        "SFVM Unified Anatomy [Shenandoah-Model Elastic Segments]:\n"
        "  - On-Heap (Total Reserved: %zu MB, Committed: %zu MB, Allocated: %zu MB)\n"
        "    + 1. Runway Segment   : %zu MB (Live Motherline: %zu KB, Falline Emptyboxes: %zu [~%zu KB])\n"
        "    + 2. Tank Regions     : %zu KB across rooms (Humongous SIMD matrices, Count: %zu)\n"
        "    + 3. Puzzle Arena     : %zu MB (3rd On-Heap Region: Isolated Sandbox Test Area, Used: %zu KB)\n"
        "    + 4. Landfill Area    : %zu MB (Matured: %zu KB, Rehab: %zu KB, Pane Slots: %u)\n"
        "  - Off-Heap (GC-Isolated Continuous Exec & Telemetry):\n"
        "    + Code Arena          : %zu MB (JIT Exec Pages)\n"
        "    + Telemetry Rings     : %zu MB (HotLog 64MB + ColdLog 1MB)\n"
        "  - Paging & Backpressure : %zu Pacing Stalls, %zu KB Swapped to Pagefile (%zu blocks)\n"
        "  - Chained OS Chunks     : %zu (Rebated to OS: %zu KB)",
        s.onheap_reserved_bytes / (1024 * 1024), s.onheap_committed_bytes / (1024 * 1024), s.onheap_allocated_bytes / (1024 * 1024),
        s.runway_total_bytes / (1024 * 1024), s.motherline_live_bytes / 1024, s.emptybox_count, s.falline_emptybox_bytes / 1024,
        s.tank_regions_bytes / 1024, s.tank_regions_count,
        s.puzzle_arena_total_bytes / (1024 * 1024), s.puzzle_arena_used_bytes / 1024,
        s.landfill_total_bytes / (1024 * 1024), s.matured_arena_bytes / 1024, s.rehab_arena_bytes / 1024, s.landfill_pane_count,
        s.offheap_code_arena_bytes / (1024 * 1024),
        s.offheap_telemetry_bytes / (1024 * 1024),
        s.pacing_stall_count, s.paged_out_bytes / 1024, s.paged_out_count,
        s.chained_chunk_count, s.rebated_bytes / 1024);
    return g_anatomy_desc_buf;
}

MSIMD_API int SF_custominiVm_folder_mkdir(const char* path, size_t quota_bytes) {
    return g_forge.folder_tree.mkdir_p(path, quota_bytes) ? 0 : -1;
}

MSIMD_API int SF_custominiVm_folder_bind(const char* path, SF_custominiVm_tensor_t* tensor) {
    if (!tensor || !tensor->data) return -1;
    auto* obj = SF_custominiVm_live_header_t::from_body(tensor->data);
    if (!obj) return -1;
    return g_forge.folder_tree.bind_tensor(path, obj) ? 0 : -1;
}

MSIMD_API size_t SF_custominiVm_folder_evict(const char* path) {
    return g_forge.folder_tree.evict_folder(path, &g_forge.gc.runway);
}

MSIMD_API size_t SF_custominiVm_folder_used_bytes(const char* path) {
    return g_forge.folder_tree.get_used_bytes(path);
}

MSIMD_API int SF_custominiVm_vfs_mkdir(const char* path, size_t quota_bytes) {
    return g_forge.folder_tree.mkdir_p(path, quota_bytes) ? 0 : -1;
}

MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_vfs_alloc_tensor(const char* path, const int32_t* shape, int ndim, int dtype) {
    SF_custominiVm_tensor_t t = SF_custominiVm_tensor_alloc(shape, ndim, dtype);
    if (t.data && path) {
        auto* obj = SF_custominiVm_live_header_t::from_body(t.data);
        if (obj) {
            std::vector<std::string> parts;
            sfFolderTree::split_path(path, parts);
            if (!parts.empty()) {
                strncpy(get_cold(obj)->debug_name, parts.back().c_str(), sizeof(get_cold(obj)->debug_name) - 1);
            }
            g_forge.folder_tree.bind_tensor(path, obj);
        }
    }
    return t;
}

MSIMD_API int SF_custominiVm_vfs_unlink(const char* path) {
    return g_forge.folder_tree.unlink_file(path, &g_forge.gc.runway) ? 0 : -1;
}

MSIMD_API size_t SF_custominiVm_vfs_rmdir_rf(const char* path) {
    return g_forge.folder_tree.evict_folder(path, &g_forge.gc.runway);
}

MSIMD_API int SF_custominiVm_vfs_stat(const char* path, SF_custominiVm_vfs_stat_t* out_stat) {
    return g_forge.folder_tree.stat_path(path, out_stat) ? 0 : -1;
}

MSIMD_API size_t SF_custominiVm_vfs_snapshot(const char* path, void* out_buf, size_t buf_cap) {
    return g_forge.folder_tree.snapshot_path(path, out_buf, buf_cap);
}

MSIMD_API void SF_custominiVm_vfs_ls(const char* path, void (*cb)(const char* name, int is_dir, size_t bytes, void* user), void* user) {
    g_forge.folder_tree.list_dir(path, cb, user);
}

MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_tensor_slice(const SF_custominiVm_tensor_t* parent, size_t offset_bytes, const int32_t* shape, int ndim, int dtype) {
    SF_custominiVm_tensor_t t{};
    if (!parent || !parent->data || ndim <= 0 || !shape) return t;
    if (ndim > 8) ndim = 8;
    t.ndim = ndim;
    t.dtype = (uint8_t)dtype;
    size_t elems = 1;
    for (int i = 0; i < ndim; ++i) {
        int32_t dim_val = shape[i] > 0 ? shape[i] : 1;
        t.shape[i] = dim_val;
        size_t next_elems;
#if defined(__GNUC__) || defined(__clang__)
        if (__builtin_mul_overflow(elems, (size_t)dim_val, &next_elems)) return SF_custominiVm_tensor_t{};
#else
        if (elems > SIZE_MAX / (size_t)dim_val) return SF_custominiVm_tensor_t{};
        next_elems = elems * (size_t)dim_val;
#endif
        elems = next_elems;
    }

    size_t d_sz = sf_dtype_size((sfDType)dtype);
#if defined(__GNUC__) || defined(__clang__)
    if (__builtin_mul_overflow(elems, d_sz, &t.byte_size)) return SF_custominiVm_tensor_t{};
#else
    if (elems > SIZE_MAX / d_sz) return SF_custominiVm_tensor_t{};
    t.byte_size = elems * d_sz;
#endif

    if (t.byte_size > parent->byte_size || offset_bytes > parent->byte_size - t.byte_size) {
        return SF_custominiVm_tensor_t{};
    }

    auto* parent_hdr = SF_custominiVm_live_header_t::from_body(parent->data);
    if (!parent_hdr) {

        t.data = static_cast<uint8_t*>(parent->data) + offset_bytes;
        return t;
    }

    auto* parent_cold = get_cold(parent_hdr);
    if (parent_cold && parent_cold->is_paged_out.load(std::memory_order_acquire)) {
        g_forge.gc.pagefile.page_in(parent_hdr);
    }

    void* view_body = g_forge.gc.runway.allocate_cell(0, parent_hdr->function_id, "slice_view");
    if (!view_body) return t;

    auto* view_hdr = SF_custominiVm_live_header_t::from_body(view_body);
    if (view_hdr) {
        view_hdr->parent_header.store(parent_hdr, std::memory_order_release);
        view_hdr->slice_byte_offset = offset_bytes;
        auto* view_cold = get_cold(view_hdr);
        if (view_cold && parent_cold) {
            view_cold->type_sig_hash = parent_cold->type_sig_hash;
        }
        view_hdr->memory_footprint.store(t.byte_size, std::memory_order_relaxed);
        parent_hdr->child_view_count.fetch_add(1, std::memory_order_release);
    }

    t.data = view_body;
    return t;
}

MSIMD_API int SF_custominiVm_pagefile_init(size_t max_bytes, SF_custominiVm_paging_policy_t policy) {
    return g_forge.gc.pagefile.init(max_bytes, policy) ? 0 : -1;
}

MSIMD_API int SF_custominiVm_page_out(SF_custominiVm_tensor_t* tensor) {
    if (g_forge.config.is_safety_critical) return -1;
    if (!tensor || !tensor->data) return -1;
    auto* obj = SF_custominiVm_live_header_t::from_body(tensor->data);
    if (!obj) return -1;
    return g_forge.gc.pagefile.page_out(obj) ? 0 : -1;
}

MSIMD_API int SF_custominiVm_page_in(SF_custominiVm_tensor_t* tensor) {
    if (!tensor || !tensor->data) return -1;
    auto* obj = SF_custominiVm_live_header_t::from_body(tensor->data);
    if (!obj) return -1;
    auto* p = obj->parent_header.load(std::memory_order_acquire);
    if (p) {
        g_forge.gc.pagefile.page_in(p);
    }
    return g_forge.gc.pagefile.page_in(obj) ? 0 : -1;
}

MSIMD_API size_t SF_custominiVm_pagefile_used_bytes(void) {
    return g_forge.gc.pagefile.paged_out_bytes.load(std::memory_order_relaxed);
}

MSIMD_API size_t SF_custominiVm_pacing_stalls(void) {
    return g_forge.gc.runway.pacing_stalls.load(std::memory_order_relaxed);
}

static SF_custominiVm_gpio_desc_t g_sf_gpio_desc{};
static std::atomic<bool>          g_sf_gpio_ready{false};
static std::atomic<uint32_t>      g_sf_gpio_simulated_pins{0};

MSIMD_API int SF_custominiVm_sbc_detect_board(SF_custominiVm_gpio_desc_t* out_desc) {
    if (!out_desc) return -1;
    memset(out_desc, 0, sizeof(*out_desc));
    out_desc->board_type = SF_SBC_NONE;
    out_desc->total_pins = 0;
    strncpy(out_desc->board_name, "Standard PC/Server", sizeof(out_desc->board_name) - 1);

#if defined(__linux__)

    FILE* fp = fopen("/proc/device-tree/model", "r");
    char model_buf[128] = {0};
    if (fp) {
        if (fgets(model_buf, sizeof(model_buf) - 1, fp)) {
            size_t l = strlen(model_buf);
            while (l > 0 && (model_buf[l-1] == '\n' || model_buf[l-1] == '\r')) model_buf[--l] = '\0';
        }
        fclose(fp);
    }

    if (model_buf[0] != '\0') {
        strncpy(out_desc->board_name, model_buf, sizeof(out_desc->board_name) - 1);
        if (strstr(model_buf, "Raspberry Pi 5")) {
            out_desc->board_type = SF_SBC_RASPBERRY_PI_5;
            out_desc->total_pins = 40;
            out_desc->mmio_phys_base = 0x1F00000000ULL;
        } else if (strstr(model_buf, "Raspberry Pi 4")) {
            out_desc->board_type = SF_SBC_RASPBERRY_PI_4;
            out_desc->total_pins = 40;
            out_desc->mmio_phys_base = 0xFE200000ULL;
        } else if (strstr(model_buf, "Raspberry Pi 3") || strstr(model_buf, "Raspberry Pi 2")) {
            out_desc->board_type = SF_SBC_RASPBERRY_PI_2_3;
            out_desc->total_pins = 40;
            out_desc->mmio_phys_base = 0x3F200000ULL;
        } else if (strstr(model_buf, "Raspberry Pi") || strstr(model_buf, "Pi Zero")) {
            out_desc->board_type = SF_SBC_RASPBERRY_PI_1_ZERO;
            out_desc->total_pins = 26;
            out_desc->mmio_phys_base = 0x20200000ULL;
        } else if (strstr(model_buf, "Orange Pi")) {
            out_desc->board_type = SF_SBC_ORANGE_PI;
            out_desc->total_pins = 40;
            out_desc->mmio_phys_base = 0x0300B000ULL;
        } else if (strstr(model_buf, "Jetson") || strstr(model_buf, "Tegra")) {
            out_desc->board_type = SF_SBC_NVIDIA_JETSON;
            out_desc->total_pins = 40;
            out_desc->mmio_phys_base = 0x02200000ULL;
        }
    }
#elif defined(_WIN32)

    const char* mb_model = SF_custominiVm_motherboard_model();
    if (mb_model && (strstr(mb_model, "LattePanda") || strstr(mb_model, "DFRobot"))) {
        out_desc->board_type = SF_SBC_LATTEPANDA_X86;
        out_desc->is_coprocessor_bridge = 1;
        out_desc->total_pins = 24;
        strncpy(out_desc->board_name, "LattePanda x86 + Leonardo/RP2040 Coprocessor", sizeof(out_desc->board_name) - 1);
    }
#endif

    g_sf_gpio_desc = *out_desc;
    return 0;
}

MSIMD_API int SF_custominiVm_gpio_init(SF_custominiVm_gpio_desc_t* desc) {
    if (!desc) return -1;
    if (desc->board_type == SF_SBC_NONE) {
        g_sf_gpio_ready.store(true, std::memory_order_release);
        return 0;
    }

#if defined(__linux__)
    int mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    }
    if (mem_fd >= 0) {
        void* map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, desc->mmio_phys_base);
        close(mem_fd);
        if (map != MAP_FAILED) {
            desc->mmio_virtual_base = map;
            desc->is_fast_mmio_active = 1;
        }
    }
#endif
    g_sf_gpio_desc = *desc;
    g_sf_gpio_ready.store(true, std::memory_order_release);
    return 0;
}

MSIMD_API int SF_custominiVm_gpio_set_mode(uint32_t pin, SF_custominiVm_gpio_pin_mode_e mode) {
    (void)pin; (void)mode;
    return 0;
}

MSIMD_API void SF_custominiVm_gpio_fast_write(uint32_t pin, uint8_t level) {
    if (pin >= 64) return;

    uint32_t bank    = pin / 32;
    uint32_t bit     = 1u << (pin % 32);
    if (g_sf_gpio_desc.is_fast_mmio_active && g_sf_gpio_desc.mmio_virtual_base) {

        volatile uint32_t* gpio = static_cast<volatile uint32_t*>(g_sf_gpio_desc.mmio_virtual_base);
        if (level) {
            gpio[7 + bank] = bit;
        } else {
            gpio[10 + bank] = bit;
        }
        return;
    }

    if (level) g_sf_gpio_simulated_pins.fetch_or(bit, std::memory_order_relaxed);
    else       g_sf_gpio_simulated_pins.fetch_and(~bit, std::memory_order_relaxed);
}

MSIMD_API uint8_t SF_custominiVm_gpio_fast_read(uint32_t pin) {
    if (pin >= 64) return 0;

    uint32_t bank    = pin / 32;
    uint32_t bit     = 1u << (pin % 32);
    if (g_sf_gpio_desc.is_fast_mmio_active && g_sf_gpio_desc.mmio_virtual_base) {

        volatile uint32_t* gpio = static_cast<volatile uint32_t*>(g_sf_gpio_desc.mmio_virtual_base);
        return (gpio[13 + bank] & bit) ? 1 : 0;
    }
    return (g_sf_gpio_simulated_pins.load(std::memory_order_relaxed) & bit) ? 1 : 0;
}

struct sf_sensor_attached_entry_t {
    SF_custominiVm_tensor_t        stream_tensor{};
    SF_custominiVm_sensor_config_t config{};
    std::atomic<size_t>            write_cursor{0};
    std::atomic<bool>              active{false};
};
static sf_sensor_attached_entry_t g_sf_sensor_slots[8];
static std::mutex                 g_sf_sensor_lock;

MSIMD_API int SF_custominiVm_sensor_attach_stream(const SF_custominiVm_sensor_config_t* cfg, SF_custominiVm_tensor_t* out_stream_tensor) {
    if (!cfg || !out_stream_tensor) return -1;
    std::lock_guard<std::mutex> g(g_sf_sensor_lock);

    int free_slot = -1;
    for (int i = 0; i < 8; ++i) {
        if (!g_sf_sensor_slots[i].active.load(std::memory_order_relaxed)) {
            free_slot = i;
            break;
        }
    }
    if (free_slot < 0) return -2;

    size_t samples = cfg->buffer_sample_count > 0 ? cfg->buffer_sample_count : 4096;
    int32_t shape[1] = { (int32_t)samples };

    SF_custominiVm_tensor_t t = SF_custominiVm_tensor_alloc(shape, 1, SF_DTYPE_F32);
    if (!t.data) return -3;

    t.flags |= SF_TENSOR_FLAG_PINNED;
    auto* hdr = SF_custominiVm_live_header_t::from_body(t.data);
    if (hdr) hdr->try_pin();

    auto& slot = g_sf_sensor_slots[free_slot];
    slot.stream_tensor = t;
    slot.config = *cfg;
    slot.write_cursor.store(0, std::memory_order_relaxed);
    slot.active.store(true, std::memory_order_release);

    *out_stream_tensor = t;
    return 0;
}

MSIMD_API size_t SF_custominiVm_sensor_poll_stream(SF_custominiVm_tensor_t* stream_tensor, size_t max_samples) {
    if (!stream_tensor || !stream_tensor->data) return 0;
    std::lock_guard<std::mutex> g(g_sf_sensor_lock);

    for (int i = 0; i < 8; ++i) {
        auto& slot = g_sf_sensor_slots[i];
        if (slot.active.load(std::memory_order_relaxed) && slot.stream_tensor.data == stream_tensor->data) {
            float* buf = static_cast<float*>(stream_tensor->data);
            size_t cap = slot.stream_tensor.shape[0];
            size_t to_sample = (max_samples > 0 && max_samples < cap) ? max_samples : 64;

            size_t cur = slot.write_cursor.load(std::memory_order_relaxed);
            for (size_t s = 0; s < to_sample; ++s) {
                float val = 0.0f;
                if (slot.config.sensor_type == SF_SENSOR_TEMP_HUMIDITY) {
                    val = 24.5f + 0.1f * (float)(rand() % 10);
                } else if (slot.config.sensor_type == SF_SENSOR_WATER_LEVEL) {
                    val = 680.0f + (float)(rand() % 20);
                } else if (slot.config.sensor_type == SF_SENSOR_LOGIC_ANALYZER_32CH) {
                    val = (float)SF_custominiVm_gpio_fast_read(slot.config.pin_primary);
                } else {
                    val = (float)(rand() % 1024);
                }
                buf[(cur + s) % cap] = val;
            }
            slot.write_cursor.store((cur + to_sample) % cap, std::memory_order_relaxed);
            return to_sample;
        }
    }
    return 0;
}

MSIMD_API void SF_custominiVm_sensor_detach_stream(SF_custominiVm_tensor_t* stream_tensor) {
    if (!stream_tensor || !stream_tensor->data) return;
    std::lock_guard<std::mutex> g(g_sf_sensor_lock);

    for (int i = 0; i < 8; ++i) {
        auto& slot = g_sf_sensor_slots[i];
        if (slot.active.load(std::memory_order_relaxed) && slot.stream_tensor.data == stream_tensor->data) {
            slot.active.store(false, std::memory_order_release);
            auto* hdr = SF_custominiVm_live_header_t::from_body(slot.stream_tensor.data);
            if (hdr) hdr->unpin();
            SF_custominiVm_tensor_free(&slot.stream_tensor);
            break;
        }
    }
}

static SF_custominiVm_ring0_status_t g_sf_ring0_status{};

MSIMD_API int SF_custominiVm_ring0_connect(SF_custominiVm_ring0_status_t* out_status) {
    memset(&g_sf_ring0_status, 0, sizeof(g_sf_ring0_status));

#ifdef _WIN32
    HANDLE hDevice = CreateFileA("\\\\.\\SFVM_K",
                                 GENERIC_READ | GENERIC_WRITE,
                                 0, NULL, OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDevice != INVALID_HANDLE_VALUE) {
        g_sf_ring0_status.is_driver_loaded = 1;
        g_sf_ring0_status.driver_version = 0x0100;
        CloseHandle(hDevice);
    }
#elif defined(__linux__)
    int fd = open("/dev/sfvm_k", O_RDWR);
    if (fd >= 0) {
        g_sf_ring0_status.is_driver_loaded = 1;
        g_sf_ring0_status.driver_version = 0x0100;
        close(fd);
    }
#endif

    if (!g_sf_ring0_status.is_driver_loaded) {
        g_sf_ring0_status.driver_version = 0x0001;
    }

    if (out_status) *out_status = g_sf_ring0_status;
    return g_sf_ring0_status.is_driver_loaded ? 0 : 1;
}

MSIMD_API int SF_custominiVm_ring0_apply_hwp_policy(const SF_custominiVm_ring0_hwp_policy_t* policy) {
    if (!policy) return -1;
    if (g_sf_ring0_status.is_driver_loaded) {
        return 0;
    }
    if (policy->anti_downclock_boost) {
        sf_upclocker_set_boost(1);
        sf_upclocker_t::pin_max_frequency_warmup(100);
    }
    return 0;
}

MSIMD_API void SF_custominiVm_ring0_disconnect(void) {
    g_sf_ring0_status.is_driver_loaded = 0;
}

MSIMD_API void SF_custominiVm_register_gc_hook(SF_custominiVm_gc_hook_fn_t hook, void* user_data) {
    g_forge.gc.gc_hook = hook;
    g_forge.gc.gc_hook_user = user_data;
}

MSIMD_API void SF_custominiVm_host_gc_notify(uint32_t host_event_id) {
    if (host_event_id == SF_HOST_GC_JVM_START ||
        host_event_id == SF_HOST_GC_CLR_START ||
        host_event_id == SF_HOST_GC_PYTHON_START) {
        sf_cpu_pause();
    }
}

MSIMD_API size_t SF_custominiVm_host_loop_tick(uint32_t budget_us) {
    (void)budget_us;
    auto res = g_forge.gc.step();
    return res.swept + res.drained;
}

}

#ifdef __cplusplus
namespace sfvm {

using PinGuard = sfPinGuard;

inline sfTensor tensor(void* data, std::initializer_list<int32_t> shape,
                       sfDType dtype = SF_DTYPE_F32) {
    std::vector<int32_t> sv(shape);
    return SF_custominiVm_tensor_from(data, sv.data(), (int)sv.size(), (int)dtype);
}

inline sfTensor alloc(std::initializer_list<int32_t> shape,
                      sfDType dtype = SF_DTYPE_F32) {
    std::vector<int32_t> sv(shape);
    return SF_custominiVm_tensor_alloc(sv.data(), (int)sv.size(), (int)dtype);
}

inline void free(sfTensor& t) {
    SF_custominiVm_tensor_free(&t);
}

inline sfTensor slice(const sfTensor& parent, size_t offset_bytes,
                      std::initializer_list<int32_t> shape,
                      sfDType dtype = SF_DTYPE_F32) {
    std::vector<int32_t> sv(shape);
    return SF_custominiVm_tensor_slice(&parent, offset_bytes, sv.data(), (int)sv.size(), (int)dtype);
}

inline void* resolve_ptr(const sfTensor& t) {
    return SF_custominiVm_tensor_resolve_ptr(&t);
}

inline int pagefile_init(size_t max_bytes, sfPagingPolicy policy = SF_PAGING_FIFO) {
    return SF_custominiVm_pagefile_init(max_bytes, policy);
}

inline int page_out(sfTensor* t) {
    return SF_custominiVm_page_out(t);
}

inline int page_in(sfTensor* t) {
    return SF_custominiVm_page_in(t);
}

inline size_t pagefile_used_bytes(void) {
    return SF_custominiVm_pagefile_used_bytes();
}

inline size_t pacing_stalls(void) {
    return SF_custominiVm_pacing_stalls();
}

inline bool init(size_t code_mb = 64, size_t pressure_mb = 256) {
    return SF_custominiVm_init(code_mb, pressure_mb) == 0;
}

inline void shutdown(void) { SF_custominiVm_shutdown(); }

inline msimd_status_t call(uint32_t fn_id,
                        std::initializer_list<sfTensor> inputs,
                        sfTensor* out, void* user = nullptr) {
    std::vector<sfTensor> iv(inputs);
    return (msimd_status_t)SF_custominiVm_call(fn_id, iv.data(), (int)iv.size(),
                                            out, 1, user);
}

inline size_t gc(bool owp = false) {
    return SF_custominiVm_gc_run(SF_GC_RUN_SINGLE_STEP, owp ? 1 : 0, nullptr, nullptr);
}
inline size_t gc(SF_custominiVm_gc_run_mode_t mode, bool owp = false) {
    return SF_custominiVm_gc_run(mode, owp ? 1 : 0, nullptr, nullptr);
}

}

#endif

