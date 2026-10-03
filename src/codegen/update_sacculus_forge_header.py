# -*- coding: utf-8 -*-
"""
Script to generate the fully-documented, English-only corporate Doxygen version of include/SacculusForge.h.
"""

content = '''/**
 * @file SacculusForge.h
 * @brief SacculusForge Virtual Machine (SFVM) Runtime Engine, JIT Compiler, Generational Garbage Collector (sfGC), and Heterogeneous Hardware Interop ABI.
 *
 * =================================================================================================
 * SACCULUSFORGE RUNTIME ARCHITECTURE & SPECIFICATION SPECIFICATION SPECIFICATION SPECIFICATION
 * =================================================================================================
 * The SacculusForge Virtual Machine (SFVM, represented via the C API family SF_custominiVm_*) is a
 * high-performance, deterministic execution environment engineered specifically for high-throughput
 * scientific tensor computation, multi-tier SIMD acceleration, and safety-critical real-time processing.
 * Designed with a strict zero-overhead philosophy, SFVM unifies a multi-generational memory model,
 * an off-heap JIT code arena, a zero-syscall memory-mapped I/O (MMIO) hardware engine, and a low-latency
 * Ring-0 kernel bridge into a single, cohesive binary.
 *
 * <h3>1. Memory Topography & Generational sfGC Model</h3>
 * Memory within SFVM is partitioned into distinct On-Heap and Off-Heap domains, strictly separated
 * to provide predictable allocation pacing, lockless telemetry, and deterministic pause profiles:
 * - <b>Runway (Young & Nursery Domain):</b> Comprises the active allocation space divided into the
 *   <b>Motherline</b> (where newly allocated tensor payloads and live headers reside) and the
 *   <b>Falline</b> (a chain of segregated, recycled empty boxes designed for rapid LIFO/FIFO reuse).
 * - <b>Tank Regions (Humongous Tensor Domain):</b> Dedicated, contiguous large-page memory allocations
 *   reserved for massive multi-megabyte and multi-gigabyte tensors that exceed standard slab chunk sizes.
 *   These regions bypass standard generational sliding compaction to eliminate copying overhead.
 * - <b>Landfill (Mature & Evacuated Domain):</b> Segregated memory arena where aged objects that have
 *   survived multiple GC generational sweeps undergo final reclamation, rehabilitation, or eviction
 *   to external backing stores (such as anonymous disk swap pagefiles).
 * - <b>Independent Puzzle Arena:</b> An on-heap isolated execution sandbox designed for continuous,
 *   concurrent auto-tuning and benchmarking of competing SIMD/ISA vector kernels (e.g. AVX-512, AVX2,
 *   ARM SVE2, RISC-V RVV) against precision and epsilon tolerances.
 * - <b>Off-Heap Arenas:</b> Completely outside the jurisdiction of sfGC collectors. Houses executable JIT
 *   code buffers protected via strict W^X (Write XOR Execute) memory page permissions, as well as circular
 *   lockless telemetry ring buffers (HotLog and ColdLog) mapped to cacheline-isolated off-heap pages.
 *
 * <h3>2. Dual Standard Profile Architecture (NORMAX vs. SSIOLSC)</h3>
 * Modeled after the Khronos Vulkan SC (Safety Critical) paradigm, SFVM compiles into a single unified
 * binary that dynamically supports two mutually exclusive runtime profiles:
 * - <b>NORMAX (High-Throughput HPC Profile):</b> Optimized for peak floating-point throughput, aggressive
 *   dynamic slab chunk chaining, adaptive background concurrent garbage collection, and dynamic SIMD
 *   speculation across heterogeneous symmetric and asymmetric cores.
 * - <b>SSIOLSC (Deterministic Safety-Critical Profile):</b> Compliant with ISO 26262 ASIL-D, IEC 61508,
 *   and DO-178C requirements. Dynamic memory growth after initialization is strictly forbidden.
 *   Memory compaction is executed strictly within deterministic phase boundaries (Phase-STW) or operates
 *   in pure Zero-GC mode (PrimitiveRW), bypassing object headers entirely for direct sector writes.
 *
 * <h3>3. Zero-Syscall MMIO & Heterogeneous Hardware Bridges</h3>
 * SFVM provides native physical hardware integration targeting both standard x86_64/ARM64 servers and
 * Single-Board Computers (Raspberry Pi 1-5, LattePanda, NVIDIA Jetson, Orange Pi). Through direct virtual
 * mapping of peripheral physical addresses, GPIO lines and real-time sensor streams (IMU, optical, analog)
 * are ingested directly into pinned, cache-aligned tensor buffers with sub-microsecond latency.
 *
 * <h3>4. Concurrency, ABI & Multi-Language Bindings</h3>
 * All C interfaces expose standard C-linkage (`extern "C"`), ensuring seamless interoperability with
 * modern foreign function interfaces (FFI), including Java Foreign Function & Memory (FFM) API (JEP 424/442),
 * .NET CoreCLR P/Invoke, Rust `cxx`, Python `nanobind`/`ctypes`, and ISO C++20. All structures maintain
 * 64-byte or 32-byte cacheline alignment to eliminate false sharing across multi-threaded cores.
 */

#ifndef SACCULUS_FORGE_H
#define SACCULUS_FORGE_H

#include "MathventageSIMD.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */
/* CORE ARCHITECTURE CONSTANTS & AFFINITY BITMAP SPECIFICATION               */
/* ========================================================================= */

/**
 * @def SF_MAX_CORES
 * @brief Maximum number of hardware logical processor cores monitored and tracked by SFVM.
 *
 * Tracks processor affinity and per-core cycle telemetry across up to 256 logical execution units.
 * Represented internally as a contiguous 256-bit affinity bitset (4 consecutive 64-bit words).
 * This layout ensures zero-allocation thread affinity masks compatible with Windows Processor Groups
 * and Linux CPU_SET256 kernel scheduling structures.
 */
#define SF_MAX_CORES            256u

/**
 * @def SF_CORE_WORDS
 * @brief Number of 64-bit integer words required to represent the 256-core affinity bitmask.
 *
 * Formula: 256 cores / 64 bits per word = 4 words. Each word corresponds to a quadword bitmask,
 * allowing single-instruction bitwise tests (such as `popcnt`, `tzcnt`, and `blsr`) to evaluate
 * multi-core residency with single-cycle latency.
 */
#define SF_CORE_WORDS           4u

/**
 * @def SACFOR_MAX_CORES
 * @brief Legacy alias for SF_MAX_CORES to preserve backward source compatibility.
 */
#define SACFOR_MAX_CORES        SF_MAX_CORES

/**
 * @def SACFOR_CORE_WORDS
 * @brief Legacy alias for SF_CORE_WORDS to preserve backward source compatibility.
 */
#define SACFOR_CORE_WORDS       SF_CORE_WORDS

/* ========================================================================= */
/* TENSOR DATA TYPES & BUFFER FLAGS                                          */
/* ========================================================================= */

/**
 * @enum SF_custominiVm_dtype_t
 * @brief Primitive data element types supported by the SFVM tensor runtime.
 *
 * Enumerates all fundamental arithmetic types valid for tensor memory buffers.
 * Each data type adheres strictly to IEEE 754 floating-point standards or two\\'s complement
 * integer formats, with deterministic bit-widths and alignment requirements.
 */
typedef enum SF_custominiVm_dtype_t {
    /** @brief 32-bit single-precision floating point (IEEE 754 binary32), 4-byte element size. */
    SF_DTYPE_F32 = 0,
    /** @brief 64-bit double-precision floating point (IEEE 754 binary64), 8-byte element size. */
    SF_DTYPE_F64 = 1,
    /** @brief 32-bit signed two\\'s-complement integer, 4-byte element size. */
    SF_DTYPE_I32 = 2,
    /** @brief 64-bit signed two\\'s-complement integer, 8-byte element size. */
    SF_DTYPE_I64 = 3,
    /** @brief 16-bit half-precision floating point (IEEE 754 binary16), 2-byte element size. */
    SF_DTYPE_F16 = 4,
    /** @brief 8-bit unsigned integer (byte payload / quantized tensor element), 1-byte element size. */
    SF_DTYPE_U8  = 5,
} SF_custominiVm_dtype_t;

/**
 * @typedef sfDType
 * @brief Modern concise alias for SF_custominiVm_dtype_t.
 */
typedef SF_custominiVm_dtype_t sfDType;

/**
 * @enum SF_custominiVm_tensor_flag_t
 * @brief Ingress lifecycle and placement directives transmitted from the host application to SFVM.
 *
 * Bitmask flags governing how SFVM, its allocator, and the generational sfGC treat a tensor.
 * These flags are evaluated upon tensor admission and remain invariant throughout its lifecycle.
 */
typedef enum SF_custominiVm_tensor_flag_t {
    /** @brief Default admission flag; standard garbage-collected lifecycle. */
    SF_TENSOR_FLAG_NONE       = 0x00,

    /**
     * @brief Pinned buffer flag.
     * Indicates that the underlying memory buffer is pinned by an external host environment
     * (such as the Java JVM via JNI/FFM or .NET CoreCLR GC). When set, the sfGC sliding compactor
     * is strictly forbidden from relocating, reallocating, or compacting this tensor.
     */
    SF_TENSOR_FLAG_PINNED     = 0x01,

    /**
     * @brief Immutable buffer flag.
     * Declares the payload memory strictly read-only. Modifying writes are prohibited, enabling
     * zero-copy shared memory reuse and memory-mapped file protection via OS page-table locks.
     */
    SF_TENSOR_FLAG_IMMUTABLE  = 0x02,

    /**
     * @brief Anti-paging DRAM retention flag.
     * Prevents the tensor memory from ever being swapped out to the anonymous disk pagefile.
     * Critical for real-time sensor loops and high-frequency trading where page-fault latency is fatal.
     */
    SF_TENSOR_FLAG_NO_PAGING  = 0x04,

    /**
     * @brief Ephemeral short-lived buffer flag.
     * Hints to the allocator that the tensor is transient and will not survive past the current
     * kernel dispatch step. Ephemeral buffers are allocated preferentially from the nursery Falline.
     */
    SF_TENSOR_FLAG_EPHEMERAL  = 0x08,
} SF_custominiVm_tensor_flag_t;

/**
 * @enum SF_custominiVm_tensor_status_t
 * @brief Egress telemetry and health status flags returned from SFVM to the host application.
 *
 * Provides real-time visibility into the memory state of a tensor following GC sweeps,
 * virtual memory paging actions, or memory integrity verification checks.
 */
typedef enum SF_custominiVm_tensor_status_t {
    /** @brief Tensor memory is contiguous, valid, resident in physical RAM, and healthy. */
    SF_TENSOR_STATUS_OK           = 0x00,

    /**
     * @brief Forwarded pointer status.
     * The tensor was relocated during a generational sliding compaction sweep. The internal
     * data pointer has been automatically resolved through its Brooks forwarding pointer.
     */
    SF_TENSOR_STATUS_FORWARDED    = 0x01,

    /**
     * @brief Soft page-fault resolution status.
     * The tensor was cold and previously swapped out to the anonymous disk pagefile; it has been
     * transparently paged back into physical RAM upon access.
     */
    SF_TENSOR_STATUS_PAGED_IN     = 0x02,

    /**
     * @brief Dependent slice status.
     * The tensor represents a sub-window or slice view of an existing parent tensor, sharing
     * its underlying physical buffer without owning the underlying memory allocation.
     */
    SF_TENSOR_STATUS_VIEW_SLICE   = 0x04,

    /**
     * @brief Quarantined corrupted status.
     * A canary corruption or buffer overrun violation was detected in the boundary guards of this
     * tensor. It has been immediately quarantined by sfGC to prevent systemic memory contamination.
     */
    SF_TENSOR_STATUS_QUARANTINED  = 0x08,
} SF_custominiVm_tensor_status_t;

/* ========================================================================= */
/* TENSOR DESCRIPTOR (CACHE-ALIGNED C ABI COMPLIANT HANDLE)                  */
/* ========================================================================= */

/**
 * @struct SF_custominiVm_tensor_t
 * @brief Core tensor handle representing a multidimensional contiguous array in SFVM.
 *
 * Designed with a strict zero-overhead philosophy, this structure provides a 100% C ABI-compliant
 * memory layout. It fits cleanly into a standard 64-byte processor cacheline, ensuring that
 * accessing tensor metadata incurs zero extraneous memory bus transactions.
 *
 * Dimensions and shapes support up to 8 rank dimensions (compatible with high-order tensor
 * decompositions, batch-sequence-head-dim representations in deep neural networks, and 3D/4D
 * spatial-temporal physical simulations).
 */
typedef struct SF_custominiVm_tensor_t {
    /** @brief Pointer to the raw payload data buffer. Must be 64-byte aligned for AVX-512 operations. */
    void*    data;
    /** @brief Extents along each dimension. Unused dimensions beyond ndim are initialized to 0. */
    int32_t  shape[8];
    /** @brief Number of active dimensions (rank of the tensor). Must be in the range [0, 8]. */
    int32_t  ndim;
    /** @brief Element data type, encoded as an SF_custominiVm_dtype_t value. */
    uint8_t  dtype;
    /** @brief Host-to-VM ingress control directives (bitmask of SF_custominiVm_tensor_flag_t). */
    uint8_t  flags;
    /** @brief VM-to-Host egress telemetry status (bitmask of SF_custominiVm_tensor_status_t). */
    uint8_t  status;
    /** @brief Reserved internal alignment byte. Preserves struct alignment and zero-padding. */
    uint8_t  _reserved;
    /** @brief Total size of the contiguous payload data in bytes (shape[0] * ... * shape[ndim-1] * sizeof(dtype)). */
    size_t   byte_size;
} SF_custominiVm_tensor_t;

/**
 * @typedef sfTensor
 * @brief Modern concise alias for SF_custominiVm_tensor_t.
 */
typedef SF_custominiVm_tensor_t sfTensor;

/* ========================================================================= */
/* TELEMETRY & EVENT LOGGING STRUCTURES (OFF-HEAP CACHELINE-ISOLATED)        */
/* ========================================================================= */

/**
 * @struct SF_custominiVm_hotlog_entry_t
 * @brief High-frequency telemetry log entry written continuously by the sfGC runtime.
 *
 * Exactly 64 bytes in size (occupying precisely one standard CPU cacheline), this structure is
 * written by GC collector threads to off-heap ring buffers without taking any global kernel locks.
 * It provides cycle-accurate insight into generational sweeps, memory footprint shifts, and thread
 * execution topology.
 */
typedef struct SF_custominiVm_hotlog_entry_t {
    /** @brief POSIX wall-clock timestamp measured in nanoseconds since the Unix epoch. */
    uint64_t  ts_ns;
    /** @brief Processor time-stamp counter (RDTSC) reading at the moment of event emission. */
    uint64_t  tsc;
    /** @brief Monotonically increasing sfGC generational sweep sequence number. */
    uint64_t  gc_sweep_no;
    /** @brief Unique numeric identifier of the active kernel or JIT function being executed. */
    uint32_t  fn_id;
    /** @brief Total volume of on-heap memory currently allocated and active, measured in kilobytes. */
    uint32_t  heap_used_kb;
    /** @brief Internal allocator slot or slab index where the event originated. */
    uint16_t  slot;
    /** @brief Logical CPU core index executing the collector thread at timestamp capture. */
    uint16_t  current_core;
    /** @brief Target memory generation (0 = Nursery/Motherline, 1 = Mature/Landfill, 2 = Tank). */
    uint8_t   generation;
    /** @brief Memory region code where the event occurred. */
    uint8_t   region;
    /** @brief Internal event type code (allocation, compaction, evacuation, soft fault). */
    uint8_t   event;
    /** @brief Severity classification (0 = Trace, 1 = Info, 2 = Warning, 3 = Critical Error). */
    uint8_t   severity;
    /** @brief Null-terminated textual narrative describing the event (up to 21 characters + null). */
    char      msg[22];
    /** @brief Atomic visibility flag. Set to 1 when writing is finalized, enabling lockless reading. */
    uint8_t   ready;
    /** @brief Explicit padding byte ensuring the total structure size equals exactly 64 bytes. */
    uint8_t   _pad1;
} SF_custominiVm_hotlog_entry_t;

/**
 * @typedef sfHotlogEntry
 * @brief Modern concise alias for SF_custominiVm_hotlog_entry_t.
 */
typedef SF_custominiVm_hotlog_entry_t sfHotlogEntry;

/**
 * @struct SF_custominiVm_coldlog_entry_t
 * @brief Low-frequency telemetry event log entry for system warnings and administrative alerts.
 *
 * Exactly 32 bytes in size (half of a CPU cacheline), this structure records infrequent, high-value
 * system transitions such as profile transitions, out-of-memory warnings, and hardware bridge errors.
 */
typedef struct SF_custominiVm_coldlog_entry_t {
    /** @brief Nanosecond timestamp of event generation. */
    uint64_t  ts_ns;
    /** @brief Active kernel function identifier, or 0 if generated by the VM supervisor. */
    uint32_t  fn_id;
    /** @brief Categorical event code. */
    uint8_t   event;
    /** @brief Severity level (0 = Info, 1 = Notice, 2 = Warn, 3 = Error, 4 = Fatal). */
    uint8_t   severity;
    /** @brief Atomic readiness flag for lockless single-reader consumption. */
    uint8_t   ready;
    /** @brief Explicit structure padding byte. */
    uint8_t   _pad1;
    /** @brief Null-terminated textual message string (up to 15 characters + null). */
    char      msg[16];
} SF_custominiVm_coldlog_entry_t;

/**
 * @typedef sfColdlogEntry
 * @brief Modern concise alias for SF_custominiVm_coldlog_entry_t.
 */
typedef SF_custominiVm_coldlog_entry_t sfColdlogEntry;

/* ========================================================================= */
/* OPAQUE INTERNAL RUNTIME HEADERS & KERNEL FUNCTION PROTOTYPES              */
/* ========================================================================= */

/**
 * @struct SF_custominiVm_live_header_t
 * @brief Opaque internal object header prepended to live allocations in the Motherline nursery.
 */
typedef struct SF_custominiVm_live_header_t SF_custominiVm_live_header_t;

/**
 * @typedef sfLiveHeader
 * @brief Modern concise alias for SF_custominiVm_live_header_t.
 */
typedef struct SF_custominiVm_live_header_t sfLiveHeader;

/**
 * @struct SF_custominiVm_landfill_header_t
 * @brief Opaque internal object header managing mature objects in the Landfill domain.
 */
typedef struct SF_custominiVm_landfill_header_t SF_custominiVm_landfill_header_t;

/**
 * @typedef sfLandfillHeader
 * @brief Modern concise alias for SF_custominiVm_landfill_header_t.
 */
typedef struct SF_custominiVm_landfill_header_t sfLandfillHeader;

/**
 * @struct SF_custominiVm_emptybox_header_t
 * @brief Opaque internal header tracking reusable empty-box regions within the Falline runway.
 */
typedef struct SF_custominiVm_emptybox_header_t SF_custominiVm_emptybox_header_t;

/**
 * @typedef sfEmptyboxHeader
 * @brief Modern concise alias for SF_custominiVm_emptybox_header_t.
 */
typedef struct SF_custominiVm_emptybox_header_t sfEmptyboxHeader;

/**
 * @typedef SF_custominiVm_kernel_fn_t
 * @brief Standardized function pointer prototype for dispatchable SFVM compute kernels.
 *
 * All mathematical, tensor, and JIT-compiled kernels adhere to this unified dispatch signature,
 * enabling seamless dispatch via SF_custominiVm_call().
 *
 * @param[in]     inputs     Pointer to an array of input tensor handles.
 * @param[in]     n_in       Number of valid input tensors in the inputs array.
 * @param[in,out] outputs    Pointer to an array of output tensor handles to be populated.
 * @param[in]     n_out      Number of expected output tensors in the outputs array.
 * @param[in,out] user_data  Arbitrary user context pointer passed through from dispatch invocation.
 */
typedef void (*SF_custominiVm_kernel_fn_t)(
    const SF_custominiVm_tensor_t* inputs,  int n_in,
    SF_custominiVm_tensor_t*       outputs, int n_out,
    void*                          user_data);

/**
 * @typedef sfKernelFn
 * @brief Modern concise alias for SF_custominiVm_kernel_fn_t.
 */
typedef SF_custominiVm_kernel_fn_t sfKernelFn;

/* ========================================================================= */
/* SFVM PUBLIC MANAGEMENT & INITIALIZATION API                               */
/* ========================================================================= */

/**
 * @brief Initializes the SFVM runtime engine with explicit arena capacities.
 *
 * Allocates and configures the core virtual memory arenas for the VM. This function must be
 * called before any tensors are created or kernels are dispatched.
 *
 * Memory configured via this call is mapped into contiguous virtual address space, with guard
 * pages positioned between the On-Heap Runway and the Off-Heap JIT code arenas.
 *
 * @param[in] code_arena_mb  Size of the off-heap executable JIT code arena in megabytes.
 * @param[in] pressure_mb    Threshold in megabytes that triggers automatic background GC sweeps.
 * @return MSIMD_SUCCESS (0) on success, or an MSIMD negative error status code on failure.
 */
MSIMD_API int                  SF_custominiVm_init(size_t code_arena_mb, size_t pressure_mb);

/**
 * @brief Initializes the SFVM runtime using a declarative configuration flag string.
 *
 * Configures all operational parameters of the VM, the generational sfGC collector, and hardware
 * execution profiles using structured key-value parameters.
 *
 * Parameter format follows the standard syntax: `-#SF|Key:Value`. Multiple parameters are
 * delimited by semicolons or whitespace.
 *
 * <h3>Supported Configuration Parameters:</h3>
 * - <b>`-#SF|Profile:Standard`</b> or <b>`-#SF|Profile:SC`</b>: Selects between the NORMAX standard
 *   high-throughput profile and the SSIOLSC Safety-Critical deterministic profile (ISO 26262 ASIL-D).
 * - <b>`-#SF|GC_Mode:Cycles`</b> or <b>`-#SF|GC_Mode:SIMD`</b>: Selects between standard cycle-counted
 *   GC collection and fully vectorized SIMD parallel sweeping across object header arrays.
 * - <b>`-#SF|Use_Critical_SIMDly_Operations:Allow / Deny / Auto`</b>: Controls whether critical vector
 *   instructions may be utilized during nursery compaction. Note: Cannot be set to `:Deny` if
 *   `GC_Mode:SIMD` is selected (results in `MSIMD_ERROR_FLAG_CONFLICT`).
 * - <b>`-#SF|Snatch_and_Rebate_Mode:Auto / Rapid / Lazy / GYC`</b>: Configures how aggressively unused
 *   Falline empty-box memory is returned to the active nursery runway.
 * - <b>`-#SF|FreeList_Recycle_Policy:LIFO / FIFO`</b>: Cache recycling policy for the Falline free list.
 * - <b>`-#SF|Runway_Handsel_Attempt_Value:1Gb / 512Mb`</b>: Initial virtual memory commitment for Runway.
 * - <b>`-#SF|Landfill_Handsel_Attempt_Value:285Mb`</b>: Initial virtual memory commitment for Landfill.
 * - <b>`-#SF|Wakes_GC_Cycling_Interval:1Ms / 280Ms`</b>: Periodic interval for background GC wakeups.
 * - <b>`-#SF|Line_Compaction_Freq:350Ms`</b>: Frequency of sliding generational compaction passes.
 * - <b>`-#SF|In_Heap_Forwarding_Rate:7`</b>: In-heap Brooks pointer forwarding resolution rate.
 * - <b>`-#SF|Pressure_Threshold:256Mb`</b>: Heap usage pressure limit triggering urgent GC sweeps.
 * - <b>`-#SF|AlwaysPreTouch:1 / 0`</b>: When enabled (1), pages are committed immediately to eliminate
 *   runtime page faults during deterministic execution phases.
 * - <b>`-#SF|GC_Heuristic:Adaptive / Aggressive / Bureaucratic`</b>: Automated collector tuning model.
 * - <b>`-#SF|Always_Focus_Compactizing:1 / 0`</b>: Forces full compaction on every sweep pass.
 * - <b>`-#SF|Forbid_Runtime_Heap:1 / 0`</b>: Strictly locks heap size post-initialization (mandatory in SC).
 * - <b>`-#SF|Max_Bounded_Iterations:1000000`</b>: Worst-Case Execution Time (WCET) iteration ceiling.
 *
 * @param[in] flags_str Null-terminated ASCII string containing declarative configuration flags.
 * @return MSIMD_SUCCESS (0) on success, or an MSIMD negative error status code on invalid flags.
 */
MSIMD_API int                  SF_custominiVm_init_with_flags(const char* flags_str);

/**
 * @brief Retrieves the active operational profile of the SFVM engine.
 * @return MSIMD_PROFILE_STANDARD (NORMAX) or MSIMD_PROFILE_SAFETY_CRITICAL (SSIOLSC).
 */
MSIMD_API msimd_profile_mode_t SF_custominiVm_get_profile(void);

/**
 * @brief Queries whether the engine is currently executing under the Safety-Critical (SSIOLSC) profile.
 * @return 1 if executing in Safety-Critical mode, 0 if executing in standard NORMAX mode.
 */
MSIMD_API int                  SF_custominiVm_is_safety_critical(void);

/**
 * @brief Shuts down the SFVM runtime engine and releases all allocated memory arenas.
 *
 * Flushes all pending telemetry logs, unmaps all virtual memory arenas, terminates background
 * collector worker threads, and releases hardware MMIO device bindings.
 */
MSIMD_API void                 SF_custominiVm_shutdown(void);

/* ========================================================================= */
/* TENSOR LIFECYCLE & SLAB ALLOCATOR API                                     */
/* ========================================================================= */

/**
 * @brief Wraps an existing, externally managed memory buffer in an SFVM tensor handle.
 *
 * Performs zero-copy encapsulation of external memory (such as buffers allocated by host
 * runtimes, OpenGL/Vulkan mapped buffers, or static arrays).
 *
 * @param[in] data   Pointer to the existing raw data buffer.
 * @param[in] shape  Pointer to an array of dimension extents (length ndim).
 * @param[in] ndim   Number of dimensions (rank). Must be between 1 and 8.
 * @param[in] dtype  Element data type, encoded as an SF_custominiVm_dtype_t value.
 * @return Initialized SF_custominiVm_tensor_t handle wrapping the provided buffer.
 */
MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_tensor_from(void* data, const int32_t* shape,
                                                             int ndim, int dtype);

/**
 * @brief Allocates a new contiguous tensor from the active SFVM Runway nursery heap.
 *
 * Allocates sufficient memory for the specified multidimensional shape and element type.
 * Memory is guaranteed to be 64-byte aligned, satisfying AVX-512 and ARM SVE vector constraints.
 *
 * @param[in] shape  Array of dimension extents defining the tensor geometry.
 * @param[in] ndim   Rank of the tensor (1 to 8).
 * @param[in] dtype  Element data type (SF_custominiVm_dtype_t).
 * @return Allocated and initialized SF_custominiVm_tensor_t handle.
 */
MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_tensor_alloc(const int32_t* shape, int ndim, int dtype);

/**
 * @brief Explicitly deallocates a tensor and relinquishes its memory to the Falline runway.
 *
 * Reclaims the tensor payload buffer immediately, placing the region into the Falline
 * empty-box list for instant reuse. If the tensor is a sub-slice, only view metadata is destroyed.
 *
 * @param[in,out] tensor Pointer to the tensor handle to be freed. The handle is zeroed upon return.
 */
MSIMD_API void                    SF_custominiVm_tensor_free(SF_custominiVm_tensor_t* tensor);

/**
 * @brief Allocates an arbitrary raw memory block from the SFVM heap with explicit byte alignment.
 *
 * @param[in] bytes      Size of the requested memory allocation in bytes.
 * @param[in] alignment  Requested memory boundary alignment in bytes (e.g. 16, 32, 64, or 128).
 * @return Pointer to the allocated memory block, or NULL if allocation fails.
 */
MSIMD_API void*                   SF_custominiVm_heap_alloc(size_t bytes, size_t alignment);

/**
 * @brief Frees a previously allocated raw memory block.
 *
 * @param[in] ptr   Pointer to the memory block returned by SF_custominiVm_heap_alloc().
 * @param[in] bytes Exact byte size of the allocation specified during allocation.
 */
MSIMD_API void                    SF_custominiVm_heap_free(void* ptr, size_t bytes);

/**
 * @brief Returns the total volume of currently active on-heap memory in bytes.
 * @return Total allocated memory in bytes across Runway, Tank, and Landfill arenas.
 */
MSIMD_API size_t                  SF_custominiVm_heap_allocated_bytes(void);

/* ========================================================================= */
/* KERNEL REGISTRATION, SYMBOL BINDING & DISPATCH API                        */
/* ========================================================================= */

/**
 * @brief Registers a native compute kernel function with the SFVM dispatch table.
 *
 * @param[in] fn_id Unique integer function identifier assigned to the kernel.
 * @param[in] fn    Pointer to the kernel implementation function adhering to SF_custominiVm_kernel_fn_t.
 */
MSIMD_API void SF_custominiVm_register(uint32_t fn_id, SF_custominiVm_kernel_fn_t fn);

/**
 * @brief Binds a symbolic name and sample input signature to a registered kernel identifier.
 *
 * Establishes symbolic resolution bindings, enabling JIT compilers and runtime loaders to
 * resolve the kernel by name and perform ahead-of-time type and rank verification.
 *
 * @param[in] name           Null-terminated symbolic name of the kernel.
 * @param[in] fn_id          Target numeric kernel function identifier.
 * @param[in] example_input  Array of example input tensors used to deduce shape and dtype signatures.
 * @param[in] n_inputs       Number of tensors in the example_input array.
 * @return MSIMD_SUCCESS (0) on success, or an error status code on signature collision.
 */
MSIMD_API int  SF_custominiVm_bind(const char* name, uint32_t fn_id,
                                   const SF_custominiVm_tensor_t* example_input, int n_inputs);

/**
 * @brief Executes a registered or JIT-compiled kernel with specified input and output tensors.
 *
 * Performs high-speed direct dispatch with zero intermediary memory copying. In NORMAX mode,
 * execution checks safepoints for concurrent GC coordination. In SSIOLSC mode, execution occurs
 * with guaranteed deterministic latency.
 *
 * @param[in]     fn_id      Registered numeric identifier of the kernel to invoke.
 * @param[in]     inputs     Array of input tensors supplied to the kernel.
 * @param[in]     n_in       Count of input tensors.
 * @param[in,out] outputs    Array of output tensors populated by the kernel.
 * @param[in]     n_out      Count of output tensors.
 * @param[in,out] user_data  Arbitrary user context pointer forwarded to the kernel function.
 * @return MSIMD_SUCCESS (0) on success, or an MSIMD negative error status code on failure.
 */
MSIMD_API int  SF_custominiVm_call(uint32_t fn_id,
                                   const SF_custominiVm_tensor_t* inputs,  int n_in,
                                   SF_custominiVm_tensor_t*       outputs, int n_out,
                                   void*                          user_data);

/* ========================================================================= */
/* LOCKLESS TELEMETRY LOG DRAINING API                                       */
/* ========================================================================= */

/**
 * @brief Drains all unread entries from the high-frequency off-heap HotLog ring buffer.
 *
 * Iterates through all newly written 64-byte HotLog telemetry records, passing each record
 * to the provided callback function. This operation is thread-safe and lockless.
 *
 * @param[in] cb   Callback function invoked for each HotLog entry.
 * @param[in] user Arbitrary user context pointer passed to the callback.
 */
MSIMD_API void SF_custominiVm_hotlog_drain(
                void (*cb)(const SF_custominiVm_hotlog_entry_t*, void*), void* user);

/**
 * @brief Drains all unread entries from the low-frequency ColdLog event buffer.
 *
 * @param[in] cb   Callback function invoked for each ColdLog entry.
 * @param[in] user Arbitrary user context pointer passed to the callback.
 */
MSIMD_API void SF_custominiVm_coldlog_drain(
                void (*cb)(const SF_custominiVm_coldlog_entry_t*, void*), void* user);

/* ========================================================================= */
/* GENERATIONAL sfGC CONTROL & OBJECT ADMISSION API                          */
/* ========================================================================= */

/**
 * @brief Manually admits an externally created live object header into the sfGC nursery tracking chain.
 * @param[in,out] obj Pointer to the live header to admit.
 */
MSIMD_API void SF_custominiVm_gc_admit(SF_custominiVm_live_header_t* obj);

/**
 * @brief Updates the generational touch metrics and core residency of an active live header.
 *
 * Records execution cycles and processor affinity, updating the generational heat profile
 * used by the adaptive collector to schedule sliding compaction passes.
 *
 * @param[in,out] obj      Pointer to the live object header.
 * @param[in]     cycles   Elapsed processor cycle count spent operating on the object.
 * @param[in]     core_id  Logical CPU core identifier where execution took place.
 */
MSIMD_API void SF_custominiVm_gc_touch(SF_custominiVm_live_header_t* obj,
                                       uint64_t cycles, uint16_t core_id);

/**
 * @brief Registers an administrative hook callback invoked whenever an object header is destroyed by sfGC.
 *
 * @param[in] cb   Callback function receiving the deallocated object header pointer.
 * @param[in] user Arbitrary user context pointer passed to the callback.
 */
MSIMD_API void SF_custominiVm_gc_set_on_free(
                void (*cb)(SF_custominiVm_live_header_t*, void*), void* user);

/**
 * @brief Triggers an explicit, synchronous garbage collection sweep across all on-heap generations.
 *
 * Sweeps the Motherline, evacuates dead objects to the Landfill, compacts fragmented free space,
 * and updates system telemetry.
 */
MSIMD_API void SF_custominiVm_gc_collect(void);

/**
 * @brief Queries the total committed heap memory in bytes across all active arenas.
 * @return Total committed memory volume in bytes.
 */
MSIMD_API size_t   SF_custominiVm_heap_bytes(void);

/**
 * @brief Returns the lifetime cumulative count of completed GC sweep cycles.
 * @return Monotonically increasing sweep counter.
 */
MSIMD_API uint64_t SF_custominiVm_gc_sweep_count(void);

/**
 * @brief Returns the total volume of memory currently consumed within the Landfill pit in bytes.
 * @return Consumed Landfill memory in bytes.
 */
MSIMD_API size_t   SF_custominiVm_landfill_pit_used(void);

/**
 * @brief Returns the number of active segmented memory panes currently allocated in the Landfill.
 * @return Number of active Landfill panes.
 */
MSIMD_API uint32_t SF_custominiVm_landfill_pane_count(void);

/**
 * @brief Returns the cumulative volume of memory rebated from Falline back to active runway in bytes.
 * @return Cumulative rebated bytes.
 */
MSIMD_API size_t   SF_custominiVm_heap_rebated_bytes(void);

/**
 * @brief Returns the total count of rebate cycles performed by the runway allocator.
 * @return Cumulative rebate cycle count.
 */
MSIMD_API uint32_t SF_custominiVm_heap_rebate_cycles(void);

/* ========================================================================= */
/* JIT BITCODE LOADING, SYMBOL RESOLUTION & SANDBOX TESTING                  */
/* ========================================================================= */

/**
 * @brief Loads and decompresses a compressed bitcode blob into the off-heap JIT code arena.
 *
 * Supports zstandard-compressed and raw bitcode blobs. Decompresses the bitcode into write-enabled
 * off-heap code pages, resolves internal relocation tables, and seals the memory with
 * PAGE_EXECUTE_READ (W^X) permissions prior to making the functions callable.
 *
 * @param[in] bitcode_blob Pointer to the compressed bitcode binary data in memory.
 * @param[in] size         Size of the bitcode blob in bytes.
 * @param[in] fn_id_base   Starting function identifier offset for kernels declared in the blob.
 * @return MSIMD_SUCCESS (0) on success, or an error status code on malformed bitcode.
 */
MSIMD_API int SF_custominiVm_jit_load(const uint8_t* bitcode_blob, size_t size,
                                      uint32_t fn_id_base);

/**
 * @brief Resolves an exported symbol by name within the JIT code arena and binds it to a function ID.
 *
 * @param[in] symbol Null-terminated ASCII name of the exported function.
 * @param[in] fn_id  Target numeric function identifier to assign to the resolved symbol.
 * @return MSIMD_SUCCESS (0) on success, or an error status code if the symbol is not found.
 */
MSIMD_API int SF_custominiVm_jit_resolve(const char* symbol, uint32_t fn_id);

/**
 * @brief Executes a candidate kernel in an isolated sandbox to verify numerical correctness and stability.
 *
 * Compares output results of the candidate kernel against the currently active baseline kernel,
 * verifying that output errors remain within acceptable machine precision bounds prior to hotswapping.
 *
 * @param[in] fn_id     Numeric function identifier of the active baseline kernel.
 * @param[in] candidate Function pointer to the new candidate kernel implementation.
 * @param[in] name      Null-terminated descriptive name of the candidate kernel.
 */
MSIMD_API void SF_custominiVm_sandbox_test(uint32_t fn_id, SF_custominiVm_kernel_fn_t candidate,
                                           const char* name);

/* ========================================================================= */
/* INDEPENDENT PUZZLE ARENA & DYNAMIC ISA CANDIDATE EVALUATION               */
/* ========================================================================= */

/**
 * @struct SF_custominiVm_isa_candidate_t
 * @brief Descriptor for a competing architecture-specific vector kernel candidate.
 */
typedef struct SF_custominiVm_isa_candidate_t {
    /** @brief Function pointer to the candidate kernel. */
    SF_custominiVm_kernel_fn_t fn;
    /** @brief Null-terminated target ISA tag string (e.g. "AVX512", "AVX2", "SVE2", "RVV", "AUTO"). */
    const char*                isa_tag;
    /** @brief Null-terminated descriptive variant name (e.g. "avx512_unrolled4"). */
    const char*                variant_name;
} SF_custominiVm_isa_candidate_t;

/**
 * @typedef sfIsaCandidate
 * @brief Modern concise alias for SF_custominiVm_isa_candidate_t.
 */
typedef SF_custominiVm_isa_candidate_t sfIsaCandidate;

/**
 * @enum SF_custominiVm_puzzle_exec_mode_t
 * @brief Execution strategies for benchmarking candidate puzzle kernels in the sandbox arena.
 */
typedef enum SF_custominiVm_puzzle_exec_mode_t {
    /** @brief Sequential mode: executes candidates one-by-one on an isolated core for pristine cache profiling. */
    SF_PUZZLE_EXEC_SEQUENTIAL = 0,
    /** @brief Parallel mode: executes competing candidates concurrently on idle cores for rapid tournament selection. */
    SF_PUZZLE_EXEC_PARALLEL   = 1,
    /** @brief Adaptive mode: dynamically selects parallel execution if cores are idle, or sequential if under load. */
    SF_PUZZLE_EXEC_ADAPTIVE   = 2,
} SF_custominiVm_puzzle_exec_mode_t;

/**
 * @typedef sfPuzzleExecMode
 * @brief Modern concise alias for SF_custominiVm_puzzle_exec_mode_t.
 */
typedef SF_custominiVm_puzzle_exec_mode_t sfPuzzleExecMode;

/**
 * @brief Sets the execution strategy for the Independent Puzzle Arena engine.
 * @param[in] mode Target puzzle execution strategy (Sequential, Parallel, or Adaptive).
 */
MSIMD_API void                        SF_custominiVm_puzzle_set_mode(SF_custominiVm_puzzle_exec_mode_t mode);

/**
 * @brief Queries the currently configured execution strategy of the Puzzle Arena.
 * @return Currently active puzzle execution mode.
 */
MSIMD_API SF_custominiVm_puzzle_exec_mode_t SF_custominiVm_puzzle_get_mode(void);

/**
 * @brief Returns the number of dynamically chained slab chunks currently active in the heap.
 * @return Count of active chained slab chunks.
 */
MSIMD_API size_t                      SF_custominiVm_heap_chained_chunks(void);

/**
 * @brief Resolves the true physical payload address of a tensor handle through Brooks forwarding pointers.
 *
 * If the tensor has been relocated during an in-flight sliding compaction pass, resolves the
 * current valid memory pointer via atomic forwarding chain traversal.
 *
 * @param[in] tensor Pointer to the tensor handle to resolve.
 * @return Direct pointer to resident memory.
 */
MSIMD_API void*                       SF_custominiVm_tensor_resolve_ptr(const SF_custominiVm_tensor_t* tensor);

/* ========================================================================= */
/* UNIFIED MEMORY ANATOMY & SYSTEM METRICS SPECIFICATION                     */
/* ========================================================================= */

/**
 * @struct SF_custominiVm_anatomy_stat_t
 * @brief Comprehensive diagnostic telemetry structure detailing memory distribution across all arenas.
 *
 * Provides a unified, complete view of memory allocation, commitment, and fragmentation across
 * On-Heap Runway (Motherline & Falline), Tank regions, Puzzle Arena, Landfill, Off-Heap JIT code
 * heaps, and swap pagefile utilization.
 */
typedef struct SF_custominiVm_anatomy_stat_t {
    /* 1. On-Heap (Garbage-Collected Unified Memory) */
    /** @brief Total virtual memory reserved for on-heap operations in bytes. */
    size_t onheap_reserved_bytes;
    /** @brief Physical RAM pages currently committed to on-heap arenas in bytes. */
    size_t onheap_committed_bytes;
    /** @brief Total active tensor payload memory currently allocated in bytes. */
    size_t onheap_allocated_bytes;

    /* 1.1 Runway Nursery Domain */
    /** @brief Total virtual capacity of the Runway nursery domain in bytes. */
    size_t runway_total_bytes;
    /** @brief Memory currently utilized by live objects in the Runway in bytes. */
    size_t runway_used_bytes;
    /** @brief Volume of live objects currently resident in the Motherline nursery in bytes. */
    size_t motherline_live_bytes;
    /** @brief Volume of empty, reusable memory boxes resident in the Falline in bytes. */
    size_t falline_emptybox_bytes;
    /** @brief Count of discrete empty boxes currently linked in the Falline free list. */
    size_t emptybox_count;

    /* 1.2 Tank Regions (Humongous Tensors) */
    /** @brief Total memory committed to humongous Tank region allocations in bytes. */
    size_t tank_regions_bytes;
    /** @brief Number of discrete humongous Tank regions currently active. */
    size_t tank_regions_count;

    /* 1.3 Independent Puzzle Arena */
    /** @brief Total capacity assigned to the Independent Puzzle Arena sandbox in bytes. */
    size_t puzzle_arena_total_bytes;
    /** @brief Memory currently consumed by competing candidate benchmarks in bytes. */
    size_t puzzle_arena_used_bytes;

    /* 1.4 Landfill Domain (Mature Objects & Evacuation) */
    /** @brief Total capacity of the Landfill arena in bytes. */
    size_t landfill_total_bytes;
    /** @brief Consumed memory within the Landfill arena in bytes. */
    size_t landfill_used_bytes;
    /** @brief Volume of matured, long-lived tensors surviving multiple generations in bytes. */
    size_t matured_arena_bytes;
    /** @brief Volume of memory undergoing rehabilitation prior to reclamation in bytes. */
    size_t rehab_arena_bytes;
    /** @brief Count of active segmented memory panes in the Landfill. */
    uint32_t landfill_pane_count;

    /* 2. Off-Heap Domain (Non-GC Executable & Telemetry) */
    /** @brief Total capacity of the executable off-heap JIT code arena in bytes. */
    size_t offheap_code_arena_bytes;
    /** @brief Total memory consumed by HotLog and ColdLog telemetry ring buffers in bytes. */
    size_t offheap_telemetry_bytes;

    /* 3. Dynamic Slab Expansion & Rebating */
    /** @brief Count of dynamically chained slab chunks allocated to meet allocation spikes. */
    size_t chained_chunk_count;
    /** @brief Volume of memory successfully returned from Falline to Motherline in bytes. */
    size_t rebated_bytes;

    /* 4. Allocation Pacing & Virtual Memory Pagefile Paging */
    /** @brief Total count of allocation pauses (pacing stalls) enforced to prevent heap exhaustion. */
    size_t pacing_stall_count;
    /** @brief Total volume of cold tensor memory currently swapped to disk pagefile in bytes. */
    size_t paged_out_bytes;
    /** @brief Number of discrete tensor buffers currently resident in the pagefile. */
    size_t paged_out_count;
} SF_custominiVm_anatomy_stat_t;

/**
 * @typedef sfAnatomyStat
 * @brief Modern concise alias for SF_custominiVm_anatomy_stat_t.
 */
typedef struct SF_custominiVm_anatomy_stat_t sfAnatomyStat;

/**
 * @brief Queries the detailed memory anatomy of the SFVM runtime engine.
 * @param[out] out_stat Pointer to an anatomy structure populated with current memory telemetry.
 */
MSIMD_API void        SF_custominiVm_anatomy_query(SF_custominiVm_anatomy_stat_t* out_stat);

/**
 * @brief Generates a formatted ASCII diagnostic summary report describing system memory anatomy.
 * @return Pointer to a static, thread-local null-terminated string containing the formatted report.
 */
MSIMD_API const char* SF_custominiVm_anatomy_description(void);

/* ========================================================================= */
/* ADVANCED GC EXECUTION MODES, OWP & MTP CONTROL SPECIFICATION              */
/* ========================================================================= */

/**
 * @enum SF_custominiVm_gc_run_mode_t
 * @brief Operational stepping modes governing execution of the sfGC engine.
 */
typedef enum SF_custominiVm_gc_run_mode_t {
    /** @brief Single Step: Executes exactly one incremental cycle and returns immediately. */
    SF_GC_RUN_SINGLE_STEP     = 0,
    /** @brief CONTINUNTIL.FINISH: Executes continuously until all pending dead objects are fully reclaimed. */
    SF_GC_RUN_CONTINUE_FINISH = 1,
    /** @brief STEPPING_MTP: Stepping mode evaluating a user-supplied Manual Termination Point (MTP) predicate. */
    SF_GC_RUN_STEPPING_MTP    = 2,
} SF_custominiVm_gc_run_mode_t;

/**
 * @typedef sfGcRunMode
 * @brief Modern concise alias for SF_custominiVm_gc_run_mode_t.
 */
typedef SF_custominiVm_gc_run_mode_t sfGcRunMode;

/**
 * @struct SF_custominiVm_env_change_t
 * @brief Environmental telemetry delta transmitted to user MTP predicate functions during GC stepping.
 */
typedef struct SF_custominiVm_env_change_t {
    /** @brief Boolean indicator: 1 if environmental changes occurred during the step, 0 otherwise. */
    int      changed;
    /** @brief Net delta of new bytes allocated since the previous stepping iteration. */
    uint32_t allocations_detected;
    /** @brief Number of allocation pacing stalls triggered since the previous step. */
    uint32_t pacing_stalls_delta;
    /** @brief Number of kernel calls dispatched since the previous step. */
    uint32_t calls_delta;
    /** @brief Number of tensor resurrection requests processed since the previous step. */
    uint32_t resurrections_delta;
    /** @brief Net change in bytes swapped to or from the disk pagefile. */
    uint32_t paged_out_delta;
} SF_custominiVm_env_change_t;

/**
 * @typedef sfEnvChange
 * @brief Modern concise alias for SF_custominiVm_env_change_t.
 */
typedef SF_custominiVm_env_change_t sfEnvChange;

/**
 * @typedef SF_custominiVm_mtp_predicate_fn
 * @brief User-defined Manual Termination Point (MTP) predicate function evaluated during GC stepping.
 *
 * @param[in] stat      Snapshot of current system memory anatomy.
 * @param[in] env       Environmental changes observed during the most recent incremental step.
 * @param[in] user_data User context pointer passed to SF_custominiVm_gc_run().
 * @return 1 to terminate the GC run loop, 0 to permit continued stepping.
 */
typedef int (*SF_custominiVm_mtp_predicate_fn)(const SF_custominiVm_anatomy_stat_t* stat,
                                               const SF_custominiVm_env_change_t* env,
                                               void* user_data);

/**
 * @brief Executes an advanced garbage collection loop under the specified stepping mode and constraints.
 *
 * @param[in] mode      Execution stepping mode (Single Step, Continue to Finish, or Stepping with MTP).
 * @param[in] owp       One-Way Pacing flag: if non-zero, applies strict backpressure pacing during execution.
 * @param[in] mtp_pred  User termination predicate function evaluated when mode is SF_GC_RUN_STEPPING_MTP.
 * @param[in] user_data Arbitrary user context pointer passed directly to mtp_pred.
 * @return Total volume of memory reclaimed during the execution run in bytes.
 */
MSIMD_API size_t SF_custominiVm_gc_run(SF_custominiVm_gc_run_mode_t mode,
                                       int owp,
                                       SF_custominiVm_mtp_predicate_fn mtp_pred,
                                       void* user_data);

/* ========================================================================= */
/* DETERMINISTIC CONSCIENTIOUS CARETAKER MANIFEST ("SHAKE THE BED & SLEEP")   */
/* ========================================================================= */

/**
 * @struct SF_custominiVm_gc_manifest_t
 * @brief Formal audit manifest generated upon completion of a thorough, deep cleaning GC cycle.
 *
 * Provides complete verification telemetry detailing every memory cell inspected, swept,
 * and evacuated, verifying that all trailing debris was cleared and the heap has entered
 * a fully quiescent, predictable state.
 */
typedef struct SF_custominiVm_gc_manifest_t {
    /** @brief Unique sequential sweep identification number. */
    uint64_t sweep_id;
    /** @brief Number of generational memory rooms inspected during the pass. */
    uint32_t rooms_inspected;
    /** @brief Total count of individual allocation cells examined and swept. */
    size_t   cells_swept;
    /** @brief Count of dead object bodies drained and evacuated to the Landfill domain. */
    size_t   cells_drained;
    /** @brief Net volume of memory reclaimed and returned to active runways in bytes. */
    size_t   bytes_reclaimed;
    /** @brief Total volume of live heap memory remaining active after the sweep in bytes. */
    size_t   heap_bytes_after;
    /** @brief Wall-clock duration of the sweeping cycle measured in nanoseconds. */
    uint64_t elapsed_ns;
    /** @brief Boolean flag: 1 if all trailing debris and edge allocations were cleared, 0 otherwise. */
    int      trailing_debris_cleared;
    /** @brief Boolean flag: 1 if the heap is completely quiescent and collector threads are sleeping. */
    int      quiescent;
    /** @brief Human-readable narrative summary detailing sweep outcomes (up to 159 chars + null). */
    char     narrative[160];
} SF_custominiVm_gc_manifest_t;

/**
 * @typedef sfGcManifest
 * @brief Modern concise alias for SF_custominiVm_gc_manifest_t.
 */
typedef SF_custominiVm_gc_manifest_t sfGcManifest;

/**
 * @brief Executes a comprehensive, deterministic deep-clean sweep ("Shake the Bed and Sleep").
 *
 * Thoroughly inspects all generational rooms (Nursery, Mature, Landfill), drains dead objects,
 * eliminates trailing allocation debris, compacts remaining live objects, and produces a complete
 * audit manifest before placing the collector into a quiescent sleep state.
 *
 * @return Completed SF_custominiVm_gc_manifest_t audit record.
 */
MSIMD_API SF_custominiVm_gc_manifest_t SF_custominiVm_gc_shake_bed(void);

/**
 * @brief Retrieves the audit manifest generated by the most recently executed deep-clean sweep.
 * @param[out] out_manifest Pointer to a manifest structure populated with the latest sweep audit.
 */
MSIMD_API void                         SF_custominiVm_gc_get_last_manifest(SF_custominiVm_gc_manifest_t* out_manifest);

/* ========================================================================= */
/* AUTOMATIC GC HEURISTICS & EXECUTION ARCHITECTURE PROFILES                 */
/* ========================================================================= */

/**
 * @enum SF_custominiVm_gc_heuristic_t
 * @brief Autonomous behavioral heuristics governing background sfGC collector scheduling.
 */
typedef enum SF_custominiVm_gc_heuristic_t {
    /** @brief Adaptive: Dynamically adjusts collection wakeups and compaction based on allocation velocity. */
    SF_GC_HEURISTIC_ADAPTIVE     = 0,
    /** @brief Aggressive: Enforces tight memory footprint with minimal sleep intervals and frequent compaction. */
    SF_GC_HEURISTIC_AGGRESSIVE   = 1,
    /** @brief Bureaucratic: Uncompromising hygiene. Strict STW, exhaustive canary validation, zero tolerance. */
    SF_GC_HEURISTIC_BUREAUCRATIC = 2,
    /** @brief Disabled (No-sfGC): Fully disables garbage collection for direct raw disk-sector overwrites. */
    SF_GC_HEURISTIC_DISABLED     = 3,
} SF_custominiVm_gc_heuristic_t;

/**
 * @typedef sfGcHeuristic
 * @brief Modern concise alias for SF_custominiVm_gc_heuristic_t.
 */
typedef SF_custominiVm_gc_heuristic_t sfGcHeuristic;

/**
 * @enum SF_custominiVm_gc_execution_mode_t
 * @brief Execution architecture tiers defining memory compaction and collector presence.
 */
typedef enum SF_custominiVm_gc_execution_mode_t {
    /** @brief NORMAX / Standard: Multi-generational dynamic Stop-The-World with concurrent heuristics. */
    SF_GC_EXECUTION_DYNAMIC_STW  = 0,
    /** @brief SSIOLSC Tier 1: Phase-STW with mandatory sliding compaction executed strictly between phases. */
    SF_GC_EXECUTION_PHASE_STW    = 1,
    /** @brief SSIOLSC Tier 2: No-sfGC direct sector overwrite mode (Zero-GC, ultra-low latency, 50 kHz loops). */
    SF_GC_EXECUTION_NO_SFGC      = 2,
} SF_custominiVm_gc_execution_mode_t;

/**
 * @brief Configures the active autonomous garbage collection heuristic model.
 * @param[in] heuristic Selected heuristic policy (Adaptive, Aggressive, Bureaucratic, or Disabled).
 */
MSIMD_API void                          SF_custominiVm_gc_set_heuristic(SF_custominiVm_gc_heuristic_t heuristic);

/**
 * @brief Queries the currently active autonomous garbage collection heuristic model.
 * @return Active heuristic enumeration constant.
 */
MSIMD_API SF_custominiVm_gc_heuristic_t SF_custominiVm_gc_get_heuristic(void);

/**
 * @brief Enables or disables the No-sfGC (Zero-GC) execution mode.
 * @param[in] enabled Set to 1 to disable all GC operations (Zero-GC), or 0 to restore active GC.
 */
MSIMD_API void                          SF_custominiVm_gc_set_no_sfgc(int enabled);

/**
 * @brief Queries whether the engine is currently operating in No-sfGC (Zero-GC) mode.
 * @return 1 if No-sfGC mode is active, 0 if standard GC collection is operating.
 */
MSIMD_API int                           SF_custominiVm_gc_is_no_sfgc(void);

/* ========================================================================= */
/* SCGC SECONDARY TYPING & RAW PRIMITIVA SECTOR I/O API                      */
/* ========================================================================= */

/**
 * @enum SF_custominiVm_scgc_type_t
 * @brief Secondary operational modes for the Safety-Critical No-sfGC execution profile.
 *
 * Governs memory access semantics under the SSIOLSC Safety-Critical profile:
 * - <b>DEFAULT:</b> Phase-STW Bureaucratic GC. The collector remains active; strict Stop-The-World
 *   sweeps and mandatory compaction passes occur exclusively at deterministic phase boundaries.
 *   Standard sfLiveHeader metadata chains remain valid.
 * - <b>PRIMITIVE_RW:</b> Pure Zero-GC / Raw Sector mode. Garbage collection is completely deactivated.
 *   Object headers are bypassed entirely. Allocations yield raw bump-pointer sectors accessed
 *   via SF_primitiva_write() and SF_primitiva_read(), offering zero runtime overhead.
 */
typedef enum SF_custominiVm_scgc_type_t {
    /** @brief Phase-STW Bureaucratic GC (SSIOLSC Tier 1: Collector active at phase boundaries). */
    SF_SCGC_TYPE_DEFAULT      = 0,
    /** @brief Pure Zero-GC / Raw Sector Mode (SSIOLSC Tier 2: Headers bypassed, direct sector writes). */
    SF_SCGC_TYPE_PRIMITIVE_RW = 1,
} SF_custominiVm_scgc_type_t;

/**
 * @typedef sfScgcType
 * @brief Modern concise alias for SF_custominiVm_scgc_type_t.
 */
typedef SF_custominiVm_scgc_type_t sfScgcType;

/**
 * @brief Sets the secondary operational type for Safety-Critical memory management.
 * @param[in] type Selected SCGC mode (SF_SCGC_TYPE_DEFAULT or SF_SCGC_TYPE_PRIMITIVE_RW).
 */
MSIMD_API void              SF_custominiVm_set_scgc_type(SF_custominiVm_scgc_type_t type);

/**
 * @brief Queries the currently configured SCGC secondary operational type.
 * @return Currently active SCGC operational type.
 */
MSIMD_API SF_custominiVm_scgc_type_t SF_custominiVm_get_scgc_type(void);

/**
 * @brief Allocates a raw unmanaged memory sector bypassing all object headers (PrimitiveRW mode).
 *
 * @param[in] bytes Requested memory sector size in bytes.
 * @param[in] align Memory boundary alignment in bytes (typically 64 for SIMD alignment).
 * @return Direct pointer to the raw allocated sector memory.
 */
MSIMD_API void* SF_primitiva_alloc(size_t bytes, size_t align);

/**
 * @brief Writes data directly into an unmanaged memory sector without triggering GC write barriers.
 *
 * @param[out] sector_ptr Destination sector pointer returned by SF_primitiva_alloc().
 * @param[in]  src        Source data buffer.
 * @param[in]  bytes      Number of bytes to copy.
 */
MSIMD_API void  SF_primitiva_write(void* sector_ptr, const void* src, size_t bytes);

/**
 * @brief Reads data directly from an unmanaged memory sector without triggering GC read barriers.
 *
 * @param[in]  sector_ptr Source sector pointer returned by SF_primitiva_alloc().
 * @param[out] dst        Destination buffer receiving the data.
 * @param[in]  bytes      Number of bytes to read.
 */
MSIMD_API void  SF_primitiva_read (const void* sector_ptr, void* dst, size_t bytes);

/**
 * @brief Overwrites an existing raw memory sector in-place using streaming non-temporal stores.
 *
 * @param[out] sector_ptr Destination sector pointer.
 * @param[in]  src        Source data to write.
 * @param[in]  bytes      Number of bytes to overwrite.
 */
MSIMD_API void  SF_primitiva_overwrite(void* sector_ptr, const void* src, size_t bytes);

/**
 * @brief Clears an unmanaged memory sector to zero using vectorized non-temporal stores.
 *
 * @param[out] sector_ptr Destination sector pointer to zero.
 * @param[in]  bytes      Number of bytes to clear.
 */
MSIMD_API void  SF_primitiva_zero(void* sector_ptr, size_t bytes);

/* ========================================================================= */
/* PUZZLE SUBMISSION & ISA CANDIDATE TOURNAMENT API                          */
/* ========================================================================= */

/**
 * @brief Submits a single candidate kernel to the Independent Puzzle Arena for automated benchmarking.
 *
 * @param[in] fn_id         Numeric identifier of the active baseline kernel.
 * @param[in] candidate     Function pointer to the competing candidate kernel implementation.
 * @param[in] name          Descriptive name string for the candidate.
 * @param[in] target_isa    Target architecture tag string (e.g. "AVX512", "AVX2", "NEON").
 * @param[in] target_epsilon Permissible numerical error tolerance between baseline and candidate outputs.
 * @param[in] target_tensor Optional template tensor defining expected geometry and data types.
 */
MSIMD_API void        SF_custominiVm_puzzle_submit(uint32_t fn_id, SF_custominiVm_kernel_fn_t candidate, const char* name,
                                                   const char* target_isa, float target_epsilon,
                                                   const SF_custominiVm_tensor_t* target_tensor);

/**
 * @brief Submits a suite of multiple competing ISA candidate kernels for tournament evaluation.
 *
 * @param[in] fn_id          Target numeric kernel function identifier.
 * @param[in] candidates     Array of candidate descriptors to evaluate.
 * @param[in] n_candidates   Number of candidates in the array.
 * @param[in] target_epsilon Maximum permissible output divergence epsilon.
 * @param[in] target_tensor  Template tensor defining test workload geometry.
 */
MSIMD_API void        SF_custominiVm_puzzle_submit_suite(uint32_t fn_id,
                                                         const SF_custominiVm_isa_candidate_t* candidates,
                                                         int n_candidates,
                                                         float target_epsilon,
                                                         const SF_custominiVm_tensor_t* target_tensor);

/**
 * @brief Submits a batch of empirical test input samples for candidate precision and performance verification.
 *
 * @param[in] fn_id                  Target kernel function identifier.
 * @param[in] batch_samples          Array of input sample tensors passed to candidate kernels.
 * @param[in] sample_count           Number of sample tensors in the batch.
 * @param[in] target_precision_tensor Ground-truth reference tensor used to verify numerical accuracy.
 * @param[in] target_epsilon         Acceptable absolute error tolerance.
 */
MSIMD_API void        SF_custominiVm_puzzle_submit_batch(uint32_t fn_id,
                                                            const SF_custominiVm_tensor_t* batch_samples,
                                                            int sample_count,
                                                            const SF_custominiVm_tensor_t* target_precision_tensor,
                                                            float target_epsilon);

/**
 * @brief Configures the default sample iteration count for puzzle arena benchmark runs.
 * @param[in] count Number of benchmark iterations executed per candidate kernel.
 */
MSIMD_API void        SF_custominiVm_puzzle_set_sample_count(size_t count);

/**
 * @brief Queries the default sample iteration count configured for puzzle arena benchmark runs.
 * @return Configured benchmark iteration count.
 */
MSIMD_API size_t      SF_custominiVm_puzzle_get_sample_count(void);

/**
 * @brief Queries whether continuous puzzle stream benchmarking is currently paused.
 * @return 1 if stream benchmarking is paused, 0 if actively evaluating candidates.
 */
MSIMD_API int         SF_custominiVm_puzzle_is_stream_paused(void);

/**
 * @brief Returns an ASCII summary string describing the host CPU instruction set architecture features.
 * @return Pointer to a static null-terminated string listing detected CPU vector extensions.
 */
MSIMD_API const char* SF_custominiVm_cpu_isa_description(void);

/**
 * @brief Checks whether the host CPU supports a specific hardware instruction set extension.
 *
 * @param[in] isa_tag Null-terminated tag string to query (e.g. "AVX512F", "AVX2", "BMI2", "NEON", "SVE2").
 * @return 1 if supported and enabled by the operating system, 0 otherwise.
 */
MSIMD_API int         SF_custominiVm_cpu_has_isa(const char* isa_tag);

/**
 * @brief Returns the total number of recognized CPU instruction set extensions detected on the host.
 * @return Count of detected ISA features.
 */
MSIMD_API int         SF_custominiVm_cpu_isa_count(void);

/**
 * @brief Retrieves the descriptive tag string for a detected CPU instruction set feature by index.
 *
 * @param[in] index Zero-based index within the range [0, SF_custominiVm_cpu_isa_count() - 1].
 * @return Pointer to a static null-terminated feature tag string, or NULL if index is out of bounds.
 */
MSIMD_API const char* SF_custominiVm_cpu_isa_at(int index);

/* ========================================================================= */
/* HARDWARE TOPOLOGY, CACHE HIERARCHY & DRAM PROFILING SPECIFICATION         */
/* ========================================================================= */

/**
 * @struct SF_custominiVm_cache_level_info_t
 * @brief Hardware cache telemetry for a single hierarchical cache level (L1, L2, or L3).
 */
typedef struct SF_custominiVm_cache_level_info_t {
    /** @brief Total cache capacity measured in kilobytes. */
    uint32_t size_kb;
    /** @brief Width of a single cacheline in bytes (typically 64 or 128 bytes). */
    uint16_t line_size;
    /** @brief Cache set associativity (number of ways). */
    uint8_t  associativity;
    /** @brief Cache classification: 1 = Data Cache, 2 = Instruction Cache, 3 = Unified Cache. */
    uint8_t  type;
} SF_custominiVm_cache_level_info_t;

/**
 * @struct SF_custominiVm_hardware_topology_t
 * @brief Exhaustive hardware architecture and physical execution topology description.
 *
 * Encapsulates full system topology including hierarchical cache geometry (L1i, L1d, L2, L3),
 * physical memory capacity, huge page capabilities, NUMA node topology, memory channels, bus width,
 * asymmetric core architecture (Performance vs. Efficient cores), motherboard identity,
 * DRAM XMP/EXPO frequency profiles, and the active state of the SFVM Upclocker governor.
 */
typedef struct SF_custominiVm_hardware_topology_t {
    /** @brief L1 Instruction Cache specification. */
    SF_custominiVm_cache_level_info_t l1i;
    /** @brief L1 Data Cache specification. */
    SF_custominiVm_cache_level_info_t l1d;
    /** @brief Level 2 Cache specification. */
    SF_custominiVm_cache_level_info_t l2;
    /** @brief Level 3 (Last-Level) Cache specification. */
    SF_custominiVm_cache_level_info_t l3;
    /** @brief Primary CPU cacheline size in bytes. */
    uint16_t primary_cacheline_size;
    /** @brief Alignment padding bytes. */
    uint8_t  _pad0[2];

    /** @brief Total physical system RAM installed in bytes. */
    uint64_t total_phys_ram_bytes;
    /** @brief Physical RAM currently available and uncommitted in bytes. */
    uint64_t avail_phys_ram_bytes;
    /** @brief Standard operating system virtual memory page size in bytes (typically 4096). */
    uint32_t page_size;
    /** @brief Hardware huge page size in bytes (e.g. 2097152 for 2MB, 1073741824 for 1GB). */
    uint32_t huge_page_size;
    /** @brief Boolean flag: 1 if hardware huge pages are supported and accessible, 0 otherwise. */
    uint32_t huge_pages_supported;

    /** @brief Number of Non-Uniform Memory Access (NUMA) nodes detected across the system. */
    uint8_t  numa_nodes;
    /** @brief Number of active physical DRAM memory channels populated on the motherboard. */
    uint8_t  memory_channels;
    /** @brief Total physical memory bus width in bits (e.g. 128 for dual-channel, 256 for quad-channel). */
    uint16_t bus_width_bits;
    /** @brief Estimated theoretical memory bus bandwidth in gigabits per second (Gbps). */
    uint32_t estimated_bandwidth_gbps;
    /** @brief Boolean flag: 1 if system features Unified Memory Architecture (e.g. Apple Silicon, Tegra). */
    uint32_t is_uma;

    /** @brief Total physical processor cores present across all CPU packages. */
    uint32_t physical_cores;
    /** @brief Total logical execution threads (including simultaneous multithreading / Hyper-Threading). */
    uint32_t logical_cores;
    /** @brief Count of high-frequency Performance cores (P-cores) present in hybrid architectures. */
    uint32_t p_cores;
    /** @brief Count of high-efficiency cores (E-cores) present in hybrid architectures. */
    uint32_t e_cores;
    /** @brief Boolean flag: 1 if CPU features heterogeneous hybrid architecture (Intel Alder/Raptor Lake, ARM big.LITTLE). */
    uint32_t is_heterogeneous;

    /** @brief Motherboard manufacturer and model string (e.g. "ASUSTeK COMPUTER INC. ROG STRIX Z790-E"). */
    char     motherboard_model[128];
    /** @brief Effective DRAM frequency profile in megatransfers per second (MT/s, e.g. 6000, 5600, 3200). */
    uint32_t dram_mhz;
    /** @brief Boolean flag: 1 if an overclocked memory profile (Intel XMP, AMD EXPO, or DOCP) is active. */
    uint32_t is_xmp_expo_active;
    /** @brief Current real-time measured CPU core clock frequency in megahertz (MHz). */
    uint32_t cpu_cur_freq_mhz;
    /** @brief Nominal peak turbo clock frequency in megahertz (MHz). */
    uint32_t cpu_max_freq_mhz;

    /** @brief Boolean flag: 1 if the SFVM Upclocker performance governor lock is actively engaged. */
    uint32_t is_upclocker_active;

    /** @brief Formatted textual summary describing the complete detected hardware topology. */
    char summary[1024];
} SF_custominiVm_hardware_topology_t;

/**
 * @brief Retrieves a read-only pointer to the detected hardware topology telemetry structure.
 * @return Pointer to a cached static SF_custominiVm_hardware_topology_t structure.
 */
MSIMD_API const SF_custominiVm_hardware_topology_t* SF_custominiVm_hardware_topology(void);

/**
 * @brief Queries the capacity of the Level 1 Data Cache (L1d) in bytes.
 * @return L1d cache capacity in bytes.
 */
MSIMD_API size_t SF_custominiVm_cpu_l1d_bytes(void);

/**
 * @brief Queries the capacity of the Level 2 Cache (L2) in bytes.
 * @return L2 cache capacity in bytes.
 */
MSIMD_API size_t SF_custominiVm_cpu_l2_bytes(void);

/**
 * @brief Queries the capacity of the Level 3 (Last-Level) Cache (L3) in bytes.
 * @return L3 cache capacity in bytes.
 */
MSIMD_API size_t SF_custominiVm_cpu_l3_bytes(void);

/**
 * @brief Returns the primary processor cacheline size in bytes (typically 64 bytes).
 * @return Cacheline size in bytes.
 */
MSIMD_API size_t SF_custominiVm_cpu_cacheline_bytes(void);

/**
 * @brief Queries the total physical system RAM installed in the host machine in bytes.
 * @return Total installed physical RAM in bytes.
 */
MSIMD_API size_t SF_custominiVm_dram_total_bytes(void);

/**
 * @brief Queries currently uncommitted and available physical system RAM in bytes.
 * @return Available physical RAM in bytes.
 */
MSIMD_API size_t SF_custominiVm_dram_avail_bytes(void);

/**
 * @brief Queries the count of active populated physical memory channels on the motherboard.
 * @return Number of populated memory channels (e.g. 1, 2, 4, 8).
 */
MSIMD_API int    SF_custominiVm_dram_channels(void);

/**
 * @brief Queries the total number of detected Non-Uniform Memory Access (NUMA) nodes.
 * @return Number of NUMA nodes present.
 */
MSIMD_API int    SF_custominiVm_cpu_numa_nodes(void);

/**
 * @brief Queries the count of high-performance cores (P-cores) present in the processor.
 * @return Number of P-cores.
 */
MSIMD_API int    SF_custominiVm_cpu_p_cores(void);

/**
 * @brief Queries the count of high-efficiency cores (E-cores) present in the processor.
 * @return Number of E-cores.
 */
MSIMD_API int    SF_custominiVm_cpu_e_cores(void);

/**
 * @brief Checks whether the system architecture employs Unified Memory (UMA).
 * @return 1 if Unified Memory Architecture is detected, 0 for discrete CPU/GPU topologies.
 */
MSIMD_API int    SF_custominiVm_cpu_is_uma(void);

/**
 * @brief Returns a null-terminated string identifying the motherboard model detected via DMI/SMBIOS.
 * @return Pointer to a static ASCII string containing the motherboard model name.
 */
MSIMD_API const char* SF_custominiVm_motherboard_model(void);

/**
 * @brief Queries the current operating DRAM transfer rate in MT/s (or MHz).
 * @return DRAM transfer rate in megatransfers per second.
 */
MSIMD_API uint32_t    SF_custominiVm_dram_mhz(void);

/**
 * @brief Checks whether an overclocked high-performance memory profile (XMP/EXPO/DOCP) is active.
 * @return 1 if an XMP, EXPO, or DOCP profile is actively engaged, 0 for standard JEDEC timings.
 */
MSIMD_API int         SF_custominiVm_dram_is_xmp_expo(void);

/**
 * @brief Measures the real-time instantaneous operating clock frequency of the current CPU core in MHz.
 * @return Current measured frequency in megahertz.
 */
MSIMD_API uint32_t    SF_custominiVm_cpu_current_mhz(void);

/**
 * @brief Engages or disengages the SFVM Upclocker performance governor lock.
 *
 * When enabled (1), requests high-performance P-state locks and overrides aggressive OS thermal
 * downclocking to maintain peak instruction throughput during compute-intensive kernel loops.
 *
 * @param[in] enable Set to 1 to engage Upclocker boost, 0 to return control to standard OS governor.
 */
MSIMD_API void        SF_custominiVm_upclocker_set_boost(int enable);

/**
 * @brief Queries whether the SFVM Upclocker performance governor is actively engaged.
 * @return 1 if Upclocker boost is currently active, 0 otherwise.
 */
MSIMD_API int         SF_custominiVm_upclocker_is_boosted(void);

/**
 * @brief Warms up the processor execution units by executing a calibrated spinloop.
 *
 * Elevates the processor from low-power C-states (C1/C6) to full active C0 execution state
 * before latency-critical tensor kernels are dispatched.
 *
 * @param[in] duration_us Spinloop duration in microseconds.
 */
MSIMD_API void        SF_custominiVm_upclocker_warmup(uint32_t duration_us);

/* ========================================================================= */
/* VIRTUAL FILE SYSTEM (VFS) & DIRECTORY-HEAP STORAGE API                    */
/* ========================================================================= */

/**
 * @struct SF_custominiVm_vfs_stat_t
 * @brief Status and quota telemetry for a node within the SFVM Virtual File System (VFS).
 */
typedef struct SF_custominiVm_vfs_stat_t {
    /** @brief Unique virtual inode number assigned to the VFS node. */
    uint64_t inode_no;
    /** @brief Total size of the underlying tensor payload in bytes (0 for directories). */
    size_t   size_bytes;
    /** @brief Maximum memory quota assigned to this folder branch in bytes (0 = unlimited). */
    size_t   quota_bytes;
    /** @brief Total volume of memory currently consumed by tensors bound in this folder in bytes. */
    size_t   used_bytes;
    /** @brief Number of child directories and bound tensors residing in this node. */
    uint32_t child_count;
    /** @brief Boolean flag: 1 if this node represents a directory, 0 if it is a tensor file. */
    uint8_t  is_dir;
    /** @brief Structure alignment padding bytes. */
    uint8_t  _pad[3];
} SF_custominiVm_vfs_stat_t;

/**
 * @typedef sfVfsStat
 * @brief Modern concise alias for SF_custominiVm_vfs_stat_t.
 */
typedef struct SF_custominiVm_vfs_stat_t sfVfsStat;

/**
 * @brief Creates a virtual directory node with an optional memory quota in the VFS heap.
 *
 * @param[in] path        Null-terminated virtual directory path (e.g. "/tensors/weights").
 * @param[in] quota_bytes Maximum cumulative memory quota for the folder in bytes (0 = unlimited).
 * @return MSIMD_SUCCESS (0) on success, or an error status code on failure.
 */
MSIMD_API int    SF_custominiVm_folder_mkdir(const char* path, size_t quota_bytes);

/**
 * @brief Binds an existing tensor handle to a virtual path within the VFS directory hierarchy.
 *
 * @param[in] path   Target virtual file path (e.g. "/tensors/weights/layer1.bin").
 * @param[in] tensor Pointer to the tensor handle to bind to the path.
 * @return MSIMD_SUCCESS (0) on success, or an error status code if quota is exceeded.
 */
MSIMD_API int    SF_custominiVm_folder_bind(const char* path, SF_custominiVm_tensor_t* tensor);

/**
 * @brief Evicts all tensors residing within the specified virtual directory path.
 *
 * Relinquishes the memory of all bound tensors in the directory, returning their payload buffers
 * to the Falline runway allocator.
 *
 * @param[in] path Virtual directory path to evict.
 * @return Total volume of memory reclaimed through eviction in bytes.
 */
MSIMD_API size_t SF_custominiVm_folder_evict(const char* path);

/**
 * @brief Queries the total memory volume currently consumed by tensors within a virtual directory.
 *
 * @param[in] path Virtual directory path to query.
 * @return Consumed memory in bytes.
 */
MSIMD_API size_t SF_custominiVm_folder_used_bytes(const char* path);

/**
 * @brief Creates a virtual directory hierarchy in the VFS with an assigned memory quota.
 *
 * @param[in] path        Virtual path to create.
 * @param[in] quota_bytes Assigned memory quota in bytes.
 * @return MSIMD_SUCCESS (0) on success, or an error status code.
 */
MSIMD_API int    SF_custominiVm_vfs_mkdir(const char* path, size_t quota_bytes);

/**
 * @brief Allocates a new tensor and binds it immediately to a specified VFS path.
 *
 * @param[in] path  Target virtual file path.
 * @param[in] shape Array of dimension extents.
 * @param[in] ndim  Tensor rank (1 to 8).
 * @param[in] dtype Element data type (SF_custominiVm_dtype_t).
 * @return Allocated and bound SF_custominiVm_tensor_t handle.
 */
MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_vfs_alloc_tensor(const char* path, const int32_t* shape, int ndim, int dtype);

/**
 * @brief Unlinks and removes a bound tensor file from the VFS hierarchy.
 *
 * @param[in] path Virtual file path to remove.
 * @return MSIMD_SUCCESS (0) on success, or an error status code if not found.
 */
MSIMD_API int    SF_custominiVm_vfs_unlink(const char* path);

/**
 * @brief Recursively removes a virtual directory and all child tensors and folders within it.
 *
 * @param[in] path Virtual directory path to recursively remove.
 * @return Total volume of memory reclaimed in bytes.
 */
MSIMD_API size_t SF_custominiVm_vfs_rmdir_rf(const char* path);

/**
 * @brief Queries status and quota telemetry for a specific node in the VFS.
 *
 * @param[in]  path     Virtual path to inspect.
 * @param[out] out_stat Pointer to a vfs_stat structure populated with node telemetry.
 * @return MSIMD_SUCCESS (0) on success, or an error status code if the path does not exist.
 */
MSIMD_API int    SF_custominiVm_vfs_stat(const char* path, SF_custominiVm_vfs_stat_t* out_stat);

/**
 * @brief Creates an in-memory snapshot image of the VFS tree structure rooted at the given path.
 *
 * @param[in]  path     Root virtual path to serialize.
 * @param[out] out_buf  Destination buffer receiving the serialized snapshot.
 * @param[in]  buf_cap  Capacity of the destination buffer in bytes.
 * @return Total number of bytes written to out_buf, or required capacity if buf_cap is insufficient.
 */
MSIMD_API size_t SF_custominiVm_vfs_snapshot(const char* path, void* out_buf, size_t buf_cap);

/**
 * @brief Enumerates child nodes residing within a specified virtual directory path.
 *
 * @param[in] path Virtual directory path to list.
 * @param[in] cb   Callback function receiving node details (name, is_dir, size_bytes, user context).
 * @param[in] user Arbitrary user context pointer passed to the callback.
 */
MSIMD_API void   SF_custominiVm_vfs_ls(const char* path, void (*cb)(const char* name, int is_dir, size_t bytes, void* user), void* user);

/* ========================================================================= */
/* TENSOR SLICING & ANONYMOUS DISK PAGEFILE PAGING SPECIFICATION             */
/* ========================================================================= */

/**
 * @brief Creates a dependent sub-slice (view) of an existing parent tensor without copying data.
 *
 * The resulting child tensor references a sub-region within the parent tensor\\'s payload buffer,
 * offset by offset_bytes. The child tensor is marked with SF_TENSOR_STATUS_VIEW_SLICE.
 * Memory ownership remains tied to the root ancestor tensor.
 *
 * @param[in] parent       Pointer to the parent tensor to slice.
 * @param[in] offset_bytes Byte offset into the parent payload where the sub-slice begins.
 * @param[in] shape        Array of dimension extents defining the shape of the slice.
 * @param[in] ndim         Rank of the child slice tensor (1 to 8).
 * @param[in] dtype        Element data type (must match or be bit-compatible with parent).
 * @return New SF_custominiVm_tensor_t handle representing the dependent sub-slice view.
 */
MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_tensor_slice(const SF_custominiVm_tensor_t* parent, size_t offset_bytes, const int32_t* shape, int ndim, int dtype);

/**
 * @enum SF_custominiVm_paging_policy_t
 * @brief Eviction and replacement policies governing the anonymous virtual memory disk pagefile.
 */
typedef enum SF_custominiVm_paging_policy_t {
    /** @brief First-In, First-Out (FIFO): The oldest swapped tensor is selected for eviction first. */
    SF_PAGING_FIFO = 0,
    /** @brief Last-In, First-Out (LIFO): The most recently swapped tensor is selected for eviction first. */
    SF_PAGING_LIFO = 1,
    /** @brief First-In, Last-Out (FILO): Equivalent to LIFO. */
    SF_PAGING_FILO = 2,
    /** @brief Last-In, Last-Out (LILO): Equivalent to FIFO. */
    SF_PAGING_LILO = 3,
} SF_custominiVm_paging_policy_t;

/**
 * @typedef sfPagingPolicy
 * @brief Modern concise alias for SF_custominiVm_paging_policy_t.
 */
typedef SF_custominiVm_paging_policy_t sfPagingPolicy;

/**
 * @brief Initializes the anonymous backing disk pagefile for virtual memory swap operations.
 *
 * Pre-allocates and formats an anonymous backing file on the primary fast storage drive,
 * enabling transparent paging out of cold, inactive tensors when physical RAM pressure peaks.
 *
 * @param[in] max_bytes Maximum disk pagefile capacity in bytes.
 * @param[in] policy    Paging eviction policy (FIFO, LIFO).
 * @return MSIMD_SUCCESS (0) on success, or an error status code on disk I/O failure.
 */
MSIMD_API int    SF_custominiVm_pagefile_init(size_t max_bytes, SF_custominiVm_paging_policy_t policy);

/**
 * @brief Swaps a cold tensor payload out of physical RAM and writes it to the disk pagefile.
 *
 * Once paged out, physical RAM occupied by the tensor is relinquished, and the tensor\\'s
 * status flag is updated. Accessing the tensor automatically triggers transparent page-in.
 *
 * @param[in,out] tensor Pointer to the tensor handle to page out.
 * @return MSIMD_SUCCESS (0) on success, or an error status code on failure.
 */
MSIMD_API int    SF_custominiVm_page_out(SF_custominiVm_tensor_t* tensor);

/**
 * @brief Pages an evicted tensor payload back from the disk pagefile into physical RAM (Soft Fault).
 *
 * @param[in,out] tensor Pointer to the swapped-out tensor handle to retrieve.
 * @return MSIMD_SUCCESS (0) on success, or an error status code on failure.
 */
MSIMD_API int    SF_custominiVm_page_in(SF_custominiVm_tensor_t* tensor);

/**
 * @brief Queries the volume of disk pagefile storage currently consumed by paged-out tensors in bytes.
 * @return Consumed pagefile bytes.
 */
MSIMD_API size_t SF_custominiVm_pagefile_used_bytes(void);

/**
 * @brief Queries the cumulative number of allocation pacing stalls triggered by memory pressure.
 * @return Cumulative count of pacing stall events.
 */
MSIMD_API size_t SF_custominiVm_pacing_stalls(void);

/* ========================================================================= */
/* SINGLE-BOARD COMPUTER (SBC) MMIO & ZERO-SYSCALL GPIO HARDWARE SPECIFICATION */
/* ========================================================================= */

/**
 * @enum SF_custominiVm_sbc_board_type_e
 * @brief Supported Single-Board Computer (SBC) and embedded processor board architectures.
 */
typedef enum SF_custominiVm_sbc_board_type_e {
    /** @brief Standard x86_64 PC / Server hardware (no direct onboard GPIO MMIO registers). */
    SF_SBC_NONE                   = 0,
    /** @brief Raspberry Pi 1 / Zero: Broadcom BCM2835 SoC (MMIO Base: 0x20200000). */
    SF_SBC_RASPBERRY_PI_1_ZERO    = 1,
    /** @brief Raspberry Pi 2 / 3: Broadcom BCM2836/2837 SoC (MMIO Base: 0x3F200000, ARM Cortex-A7/A53). */
    SF_SBC_RASPBERRY_PI_2_3       = 2,
    /** @brief Raspberry Pi 4: Broadcom BCM2711 SoC (MMIO Base: 0xFE200000, ARM Cortex-A72). */
    SF_SBC_RASPBERRY_PI_4         = 3,
    /** @brief Raspberry Pi 5: RP1 Southbridge PCIe BAR (rp1-peripherals specification). */
    SF_SBC_RASPBERRY_PI_5         = 4,
    /** @brief LattePanda x86: Intel x86_64 host with onboard ATmega32u4 or RP2040 coprocessor bridge. */
    SF_SBC_LATTEPANDA_X86         = 5,
    /** @brief NVIDIA Jetson: Tegra SoC MMIO with Unified CPU/GPU zero-copy memory mapping. */
    SF_SBC_NVIDIA_JETSON          = 6,
    /** @brief Orange Pi: Allwinner H616 / Rockchip RK3588 peripheral MMIO mapping. */
    SF_SBC_ORANGE_PI              = 7,
    /** @brief Generic Linux Character Device fallback via /dev/gpiochip Character Device ABI v2. */
    SF_SBC_GENERIC_LINUX_DEV      = 8,
} SF_custominiVm_sbc_board_type_e;

/**
 * @typedef sfSbcBoardType
 * @brief Modern concise alias for SF_custominiVm_sbc_board_type_e.
 */
typedef SF_custominiVm_sbc_board_type_e sfSbcBoardType;

/**
 * @enum SF_custominiVm_gpio_pin_mode_e
 * @brief Directional operating modes for hardware General-Purpose Input/Output (GPIO) pins.
 */
typedef enum SF_custominiVm_gpio_pin_mode_e {
    /** @brief Standard digital input mode. */
    SF_GPIO_MODE_INPUT          = 0,
    /** @brief Standard digital output mode. */
    SF_GPIO_MODE_OUTPUT         = 1,
    /** @brief Hardware Pulse-Width Modulation (PWM) signal generation mode. */
    SF_GPIO_MODE_PWM            = 2,
    /** @brief Fast MMIO mode: direct register memory writes in ~1-2 CPU clock cycles with zero syscall overhead. */
    SF_GPIO_MODE_FAST_MMIO      = 3,
    /** @brief Hardware interrupt triggered on rising signal edge. */
    SF_GPIO_MODE_IRQ_RISING     = 4,
    /** @brief Hardware interrupt triggered on falling signal edge. */
    SF_GPIO_MODE_IRQ_FALLING    = 5,
} SF_custominiVm_gpio_pin_mode_e;

/**
 * @typedef sfGpioPinMode
 * @brief Modern concise alias for SF_custominiVm_gpio_pin_mode_e.
 */
typedef SF_custominiVm_gpio_pin_mode_e sfGpioPinMode;

/**
 * @struct SF_custominiVm_gpio_desc_t
 * @brief Hardware memory-mapped I/O (MMIO) descriptor for physical GPIO peripherals.
 */
typedef struct SF_custominiVm_gpio_desc_t {
    /** @brief Detected single-board computer hardware type. */
    SF_custominiVm_sbc_board_type_e board_type;
    /** @brief Physical peripheral hardware base address (e.g. 0x3F200000 on Raspberry Pi 2/3). */
    uint64_t mmio_phys_base;
    /** @brief Virtual address pointer mapped to the physical peripheral registers via /dev/mem or mmap. */
    void*    mmio_virtual_base;
    /** @brief Total count of accessible GPIO pins available on the board header. */
    uint32_t total_pins;
    /** @brief Boolean flag: 1 if fast direct-MMIO memory register writes are active. */
    uint8_t  is_fast_mmio_active;
    /** @brief Boolean flag: 1 if board communicates through an onboard microcontroller coprocessor. */
    uint8_t  is_coprocessor_bridge;
    /** @brief Alignment padding bytes. */
    uint8_t  _reserved[6];
    /** @brief Null-terminated board name string (e.g. "Raspberry Pi 3 Model B Rev 1.2"). */
    char     board_name[64];
} SF_custominiVm_gpio_desc_t;

/**
 * @typedef sfGpioDesc
 * @brief Modern concise alias for SF_custominiVm_gpio_desc_t.
 */
typedef SF_custominiVm_gpio_desc_t sfGpioDesc;

/* ========================================================================= */
/* CONTINUOUS REAL-TIME SENSOR STREAMING SPECIFICATION                       */
/* ========================================================================= */

/**
 * @enum SF_custominiVm_sensor_type_e
 * @brief Supported real-time sensor device classifications for direct DMA/MMIO tensor streaming.
 */
typedef enum SF_custominiVm_sensor_type_e {
    /** @brief Generic analog voltage/current sensor via ADC interface. */
    SF_SENSOR_GENERIC_ANALOG       = 0x01,
    /** @brief Digital temperature and humidity sensor (e.g. DHT11, DHT22, Sensirion SHT3x). */
    SF_SENSOR_TEMP_HUMIDITY        = 0x02,
    /** @brief Analog or digital capacitive/resistive water level sensor. */
    SF_SENSOR_WATER_LEVEL          = 0x03,
    /** @brief Infrared optical barrier / reflectance obstacle detection sensor. */
    SF_SENSOR_IR_OPTICAL           = 0x04,
    /** @brief 6-DoF or 9-DoF Inertial Measurement Unit (IMU) Accelerometer/Gyroscope (e.g. MPU-6050, ICM-20948). */
    SF_SENSOR_IMU_ACCEL_GYRO       = 0x05,
    /** @brief High-speed quadrature optical/magnetic rotary encoder counter. */
    SF_SENSOR_ROTARY_ENCODER       = 0x06,
    /** @brief 32-channel parallel digital logic analyzer capture port stream. */
    SF_SENSOR_LOGIC_ANALYZER_32CH  = 0x07,
} SF_custominiVm_sensor_type_e;

/**
 * @typedef sfSensorType
 * @brief Modern concise alias for SF_custominiVm_sensor_type_e.
 */
typedef SF_custominiVm_sensor_type_e sfSensorType;

/**
 * @struct SF_custominiVm_sensor_config_t
 * @brief Configuration parameters for continuous, zero-copy real-time sensor tensor ingestion.
 *
 * Configures a continuous hardware sensor ingestion stream that samples physical sensors
 * at deterministic frequencies and streams samples directly into a pinned SFVM tensor
 * ring-buffer without copying data and without incurring GC Stop-The-World pauses.
 */
typedef struct SF_custominiVm_sensor_config_t {
    /** @brief Categorical sensor classification type. */
    SF_custominiVm_sensor_type_e sensor_type;
    /** @brief Primary hardware GPIO signal or data pin index. */
    uint32_t pin_primary;
    /** @brief Secondary pin index (e.g. I2C SCL, SPI CLK, or Encoder Channel B). */
    uint32_t pin_secondary;
    /** @brief Hardware sampling frequency in hertz (Hz). */
    uint32_t sample_rate_hz;
    /** @brief Number of historical sample frames buffered within the circular tensor ring-buffer. */
    uint32_t buffer_sample_count;
    /** @brief Boolean flag: 1 if hardware Direct Memory Access (DMA) or PWM pacing is utilized. */
    uint8_t  dma_enabled;
    /** @brief Boolean flag: 1 to mark the stream tensor as strictly PINNED to prevent GC compaction. */
    uint8_t  gc_exempt_pinning;
    /** @brief Structure alignment padding bytes. */
    uint8_t  _pad[2];
} SF_custominiVm_sensor_config_t;

/**
 * @typedef sfSensorConfig
 * @brief Modern concise alias for SF_custominiVm_sensor_config_t.
 */
typedef SF_custominiVm_sensor_config_t sfSensorConfig;

/**
 * @brief Probes system hardware and identifies the host Single-Board Computer (SBC) architecture.
 *
 * @param[out] out_desc Pointer to a GPIO descriptor structure populated with detected board details.
 * @return MSIMD_SUCCESS (0) on success, or an error status code if unsupported.
 */
MSIMD_API int     SF_custominiVm_sbc_detect_board(SF_custominiVm_gpio_desc_t* out_desc);

/**
 * @brief Initializes the low-level MMIO hardware subsystem using the provided board descriptor.
 *
 * Maps peripheral physical addresses into process virtual address space via `/dev/mem` or
 * character device interfaces, configuring zero-syscall fast memory access.
 *
 * @param[in,out] desc Pointer to the initialized GPIO descriptor.
 * @return MSIMD_SUCCESS (0) on success, or an error status code on permission/mapping failure.
 */
MSIMD_API int     SF_custominiVm_gpio_init(SF_custominiVm_gpio_desc_t* desc);

/**
 * @brief Sets the operating direction and electronic mode of a specific GPIO pin.
 *
 * @param[in] pin  Physical GPIO pin index.
 * @param[in] mode Target directional mode (Input, Output, PWM, Fast MMIO).
 * @return MSIMD_SUCCESS (0) on success, or an error status code.
 */
MSIMD_API int     SF_custominiVm_gpio_set_mode(uint32_t pin, SF_custominiVm_gpio_pin_mode_e mode);

/**
 * @brief Performs an instantaneous digital write to a GPIO pin via direct MMIO register manipulation.
 *
 * Operates in approximately 1 to 2 CPU clock cycles without incurring operating system syscall overhead.
 *
 * @param[in] pin   Physical GPIO pin index.
 * @param[in] level Digital logic level (1 = High / 3.3V, 0 = Low / GND).
 */
MSIMD_API void    SF_custominiVm_gpio_fast_write(uint32_t pin, uint8_t level);

/**
 * @brief Performs an instantaneous digital read of a GPIO pin via direct MMIO register access.
 *
 * @param[in] pin Physical GPIO pin index.
 * @return Current digital logic level (1 = High, 0 = Low).
 */
MSIMD_API uint8_t SF_custominiVm_gpio_fast_read(uint32_t pin);

/**
 * @brief Attaches a real-time sensor device stream directly to a pinned SFVM tensor ring-buffer.
 *
 * Establishes continuous background sampling of sensor data into out_stream_tensor without
 * dynamic memory allocations or GC pauses.
 *
 * @param[in]  cfg                Sensor hardware configuration and sampling parameters.
 * @param[out] out_stream_tensor  Output tensor handle initialized as a circular sample ring-buffer.
 * @return MSIMD_SUCCESS (0) on success, or an error status code.
 */
MSIMD_API int     SF_custominiVm_sensor_attach_stream(const SF_custominiVm_sensor_config_t* cfg, SF_custominiVm_tensor_t* out_stream_tensor);

/**
 * @brief Polls newly arrived sample frames from an active real-time sensor stream tensor.
 *
 * @param[in,out] stream_tensor Active sensor stream tensor handle.
 * @param[in]     max_samples   Maximum number of newly ingested sample frames to process.
 * @return Total number of new sample frames successfully retrieved.
 */
MSIMD_API size_t  SF_custominiVm_sensor_poll_stream(SF_custominiVm_tensor_t* stream_tensor, size_t max_samples);

/**
 * @brief Detaches an active sensor device stream and releases hardware DMA/interrupt resources.
 *
 * @param[in,out] stream_tensor Sensor stream tensor handle to detach.
 */
MSIMD_API void    SF_custominiVm_sensor_detach_stream(SF_custominiVm_tensor_t* stream_tensor);

/* ========================================================================= */
/* RING-0 KERNEL BRIDGE (SFVM_K.sys / sfvm_k.ko) SPECIFICATION                */
/* ========================================================================= */

/**
 * @struct SF_custominiVm_ring0_status_t
 * @brief Connectivity and privilege status of the Ring-0 kernel acceleration driver bridge.
 */
typedef struct SF_custominiVm_ring0_status_t {
    /** @brief Boolean flag: 1 if the SFVM_K kernel driver is loaded and IOCTL channel is established. */
    uint8_t  is_driver_loaded;
    /** @brief Boolean flag: 1 if CR4.PCE is enabled, permitting user-mode Ring-3 execution of RDPMC instructions. */
    uint8_t  cr4_pce_enabled;
    /** @brief Boolean flag: 1 if Hardware P-States (HWP) turbo override is active with EPP=0. */
    uint8_t  hwp_turbo_override;
    /** @brief Boolean flag: 1 if 1GB physically locked Giant Pages are available for tensor allocation. */
    uint8_t  giant_pages_available;
    /** @brief Kernel driver communication protocol version (e.g. 0x0100 for version 1.0). */
    uint32_t driver_version;
    /** @brief Bitmask of isolated CPU cores where kernel-level pinning and modifications are enforced. */
    uint64_t pinned_core_mask;
} SF_custominiVm_ring0_status_t;

/**
 * @typedef sfRing0Status
 * @brief Modern concise alias for SF_custominiVm_ring0_status_t.
 */
typedef SF_custominiVm_ring0_status_t sfRing0Status;

/**
 * @struct SF_custominiVm_ring0_hwp_policy_t
 * @brief Hardware P-State (HWP) power and frequency configuration parameters enforced via Ring-0.
 */
typedef struct SF_custominiVm_ring0_hwp_policy_t {
    /** @brief Minimum CPU core frequency floor in megahertz (prevents AVX-512 frequency downclock penalties). */
    uint32_t min_freq_mhz;
    /** @brief Maximum permissible peak turbo clock frequency in megahertz. */
    uint32_t max_freq_mhz;
    /** @brief Energy Performance Preference (EPP): 0 = Maximum Performance, 128 = Balanced, 255 = Power Save. */
    uint8_t  epp_preference;
    /** @brief Boolean flag: 1 to deliver an immediate HWP boost pulse, bypassing governor hysteresis latency. */
    uint8_t  anti_downclock_boost;
    /** @brief Boolean flag: 1 to temporarily suspend L2 Streamer hardware prefetchers during SIMD GC sweeps. */
    uint8_t  disable_prefetchers;
    /** @brief Structure padding byte. */
    uint8_t  _pad[1];
} SF_custominiVm_ring0_hwp_policy_t;

/**
 * @typedef sfRing0HwpPolicy
 * @brief Modern concise alias for SF_custominiVm_ring0_hwp_policy_t.
 */
typedef SF_custominiVm_ring0_hwp_policy_t sfRing0HwpPolicy;

/**
 * @brief Establishes an authenticated communication channel with the Ring-0 kernel acceleration driver.
 *
 * Opens an IOCTL device handle to `SFVM_K.sys` (Windows) or `/dev/sfvm_k` (Linux), enabling
 * privileged performance counter access, MSR adjustments, and physically locked memory pages.
 *
 * @param[out] out_status Pointer to a status structure populated with kernel bridge telemetry.
 * @return MSIMD_SUCCESS (0) on success, or an error status code if driver is not present.
 */
MSIMD_API int  SF_custominiVm_ring0_connect(SF_custominiVm_ring0_status_t* out_status);

/**
 * @brief Enforces an advanced Hardware P-State (HWP) power and frequency policy via Ring-0 MSR writes.
 *
 * Configures hardware Energy-Performance Preference (EPP) registers and locks minimum core
 * frequencies to eliminate thermal throttling and AVX-512 downclock penalties.
 *
 * @param[in] policy Pointer to the target HWP policy specification.
 * @return MSIMD_SUCCESS (0) on success, or an error status code.
 */
MSIMD_API int  SF_custominiVm_ring0_apply_hwp_policy(const SF_custominiVm_ring0_hwp_policy_t* policy);

/**
 * @brief Closes the communication channel with the Ring-0 kernel driver and restores default power profiles.
 */
MSIMD_API void SF_custominiVm_ring0_disconnect(void);

/* ========================================================================= */
/* MULTI-LANGUAGE BINDING & HOST GC SYNCHRONIZATION API                      */
/* ========================================================================= */

/**
 * @enum SF_custominiVm_gc_hook_event_e
 * @brief Lifecycle hook events dispatched by sfGC to registered external runtime callbacks.
 */
typedef enum SF_custominiVm_gc_hook_event_e {
    /** @brief Dispatched immediately prior to initiating an incremental generational sweep. */
    SF_GC_HOOK_PRE_SWEEP        = 0x01,
    /** @brief Dispatched immediately upon the completion of a generational sweep. */
    SF_GC_HOOK_POST_SWEEP       = 0x02,
    /** @brief Dispatched before executing sliding compaction (Brooks forwarding pointers active). */
    SF_GC_HOOK_PRE_COMPACT      = 0x03,
    /** @brief Dispatched when sliding compaction pass has finalized. */
    SF_GC_HOOK_POST_COMPACT     = 0x04,
    /** @brief Dispatched when Landfill arena exceeds critical threshold, requiring urgent host evacuation. */
    SF_GC_HOOK_EMERGENCY_EVAC   = 0x05,
    /** @brief Dispatched when a cooperative Stop-The-World pause is requested across threads. */
    SF_GC_HOOK_STW_REQUEST      = 0x06,
    /** @brief Dispatched when all threads are released from a Stop-The-World pause. */
    SF_GC_HOOK_STW_RELEASE      = 0x07,
} SF_custominiVm_gc_hook_event_e;

/**
 * @typedef sfGcHookEvent
 * @brief Modern concise alias for SF_custominiVm_gc_hook_event_e.
 */
typedef SF_custominiVm_gc_hook_event_e sfGcHookEvent;

/**
 * @typedef SF_custominiVm_gc_hook_fn_t
 * @brief Function pointer signature for external runtime GC lifecycle event callbacks.
 *
 * @param[in] event           Lifecycle event code (SF_custominiVm_gc_hook_event_e).
 * @param[in] sweep_no        Monotonically increasing generational sweep sequence identifier.
 * @param[in] heap_used_bytes Total active on-heap memory volume in bytes at event dispatch.
 * @param[in] user_data       Arbitrary user context pointer registered with the hook.
 */
typedef void (*SF_custominiVm_gc_hook_fn_t)(
    SF_custominiVm_gc_hook_event_e event,
    uint64_t sweep_no,
    size_t heap_used_bytes,
    void* user_data);

/**
 * @typedef sfGcHookFn
 * @brief Modern concise alias for SF_custominiVm_gc_hook_fn_t.
 */
typedef SF_custominiVm_gc_hook_fn_t sfGcHookFn;

/**
 * @def SF_HOST_GC_JVM_START
 * @brief Host notification: Java Virtual Machine (ZGC/Shenandoah/G1) garbage collection initiated.
 */
#define SF_HOST_GC_JVM_START     0x10

/**
 * @def SF_HOST_GC_JVM_END
 * @brief Host notification: Java Virtual Machine garbage collection completed.
 */
#define SF_HOST_GC_JVM_END       0x11

/**
 * @def SF_HOST_GC_CLR_START
 * @brief Host notification: .NET CoreCLR GC (Gen 0/1/2) collection initiated.
 */
#define SF_HOST_GC_CLR_START     0x20

/**
 * @def SF_HOST_GC_CLR_END
 * @brief Host notification: .NET CoreCLR GC collection completed.
 */
#define SF_HOST_GC_CLR_END       0x21

/**
 * @def SF_HOST_GC_PYTHON_START
 * @brief Host notification: Python runtime `gc.collect()` initiated.
 */
#define SF_HOST_GC_PYTHON_START  0x30

/**
 * @def SF_HOST_GC_PYTHON_END
 * @brief Host notification: Python runtime garbage collection completed.
 */
#define SF_HOST_GC_PYTHON_END    0x31

/**
 * @brief Registers an administrative lifecycle hook callback invoked during sfGC events.
 *
 * @param[in] hook      Callback function pointer conforming to SF_custominiVm_gc_hook_fn_t.
 * @param[in] user_data Arbitrary user context pointer passed to the callback.
 */
MSIMD_API void   SF_custominiVm_register_gc_hook(SF_custominiVm_gc_hook_fn_t hook, void* user_data);

/**
 * @brief Notifies the SFVM runtime of an external host application GC cycle initiation or completion.
 *
 * Enables bidirectional synchronization between host managed runtimes (JVM, CLR, Python) and SFVM,
 * preventing overlapping collection pauses and synchronizing memory pressure adjustments.
 *
 * @param[in] host_event_id Notification event code (e.g. SF_HOST_GC_JVM_START, SF_HOST_GC_CLR_START).
 */
MSIMD_API void   SF_custominiVm_host_gc_notify(uint32_t host_event_id);

/**
 * @brief Advances incremental SFVM housekeeping tasks within a time-budgeted host application loop.
 *
 * Designed for cooperative host game engines and real-time simulators. Executes pending
 * incremental GC sweeps, telemetry drains, and Falline empty-box recycling within a strict
 * time budget.
 *
 * @param[in] budget_us Maximum execution time slice allocated to SFVM housekeeping in microseconds.
 * @return Total volume of memory reclaimed during the tick in bytes.
 */
MSIMD_API size_t SF_custominiVm_host_loop_tick(uint32_t budget_us);

/* ========================================================================= */
/* TWO-STAGE LIFECYCLE INITIALIZATION SPECIFICATION                          */
/* ========================================================================= */

/**
 * @brief Stage 1 Initializer: Discovers hardware topology, sets up virtual memory arenas, and mounts VFS.
 *
 * Establishes the foundational memory infrastructure, maps peripheral MMIO ranges, and formats
 * virtual file system structures before any bitcode is ingested.
 *
 * @param[in] custom_cfg Optional custom configuration pointer (may be NULL for default discovery).
 * @return MSIMD_SUCCESS (0) on success, or an error status code on failure.
 */
MSIMD_API int SF_custominiVm_setup_start(const void* custom_cfg);

/**
 * @brief Stage 2 Initializer: Ingests embedded bitcode, decompresses code heaps, and boots JIT engine.
 *
 * Completes initialization by verifying off-heap JIT code memory, resolving core kernel symbols,
 * and transitioning the engine into the fully active execution state.
 *
 * @return MSIMD_SUCCESS (0) on success, or an error status code on bitcode decompression error.
 */
MSIMD_API int SF_custominiVm_start(void);

/**
 * @brief Queries whether Stage 1 initialization (Setup) has been successfully executed.
 * @return 1 if Stage 1 setup is complete, 0 otherwise.
 */
MSIMD_API int SF_custominiVm_is_setup(void);

/**
 * @brief Queries whether Stage 2 initialization (Start) has completed and the VM is actively running.
 * @return 1 if the VM is running and accepting kernel calls, 0 otherwise.
 */
MSIMD_API int SF_custominiVm_is_started(void);

/* ========================================================================= */
/* GEARKIT TRANSMISSION ENGINE & DUAL-PROFILE API SPECIFICATION              */
/* ========================================================================= */

/**
 * @brief Polls the cooperative garbage collection safepoint.
 *
 * Emitted at loop back-edges and function epilogues in JIT-compiled kernels. Checks whether
 * a Stop-The-World compaction has been requested, yielding the thread cooperatively if required.
 */
MSIMD_API void sf_gc_safepoint_poll(void);

#ifdef __cplusplus
/**
 * @brief Evaluates whether an allocation controlpoint threshold has been exceeded.
 *
 * @param[in] allocated_bytes Volume of memory newly allocated in the current block (default 0).
 * @return 1 if an allocation pacing stall or GC cycle is required, 0 if allocation may proceed.
 */
MSIMD_API int  sf_gc_controlpoint_check(size_t allocated_bytes = 0);
#else
/**
 * @brief Evaluates whether an allocation controlpoint threshold has been exceeded.
 *
 * @param[in] allocated_bytes Volume of memory newly allocated in the current block.
 * @return 1 if an allocation pacing stall or GC cycle is required, 0 if allocation may proceed.
 */
MSIMD_API int  sf_gc_controlpoint_check(size_t allocated_bytes);
#endif

/**
 * @struct SF_custominiVm_code_heap_stat_t
 * @brief Telemetry metrics detailing the off-heap JIT code arena state and compilation counters.
 */
typedef struct SF_custominiVm_code_heap_stat_t {
    /** @brief Total byte capacity of the immutable rawcode cell storage. */
    size_t rawcode_capacity;
    /** @brief Consumed memory volume within the rawcode storage in bytes. */
    size_t rawcode_used_bytes;
    /** @brief Boolean flag: 1 if rawcode cells are sealed with PAGE_READONLY protection. */
    int    rawcode_is_immutable;

    /** @brief Total byte capacity of processed executable code chunks. */
    size_t processed_chunk_capacity;
    /** @brief Consumed byte volume of compiled executable code chunks. */
    size_t processed_chunk_used_bytes;
    /** @brief Boolean flag: 1 if processed code pages are sealed with PAGE_EXECUTE_READ (W^X). */
    int    processed_chunk_is_sealed;

    /** @brief Active JIT tier level (1 = Vector Ahead-of-Time, 2 = Micro-JIT, 3 = Puzzle Arena). */
    uint32_t active_jit_tier;
    /** @brief Total cumulative count of kernels compiled and resident in the code heap. */
    uint32_t compiled_kernel_count;
    /** @brief Total count of deoptimization events triggered due to dynamic type/rank mismatches. */
    uint32_t deopt_event_count;
} SF_custominiVm_code_heap_stat_t;

/**
 * @typedef sfCodeHeapStat
 * @brief Modern concise alias for SF_custominiVm_code_heap_stat_t.
 */
typedef SF_custominiVm_code_heap_stat_t sfCodeHeapStat;

/**
 * @brief Queries operational metrics and memory distribution of the off-heap JIT code arena.
 * @param[out] out_stat Pointer to a code heap telemetry structure populated by the query.
 */
MSIMD_API void SF_custominiVm_code_heap_query(SF_custominiVm_code_heap_stat_t* out_stat);

/**
 * @brief Engages the NORMAX standard high-throughput HPC execution gear profile.
 *
 * Enables dynamic slab chunk chaining, adaptive background concurrent garbage collection,
 * and aggressive SIMD vector speculation across heterogeneous processor cores.
 *
 * @return MSIMD_SUCCESS (0) on success, or an error status code.
 */
MSIMD_API int SacFor_Gearkit_NORMAX(void);

/**
 * @brief Engages the default SSIOLSC Safety-Critical deterministic execution gear profile.
 *
 * Switches to Phase-STW Bureaucratic GC (ISO 26262 ASIL-D compliant). Forbids runtime dynamic
 * memory expansion and restricts compaction strictly to deterministic phase boundaries.
 *
 * @return MSIMD_SUCCESS (0) on success, or an error status code.
 */
MSIMD_API int SacFor_Gearkit_SSIOLSC(void);

/**
 * @brief Engages the SSIOLSC Safety-Critical profile with an explicit secondary SCGC operational mode.
 *
 * @param[in] scgc_type Secondary SCGC mode (Phase-STW Bureaucratic GC or Pure Zero-GC PrimitiveRW).
 * @return MSIMD_SUCCESS (0) on success, or an error status code.
 */
MSIMD_API int SacFor_Gearkit_SSIOLSC_Ex(SF_custominiVm_scgc_type_t scgc_type);

/**
 * @brief Queries the currently engaged Gearkit transmission profile mode.
 * @return MSIMD_PROFILE_STANDARD (NORMAX) or MSIMD_PROFILE_SAFETY_CRITICAL (SSIOLSC).
 */
MSIMD_API msimd_profile_mode_t SacFor_Gearkit_GetMode(void);

#ifdef __cplusplus
} // extern "C"

/**
 * @struct SF_Gearkit_Model
 * @brief Modern C++ static encapsulation of the SacculusForge Gearkit transmission and runtime engine.
 *
 * Provides expressive, type-safe C++ static methods for single-line gear shifting between
 * NORMAX (HPC) and SSIOLSC (Safety Critical) profiles, raw unmanaged Primitiva memory operations,
 * and Independent Puzzle Arena batch tournament submissions.
 */
struct SF_Gearkit_Model {
    /**
     * @brief Switches the engine to the NORMAX high-throughput HPC profile.
     * @return MSIMD_SUCCESS (0) on success, or an error status code.
     */
    static inline int NORMAX() {
        return SacFor_Gearkit_NORMAX();
    }

    /**
     * @brief Switches the engine to the default SSIOLSC Safety-Critical profile (Phase-STW Bureaucratic).
     * @return MSIMD_SUCCESS (0) on success, or an error status code.
     */
    static inline int SSIOLSC() {
        return SacFor_Gearkit_SSIOLSC();
    }

    /**
     * @brief Switches the engine to the SSIOLSC Safety-Critical profile with selectable Primitiva R/W mode.
     *
     * @param[in] use_primitiva_rw If true, selects SF_SCGC_TYPE_PRIMITIVE_RW (Zero-GC / Raw Sector mode);
     *                             if false, selects SF_SCGC_TYPE_DEFAULT (Phase-STW Bureaucratic mode).
     * @return MSIMD_SUCCESS (0) on success, or an error status code.
     */
    static inline int SSIOLSC(bool use_primitiva_rw) {
        return SacFor_Gearkit_SSIOLSC_Ex(
            use_primitiva_rw ? SF_SCGC_TYPE_PRIMITIVE_RW : SF_SCGC_TYPE_DEFAULT);
    }

    /**
     * @brief Switches the engine to the SSIOLSC profile with an explicit SCGC mode enum.
     *
     * @param[in] scgc_type Target SCGC operational mode.
     * @return MSIMD_SUCCESS (0) on success, or an error status code.
     */
    static inline int SSIOLSC(SF_custominiVm_scgc_type_t scgc_type) {
        return SacFor_Gearkit_SSIOLSC_Ex(scgc_type);
    }

    /**
     * @brief Queries the currently active operational profile mode.
     * @return Active msimd_profile_mode_t enum constant.
     */
    static inline msimd_profile_mode_t GetMode() {
        return SacFor_Gearkit_GetMode();
    }

    /**
     * @brief Checks whether the engine is actively executing in Safety-Critical mode.
     * @return True if in Safety-Critical mode, false otherwise.
     */
    static inline bool IsSafetyCritical() {
        return SacFor_Gearkit_GetMode() == MSIMD_PROFILE_SAFETY_CRITICAL;
    }

    /**
     * @brief Allocates an unmanaged raw memory sector bypassing all object headers (PrimitiveRW mode).
     *
     * @param[in] bytes Requested sector capacity in bytes.
     * @param[in] align Memory boundary alignment in bytes (default 64).
     * @return Pointer to the allocated raw sector.
     */
    static inline void* PrimitivaAlloc(size_t bytes, size_t align = 64) {
        return SF_primitiva_alloc(bytes, align);
    }

    /**
     * @brief Writes data directly into an unmanaged raw memory sector.
     *
     * @param[out] sector Pointer to destination sector.
     * @param[in]  src    Pointer to source buffer.
     * @param[in]  bytes  Number of bytes to copy.
     */
    static inline void PrimitivaWrite(void* sector, const void* src, size_t bytes) {
        SF_primitiva_write(sector, src, bytes);
    }

    /**
     * @brief Reads data directly from an unmanaged raw memory sector.
     *
     * @param[in]  sector Pointer to source sector.
     * @param[out] dst    Pointer to destination buffer.
     * @param[in]  bytes  Number of bytes to read.
     */
    static inline void PrimitivaRead(const void* sector, void* dst, size_t bytes) {
        SF_primitiva_read(sector, dst, bytes);
    }

    /**
     * @brief Overwrites an unmanaged raw memory sector in-place using non-temporal streaming stores.
     *
     * @param[out] sector Pointer to destination sector.
     * @param[in]  src    Pointer to source data.
     * @param[in]  bytes  Number of bytes to overwrite.
     */
    static inline void PrimitivaOverwrite(void* sector, const void* src, size_t bytes) {
        SF_primitiva_overwrite(sector, src, bytes);
    }

    /**
     * @brief Clears an unmanaged raw memory sector to zero using vectorized non-temporal stores.
     *
     * @param[out] sector Pointer to sector to clear.
     * @param[in]  bytes  Number of bytes to zero.
     */
    static inline void PrimitivaZero(void* sector, size_t bytes) {
        SF_primitiva_zero(sector, bytes);
    }

    /**
     * @brief Queries the currently configured SCGC secondary operational mode.
     * @return Current SF_custominiVm_scgc_type_t mode.
     */
    static inline SF_custominiVm_scgc_type_t GetScgcType() {
        return SF_custominiVm_get_scgc_type();
    }

    /**
     * @brief Sets the SCGC secondary operational mode.
     * @param[in] t Target SF_custominiVm_scgc_type_t mode.
     */
    static inline void SetScgcType(SF_custominiVm_scgc_type_t t) {
        SF_custominiVm_set_scgc_type(t);
    }

    /**
     * @brief Submits a batch of empirical test input samples for candidate kernel evaluation in the Puzzle Arena.
     *
     * @param[in] fn_id                  Target kernel function identifier.
     * @param[in] batch_samples          Array of input sample tensors passed to candidate kernels.
     * @param[in] sample_count           Number of sample tensors in the batch.
     * @param[in] target_precision_tensor Ground-truth reference tensor used to verify numerical accuracy.
     * @param[in] target_epsilon         Acceptable absolute error tolerance (default 1e-4f).
     */
    static inline void SubmitPuzzleBatch(uint32_t fn_id,
                                        const SF_custominiVm_tensor_t* batch_samples,
                                        int sample_count,
                                        const SF_custominiVm_tensor_t* target_precision_tensor,
                                        float target_epsilon = 1e-4f) {
        SF_custominiVm_puzzle_submit_batch(fn_id, batch_samples, sample_count, target_precision_tensor, target_epsilon);
    }

    /**
     * @brief Configures the default benchmark iteration sample count for candidate evaluations.
     * @param[in] count Benchmark iteration count.
     */
    static inline void SetPuzzleSampleCount(size_t count) {
        SF_custominiVm_puzzle_set_sample_count(count);
    }

    /**
     * @brief Queries the default benchmark iteration sample count for candidate evaluations.
     * @return Benchmark iteration count.
     */
    static inline size_t GetPuzzleSampleCount() {
        return SF_custominiVm_puzzle_get_sample_count();
    }

    /**
     * @brief Checks whether continuous puzzle stream benchmarking is currently paused.
     * @return True if paused, false if actively evaluating candidates.
     */
    static inline bool IsStreamPaused() {
        return SF_custominiVm_puzzle_is_stream_paused() != 0;
    }
};

/**
 * @typedef SacFor_Gearkit_Model
 * @brief Legacy alias for SF_Gearkit_Model to maintain backward compatibility.
 */
using SacFor_Gearkit_Model = SF_Gearkit_Model;

/**
 * @typedef SF_Gearkit
 * @brief Standard ergonomic alias for SF_Gearkit_Model.
 */
using SF_Gearkit           = SF_Gearkit_Model;

#endif // __cplusplus

#endif // SACCULUS_FORGE_H
'''

with open(r"C:\Users\aziml\my_projects\MathventageSIMD\include\SacculusForge.h", "w", encoding="utf-8") as f:
    f.write(content)

print("Successfully written pristine documented SacculusForge.h")
