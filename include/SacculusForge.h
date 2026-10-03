/**
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
 * Each data type adheres strictly to IEEE 754 floating-point standards or two\'s complement
 * integer formats, with deterministic bit-widths and alignment requirements.
 */
typedef enum SF_custominiVm_dtype_t {
    /** @brief 32-bit single-precision floating point (IEEE 754 binary32), 4-byte element size. */
    SF_DTYPE_F32 = 0,
    /** @brief 64-bit double-precision floating point (IEEE 754 binary64), 8-byte element size. */
    SF_DTYPE_F64 = 1,
    /** @brief 32-bit signed two\'s-complement integer, 4-byte element size. */
    SF_DTYPE_I32 = 2,
    /** @brief 64-bit signed two\'s-complement integer, 8-byte element size. */
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
/* KERNEL FUNCTION PROTOTYPE SPECIFICATION                                   */
/* ========================================================================= */
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
/* ZERO-COPY TENSOR SLICING (SUB-VIEW CREATION)                              */
/* ========================================================================= */


/**
 * @brief Creates a dependent sub-slice (view) of an existing parent tensor without copying data.
 *
 * The resulting child tensor references a sub-region within the parent tensor\'s payload buffer,
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

/* ========================================================================= */
/* GENERATIONAL sfGC CONTROL & LIFECYCLE SWEEP API                           */
/* ========================================================================= */
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
#endif

#ifdef __cplusplus

/**
 * @struct SF_Gearkit_Model
 * @brief Ergonomic modern C++ interface for dynamic operational profile shifting.
 *
 * Provides static methods to switch between the NORMAX (HPC / Dynamic GC) profile
 * and the SSIOLSC (Safety-Critical / Deterministic GC) execution profiles.
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

#include <type_traits>


/* ========================================================================= */
/* CONTINUNTIL.FINISH & STEPPING_MTP TAGS & CONSTANTS                        */
/* ========================================================================= */

/**
 * @struct _sf_continuntil_tag
 * @brief Tag type enabling ergonomic `CONTINUNTIL.FINISH` syntax for continuous GC execution.
 */
struct _sf_continuntil_tag {
    static constexpr SF_custominiVm_gc_run_mode_t FINISH = SF_GC_RUN_CONTINUE_FINISH;
    constexpr SF_custominiVm_gc_run_mode_t operator()() const { return FINISH; }
};

#if defined(__cplusplus) && __cplusplus >= 201703L
/**
 * @brief Global tag instance enabling intuitive syntax: `Sfvm.Gc(CONTINUNTIL.FINISH)`.
 */
inline constexpr _sf_continuntil_tag CONTINUNTIL{};
#else
static const _sf_continuntil_tag CONTINUNTIL{};
#endif

/** @brief Stepping mode constant for Manual Termination Point (MTP) execution. */
static constexpr SF_custominiVm_gc_run_mode_t STEPPING_MTP = SF_GC_RUN_STEPPING_MTP;
/** @brief Stepping mode constant for single-cycle incremental GC stepping. */
static constexpr SF_custominiVm_gc_run_mode_t STEP_SINGLE  = SF_GC_RUN_SINGLE_STEP;

/* ========================================================================= */
/* C++ SFVM RUNTIME & MANUAL GC HELPER (Sfvm.Gc() / Sfvm::Gc())              */
/* ========================================================================= */

/**
 * @struct sfvm_runtime_t
 * @brief High-level modern C++ static interface for SFVM garbage collection, memory telemetry, and heuristics.
 *
 * Provides an idiomatic C++ API wrapping low-level C SF_custominiVm_* functions. Supports single-step
 * collection, continuous draining to finish, predicate-controlled stepping with C++ lambdas,
 * and comprehensive Caretaker Manifest reporting.
 */
struct sfvm_runtime_t {
    /**
     * @brief Executes a single incremental garbage collection cycle.
     *
     * @param[in] owp One-Way Pacing flag: if true, applies strict allocation backpressure pacing.
     * @return Volume of memory reclaimed during the step in bytes.
     */
    static inline size_t Gc(bool owp = false) {
        return SF_custominiVm_gc_run(SF_GC_RUN_SINGLE_STEP, owp ? 1 : 0, nullptr, nullptr);
    }

    /**
     * @brief Executes garbage collection under a specified operational stepping mode.
     *
     * Example: `Sfvm.Gc(CONTINUNTIL.FINISH)` or `Sfvm.Gc(STEPPING_MTP, true)`.
     *
     * @param[in] mode Stepping mode (SF_GC_RUN_SINGLE_STEP, CONTINUE_FINISH, or STEPPING_MTP).
     * @param[in] owp  One-Way Pacing flag.
     * @return Total volume of memory reclaimed in bytes.
     */
    static inline size_t Gc(SF_custominiVm_gc_run_mode_t mode, bool owp = false) {
        return SF_custominiVm_gc_run(mode, owp ? 1 : 0, nullptr, nullptr);
    }

    /**
     * @brief Executes continuous collection until the heap is fully drained using the CONTINUNTIL tag.
     *
     * Example: `Sfvm.Gc(CONTINUNTIL)`.
     *
     * @param[in] tag Unused tag parameter for overload resolution.
     * @param[in] owp One-Way Pacing flag.
     * @return Total volume of memory reclaimed in bytes.
     */
    static inline size_t Gc(const _sf_continuntil_tag&, bool owp = false) {
        return SF_custominiVm_gc_run(SF_GC_RUN_CONTINUE_FINISH, owp ? 1 : 0, nullptr, nullptr);
    }

    /**
     * @brief Executes stepping collection evaluating a C-style function pointer MTP predicate.
     *
     * @param[in] mode      Target stepping mode.
     * @param[in] owp       One-Way Pacing flag.
     * @param[in] pred      Function pointer predicate returning 1 to terminate, 0 to continue.
     * @param[in] user_data Context pointer forwarded to pred.
     * @return Reclaimed bytes.
     */
    static inline size_t Gc(SF_custominiVm_gc_run_mode_t mode, bool owp,
                            SF_custominiVm_mtp_predicate_fn pred, void* user_data = nullptr) {
        return SF_custominiVm_gc_run(mode, owp ? 1 : 0, pred, user_data);
    }

    /**
     * @brief Executes stepping collection evaluating a modern C++ lambda or callable predicate.
     *
     * Example:
     * @code
     * Sfvm.Gc(STEPPING_MTP, false, [](const sfAnatomyStat* s, const sfEnvChange* e) {
     *     return s->falline_emptybox_bytes < 1024 * 1024;
     * });
     * @endcode
     *
     * @tparam Predicate Callable object type matching `bool(const sfAnatomyStat*, const sfEnvChange*)`.
     * @param[in] mode Target stepping mode.
     * @param[in] owp  One-Way Pacing flag.
     * @param[in] pred Lambda or functor predicate.
     * @return Reclaimed bytes.
     */
    template <typename Predicate, typename = typename std::enable_if<!std::is_convertible<Predicate, bool>::value && !std::is_convertible<Predicate, SF_custominiVm_gc_run_mode_t>::value>::type>
    static inline size_t Gc(SF_custominiVm_gc_run_mode_t mode, bool owp, Predicate pred) {
        auto thunk = [](const SF_custominiVm_anatomy_stat_t* s, const SF_custominiVm_env_change_t* e, void* ctx) -> int {
            auto* p = static_cast<Predicate*>(ctx);
            return (*p)(s, e) ? 1 : 0;
        };
        return SF_custominiVm_gc_run(mode, owp ? 1 : 0, thunk, &pred);
    }

    /**
     * @brief Triggers a deterministic deep clean sweep ("Shake the Bed and Sleep") and returns the full audit manifest.
     * @return Completed Caretaker Manifest audit record.
     */
    static inline SF_custominiVm_gc_manifest_t GcReport() {
        return SF_custominiVm_gc_shake_bed();
    }

    /**
     * @brief Retrieves the audit manifest generated by the most recent deep clean sweep.
     * @return Last recorded Caretaker Manifest.
     */
    static inline SF_custominiVm_gc_manifest_t LastManifest() {
        SF_custominiVm_gc_manifest_t m{};
        SF_custominiVm_gc_get_last_manifest(&m);
        return m;
    }

    /**
     * @brief Configures the autonomous garbage collection heuristic model.
     * @param[in] h Selected heuristic (Adaptive, Aggressive, Bureaucratic, or Disabled).
     */
    static inline void SetHeuristic(SF_custominiVm_gc_heuristic_t h) {
        SF_custominiVm_gc_set_heuristic(h);
    }

    /**
     * @brief Queries the currently configured autonomous garbage collection heuristic model.
     * @return Active heuristic enum value.
     */
    static inline SF_custominiVm_gc_heuristic_t GetHeuristic() {
        return SF_custominiVm_gc_get_heuristic();
    }

    /**
     * @brief Enables or disables No-sfGC (Zero-GC) execution mode.
     * @param[in] enabled Set to true for Zero-GC mode, false to restore active collection.
     */
    static inline void SetNoGc(bool enabled) {
        SF_custominiVm_gc_set_no_sfgc(enabled ? 1 : 0);
    }

    /**
     * @brief Checks whether the engine is currently executing in No-sfGC (Zero-GC) mode.
     * @return True if in Zero-GC mode, false otherwise.
     */
    static inline bool IsNoGc() {
        return SF_custominiVm_gc_is_no_sfgc() != 0;
    }

    /** @brief Returns total active on-heap memory volume in bytes. */
    static inline size_t HeapBytes() { return SF_custominiVm_heap_bytes(); }
    /** @brief Returns cumulative rebated memory returned to active runways in bytes. */
    static inline size_t RebatedBytes() { return SF_custominiVm_heap_rebated_bytes(); }
    /** @brief Returns cumulative count of completed GC sweep cycles. */
    static inline uint64_t SweepCount() { return SF_custominiVm_gc_sweep_count(); }
    /** @brief Shuts down the SFVM runtime engine and frees all allocated resources. */
    static inline void Shutdown() { SF_custominiVm_shutdown(); }
};

#if defined(__cplusplus) && __cplusplus >= 201703L
/**
 * @brief Global singleton instance of the modern C++ SFVM runtime helper.
 */
inline constexpr sfvm_runtime_t Sfvm{};
#else
static const sfvm_runtime_t Sfvm{};
#endif

/* ========================================================================= */
/* TWO-STAGE LIFECYCLE C++ WRAPPERS (SFVM_Setup & SFVM_VM)                   */
/* ========================================================================= */

/**
 * @struct sfvm_setup_t
 * @brief C++ Stage 1 Initializer: Discovers hardware topology, allocates virtual memory arenas, and mounts VFS.
 */
struct sfvm_setup_t {
    /**
     * @brief Executes Stage 1 initialization.
     * @param[in] cfg Optional custom configuration descriptor (default nullptr).
     * @return MSIMD_SUCCESS (0) on success, or an error status code.
     */
    static inline int Start(const void* cfg = nullptr) {
        return SF_custominiVm_setup_start(cfg);
    }

    /**
     * @brief Checks whether Stage 1 setup has completed successfully.
     * @return True if setup is complete, false otherwise.
     */
    static inline bool IsReady() {
        return SF_custominiVm_is_setup() != 0;
    }

    /** @brief Function call operator overload executing Start(cfg). */
    inline int operator()(const void* cfg = nullptr) const {
        return Start(cfg);
    }
};

#if defined(__cplusplus) && __cplusplus >= 201703L
/** @brief Global callable Stage 1 initializer object: `SFVM_Setup()`. */
inline constexpr sfvm_setup_t SFVM_Setup{};
#else
static const sfvm_setup_t SFVM_Setup{};
#endif

/**
 * @struct sfvm_vm_t
 * @brief C++ Stage 2 Initializer: Ingests bitcode, decompresses code heaps, and boots JIT engine.
 */
struct sfvm_vm_t {
    /**
     * @brief Executes Stage 2 initialization and launches the execution engine.
     * @return MSIMD_SUCCESS (0) on success, or an error status code.
     */
    static inline int Start() {
        return SF_custominiVm_start();
    }

    /**
     * @brief Checks whether the VM has started and is actively executing.
     * @return True if actively running, false otherwise.
     */
    static inline bool IsRunning() {
        return SF_custominiVm_is_started() != 0;
    }

    /** @brief Shuts down the VM execution engine. */
    static inline void Shutdown() {
        SF_custominiVm_shutdown();
    }

    /** @brief Function call operator overload executing Start(). */
    inline int operator()() const {
        return Start();
    }
};

#if defined(__cplusplus) && __cplusplus >= 201703L
/** @brief Global callable Stage 2 initializer object: `SFVM_VM()`. */
inline constexpr sfvm_vm_t SFVM_VM{};
#else
static const sfvm_vm_t SFVM_VM{};
#endif

#endif // __cplusplus

#endif // SACCULUS_FORGE_H
