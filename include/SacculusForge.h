/// \file SacculusForge.h
/// \brief SacculusForge Virtual Machine (SFVM) Runtime Engine, JIT Compiler, Generational Garbage Collector (sfGC), and Heterogeneous Hardware Interop ABI.
///
/// =================================================================================
/// SACCULUSFORGE RUNTIME ARCHITECTURE & SPECIFICATION SPECIFICATION SPECIFICATION SPECIFICATION
/// =================================================================================
/// The SacculusForge Virtual Machine (SFVM, represented via the C API family SF_custominiVm_*) is a
/// high-performance, deterministic execution environment engineered specifically for high-throughput
/// scientific tensor computation, multi-tier SIMD acceleration, and safety-critical real-time processing.
/// Designed with a strict zero-overhead philosophy, SFVM unifies a multi-generational memory model,
/// an off-heap JIT code arena, a zero-syscall memory-mapped I/O (MMIO) hardware engine, and a low-latency
/// Ring-0 kernel bridge into a single, cohesive binary.
///
/// ### 1. Memory Topography & Generational sfGC Model
/// Memory within SFVM is partitioned into distinct On-Heap and Off-Heap domains, strictly separated
/// to provide predictable allocation pacing, lockless telemetry, and deterministic pause profiles:
/// - **Runway (Young & Nursery Domain):** Comprises the active allocation space divided into the
///   **Motherline** (where newly allocated tensor payloads and live headers reside) and the
///   **Falline** (a chain of segregated, recycled empty boxes designed for rapid LIFO/FIFO reuse).
/// - **Tank Regions (Humongous Tensor Domain):** Dedicated, contiguous large-page memory allocations
///   reserved for massive multi-megabyte and multi-gigabyte tensors that exceed standard slab chunk sizes.
///   These regions bypass standard generational sliding compaction to eliminate copying overhead.
/// - **Landfill (Mature & Evacuated Domain):** Segregated memory arena where aged objects that have
///   survived multiple GC generational sweeps undergo final reclamation, rehabilitation, or eviction
///   to external backing stores (such as anonymous disk swap pagefiles).
/// - **Independent Puzzle Arena:** An on-heap isolated execution sandbox designed for continuous,
///   concurrent auto-tuning and benchmarking of competing SIMD/ISA vector kernels (e.g. AVX-512, AVX2,
///   ARM SVE2, RISC-V RVV) against precision and epsilon tolerances.
/// - **Off-Heap Arenas:** Completely outside the jurisdiction of sfGC collectors. Houses executable JIT
///   code buffers protected via strict W^X (Write XOR Execute) memory page permissions, as well as circular
///   lockless telemetry ring buffers (HotLog and ColdLog) mapped to cacheline-isolated off-heap pages.
///
/// ### 2. Dual Standard Profile Architecture (NORMAX vs. SSIOLSC)
/// Modeled after the Khronos Vulkan SC (Safety Critical) paradigm, SFVM compiles into a single unified
/// binary that dynamically supports two mutually exclusive runtime profiles:
/// - **NORMAX (High-Throughput HPC Profile):** Optimized for peak floating-point throughput, aggressive
///   dynamic slab chunk chaining, adaptive background concurrent garbage collection, and dynamic SIMD
///   speculation across heterogeneous symmetric and asymmetric cores.
/// - **SSIOLSC (Deterministic Safety-Critical Profile):** Compliant with ISO 26262 ASIL-D, IEC 61508,
///   and DO-178C requirements. Dynamic memory growth after initialization is strictly forbidden.
///   Memory compaction is executed strictly within deterministic phase boundaries (Phase-STW) or operates
///   in pure Zero-GC mode (PrimitiveRW), bypassing object headers entirely for direct sector writes.
///
/// ### 3. Zero-Syscall MMIO & Heterogeneous Hardware Bridges
/// SFVM provides native physical hardware integration targeting both standard x86_64/ARM64 servers and
/// Single-Board Computers (Raspberry Pi 1-5, LattePanda, NVIDIA Jetson, Orange Pi). Through direct virtual
/// mapping of peripheral physical addresses, GPIO lines and real-time sensor streams (IMU, optical, analog)
/// are ingested directly into pinned, cache-aligned tensor buffers with sub-microsecond latency.
///
/// ### 4. Concurrency, ABI & Multi-Language Bindings
/// All C interfaces expose standard C-linkage (`extern "C"`), ensuring seamless interoperability with
/// modern foreign function interfaces (FFI), including Java Foreign Function & Memory (FFM) API (JEP 424/442),
/// .NET CoreCLR P/Invoke, Rust `cxx`, Python `nanobind`/`ctypes`, and ISO C++20. All structures maintain
/// 64-byte or 32-byte cacheline alignment to eliminate false sharing across multi-threaded cores.

#ifndef SACCULUS_FORGE_H
#define SACCULUS_FORGE_H

#include "MathventageSIMD.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */
/* CORE ARCHITECTURE CONSTANTS & AFFINITY BITMAP SPECIFICATION               */
/* ========================================================================= */


/// \def SF_MAX_CORES
/// \brief Maximum number of hardware logical processor cores monitored and tracked by SFVM.
///
/// Tracks processor affinity and per-core cycle telemetry across up to 256 logical execution units.
/// Represented internally as a contiguous 256-bit affinity bitset (4 consecutive 64-bit words).
/// This layout ensures zero-allocation thread affinity masks compatible with Windows Processor Groups
/// and Linux CPU_SET256 kernel scheduling structures.
#define SF_MAX_CORES            256u

/// \def SF_CORE_WORDS
/// \brief Number of 64-bit integer words required to represent the 256-core affinity bitmask.
///
/// Formula: 256 cores / 64 bits per word = 4 words. Each word corresponds to a quadword bitmask,
/// allowing single-instruction bitwise tests (such as `popcnt`, `tzcnt`, and `blsr`) to evaluate
/// multi-core residency with single-cycle latency.
#define SF_CORE_WORDS           4u

/// \def SACFOR_MAX_CORES
/// \brief Legacy alias for SF_MAX_CORES to preserve backward source compatibility.
#define SACFOR_MAX_CORES        SF_MAX_CORES

/// \def SACFOR_CORE_WORDS
/// \brief Legacy alias for SF_CORE_WORDS to preserve backward source compatibility.
#define SACFOR_CORE_WORDS       SF_CORE_WORDS

/* ========================================================================= */
/* TENSOR DATA TYPES & BUFFER FLAGS                                          */
/* ========================================================================= */


/// \enum SF_custominiVm_dtype_t
/// \brief Primitive data element types supported by the SFVM tensor runtime.
///
/// Enumerates all fundamental arithmetic types valid for tensor memory buffers.
/// Each data type adheres strictly to IEEE 754 floating-point standards or two\'s complement
/// integer formats, with deterministic bit-widths and alignment requirements.
typedef enum SF_custominiVm_dtype_t {
    /// \brief 32-bit single-precision floating point (IEEE 754 binary32), 4-byte element size.
    SF_DTYPE_F32 = 0,
    /// \brief 64-bit double-precision floating point (IEEE 754 binary64), 8-byte element size.
    SF_DTYPE_F64 = 1,
    /// \brief 32-bit signed two\'s-complement integer, 4-byte element size.
    SF_DTYPE_I32 = 2,
    /// \brief 64-bit signed two\'s-complement integer, 8-byte element size.
    SF_DTYPE_I64 = 3,
    /// \brief 16-bit half-precision floating point (IEEE 754 binary16), 2-byte element size.
    SF_DTYPE_F16 = 4,
    /// \brief 8-bit unsigned integer (byte payload / quantized tensor element), 1-byte element size.
    SF_DTYPE_U8   = 5,
    /// \brief 8-bit floating point (E4M3/E5M2 quantized tensor element), 1-byte element size.
    SF_DTYPE_F8   = 6,
    /// \brief 8-bit signed two's-complement integer, 1-byte element size.
    SF_DTYPE_I8   = 7,
    /// \brief 16-bit signed two's-complement integer, 2-byte element size.
    SF_DTYPE_I16  = 8,
    /// \brief 128-bit quad-precision floating point (IEEE 754 binary128), 16-byte element size.
    SF_DTYPE_F128 = 9,
    /// \brief 128-bit signed integer (multi-limb carry chain), 16-byte element size.
    SF_DTYPE_I128 = 10,
    /// \brief 256-bit oct-single / quad-double floating point, 32-byte element size.
    SF_DTYPE_F256 = 11,
    /// \brief 256-bit signed integer (multi-limb carry chain), 32-byte element size.
    SF_DTYPE_I256 = 12,
    /// \brief 16-bit unsigned integer, 2-byte element size.
    SF_DTYPE_U16  = 13,
    /// \brief 32-bit unsigned integer, 4-byte element size.
    SF_DTYPE_U32  = 14,
    /// \brief 64-bit unsigned integer, 8-byte element size.
    SF_DTYPE_U64  = 15,
    /// \brief 128-bit unsigned integer (multi-limb carry chain), 16-byte element size.
    SF_DTYPE_U128 = 16,
    /// \brief 256-bit unsigned integer (multi-limb carry chain), 32-byte element size.
    SF_DTYPE_U256 = 17,
} SF_custominiVm_dtype_t;

/// \typedef sfDType
/// \brief Modern concise alias for SF_custominiVm_dtype_t.
typedef SF_custominiVm_dtype_t sfDType;

/// \enum SF_custominiVm_tensor_flag_t
/// \brief Ingress lifecycle and placement directives transmitted from the host application to SFVM.
///
/// Bitmask flags governing how SFVM, its allocator, and the generational sfGC treat a tensor.
/// These flags are evaluated upon tensor admission and remain invariant throughout its lifecycle.
typedef enum SF_custominiVm_tensor_flag_t {
    /// \brief Default admission flag; standard garbage-collected lifecycle.
    SF_TENSOR_FLAG_NONE       = 0x00,

    /// \brief Pinned buffer flag.
    /// Indicates that the underlying memory buffer is pinned by an external host environment
    /// (such as the Java JVM via JNI/FFM or .NET CoreCLR GC). When set, the sfGC sliding compactor
    /// is strictly forbidden from relocating, reallocating, or compacting this tensor.
    SF_TENSOR_FLAG_PINNED     = 0x01,

    /// \brief Immutable buffer flag.
    /// Declares the payload memory strictly read-only. Modifying writes are prohibited, enabling
    /// zero-copy shared memory reuse and memory-mapped file protection via OS page-table locks.
    SF_TENSOR_FLAG_IMMUTABLE  = 0x02,

    /// \brief Anti-paging DRAM retention flag.
    /// Prevents the tensor memory from ever being swapped out to the anonymous disk pagefile.
    /// Critical for real-time sensor loops and high-frequency trading where page-fault latency is fatal.
    SF_TENSOR_FLAG_NO_PAGING  = 0x04,

    /// \brief Ephemeral short-lived buffer flag.
    /// Hints to the allocator that the tensor is transient and will not survive past the current
    /// kernel dispatch step. Ephemeral buffers are allocated preferentially from the nursery Falline.
    SF_TENSOR_FLAG_EPHEMERAL  = 0x08,
} SF_custominiVm_tensor_flag_t;

/// \enum SF_custominiVm_tensor_status_t
/// \brief Egress telemetry and health status flags returned from SFVM to the host application.
///
/// Provides real-time visibility into the memory state of a tensor following GC sweeps,
/// virtual memory paging actions, or memory integrity verification checks.
typedef enum SF_custominiVm_tensor_status_t {
    /// \brief Tensor memory is contiguous, valid, resident in physical RAM, and healthy.
    SF_TENSOR_STATUS_OK           = 0x00,

    /// \brief Forwarded pointer status.
    /// The tensor was relocated during a generational sliding compaction sweep. The internal
    /// data pointer has been automatically resolved through its Brooks forwarding pointer.
    SF_TENSOR_STATUS_FORWARDED    = 0x01,

    /// \brief Soft page-fault resolution status.
    /// The tensor was cold and previously swapped out to the anonymous disk pagefile; it has been
    /// transparently paged back into physical RAM upon access.
    SF_TENSOR_STATUS_PAGED_IN     = 0x02,

    /// \brief Dependent slice status.
    /// The tensor represents a sub-window or slice view of an existing parent tensor, sharing
    /// its underlying physical buffer without owning the underlying memory allocation.
    SF_TENSOR_STATUS_VIEW_SLICE   = 0x04,

    /// \brief Quarantined corrupted status.
    /// A canary corruption or buffer overrun violation was detected in the boundary guards of this
    /// tensor. It has been immediately quarantined by sfGC to prevent systemic memory contamination.
    SF_TENSOR_STATUS_QUARANTINED  = 0x08,
} SF_custominiVm_tensor_status_t;

/* ========================================================================= */
/* TENSOR DESCRIPTOR (CACHE-ALIGNED C ABI COMPLIANT HANDLE)                  */
/* ========================================================================= */


/// \struct SF_custominiVm_tensor_t
/// \brief Core tensor handle representing a multidimensional contiguous array in SFVM.
///
/// Designed with a strict zero-overhead philosophy, this structure provides a 100% C ABI-compliant
/// memory layout. It fits cleanly into a standard 64-byte processor cacheline, ensuring that
/// accessing tensor metadata incurs zero extraneous memory bus transactions.
///
/// Dimensions and shapes support up to 8 rank dimensions (compatible with high-order tensor
/// decompositions, batch-sequence-head-dim representations in deep neural networks, and 3D/4D
/// spatial-temporal physical simulations).
typedef struct SF_custominiVm_tensor_t {
    /// \brief Pointer to the raw payload data buffer. Must be 64-byte aligned for AVX-512 operations.
    void*    data;
    /// \brief Extents along each dimension. Unused dimensions beyond ndim are initialized to 0.
    int32_t  shape[8];
    /// \brief Number of active dimensions (rank of the tensor). Must be in the range [0, 8].
    int32_t  ndim;
    /// \brief Element data type, encoded as an SF_custominiVm_dtype_t value.
    uint8_t  dtype;
    /// \brief Host-to-VM ingress control directives (bitmask of SF_custominiVm_tensor_flag_t).
    uint8_t  flags;
    /// \brief VM-to-Host egress telemetry status (bitmask of SF_custominiVm_tensor_status_t).
    uint8_t  status;
    /// \brief Reserved internal alignment byte. Preserves struct alignment and zero-padding.
    uint8_t  _reserved;
    /// \brief Total size of the contiguous payload data in bytes (shape[0] * ... * shape[ndim-1] * sizeof(dtype)).
    size_t   byte_size;
} SF_custominiVm_tensor_t;

/// \typedef sfTensor
/// \brief Modern concise alias for SF_custominiVm_tensor_t.
typedef SF_custominiVm_tensor_t sfTensor;

/* ========================================================================= */
/* KERNEL FUNCTION PROTOTYPE SPECIFICATION                                   */
/* ========================================================================= */
/// \typedef SF_custominiVm_kernel_fn_t
/// \brief Standardized function pointer prototype for dispatchable SFVM compute kernels.
///
/// All mathematical, tensor, and JIT-compiled kernels adhere to this unified dispatch signature,
/// enabling seamless dispatch via SF_custominiVm_call().
///
/// \param[in]     inputs     Pointer to an array of input tensor handles.
/// \param[in]     n_in       Number of valid input tensors in the inputs array.
/// \param[in,out] outputs    Pointer to an array of output tensor handles to be populated.
/// \param[in]     n_out      Number of expected output tensors in the outputs array.
/// \param[in,out] user_data  Arbitrary user context pointer passed through from dispatch invocation.
typedef void (*SF_custominiVm_kernel_fn_t)(
    const SF_custominiVm_tensor_t* inputs,  int n_in,
    SF_custominiVm_tensor_t*       outputs, int n_out,
    void*                          user_data);

/// \typedef sfKernelFn
/// \brief Modern concise alias for SF_custominiVm_kernel_fn_t.
typedef SF_custominiVm_kernel_fn_t sfKernelFn;

/* ========================================================================= */
/* SFVM PUBLIC MANAGEMENT & INITIALIZATION API                               */
/* ========================================================================= */


/// \brief Initializes the SFVM runtime engine with explicit arena capacities.
///
/// Allocates and configures the core virtual memory arenas for the VM. This function must be
/// called before any tensors are created or kernels are dispatched.
///
/// Memory configured via this call is mapped into contiguous virtual address space, with guard
/// pages positioned between the On-Heap Runway and the Off-Heap JIT code arenas.
///
/// \param[in] code_arena_mb  Size of the off-heap executable JIT code arena in megabytes.
/// \param[in] pressure_mb    Threshold in megabytes that triggers automatic background GC sweeps.
/// \return MSIMD_SUCCESS (0) on success, or an MSIMD negative error status code on failure.
MSIMD_API int                  SF_custominiVm_init(size_t code_arena_mb, size_t pressure_mb);

/// \brief Initializes the SFVM runtime using a declarative configuration flag string.
///
/// Configures all operational parameters of the VM, the generational sfGC collector, and hardware
/// execution profiles using structured key-value parameters.
///
/// Parameter format follows the standard syntax: `-#SF|Key:Value`. Multiple parameters are
/// delimited by semicolons or whitespace.
///
/// ### Supported Configuration Parameters:
/// - **`-#SF|Profile:Standard`** or **`-#SF|Profile:SC`**: Selects between the NORMAX standard
///   high-throughput profile and the SSIOLSC Safety-Critical deterministic profile (ISO 26262 ASIL-D).
/// - **`-#SF|GC_Mode:Cycles`** or **`-#SF|GC_Mode:SIMD`**: Selects between standard cycle-counted
///   GC collection and fully vectorized SIMD parallel sweeping across object header arrays.
/// - **`-#SF|Use_Critical_SIMDly_Operations:Allow / Deny / Auto`**: Controls whether critical vector
///   instructions may be utilized during nursery compaction. Note: Cannot be set to `:Deny` if
///   `GC_Mode:SIMD` is selected (results in `MSIMD_ERROR_FLAG_CONFLICT`).
/// - **`-#SF|Snatch_and_Rebate_Mode:Auto / Rapid / Lazy / GYC`**: Configures how aggressively unused
///   Falline empty-box memory is returned to the active nursery runway.
/// - **`-#SF|FreeList_Recycle_Policy:LIFO / FIFO`**: Cache recycling policy for the Falline free list.
/// - **`-#SF|Runway_Handsel_Attempt_Value:1Gb / 512Mb`**: Initial virtual memory commitment for Runway.
/// - **`-#SF|Landfill_Handsel_Attempt_Value:285Mb`**: Initial virtual memory commitment for Landfill.
/// - **`-#SF|Wakes_GC_Cycling_Interval:1Ms / 280Ms`**: Periodic interval for background GC wakeups.
/// - **`-#SF|Line_Compaction_Freq:350Ms`**: Frequency of sliding generational compaction passes.
/// - **`-#SF|In_Heap_Forwarding_Rate:7`**: In-heap Brooks pointer forwarding resolution rate.
/// - **`-#SF|Pressure_Threshold:256Mb`**: Heap usage pressure limit triggering urgent GC sweeps.
/// - **`-#SF|AlwaysPreTouch:1 / 0`**: When enabled (1), pages are committed immediately to eliminate
///   runtime page faults during deterministic execution phases.
/// - **`-#SF|GC_Heuristic:Adaptive / Aggressive / Bureaucratic`**: Automated collector tuning model.
/// - **`-#SF|Always_Focus_Compactizing:1 / 0`**: Forces full compaction on every sweep pass.
/// - **`-#SF|Forbid_Runtime_Heap:1 / 0`**: Strictly locks heap size post-initialization (mandatory in SC).
/// - **`-#SF|Max_Bounded_Iterations:1000000`**: Worst-Case Execution Time (WCET) iteration ceiling.
///
/// \param[in] flags_str Null-terminated ASCII string containing declarative configuration flags.
/// \return MSIMD_SUCCESS (0) on success, or an MSIMD negative error status code on invalid flags.
MSIMD_API int                  SF_custominiVm_init_with_flags(const char* flags_str);

/// \brief Retrieves the active operational profile of the SFVM engine.
/// \return MSIMD_PROFILE_STANDARD (NORMAX) or MSIMD_PROFILE_SAFETY_CRITICAL (SSIOLSC).
MSIMD_API msimd_profile_mode_t SF_custominiVm_get_profile(void);

/// \brief Queries whether the engine is currently executing under the Safety-Critical (SSIOLSC) profile.
/// \return 1 if executing in Safety-Critical mode, 0 if executing in standard NORMAX mode.
MSIMD_API int                  SF_custominiVm_is_safety_critical(void);

/// \brief Shuts down the SFVM runtime engine and releases all allocated memory arenas.
///
/// Flushes all pending telemetry logs, unmaps all virtual memory arenas, terminates background
/// collector worker threads, and releases hardware MMIO device bindings.
MSIMD_API void                 SF_custominiVm_shutdown(void);

/// \brief Emergency OS-level sweep that guarantees zero residual memory leaks to the host operating system.
///
/// In the event of abrupt termination, unhandled panic, or abnormal shutdown failure, this function
/// directly executes operating system calls (VirtualFree with MEM_RELEASE on Windows, munmap on POSIX)
/// across all registered runtime blocks, VFS pagefiles, shared ring buffers, MMIO sectors, and Primitiva memory.
/// \return Total number of bytes reclaimed and unmapped at the OS level.
MSIMD_API size_t               SF_custominiVm_emergency_os_sweep(void);

/* ========================================================================= */
/* TENSOR LIFECYCLE & SLAB ALLOCATOR API                                     */
/* ========================================================================= */


/// \brief Wraps an existing, externally managed memory buffer in an SFVM tensor handle.
///
/// Performs zero-copy encapsulation of external memory (such as buffers allocated by host
/// runtimes, OpenGL/Vulkan mapped buffers, or static arrays).
///
/// \param[in] data   Pointer to the existing raw data buffer.
/// \param[in] shape  Pointer to an array of dimension extents (length ndim).
/// \param[in] ndim   Number of dimensions (rank). Must be between 1 and 8.
/// \param[in] dtype  Element data type, encoded as an SF_custominiVm_dtype_t value.
/// \return Initialized SF_custominiVm_tensor_t handle wrapping the provided buffer.
MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_tensor_from(void* data, const int32_t* shape,
                                                             int ndim, int dtype);

/// \brief Allocates a new contiguous tensor from the active SFVM Runway nursery heap.
///
/// Allocates sufficient memory for the specified multidimensional shape and element type.
/// Memory is guaranteed to be 64-byte aligned, satisfying AVX-512 and ARM SVE vector constraints.
///
/// \param[in] shape  Array of dimension extents defining the tensor geometry.
/// \param[in] ndim   Rank of the tensor (1 to 8).
/// \param[in] dtype  Element data type (SF_custominiVm_dtype_t).
/// \return Allocated and initialized SF_custominiVm_tensor_t handle.
MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_tensor_alloc(const int32_t* shape, int ndim, int dtype);

/// \brief Explicitly deallocates a tensor and relinquishes its memory to the Falline runway.
///
/// Reclaims the tensor payload buffer immediately, placing the region into the Falline
/// empty-box list for instant reuse. If the tensor is a sub-slice, only view metadata is destroyed.
///
/// \param[in,out] tensor Pointer to the tensor handle to be freed. The handle is zeroed upon return.
MSIMD_API void                    SF_custominiVm_tensor_free(SF_custominiVm_tensor_t* tensor);

/// \brief Allocates an arbitrary raw memory block from the SFVM heap with explicit byte alignment.
///
/// \param[in] bytes      Size of the requested memory allocation in bytes.
/// \param[in] alignment  Requested memory boundary alignment in bytes (e.g. 16, 32, 64, or 128).
/// \return Pointer to the allocated memory block, or NULL if allocation fails.
MSIMD_API void*                   SF_custominiVm_heap_alloc(size_t bytes, size_t alignment);

/// \brief Frees a previously allocated raw memory block.
///
/// \param[in] ptr   Pointer to the memory block returned by SF_custominiVm_heap_alloc().
/// \param[in] bytes Exact byte size of the allocation specified during allocation.
MSIMD_API void                    SF_custominiVm_heap_free(void* ptr, size_t bytes);

/// \brief Returns the total volume of currently active on-heap memory in bytes.
/// \return Total allocated memory in bytes across Runway, Tank, and Landfill arenas.
MSIMD_API size_t                  SF_custominiVm_heap_allocated_bytes(void);

/// \brief Returns the byte size of a tensor element for the specified data type.
/// \param[in] dtype Tensor element data type (SF_custominiVm_dtype_t / sfDType).
/// \return Size in bytes per element (1, 2, 4, 8, 16, or 32).
MSIMD_API size_t                  sf_dtype_size(sfDType dtype);

/// \brief Automatically detects or routes the appropriate sfDType from element size, sign, and float flags.
/// \param[in] byte_size  Element width in bytes (1, 2, 4, 8, 16, 32).
/// \param[in] is_float   Flag indicating floating-point representation.
/// \param[in] is_signed  Flag indicating signed integer representation.
/// \return Resolved sfDType enum value.
MSIMD_API sfDType                 sf_dtype_detect(size_t byte_size, bool is_float, bool is_signed);

/* ========================================================================= */
/* KERNEL REGISTRATION, SYMBOL BINDING & DISPATCH API                        */
/* ========================================================================= */


/// \brief Registers a native compute kernel function with the SFVM dispatch table.
///
/// \param[in] fn_id Unique integer function identifier assigned to the kernel.
/// \param[in] fn    Pointer to the kernel implementation function adhering to SF_custominiVm_kernel_fn_t.
MSIMD_API void SF_custominiVm_register(uint32_t fn_id, SF_custominiVm_kernel_fn_t fn);

/// \brief Binds a symbolic name and sample input signature to a registered kernel identifier.
///
/// Establishes symbolic resolution bindings, enabling JIT compilers and runtime loaders to
/// resolve the kernel by name and perform ahead-of-time type and rank verification.
///
/// \param[in] name           Null-terminated symbolic name of the kernel.
/// \param[in] fn_id          Target numeric kernel function identifier.
/// \param[in] example_input  Array of example input tensors used to deduce shape and dtype signatures.
/// \param[in] n_inputs       Number of tensors in the example_input array.
/// \return MSIMD_SUCCESS (0) on success, or an error status code on signature collision.
MSIMD_API int  SF_custominiVm_bind(const char* name, uint32_t fn_id,
                                   const SF_custominiVm_tensor_t* example_input, int n_inputs);

/// \brief Executes a registered or JIT-compiled kernel with specified input and output tensors.
///
/// Performs high-speed direct dispatch with zero intermediary memory copying. In NORMAX mode,
/// execution checks safepoints for concurrent GC coordination. In SSIOLSC mode, execution occurs
/// with guaranteed deterministic latency.
///
/// \param[in]     fn_id      Registered numeric identifier of the kernel to invoke.
/// \param[in]     inputs     Array of input tensors supplied to the kernel.
/// \param[in]     n_in       Count of input tensors.
/// \param[in,out] outputs    Array of output tensors populated by the kernel.
/// \param[in]     n_out      Count of output tensors.
/// \param[in,out] user_data  Arbitrary user context pointer forwarded to the kernel function.
/// \return MSIMD_SUCCESS (0) on success, or an MSIMD negative error status code on failure.
MSIMD_API int  SF_custominiVm_call(uint32_t fn_id,
                                   const SF_custominiVm_tensor_t* inputs,  int n_in,
                                   SF_custominiVm_tensor_t*       outputs, int n_out,
                                   void*                          user_data);

/* ========================================================================= */
/* ZERO-COPY TENSOR SLICING (SUB-VIEW CREATION)                              */
/* ========================================================================= */


/// \brief Creates a dependent sub-slice (view) of an existing parent tensor without copying data.
///
/// The resulting child tensor references a sub-region within the parent tensor\'s payload buffer,
/// offset by offset_bytes. The child tensor is marked with SF_TENSOR_STATUS_VIEW_SLICE.
/// Memory ownership remains tied to the root ancestor tensor.
///
/// \param[in] parent       Pointer to the parent tensor to slice.
/// \param[in] offset_bytes Byte offset into the parent payload where the sub-slice begins.
/// \param[in] shape        Array of dimension extents defining the shape of the slice.
/// \param[in] ndim         Rank of the child slice tensor (1 to 8).
/// \param[in] dtype        Element data type (must match or be bit-compatible with parent).
/// \return New SF_custominiVm_tensor_t handle representing the dependent sub-slice view.
MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_tensor_slice(const SF_custominiVm_tensor_t* parent, size_t offset_bytes, const int32_t* shape, int ndim, int dtype);

/// \brief Hard-pins a tensor in physical memory, preventing sfGC sweeps, relocation, compaction, or paging.
/// \param[in,out] tensor Pointer to tensor handle to pin.
/// \return 0 on success, negative error code on failure.
MSIMD_API int SF_custominiVm_tensor_pin(SF_custominiVm_tensor_t* tensor);

/// \brief Unpins a previously hard-pinned tensor, restoring standard sfGC lifecycle management.
/// \param[in,out] tensor Pointer to tensor handle to unpin.
/// \return 0 on success, negative error code on failure.
MSIMD_API int SF_custominiVm_tensor_unpin(SF_custominiVm_tensor_t* tensor);

/* ========================================================================= */
/* GENERATIONAL sfGC CONTROL & LIFECYCLE SWEEP API                           */
/* ========================================================================= */
/// \brief Triggers an explicit, synchronous garbage collection sweep across all on-heap generations.
///
/// Sweeps the Motherline, evacuates dead objects to the Landfill, compacts fragmented free space,
/// and updates system telemetry.
MSIMD_API void SF_custominiVm_gc_collect(void);

/// \brief Queries the total committed heap memory in bytes across all active arenas.
/// \return Total committed memory volume in bytes.
MSIMD_API size_t   SF_custominiVm_heap_bytes(void);

/// \brief Returns the lifetime cumulative count of completed GC sweep cycles.
/// \return Monotonically increasing sweep counter.
MSIMD_API uint64_t SF_custominiVm_gc_sweep_count(void);

/// \brief Returns the total volume of memory currently consumed within the Landfill pit in bytes.
/// \return Consumed Landfill memory in bytes.
MSIMD_API size_t   SF_custominiVm_landfill_pit_used(void);

/// \brief Returns the number of active segmented memory panes currently allocated in the Landfill.
/// \return Number of active Landfill panes.
MSIMD_API uint32_t SF_custominiVm_landfill_pane_count(void);

/// \brief Returns the cumulative volume of memory rebated from Falline back to active runway in bytes.
/// \return Cumulative rebated bytes.
MSIMD_API size_t   SF_custominiVm_heap_rebated_bytes(void);

/// \brief Returns the total count of rebate cycles performed by the runway allocator.
/// \return Cumulative rebate cycle count.
MSIMD_API uint32_t SF_custominiVm_heap_rebate_cycles(void);

/* ========================================================================= */
/* UNIFIED MEMORY ANATOMY & SYSTEM METRICS SPECIFICATION                     */
/* ========================================================================= */


/// \struct SF_custominiVm_anatomy_stat_t
/// \brief Comprehensive diagnostic telemetry structure detailing memory distribution across all arenas.
///
/// Provides a unified, complete view of memory allocation, commitment, and fragmentation across
/// On-Heap Runway (Motherline & Falline), Tank regions, Puzzle Arena, Landfill, Off-Heap JIT code
/// heaps, and swap pagefile utilization.
typedef struct SF_custominiVm_anatomy_stat_t {
    /* 1. On-Heap (Garbage-Collected Unified Memory) */
    /// \brief Total virtual memory reserved for on-heap operations in bytes.
    size_t onheap_reserved_bytes;
    /// \brief Physical RAM pages currently committed to on-heap arenas in bytes.
    size_t onheap_committed_bytes;
    /// \brief Total active tensor payload memory currently allocated in bytes.
    size_t onheap_allocated_bytes;

    /* 1.1 Runway Nursery Domain */
    /// \brief Total virtual capacity of the Runway nursery domain in bytes.
    size_t runway_total_bytes;
    /// \brief Memory currently utilized by live objects in the Runway in bytes.
    size_t runway_used_bytes;
    /// \brief Volume of live objects currently resident in the Motherline nursery in bytes.
    size_t motherline_live_bytes;
    /// \brief Volume of empty, reusable memory boxes resident in the Falline in bytes.
    size_t falline_emptybox_bytes;
    /// \brief Count of discrete empty boxes currently linked in the Falline free list.
    size_t emptybox_count;

    /* 1.2 Tank Regions (Humongous Tensors) */
    /// \brief Total memory committed to humongous Tank region allocations in bytes.
    size_t tank_regions_bytes;
    /// \brief Number of discrete humongous Tank regions currently active.
    size_t tank_regions_count;

    /* 1.3 Independent Puzzle Arena */
    /// \brief Total capacity assigned to the Independent Puzzle Arena sandbox in bytes.
    size_t puzzle_arena_total_bytes;
    /// \brief Memory currently consumed by competing candidate benchmarks in bytes.
    size_t puzzle_arena_used_bytes;

    /* 1.4 Landfill Domain (Mature Objects & Evacuation) */
    /// \brief Total capacity of the Landfill arena in bytes.
    size_t landfill_total_bytes;
    /// \brief Consumed memory within the Landfill arena in bytes.
    size_t landfill_used_bytes;
    /// \brief Volume of matured, long-lived tensors surviving multiple generations in bytes.
    size_t matured_arena_bytes;
    /// \brief Volume of memory undergoing rehabilitation prior to reclamation in bytes.
    size_t rehab_arena_bytes;
    /// \brief Count of active segmented memory panes in the Landfill.
    uint32_t landfill_pane_count;

    /* 2. Off-Heap Domain (Non-GC Executable & Telemetry) */
    /// \brief Total capacity of the executable off-heap JIT code arena in bytes.
    size_t offheap_code_arena_bytes;
    /// \brief Total memory consumed by HotLog and ColdLog telemetry ring buffers in bytes.
    size_t offheap_telemetry_bytes;

    /* 3. Dynamic Slab Expansion & Rebating */
    /// \brief Count of dynamically chained slab chunks allocated to meet allocation spikes.
    size_t chained_chunk_count;
    /// \brief Volume of memory successfully returned from Falline to Motherline in bytes.
    size_t rebated_bytes;

    /* 4. Allocation Pacing & Virtual Memory Pagefile Paging */
    /// \brief Total count of allocation pauses (pacing stalls) enforced to prevent heap exhaustion.
    size_t pacing_stall_count;
    /// \brief Total volume of cold tensor memory currently swapped to disk pagefile in bytes.
    size_t paged_out_bytes;
    /// \brief Number of discrete tensor buffers currently resident in the pagefile.
    size_t paged_out_count;
} SF_custominiVm_anatomy_stat_t;

/// \typedef sfAnatomyStat
/// \brief Modern concise alias for SF_custominiVm_anatomy_stat_t.
typedef struct SF_custominiVm_anatomy_stat_t sfAnatomyStat;

/// \brief Queries the detailed memory anatomy of the SFVM runtime engine.
/// \param[out] out_stat Pointer to an anatomy structure populated with current memory telemetry.
MSIMD_API void        SF_custominiVm_anatomy_query(SF_custominiVm_anatomy_stat_t* out_stat);

/// \brief Generates a formatted ASCII diagnostic summary report describing system memory anatomy.
/// \return Pointer to a static, thread-local null-terminated string containing the formatted report.
MSIMD_API const char* SF_custominiVm_anatomy_description(void);

/* ========================================================================= */
/* ADVANCED GC EXECUTION MODES, OWP & MTP CONTROL SPECIFICATION              */
/* ========================================================================= */


/// \enum SF_custominiVm_gc_run_mode_t
/// \brief Operational stepping modes governing execution of the sfGC engine.
typedef enum SF_custominiVm_gc_run_mode_t {
    /// \brief Single Step: Executes exactly one incremental cycle and returns immediately.
    SF_GC_RUN_SINGLE_STEP     = 0,
    /// \brief CONTINUNTIL.FINISH: Executes continuously until all pending dead objects are fully reclaimed.
    SF_GC_RUN_CONTINUE_FINISH = 1,
    /// \brief STEPPING_MTP: Stepping mode evaluating a user-supplied Manual Termination Point (MTP) predicate.
    SF_GC_RUN_STEPPING_MTP    = 2,
} SF_custominiVm_gc_run_mode_t;

/// \typedef sfGcRunMode
/// \brief Modern concise alias for SF_custominiVm_gc_run_mode_t.
typedef SF_custominiVm_gc_run_mode_t sfGcRunMode;

/// \struct SF_custominiVm_env_change_t
/// \brief Environmental telemetry delta transmitted to user MTP predicate functions during GC stepping.
typedef struct SF_custominiVm_env_change_t {
    /// \brief Boolean indicator: 1 if environmental changes occurred during the step, 0 otherwise.
    int      changed;
    /// \brief Net delta of new bytes allocated since the previous stepping iteration.
    uint32_t allocations_detected;
    /// \brief Number of allocation pacing stalls triggered since the previous step.
    uint32_t pacing_stalls_delta;
    /// \brief Number of kernel calls dispatched since the previous step.
    uint32_t calls_delta;
    /// \brief Number of tensor resurrection requests processed since the previous step.
    uint32_t resurrections_delta;
    /// \brief Net change in bytes swapped to or from the disk pagefile.
    uint32_t paged_out_delta;
} SF_custominiVm_env_change_t;

/// \typedef sfEnvChange
/// \brief Modern concise alias for SF_custominiVm_env_change_t.
typedef SF_custominiVm_env_change_t sfEnvChange;

/// \typedef SF_custominiVm_mtp_predicate_fn
/// \brief User-defined Manual Termination Point (MTP) predicate function evaluated during GC stepping.
///
/// \param[in] stat      Snapshot of current system memory anatomy.
/// \param[in] env       Environmental changes observed during the most recent incremental step.
/// \param[in] user_data User context pointer passed to SF_custominiVm_gc_run().
/// \return 1 to terminate the GC run loop, 0 to permit continued stepping.
typedef int (*SF_custominiVm_mtp_predicate_fn)(const SF_custominiVm_anatomy_stat_t* stat,
                                               const SF_custominiVm_env_change_t* env,
                                               void* user_data);

/// \brief Executes an advanced garbage collection loop under the specified stepping mode and constraints.
///
/// \param[in] mode      Execution stepping mode (Single Step, Continue to Finish, or Stepping with MTP).
/// \param[in] owp       One-Way Pacing flag: if non-zero, applies strict backpressure pacing during execution.
/// \param[in] mtp_pred  User termination predicate function evaluated when mode is SF_GC_RUN_STEPPING_MTP.
/// \param[in] user_data Arbitrary user context pointer passed directly to mtp_pred.
/// \return Total volume of memory reclaimed during the execution run in bytes.
MSIMD_API size_t SF_custominiVm_gc_run(SF_custominiVm_gc_run_mode_t mode,
                                       int owp,
                                       SF_custominiVm_mtp_predicate_fn mtp_pred,
                                       void* user_data);

/* ========================================================================= */
/* DETERMINISTIC CONSCIENTIOUS CARETAKER MANIFEST ("SHAKE THE BED & SLEEP")   */
/* ========================================================================= */


/// \struct SF_custominiVm_gc_manifest_t
/// \brief Formal audit manifest generated upon completion of a thorough, deep cleaning GC cycle.
///
/// Provides complete verification telemetry detailing every memory cell inspected, swept,
/// and evacuated, verifying that all trailing debris was cleared and the heap has entered
/// a fully quiescent, predictable state.
typedef struct SF_custominiVm_gc_manifest_t {
    /// \brief Unique sequential sweep identification number.
    uint64_t sweep_id;
    /// \brief Number of generational memory rooms inspected during the pass.
    uint32_t rooms_inspected;
    /// \brief Total count of individual allocation cells examined and swept.
    size_t   cells_swept;
    /// \brief Count of dead object bodies drained and evacuated to the Landfill domain.
    size_t   cells_drained;
    /// \brief Net volume of memory reclaimed and returned to active runways in bytes.
    size_t   bytes_reclaimed;
    /// \brief Total volume of live heap memory remaining active after the sweep in bytes.
    size_t   heap_bytes_after;
    /// \brief Wall-clock duration of the sweeping cycle measured in nanoseconds.
    uint64_t elapsed_ns;
    /// \brief Boolean flag: 1 if all trailing debris and edge allocations were cleared, 0 otherwise.
    int      trailing_debris_cleared;
    /// \brief Boolean flag: 1 if the heap is completely quiescent and collector threads are sleeping.
    int      quiescent;
    /// \brief Human-readable narrative summary detailing sweep outcomes (up to 159 chars + null).
    char     narrative[160];
} SF_custominiVm_gc_manifest_t;

/// \typedef sfGcManifest
/// \brief Modern concise alias for SF_custominiVm_gc_manifest_t.
typedef SF_custominiVm_gc_manifest_t sfGcManifest;

/// \brief Executes a comprehensive, deterministic deep-clean sweep ("Shake the Bed and Sleep").
///
/// Thoroughly inspects all generational rooms (Nursery, Mature, Landfill), drains dead objects,
/// eliminates trailing allocation debris, compacts remaining live objects, and produces a complete
/// audit manifest before placing the collector into a quiescent sleep state.
///
/// \return Completed SF_custominiVm_gc_manifest_t audit record.
MSIMD_API SF_custominiVm_gc_manifest_t SF_custominiVm_gc_shake_bed(void);

/// \brief Retrieves the audit manifest generated by the most recently executed deep-clean sweep.
/// \param[out] out_manifest Pointer to a manifest structure populated with the latest sweep audit.
MSIMD_API void                         SF_custominiVm_gc_get_last_manifest(SF_custominiVm_gc_manifest_t* out_manifest);

/* ========================================================================= */
/* AUTOMATIC GC HEURISTICS & EXECUTION ARCHITECTURE PROFILES                 */
/* ========================================================================= */


/// \enum SF_custominiVm_gc_heuristic_t
/// \brief Autonomous behavioral heuristics governing background sfGC collector scheduling.
typedef enum SF_custominiVm_gc_heuristic_t {
    /// \brief Adaptive: Dynamically adjusts collection wakeups and compaction based on allocation velocity.
    SF_GC_HEURISTIC_ADAPTIVE     = 0,
    /// \brief Aggressive: Enforces tight memory footprint with minimal sleep intervals and frequent compaction.
    SF_GC_HEURISTIC_AGGRESSIVE   = 1,
    /// \brief Bureaucratic: Uncompromising hygiene. Strict STW, exhaustive canary validation, zero tolerance.
    SF_GC_HEURISTIC_BUREAUCRATIC = 2,
    /// \brief Disabled (No-sfGC): Fully disables garbage collection for direct raw disk-sector overwrites.
    SF_GC_HEURISTIC_DISABLED     = 3,
} SF_custominiVm_gc_heuristic_t;

/// \typedef sfGcHeuristic
/// \brief Modern concise alias for SF_custominiVm_gc_heuristic_t.
typedef SF_custominiVm_gc_heuristic_t sfGcHeuristic;

/// \enum SF_custominiVm_gc_execution_mode_t
/// \brief Execution architecture tiers defining memory compaction and collector presence.
typedef enum SF_custominiVm_gc_execution_mode_t {
    /// \brief NORMAX / Standard: Multi-generational dynamic Stop-The-World with concurrent heuristics.
    SF_GC_EXECUTION_DYNAMIC_STW  = 0,
    /// \brief SSIOLSC Tier 1: Phase-STW with mandatory sliding compaction executed strictly between phases.
    SF_GC_EXECUTION_PHASE_STW    = 1,
    /// \brief SSIOLSC Tier 2: No-sfGC direct sector overwrite mode (Zero-GC, ultra-low latency, 50 kHz loops).
    SF_GC_EXECUTION_NO_SFGC      = 2,
} SF_custominiVm_gc_execution_mode_t;

/// \brief Configures the active autonomous garbage collection heuristic model.
/// \param[in] heuristic Selected heuristic policy (Adaptive, Aggressive, Bureaucratic, or Disabled).
MSIMD_API void                          SF_custominiVm_gc_set_heuristic(SF_custominiVm_gc_heuristic_t heuristic);

/// \brief Queries the currently active autonomous garbage collection heuristic model.
/// \return Active heuristic enumeration constant.
MSIMD_API SF_custominiVm_gc_heuristic_t SF_custominiVm_gc_get_heuristic(void);

/// \brief Enables or disables the No-sfGC (Zero-GC) execution mode.
/// \param[in] enabled Set to 1 to disable all GC operations (Zero-GC), or 0 to restore active GC.
MSIMD_API void                          SF_custominiVm_gc_set_no_sfgc(int enabled);

/// \brief Queries whether the engine is currently operating in No-sfGC (Zero-GC) mode.
/// \return 1 if No-sfGC mode is active, 0 if standard GC collection is operating.
MSIMD_API int                           SF_custominiVm_gc_is_no_sfgc(void);

/* ========================================================================= */
/* TWO-STAGE LIFECYCLE INITIALIZATION SPECIFICATION                          */
/* ========================================================================= */


/// \brief Stage 1 Initializer: Discovers hardware topology, sets up virtual memory arenas, and mounts VFS.
///
/// Establishes the foundational memory infrastructure, maps peripheral MMIO ranges, and formats
/// virtual file system structures before any bitcode is ingested.
///
/// \param[in] custom_cfg Optional custom configuration pointer (may be NULL for default discovery).
/// \return MSIMD_SUCCESS (0) on success, or an error status code on failure.
MSIMD_API int SF_custominiVm_setup_start(const void* custom_cfg);

/// \brief Stage 2 Initializer: Ingests embedded bitcode, decompresses code heaps, and boots JIT engine.
///
/// Completes initialization by verifying off-heap JIT code memory, resolving core kernel symbols,
/// and transitioning the engine into the fully active execution state.
///
/// \return MSIMD_SUCCESS (0) on success, or an error status code on bitcode decompression error.
MSIMD_API int SF_custominiVm_start(void);

/// \brief Queries whether Stage 1 initialization (Setup) has been successfully executed.
/// \return 1 if Stage 1 setup is complete, 0 otherwise.
MSIMD_API int SF_custominiVm_is_setup(void);

/// \brief Queries whether Stage 2 initialization (Start) has completed and the VM is actively running.
/// \return 1 if the VM is running and accepting kernel calls, 0 otherwise.
MSIMD_API int SF_custominiVm_is_started(void);

/* ========================================================================= */
/* GEARKIT TRANSMISSION ENGINE & DUAL-PROFILE API SPECIFICATION              */
/* ========================================================================= */
/// \enum SF_custominiVm_scgc_type_t
/// \brief Secondary operational modes for the Safety-Critical No-sfGC execution profile.
///
/// Governs memory access semantics under the SSIOLSC Safety-Critical profile:
/// - **DEFAULT:** Phase-STW Bureaucratic GC. The collector remains active; strict Stop-The-World
///   sweeps and mandatory compaction passes occur exclusively at deterministic phase boundaries.
///   Standard sfLiveHeader metadata chains remain valid.
/// - **PRIMITIVE_RW:** Pure Zero-GC / Raw Sector mode. Garbage collection is completely deactivated.
///   Object headers are bypassed entirely. Allocations yield raw bump-pointer sectors accessed
///   via SF_primitiva_write() and SF_primitiva_read(), offering zero runtime overhead.
typedef enum SF_custominiVm_scgc_type_t {
    /// \brief Phase-STW Bureaucratic GC (SSIOLSC Tier 1: Collector active at phase boundaries).
    SF_SCGC_TYPE_DEFAULT      = 0,
    /// \brief Pure Zero-GC / Raw Sector Mode (SSIOLSC Tier 2: Headers bypassed, direct sector writes).
    SF_SCGC_TYPE_PRIMITIVE_RW = 1,
} SF_custominiVm_scgc_type_t;

/// \typedef sfScgcType
/// \brief Modern concise alias for SF_custominiVm_scgc_type_t.
typedef SF_custominiVm_scgc_type_t sfScgcType;


/* ========================================================================= */
/* SFVM & sfGC 4-POINT EXECUTION COORDINATION                                */
/* (SAFEPOINT / CONTROLPOINT / HOOKPOINT / PAUSEPOINT)                       */
/* ========================================================================= */

/// \enum SF_custominiVm_hookpoint_event_t
/// \brief Lifecycle and diagnostic events dispatched through sf_vm_hookpoint_trigger() / sf_gc_hookpoint_trigger().
typedef enum SF_custominiVm_hookpoint_event_t {
    /// \brief Kernel prologue entry (pins active tensor, records entry RDTSC, tracks NUMA/JIT tier).
    SF_HOOKPOINT_KERNEL_ENTER    = 1,
    /// \brief Kernel epilogue exit (validates tail canary, updates hotness/cycles, unpins tensor).
    SF_HOOKPOINT_KERNEL_EXIT     = 2,
    /// \brief Explicit pre-sweep collector hook notification.
    SF_HOOKPOINT_GC_PRE_SWEEP    = 3,
    /// \brief Explicit post-sweep collector hook notification.
    SF_HOOKPOINT_GC_POST_SWEEP   = 4,
    /// \brief Explicit pre-compaction collector hook notification.
    SF_HOOKPOINT_GC_PRE_COMPACT  = 5,
    /// \brief Explicit post-compaction collector hook notification.
    SF_HOOKPOINT_GC_POST_COMPACT = 6,
    /// \brief Hardware/precision deoptimization event notification.
    SF_HOOKPOINT_DEOPT_TRIGGER   = 7,
} SF_custominiVm_hookpoint_event_t;

typedef SF_custominiVm_hookpoint_event_t sfHookpointEvent;

/// \struct SF_custominiVm_point_telemetry_t
/// \brief Real-time telemetry counters for the 4-point sfGC execution coordination subsystem.
typedef struct SF_custominiVm_point_telemetry_t {
    uint64_t safepoint_polls;
    uint64_t safepoint_rendezvous_stalls;
    uint32_t safepoint_parked_threads;
    uint32_t active_in_flight_kernels;
    uint64_t controlpoint_checks;
    uint64_t controlpoint_pacing_triggers;
    uint64_t controlpoint_auto_pageins;
    uint64_t controlpoint_bytes_validated;
    uint64_t hookpoint_enter_count;
    uint64_t hookpoint_exit_count;
    uint64_t hookpoint_auto_pinned_tensors;
    uint64_t hookpoint_canary_faults_caught;
    uint64_t pausepoint_polls;
    uint64_t pausepoint_preemptions;
} SF_custominiVm_point_telemetry_t;

typedef SF_custominiVm_point_telemetry_t sfPointTelemetry;

/// \brief Polls the cooperative sfGC safepoint.
///
/// Emitted at kernel prologues and JIT loop back-edges. Synchronizes mutator threads
/// with Stop-The-World compaction/evacuation barriers and stream hot-swap gates,
/// refreshing forwarded tensor pointers before resuming execution.
MSIMD_API void sf_gc_safepoint_poll(void);

#ifdef __cplusplus
/// \brief Evaluates working-set allocation controlpoint pressure, page-in state, and SRAM streaming advisory for sfGC.
///
/// \param[in] allocated_bytes Volume of memory newly allocated or touched in the current block (default 0).
/// \return 1 if an allocation pacing stall or proactive GC cycle was triggered, 0 otherwise.
MSIMD_API int  sf_gc_controlpoint_check(size_t allocated_bytes = 0);
#else
MSIMD_API int  sf_gc_controlpoint_check(size_t allocated_bytes);
#endif

/// \brief Triggers a VM/sfGC execution hookpoint at kernel prologue (event_id=1) or epilogue (event_id=2).
///
/// On entry: records thread-local RDTSC timestamp, auto-pins managed output tensor against concurrent
/// E-core/P-core relocation, pages in swapped tensors, records NUMA node locality, and increments
/// tiered JIT invocation counters.
/// On exit: verifies tensor tail canary integrity, records execution cycle delta on the live header
/// and dispatch table, flushes WC buffers if streaming stores were used, and unpins the tensor.
///
/// \param[in] event_id  Hookpoint event type (1 = Enter, 2 = Exit, or SF_custominiVm_hookpoint_event_t).
/// \param[in] fn_id     Target mathematical/algorithmic kernel function ID.
/// \param[in] context   Pointer to active output buffer / tensor payload (may be NULL).
/// \param[in] byte_size Active payload footprint in bytes.
MSIMD_API void sf_vm_hookpoint_trigger(uint32_t event_id, uint32_t fn_id, void* context, size_t byte_size);
MSIMD_API void sf_gc_hookpoint_trigger(uint32_t event_id, uint32_t fn_id, void* context, size_t byte_size);

/// \brief Polls the ultra-low-overhead inner-loop sfGC pausepoint inside unrolled SIMD vector loops.
///
/// Checks for active sfGC Stop-The-World compaction/evacuation rendezvous, Puzzle Arena stream hot-swap gates,
/// user pause gates, and adaptive thermal/current-spike backoff.
///
/// \return 1 if the loop iteration paused/yielded for an sfGC barrier or hot-swap, 0 otherwise.
MSIMD_API int  sf_gc_pausepoint_poll(void);
MSIMD_API int  sf_vm_pausepoint_poll(void);

/// \brief Arms or disarms the global cooperative sfGC pausepoint gate for inner SIMD vector loops.
/// \param[in] armed Non-zero to arm pausepoint preemption gate, 0 to clear.
MSIMD_API void SF_custominiVm_set_pausepoint_gate(int armed);

/// \brief Queries whether the global cooperative sfGC pausepoint gate is currently armed.
/// \return 1 if armed, 0 otherwise.
MSIMD_API int  SF_custominiVm_get_pausepoint_gate(void);

/// \brief Queries real-time telemetry counters for all 4 execution coordination points.
/// \param[out] out_stat Pointer to telemetry structure to populate.
MSIMD_API void SF_custominiVm_point_telemetry_query(SF_custominiVm_point_telemetry_t* out_stat);

/* ========================================================================= */
/* CONTROLPOINT HOST GC COORDINATION & RUNTIME INTEGRATION API               */
/* ========================================================================= */

/// \enum SF_custominiVm_host_gc_mode_t
/// \brief Host Runtime VM Cooperative GC Modes.
/// Controls how SFVM coordinates with external language runtimes (Java JVM, .NET CLR, etc.).
typedef enum SF_custominiVm_host_gc_mode_t {
    SF_HOST_GC_AUTONOMOUS         = 0, ///< SFVM manages sfGC autonomously without waiting for host VM
    SF_HOST_GC_COOPERATIVE_SYNC   = 1, ///< SFVM coordinates with host VM: notifies controlpoint hooks before/after GC sweeps and pacing
    SF_HOST_GC_DELEGATED_HOST_LEAD= 2  ///< SFVM disables internal automatic timer sweeps and delegates GC pacing to host application
} SF_custominiVm_host_gc_mode_t;

/// \enum SF_custominiVm_controlpoint_action_t
/// \brief Directives dispatched from SFVM controlpoint to registered host language bindings.
typedef enum SF_custominiVm_controlpoint_action_t {
    SF_CP_ACTION_NONE              = 0,
    SF_CP_ACTION_PACING_STALL      = 1, ///< Heap pressure threshold reached: host binding should throttle allocation velocity
    SF_CP_ACTION_PAUSE_MUTATORS    = 2, ///< STW compaction / Pitstop swap active: host binding should pause registered mutator threads
    SF_CP_ACTION_RESUME_MUTATORS   = 3, ///< Compaction / Pitstop swap complete: host binding may resume paused threads
    SF_CP_ACTION_REQUEST_HOST_GC   = 4  ///< SFVM requests host runtime to trigger its own GC (e.g. System.gc()) to collect dormant handles
} SF_custominiVm_controlpoint_action_t;

/// \typedef sf_controlpoint_hook_fn_t
/// \brief Callback invoked by SFVM controlpoint to manage host language runtime threads and GC state.
/// \param[in] action    Directives from SFVM (e.g. pause mutator threads, resume, pace, or host GC request).
/// \param[in] param     Associated parameter (e.g. pacing stall duration, heap pressure bytes).
/// \param[in] user_data User context registered with the hook.
typedef void (*sf_controlpoint_hook_fn_t)(SF_custominiVm_controlpoint_action_t action, uint64_t param, void* user_data);

/// \brief Registers a host runtime controlpoint hook for multi-language VM coordination.
MSIMD_API void SF_custominiVm_register_controlpoint_hook(sf_controlpoint_hook_fn_t hook, void* user_data);

/// \brief Unregisters the active host runtime controlpoint hook.
MSIMD_API void SF_custominiVm_unregister_controlpoint_hook(void);

/// \brief Sets the host GC cooperative mode.
MSIMD_API void SF_custominiVm_set_host_gc_cooperative_mode(int mode);

/// \brief Queries the active host GC cooperative mode.
MSIMD_API int  SF_custominiVm_get_host_gc_cooperative_mode(void);

/// \brief Explicitly dispatches a controlpoint action directive to the registered host hook.
MSIMD_API void SF_custominiVm_trigger_host_controlpoint(SF_custominiVm_controlpoint_action_t action, uint64_t param);

/* ========================================================================= */
/* SF::GroupCompute() & ALU SATURATION / RMRP / THREAD BUDGET CONTROLS       */
/* ========================================================================= */

/// \struct sf_simd_ladder_slot_t
/// \brief Represents a candidate SIMD ISA execution entry in an object's branched SIMD table.
typedef struct sf_simd_ladder_slot_t {
    char     name[16];
    uint32_t ladder_level;
    uint8_t  is_active;
    uint8_t  _pad[3];
} sf_simd_ladder_slot_t;

/// \struct SF_custominiVm_group_params_t
/// \brief Parameters for high-level GroupCompute operations.
typedef struct SF_custominiVm_group_params_t {
    uint8_t priority;      ///< 0 - 10 ALU Queue Priority
    uint8_t bypass_warmup; ///< 1 = Bypass Yapbozcu warmup (latency-critical), 0 = normal
    uint8_t _pad[2];
} SF_custominiVm_group_params_t;

/// \brief Issues an SFVM GroupCompute transaction and returns a unique 32-bit hex group ID.
MSIMD_API uint32_t SF_custominiVm_group_compute(uint8_t priority, int bypass_warmup);

/// \brief Resets thread-local GroupCompute parameters to defaults.
MSIMD_API void SF_custominiVm_group_compute_reset(void);

/// \brief Begins a scoped batch processing block for grouping MathventageSIMD mathematical operations.
/// All tensors allocated and kernels dispatched within this block inherit the specified priority
/// and warmup policy under a unique batch group ID.
/// \param[in] priority ALU queue priority (0 to 10). High priority (>= 8) overrides queue dispatching.
/// \param[in] bypass_warmup 1 to bypass Yapbozcu optimization warmup (latency-critical), 0 for standard adaptive mode.
/// \return Assigned 32-bit unique hexadecimal batch group identifier (0xXXXXXXXX).
MSIMD_API uint32_t SF_custominiVm_batch_begin(uint8_t priority, int bypass_warmup);

/// \brief Finalizes and commits the currently active scoped batch processing block.
MSIMD_API void     SF_custominiVm_batch_end(void);

/// \brief Returns the active batch group identifier, or 0 if no batch processing block is open.
MSIMD_API uint32_t SF_custominiVm_get_current_batch_id(void);

/// \enum SF_custominiVm_emu_schema_t
/// \brief Multi-limb silicon emulation schemas from Sections 14-15 of Machines-Rules.td.
/// When primary native ALU lanes are saturated under Allow_Operation_Dispatchings,
/// high-precision and standard tensors are partitioned across compatible multi-limb execution banks.
typedef enum SF_custominiVm_emu_schema_t {
    SF_EMU_SCHEMA_NONE         = 0x00, ///< Native hardware ALU execution (no emulation)
    
    /* Float Multi-Limb Schemas (Section 14) */
    SF_EMU_SCHEMA_FLOAT_2X8    = 0x01, ///< 16-bit Float via 2x 8-bit limbs
    SF_EMU_SCHEMA_FLOAT_4X8    = 0x02, ///< 32-bit Float via 4x 8-bit limbs
    SF_EMU_SCHEMA_FLOAT_2X16   = 0x03, ///< 32-bit Float via 2x 16-bit limbs
    SF_EMU_SCHEMA_FLOAT_4X16   = 0x04, ///< 64-bit Float via 4x 16-bit limbs
    SF_EMU_SCHEMA_FLOAT_2X32   = 0x05, ///< 64-bit Float via 2x 32-bit limbs
    SF_EMU_SCHEMA_FLOAT_4X32   = 0x06, ///< 128-bit Float via 4x 32-bit limbs
    SF_EMU_SCHEMA_FLOAT_2X64   = 0x07, ///< 128-bit Float via 2x 64-bit limbs (Double-Double)
    SF_EMU_SCHEMA_FLOAT_4X64   = 0x08, ///< 256-bit Float via 4x 64-bit limbs (Quad-Double)
    SF_EMU_SCHEMA_FLOAT_8X32   = 0x09, ///< 256-bit Float via 8x 32-bit limbs (Oct-Single)

    /* Integer Multi-Limb Schemas (Section 14) */
    SF_EMU_SCHEMA_INT_2X8      = 0x11, ///< 16-bit Integer via 2x 8-bit limbs
    SF_EMU_SCHEMA_INT_4X8      = 0x12, ///< 32-bit Integer via 4x 8-bit limbs
    SF_EMU_SCHEMA_INT_2X16     = 0x13, ///< 32-bit Integer via 2x 16-bit limbs
    SF_EMU_SCHEMA_INT_4X16     = 0x14, ///< 64-bit Integer via 4x 16-bit limbs
    SF_EMU_SCHEMA_INT_2X32     = 0x15, ///< 64-bit Integer via 2x 32-bit limbs
    SF_EMU_SCHEMA_INT_4X32     = 0x16, ///< 128-bit Integer via 4x 32-bit limbs
    SF_EMU_SCHEMA_INT_2X64     = 0x17, ///< 128-bit Integer via 2x 64-bit limbs (Int128)
    SF_EMU_SCHEMA_INT_4X64     = 0x18, ///< 256-bit Integer via 4x 64-bit limbs (Int256)
    SF_EMU_SCHEMA_INT_8X32     = 0x19, ///< 256-bit Integer via 8x 32-bit limbs
} SF_custominiVm_emu_schema_t;

/// \brief Enables or disables dynamic operation dispatching across ALU execution queues.
MSIMD_API void SF_custominiVm_set_allow_operation_dispatchings(int allow);
MSIMD_API int  SF_custominiVm_get_allow_operation_dispatchings(void);

/// \brief Dispatches an operation to an ALU execution queue.
/// If Allow_Operation_Dispatchings is enabled and preferred ALU is saturated,
/// dynamically dispatches to an unsaturated ALU queue unless priority >= 8.
/// \return Dispatched ALU lane index.
MSIMD_API int SF_custominiVm_alu_dispatch_op(uint32_t preferred_alu, uint8_t priority);

/// \brief Completes an operation on the specified ALU execution queue.
MSIMD_API void SF_custominiVm_alu_complete_op(uint32_t alu_id);

/// \brief Queries total operations dynamically dispatched away from saturated ALU queues.
MSIMD_API uint64_t SF_custominiVm_get_alu_dispatched_count(void);

/// \brief Resolves execution route for a tensor: checks native ALU lane depth, and if saturated
/// and Allow_Operation_Dispatchings is enabled, steers to a compatible multi-limb emulation bank.
/// \param[in]  tensor          Pointer to tensor handle.
/// \param[in]  priority        ALU queue priority (0-10).
/// \param[out] out_is_emulated Pointer receiving 1 if routed via emulation, 0 if native.
/// \param[out] out_schema_id   Pointer receiving SF_custominiVm_emu_schema_t ID.
/// \return Assigned ALU execution lane index.
MSIMD_API int SF_custominiVm_alu_resolve_tensor_route(const SF_custominiVm_tensor_t* tensor, uint8_t priority,
                                                      int* out_is_emulated, int* out_schema_id);

/// \brief Resolves execution route by data type directly.
MSIMD_API int SF_custominiVm_alu_resolve_route_by_dtype(uint8_t dtype, uint8_t priority,
                                                        int* out_is_emulated, int* out_schema_id);

/// \brief Returns human-readable descriptor for a multi-limb emulation schema.
MSIMD_API const char* SF_custominiVm_get_emulation_schema_name(int schema_id);

/// \brief Queries total operations dispatched to multi-limb software emulation banks.
MSIMD_API uint64_t SF_custominiVm_get_alu_emulation_dispatched_count(void);

/// \brief Configures Reduce Memory Resident Propagation (RMRP).
/// \param enable 1 to enable RMRP on-heap compaction, 0 to disable.
/// \param method 0 for PLUMP (Prune + coalesce), 1 for SEQUENTIAL (Installment).
MSIMD_API void SF_custominiVm_set_rmrp(int enable, int method);
MSIMD_API int  SF_custominiVm_get_rmrp(void);

/// \brief Manually triggers an RMRP (Reduce Memory Resident Propagation) on-heap compaction pass.
/// \return Total bytes decommitted to OS or rebated into internal Sadaka pool.
MSIMD_API size_t SF_custominiVm_run_rmrp(void);

/// \brief Queries object distribution across Runway Motherline Egg, Fledgling, and Pitstop zones.
/// \param[out] out_egg Count of objects in Egg zone (unoptimized).
/// \param[out] out_fledgling Count of objects in Fledgling zone (optimized/verified).
/// \param[out] out_pitstop Count of objects currently excised into Pitstop.
MSIMD_API void SF_custominiVm_runway_query_zones(size_t* out_egg, size_t* out_fledgling, size_t* out_pitstop);

/// \brief Excises an object into Pitstop, updates its active ISA intrinsic, and returns it to Fledgling.
/// Uses in-place Empty Box placeholder without compacting runway memory.
/// \param[in] tensor_payload Managed tensor buffer pointer.
/// \param[in] new_isa_name Name of new active intrinsic candidate (e.g. "AVX512", "AVX2").
/// \param[in] cycles_saved CPU cycles gained after optimization.
/// \return 0 on success, negative error code on failure.
MSIMD_API int  SF_custominiVm_pitstop_test_cycle(void* tensor_payload, const char* new_isa_name, uint64_t cycles_saved);

/// \enum SF_custominiVm_yapbozcu_morph_t
/// \brief Morph and excision paradigms for Yapbozcu JIT/ISA live hot-patching.
typedef enum SF_custominiVm_yapbozcu_morph_e {
    SF_YAPBOZCU_MORPH_EXCISION = 0, ///< Mode 1: Traditional excision; slot becomes Empty Box/Busy tombstone; callers get BUSY or wait/callback.
    SF_YAPBOZCU_MORPH_SHADOW   = 1  ///< Mode 2: Zero-pause shadow copy; live object stays in Motherline, atomic in-place overwrite upon completion.
} SF_custominiVm_yapbozcu_morph_t;

/// \brief Callback type for asynchronous object ready notifications (Mode 1).
typedef void (*sf_object_ready_callback_t)(void* obj_ptr, void* user_data);

/// \brief Sets the Yapbozcu morph method (0 = Excision, 1 = Shadow).
MSIMD_API void SF_custominiVm_set_yapbozcu_morph_method(int method);

/// \brief Gets the active Yapbozcu morph method (0 = Excision, 1 = Shadow).
MSIMD_API int  SF_custominiVm_get_yapbozcu_morph_method(void);

/// \brief Checks whether a tensor/object is currently busy (being morphed/optimized in Pitstop).
/// \return 1 if busy/optimizing, 0 if live and ready.
MSIMD_API int  SF_custominiVm_is_object_busy(const void* tensor_payload);

/// \brief Synchronously blocks calling thread until an object completes Yapbozcu optimization in Pitstop.
/// \param tensor_payload Pointer to tensor data or live object.
/// \param timeout_ms Maximum time in milliseconds to wait (0 = infinite).
/// \return 0 on success (object ready), -1 on timeout or invalid.
MSIMD_API int  SF_custominiVm_await_object_ready(void* tensor_payload, uint32_t timeout_ms);

/// \brief Registers an asynchronous notification callback to be invoked when an object completes Yapbozcu optimization.
MSIMD_API int  SF_custominiVm_register_object_ready_callback(void* tensor_payload, sf_object_ready_callback_t callback, void* user_data);


/// \struct SF_custominiVm_live_meta_t
/// \brief Diagnostic introspection snapshot of an allocated tensor's Live Header.
typedef struct SF_custominiVm_live_meta_t {
    uint32_t function_id;
    uint32_t group_hex_id;
    uint8_t  group_priority;
    uint8_t  opt_permitted;
    uint8_t  has_optimized;
    uint8_t  opt_success;
    uint16_t active_simd_idx;
    char     active_isa[32];
    uint64_t cycles_gained;
} SF_custominiVm_live_meta_t;

/// \brief Queries live header metadata for an allocated tensor.
/// \return 0 on success, negative error code on failure.
MSIMD_API int  SF_custominiVm_tensor_query_live_meta(const SF_custominiVm_tensor_t* tensor, SF_custominiVm_live_meta_t* out_meta);

/// \brief Assigns a function ID to an allocated tensor for batch optimization grouping.
MSIMD_API void SF_custominiVm_tensor_set_function_id(SF_custominiVm_tensor_t* tensor, uint32_t fn_id);

/// \brief Marks a tensor for rehabilitation upon the next GC sweep.
MSIMD_API void SF_custominiVm_tensor_mark_rehab(SF_custominiVm_tensor_t* tensor);

/// \brief Requests resurrection for all tensors belonging to the specified function ID from Rehab Arena.
MSIMD_API void SF_custominiVm_request_resurrection(uint32_t fn_id);

/// \brief Manually scans the Rehab Arena and processes all pending resurrection requests.
/// \return Number of tensors physically resurrected and restored to Motherline Egg zone.
MSIMD_API size_t SF_custominiVm_scan_rehab_arena(void);

/// \brief Returns the total cumulative count of tensor physical resurrections from Rehab Arena.
MSIMD_API uint64_t SF_custominiVm_get_resurrection_count(void);

/// \brief Looks up an active tensor by its function ID in the Motherline.
MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_find_tensor_by_function_id(uint32_t fn_id);

/// \brief C-compatible branch prediction director hint.
/// \param[in] condition Logical branch condition.
/// \param[in] pattern BranchPattern (0 = Uniform, 1 = Chaotic, 2 = Bias Likely, 3 = Bias Unlikely).
/// \return Evaluated condition with BPU compiler prediction hints.
MSIMD_API int  SF_custominiVm_branch_predict(int condition, int pattern);

/// \enum SF_custominiVm_hardware_tier_t
/// \brief Dynamic hardware classification tier for adaptive thread budgeting and resource allocation.
typedef enum SF_custominiVm_hardware_tier_t {
    SF_HW_TIER_ALT        = 0, ///< Alt / Giriş / Embedded / SBC (2-4 cores, e.g. Raspberry Pi, Atom, Low-power)
    SF_HW_TIER_ORTA_ALT   = 1, ///< Orta-Alt (Lower-Midrange / Laptop, 4C/8T, 5-8 threads)
    SF_HW_TIER_ORTA_ORTA  = 2, ///< Orta-Orta (Midrange Desktop, 6C/12T - 14T, e.g. Ryzen 5 5600X, Core i5-13400F, Ultra 5 245K)
    SF_HW_TIER_ORTA_UST   = 3, ///< Orta-Üst (High-end Desktop, 15-32 threads, e.g. Core i9/i7, Ryzen 9/7)
    SF_HW_TIER_SUR        = 4  ///< Sür / Extreme (HPC/Server/Workstation, >32 cores, e.g. Threadripper/EPYC/Xeon)
} SF_custominiVm_hardware_tier_t;

/// \brief Queries recommended adaptive thread budget tuned for the underlying CPU topology.
MSIMD_API void SF_custominiVm_query_thread_budget(uint32_t* out_gc, uint32_t* out_puzzle, uint32_t* out_vm);

/// \brief Queries the detected hardware tier classification.
MSIMD_API SF_custominiVm_hardware_tier_t SF_custominiVm_query_hardware_tier(void);

/// \brief Returns human-readable name of the detected hardware tier.
MSIMD_API const char* SF_custominiVm_query_hardware_tier_name(void);

/// \brief Resolves a natural English technical intent token or fuzzy CLI argument to its canonical SFVM flag and value.
/// \param[in] input_token The raw input token (e.g. "boost", "shrink", "vector", "steal", "safety", "sram").
/// \param[out] out_canonical_flag Buffer to receive canonical flag name (e.g. "Upclocker").
/// \param[in] flag_buf_sz Size of out_canonical_flag buffer.
/// \param[out] out_canonical_value Buffer to receive suggested canonical value (e.g. "1").
/// \param[in] val_buf_sz Size of out_canonical_value buffer.
/// \return 1 if successfully resolved, 0 if unrecognized.
MSIMD_API int SF_custominiVm_cli_resolve_intent(const char* input_token,
                                                char* out_canonical_flag, size_t flag_buf_sz,
                                                char* out_canonical_value, size_t val_buf_sz);

/// \struct SF_custominiVm_self_heal_stats_t
/// \brief Telemetry counters tracking SFGC self-healing and memory corruption recovery operations.
typedef struct SF_custominiVm_self_heal_stats_t {
    uint64_t heap_corruptions_healed;
    uint64_t brooks_cycles_broken;
    uint64_t header_magics_repaired;
    uint64_t emptybox_chains_rebuilt;
    uint64_t bitcode_exceptions_shielded;
    uint64_t code_errors_captured;
} SF_custominiVm_self_heal_stats_t;

/// \brief Manually triggers an active self-healing sweep across runway heap and object graphs.
/// \return Number of corrupted structures/pointers successfully healed during the sweep.
MSIMD_API uint64_t SF_custominiVm_self_heal_run_check(void);

/// \brief Queries cumulative self-healing statistics and recovered corruption counters.
MSIMD_API void SF_custominiVm_self_heal_query_stats(SF_custominiVm_self_heal_stats_t* out_stats);

/// \brief Captures and logs a code execution error in SFVM.
/// Matches: "SFVM captured 'A' error at 'B' piece of code!"
MSIMD_API void SF_custominiVm_self_heal_capture_code_error(const char* location, const char* detail);

/// \brief Captures and shields a JIT/embedded bitcode exception in SFVM.
/// Matches: "SFVM captured 'C' exception in embedded bitcode!"
MSIMD_API void SF_custominiVm_self_heal_capture_bitcode_exception(uint32_t fn_id, const char* section);

/// \brief Test & diagnostic hook: deliberately injects corruption into tensor header for self-heal testing.
MSIMD_API void SF_custominiVm_diagnostic_corrupt_tensor(SF_custominiVm_tensor_t* tensor, uint8_t bad_gen, int make_cyclic_fwd);

/// \brief Flushes modified SRAM cache lines to DRAM with minimal latency without invalidating L1/L2 cache.
MSIMD_API void SF_custominiVm_sram_commit_to_dram(void* ptr, size_t bytes);

/// \brief Issues a global hardware store-fence making updated SRAM cache lines visible to core siblings.
MSIMD_API void SF_custominiVm_sram_broadcast_volatility(void* ptr, size_t bytes);

/// \brief Streams tensor payload directly to DRAM bypassing SRAM cache hierarchy (non-temporal stores).
MSIMD_API void SF_custominiVm_dram_stream_store(void* dst, const void* src, size_t bytes);

/// \brief Gets the active global volatility method (0 = DRAM, 1 = SRAM).
MSIMD_API int SF_custominiVm_get_global_volatility(void);

/// \brief Sets the global volatility method (0 = DRAM, 1 = SRAM).
MSIMD_API void SF_custominiVm_set_global_volatility(int method);

/// \brief Sets the Inline Assembly (iasm) execution safety threshold (0..10).
/// When task priority < threshold, SFVM uses safe Ring 3 OS APIs and standard memory barriers.
/// When task priority >= threshold, SFVM engages direct hardware inline asm and non-temporal streaming stores.
MSIMD_API void     SF_custominiVm_set_iasm_threshold(uint32_t threshold);

/// \brief Gets the current Inline Assembly execution safety threshold (0..10, default = 5).
MSIMD_API uint32_t SF_custominiVm_get_iasm_threshold(void);

/// \brief Enables or disables strict Ring 3 mode. If enabled, inline assembly and non-temporal streaming are strictly forbidden regardless of priority.
MSIMD_API void     SF_custominiVm_set_strict_ring3(int enable);

/// \brief Returns 1 if strict Ring 3 mode is active, 0 otherwise.
MSIMD_API int      SF_custominiVm_is_strict_ring3(void);

/// \brief Checks whether Inline Assembly and hardware streaming are currently unlocked for the given priority level (0..10).
MSIMD_API int      SF_custominiVm_is_iasm_active(uint32_t priority);

/// \brief Configures strict flag validation mode (default: false).
/// If enabled, any unrecognized flag, unresolved conflict, or unsupported flag
/// causes an immediate fail-fast termination with full diagnostic telemetry.
MSIMD_API void     SF_custominiVm_set_strict_flags(int enable);

/// \brief Returns 1 if strict flag validation mode is active, 0 otherwise.
MSIMD_API int      SF_custominiVm_is_strict_flags(void);

/// \brief Direct OS thread pinning to a logical core.
/// \note Usage is not generally recommended; intended for internal or specialized tuning to avoid OS scheduler quirks.
MSIMD_API int SF_custominiVm_thread_pin_to_core(int logical_cpu);

/// \brief Direct OS thread pinning to a NUMA node.
/// \note Usage is not generally recommended; intended for internal or specialized tuning to avoid OS scheduler quirks.
MSIMD_API int SF_custominiVm_thread_pin_to_numa_node(uint32_t numa_node);

/// \brief Direct OS thread priority elevation (THREAD_PRIORITY_HIGHEST / SCHED_FIFO).
/// \note Usage is not generally recommended; intended for internal or specialized tuning to avoid OS scheduler quirks.
MSIMD_API int SF_custominiVm_thread_set_priority_high(void);

/// \brief Branchless conditional move hinters to prevent BPU speculative execution misprediction on chaotic patterns.
MSIMD_API int32_t sf_hinter_cmov_i32(int condition, int32_t val_true, int32_t val_false, const void* precision_target);
MSIMD_API int64_t sf_hinter_cmov_i64(int condition, int64_t val_true, int64_t val_false, const void* precision_target);

/// \brief Drains buffered hotlog entries into persistent coldlog storage.
/// \return Number of log events successfully migrated to coldlog.
MSIMD_API size_t SF_custominiVm_log_boot_drain(void);

/* ========================================================================= */
/* SFVM TELEMETRY & EVENT STREAMING SPECIFICATION                           */
/* ========================================================================= */

/// \enum SF_custominiVm_event_category_e
/// \brief Comprehensive categorical event classifications across all SFVM subsystems.
typedef enum SF_custominiVm_event_category_e {
    /* ── 0x01..0x10: Legacy / Core Events ── */
    SF_Ebin0_ALLOC                  = 0x01, ///< General memory allocation
    SF_Ebin0_FREE                   = 0x02, ///< General memory free / cell release
    SF_Ebin0_PROMOTE                = 0x03, ///< General generational promotion
    SF_Ebin0_DEMOTE                 = 0x04, ///< General generational demotion
    SF_Ebin0_TO_FALLINE             = 0x05, ///< Falline emptybox eviction
    SF_Ebin0_TO_LANDFILL            = 0x06, ///< Landfill mortuary migration
    SF_Ebin0_EMPTYBOX               = 0x07, ///< EmptyBox recycling event
    SF_Ebin0_LANDFILL_EVAC          = 0x08, ///< Landfill flush evacuation
    SF_Ebin0_HOTSWAP                = 0x09, ///< Dynamic kernel hotswap
    SF_Ebin0_HOTSWAP_FAIL           = 0x0A, ///< Kernel hotswap rejected
    SF_Ebin0_GC_SWEEP               = 0x0B, ///< Generational sweep cycle
    SF_Ebin0_GC_COMPACT             = 0x0C, ///< Sliding compaction cycle
    SF_Ebin0_GC_ALIGN               = 0x0D, ///< Region alignment / migration
    SF_Ebin0_SAFEPOINT              = 0x0E, ///< Safepoint synchronization poll
    SF_Ebin0_HOOK                   = 0x0F, ///< Hookpoint ISA probe event
    SF_Ebin0_ERROR                  = 0x10, ///< Critical error or panic event

    /* ── 0x20..0x2F: Runway & Allocator Subsystem ── */
    SF_Ebin0_ALLOC_BUMP             = 0x20, ///< Runway bump-pointer advance
    SF_Ebin0_ALLOC_FREELIST         = 0x21, ///< Slab freelist cell reuse
    SF_Ebin0_ALLOC_CHUNK_CHAIN      = 0x22, ///< Chained OS virtual chunk allocated (64MB)
    SF_Ebin0_ALLOC_REBATE           = 0x23, ///< Idle virtual pages decommitted to OS
    SF_Ebin0_ALLOC_PACE_STALL       = 0x24, ///< Allocation pacing backpressure pause
    SF_Ebin0_MOTHERLINE_EGG_ADMIT   = 0x25, ///< Unoptimized object admitted to Egg
    SF_Ebin0_MOTHERLINE_FLEDGLING   = 0x26, ///< Optimized object promoted to Fledgling
    SF_Ebin0_PITSTOP_EXCISE         = 0x27, ///< Object excised to Pitstop with EmptyBox slot
    SF_Ebin0_PITSTOP_SHADOW         = 0x28, ///< Object cloned into Shadow RCU mode in Pitstop
    SF_Ebin0_PITSTOP_RETURN         = 0x29, ///< Morphed object returned to Motherline
    SF_Ebin0_PITSTOP_AWAIT_STALL    = 0x2A, ///< Mutator thread stalled awaiting object readiness
    SF_Ebin0_PITSTOP_CALLBACK       = 0x2B, ///< Asynchronous object readiness callback fired
    SF_Ebin0_FALLINE_SNATCH_HIT     = 0x2C, ///< Reusable EmptyBox found and snatched
    SF_Ebin0_FALLINE_SNATCH_MISS    = 0x2D, ///< EmptyBox snatch missed, bump fallback taken
    SF_Ebin0_FALLINE_SPLIT          = 0x2E, ///< Oversized EmptyBox split into remnant
    SF_Ebin0_FALLINE_COALESCE       = 0x2F, ///< Contiguous EmptyBoxes merged

    /* ── 0x30..0x3F: 4-Point Execution Topology ── */
    SF_Ebin0_SAFEPOINT_RENDEZVOUS   = 0x30, ///< Mutator parked at safepoint rendezvous
    SF_Ebin0_SAFEPOINT_RELEASE      = 0x31, ///< Mutator released from safepoint rendezvous
    SF_Ebin0_HOOKPOINT_ENTER        = 0x32, ///< Kernel invocation prologue hook
    SF_Ebin0_HOOKPOINT_EXIT         = 0x33, ///< Kernel epilogue execution metrics hook
    SF_Ebin0_CONTROLPOINT_AUDIT     = 0x34, ///< Working set and heap pressure inspection
    SF_Ebin0_CONTROLPOINT_PAGEIN    = 0x35, ///< Automatic disk pagefile fetch at controlpoint
    SF_Ebin0_PAUSEPOINT_HALT        = 0x36, ///< Cooperative pausepoint thread halt
    SF_Ebin0_INTERRUPT_GATE_SET     = 0x37, ///< Asynchronous interrupt gate toggled

    /* ── 0x40..0x4F: sfGC Engine Lifecycle ── */
    SF_Ebin0_GC_PHASE_CHANGE        = 0x40, ///< GC lifecycle phase transition
    SF_Ebin0_ROOM_PROMOTE_YOUNG_ADOL= 0x41, ///< Young -> Adolescent room promotion
    SF_Ebin0_ROOM_PROMOTE_ADOL_SEN  = 0x42, ///< Adolescent -> Senior room promotion
    SF_Ebin0_REGION_MIGRATE         = 0x43, ///< Region migration (Demand/Middle/Tank/Hotfield)
    SF_Ebin0_RMRP_SPAN_TRIM         = 0x44, ///< RMRP marginal span compaction trim
    SF_Ebin0_RMRP_COALESCE_DECOMMIT = 0x45, ///< RMRP compacted block released to OS
    SF_Ebin0_RESURRECT_REQUEST      = 0x46, ///< Evicted tensor resurrection requested
    SF_Ebin0_RESURRECT_RESTORED     = 0x47, ///< Tensor resurrected from Rehab Arena
    SF_Ebin0_RESURRECT_QUEUE_DROP   = 0x48, ///< Resurrection queue overflow drop
    SF_Ebin0_LANDFILL_CRITICAL_WARN = 0x49, ///< Landfill capacity exceeded critical threshold
    SF_Ebin0_SLICEVIEW_SALVAGED     = 0x4A, ///< Orphaned slice view cloned into independent cell

    /* ── 0x50..0x5F: ALU & Execution Focus Scheduler ── */
    SF_Ebin0_ALU_DISPATCH_DIRECT    = 0x50, ///< Kernel dispatched to preferred ALU lane
    SF_Ebin0_ALU_SATURATE_STEAL     = 0x51, ///< ALU queue saturated, kernel stolen by peer
    SF_Ebin0_GROUP_COMPUTE_BEGIN    = 0x52, ///< GroupCompute execution transaction began
    SF_Ebin0_GROUP_COMPUTE_RESET    = 0x53, ///< GroupCompute execution transaction finalized
    SF_Ebin0_CORE_PIN_PERF          = 0x54, ///< Thread hard-pinned to Performance Core
    SF_Ebin0_CORE_UNPIN_ANY         = 0x55, ///< Thread unpinned to OS scheduler pool
    SF_Ebin0_UPCLOCKER_BOOST_ON     = 0x56, ///< Upclocker turbo frequency boost engaged
    SF_Ebin0_UPCLOCKER_BOOST_OFF    = 0x57, ///< Upclocker turbo frequency boost disengaged
    SF_Ebin0_FOCUS_MODE_SWITCH      = 0x58, ///< Execution focus mode switched (SCP/MCP/DUAL)

    /* ── 0x60..0x6F: SRAM & Volatility ── */
    SF_Ebin0_SRAM_PREWARM_START     = 0x60, ///< L1/L2 SRAM cacheline pre-warming engaged
    SF_Ebin0_STORE_FENCE_BROADCAST  = 0x61, ///< Inter-core store fence volatility broadcast
    SF_Ebin0_STREAMING_STORE_ENGAGE = 0x62, ///< Non-temporal streaming store engaged (L3 bypass)
    SF_Ebin0_STREAMING_STORE_FLUSH  = 0x63, ///< Write-combining write buffers flushed
    SF_Ebin0_SCRATCHPAD_ALLOC       = 0x64, ///< Zero-latency on-chip scratchpad buffer allocated
    SF_Ebin0_SRAM_COMMIT_DRAM       = 0x65, ///< SRAM cachelines committed to DRAM

    /* ── 0x70..0x7F: JIT & Entangle (Kernel Fusion) ── */
    SF_Ebin0_BITCODE_DECOMPRESS_OK  = 0x70, ///< Bitcode payload successfully decompressed
    SF_Ebin0_ORCJIT_MODULE_ADDED    = 0x71, ///< LLVM IR module registered in OrcJIT
    SF_Ebin0_ORCJIT_RESOLVE_SUCCESS = 0x72, ///< JIT function symbol successfully resolved
    SF_Ebin0_ENTANGLE_TX_BEGIN      = 0x73, ///< Batch register entangle transaction opened
    SF_Ebin0_ENTANGLE_TX_COMMIT     = 0x74, ///< Batch register entangle transaction committed
    SF_Ebin0_LLVM_PASS_INJECTED     = 0x75, ///< SFVM execution interlock passes injected

    /* ── 0x80..0x8F: Self-Healing & Reliability ── */
    SF_Ebin0_HEAL_HEADER_REPAIRED   = 0x80, ///< Corrupted object header fields restored
    SF_Ebin0_HEAL_BROOKS_SEVERED    = 0x81, ///< Cyclic or dangling Brooks pointer severed
    SF_Ebin0_HEAL_CANARY_QUARANTINE = 0x82, ///< Canary guard failure; object quarantined
    SF_Ebin0_HEAL_NUMERIC_DIVERGE   = 0x83, ///< Numeric NaN/Inf divergence detected
    SF_Ebin0_HEAL_BITCODE_EXCEPTION = 0x84, ///< JIT kernel error caught; fallback activated

    /* ── 0x90..0x9F: VFS & Storage / Pagefile Paging ── */
    SF_Ebin0_VFS_MKDIR_SUCCESS      = 0x90, ///< VFS directory created with optional quota
    SF_Ebin0_VFS_BIND_TENSOR        = 0x91, ///< Tensor payload bound to VFS path
    SF_Ebin0_VFS_UNLINK_TENSOR      = 0x92, ///< Tensor payload unlinked from VFS
    SF_Ebin0_VFS_EVICT_TREE         = 0x93, ///< VFS folder tree evacuated
    SF_Ebin0_PAGEFILE_PAGEOUT       = 0x94, ///< Cold tensor swapped out to disk pagefile
    SF_Ebin0_PAGEFILE_PAGEIN        = 0x95, ///< Swapped tensor paged back into physical RAM
    SF_Ebin0_HOST_GC_INTERCEPT      = 0x96, ///< Host (JVM/CLR/Python) GC notification captured
} SF_custominiVm_event_category_e;

typedef SF_custominiVm_event_category_e sacculus_event_t;
typedef SF_custominiVm_event_category_e sfEvent;

/// \enum SF_custominiVm_severity_e
/// \brief Severity classification levels for telemetry log records.
typedef enum SF_custominiVm_severity_e {
    SF_Ebin1_TRACE = 0, ///< Granular nanosecond execution trace
    SF_Ebin1_INFO  = 1, ///< Informational system transition
    SF_Ebin1_WARN  = 2, ///< Non-fatal degradation or warning
    SF_Ebin1_ERROR = 3, ///< Recoverable execution error
    SF_Ebin1_FATAL = 4, ///< Fatal system fault or unrecoverable panic
} SF_custominiVm_severity_e;

typedef SF_custominiVm_severity_e sacculus_severity_t;
typedef SF_custominiVm_severity_e sfSeverity;

/// \struct SF_custominiVm_hotlog_entry_t
/// \brief Real-time 64-byte telemetry event record isolated to a single CPU cacheline.
typedef struct SF_custominiVm_hotlog_entry_t {
    uint64_t  ts_ns;
    uint64_t  tsc;
    uint64_t  gc_sweep_no;
    uint32_t  fn_id;
    uint32_t  heap_used_kb;
    uint16_t  slot;
    uint16_t  current_core;
    uint8_t   generation;
    uint8_t   region;
    uint8_t   event;
    uint8_t   severity;
    char      msg[22];
    uint8_t   ready;
    uint8_t   _pad1;
} SF_custominiVm_hotlog_entry_t;

typedef SF_custominiVm_hotlog_entry_t sfHotlogEntry;

/// \struct SF_custominiVm_coldlog_entry_t
/// \brief Historical 64-byte telemetry event record with an expanded message buffer.
typedef struct SF_custominiVm_coldlog_entry_t {
    uint64_t  ts_ns;
    uint32_t  fn_id;
    uint8_t   event;
    uint8_t   severity;
    uint8_t   ready;
    uint8_t   _pad1;
    char      msg[48];
} SF_custominiVm_coldlog_entry_t;

typedef SF_custominiVm_coldlog_entry_t sfColdlogEntry;

/// \enum SF_custominiVm_gc_hook_event_e
/// \brief Lifecycle hook events for external runtime garbage collection callbacks.
typedef enum SF_custominiVm_gc_hook_event_e {
    SF_GC_HOOK_PRE_MARK         = 0x00,
    SF_GC_HOOK_POST_MARK        = 0x01,
    SF_GC_HOOK_PRE_SWEEP        = 0x02,
    SF_GC_HOOK_POST_SWEEP       = 0x03,
    SF_GC_HOOK_PRE_COMPACT      = 0x04,
    SF_GC_HOOK_POST_COMPACT     = 0x05,
    SF_GC_HOOK_EMERGENCY_EVAC   = 0x06,
    SF_GC_HOOK_STW_REQUEST      = 0x07,
    SF_GC_HOOK_STW_RELEASE      = 0x08,
} SF_custominiVm_gc_hook_event_e;

typedef void (*SF_custominiVm_gc_hook_fn_t)(
    SF_custominiVm_gc_hook_event_e event,
    uint64_t sweep_no,
    size_t heap_used_bytes,
    void* user_data
);

typedef void (*sf_telemetry_stream_fn_t)(
    const SF_custominiVm_hotlog_entry_t* entry,
    const char* formatted_line,
    void* user_data
);

MSIMD_API void   SF_custominiVm_hotlog_drain(void (*cb)(const SF_custominiVm_hotlog_entry_t*, void*), void* user);
MSIMD_API void   SF_custominiVm_coldlog_drain(void (*cb)(const SF_custominiVm_coldlog_entry_t*, void*), void* user);
MSIMD_API void   SF_custominiVm_register_gc_hook(SF_custominiVm_gc_hook_fn_t hook, void* user_data);
MSIMD_API int    SF_custominiVm_telemetry_stream_start(uint32_t flush_interval_us, sf_telemetry_stream_fn_t callback, void* user_data);
MSIMD_API void   SF_custominiVm_telemetry_stream_stop(void);
MSIMD_API int    SF_custominiVm_telemetry_stream_is_running(void);
MSIMD_API size_t SF_custominiVm_hotlog_format_cli(const SF_custominiVm_hotlog_entry_t* entry, char* buffer, size_t buffer_size);
MSIMD_API size_t SF_custominiVm_hotlog_format_json(const SF_custominiVm_hotlog_entry_t* entry, char* buffer, size_t buffer_size);
MSIMD_API size_t SF_custominiVm_coldlog_format_cli(const SF_custominiVm_coldlog_entry_t* entry, char* buffer, size_t buffer_size);
MSIMD_API size_t SF_custominiVm_coldlog_format_json(const SF_custominiVm_coldlog_entry_t* entry, char* buffer, size_t buffer_size);
MSIMD_API void   SF_custominiVm_set_log_filter_mask(uint32_t subsystem_mask);
MSIMD_API uint32_t SF_custominiVm_get_log_filter_mask(void);

/// \typedef sf_raw_log_stream_fn_t
/// \brief Universal raw log stream callback for host language bindings and IDE integration.
/// Receives all live ANSI colored, hexadecimal-tagged logs, warnings, notices, panics, and telemetry events.
/// \param[in] raw_ansi_text Null-terminated text formatted with ANSI colors and hex metadata.
/// \param[in] text_length   Length of text in bytes.
/// \param[in] user_data     Host-supplied context pointer.
typedef void (*sf_raw_log_stream_fn_t)(const char* raw_ansi_text, size_t text_length, void* user_data);

/// \brief Registers a real-time raw log stream listener.
/// Language bindings (Java/JNI, C#, etc.) subscribe to this hook to display SFVM logs inside IDE consoles.
MSIMD_API void SF_custominiVm_register_raw_log_stream(sf_raw_log_stream_fn_t stream_fn, void* user_data);

/// \brief Unregisters the active raw log stream listener.
MSIMD_API void SF_custominiVm_unregister_raw_log_stream(void);

/// \brief Controls whether logs are mirrored to the standard OS console (stdout/stderr).
/// Default is enabled (1). Set to 0 if the host application exclusively handles log display via raw stream hook.
MSIMD_API void SF_custominiVm_set_console_mirroring(int enable);

/// \brief Queries whether console mirroring is active.
MSIMD_API int  SF_custominiVm_get_console_mirroring(void);

/// \brief Explicitly emits a custom formatted log line through the SFVM log broadcasting pipeline.
MSIMD_API void SF_custominiVm_emit_raw_log(const char* formatted_text);

/// \brief Checks whether Batch Register & Entangle Execution mode is active.
MSIMD_API int SF_custominiVm_entangle_is_active(void);

/// \brief Begins a fused batch register entangle transaction.
MSIMD_API void SF_custominiVm_entangle_begin(void);

/// \brief Commits a fused batch register entangle transaction.
MSIMD_API void SF_custominiVm_entangle_commit(void);
MSIMD_API void SF_custominiVm_set_entangle_execution(int enable);
MSIMD_API int  SF_custominiVm_get_entangle_execution(void);


/// \struct SF_custominiVm_code_heap_stat_t
/// \brief Telemetry metrics detailing the off-heap JIT code arena state and compilation counters.
typedef struct SF_custominiVm_code_heap_stat_t {
    /// \brief Total byte capacity of the immutable rawcode cell storage.
    size_t rawcode_capacity;
    /// \brief Consumed memory volume within the rawcode storage in bytes.
    size_t rawcode_used_bytes;
    /// \brief Boolean flag: 1 if rawcode cells are sealed with PAGE_READONLY protection.
    int    rawcode_is_immutable;

    /// \brief Total byte capacity of processed executable code chunks.
    size_t processed_chunk_capacity;
    /// \brief Consumed byte volume of compiled executable code chunks.
    size_t processed_chunk_used_bytes;
    /// \brief Boolean flag: 1 if processed code pages are sealed with PAGE_EXECUTE_READ (W^X).
    int    processed_chunk_is_sealed;

    /// \brief Active JIT tier level (1 = Vector Ahead-of-Time, 2 = Micro-JIT, 3 = Puzzle Arena).
    uint32_t active_jit_tier;
    /// \brief Total cumulative count of kernels compiled and resident in the code heap.
    uint32_t compiled_kernel_count;
    /// \brief Total count of deoptimization events triggered due to dynamic type/rank mismatches.
    uint32_t deopt_event_count;
} SF_custominiVm_code_heap_stat_t;

/// \typedef sfCodeHeapStat
/// \brief Modern concise alias for SF_custominiVm_code_heap_stat_t.
typedef SF_custominiVm_code_heap_stat_t sfCodeHeapStat;

/// \brief Queries operational metrics and memory distribution of the off-heap JIT code arena.
/// \param[out] out_stat Pointer to a code heap telemetry structure populated by the query.
MSIMD_API void SF_custominiVm_code_heap_query(SF_custominiVm_code_heap_stat_t* out_stat);

/// \brief Engages the NORMAX standard high-throughput HPC execution gear profile.
///
/// Enables dynamic slab chunk chaining, adaptive background concurrent garbage collection,
/// and aggressive SIMD vector speculation across heterogeneous processor cores.
///
/// \return MSIMD_SUCCESS (0) on success, or an error status code.
MSIMD_API int SacFor_Gearkit_NORMAX(void);

/// \brief Engages the default SSIOLSC Safety-Critical deterministic execution gear profile.
///
/// Switches to Phase-STW Bureaucratic GC (ISO 26262 ASIL-D compliant). Forbids runtime dynamic
/// memory expansion and restricts compaction strictly to deterministic phase boundaries.
///
/// \return MSIMD_SUCCESS (0) on success, or an error status code.
MSIMD_API int SacFor_Gearkit_SSIOLSC(void);

/// \brief Engages the SSIOLSC Safety-Critical profile with an explicit secondary SCGC operational mode.
///
/// \param[in] scgc_type Secondary SCGC mode (Phase-STW Bureaucratic GC or Pure Zero-GC PrimitiveRW).
/// \return MSIMD_SUCCESS (0) on success, or an error status code.
MSIMD_API int SacFor_Gearkit_SSIOLSC_Ex(SF_custominiVm_scgc_type_t scgc_type);

/// \brief Queries the currently engaged Gearkit transmission profile mode.
/// \return MSIMD_PROFILE_STANDARD (NORMAX) or MSIMD_PROFILE_SAFETY_CRITICAL (SSIOLSC).
MSIMD_API msimd_profile_mode_t SacFor_Gearkit_GetMode(void);

/* ========================================================================= */
/* SPECIALIZED HARDWARE ENGINE SUBSYSTEMS (HYBRID, NUMA, SMT/HT, SRAM)       */
/* ========================================================================= */

/// \enum SF_custominiVm_cpu_topology_class_t
/// \brief Classification of CPU microarchitecture core symmetry.
typedef enum SF_custominiVm_cpu_topology_class_t {
    SF_CPU_TOPOLOGY_HOMOGENEOUS   = 0, ///< Uniform Symmetric Multi-Processing (all cores identical)
    SF_CPU_TOPOLOGY_HYBRID_INTEL  = 1, ///< Intel Hybrid: Golden/Raptor Cove (P) + Gracemont/Skymont (E)
    SF_CPU_TOPOLOGY_HYBRID_ARM    = 2, ///< ARM big.LITTLE / DynamIQ: Cortex-X/A7x (P) + Cortex-A5x (E)
    SF_CPU_TOPOLOGY_HYBRID_APPLE  = 3  ///< Apple Silicon: Firestorm/Avalanche (P) + Icestorm/Blizzard (E)
} SF_custominiVm_cpu_topology_class_t;

/// \enum SF_custominiVm_core_tier_t
/// \brief Target core performance tier for thread affinity steering.
typedef enum SF_custominiVm_core_tier_t {
    SF_CORE_TIER_ANY         = 0, ///< OS scheduler discretion; any available logical core
    SF_CORE_TIER_PERFORMANCE = 1, ///< Bind strictly to high-frequency, wide-vector Performance cores (P-cores)
    SF_CORE_TIER_EFFICIENCY  = 2  ///< Bind strictly to low-power Efficient cores (E-cores) for GC/telemetry/IO
} SF_custominiVm_core_tier_t;

/// \enum SF_custominiVm_mem_architecture_t
/// \brief Physical memory topology and interconnect architecture.
typedef enum SF_custominiVm_mem_architecture_t {
    SF_MEM_ARCH_UMA             = 0, ///< Unified Memory Architecture: CPU/GPU zero-copy pool, uniform latency
    SF_MEM_ARCH_NUMA_SYMMETRIC  = 1, ///< Symmetric multi-socket / multi-CCD NUMA topology
    SF_MEM_ARCH_NUMA_ASYMMETRIC = 2  ///< Heterogeneous NUMA (CXL memory tiers, hybrid HBM+DDR domains)
} SF_custominiVm_mem_architecture_t;

/// \enum SF_custominiVm_smt_mode_t
/// \brief Simultaneous Multi-Threading (SMT / Hyper-Threading) operational state.
typedef enum SF_custominiVm_smt_mode_t {
    SF_SMT_MODE_DISABLED_OR_ABSENT = 0, ///< 1 logical thread per physical execution core (no sibling contention)
    SF_SMT_MODE_2WAY               = 1, ///< 2-way SMT (Intel Hyper-Threading, AMD SMT2)
    SF_SMT_MODE_4WAY_PLUS          = 2  ///< 4-way+ SMT (IBM POWER, specialized hardware)
} SF_custominiVm_smt_mode_t;

/// \enum SF_custominiVm_workload_affinity_policy_t
/// \brief Execution thread pool affinity steering policy.
typedef enum SF_custominiVm_workload_affinity_policy_t {
    SF_AFFINITY_PHYSICAL_CORES_ONLY = 0, ///< Compute/SIMD: 1 thread per physical core (prevents SMT port collisions)
    SF_AFFINITY_ALL_LOGICAL_THREADS = 1, ///< Memory-bound/IO: All logical threads (maximizes SMT latency hiding)
    SF_AFFINITY_ISOLATE_P_CORES     = 2  ///< Hybrid CPU: Confines execution strictly to physical P-cores
} SF_custominiVm_workload_affinity_policy_t;

/// \enum SF_custominiVm_sram_regime_t
/// \brief On-chip SRAM and hierarchical cache geometry regime.
typedef enum SF_custominiVm_sram_regime_t {
    SF_SRAM_REGIME_STANDARD_HIERARCHICAL = 0, ///< Standard multi-level hierarchical SRAM (L1d, L2, L3)
    SF_SRAM_REGIME_3D_VCACHE             = 1, ///< Massive LLC (96MB+ L3): massive working set in-cache residency
    SF_SRAM_REGIME_SCRATCHPAD_TCM        = 2, ///< Embedded on-chip Scratchpad SRAM / Tightly Coupled Memory
    SF_SRAM_REGIME_WIDE_CACHELINE_128B   = 3  ///< 128-byte primary cachelines (Apple Silicon, IBM POWER)
} SF_custominiVm_sram_regime_t;

/// \enum SF_custominiVm_execution_focus_t
/// \brief SFVM New Era Execution Focus Topology (Single-Core, Multi-Core, Asymmetric P+E Dual Performance).
typedef enum SF_custominiVm_execution_focus_t {
    SF_FOCUS_AUTO            = 0, ///< Autonomous detection via TableGen silicon profiles and CPUID topology
    SF_FOCUS_SCP_HOMO        = 1, ///< Single-Core Focus on Homogeneous CPU (Peak Turbo locking, core-floating)
    SF_FOCUS_SCP_HYBRID      = 2, ///< Single-Core Focus on Asymmetric CPU (Strict P-Core pinning, E-Core escape forbidden)
    SF_FOCUS_MCP_HOMO        = 3, ///< Multi-Core Focus on Homogeneous CPU (Uniform N/threads partitioning, massive AVX/NUMA)
    SF_FOCUS_MCP_HYBRID      = 4, ///< Multi-Core Focus on Asymmetric CPU (Weighted Chunking: 2.5x P-Core, 1.0x E-Core)
    SF_FOCUS_DUAL_HOMO       = 5, ///< Dual Single+Multi Focus Homogeneous (1 Turbo Master SIMD + N-1 high-throughput workers)
    SF_FOCUS_DUAL_HYBRID     = 6, ///< Dual Single+Multi Focus Asymmetric (P-Core compute mutator + E-Core background sfGC/ColdLog)

    // Shorthand category / macro aliases
    SF_FOCUS_SCP             = 10, ///< Generic Single-Core Performance (auto-resolves to HOMO or HYBRID)
    SF_FOCUS_MCP             = 11, ///< Generic Multi-Core Performance (auto-resolves to HOMO or HYBRID)
    SF_FOCUS_HYBRID_DUAL     = 12, ///< Generic Dual Single+Multi Performance (auto-resolves to HOMO or HYBRID)
    SF_FOCUS_DUAL            = 12, ///< Alias for SF_FOCUS_HYBRID_DUAL

    // Legacy / descriptive aliases
    SF_FOCUS_SCP_HOMOGENEOUS  = 1,
    SF_FOCUS_SCP_ASYMMETRIC   = 2,
    SF_FOCUS_MCP_HOMOGENEOUS  = 3,
    SF_FOCUS_MCP_ASYMMETRIC   = 4,
    SF_FOCUS_DUAL_HOMOGENEOUS = 5,
    SF_FOCUS_DUAL_ASYMMETRIC  = 6
} SF_custominiVm_execution_focus_t;

/// \brief Modern concise alias for SF_custominiVm_execution_focus_t.
typedef SF_custominiVm_execution_focus_t sfExecutionFocus;

/// \brief Checks whether the processor possesses a hybrid (heterogeneous) core architecture.
/// \return 1 if hybrid (P/E cores), 0 if homogeneous.
MSIMD_API int SF_custominiVm_is_hybrid_cpu(void);

/// \brief Queries the specific CPU core symmetry classification.
/// \return CPU topology class enum.
MSIMD_API SF_custominiVm_cpu_topology_class_t SF_custominiVm_cpu_topology_class(void);

/// \brief Checks whether the host system features a Non-Uniform Memory Access (NUMA) architecture.
/// \return 1 if multi-node NUMA, 0 if UMA / single uniform domain.
MSIMD_API int SF_custominiVm_is_numa(void);

/// \brief Queries the memory architecture classification.
/// \return Memory architecture enum.
MSIMD_API SF_custominiVm_mem_architecture_t SF_custominiVm_mem_architecture(void);

/// \brief Checks whether Simultaneous Multi-Threading (Hyper-Threading / SMT) is actively enabled.
/// \return 1 if SMT active, 0 if disabled or absent.
MSIMD_API int SF_custominiVm_is_ht_active(void);

/// \brief Queries the active SMT concurrency mode.
/// \return SMT mode enum.
MSIMD_API SF_custominiVm_smt_mode_t SF_custominiVm_smt_mode(void);

/// \brief Queries the SRAM and cache architectural regime.
/// \return SRAM regime enum.
MSIMD_API SF_custominiVm_sram_regime_t SF_custominiVm_sram_regime(void);

/// \brief Calculates optimal worker thread concurrency based on workload nature and CPU/SMT topology.
/// \param[in] is_compute_heavy If non-zero, sizes for vector/SIMD avoiding SMT port sharing.
/// \return Optimal thread worker count.
MSIMD_API uint32_t SF_custominiVm_optimal_concurrency(int is_compute_heavy);

/// \brief Binds the calling thread to a specific core performance tier (P-core vs E-core).
/// \param[in] tier Target tier (Performance, Efficiency, or Any).
/// \return 1 on success, 0 on failure or unsupported.
MSIMD_API int SF_custominiVm_bind_calling_thread(SF_custominiVm_core_tier_t tier);

/// \brief Applies a system-wide execution affinity policy to the calling thread.
/// \param[in] policy Target affinity policy.
/// \return 1 on success, 0 on failure.
MSIMD_API int SF_custominiVm_apply_affinity_policy(SF_custominiVm_workload_affinity_policy_t policy);

/// \brief Allocates an ultra-low latency, zero-page-fault scratch buffer from on-chip SRAM / scratchpad memory.
/// \param[in] bytes Allocation size in bytes.
/// \param[in] alignment Byte alignment boundary (default 64).
/// \return Pointer to aligned scratchpad memory, or nullptr if exhausted.
MSIMD_API void* SF_custominiVm_sram_scratchpad_alloc(size_t bytes, size_t alignment);

/// \brief Resets the on-chip SRAM scratchpad bump allocator, reclaiming all scratch memory in O(1).
MSIMD_API void SF_custominiVm_sram_scratchpad_reset(void);

/// \brief Allocates memory explicitly bound to a specific physical NUMA node domain.
/// \param[in] bytes Allocation size in bytes.
/// \param[in] node Target NUMA node ID (0-based).
/// \return Pointer to node-local memory, or nullptr on failure.
MSIMD_API void* SF_custominiVm_numa_alloc(size_t bytes, uint8_t node);

/// \brief Releases memory previously allocated via SF_custominiVm_numa_alloc.
/// \param[in] ptr Memory buffer pointer.
/// \param[in] bytes Allocation size in bytes.
/// \param[in] node NUMA node ID where memory was hosted.
MSIMD_API void SF_custominiVm_numa_free(void* ptr, size_t bytes, uint8_t node);

/* ========================================================================= */
/* SFVM NEW ERA EXECUTION FOCUS TOPOLOGY (SCP, MCP, HYBRID_DUAL 6-WAY MATRIX)*/
/* ========================================================================= */

/// \brief Queries the actively configured SFVM execution focus mode.
/// \return Resolved active focus mode (SF_FOCUS_SCP_HOMO, SF_FOCUS_SCP_HYBRID, SF_FOCUS_MCP_HOMO, etc.).
MSIMD_API SF_custominiVm_execution_focus_t SF_custominiVm_get_execution_focus(void);

/// \brief Explicitly sets the SFVM execution focus mode, reconfiguring scheduling, chunking and affinity.
/// \param[in] focus Target focus enum (category constant or explicit 6-way orthogonal mode).
MSIMD_API void SF_custominiVm_set_execution_focus(SF_custominiVm_execution_focus_t focus);

/// \brief Returns human-readable mnemonic identifier of the execution focus mode.
/// \param[in] focus Target focus enum.
/// \return Static C-string name (e.g. "SCP_HOMO", "SCP_HYBRID", "MCP_HOMO", "MCP_HYBRID", "DUAL_HOMO", "DUAL_HYBRID", "AUTO").
MSIMD_API const char* SF_custominiVm_get_execution_focus_name(SF_custominiVm_execution_focus_t focus);

/// \brief Automatically detects the optimal execution focus based on TableGen silicon microarchitecture and topology.
/// \return Detected execution focus mode.
MSIMD_API SF_custominiVm_execution_focus_t SF_custominiVm_detect_cpu_execution_focus(void);

/// \brief Retrieves the matched silicon microarchitecture profile name from Machines-Rules.td.
/// \return Static string representing the microarchitecture (e.g., "Intel Lion Cove", "AMD Zen 5", "ARM Neoverse N1").
MSIMD_API const char* SF_custominiVm_get_focus_uarch_name(void);

/// \brief Queries the P-Core and E-Core workload chunking weight factors under the active focus mode.
/// \param[out] out_p_weight Pointer to receive P-Core chunk weight factor (e.g., 2.5 on hybrid, 1.0 on homo).
/// \param[out] out_e_weight Pointer to receive E-Core chunk weight factor (e.g., 1.0 on hybrid, 1.0 on homo).
MSIMD_API void SF_custominiVm_get_focus_chunk_weights(double* out_p_weight, double* out_e_weight);

/// \brief Returns non-zero if the active execution focus is any Single-Core Performance (SCP) variant.
MSIMD_API int SF_custominiVm_is_focus_scp(void);

/// \brief Returns non-zero if the active execution focus is any Multi-Core Performance (MCP) variant.
MSIMD_API int SF_custominiVm_is_focus_mcp(void);

/// \brief Returns non-zero if the active execution focus is any Dual Single+Multi Performance variant.
MSIMD_API int SF_custominiVm_is_focus_dual(void);

/* ========================================================================= */
/* SFVM HARDWARE TOPOLOGY, DISPATCHER & INTRINSIC ACCELERATION APIS          */
/* ========================================================================= */
#ifndef MSIMD_PARALLEL_FOR_FN_DEFINED
#define MSIMD_PARALLEL_FOR_FN_DEFINED
typedef void (*msimd_parallel_for_fn)(void* user_data, size_t start, size_t end);
#endif

MSIMD_API void           SF_custominiVm_query_hw_topology(msimd_hw_topology_t* out_topo);
MSIMD_API void           SF_custominiVm_parallel_for(size_t total, msimd_parallel_for_fn fn, void* user_data);
MSIMD_API void           SF_custominiVm_prefetch_t0(const void* addr);
MSIMD_API void           SF_custominiVm_prefetch_t1(const void* addr);
MSIMD_API void           SF_custominiVm_prefetch_t2(const void* addr);
MSIMD_API void           SF_custominiVm_prefetch_nta(const void* addr);
MSIMD_API void           SF_custominiVm_prefetch_stream(const void* base, size_t count, size_t stride);
MSIMD_API void           SF_custominiVm_stream_store_f32(float* dst, const float* src, size_t count);
MSIMD_API void           SF_custominiVm_stream_store_f64(double* dst, const double* src, size_t count);
MSIMD_API void           SF_custominiVm_fence_full(void);
MSIMD_API void           SF_custominiVm_fence_store(void);
MSIMD_API void           SF_custominiVm_fence_load(void);
MSIMD_API void           SF_custominiVm_memcpy_nt(void* dst, const void* src, size_t size);
MSIMD_API void           SF_custominiVm_memzero_nt(void* dst, size_t size);
MSIMD_API uint64_t       SF_custominiVm_rdtsc_begin(void);
MSIMD_API uint64_t       SF_custominiVm_rdtsc_end(void);
MSIMD_API uint64_t       SF_custominiVm_rdtsc_freq_hz(void);
MSIMD_API double         SF_custominiVm_cycles_to_ns(uint64_t cycles, uint64_t freq_hz);
MSIMD_API int            SF_custominiVm_thread_pin_to_core(int logical_cpu);
MSIMD_API int            SF_custominiVm_thread_pin_to_numa_node(uint32_t numa_node);
MSIMD_API int            SF_custominiVm_thread_set_priority_high(void);
MSIMD_API msimd_status_t SF_custominiVm_calibrate_pipeline(msimd_context ctx, int* out_best_level, double* out_gflops);
MSIMD_API void           SF_custominiVm_upclocker_set_boost(int enable);
MSIMD_API int            SF_custominiVm_upclocker_is_boosted(void);
MSIMD_API void           SF_custominiVm_upclocker_warmup(uint32_t duration_us);

/* ========================================================================= */
/* VIRTUAL FILE SYSTEM (VFS) & STRICT BRANCH VALIDATION API                  */
/* ========================================================================= */

#ifndef SF_VFS_STAT_DEFINED
#define SF_VFS_STAT_DEFINED
/// \struct SF_custominiVm_vfs_stat_t
/// \brief Status and quota telemetry for a node within the SFVM Virtual File System (VFS).
typedef struct SF_custominiVm_vfs_stat_t {
    /// \brief Unique virtual inode number assigned to the VFS node.
    uint64_t inode_no;
    /// \brief Total size of the underlying tensor payload in bytes (0 for directories).
    size_t   size_bytes;
    /// \brief Maximum memory quota assigned to this folder branch in bytes (0 = unlimited).
    size_t   quota_bytes;
    /// \brief Total volume of memory currently consumed by tensors bound in this folder in bytes.
    size_t   used_bytes;
    /// \brief Number of child directories and bound tensors residing in this node.
    uint32_t child_count;
    /// \brief Boolean flag: 1 if this node represents a directory, 0 if it is a tensor file.
    uint8_t  is_dir;
    /// \brief Structure alignment padding bytes.
    uint8_t  _pad[3];
} SF_custominiVm_vfs_stat_t;

/// \typedef sfVfsStat
/// \brief Modern concise alias for SF_custominiVm_vfs_stat_t.
typedef struct SF_custominiVm_vfs_stat_t sfVfsStat;
#endif

#ifndef SF_VFS_PANIC_HOOK_DEFINED
#define SF_VFS_PANIC_HOOK_DEFINED
/// \typedef sf_vfs_panic_hook_t
/// \brief Callback function prototype invoked upon VFS panic before termination.
typedef void (*sf_vfs_panic_hook_t)(const char* missing_path, const char* operation_context, void* user_data);
#endif

/// \brief Creates a virtual directory node with an optional memory quota in the VFS heap.
MSIMD_API int         SF_custominiVm_folder_mkdir(const char* path, size_t quota_bytes);

/// \brief Binds an existing tensor handle to a virtual path within the VFS directory hierarchy.
MSIMD_API int         SF_custominiVm_folder_bind(const char* path, SF_custominiVm_tensor_t* tensor);

/// \brief Evicts all tensors residing within the specified virtual directory path.
MSIMD_API size_t      SF_custominiVm_folder_evict(const char* path);

/// \brief Queries the total memory volume currently consumed by tensors within a virtual directory.
MSIMD_API size_t      SF_custominiVm_folder_used_bytes(const char* path);

/// \brief Creates a virtual directory hierarchy in the VFS with an assigned memory quota.
MSIMD_API int         SF_custominiVm_vfs_mkdir(const char* path, size_t quota_bytes);

/// \brief Allocates a new tensor and binds it immediately to a specified VFS path.
MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_vfs_alloc_tensor(const char* path, const int32_t* shape, int ndim, int dtype);

/// \brief Unlinks and removes a bound tensor file from the VFS hierarchy.
MSIMD_API int         SF_custominiVm_vfs_unlink(const char* path);

/// \brief Recursively removes a virtual directory and all child tensors and folders within it.
MSIMD_API size_t      SF_custominiVm_vfs_rmdir_rf(const char* path);

/// \brief Queries status and quota telemetry for a specific node in the VFS.
MSIMD_API int         SF_custominiVm_vfs_stat(const char* path, SF_custominiVm_vfs_stat_t* out_stat);

/// \brief Creates an in-memory snapshot image of the VFS tree structure rooted at the given path.
MSIMD_API size_t      SF_custominiVm_vfs_snapshot(const char* path, void* out_buf, size_t buf_cap);

/// \brief Enumerates child nodes residing within a specified virtual directory path.
MSIMD_API void        SF_custominiVm_vfs_ls(const char* path, void (*cb)(const char* name, int is_dir, size_t bytes, void* user), void* user);

/// \brief Enables or disables strict VFS branch validation mode (default: enabled).
/// When enabled, any operation or lookup on a non-existent VFS branch triggers an immediate safe crash/panic.
MSIMD_API void        SF_custominiVm_set_strict_vfs_branch_validation(int enable);

/// \brief Queries whether strict VFS branch validation is currently active.
MSIMD_API int         SF_custominiVm_get_strict_vfs_branch_validation(void);

/// \brief Alias for SF_custominiVm_get_strict_vfs_branch_validation.
MSIMD_API int         SF_custominiVm_is_strict_vfs_branch_validation(void);

/// \brief Controls whether diagnostic modal alert dialogs are spawned on fatal VFS panics (default: enabled on desktop).
MSIMD_API void        SF_custominiVm_vfs_set_panic_dialog_enabled(int enable);

/// \brief Queries whether diagnostic modal alert dialogs are enabled on fatal VFS panics.
MSIMD_API int         SF_custominiVm_vfs_get_panic_dialog_enabled(void);

/// \brief Sets panic behavior flags for testing and headless execution.
MSIMD_API void        SF_custominiVm_vfs_set_panic_behavior(int suppress_abort, int show_dialog);

/// \brief Registers an optional callback hook to be notified when a VFS panic occurs.
MSIMD_API void        SF_custominiVm_vfs_set_panic_hook(sf_vfs_panic_hook_t hook, void* user_data);

/// \brief Manually triggers an immediate safe VFS panic with diagnostic logging, alert window, and fail-fast termination.
MSIMD_API void        SF_custominiVm_vfs_trigger_panic(const char* missing_path, const char* operation_context);

/// \brief Returns cumulative count of VFS panic events triggered during runtime lifetime.
MSIMD_API uint64_t    SF_custominiVm_vfs_get_panic_count(void);

/// \brief Returns the most recent missing VFS branch path that caused a panic.
MSIMD_API const char* SF_custominiVm_vfs_get_last_panic_path(void);

/// \brief Returns the operation context of the most recent VFS panic.
MSIMD_API const char* SF_custominiVm_vfs_get_last_panic_op(void);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus

/// \struct SF_Gearkit_Model
/// \brief Ergonomic modern C++ interface for dynamic operational profile shifting.
///
/// Provides static methods to switch between the NORMAX (HPC / Dynamic GC) profile
/// and the SSIOLSC (Safety-Critical / Deterministic GC) execution profiles.
struct SF_Gearkit_Model {
    /// \brief Switches the engine to the NORMAX high-throughput HPC profile.
    /// \return MSIMD_SUCCESS (0) on success, or an error status code.
    static inline int NORMAX() {
        return SacFor_Gearkit_NORMAX();
    }

    /// \brief Switches the engine to the default SSIOLSC Safety-Critical profile (Phase-STW Bureaucratic).
    /// \return MSIMD_SUCCESS (0) on success, or an error status code.
    static inline int SSIOLSC() {
        return SacFor_Gearkit_SSIOLSC();
    }

    /// \brief Switches the engine to the SSIOLSC Safety-Critical profile with selectable Primitiva R/W mode.
    ///
    /// \param[in] use_primitiva_rw If true, selects SF_SCGC_TYPE_PRIMITIVE_RW (Zero-GC / Raw Sector mode);
    ///                             if false, selects SF_SCGC_TYPE_DEFAULT (Phase-STW Bureaucratic mode).
    /// \return MSIMD_SUCCESS (0) on success, or an error status code.
    static inline int SSIOLSC(bool use_primitiva_rw) {
        return SacFor_Gearkit_SSIOLSC_Ex(
            use_primitiva_rw ? SF_SCGC_TYPE_PRIMITIVE_RW : SF_SCGC_TYPE_DEFAULT);
    }

    /// \brief Switches the engine to the SSIOLSC profile with an explicit SCGC mode enum.
    ///
    /// \param[in] scgc_type Target SCGC operational mode.
    /// \return MSIMD_SUCCESS (0) on success, or an error status code.
    static inline int SSIOLSC(SF_custominiVm_scgc_type_t scgc_type) {
        return SacFor_Gearkit_SSIOLSC_Ex(scgc_type);
    }

    /// \brief Queries the currently active operational profile mode.
    /// \return Active msimd_profile_mode_t enum constant.
    static inline msimd_profile_mode_t GetMode() {
        return SacFor_Gearkit_GetMode();
    }

    /// \brief Checks whether the engine is actively executing in Safety-Critical mode.
    /// \return True if in Safety-Critical mode, false otherwise.
    static inline bool IsSafetyCritical() {
        return SacFor_Gearkit_GetMode() == MSIMD_PROFILE_SAFETY_CRITICAL;
    }
};

/// \typedef SacFor_Gearkit_Model
/// \brief Legacy alias for SF_Gearkit_Model to maintain backward compatibility.
using SacFor_Gearkit_Model = SF_Gearkit_Model;

/// \typedef SF_Gearkit
/// \brief Standard ergonomic alias for SF_Gearkit_Model.
using SF_Gearkit           = SF_Gearkit_Model;

#include <type_traits>
#include <string>


/* ========================================================================= */
/* CONTINUNTIL.FINISH & STEPPING_MTP TAGS & CONSTANTS                        */
/* ========================================================================= */

/// \struct _sf_continuntil_tag
/// \brief Tag type enabling ergonomic `CONTINUNTIL.FINISH` syntax for continuous GC execution.
struct _sf_continuntil_tag {
    static constexpr SF_custominiVm_gc_run_mode_t FINISH = SF_GC_RUN_CONTINUE_FINISH;
    constexpr SF_custominiVm_gc_run_mode_t operator()() const { return FINISH; }
};

#if defined(__cplusplus) && __cplusplus >= 201703L
/// \brief Global tag instance enabling intuitive syntax: `Sfvm.Gc(CONTINUNTIL.FINISH)`.
inline constexpr _sf_continuntil_tag CONTINUNTIL{};
#else
static const _sf_continuntil_tag CONTINUNTIL{};
#endif

/// \brief Stepping mode constant for Manual Termination Point (MTP) execution.
static constexpr SF_custominiVm_gc_run_mode_t STEPPING_MTP = SF_GC_RUN_STEPPING_MTP;
/// \brief Stepping mode constant for single-cycle incremental GC stepping.
static constexpr SF_custominiVm_gc_run_mode_t STEP_SINGLE  = SF_GC_RUN_SINGLE_STEP;

/* ========================================================================= */
/* C++ SFVM RUNTIME & MANUAL GC HELPER (Sfvm.Gc() / Sfvm::Gc())              */
/* ========================================================================= */

/// \struct sfvm_runtime_t
/// \brief High-level modern C++ static interface for SFVM garbage collection, memory telemetry, and heuristics.
///
/// Provides an idiomatic C++ API wrapping low-level C SF_custominiVm_* functions. Supports single-step
/// collection, continuous draining to finish, predicate-controlled stepping with C++ lambdas,
/// and comprehensive Caretaker Manifest reporting.
struct sfvm_runtime_t {
    /// \brief Executes a single incremental garbage collection cycle.
    ///
    /// \param[in] owp One-Way Pacing flag: if true, applies strict allocation backpressure pacing.
    /// \return Volume of memory reclaimed during the step in bytes.
    static inline size_t Gc(bool owp = false) {
        return SF_custominiVm_gc_run(SF_GC_RUN_SINGLE_STEP, owp ? 1 : 0, nullptr, nullptr);
    }

    /// \brief Executes garbage collection under a specified operational stepping mode.
    ///
    /// Example: `Sfvm.Gc(CONTINUNTIL.FINISH)` or `Sfvm.Gc(STEPPING_MTP, true)`.
    ///
    /// \param[in] mode Stepping mode (SF_GC_RUN_SINGLE_STEP, CONTINUE_FINISH, or STEPPING_MTP).
    /// \param[in] owp  One-Way Pacing flag.
    /// \return Total volume of memory reclaimed in bytes.
    static inline size_t Gc(SF_custominiVm_gc_run_mode_t mode, bool owp = false) {
        return SF_custominiVm_gc_run(mode, owp ? 1 : 0, nullptr, nullptr);
    }

    /// \brief Executes continuous collection until the heap is fully drained using the CONTINUNTIL tag.
    ///
    /// Example: `Sfvm.Gc(CONTINUNTIL)`.
    ///
    /// \param[in] tag Unused tag parameter for overload resolution.
    /// \param[in] owp One-Way Pacing flag.
    /// \return Total volume of memory reclaimed in bytes.
    static inline size_t Gc(const _sf_continuntil_tag&, bool owp = false) {
        return SF_custominiVm_gc_run(SF_GC_RUN_CONTINUE_FINISH, owp ? 1 : 0, nullptr, nullptr);
    }

    /// \brief Executes stepping collection evaluating a C-style function pointer MTP predicate.
    ///
    /// \param[in] mode      Target stepping mode.
    /// \param[in] owp       One-Way Pacing flag.
    /// \param[in] pred      Function pointer predicate returning 1 to terminate, 0 to continue.
    /// \param[in] user_data Context pointer forwarded to pred.
    /// \return Reclaimed bytes.
    static inline size_t Gc(SF_custominiVm_gc_run_mode_t mode, bool owp,
                            SF_custominiVm_mtp_predicate_fn pred, void* user_data = nullptr) {
        return SF_custominiVm_gc_run(mode, owp ? 1 : 0, pred, user_data);
    }

    /// \brief Executes stepping collection evaluating a modern C++ lambda or callable predicate.
    ///
    /// Example:
    /// \code
    /// Sfvm.Gc(STEPPING_MTP, false, [](const sfAnatomyStat* s, const sfEnvChange* e) {
    ///     return s->falline_emptybox_bytes < 1024 * 1024;
    /// });
    /// \endcode
    ///
    /// \tparam Predicate Callable object type matching `bool(const sfAnatomyStat*, const sfEnvChange*)`.
    /// \param[in] mode Target stepping mode.
    /// \param[in] owp  One-Way Pacing flag.
    /// \param[in] pred Lambda or functor predicate.
    /// \return Reclaimed bytes.
    template <typename Predicate, typename = typename std::enable_if<!std::is_convertible<Predicate, bool>::value && !std::is_convertible<Predicate, SF_custominiVm_gc_run_mode_t>::value>::type>
    static inline size_t Gc(SF_custominiVm_gc_run_mode_t mode, bool owp, Predicate pred) {
        auto thunk = [](const SF_custominiVm_anatomy_stat_t* s, const SF_custominiVm_env_change_t* e, void* ctx) -> int {
            auto* p = static_cast<Predicate*>(ctx);
            return (*p)(s, e) ? 1 : 0;
        };
        return SF_custominiVm_gc_run(mode, owp ? 1 : 0, thunk, &pred);
    }

    /// \brief Triggers a deterministic deep clean sweep ("Shake the Bed and Sleep") and returns the full audit manifest.
    /// \return Completed Caretaker Manifest audit record.
    static inline SF_custominiVm_gc_manifest_t GcReport() {
        return SF_custominiVm_gc_shake_bed();
    }

    /// \brief Retrieves the audit manifest generated by the most recent deep clean sweep.
    /// \return Last recorded Caretaker Manifest.
    static inline SF_custominiVm_gc_manifest_t LastManifest() {
        SF_custominiVm_gc_manifest_t m{};
        SF_custominiVm_gc_get_last_manifest(&m);
        return m;
    }

    /// \brief Configures the autonomous garbage collection heuristic model.
    /// \param[in] h Selected heuristic (Adaptive, Aggressive, Bureaucratic, or Disabled).
    static inline void SetHeuristic(SF_custominiVm_gc_heuristic_t h) {
        SF_custominiVm_gc_set_heuristic(h);
    }

    /// \brief Queries the currently configured autonomous garbage collection heuristic model.
    /// \return Active heuristic enum value.
    static inline SF_custominiVm_gc_heuristic_t GetHeuristic() {
        return SF_custominiVm_gc_get_heuristic();
    }

    /// \brief Enables or disables No-sfGC (Zero-GC) execution mode.
    /// \param[in] enabled Set to true for Zero-GC mode, false to restore active collection.
    static inline void SetNoGc(bool enabled) {
        SF_custominiVm_gc_set_no_sfgc(enabled ? 1 : 0);
    }

    /// \brief Checks whether the engine is currently executing in No-sfGC (Zero-GC) mode.
    /// \return True if in Zero-GC mode, false otherwise.
    static inline bool IsNoGc() {
        return SF_custominiVm_gc_is_no_sfgc() != 0;
    }

    /// \brief Returns total active on-heap memory volume in bytes.
    static inline size_t HeapBytes() { return SF_custominiVm_heap_bytes(); }
    /// \brief Returns cumulative rebated memory returned to active runways in bytes.
    static inline size_t RebatedBytes() { return SF_custominiVm_heap_rebated_bytes(); }
    /// \brief Returns cumulative count of completed GC sweep cycles.
    static inline uint64_t SweepCount() { return SF_custominiVm_gc_sweep_count(); }

    /* ===================================================================== */
    /* SPECIALIZED HARDWARE ENGINE SUBSYSTEM WRAPPERS                        */
    /* ===================================================================== */

    /// \brief Checks whether the processor possesses a hybrid (P/E-core) architecture.
    static inline bool IsHybrid() { return SF_custominiVm_is_hybrid_cpu() != 0; }
    /// \brief Queries whether the system is Non-Uniform Memory Access (NUMA).
    static inline bool IsNuma() { return SF_custominiVm_is_numa() != 0; }
    /// \brief Queries whether Simultaneous Multi-Threading (SMT/Hyper-Threading) is active.
    static inline bool IsHtActive() { return SF_custominiVm_is_ht_active() != 0; }
    /// \brief Queries the SRAM and cache architectural regime.
    static inline SF_custominiVm_sram_regime_t SramRegime() { return SF_custominiVm_sram_regime(); }
    /// \brief Computes optimal concurrency sizing for compute-heavy SIMD or throughput workloads.
    static inline uint32_t OptimalConcurrency(bool is_compute_heavy = true) {
        return SF_custominiVm_optimal_concurrency(is_compute_heavy ? 1 : 0);
    }
    /// \brief Binds the calling thread to a specific core performance tier (P-core vs E-core).
    static inline bool BindThread(SF_custominiVm_core_tier_t tier) {
        return SF_custominiVm_bind_calling_thread(tier) != 0;
    }
    /// \brief Applies an execution thread affinity policy.
    static inline bool ApplyAffinityPolicy(SF_custominiVm_workload_affinity_policy_t policy) {
        return SF_custominiVm_apply_affinity_policy(policy) != 0;
    }
    /// \brief Allocates an ultra-low latency buffer from on-chip SRAM / scratchpad memory.
    static inline void* ScratchpadAlloc(size_t bytes, size_t alignment = 64) {
        return SF_custominiVm_sram_scratchpad_alloc(bytes, alignment);
    }
    /// \brief Resets the on-chip SRAM scratchpad bump allocator in O(1).
    static inline void ScratchpadReset() { SF_custominiVm_sram_scratchpad_reset(); }

    /// \brief Returns the currently active execution focus mode.
    static inline SF_custominiVm_execution_focus_t Focus() {
        return SF_custominiVm_get_execution_focus();
    }
    /// \brief Sets the execution focus mode, reconfiguring scheduler and worker allocation.
    static inline void SetFocus(SF_custominiVm_execution_focus_t focus) {
        SF_custominiVm_set_execution_focus(focus);
    }

    /// \brief Shuts down the SFVM runtime engine and frees all allocated resources.
    static inline void Shutdown() { SF_custominiVm_shutdown(); }
};

#if defined(__cplusplus) && __cplusplus >= 201703L
/// \brief Global singleton instance of the modern C++ SFVM runtime helper.
inline constexpr sfvm_runtime_t Sfvm{};
#else
static const sfvm_runtime_t Sfvm{};
#endif

/* ========================================================================= */
/* TWO-STAGE LIFECYCLE C++ WRAPPERS (SFVM_Setup & SFVM_VM)                   */
/* ========================================================================= */

/// \struct sfvm_setup_t
/// \brief C++ Stage 1 Initializer: Discovers hardware topology, allocates virtual memory arenas, and mounts VFS.
struct sfvm_setup_t {
    /// \brief Executes Stage 1 initialization.
    /// \param[in] cfg Optional custom configuration descriptor (default nullptr).
    /// \return MSIMD_SUCCESS (0) on success, or an error status code.
    static inline int Start(const void* cfg = nullptr) {
        return SF_custominiVm_setup_start(cfg);
    }

    /// \brief Checks whether Stage 1 setup has completed successfully.
    /// \return True if setup is complete, false otherwise.
    static inline bool IsReady() {
        return SF_custominiVm_is_setup() != 0;
    }

    /// \brief Function call operator overload executing Start(cfg).
    inline int operator()(const void* cfg = nullptr) const {
        return Start(cfg);
    }
};

#if defined(__cplusplus) && __cplusplus >= 201703L
/// \brief Global callable Stage 1 initializer object: `SFVM_Setup()`.
inline constexpr sfvm_setup_t SFVM_Setup{};
#else
static const sfvm_setup_t SFVM_Setup{};
#endif

/// \struct sfvm_vm_t
/// \brief C++ Stage 2 Initializer: Ingests bitcode, decompresses code heaps, and boots JIT engine.
struct sfvm_vm_t {
    /// \brief Executes Stage 2 initialization and launches the execution engine.
    /// \return MSIMD_SUCCESS (0) on success, or an error status code.
    static inline int Start() {
        return SF_custominiVm_start();
    }

    /// \brief Checks whether the VM has started and is actively executing.
    /// \return True if actively running, false otherwise.
    static inline bool IsRunning() {
        return SF_custominiVm_is_started() != 0;
    }

    /// \brief Shuts down the VM execution engine.
    static inline void Shutdown() {
        SF_custominiVm_shutdown();
    }

    /// \brief Function call operator overload executing Start().
    inline int operator()() const {
        return Start();
    }
};

#if defined(__cplusplus) && __cplusplus >= 201703L
/// \brief Global callable Stage 2 initializer object: `SFVM_VM()`.
inline constexpr sfvm_vm_t SFVM_VM{};
#else
static const sfvm_vm_t SFVM_VM{};
#endif

/* ========================================================================= */
/* SF HIGH-LEVEL MODERN NAMESPACE                                            */
/* ========================================================================= */
namespace SF {
    struct GroupParams {
        uint8_t priority      = 5;     ///< 0 - 10 ALU Queue Priority
        bool    bypass_warmup = false; ///< true = Bypass Yapbozcu optimization (latency-critical)
    };

    /// \brief High-level Group Compute Dispatcher.
    /// Assigns a unique group hex ID and stamps priority/warmup bypass into allocated objects.
    inline uint32_t GroupCompute(const GroupParams& params = GroupParams{}) {
        return SF_custominiVm_group_compute(params.priority, params.bypass_warmup ? 1 : 0);
    }

    /// \brief Resets thread-local GroupCompute parameters to defaults.
    inline void GroupComputeReset() {
        SF_custominiVm_group_compute_reset();
    }

    /// \struct EntangleScope
    /// \brief Unified modern C++ RAII transaction manager for Batch Register & Entangle Execution (Kernel Fusion)
    ///        and Group Compute dispatching. Wrapping MathventageSIMD calls inside an
    ///        SF::EntangleScope scope(priority, bypass_warmup) automatically:
    ///        1) Stamps all operations with a unique 32-bit batch group hex ID.
    ///        2) Sets ALU queue priority (0-10) and warmup bypass behavior.
    ///        3) Engages JIT register scratchpad fusion across the scope lifetime.
    struct EntangleScope {
        uint32_t group_id{0};

        explicit EntangleScope(uint8_t priority = 5, bool bypass_warmup = false)
            : group_id(SF_custominiVm_batch_begin(priority, bypass_warmup ? 1 : 0)) {
            SF_custominiVm_entangle_begin();
        }

        ~EntangleScope() {
            SF_custominiVm_entangle_commit();
            if (group_id) {
                SF_custominiVm_batch_end();
                group_id = 0;
            }
        }

        EntangleScope(const EntangleScope&) = delete;
        EntangleScope& operator=(const EntangleScope&) = delete;

        EntangleScope(EntangleScope&& o) noexcept : group_id(o.group_id) {
            o.group_id = 0;
        }

        EntangleScope& operator=(EntangleScope&& o) noexcept {
            if (this != &o) {
                if (group_id) {
                    SF_custominiVm_entangle_commit();
                    SF_custominiVm_batch_end();
                }
                group_id = o.group_id;
                o.group_id = 0;
            }
            return *this;
        }
    };

    /// \brief Aliases for seamless semantic versatility and backward compatibility:
    using BatchScope = EntangleScope;
    using GroupScope = EntangleScope;

    /// \enum BranchPattern
    /// \brief Branch bias pattern for hardware instruction branch guidance.
    enum class BranchPattern {
        DETERMINISTIC_UNIFORM = 0,
        CHAOTIC_RANDOM        = 1,
        BIAS_LIKELY           = 2,
        BIAS_UNLIKELY         = 3
    };

    /// \class BranchPredictingDirector
    /// \brief Guides compiler and CPU BPU branch evaluation for deterministic SIMD loops and VM interlocks.
    class BranchPredictingDirector {
    public:
        static inline bool Predict(bool condition, BranchPattern pattern = BranchPattern::BIAS_LIKELY) {
            switch (pattern) {
                case BranchPattern::BIAS_LIKELY:
#if defined(__GNUC__) || defined(__clang__)
                    return __builtin_expect(!!condition, 1);
#else
                    return condition;
#endif
                case BranchPattern::BIAS_UNLIKELY:
#if defined(__GNUC__) || defined(__clang__)
                    return __builtin_expect(!!condition, 0);
#else
                    return condition;
#endif
                case BranchPattern::CHAOTIC_RANDOM:
                    return sf_hinter_cmov_i32(condition ? 1 : 0, 1, 0, nullptr) != 0;
                default:
                    return condition;
            }
        }
    };

    /// \struct SelfHeal
    /// \brief C++ namespace helper for querying and triggering self-healing recovery passes.
    struct SelfHeal {
        static inline uint64_t RunCheck() {
            return SF_custominiVm_self_heal_run_check();
        }
        static inline SF_custominiVm_self_heal_stats_t QueryStats() {
            SF_custominiVm_self_heal_stats_t s{};
            SF_custominiVm_self_heal_query_stats(&s);
            return s;
        }
        static inline void CaptureCodeError(const char* location, const char* detail) {
            SF_custominiVm_self_heal_capture_code_error(location, detail);
        }
        static inline void CaptureBitcodeException(uint32_t fn_id, const char* section) {
            SF_custominiVm_self_heal_capture_bitcode_exception(fn_id, section);
        }
    };

    /// \struct OSHinting
    /// \brief Direct OS scheduler hinting and hard-affinity controls.
    /// \warning Direct thread pinning is generally discouraged; provided for low-level host engine integration.
    struct OSHinting {
        static inline bool PinToCore(int logical_cpu) {
            return SF_custominiVm_thread_pin_to_core(logical_cpu) != 0;
        }
        static inline bool PinToNumaNode(uint32_t numa_node) {
            return SF_custominiVm_thread_pin_to_numa_node(numa_node) != 0;
        }
        static inline bool SetPriorityHigh() {
            return SF_custominiVm_thread_set_priority_high() != 0;
        }
    };

    /// \struct HardwareBudget
    /// \brief C++ helper for querying adaptive hardware tier classification and thread budgets.
    struct HardwareBudget {
        static inline SF_custominiVm_hardware_tier_t GetTier() {
            return SF_custominiVm_query_hardware_tier();
        }
        static inline const char* GetTierName() {
            return SF_custominiVm_query_hardware_tier_name();
        }
        static inline void QueryBudget(uint32_t* out_gc, uint32_t* out_puzzle, uint32_t* out_vm) {
            SF_custominiVm_query_thread_budget(out_gc, out_puzzle, out_vm);
        }
    };

    /// \struct SemanticCLI
    /// \brief C++ helper for resolving intent-aware CLI options and fuzzy flags.
    struct SemanticCLI {
        static inline bool ResolveIntent(const std::string& token, std::string& out_flag, std::string& out_val) {
            char flag_buf[64] = {0};
            char val_buf[64] = {0};
            int rc = SF_custominiVm_cli_resolve_intent(token.c_str(), flag_buf, sizeof(flag_buf), val_buf, sizeof(val_buf));
            if (rc != 0) {
                out_flag = flag_buf;
                out_val = val_buf;
                return true;
            }
            return false;
        }
    };

    /// \struct ExecutionFocus
    /// \brief C++ helper for querying, configuring, and inspecting SFVM New Era Execution Focus.
    struct ExecutionFocus {
        /// \brief Queries the active execution focus mode.
        static inline SF_custominiVm_execution_focus_t Get() {
            return SF_custominiVm_get_execution_focus();
        }
        /// \brief Sets the execution focus mode.
        static inline void Set(SF_custominiVm_execution_focus_t focus) {
            SF_custominiVm_set_execution_focus(focus);
        }
        /// \brief Triggers automatic silicon TableGen profile detection.
        static inline SF_custominiVm_execution_focus_t Detect() {
            return SF_custominiVm_detect_cpu_execution_focus();
        }
        /// \brief Gets human-readable name of any execution focus mode.
        static inline const char* GetName(SF_custominiVm_execution_focus_t focus) {
            return SF_custominiVm_get_execution_focus_name(focus);
        }
        /// \brief Gets human-readable name of the currently active focus mode.
        static inline const char* GetCurrentName() {
            return SF_custominiVm_get_execution_focus_name(Get());
        }
        /// \brief Gets the detected microarchitecture name from Machines-Rules.td.
        static inline const char* GetUarchName() {
            return SF_custominiVm_get_focus_uarch_name();
        }
        /// \brief Queries the P-Core and E-Core workload chunking weight factors.
        static inline void GetChunkWeights(double& p_weight, double& e_weight) {
            SF_custominiVm_get_focus_chunk_weights(&p_weight, &e_weight);
        }
        /// \brief Returns true if active focus is Single-Core Performance (SCP).
        static inline bool IsSCP() {
            return SF_custominiVm_is_focus_scp() != 0;
        }
        /// \brief Returns true if active focus is Multi-Core Performance (MCP).
        static inline bool IsMCP() {
            return SF_custominiVm_is_focus_mcp() != 0;
        }
        /// \brief Returns true if active focus is Dual Single+Multi Performance.
        static inline bool IsDual() {
            return SF_custominiVm_is_focus_dual() != 0;
        }
    };
 
    /// \struct Yapbozcu
    /// \brief C++ helper for managing Yapbozcu JIT/ISA live hot-patching modes and object busy/ready notifications.
    struct Yapbozcu {
        static inline void SetMorphMethod(SF_custominiVm_yapbozcu_morph_t mode) {
            SF_custominiVm_set_yapbozcu_morph_method(static_cast<int>(mode));
        }
        static inline SF_custominiVm_yapbozcu_morph_t GetMorphMethod() {
            return static_cast<SF_custominiVm_yapbozcu_morph_t>(SF_custominiVm_get_yapbozcu_morph_method());
        }
        static inline bool IsBusy(const void* tensor_payload) {
            return SF_custominiVm_is_object_busy(tensor_payload) != 0;
        }
        static inline bool AwaitReady(void* tensor_payload, uint32_t timeout_ms = 0) {
            return SF_custominiVm_await_object_ready(tensor_payload, timeout_ms) == 0;
        }
        static inline bool OnReady(void* tensor_payload, sf_object_ready_callback_t callback, void* user_data = nullptr) {
            return SF_custominiVm_register_object_ready_callback(tensor_payload, callback, user_data) == 0;
        }
    };

    /// \struct Rehab
    /// \brief C++ helper for Rehab Arena tensor rehabilitation and physical resurrection.
    struct Rehab {
        static inline void Mark(SF_custominiVm_tensor_t* tensor) {
            SF_custominiVm_tensor_mark_rehab(tensor);
        }
        static inline void RequestResurrection(uint32_t fn_id) {
            SF_custominiVm_request_resurrection(fn_id);
        }
        static inline size_t Scan() {
            return SF_custominiVm_scan_rehab_arena();
        }
        static inline uint64_t ResurrectionCount() {
            return SF_custominiVm_get_resurrection_count();
        }
        static inline SF_custominiVm_tensor_t FindByFunctionId(uint32_t fn_id) {
            return SF_custominiVm_find_tensor_by_function_id(fn_id);
        }
    };

    /// \struct VFS
    /// \brief C++ helper for SFVM Virtual File System (VFS) operations, diagnostics, and strict branch validation.
    struct VFS {
        /// \brief Creates a virtual directory with an optional quota.
        static inline int Mkdir(const char* path, size_t quota_bytes = 0) {
            return SF_custominiVm_vfs_mkdir(path, quota_bytes);
        }
        /// \brief Binds a tensor into the VFS hierarchy.
        static inline int Bind(const char* path, SF_custominiVm_tensor_t* tensor) {
            return SF_custominiVm_folder_bind(path, tensor);
        }
        /// \brief Evicts all tensors within the specified virtual directory path.
        static inline size_t Evict(const char* path) {
            return SF_custominiVm_folder_evict(path);
        }
        /// \brief Queries memory consumed within a virtual directory path.
        static inline size_t UsedBytes(const char* path) {
            return SF_custominiVm_folder_used_bytes(path);
        }
        /// \brief Queries status and quota telemetry for a VFS node.
        static inline int Stat(const char* path, SF_custominiVm_vfs_stat_t* out_stat) {
            return SF_custominiVm_vfs_stat(path, out_stat);
        }
        /// \brief Unlinks a tensor file from the VFS hierarchy.
        static inline int Unlink(const char* path) {
            return SF_custominiVm_vfs_unlink(path);
        }
        /// \brief Recursively removes a virtual directory and its children.
        static inline size_t RmdirRf(const char* path) {
            return SF_custominiVm_vfs_rmdir_rf(path);
        }
        /// \brief Configures strict branch validation mode (default: enabled).
        static inline void SetStrictBranchValidation(bool enable) {
            SF_custominiVm_set_strict_vfs_branch_validation(enable ? 1 : 0);
        }
        /// \brief Queries whether strict branch validation is enabled.
        static inline bool IsStrictBranchValidation() {
            return SF_custominiVm_get_strict_vfs_branch_validation() != 0;
        }
        /// \brief Configures diagnostic alert dialog spawning.
        static inline void SetPanicDialogEnabled(bool enable) {
            SF_custominiVm_vfs_set_panic_dialog_enabled(enable ? 1 : 0);
        }
        /// \brief Queries whether alert dialogs are enabled on panic.
        static inline bool IsPanicDialogEnabled() {
            return SF_custominiVm_vfs_get_panic_dialog_enabled() != 0;
        }
        /// \brief Sets panic behavior flags for testing and headless execution.
        static inline void SetPanicBehavior(bool suppress_abort, bool show_dialog) {
            SF_custominiVm_vfs_set_panic_behavior(suppress_abort ? 1 : 0, show_dialog ? 1 : 0);
        }
        /// \brief Registers a diagnostic hook callback to intercept VFS panics.
        static inline void SetPanicHook(sf_vfs_panic_hook_t hook, void* user_data = nullptr) {
            SF_custominiVm_vfs_set_panic_hook(hook, user_data);
        }
        /// \brief Triggers an immediate safe VFS panic.
        static inline void TriggerPanic(const char* missing_path, const char* operation_context) {
            SF_custominiVm_vfs_trigger_panic(missing_path, operation_context);
        }
        /// \brief Returns cumulative count of VFS panic events triggered.
        static inline uint64_t GetPanicCount() {
            return SF_custominiVm_vfs_get_panic_count();
        }
        /// \brief Returns the most recent missing VFS branch path.
        static inline const char* GetLastPanicPath() {
            return SF_custominiVm_vfs_get_last_panic_path();
        }
        /// \brief Returns the operation context of the most recent VFS panic.
        static inline const char* GetLastPanicOp() {
            return SF_custominiVm_vfs_get_last_panic_op();
        }
        /// \brief Returns the operation context of the most recent VFS panic.
        static inline const char* GetLastPanicContext() {
            return SF_custominiVm_vfs_get_last_panic_op();
        }
    };

    struct Telemetry {
        /// \brief Migrates buffered events from hotlog to coldlog.
        static inline size_t DrainLogBoot() {
            return SF_custominiVm_log_boot_drain();
        }
        /// \brief Drains active hotlog ring buffer entries via user callback.
        static inline void DrainHotLog(void (*cb)(const SF_custominiVm_hotlog_entry_t*, void*), void* user) {
            SF_custominiVm_hotlog_drain(cb, user);
        }
        /// \brief Drains active coldlog ring buffer entries via user callback.
        static inline void DrainColdLog(void (*cb)(const SF_custominiVm_coldlog_entry_t*, void*), void* user) {
            SF_custominiVm_coldlog_drain(cb, user);
        }
        /// \brief Registers an external GC lifecycle hook callback.
        static inline void RegisterGcHook(SF_custominiVm_gc_hook_fn_t hook, void* user_data) {
            SF_custominiVm_register_gc_hook(hook, user_data);
        }
        /// \brief Starts background telemetry streaming daemon.
        static inline int StartStream(uint32_t flush_interval_us, sf_telemetry_stream_fn_t cb, void* user) {
            return SF_custominiVm_telemetry_stream_start(flush_interval_us, cb, user);
        }
        /// \brief Stops background telemetry streaming daemon.
        static inline void StopStream() {
            SF_custominiVm_telemetry_stream_stop();
        }
        /// \brief Checks whether telemetry streaming daemon is running.
        static inline bool IsStreamRunning() {
            return SF_custominiVm_telemetry_stream_is_running() != 0;
        }
        /// \brief Formats hotlog record to human-readable ANSI colored terminal text.
        static inline std::string FormatCLI(const SF_custominiVm_hotlog_entry_t& e) {
            char buf[512] = {0};
            SF_custominiVm_hotlog_format_cli(&e, buf, sizeof(buf));
            return std::string(buf);
        }
        /// \brief Formats hotlog record to structured NDJSON line.
        static inline std::string FormatJSON(const SF_custominiVm_hotlog_entry_t& e) {
            char buf[512] = {0};
            SF_custominiVm_hotlog_format_json(&e, buf, sizeof(buf));
            return std::string(buf);
        }
        /// \brief Formats coldlog record to human-readable ANSI colored terminal text.
        static inline std::string FormatCLI(const SF_custominiVm_coldlog_entry_t& e) {
            char buf[512] = {0};
            SF_custominiVm_coldlog_format_cli(&e, buf, sizeof(buf));
            return std::string(buf);
        }
        /// \brief Formats coldlog record to structured NDJSON line.
        static inline std::string FormatJSON(const SF_custominiVm_coldlog_entry_t& e) {
            char buf[512] = {0};
            SF_custominiVm_coldlog_format_json(&e, buf, sizeof(buf));
            return std::string(buf);
        }
        /// \brief Updates subsystem log filter mask.
        static inline void SetFilterMask(uint32_t mask) {
            SF_custominiVm_set_log_filter_mask(mask);
        }
        /// \brief Retrieves current subsystem log filter mask.
        static inline uint32_t GetFilterMask() {
            return SF_custominiVm_get_log_filter_mask();
        }
        /// \brief Registers a real-time raw log stream listener.
        static inline void RegisterRawStream(sf_raw_log_stream_fn_t stream_fn, void* user = nullptr) {
            SF_custominiVm_register_raw_log_stream(stream_fn, user);
        }
        /// \brief Unregisters active raw log stream listener.
        static inline void UnregisterRawStream() {
            SF_custominiVm_unregister_raw_log_stream();
        }
        /// \brief Configures console mirroring toggle.
        static inline void SetConsoleMirroring(bool enable) {
            SF_custominiVm_set_console_mirroring(enable ? 1 : 0);
        }
        /// \brief Queries whether console mirroring is active.
        static inline bool GetConsoleMirroring() {
            return SF_custominiVm_get_console_mirroring() != 0;
        }
        /// \brief Emits a custom raw log string to all registered listeners.
        static inline void EmitRaw(const char* formatted_text) {
            SF_custominiVm_emit_raw_log(formatted_text);
        }
    };

    /// \struct Controlpoint
    /// \brief C++ helper for Controlpoint host runtime integration and cooperative GC coordination.
    struct Controlpoint {
        static inline void RegisterHook(sf_controlpoint_hook_fn_t hook, void* user_data = nullptr) {
            SF_custominiVm_register_controlpoint_hook(hook, user_data);
        }
        static inline void UnregisterHook() {
            SF_custominiVm_unregister_controlpoint_hook();
        }
        static inline void SetMode(SF_custominiVm_host_gc_mode_t mode) {
            SF_custominiVm_set_host_gc_cooperative_mode(static_cast<int>(mode));
        }
        static inline SF_custominiVm_host_gc_mode_t GetMode() {
            return static_cast<SF_custominiVm_host_gc_mode_t>(SF_custominiVm_get_host_gc_cooperative_mode());
        }
        static inline void Trigger(SF_custominiVm_controlpoint_action_t action, uint64_t param = 0) {
            SF_custominiVm_trigger_host_controlpoint(action, param);
        }
    };

    /// \struct ALU
    /// \brief C++ helper for ALU operation dispatching, multi-limb emulation routing, and saturation management.
    struct ALU {
        static inline void SetAllowDispatchings(bool allow) {
            SF_custominiVm_set_allow_operation_dispatchings(allow ? 1 : 0);
        }
        static inline bool GetAllowDispatchings() {
            return SF_custominiVm_get_allow_operation_dispatchings() != 0;
        }
        static inline int DispatchOp(uint32_t preferred_alu, uint8_t priority = 0) {
            return SF_custominiVm_alu_dispatch_op(preferred_alu, priority);
        }
        static inline void CompleteOp(uint32_t alu_id) {
            SF_custominiVm_alu_complete_op(alu_id);
        }
        static inline uint64_t GetDispatchedCount() {
            return SF_custominiVm_get_alu_dispatched_count();
        }
        static inline uint64_t GetEmulationDispatchedCount() {
            return SF_custominiVm_get_alu_emulation_dispatched_count();
        }
        static inline int ResolveTensorRoute(const SF_custominiVm_tensor_t* tensor, uint8_t priority,
                                             bool* out_is_emulated = nullptr, SF_custominiVm_emu_schema_t* out_schema = nullptr) {
            int emu = 0, schema = 0;
            int lane = SF_custominiVm_alu_resolve_tensor_route(tensor, priority, &emu, &schema);
            if (out_is_emulated) *out_is_emulated = (emu != 0);
            if (out_schema) *out_schema = static_cast<SF_custominiVm_emu_schema_t>(schema);
            return lane;
        }
        static inline int ResolveRouteByDType(uint8_t dtype, uint8_t priority,
                                              bool* out_is_emulated = nullptr, SF_custominiVm_emu_schema_t* out_schema = nullptr) {
            int emu = 0, schema = 0;
            int lane = SF_custominiVm_alu_resolve_route_by_dtype(dtype, priority, &emu, &schema);
            if (out_is_emulated) *out_is_emulated = (emu != 0);
            if (out_schema) *out_schema = static_cast<SF_custominiVm_emu_schema_t>(schema);
            return lane;
        }
        static inline const char* GetSchemaName(SF_custominiVm_emu_schema_t schema) {
            return SF_custominiVm_get_emulation_schema_name(static_cast<int>(schema));
        }
    };

    /// \struct InlineASM
    /// \brief C++ helper for Inline Assembly (iasm) threshold gating and Ring 3 safety levels.
    struct InlineASM {
        static inline void SetThreshold(uint32_t threshold) {
            SF_custominiVm_set_iasm_threshold(threshold);
        }
        static inline uint32_t GetThreshold() {
            return SF_custominiVm_get_iasm_threshold();
        }
        static inline void SetStrictRing3(bool enable) {
            SF_custominiVm_set_strict_ring3(enable ? 1 : 0);
        }
        static inline bool IsStrictRing3() {
            return SF_custominiVm_is_strict_ring3() != 0;
        }
        static inline bool IsActive(uint32_t priority = 0) {
            return SF_custominiVm_is_iasm_active(priority) != 0;
        }
    };

    /// \struct Flags
    /// \brief C++ helper for SFVM command-line flag diagnostics and strict flag validation.
    struct Flags {
        static inline void SetStrictValidation(bool enable) {
            SF_custominiVm_set_strict_flags(enable ? 1 : 0);
        }
        static inline bool IsStrictValidation() {
            return SF_custominiVm_is_strict_flags() != 0;
        }
    };
}

#endif // __cplusplus

#endif // SACCULUS_FORGE_H

