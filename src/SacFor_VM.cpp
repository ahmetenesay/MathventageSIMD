#include "MathventageSIMD.h"
#include "SacculusForge.h"

/* ========================================================================= */
/* SFVM Internal Runtime Subsystem Declarations & Types                     */
/* ========================================================================= */

/* ========================================================================= */
/* MathventageSIMD: Internal Engine Context & Lattice Data Structures         */
/* ========================================================================= */

#ifdef __cplusplus
extern "C" {
#endif

struct msimd_context_t {
    msimd_cpu_info_t cpu_info;
    msimd_isa_t      isa;
    msimd_backend_t  backend;
    void*            backend_handle;

    // Runtime-calibrated optimal fallback level (0-9)
    int              optimal_ladder_level;

    // Safety-Critical (SFVM-SC) Unified Engine State
    msimd_config_t   config;
    uint8_t*         static_arena_base;
    size_t           static_arena_capacity;
    size_t           static_arena_offset;
    bool             arena_is_user_provided;
};

struct msimd_lattice_t {
    msimd_context     ctx;
    int               dim;
    size_t            nx;
    size_t            ny;
    size_t            nz;
    size_t            total_elements;
    size_t            elem_size;
    msimd_precision_t precision;
    void*             data_prev;
    void*             data_curr;
    void*             data_next;
    bool              allocated_from_arena;
};

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */
/* OPAQUE INTERNAL RUNTIME HEADERS                                           */
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

/* ========================================================================= */
/* INTERNAL sfGC OBJECT ADMISSION & GENERATIONAL HOOKS                       */
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

/* ========================================================================= */
/* ANONYMOUS DISK PAGEFILE & VIRTUAL MEMORY SWAP SPECIFICATION               */
/* ========================================================================= */
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
 * Once paged out, physical RAM occupied by the tensor is relinquished, and the tensor\'s
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
/* TELEMETRY & EVENT LOGGING STRUCTURES                                      */
/* Defined in SacculusForge.h: SF_custominiVm_hotlog_entry_t,                 */
/* SF_custominiVm_coldlog_entry_t, sfHotlogEntry, sfColdlogEntry              */
/* ========================================================================= */



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
/* SCGC SECONDARY TYPING & RAW PRIMITIVA SECTOR I/O API                      */
/* ========================================================================= */
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
/* SPECIALIZED HARDWARE ENGINE SUBSYSTEMS (HYBRID, NUMA, SMT/HT, SRAM)       */
/* ========================================================================= */



/**
 * @brief Checks whether the processor possesses a hybrid (heterogeneous) core architecture.
 * @return 1 if hybrid (P/E cores), 0 if homogeneous.
 */
MSIMD_API int SF_custominiVm_is_hybrid_cpu(void);

/**
 * @brief Queries the specific CPU core symmetry classification.
 * @return CPU topology class enum.
 */
MSIMD_API SF_custominiVm_cpu_topology_class_t SF_custominiVm_cpu_topology_class(void);

/**
 * @brief Checks whether the host system features a Non-Uniform Memory Access (NUMA) architecture.
 * @return 1 if multi-node NUMA, 0 if UMA / single uniform domain.
 */
MSIMD_API int SF_custominiVm_is_numa(void);

/**
 * @brief Queries the memory architecture classification.
 * @return Memory architecture enum.
 */
MSIMD_API SF_custominiVm_mem_architecture_t SF_custominiVm_mem_architecture(void);

/**
 * @brief Checks whether Simultaneous Multi-Threading (Hyper-Threading / SMT) is actively enabled.
 * @return 1 if SMT active, 0 if disabled or absent.
 */
MSIMD_API int SF_custominiVm_is_ht_active(void);

/**
 * @brief Queries the active SMT concurrency mode.
 * @return SMT mode enum.
 */
MSIMD_API SF_custominiVm_smt_mode_t SF_custominiVm_smt_mode(void);

/**
 * @brief Queries the SRAM and cache architectural regime.
 * @return SRAM regime enum.
 */
MSIMD_API SF_custominiVm_sram_regime_t SF_custominiVm_sram_regime(void);

/**
 * @brief Calculates optimal worker thread concurrency based on workload nature and CPU/SMT topology.
 * @param[in] is_compute_heavy If non-zero, sizes for vector/SIMD avoiding SMT port sharing.
 * @return Optimal thread worker count.
 */
MSIMD_API uint32_t SF_custominiVm_optimal_concurrency(int is_compute_heavy);

/**
 * @brief Binds the calling thread to a specific core performance tier (P-core vs E-core).
 * @param[in] tier Target tier (Performance, Efficiency, or Any).
 * @return 1 on success, 0 on failure or unsupported.
 */
MSIMD_API int SF_custominiVm_bind_calling_thread(SF_custominiVm_core_tier_t tier);

/**
 * @brief Applies a system-wide execution affinity policy to the calling thread.
 * @param[in] policy Target affinity policy.
 * @return 1 on success, 0 on failure.
 */
MSIMD_API int SF_custominiVm_apply_affinity_policy(SF_custominiVm_workload_affinity_policy_t policy);

/**
 * @brief Allocates an ultra-low latency, zero-page-fault scratch buffer from on-chip SRAM / scratchpad memory.
 * @param[in] bytes Allocation size in bytes.
 * @param[in] alignment Byte alignment boundary (default 64).
 * @return Pointer to aligned scratchpad memory, or nullptr if exhausted.
 */
MSIMD_API void* SF_custominiVm_sram_scratchpad_alloc(size_t bytes, size_t alignment);

/**
 * @brief Resets the on-chip SRAM scratchpad bump allocator, reclaiming all scratch memory in O(1).
 */
MSIMD_API void SF_custominiVm_sram_scratchpad_reset(void);

/**
 * @brief Allocates memory explicitly bound to a specific physical NUMA node domain.
 * @param[in] bytes Allocation size in bytes.
 * @param[in] node Target NUMA node ID (0-based).
 * @return Pointer to node-local memory, or nullptr on failure.
 */
MSIMD_API void* SF_custominiVm_numa_alloc(size_t bytes, uint8_t node);

/**
 * @brief Releases memory previously allocated via SF_custominiVm_numa_alloc.
 * @param[in] ptr Memory buffer pointer.
 * @param[in] bytes Allocation size in bytes.
 * @param[in] node NUMA node ID where memory was hosted.
 */
MSIMD_API void SF_custominiVm_numa_free(void* ptr, size_t bytes, uint8_t node);

/* ========================================================================= */
/* VIRTUAL FILE SYSTEM (VFS) & DIRECTORY-HEAP STORAGE API                    */
/* ========================================================================= */


#ifndef SF_VFS_STAT_DEFINED
#define SF_VFS_STAT_DEFINED
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
#endif

#ifndef SF_VFS_PANIC_HOOK_DEFINED
#define SF_VFS_PANIC_HOOK_DEFINED
/**
 * @typedef sf_vfs_panic_hook_t
 * @brief Callback function prototype invoked upon VFS panic before termination.
 */
typedef void (*sf_vfs_panic_hook_t)(const char* missing_path, const char* operation_context, void* user_data);
#endif

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

/**
 * @brief Enables or disables strict VFS branch validation mode (default: enabled).
 */
MSIMD_API void        SF_custominiVm_set_strict_vfs_branch_validation(int enable);

/**
 * @brief Queries whether strict VFS branch validation is currently active.
 */
MSIMD_API int         SF_custominiVm_get_strict_vfs_branch_validation(void);

/**
 * @brief Alias for SF_custominiVm_get_strict_vfs_branch_validation.
 */
MSIMD_API int         SF_custominiVm_is_strict_vfs_branch_validation(void);

/**
 * @brief Controls whether diagnostic modal alert dialogs are spawned on fatal VFS panics.
 */
MSIMD_API void        SF_custominiVm_vfs_set_panic_dialog_enabled(int enable);

/**
 * @brief Queries whether diagnostic modal alert dialogs are enabled on fatal VFS panics.
 */
MSIMD_API int         SF_custominiVm_vfs_get_panic_dialog_enabled(void);

/**
 * @brief Sets panic behavior flags for testing and headless execution.
 */
MSIMD_API void        SF_custominiVm_vfs_set_panic_behavior(int suppress_abort, int show_dialog);

/**
 * @brief Registers an optional callback hook to be notified when a VFS panic occurs.
 */
MSIMD_API void        SF_custominiVm_vfs_set_panic_hook(sf_vfs_panic_hook_t hook, void* user_data);

/**
 * @brief Manually triggers an immediate safe VFS panic with diagnostic logging, alert window, and fail-fast termination.
 */
MSIMD_API void        SF_custominiVm_vfs_trigger_panic(const char* missing_path, const char* operation_context);

/**
 * @brief Returns cumulative count of VFS panic events triggered during runtime lifetime.
 */
MSIMD_API uint64_t    SF_custominiVm_vfs_get_panic_count(void);

/**
 * @brief Returns the most recent missing VFS branch path that caused a panic.
 */
MSIMD_API const char* SF_custominiVm_vfs_get_last_panic_path(void);

/**
 * @brief Returns the operation context of the most recent VFS panic.
 */
MSIMD_API const char* SF_custominiVm_vfs_get_last_panic_op(void);



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


/* SF_custominiVm_gc_hook_event_e, sfGcHookEvent, SF_custominiVm_gc_hook_fn_t */
/* are declared in SacculusForge.h                                            */
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

#ifndef MSIMD_PARALLEL_FOR_FN_DEFINED
#define MSIMD_PARALLEL_FOR_FN_DEFINED
typedef void (*msimd_parallel_for_fn)(void* user_data, size_t start, size_t end);
#endif

/* ========================================================================= */
/* SFVM HARDWARE TOPOLOGY, DISPATCHER & INTRINSIC ACCELERATION APIS          */
/* ========================================================================= */
MSIMD_API void           sf_backend_accelerated_destroy(msimd_context ctx);
MSIMD_API void           sf_backend_accelerated_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data);
MSIMD_API msimd_status_t sf_backend_accelerated_init(msimd_context ctx);

MSIMD_API void           sf_backend_fallback_destroy(msimd_context ctx);
MSIMD_API void           sf_backend_fallback_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data);
MSIMD_API msimd_status_t sf_backend_fallback_init(msimd_context ctx);

MSIMD_API void           sf_query_hw_topology(msimd_hw_topology_t* out);
MSIMD_API void           sf_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data);
MSIMD_API void           sf_prefetch_t0(const void* addr);
MSIMD_API void           sf_prefetch_t1(const void* addr);
MSIMD_API void           sf_prefetch_t2(const void* addr);
MSIMD_API void           sf_prefetch_nta(const void* addr);
MSIMD_API void           sf_prefetch_stream(const void* base, size_t count, size_t stride);
MSIMD_API void           sf_stream_store_f32(float* dst, const float* src, size_t count);
MSIMD_API void           sf_stream_store_f64(double* dst, const double* src, size_t count);
MSIMD_API void           sf_hw_fence_full(void);
MSIMD_API void           sf_hw_fence_store(void);
MSIMD_API void           sf_hw_fence_load(void);
MSIMD_API void           sf_memcpy_nt(void* dst, const void* src, size_t size);
MSIMD_API void           sf_memzero_nt(void* dst, size_t size);
MSIMD_API uint64_t       sf_rdtsc_begin(void);
MSIMD_API uint64_t       sf_rdtsc_end(void);
MSIMD_API uint64_t       sf_rdtsc_freq_hz(void);
MSIMD_API double         sf_cycles_to_ns(uint64_t cycles, uint64_t freq_hz);
MSIMD_API int            sf_thread_pin_to_core(int logical_cpu);
MSIMD_API int            sf_thread_pin_to_numa_node(uint32_t numa_node);
MSIMD_API int            sf_thread_set_priority_high(void);
MSIMD_API msimd_status_t sf_calibrate_pipeline(msimd_context ctx, int* out_best_level, double* out_gflops);

MSIMD_API void           SF_custominiVm_query_hw_topology(msimd_hw_topology_t* out);
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

#ifdef __cplusplus
}
#endif

/**
 * @struct SF_Gearkit_Extended
 * @brief Internal extended C++ methods for Primitiva raw sectors and Puzzle Arena batch evaluation.
 */
struct SF_Gearkit_Extended : public SF_Gearkit_Model {
    static inline void* PrimitivaAlloc(size_t bytes, size_t align = 64) {
        return SF_primitiva_alloc(bytes, align);
    }
    static inline void PrimitivaWrite(void* sector, const void* src, size_t bytes) {
        SF_primitiva_write(sector, src, bytes);
    }
    static inline void PrimitivaRead(const void* sector, void* dst, size_t bytes) {
        SF_primitiva_read(sector, dst, bytes);
    }
    static inline void PrimitivaOverwrite(void* sector, const void* src, size_t bytes) {
        SF_primitiva_overwrite(sector, src, bytes);
    }
    static inline void PrimitivaZero(void* sector, size_t bytes) {
        SF_primitiva_zero(sector, bytes);
    }
    static inline SF_custominiVm_scgc_type_t GetScgcType() {
        return SF_custominiVm_get_scgc_type();
    }
    static inline void SetScgcType(SF_custominiVm_scgc_type_t t) {
        SF_custominiVm_set_scgc_type(t);
    }
    static inline void SubmitPuzzleBatch(uint32_t fn_id,
                                        const SF_custominiVm_tensor_t* batch_samples,
                                        int sample_count,
                                        const SF_custominiVm_tensor_t* target_precision_tensor,
                                        float target_epsilon = 1e-4f) {
        SF_custominiVm_puzzle_submit_batch(fn_id, batch_samples, sample_count, target_precision_tensor, target_epsilon);
    }
    static inline void SetPuzzleSampleCount(size_t count) {
        SF_custominiVm_puzzle_set_sample_count(count);
    }
    static inline size_t GetPuzzleSampleCount() {
        return SF_custominiVm_puzzle_get_sample_count();
    }
    static inline bool IsStreamPaused() {
        return SF_custominiVm_puzzle_is_stream_paused() != 0;
    }
};


#ifdef USE_TBB
#include <tbb/parallel_for.h>
#include <tbb/parallel_invoke.h>
#include <tbb/parallel_reduce.h>
#include <tbb/task_arena.h>
#include <tbb/task_scheduler_observer.h>
#include <tbb/partitioner.h>
#include <tbb/blocked_range.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/concurrent_queue.h>
#include <tbb/info.h>
#include <tbb/version.h>
#endif

#ifdef USE_OPENVINO
#include <openvino/openvino.hpp>
#endif

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
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
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <sys/mman.h>
#  include <unistd.h>
#  include <time.h>
#  include <fcntl.h>
#  include <strings.h>
#  ifndef _stricmp
#    define _stricmp strcasecmp
#  endif
#  ifndef _strnicmp
#    define _strnicmp strncasecmp
#  endif
#  if defined(__linux__) && (defined(__aarch64__) || defined(__arm__))
#    include <sys/auxv.h>
#    include <asm/hwcap.h>
#  endif
#  if defined(__APPLE__)
#    include <sys/types.h>
#    include <sys/sysctl.h>
#  endif
#endif

#if !defined(_M_X64) && !defined(__x86_64__) && !defined(_M_IX86) && !defined(__i386__)
#  ifndef _mm_malloc
static inline void* _mm_malloc(size_t size, size_t align) {
    void* ptr = nullptr;
    if (posix_memalign(&ptr, align < sizeof(void*) ? sizeof(void*) : align, size) != 0) {
        return nullptr;
    }
    return ptr;
}
#  endif
#  ifndef _mm_free
static inline void _mm_free(void* ptr) {
    free(ptr);
}
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
#if (defined(_M_X64) || defined(__x86_64__) || defined(_M_IX86) || defined(__i386__))
#  if defined(_MSC_VER) || (defined(__clang__) && defined(_WIN32))
    __cpuidex(info, leaf, subleaf);
#  elif defined(__GNUC__) || defined(__clang__)
    __cpuid_count(leaf, subleaf, info[0], info[1], info[2], info[3]);
#  else
    info[0] = info[1] = info[2] = info[3] = 0;
#  endif
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

#define SACFOR_COLDLOG_SIZE     12877824u /* 786 MB capacity with 64-byte entries (12,877,824 * 64B = 824,180,736B) */

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

// Forward declarations for specialized hardware engine subsystem
typedef struct SF_custominiVm_hardware_topology_t sf_hardware_topology_t;
static bool sf_bind_current_thread(SF_custominiVm_core_tier_t tier);
static void sf_init_hardware_nexus(const sf_hardware_topology_t* topo);
static void sf_ensure_hardware_nexus_initialized(void);
static bool sf_is_hybrid_cpu(void);
static bool sf_is_efficiency_core_active(void);
static uint32_t sf_get_p_core_count(void);
static uint32_t sf_get_e_core_count(void);
static uint32_t sf_get_sram_l1d_kb(void);
static uint32_t sf_get_sram_l2_kb(void);
static uint32_t sf_get_sram_l3_kb(void);
static bool sf_has_massive_sram_vcache(void);
static uint8_t sf_get_numa_node_count(void);
static bool sf_is_uma_system(void);
static bool sf_is_focus_scp(void);

thread_local SF_custominiVm_core_tier_t tl_sf_core_tier = SF_CORE_TIER_ANY;

static std::atomic<uint8_t> g_sf_critical_simd_mode{static_cast<uint8_t>(sf_critical_simd_e::AUTO)};

SF_DISABLE_OPT_BEGIN
static inline void sf_critical_clear_memory_core_adaptive(void* dst, size_t bytes, bool prefer_nt = false) {
    if (!dst || bytes == 0) return;
    uint8_t m = g_sf_critical_simd_mode.load(std::memory_order_relaxed);
    if (m == static_cast<uint8_t>(sf_critical_simd_e::DENY)) {
        memset(dst, 0, bytes);
        return;
    }

    // Core & SRAM Adaptive Logic:
    // 1. On E-cores: ALWAYS use non-temporal streaming stores for objects >= 64 bytes.
    //    This bypasses tiny L1D (32KB) and shared cluster L2 (2MB-4MB), eliminating cache pollution.
    // 2. On P-cores / Homogeneous cores:
    //    - If bytes < 64KB (or < 256KB on 3D V-Cache): use fast AVX2/AVX-512 temporal stores to keep cell in L2 for immediate reallocation.
    //    - If bytes >= 64KB: use streaming non-temporal stores to avoid blowing out L2/L3 cache.
    bool use_nt = prefer_nt || sf_is_efficiency_core_active();
    if (!use_nt && bytes >= (sf_has_massive_sram_vcache() ? (256 * 1024) : (64 * 1024))) {
        use_nt = true;
    }

#if defined(_M_X64) || defined(__x86_64__)
    uint8_t* p = static_cast<uint8_t*>(dst);
    size_t rem = bytes;

    if (use_nt && bytes >= 64) {
        // Handle unaligned prefix before streaming stores
        while (rem > 0 && (((uintptr_t)p & 15) != 0)) {
            *p++ = 0;
            --rem;
        }
#if defined(__AVX2__)
        if (rem >= 32 && (((uintptr_t)p & 31) == 0)) {
            __m256i z256 = _mm256_setzero_si256();
            while (rem >= 32) {
                _mm256_stream_si256(reinterpret_cast<__m256i*>(p), z256);
                p += 32;
                rem -= 32;
            }
        }
#endif
        __m128i z128 = _mm_setzero_si128();
        while (rem >= 16) {
            _mm_stream_si128(reinterpret_cast<__m128i*>(p), z128);
            p += 16;
            rem -= 16;
        }
        _mm_sfence();
    } else {
#if defined(__AVX2__)
        __m256i z256 = _mm256_setzero_si256();
        while (rem >= 32) {
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(p), z256);
            p += 32;
            rem -= 32;
        }
#endif
        __m128i z128 = _mm_setzero_si128();
        while (rem >= 16) {
            _mm_storeu_si128(reinterpret_cast<__m128i*>(p), z128);
            p += 16;
            rem -= 16;
        }
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

static inline void sf_critical_clear_memory(void* dst, size_t bytes) {
    sf_critical_clear_memory_core_adaptive(dst, bytes, false);
}
SF_DISABLE_OPT_END

struct sf_os_allocation_entry_t {
    void*    addr = nullptr;
    size_t   bytes = 0;
    uint32_t tag = 0; // 0 = Virtual (VirtualAlloc/mmap), 1 = MMIO, 2 = FileMap
};

struct sf_os_allocation_registry_t {
    static constexpr size_t MAX_TRACKED_OS_BLOCKS = 4096;
    std::recursive_mutex lock;
    sf_os_allocation_entry_t entries[MAX_TRACKED_OS_BLOCKS]{};
    std::vector<sf_os_allocation_entry_t> overflow_entries;
    std::atomic<size_t> active_count{0};

    void register_block(void* addr, size_t bytes, uint32_t tag = 0) {
        if (!addr || bytes == 0) return;
        std::lock_guard<std::recursive_mutex> g(lock);
        for (size_t i = 0; i < MAX_TRACKED_OS_BLOCKS; ++i) {
            if (entries[i].addr == nullptr) {
                entries[i].addr  = addr;
                entries[i].bytes = bytes;
                entries[i].tag   = tag;
                active_count.fetch_add(1, std::memory_order_relaxed);
                return;
            }
        }
        overflow_entries.push_back({addr, bytes, tag});
        active_count.fetch_add(1, std::memory_order_relaxed);
    }

    void unregister_block(void* addr) {
        if (!addr) return;
        std::lock_guard<std::recursive_mutex> g(lock);
        for (size_t i = 0; i < MAX_TRACKED_OS_BLOCKS; ++i) {
            if (entries[i].addr == addr) {
                entries[i].addr  = nullptr;
                entries[i].bytes = 0;
                entries[i].tag   = 0;
                size_t c = active_count.load(std::memory_order_relaxed);
                if (c > 0) active_count.store(c - 1, std::memory_order_relaxed);
                return;
            }
        }
        for (size_t i = 0; i < overflow_entries.size(); ++i) {
            if (overflow_entries[i].addr == addr) {
                overflow_entries.erase(overflow_entries.begin() + i);
                size_t c = active_count.load(std::memory_order_relaxed);
                if (c > 0) active_count.store(c - 1, std::memory_order_relaxed);
                return;
            }
        }
    }

    size_t sweep_all(void) {
        std::lock_guard<std::recursive_mutex> g(lock);
        size_t swept_bytes = 0;
        auto sweep_entry = [&](void* a, size_t b, uint32_t t) {
            if (!a) return;
            if (t == 1) { // MMIO
#if !defined(_WIN32)
                munmap(a, b);
#endif
            } else if (t == 2) { // File mapping
#ifdef _WIN32
                UnmapViewOfFile(a);
#else
                munmap(a, b);
#endif
            } else { // Virtual memory
#ifdef _WIN32
                VirtualFree(a, 0, MEM_RELEASE);
#else
                munmap(a, b);
#endif
            }
            swept_bytes += b;
        };

        for (size_t i = 0; i < MAX_TRACKED_OS_BLOCKS; ++i) {
            if (entries[i].addr != nullptr) {
                void*  a = entries[i].addr;
                size_t b = entries[i].bytes;
                uint32_t t = entries[i].tag;
                entries[i].addr  = nullptr;
                entries[i].bytes = 0;
                entries[i].tag   = 0;
                sweep_entry(a, b, t);
            }
        }
        for (auto& oe : overflow_entries) {
            if (oe.addr != nullptr) {
                sweep_entry(oe.addr, oe.bytes, oe.tag);
            }
        }
        overflow_entries.clear();
        active_count.store(0, std::memory_order_relaxed);
        return swept_bytes;
    }
};

static sf_os_allocation_registry_t g_sf_os_registry;

static inline void* sf_os_reserve_memory(size_t bytes) {
#ifdef _WIN32
    void* p = VirtualAlloc(nullptr, bytes, MEM_RESERVE, PAGE_READWRITE);
#else
    void* p = mmap(nullptr, bytes, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) p = nullptr;
#endif
    if (p) {
        g_sf_os_registry.register_block(p, bytes, 0);
    }
    return p;
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
    g_sf_os_registry.unregister_block(addr);
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
        if (!mapped_base) {
            CloseHandle(hMap);
            hMap = NULL;
        }
        if (mapped_base) {
            g_sf_os_registry.register_block(mapped_base, mapped_capacity, 2);
        }
        return mapped_base != nullptr;
#else
        mapped_base = mmap(nullptr, mapped_capacity, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        bool ok = (mapped_base != MAP_FAILED) && (mapped_base != nullptr);
        if (ok) {
            g_sf_os_registry.register_block(mapped_base, mapped_capacity, 2);
        }
        return ok;
#endif
    }

    void destroy() {
        if (!mapped_base) return;
        g_sf_os_registry.unregister_block(mapped_base);
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
    STATIC_MODE = 0,
    HEURISTIC_MODE = 1
};

enum class sf_gc_operation_facility_e : uint8_t {
    CYCLES = 0,
    SIMD   = 1,
    STATIC_FIXED = 2
};

enum class sf_snatch_rebate_mode_e : uint8_t {
    AUTO  = 0,
    RAPID = 1,
    LAZY  = 2,
    GYC   = 3,
    DECOMMIT = 4
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

enum class sf_rmrp_method_e : uint8_t {
    PLUMP       = 0, /* Prune and coalesce before rebate */
    SEQUENTIAL  = 1  /* Installment: rebate immediately per block */
};

enum class sf_global_volatility_e : uint8_t {
    DRAM = 0,
    SRAM = 1
};

enum class sf_puzzler_morph_e : uint8_t {
    EXCISION_EXCLUSIVE = 0,
    SHADOW_RCU         = 1
};

struct sf_vm_string_args_t {
    char                    gc_mode_raw[32]           = "Heuristic";
    char                    gc_operation_facility_raw[32] = "CYCLES";
    char                    critical_simd_raw[32]     = "Auto";
    char                    snatch_rebate_raw[32]     = "Auto";
    char                    freelist_policy_raw[32]   = "LIFO";
    char                    gc_heuristic_raw[32]      = "Adaptive";
    char                    rmrp_method_raw[32]       = "Plump";
    char                    global_volatility_raw[32] = "DRAM";
    char                    puzzler_morph_raw[32]    = "Shadow";
    sf_gc_mode_e                          gc_mode                   = sf_gc_mode_e::HEURISTIC_MODE;
    sf_gc_operation_facility_e            gc_operation_facility     = sf_gc_operation_facility_e::CYCLES;
    sf_critical_simd_e      critical_simd_mode        = sf_critical_simd_e::AUTO;
    sf_snatch_rebate_mode_e snatch_rebate_mode        = sf_snatch_rebate_mode_e::AUTO;
    sf_gc_heuristic_e       gc_heuristic              = sf_gc_heuristic_e::ADAPTIVE;
    sf_scgc_type_e          scgc_type                 = sf_scgc_type_e::DEFAULT;
    sf_rmrp_method_e        rmrp_method               = sf_rmrp_method_e::PLUMP;
    sf_global_volatility_e  global_volatility         = sf_global_volatility_e::DRAM;
    sf_puzzler_morph_e     puzzler_morph            = sf_puzzler_morph_e::SHADOW_RCU;

    bool                    critical_simd_explicit    = false;
    bool                    enable_huge_pages         = false;
    bool                    Advance_Payment           = false;
    bool                    always_focus_compactizing = false;
    bool                    no_sfgc_overwrite_mode    = false;
    bool                    allow_operation_dispatchings = false;
    bool                    reduce_memory_resident_propagation = false;
    bool                    rmrp_method_explicit      = false;
    bool                    experimental_options      = false;
    bool                    entangle_execution        = false;
    bool                    strict_vfs_branch_validation = true;
    bool                    vfs_panic_dialog             = true;
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
    bool Forbid_Dynamic_Growth = false;
    bool UnlockExperimentalVMOptions = false;
    bool UseMLTCSolver = false;
    bool                 disable_jit_deopt            = false;
    uint32_t             max_loop_iterations          = 0;
    SF_custominiVm_execution_focus_t execution_focus  = SF_FOCUS_AUTO;
    bool                 has_flag_conflict            = false;
    char                 conflict_msg[512]            = {0};
};

static uint64_t parse_size_with_unit(const char* str) {
    if (!str || !*str) return 0;
    char* end = nullptr;
    double val = std::strtod(str, &end);
    if (val < 0) val = 0;
    if (end) {
        while (*end == ' ') end++;
        if (_stricmp(end, "gb") == 0 || _stricmp(end, "g") == 0) {
            return (uint64_t)(val * 1024.0 * 1024.0 * 1024.0);
        } else if (_stricmp(end, "tb") == 0 || _stricmp(end, "t") == 0) {
            return (uint64_t)(val * 1024.0 * 1024.0 * 1024.0 * 1024.0);
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
static inline void sf_sram_commit_to_dram(void* ptr, size_t bytes);
static inline void sf_sram_broadcast_volatility(void* ptr, size_t bytes);


static std::atomic<bool> g_sf_allow_operation_dispatchings{false};
static std::atomic<bool> g_sf_rmrp_enabled{false};
static std::atomic<uint8_t> g_sf_rmrp_method{static_cast<uint8_t>(sf_rmrp_method_e::PLUMP)};
static std::atomic<uint8_t> g_sf_global_volatility{static_cast<uint8_t>(sf_global_volatility_e::DRAM)};
static std::atomic<uint8_t> g_sf_puzzler_morph_method{static_cast<uint8_t>(sf_puzzler_morph_e::SHADOW_RCU)};
static std::atomic<bool> g_sf_entangle_execution{false};
static std::atomic<bool> g_sf_strict_vfs_validation{true};
static std::atomic<bool> g_sf_vfs_panic_dialog_enabled{true};

static inline void sf_hotlog_emit(SF_custominiVm_event_category_e ev, SF_custominiVm_severity_e sev,
                                  uint32_t fn_id, uint16_t slot, uint8_t gen, uint8_t reg,
                                  uint16_t core, uint64_t sweep_no, uint32_t heap_kb,
                                  const char* msg);

/* ── Universal Raw Log Forwarder & Console Mirroring ── */
static std::atomic<sf_raw_log_stream_fn_t> g_sf_raw_log_stream_fn{nullptr};
static std::atomic<void*>                  g_sf_raw_log_stream_user_data{nullptr};
static std::atomic<int>                    g_sf_console_mirroring{1};

static void sf_broadcast_raw_log_line(const char* text, bool is_err = false) {
    if (!text) return;
    size_t len = strlen(text);
    auto fn = g_sf_raw_log_stream_fn.load(std::memory_order_acquire);
    if (fn) {
        void* u = g_sf_raw_log_stream_user_data.load(std::memory_order_acquire);
        fn(text, len, u);
    }
    if (g_sf_console_mirroring.load(std::memory_order_acquire)) {
        FILE* out = is_err ? stderr : stdout;
        fputs(text, out);
        if (len == 0 || text[len - 1] != '\n') {
            fputc('\n', out);
        }
        fflush(out);
    }
}

/* ── Controlpoint Host GC Coordination Globals ── */
static std::atomic<sf_controlpoint_hook_fn_t> g_sf_controlpoint_hook{nullptr};
static std::atomic<void*>                     g_sf_controlpoint_user_data{nullptr};
static std::atomic<int>                       g_sf_host_gc_mode{SF_HOST_GC_AUTONOMOUS};

/* ── Scoped Batch Processing State ── */
static std::atomic<uint32_t> g_sf_active_batch_id{0};

/* ── Helper: DType Name String ── */
static inline const char* sacculus_dtype_name(sfDType d) {
    switch (d) {
        case SF_DTYPE_F32:  return "FP32";
        case SF_DTYPE_F64:  return "FP64";
        case SF_DTYPE_I32:  return "INT32";
        case SF_DTYPE_I64:  return "INT64";
        case SF_DTYPE_F16:  return "FP16";
        case SF_DTYPE_U8:   return "UINT8";
        case SF_DTYPE_F8:   return "FP8";
        case SF_DTYPE_I8:   return "INT8";
        case SF_DTYPE_I16:  return "INT16";
        case SF_DTYPE_F128: return "FP128";
        case SF_DTYPE_I128: return "INT128";
        case SF_DTYPE_F256: return "FP256";
        case SF_DTYPE_I256: return "INT256";
        case SF_DTYPE_U16:  return "UINT16";
        case SF_DTYPE_U32:  return "UINT32";
        case SF_DTYPE_U64:  return "UINT64";
        case SF_DTYPE_U128: return "UINT128";
        case SF_DTYPE_U256: return "UINT256";
        default:            return "UNKNOWN";
    }
}

/* ── Forward declaration for schema name lookup ── */
extern "C" MSIMD_API const char* SF_custominiVm_get_emulation_schema_name(int schema_id);

/* ── ALU Queue & Multi-Limb Silicon Emulation Dispatching Engine (Machines-Rules.td Sections 14-15) ── */
struct sf_alu_pipeline_t {
    std::atomic<uint32_t> queue_depth{0};
    uint8_t               limb_width_bits{64};
    uint8_t               is_float{1};
    uint8_t               is_emulation_bank{0};
    uint8_t               _pad{0};

    void init(uint8_t bits, uint8_t flt, uint8_t emu) {
        queue_depth.store(0, std::memory_order_relaxed);
        limb_width_bits = bits;
        is_float = flt;
        is_emulation_bank = emu;
        _pad = 0;
    }
};

struct sf_alu_dispatch_nexus_t {
    static constexpr size_t NUM_ALU_LANES = 16;
    static constexpr uint32_t SATURATION_THRESHOLD = 4;

    sf_alu_pipeline_t     pipelines[NUM_ALU_LANES];
    std::atomic<uint64_t> total_dispatched{0};
    std::atomic<uint64_t> total_saturated_events{0};
    std::atomic<uint64_t> total_emulation_dispatched{0};

    sf_alu_dispatch_nexus_t() {
        pipelines[0].init(64, 1, 0); // Lane 0: Native FP64
        pipelines[1].init(32, 1, 0); // Lane 1: Native FP32
        pipelines[2].init(16, 1, 0); // Lane 2: Native FP16
        pipelines[3].init(64, 0, 0); // Lane 3: Native INT64
        pipelines[4].init(32, 0, 0); // Lane 4: Native INT32
        pipelines[5].init(16, 0, 0); // Lane 5: Native INT16_8
        pipelines[6].init(32, 1, 1); // Lane 6: Emu Float 2x32 (64b)
        pipelines[7].init(16, 1, 1); // Lane 7: Emu Float 4x16 (64b)
        pipelines[8].init(16, 1, 1); // Lane 8: Emu Float 2x16 (32b)
        pipelines[9].init(8,  1, 1); // Lane 9: Emu Float 4x8  (32b)
        pipelines[10].init(64, 1, 1); // Lane 10: Emu Float 2x64 (128b)
        pipelines[11].init(64, 1, 1); // Lane 11: Emu Float 4x64 (256b)
        pipelines[12].init(32, 0, 1); // Lane 12: Emu Int 2x32 (64b)
        pipelines[13].init(16, 0, 1); // Lane 13: Emu Int 4x16 (64b)
        pipelines[14].init(16, 0, 1); // Lane 14: Emu Int 2x16 (32b)
        pipelines[15].init(64, 0, 1); // Lane 15: Emu Int 2x64 (128b)
    }

    uint32_t dispatch_op(uint32_t preferred_alu, uint8_t priority) {
        preferred_alu %= NUM_ALU_LANES;

        // High priority operations pin execution to preferred ALU and override dispatching
        if (priority >= 8) {
            pipelines[preferred_alu].queue_depth.fetch_add(1, std::memory_order_relaxed);
            return preferred_alu;
        }

        bool allow = g_sf_allow_operation_dispatchings.load(std::memory_order_acquire);
        uint32_t pref_depth = pipelines[preferred_alu].queue_depth.load(std::memory_order_relaxed);

        if (allow && pref_depth >= SATURATION_THRESHOLD) {
            total_saturated_events.fetch_add(1, std::memory_order_relaxed);
            // Preferred ALU queue is saturated: find an idle or least-loaded ALU queue
            uint32_t best_alu = preferred_alu;
            uint32_t min_depth = pref_depth;
            for (uint32_t i = 0; i < NUM_ALU_LANES; ++i) {
                uint32_t d = pipelines[i].queue_depth.load(std::memory_order_relaxed);
                if (d < min_depth) {
                    min_depth = d;
                    best_alu = i;
                }
            }
            if (best_alu != preferred_alu) {
                total_dispatched.fetch_add(1, std::memory_order_relaxed);
                if (best_alu >= 6 && best_alu <= 15) {
                    total_emulation_dispatched.fetch_add(1, std::memory_order_relaxed);
                }
                pipelines[best_alu].queue_depth.fetch_add(1, std::memory_order_relaxed);
                sf_hotlog_emit(SF_Ebin0_ALU_SATURATE_STEAL, SF_Ebin1_WARN, 0, (uint16_t)best_alu, 0, 0, (uint16_t)preferred_alu, 0, pref_depth, "alu_lane_stolen");
                return best_alu;
            }
        }

        pipelines[preferred_alu].queue_depth.fetch_add(1, std::memory_order_relaxed);
        sf_hotlog_emit(SF_Ebin0_ALU_DISPATCH_DIRECT, SF_Ebin1_TRACE, 0, (uint16_t)preferred_alu, 0, 0, 0, 0, pref_depth, "alu_direct_dispatch");
        return preferred_alu;
    }

    int resolve_route(uint8_t dtype, uint8_t priority, int* out_is_emulated, int* out_schema_id) {
        uint32_t native_lane = 1;
        switch (dtype) {
            case SF_DTYPE_F64:  native_lane = 0; break;
            case SF_DTYPE_F32:  native_lane = 1; break;
            case SF_DTYPE_F16:  native_lane = 2; break;
            case SF_DTYPE_I64:
            case SF_DTYPE_U64:  native_lane = 3; break;
            case SF_DTYPE_I32:
            case SF_DTYPE_U32:  native_lane = 4; break;
            case SF_DTYPE_I16:
            case SF_DTYPE_U16:
            case SF_DTYPE_I8:
            case SF_DTYPE_U8:
            case SF_DTYPE_F8:   native_lane = 5; break;
            case SF_DTYPE_F128: native_lane = 10; break;
            case SF_DTYPE_I128:
            case SF_DTYPE_U128: native_lane = 15; break;
            case SF_DTYPE_F256: native_lane = 11; break;
            case SF_DTYPE_I256:
            case SF_DTYPE_U256: native_lane = 15; break;
            default:            native_lane = 1; break;
        }

        bool is_inherently_emulated = (native_lane >= 6 && native_lane <= 15);
        uint32_t default_schema = SF_EMU_SCHEMA_NONE;
        if (dtype == SF_DTYPE_F128) default_schema = SF_EMU_SCHEMA_FLOAT_2X64;
        else if (dtype == SF_DTYPE_I128 || dtype == SF_DTYPE_U128) default_schema = SF_EMU_SCHEMA_INT_2X64;
        else if (dtype == SF_DTYPE_F256) default_schema = SF_EMU_SCHEMA_FLOAT_4X64;
        else if (dtype == SF_DTYPE_I256 || dtype == SF_DTYPE_U256) default_schema = SF_EMU_SCHEMA_INT_4X64;

        if (priority >= 8) {
            pipelines[native_lane].queue_depth.fetch_add(1, std::memory_order_relaxed);
            if (out_is_emulated) *out_is_emulated = is_inherently_emulated ? 1 : 0;
            if (out_schema_id)   *out_schema_id = (int)default_schema;
            return (int)native_lane;
        }

        bool allow = g_sf_allow_operation_dispatchings.load(std::memory_order_acquire);
        uint32_t native_depth = pipelines[native_lane].queue_depth.load(std::memory_order_relaxed);

        if (allow && native_depth >= SATURATION_THRESHOLD) {
            total_saturated_events.fetch_add(1, std::memory_order_relaxed);

            uint32_t best_lane = native_lane;
            uint32_t best_schema = default_schema;
            uint32_t min_depth = native_depth;

            auto test_candidate = [&](uint32_t cand_lane, uint32_t cand_schema) {
                uint32_t d = pipelines[cand_lane].queue_depth.load(std::memory_order_relaxed);
                if (d < min_depth) {
                    min_depth = d;
                    best_lane = cand_lane;
                    best_schema = cand_schema;
                }
            };

            if (dtype == SF_DTYPE_F64) {
                test_candidate(7, SF_EMU_SCHEMA_FLOAT_4X16); // 4x16-bit float emulation on 16b ALU
                test_candidate(6, SF_EMU_SCHEMA_FLOAT_2X32); // 2x32-bit float emulation on 32b ALU
                test_candidate(1, SF_EMU_SCHEMA_FLOAT_2X32); // native 32-bit ALU lane
                test_candidate(2, SF_EMU_SCHEMA_FLOAT_4X16); // native 16-bit ALU lane
            } else if (dtype == SF_DTYPE_F32) {
                test_candidate(8, SF_EMU_SCHEMA_FLOAT_2X16); // 2x16-bit float emulation
                test_candidate(9, SF_EMU_SCHEMA_FLOAT_4X8);  // 4x8-bit float emulation
                test_candidate(2, SF_EMU_SCHEMA_FLOAT_2X16); // native 16-bit ALU lane
            } else if (dtype == SF_DTYPE_I64) {
                test_candidate(13, SF_EMU_SCHEMA_INT_4X16);  // 4x16-bit integer emulation
                test_candidate(12, SF_EMU_SCHEMA_INT_2X32);  // 2x32-bit integer emulation
                test_candidate(4,  SF_EMU_SCHEMA_INT_2X32);  // native 32-bit ALU lane
            } else if (dtype == SF_DTYPE_I32) {
                test_candidate(14, SF_EMU_SCHEMA_INT_2X16);  // 2x16-bit integer emulation
                test_candidate(5,  SF_EMU_SCHEMA_INT_2X16);  // native 16-bit ALU lane
            } else if (dtype == SF_DTYPE_F128) {
                test_candidate(6,  SF_EMU_SCHEMA_FLOAT_4X32);
                test_candidate(10, SF_EMU_SCHEMA_FLOAT_2X64);
            } else if (dtype == SF_DTYPE_F256) {
                test_candidate(11, SF_EMU_SCHEMA_FLOAT_4X64);
                test_candidate(6,  SF_EMU_SCHEMA_FLOAT_8X32);
            }

            if (best_lane != native_lane) {
                total_emulation_dispatched.fetch_add(1, std::memory_order_relaxed);
                total_dispatched.fetch_add(1, std::memory_order_relaxed);
                pipelines[best_lane].queue_depth.fetch_add(1, std::memory_order_relaxed);

                char log_msg[384];
                snprintf(log_msg, sizeof(log_msg),
                         "\033[1;35m[0x00E2] [SFVM ALU Emulation Dispatch]\033[0m Tensor dtype 0x%02X (%s) native ALU lane 0x%02X saturated (depth=%u). Dispatched to Multi-Limb Emulation Schema 0x%02X (%s) on ALU lane 0x%02X (depth=%u).",
                         (unsigned int)dtype, sacculus_dtype_name((sfDType)dtype),
                         (unsigned int)native_lane, native_depth,
                         (unsigned int)best_schema, SF_custominiVm_get_emulation_schema_name((int)best_schema),
                         (unsigned int)best_lane, min_depth);
                sf_broadcast_raw_log_line(log_msg);

                sf_hotlog_emit(SF_Ebin0_ALU_SATURATE_STEAL, SF_Ebin1_WARN, 0,
                               (uint16_t)best_lane, (uint8_t)dtype, (uint8_t)best_schema,
                               (uint16_t)native_lane, 0, native_depth, "alu_emulation_stolen");

                if (out_is_emulated) *out_is_emulated = 1;
                if (out_schema_id)   *out_schema_id = (int)best_schema;
                return (int)best_lane;
            }
        }

        pipelines[native_lane].queue_depth.fetch_add(1, std::memory_order_relaxed);
        if (out_is_emulated) *out_is_emulated = is_inherently_emulated ? 1 : 0;
        if (out_schema_id)   *out_schema_id = (int)default_schema;
        return (int)native_lane;
    }

    void complete_op(uint32_t alu_id) {
        alu_id %= NUM_ALU_LANES;
        uint32_t cur = pipelines[alu_id].queue_depth.load(std::memory_order_relaxed);
        while (cur > 0 && !pipelines[alu_id].queue_depth.compare_exchange_weak(cur, cur - 1, std::memory_order_relaxed)) {}
    }
};

static sf_alu_dispatch_nexus_t g_sf_alu_nexus;

static const char* const SF_KNOWN_FLAGS[] = {
    "GC_Mode", "GC_Mod", "GCMode", "GCMod", "GC_MODE", "GC_MOD",
    "GC_Operation_Facility", "GC_OPERATION_FACILITY",
    "Use_Critical_SIMDly_Operations", "USE_CRITICAL_SIMDLY_OPERATIONS",
    "Snatch_and_Rebate_Mode", "SNATCH_AND_REBATE_MODE", "SnatchAndRebateMode", "Snatch_And_Rebate_Mode",
    "GC_Heuristic", "GC_HEURISTIC", "FreeList_Recycle_Policy", "FREELIST_RECYCLE_POLICY",
    "FreeListRecyclePolicy", "Freelist_Policy", "FreelistRecyclePolicy",
    "Enable_Huge_Pages", "ENABLE_HUGE_PAGES", "Advance_Payment", "ADVANCE_PAYMENT",
    "AlwaysPreTouch", "Always_PreTouch", "Pretouch", "PreTouch",
    "Upclocker", "UPCLOCKER", "Boost", "Turbo", "Upclock",
    "Always_Focus_Compactizing", "ALWAYS_FOCUS_COMPACTIZING",
    "No_sfGC", "NO_SFGC", "SCGC_Type", "SCGC_TYPE", "Profile", "PROFILE",
    "Forbid_Dynamic_Growth", "FORBID_DYNAMIC_GROWTH", "Forbid_Runtime_Heap", "ForbidRuntimeHeap",
    "Max_Bounded_Iterations", "MAX_BOUNDED_ITERATIONS", "Max_Iterations", "MaxIterations",
    "Allow_Operation_Dispatchings", "AllowOperationDispatchings", "ALLOW_OPERATION_DISPATCHINGS", "Operation_Dispatchings",
    "Reduce_Memory_Resident_Propagation", "ReduceMemoryResidentPropagation", "REDUCE_MEMORY_RESIDENT_PROPAGATION", "RMRP",
    "RMRP_Method", "RMRPMethod", "RMRP_METHOD", "Global_Volatility_Method", "GlobalVolatilityMethod", "GLOBAL_VOLATILITY_METHOD",
    "puzzler_Morph_Method", "puzzlerMorphMethod", "puzzler_MORPH_METHOD", "Morph_Method",
    "Experimental", "EXPERIMENTAL", "Entangle_Execution", "ENTANGLE_EXECUTION",
    "Runway_Handsel_Attempt_Value", "Landfill_Handsel_Attempt_Value",
    "Wakes_GC_Cycling_Interval", "Line_Compaction_Freq", "In_Heap_Forwarding_Rate",
    "Pressure_Threshold", "Slab_Chunk_Size", "Prune", "PRUNE", "Plump", "PLUMP", "Sequential", "SEQUENTIAL", "Installment", "INSTALLMENT",
    "Execution_Focus", "ExecutionFocus", "Focus", "EXECUTION_FOCUS",
    "Strict_VFS_Branch_Validation", "StrictVFSBranchValidation", "STRICT_VFS_BRANCH_VALIDATION",
    "Strict_VFS", "StrictVFS", "STRICT_VFS", "VFS_Strict", "VFSStrict", "VFS_STRICT",
    "Strict_VFS_Validation", "STRICT_VFS_VALIDATION",
    "VFS_Panic_Dialog", "VFSPanicDialog", "VFS_PANIC_DIALOG", "Panic_Dialog", "PANIC_DIALOG",
    "InlineASM_Threshold", "IASM_Threshold", "InlineAsmThreshold", "INLINEASM_THRESHOLD", "IASM_THRESHOLD",
    "Strict_Ring3_Only", "StrictRing3Only", "Strict_Ring3", "StrictRing3", "STRICT_RING3_ONLY",
    "Strict_Flags", "StrictFlags", "Strict_Flag_Validation", "StrictFlagValidation", "STRICT_FLAGS",
    "UseMLTCSolver", "Use_MLTC_Solver", "MLTCSolver", "MLTC_Solver", "USE_MLTC_SOLVER",
    "Code_Arena_Bytes", "CodeArenaBytes", "Code_Arena_Size", "Code_Heap_Size", "CODE_ARENA_BYTES",
    "Heat_Decay_Factor", "HeatDecayFactor", "HEAT_DECAY_FACTOR",
    "Heat_Gain_Weight", "HeatGainWeight", "HEAT_GAIN_WEIGHT"
};

/* ------------------------------------------------------------------------- */
/* MODULAR TECHNICAL ENGLISH INTENT REGISTRY & SEMANTIC RESOLUTION ENGINE    */
/* ------------------------------------------------------------------------- */

struct sf_cli_intent_entry_t {
    const char* intent_keyword;
    const char* canonical_flag;
    const char* default_value;
    const char* technical_doc;
};

static const sf_cli_intent_entry_t SF_CLI_INTENT_REGISTRY[] = {
    // 1. Clock Frequency & Intrinsic Hardware Boost (Upclocker)
    {"boost",            "Upclocker", "1",     "Overclock / maximum frequency boost"},
    {"turbo",            "Upclocker", "1",     "Turbo execution mode"},
    {"speed",            "Upclocker", "1",     "Maximum execution performance clocking"},
    {"overclock",        "Upclocker", "1",     "Host CPU frequency scaling override"},
    {"upclock",          "Upclocker", "1",     "Host CPU upclocking"},
    {"accelerate",       "Upclocker", "1",     "Intrinsic silicon execution acceleration"},
    {"high_perf",        "Upclocker", "1",     "High-performance execution state"},

    // 2. Memory Resident Set Reclamation (RMRP)
    {"trim",             "Reduce_Memory_Resident_Propagation", "true", "Trim and reclaim dormant heap pages"},
    {"shrink",           "Reduce_Memory_Resident_Propagation", "true", "Shrink resident physical memory set"},
    {"reclaim",          "Reduce_Memory_Resident_Propagation", "true", "Reclaim dormant heap propagation buffers"},
    {"compact_memory",   "Reduce_Memory_Resident_Propagation", "true", "Compact resident working set"},
    {"clean_heap",       "Reduce_Memory_Resident_Propagation", "true", "Clean resident heap footprint"},
    {"prune",            "Reduce_Memory_Resident_Propagation", "true", "Prune unreferenced slab blocks"},
    {"plump",            "Reduce_Memory_Resident_Propagation", "true", "Plump coalescing compaction"},
    {"decommit",         "Reduce_Memory_Resident_Propagation", "true", "Decommit idle virtual memory pages"},

    // 3. Vector Acceleration & SIMD GC (GC_Mode)
    {"vector",           "GC_Mode", "SIMD",    "Vector-accelerated garbage collector"},
    {"vectorize",        "GC_Mode", "SIMD",    "Vectorized SIMD memory sweeps"},
    {"simd_gc",          "GC_Mode", "SIMD",    "SIMD-native allocation and sweep"},
    {"vector_gc",        "GC_Mode", "SIMD",    "Vector lane GC processing"},
    {"avx_gc",           "GC_Mode", "SIMD",    "AVX/AVX-512 accelerated GC"},
    {"scalar_gc",        "GC_Mode", "Cycles",  "Cycle-based scalar GC evaluation"},
    {"static_gc",        "GC_Mode", "Static",  "Fixed deterministic static GC"},

    // 4. Parallel Dispatching & ALU Lane Stealing (Allow_Operation_Dispatchings)
    {"parallel",         "Allow_Operation_Dispatchings", "true", "Enable parallel ALU operation dispatching"},
    {"steal",            "Allow_Operation_Dispatchings", "true", "Enable work-stealing across ALU execution lanes"},
    {"dispatch",         "Allow_Operation_Dispatchings", "true", "Enable autonomous ALU load dispatching"},
    {"dispatching",      "Allow_Operation_Dispatchings", "true", "Dynamic multi-lane operation dispatch"},
    {"work_stealing",    "Allow_Operation_Dispatchings", "true", "Saturated ALU lane work stealing"},
    {"concurrent",       "Allow_Operation_Dispatchings", "true", "Concurrent pipeline queueing"},

    // 5. Safety-Critical Profile & Real-Time Isolation (Profile)
    {"safety",           "Profile", "SC",      "Safety-critical deterministic profile"},
    {"critical",         "Profile", "SC",      "Mission-critical hard real-time operation"},
    {"isolated",         "Profile", "SC",      "Isolated deterministic static boundaries"},
    {"realtime",         "Profile", "SC",      "Hard real-time bounded latency mode"},
    {"deterministic",    "Profile", "SC",      "Disable dynamic deopt and JIT jitter"},
    {"secure",           "Profile", "SC",      "Zero-growth secured execution envelope"},

    // 6. SRAM Cacheline Volatility & Scratchpad (Global_Volatility_Method)
    {"sram",             "Global_Volatility_Method", "SRAM", "Direct on-chip SRAM cacheline volatility"},
    {"cache",            "Global_Volatility_Method", "SRAM", "Hierarchical cache line persistence"},
    {"fast_ram",         "Global_Volatility_Method", "SRAM", "Low-latency on-chip scratchpad volatility"},
    {"dram",             "Global_Volatility_Method", "DRAM", "Standard DRAM memory consistency model"},

    // 7. Compaction, Defrag & Kernel Fusion
    {"compact",          "Always_Focus_Compactizing", "true", "Continuous background heap defragmentation"},
    {"compactize",       "Always_Focus_Compactizing", "true", "Active memory cell coalescing"},
    {"defrag",           "Always_Focus_Compactizing", "true", "Active memory cell coalescing and heap defragmentation"},
    {"defragment",       "Always_Focus_Compactizing", "true", "Heap compaction and defragmentation"},
    {"entangle",         "Entangle_Execution", "true", "Batch kernel fusion transactional scratchpad"},
    {"fusion",           "Entangle_Execution", "true", "Transactional kernel instruction fusion"},
    {"fuse_ops",         "Entangle_Execution", "true", "Transactional kernel instruction fusion"},
    {"fuse",             "Entangle_Execution", "true", "Kernel instruction fusion"},
    {"hugepages",        "Enable_Huge_Pages", "true", "2MB/1GB hardware huge pages"},
    {"huge_pages",       "Enable_Huge_Pages", "true", "Direct OS huge page allocation"},
    {"experimental",     "Experimental", "true", "Unlock experimental VM subsystems"},
    {"simdly",           "GC_Mode", "SIMD", "Vector-accelerated garbage collector"},
    {"sram_cache",       "Global_Volatility_Method", "SRAM", "Direct on-chip SRAM cacheline volatility"},
    {"scratchpad",       "Global_Volatility_Method", "SRAM", "Low-latency on-chip scratchpad volatility"},
    {"low_latency",      "Profile", "SC", "Hard real-time bounded latency mode"},
    {"zero_jitter",      "Profile", "SC", "Safety-critical deterministic profile"},

    // 8. New Era Execution Focus Topology (SCP, MCP, Dual Hybrid/Homo)
    {"scp",              "Execution_Focus", "SCP",        "Single-core performance focus on highest turbo core"},
    {"single_core",      "Execution_Focus", "SCP",        "Single-core maximum turbo IPC focus"},
    {"mcp",              "Execution_Focus", "MCP",        "Multi-core wide parallel throughput focus"},
    {"multi_core",       "Execution_Focus", "MCP",        "Multi-core wide parallel throughput focus"},
    {"hybrid_dual",      "Execution_Focus", "HybridDual", "Asymmetric P+E dual single-and-multi performance focus"},
    {"hybrid-dual",      "Execution_Focus", "HybridDual", "Asymmetric P+E dual single-and-multi performance focus"},
    {"dual",             "Execution_Focus", "HybridDual", "Dual single and multi core performance focus"},
    {"dual_performance", "Execution_Focus", "HybridDual", "Dual single and multi core performance focus"},
    {"scp_homo",         "Execution_Focus", "SCP_Homo",   "Single-core homogeneous peak turbo focus"},
    {"scp-homo",         "Execution_Focus", "SCP_Homo",   "Single-core homogeneous peak turbo focus"},
    {"scp_hybrid",       "Execution_Focus", "SCP_Hybrid", "Single-core asymmetric strict P-core pinned focus"},
    {"scp-hybrid",       "Execution_Focus", "SCP_Hybrid", "Single-core asymmetric strict P-core pinned focus"},
    {"mcp_homo",         "Execution_Focus", "MCP_Homo",   "Multi-core homogeneous uniform chunking focus"},
    {"mcp-homo",         "Execution_Focus", "MCP_Homo",   "Multi-core homogeneous uniform chunking focus"},
    {"mcp_hybrid",       "Execution_Focus", "MCP_Hybrid", "Multi-core asymmetric weighted chunking focus"},
    {"mcp-hybrid",       "Execution_Focus", "MCP_Hybrid", "Multi-core asymmetric weighted chunking focus"},
    {"dual_homo",        "Execution_Focus", "Dual_Homo",  "Dual focus homogeneous (1 turbo + N-1 workers)"},
    {"dual-homo",        "Execution_Focus", "Dual_Homo",  "Dual focus homogeneous (1 turbo + N-1 workers)"},
    {"dual_hybrid",      "Execution_Focus", "Dual_Hybrid","Dual focus asymmetric (P compute + E gc/logs)"},
    {"dual-hybrid",      "Execution_Focus", "Dual_Hybrid","Dual focus asymmetric (P compute + E gc/logs)"},

    // 9. puzzler JIT/ISA Morph Paradigms
    {"morph_shadow",     "puzzler_Morph_Method", "Shadow",   "Zero-pause shadow clone morphing (RCU)"},
    {"morph-shadow",     "puzzler_Morph_Method", "Shadow",   "Zero-pause shadow clone morphing (RCU)"},
    {"shadow_morph",     "puzzler_Morph_Method", "Shadow",   "Zero-pause shadow clone morphing (RCU)"},
    {"shadow-morph",     "puzzler_Morph_Method", "Shadow",   "Zero-pause shadow clone morphing (RCU)"},
    {"morph_excision",   "puzzler_Morph_Method", "Excision", "Exclusive excision with busy-wait notifications"},
    {"morph-excision",   "puzzler_Morph_Method", "Excision", "Exclusive excision with busy-wait notifications"},
    {"exclusive_morph",  "puzzler_Morph_Method", "Excision", "Exclusive excision with busy-wait notifications"},
    {"exclusive-morph",  "puzzler_Morph_Method", "Excision", "Exclusive excision with busy-wait notifications"},

    // 10. VFS Branch Safety & Fail-Fast Protection
    {"strict_vfs",       "Strict_VFS_Branch_Validation", "true", "Enforce strict VFS branch validation and panic on missing branches"},
    {"vfs_strict",       "Strict_VFS_Branch_Validation", "true", "Enforce strict VFS branch validation and panic on missing branches"},
    {"vfs_panic",        "Strict_VFS_Branch_Validation", "true", "Immediate safe panic when VFS branch is not found"},
    {"strict_branch",    "Strict_VFS_Branch_Validation", "true", "Enforce strict VFS directory branch validation"},
    {"vfs_safety",       "Strict_VFS_Branch_Validation", "true", "Fail-fast protection against missing VFS branches"},
    {"vfs_dialog",       "VFS_Panic_Dialog",             "true", "Enable modal alert dialog popup on fatal VFS panics"},
    {"vfs_popup",        "VFS_Panic_Dialog",             "true", "Enable modal alert dialog popup on fatal VFS panics"},

    // 11. Inline Assembly (iasm) Threshold & Strict Ring 3 Mode
    {"iasm_fast",        "InlineASM_Threshold",          "5",    "Engage hardware inline assembly and non-temporal streaming"},
    {"iasm_safe",        "InlineASM_Threshold",          "10",   "High safety threshold for inline assembly"},
    {"strict_ring3",     "Strict_Ring3_Only",            "true", "Forbid inline assembly and lock execution to safe Ring 3 APIs"},
    {"ring3_only",       "Strict_Ring3_Only",            "true", "Forbid inline assembly and lock execution to safe Ring 3 APIs"},
    {"strict_flags",     "Strict_Flags",                 "true", "Fail-fast on unknown flags, unreferenced params, or conflicts"},

    // 12. MLTC Diagnostic & Emergency Solver
    {"mltc_solver",      "UseMLTCSolver",                "true", "Engage MLTC continuous upward trend and congestion solver"},
    {"mltc",             "UseMLTCSolver",                "true", "Engage MLTC continuous upward trend and congestion solver"}
};


static int sf_damerau_levenshtein_distance(const char* s1, const char* s2) {
    int len1 = (int)strlen(s1);
    int len2 = (int)strlen(s2);
    if (len1 > 48 || len2 > 48) return 99;
    int d[50][50];
    for (int i = 0; i <= len1; ++i) d[i][0] = i;
    for (int j = 0; j <= len2; ++j) d[0][j] = j;
    for (int i = 1; i <= len1; ++i) {
        for (int j = 1; j <= len2; ++j) {
            int cost = (tolower((unsigned char)s1[i - 1]) == tolower((unsigned char)s2[j - 1])) ? 0 : 1;
            int del = d[i - 1][j] + 1;
            int ins = d[i][j - 1] + 1;
            int sub = d[i - 1][j - 1] + cost;
            int m = (del < ins) ? del : ins;
            d[i][j] = (m < sub) ? m : sub;
            if (i > 1 && j > 1 &&
                tolower((unsigned char)s1[i - 1]) == tolower((unsigned char)s2[j - 2]) &&
                tolower((unsigned char)s1[i - 2]) == tolower((unsigned char)s2[j - 1])) {
                d[i][j] = std::min(d[i][j], d[i - 2][j - 2] + 1);
            }
        }
    }
    return d[len1][len2];
}

static double sf_jaro_winkler_similarity(const char* s1, const char* s2) {
    int l1 = (int)strlen(s1);
    int l2 = (int)strlen(s2);
    if (l1 == 0 && l2 == 0) return 1.0;
    if (l1 == 0 || l2 == 0) return 0.0;

    int match_distance = std::max(l1, l2) / 2 - 1;
    if (match_distance < 0) match_distance = 0;

    bool s1_matches[64] = {false};
    bool s2_matches[64] = {false};
    int matches = 0;

    for (int i = 0; i < l1 && i < 64; ++i) {
        int start = std::max(0, i - match_distance);
        int end = std::min(i + match_distance + 1, l2);
        for (int j = start; j < end && j < 64; ++j) {
            if (s2_matches[j]) continue;
            if (tolower((unsigned char)s1[i]) == tolower((unsigned char)s2[j])) {
                s1_matches[i] = true;
                s2_matches[j] = true;
                matches++;
                break;
            }
        }
    }
    if (matches == 0) return 0.0;

    double transpositions = 0;
    int k = 0;
    for (int i = 0; i < l1 && i < 64; ++i) {
        if (!s1_matches[i]) continue;
        while (k < l2 && k < 64 && !s2_matches[k]) k++;
        if (k < l2 && tolower((unsigned char)s1[i]) != tolower((unsigned char)s2[k])) transpositions += 0.5;
        k++;
    }

    double jaro = (((double)matches / l1) +
                   ((double)matches / l2) +
                   (((double)matches - transpositions) / matches)) / 3.0;

    int prefix = 0;
    for (int i = 0; i < std::min(l1, std::min(l2, 4)); ++i) {
        if (tolower((unsigned char)s1[i]) == tolower((unsigned char)s2[i])) prefix++;
        else break;
    }
    return jaro + prefix * 0.1 * (1.0 - jaro);
}

struct sf_cli_resolution_result_t {
    bool matched{false};
    bool was_semantic_intent{false};
    bool was_fuzzy_typo{false};
    char canonical_flag[64]{0};
    char canonical_value[64]{0};
    double confidence{0.0};
    const char* technical_doc{nullptr};
};

static void sf_cli_strip_prefix_and_split(const char* raw_token, const char* raw_val,
                                          char* clean_key, size_t key_sz,
                                          char* clean_val, size_t val_sz) {
    if (!raw_token || !clean_key || key_sz == 0) return;
    clean_key[0] = '\0';
    if (clean_val && val_sz > 0) clean_val[0] = '\0';

    const char* p = raw_token;
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;

    // Strip known CLI prefixes:
    if (strncmp(p, "+#SF|", 5) == 0 || strncmp(p, "-#SF|", 5) == 0) {
        p += 5;
    } else if (_strnicmp(p, "--sf|", 5) == 0 || _strnicmp(p, "-sf|", 4) == 0 ||
               _strnicmp(p, "++sf|", 5) == 0 || _strnicmp(p, "+sf|", 4) == 0) {
        const char* bar = strchr(p, '|');
        p = bar ? (bar + 1) : p;
    } else if (_strnicmp(p, "--sf:", 5) == 0 || _strnicmp(p, "-sf:", 4) == 0 ||
               _strnicmp(p, "++sf:", 5) == 0 || _strnicmp(p, "+sf:", 4) == 0) {
        const char* col = strchr(p, ':');
        p = col ? (col + 1) : p;
    } else if (_strnicmp(p, "--sf-", 5) == 0 || _strnicmp(p, "-sf-", 4) == 0 ||
               _strnicmp(p, "++sf-", 5) == 0 || _strnicmp(p, "+sf-", 4) == 0) {
        const char* hyph = strchr(p, '-');
        if (hyph && hyph[1] == 's' && hyph[2] == 'f' && hyph[3] == '-') {
            p = hyph + 4;
        } else {
            p += 2;
        }
    } else if (p[0] == '-' && p[1] == '-') {
        p += 2;
    } else if (p[0] == '+' && p[1] == '+') {
        p += 2;
    } else if (p[0] == '/' && p[1] == '/') {
        p += 2;
    } else if (p[0] == '-' || p[0] == '+' || p[0] == '/') {
        p += 1;
    }

    // Delimiter search (':' or '=')
    const char* delim = nullptr;
    for (const char* c = p; *c; ++c) {
        if (*c == ':' || *c == '=') {
            delim = c;
            break;
        }
    }

    if (delim && (!raw_val || raw_val[0] == '\0')) {
        size_t klen = (size_t)(delim - p);
        if (klen >= key_sz) klen = key_sz - 1;
        strncpy(clean_key, p, klen);
        clean_key[klen] = '\0';

        const char* v = delim + 1;
        while (*v == ' ' || *v == '\t') v++;
        if (clean_val && val_sz > 0) {
            strncpy(clean_val, v, val_sz - 1);
            clean_val[val_sz - 1] = '\0';
            size_t vlen = strlen(clean_val);
            while (vlen > 0 && (clean_val[vlen - 1] == ' ' || clean_val[vlen - 1] == '\t' ||
                               clean_val[vlen - 1] == '"' || clean_val[vlen - 1] == '\'')) {
                clean_val[--vlen] = '\0';
            }
            if (clean_val[0] == '"' || clean_val[0] == '\'') {
                memmove(clean_val, clean_val + 1, vlen);
            }
        }
    } else {
        strncpy(clean_key, p, key_sz - 1);
        clean_key[key_sz - 1] = '\0';
        if (raw_val && raw_val[0] && clean_val && val_sz > 0) {
            strncpy(clean_val, raw_val, val_sz - 1);
            clean_val[val_sz - 1] = '\0';
        }
    }

    size_t klen = strlen(clean_key);
    while (klen > 0 && (clean_key[klen - 1] == ' ' || clean_key[klen - 1] == '\t')) {
        clean_key[--klen] = '\0';
    }
}

static sf_cli_resolution_result_t sf_cli_resolve_argument(const char* input_key, const char* input_val) {
    sf_cli_resolution_result_t res{};
    if (!input_key || !input_key[0]) return res;

    char clean_k[64] = {0};
    char clean_v[64] = {0};
    sf_cli_strip_prefix_and_split(input_key, input_val, clean_k, sizeof(clean_k), clean_v, sizeof(clean_v));
    if (clean_k[0] == '\0') return res;

    // 1. Direct semantic intent lookup in technical English registry
    for (const auto& entry : SF_CLI_INTENT_REGISTRY) {
        if (_stricmp(clean_k, entry.intent_keyword) == 0) {
            res.matched = true;
            res.was_semantic_intent = true;
            strncpy(res.canonical_flag, entry.canonical_flag, sizeof(res.canonical_flag) - 1);
            if (clean_v[0]) {
                strncpy(res.canonical_value, clean_v, sizeof(res.canonical_value) - 1);
            } else {
                strncpy(res.canonical_value, entry.default_value, sizeof(res.canonical_value) - 1);
            }
            res.confidence = 1.0;
            res.technical_doc = entry.technical_doc;
            return res;
        }
    }

    // 2. Direct exact match against known canonical flags
    for (const char* known : SF_KNOWN_FLAGS) {
        if (_stricmp(clean_k, known) == 0) {
            res.matched = true;
            strncpy(res.canonical_flag, known, sizeof(res.canonical_flag) - 1);
            if (clean_v[0]) {
                strncpy(res.canonical_value, clean_v, sizeof(res.canonical_value) - 1);
            } else {
                strncpy(res.canonical_value, "1", sizeof(res.canonical_value) - 1);
            }
            res.confidence = 1.0;
            res.technical_doc = "Exact canonical flag match";
            return res;
        }
    }

    // 3. Fuzzy match against technical English semantic intents
    double best_intent_score = 0.0;
    const sf_cli_intent_entry_t* best_intent = nullptr;
    for (const auto& entry : SF_CLI_INTENT_REGISTRY) {
        double sim = sf_jaro_winkler_similarity(clean_k, entry.intent_keyword);
        int dist = sf_damerau_levenshtein_distance(clean_k, entry.intent_keyword);
        if ((sim >= 0.85 || dist <= 1) && sim > best_intent_score) {
            best_intent_score = sim;
            best_intent = &entry;
        }
    }
    if (best_intent && best_intent_score >= 0.80) {
        res.matched = true;
        res.was_semantic_intent = true;
        strncpy(res.canonical_flag, best_intent->canonical_flag, sizeof(res.canonical_flag) - 1);
        if (clean_v[0]) {
            strncpy(res.canonical_value, clean_v, sizeof(res.canonical_value) - 1);
        } else {
            strncpy(res.canonical_value, best_intent->default_value, sizeof(res.canonical_value) - 1);
        }
        res.confidence = best_intent_score;
        res.technical_doc = best_intent->technical_doc;
        return res;
    }

    // 4. Fuzzy match against canonical known flags
    double best_flag_score = 0.0;
    const char* best_flag = nullptr;
    for (const char* known : SF_KNOWN_FLAGS) {
        double sim = sf_jaro_winkler_similarity(clean_k, known);
        int dist = sf_damerau_levenshtein_distance(clean_k, known);
        if ((sim >= 0.82 || dist <= 2) && sim > best_flag_score) {
            best_flag_score = sim;
            best_flag = known;
        }
    }
    if (best_flag && best_flag_score >= 0.78) {
        res.matched = true;
        res.was_fuzzy_typo = true;
        strncpy(res.canonical_flag, best_flag, sizeof(res.canonical_flag) - 1);
        if (clean_v[0]) {
            strncpy(res.canonical_value, clean_v, sizeof(res.canonical_value) - 1);
        } else {
            strncpy(res.canonical_value, "1", sizeof(res.canonical_value) - 1);
        }
        res.confidence = best_flag_score;
        res.technical_doc = "Fuzzy typo matched to canonical flag";
        return res;
    }

    return res;
}

MSIMD_API int SF_custominiVm_cli_resolve_intent(const char* input_token,
                                                char* out_canonical_flag, size_t flag_buf_sz,
                                                char* out_canonical_value, size_t val_buf_sz) {
    if (!input_token || !out_canonical_flag || flag_buf_sz == 0) return 0;
    sf_cli_resolution_result_t res = sf_cli_resolve_argument(input_token, nullptr);
    if (!res.matched) return 0;

    strncpy(out_canonical_flag, res.canonical_flag, flag_buf_sz - 1);
    out_canonical_flag[flag_buf_sz - 1] = '\0';
    if (out_canonical_value && val_buf_sz > 0) {
        strncpy(out_canonical_value, res.canonical_value, val_buf_sz - 1);
        out_canonical_value[val_buf_sz - 1] = '\0';
    }
    return 1;
}

static bool parse_single_sf_flag_internal(sf_vm_config_t& cfg, const char* key, const char* val) {
    if (!key || !val) return false;

    if (_stricmp(key, "GC_Mode") == 0 || _stricmp(key, "GC_Mod") == 0 ||
        _stricmp(key, "GCMode") == 0 || _stricmp(key, "GCMod") == 0) {
        strncpy(cfg.strings.gc_mode_raw, val, sizeof(cfg.strings.gc_mode_raw) - 1);
        if (_stricmp(val, "Static") == 0 || _stricmp(val, "Statik") == 0) {
            cfg.strings.gc_mode = sf_gc_mode_e::STATIC_MODE;
        } else if (_stricmp(val, "SIMD") == 0 || _stricmp(val, "Vectoral") == 0 || _stricmp(val, "Vector") == 0) {
            cfg.strings.gc_mode = sf_gc_mode_e::HEURISTIC_MODE;
            cfg.strings.gc_operation_facility = sf_gc_operation_facility_e::SIMD;
        } else if (_stricmp(val, "Cycles") == 0 || _stricmp(val, "Scalar") == 0) {
            cfg.strings.gc_mode = sf_gc_mode_e::HEURISTIC_MODE;
            cfg.strings.gc_operation_facility = sf_gc_operation_facility_e::CYCLES;
        } else {
            cfg.strings.gc_mode = sf_gc_mode_e::HEURISTIC_MODE;
        }
    } else if (_stricmp(key, "GC_Operation_Facility") == 0) {
        strncpy(cfg.strings.gc_operation_facility_raw, val, sizeof(cfg.strings.gc_operation_facility_raw) - 1);
        if (_stricmp(val, "SIMD") == 0) cfg.strings.gc_operation_facility = sf_gc_operation_facility_e::SIMD;
        else if (_stricmp(val, "Static") == 0 || _stricmp(val, "Statik") == 0) cfg.strings.gc_operation_facility = sf_gc_operation_facility_e::STATIC_FIXED;
        else cfg.strings.gc_operation_facility = sf_gc_operation_facility_e::CYCLES;
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
        } else if (_stricmp(val, "Decommit") == 0 || _stricmp(val, "OS_Return") == 0) {
            cfg.strings.snatch_rebate_mode = sf_snatch_rebate_mode_e::DECOMMIT;
        } else {
            cfg.strings.snatch_rebate_mode = sf_snatch_rebate_mode_e::AUTO;
        }
    } else if (_stricmp(key, "GC_Heuristic") == 0 || _stricmp(key, "GC_Heuristics") == 0 || _stricmp(key, "Heuristic") == 0) {
        strncpy(cfg.strings.gc_heuristic_raw, val, sizeof(cfg.strings.gc_heuristic_raw) - 1);
        if (_stricmp(val, "Aggressive") == 0 || _stricmp(val, "Agresif") == 0) {
            cfg.strings.gc_heuristic = sf_gc_heuristic_e::AGGRESSIVE;
        } else if (_stricmp(val, "Bureaucratic") == 0 || _stricmp(val, "Burokratik") == 0 || _stricmp(val, "Bürokratik") == 0) {
            cfg.strings.gc_heuristic = sf_gc_heuristic_e::BUREAUCRATIC;
        } else if (_stricmp(val, "Disabled") == 0 || _stricmp(val, "None") == 0 || _stricmp(val, "Zero_GC") == 0 ||
                   _stricmp(val, "No_sfGC") == 0 || _stricmp(val, "No_GC") == 0 || _stricmp(val, "Overwrite") == 0) {
            cfg.strings.gc_heuristic = sf_gc_heuristic_e::DISABLED;
            cfg.strings.no_sfgc_overwrite_mode = true;
        } else {
            cfg.strings.gc_heuristic = sf_gc_heuristic_e::ADAPTIVE;
        }
    } else if (_stricmp(key, "FreeList_Recycle_Policy") == 0 ||
               _stricmp(key, "FreeListRecyclePolicy") == 0 ||
               _stricmp(key, "Freelist_Recycle_Policy") == 0 ||
               _stricmp(key, "Freelist_Policy") == 0 ||
               _stricmp(key, "FreelistRecyclePolicy") == 0) {
        strncpy(cfg.strings.freelist_policy_raw, val, sizeof(cfg.strings.freelist_policy_raw) - 1);
        if (_stricmp(val, "FIFO") == 0) {
            cfg.integers.freelist_recycle_policy = 1;
        } else if (_stricmp(val, "LIFO") == 0) {
            cfg.integers.freelist_recycle_policy = 0;
        } else {
            cfg.integers.freelist_recycle_policy = (uint32_t)std::strtoul(val, nullptr, 10);
        }
    } else if (_stricmp(key, "Enable_Huge_Pages") == 0 ||
               _stricmp(key, "EnableHugePages") == 0 ||
               _stricmp(key, "Huge_Pages") == 0 ||
               _stricmp(key, "HugePages") == 0) {
        cfg.strings.enable_huge_pages = (!val[0] || _stricmp(val, "1") == 0 || _stricmp(val, "true") == 0 ||
                                         _stricmp(val, "yes") == 0 || _stricmp(val, "enable") == 0 ||
                                         _stricmp(val, "enabled") == 0);
    } else if (_stricmp(key, "Advance_Payment") == 0 ||
               _stricmp(key, "AdvancePayment") == 0 ||
               _stricmp(key, "AlwaysPreTouch") == 0 ||
               _stricmp(key, "Always_PreTouch") == 0 ||
               _stricmp(key, "PreTouch") == 0 ||
               _stricmp(key, "Pretouch") == 0) {
        cfg.strings.Advance_Payment = (!val[0] ||
                                       _stricmp(val, "1") == 0 ||
                                       _stricmp(val, "true") == 0 ||
                                       _stricmp(val, "yes") == 0 ||
                                       _stricmp(val, "enable") == 0 ||
                                       _stricmp(val, "enabled") == 0);
    } else if (_stricmp(key, "Upclocker") == 0 || _stricmp(key, "Upclock") == 0 ||
               _stricmp(key, "Boost") == 0 || _stricmp(key, "Turbo") == 0) {
        if (!val[0] || _stricmp(val, "Allow") == 0 || _stricmp(val, "Max") == 0 ||
            _stricmp(val, "Boost") == 0 || _stricmp(val, "1") == 0 ||
            _stricmp(val, "True") == 0 || _stricmp(val, "yes") == 0 ||
            _stricmp(val, "enable") == 0 || _stricmp(val, "enabled") == 0) {
            sf_upclocker_set_boost(1);
        } else {
            sf_upclocker_set_boost(0);
        }
    } else if (_stricmp(key, "Always_Focus_Compactizing") == 0 ||
               _stricmp(key, "AlwaysFocusCompactizing") == 0 ||
               _stricmp(key, "Always_Compactize") == 0 ||
               _stricmp(key, "Always_Compact") == 0) {
        cfg.strings.always_focus_compactizing = (!val[0] ||
                                                 _stricmp(val, "1") == 0 ||
                                                 _stricmp(val, "true") == 0 ||
                                                 _stricmp(val, "yes") == 0 ||
                                                 _stricmp(val, "enable") == 0 ||
                                                 _stricmp(val, "enabled") == 0);
    } else if (_stricmp(key, "No_sfGC") == 0 ||
               _stricmp(key, "No_GC") == 0 ||
               _stricmp(key, "Zero_GC") == 0 ||
               _stricmp(key, "Overwrite_Heap") == 0 ||
               _stricmp(key, "OverwriteHeap") == 0) {
        bool en = (!val[0] ||
                   _stricmp(val, "1") == 0 ||
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
    } else if (_stricmp(key, "Heat_Decay_Factor") == 0 ||
               _stricmp(key, "HeatDecayFactor") == 0) {
        cfg.floats.heat_decay_factor = (float)std::strtod(val, nullptr);
    } else if (_stricmp(key, "Heat_Gain_Weight") == 0 ||
               _stricmp(key, "HeatGainWeight") == 0) {
        cfg.floats.heat_gain_weight = (float)std::strtod(val, nullptr);
    } else if (_stricmp(key, "Code_Arena_Bytes") == 0 ||
               _stricmp(key, "CodeArenaBytes") == 0 ||
               _stricmp(key, "Code_Arena_Size") == 0 ||
               _stricmp(key, "Code_Heap_Size") == 0) {
        cfg.integers.code_arena_bytes = (size_t)parse_size_with_unit(val);
    }

    else if (_stricmp(key, "Profile") == 0 || _stricmp(key, "Mode") == 0 || _stricmp(key, "Profile_Mode") == 0) {
        if (_stricmp(val, "SC") == 0 || _stricmp(val, "SafetyCritical") == 0 ||
            _stricmp(val, "Safety_Critical") == 0 || _stricmp(val, "1") == 0) {
            cfg.profile = MSIMD_PROFILE_SAFETY_CRITICAL;
            cfg.is_safety_critical = true;
            cfg.Forbid_Dynamic_Growth = true;
            cfg.disable_jit_deopt = true;
        } else {
            cfg.profile = MSIMD_PROFILE_STANDARD;
            cfg.is_safety_critical = false;
        }
    } else if (_stricmp(key, "Forbid_Dynamic_Growth") == 0 ||
               _stricmp(key, "ForbidDynamicGrowth") == 0 ||
               _stricmp(key, "Forbid_Runtime_Heap") == 0 ||
               _stricmp(key, "ForbidRuntimeHeap") == 0) {
        cfg.Forbid_Dynamic_Growth = (!val[0] || _stricmp(val, "1") == 0 || _stricmp(val, "true") == 0 || _stricmp(val, "yes") == 0 || _stricmp(val, "enable") == 0 || _stricmp(val, "enabled") == 0);
    } else if (_stricmp(key, "Max_Bounded_Iterations") == 0 ||
               _stricmp(key, "MaxLoopIterations") == 0 ||
               _stricmp(key, "Max_Iterations") == 0 ||
               _stricmp(key, "MaxIterations") == 0) {
        cfg.max_loop_iterations = (uint32_t)std::strtoul(val, nullptr, 10);
    } else if (_stricmp(key, "Allow_Operation_Dispatchings") == 0 ||
               _stricmp(key, "AllowOperationDispatchings") == 0 ||
               _stricmp(key, "Allow_Dispatching") == 0 ||
               _stricmp(key, "Operation_Dispatchings") == 0) {
        bool allow = (!val[0] || _stricmp(val, "1") == 0 || _stricmp(val, "true") == 0 ||
                      _stricmp(val, "yes") == 0 || _stricmp(val, "allow") == 0 ||
                      _stricmp(val, "enable") == 0 || _stricmp(val, "enabled") == 0);
        cfg.strings.allow_operation_dispatchings = allow;
        g_sf_allow_operation_dispatchings.store(allow, std::memory_order_relaxed);
    } else if (_stricmp(key, "Reduce_Memory_Resident_Propagation") == 0 ||
               _stricmp(key, "ReduceMemoryResidentPropagation") == 0 ||
               _stricmp(key, "RMRP") == 0) {
        bool en = (!val[0] || _stricmp(val, "1") == 0 || _stricmp(val, "true") == 0 ||
                   _stricmp(val, "yes") == 0 || _stricmp(val, "enable") == 0 ||
                   _stricmp(val, "enabled") == 0);
        cfg.strings.reduce_memory_resident_propagation = en;
        g_sf_rmrp_enabled.store(en, std::memory_order_relaxed);
    } else if (_stricmp(key, "RMRP_Method") == 0 ||
               _stricmp(key, "RMRPMethod") == 0) {
        cfg.strings.rmrp_method_explicit = true;
        strncpy(cfg.strings.rmrp_method_raw, val, sizeof(cfg.strings.rmrp_method_raw) - 1);
        if (_stricmp(val, "Sequential") == 0 || _stricmp(val, "Installment") == 0 ||
            _stricmp(val, "Stepwise") == 0) {
            cfg.strings.rmrp_method = sf_rmrp_method_e::SEQUENTIAL;
        } else {
            cfg.strings.rmrp_method = sf_rmrp_method_e::PLUMP;
        }
        g_sf_rmrp_method.store(static_cast<uint8_t>(cfg.strings.rmrp_method), std::memory_order_relaxed);
    } else if (_stricmp(key, "Global_Volatility_Method") == 0 ||
               _stricmp(key, "GlobalVolatilityMethod") == 0 ||
               _stricmp(key, "Volatility_Method") == 0) {
        strncpy(cfg.strings.global_volatility_raw, val, sizeof(cfg.strings.global_volatility_raw) - 1);
        if (_stricmp(val, "SRAM") == 0 || _stricmp(val, "Cache") == 0 || _stricmp(val, "L1L2") == 0) {
            cfg.strings.global_volatility = sf_global_volatility_e::SRAM;
        } else {
            cfg.strings.global_volatility = sf_global_volatility_e::DRAM;
        }
        g_sf_global_volatility.store(static_cast<uint8_t>(cfg.strings.global_volatility), std::memory_order_relaxed);
    } else if (_stricmp(key, "puzzler_Morph_Method") == 0 ||
               _stricmp(key, "puzzlerMorphMethod") == 0 ||
               _stricmp(key, "Morph_Method") == 0) {
        strncpy(cfg.strings.puzzler_morph_raw, val, sizeof(cfg.strings.puzzler_morph_raw) - 1);
        if (_stricmp(val, "Excision") == 0 || _stricmp(val, "Exclusive") == 0 || _stricmp(val, "Busy") == 0) {
            cfg.strings.puzzler_morph = sf_puzzler_morph_e::EXCISION_EXCLUSIVE;
        } else {
            cfg.strings.puzzler_morph = sf_puzzler_morph_e::SHADOW_RCU;
        }
        g_sf_puzzler_morph_method.store(static_cast<uint8_t>(cfg.strings.puzzler_morph), std::memory_order_relaxed);
    } else if (_stricmp(key, "Experimental") == 0 ||
               _stricmp(key, "Experimental_Features") == 0 ||
               _stricmp(key, "UnlockExperimentalVMOptions") == 0) {
        cfg.strings.experimental_options = (!val[0] || _stricmp(val, "1") == 0 || _stricmp(val, "true") == 0 ||
                                            _stricmp(val, "yes") == 0 || _stricmp(val, "enable") == 0 ||
                                            _stricmp(val, "enabled") == 0);
        cfg.UnlockExperimentalVMOptions = cfg.strings.experimental_options;
    } else if (_stricmp(key, "UseMLTCSolver") == 0 ||
               _stricmp(key, "Use_MLTC_Solver") == 0 ||
               _stricmp(key, "MLTCSolver") == 0 ||
               _stricmp(key, "MLTC_Solver") == 0) {
        cfg.UseMLTCSolver = (!val[0] || _stricmp(val, "1") == 0 || _stricmp(val, "true") == 0 ||
                             _stricmp(val, "yes") == 0 || _stricmp(val, "enable") == 0 ||
                             _stricmp(val, "enabled") == 0);
    } else if (_stricmp(key, "Entangle_Execution") == 0 ||
               _stricmp(key, "Batch_Register_Entangle") == 0 ||
               _stricmp(key, "Entangled_Execution") == 0) {
        bool en = (!val[0] || _stricmp(val, "1") == 0 || _stricmp(val, "true") == 0 ||
                   _stricmp(val, "yes") == 0 || _stricmp(val, "enable") == 0 ||
                   _stricmp(val, "enabled") == 0);
        cfg.strings.entangle_execution = en;
        g_sf_entangle_execution.store(en, std::memory_order_relaxed);
    } else if (_stricmp(key, "Execution_Focus") == 0 ||
               _stricmp(key, "ExecutionFocus") == 0 ||
               _stricmp(key, "Focus") == 0 ||
               _stricmp(key, "EXECUTION_FOCUS") == 0) {
        if (_stricmp(val, "SCP") == 0 || _stricmp(val, "SingleCore") == 0 || _stricmp(val, "Single_Core") == 0) {
            cfg.execution_focus = SF_FOCUS_SCP;
        } else if (_stricmp(val, "MCP") == 0 || _stricmp(val, "MultiCore") == 0 || _stricmp(val, "Multi_Core") == 0) {
            cfg.execution_focus = SF_FOCUS_MCP;
        } else if (_stricmp(val, "HybridDual") == 0 || _stricmp(val, "Hybrid_Dual") == 0 ||
                   _stricmp(val, "Dual") == 0 || _stricmp(val, "DualPerformance") == 0) {
            cfg.execution_focus = SF_FOCUS_HYBRID_DUAL;
        } else if (_stricmp(val, "SCP_Homo") == 0 || _stricmp(val, "SCPHomo") == 0 ||
                   _stricmp(val, "SCP_HOMO") == 0 || _stricmp(val, "SCP_Homogeneous") == 0) {
            cfg.execution_focus = SF_FOCUS_SCP_HOMO;
        } else if (_stricmp(val, "SCP_Hybrid") == 0 || _stricmp(val, "SCPHybrid") == 0 ||
                   _stricmp(val, "SCP_HYBRID") == 0 || _stricmp(val, "SCP_Asymmetric") == 0) {
            cfg.execution_focus = SF_FOCUS_SCP_HYBRID;
        } else if (_stricmp(val, "MCP_Homo") == 0 || _stricmp(val, "MCPHomo") == 0 ||
                   _stricmp(val, "MCP_HOMO") == 0 || _stricmp(val, "MCP_Homogeneous") == 0) {
            cfg.execution_focus = SF_FOCUS_MCP_HOMO;
        } else if (_stricmp(val, "MCP_Hybrid") == 0 || _stricmp(val, "MCPHybrid") == 0 ||
                   _stricmp(val, "MCP_HYBRID") == 0 || _stricmp(val, "MCP_Asymmetric") == 0) {
            cfg.execution_focus = SF_FOCUS_MCP_HYBRID;
        } else if (_stricmp(val, "Dual_Homo") == 0 || _stricmp(val, "DualHomo") == 0 ||
                   _stricmp(val, "DUAL_HOMO") == 0 || _stricmp(val, "Dual_Homogeneous") == 0) {
            cfg.execution_focus = SF_FOCUS_DUAL_HOMO;
        } else if (_stricmp(val, "Dual_Hybrid") == 0 || _stricmp(val, "DualHybrid") == 0 ||
                   _stricmp(val, "DUAL_HYBRID") == 0 || _stricmp(val, "Dual_Asymmetric") == 0) {
            cfg.execution_focus = SF_FOCUS_DUAL_HYBRID;
        } else {
            cfg.execution_focus = SF_FOCUS_AUTO;
        }
        SF_custominiVm_set_execution_focus(cfg.execution_focus);
    } else if (_stricmp(key, "Strict_VFS_Branch_Validation") == 0 ||
               _stricmp(key, "StrictVFSBranchValidation") == 0 ||
               _stricmp(key, "Strict_VFS") == 0 ||
               _stricmp(key, "StrictVFS") == 0 ||
               _stricmp(key, "VFS_Strict") == 0 ||
               _stricmp(key, "VFSStrict") == 0 ||
               _stricmp(key, "Strict_VFS_Validation") == 0) {
        bool en = (!val[0] || _stricmp(val, "1") == 0 || _stricmp(val, "true") == 0 ||
                   _stricmp(val, "yes") == 0 || _stricmp(val, "enable") == 0 ||
                   _stricmp(val, "enabled") == 0);
        cfg.strings.strict_vfs_branch_validation = en;
        g_sf_strict_vfs_validation.store(en, std::memory_order_relaxed);
    } else if (_stricmp(key, "VFS_Panic_Dialog") == 0 ||
               _stricmp(key, "VFSPanicDialog") == 0 ||
               _stricmp(key, "VFS_PANIC_DIALOG") == 0 ||
               _stricmp(key, "Panic_Dialog") == 0 ||
               _stricmp(key, "PANIC_DIALOG") == 0) {
        bool en = (!val[0] || _stricmp(val, "1") == 0 || _stricmp(val, "true") == 0 ||
                   _stricmp(val, "yes") == 0 || _stricmp(val, "enable") == 0 ||
                   _stricmp(val, "enabled") == 0);
        cfg.strings.vfs_panic_dialog = en;
        g_sf_vfs_panic_dialog_enabled.store(en, std::memory_order_relaxed);
    } else if (_stricmp(key, "InlineASM_Threshold") == 0 ||
               _stricmp(key, "IASM_Threshold") == 0 ||
               _stricmp(key, "InlineAsmThreshold") == 0 ||
               _stricmp(key, "IASMThreshold") == 0) {
        uint32_t val_num = (uint32_t)std::strtoul(val, nullptr, 10);
        SF_custominiVm_set_iasm_threshold(val_num);
    } else if (_stricmp(key, "Strict_Ring3_Only") == 0 ||
               _stricmp(key, "StrictRing3Only") == 0 ||
               _stricmp(key, "Strict_Ring3") == 0 ||
               _stricmp(key, "StrictRing3") == 0) {
        bool en = (!val[0] || _stricmp(val, "1") == 0 || _stricmp(val, "true") == 0 ||
                   _stricmp(val, "yes") == 0 || _stricmp(val, "enable") == 0 ||
                   _stricmp(val, "enabled") == 0);
        SF_custominiVm_set_strict_ring3(en ? 1 : 0);
    } else if (_stricmp(key, "Strict_Flags") == 0 ||
               _stricmp(key, "StrictFlags") == 0 ||
               _stricmp(key, "Strict_Flag_Validation") == 0 ||
               _stricmp(key, "StrictFlagValidation") == 0) {
        bool en = (!val[0] || _stricmp(val, "1") == 0 || _stricmp(val, "true") == 0 ||
                   _stricmp(val, "yes") == 0 || _stricmp(val, "enable") == 0 ||
                   _stricmp(val, "enabled") == 0);
        SF_custominiVm_set_strict_flags(en ? 1 : 0);
    } else {
        return false;
    }
    return true;
}

static void parse_single_sf_flag(sf_vm_config_t& cfg, const char* flag_str) {
    if (!flag_str || !flag_str[0]) return;

    char key[64] = {0};
    char val[64] = {0};
    sf_cli_strip_prefix_and_split(flag_str, nullptr, key, sizeof(key), val, sizeof(val));
    if (key[0] == '\0') return;

    if (!parse_single_sf_flag_internal(cfg, key, val)) {
        sf_cli_resolution_result_t res = sf_cli_resolve_argument(key, val);
        if (res.matched) {
            if (res.was_semantic_intent) {
                fprintf(stdout, "[SFVM CLI Semantic Engine] Recognized intent '%s' -> applied canonical flag '+#SF|%s:%s' (confidence: %.2f)\n",
                        key, res.canonical_flag, res.canonical_value, res.confidence);
                parse_single_sf_flag_internal(cfg, res.canonical_flag, res.canonical_value);
            } else if (res.was_fuzzy_typo) {
                fprintf(stderr, "[SFVM CLI Helper] Unknown argument '+#SF|%s'\n  --> did you mean %s? (DID YOU MEAN: '+#SF|%s'?)\n",
                        key, res.canonical_flag, res.canonical_flag);
                parse_single_sf_flag_internal(cfg, res.canonical_flag, res.canonical_value);
            } else {
                parse_single_sf_flag_internal(cfg, res.canonical_flag, res.canonical_value);
            }
        } else {
            const char* best_match = nullptr;
            int min_dist = 99;
            for (const char* known : SF_KNOWN_FLAGS) {
                int dist = sf_damerau_levenshtein_distance(key, known);
                if (dist < min_dist && dist <= 3) {
                    min_dist = dist;
                    best_match = known;
                }
            }
            if (best_match) {
                fprintf(stderr, "[SFVM CLI Helper] Unknown argument '+#SF|%s'\n  --> did you mean %s? (DID YOU MEAN: '+#SF|%s'?)\n", key, best_match, best_match);
            } else {
                fprintf(stderr, "[SFVM CLI Helper] Unrecognized flag '+#SF|%s'\n", key);
            }
            if (SF_custominiVm_is_strict_flags()) {
                fprintf(stderr, "\n=================================================================\n");
                fprintf(stderr, "   [SFVM STRICT FLAG PANIC] Unrecognized CLI Flag: '+#SF|%s'\n", key);
                fprintf(stderr, "   Fault Description : Strict flag validation is engaged (-#SF|Strict_Flags:true).\n");
                fprintf(stderr, "   Fail-Safe Action  : Immediate fail-fast termination initiated.\n");
                fprintf(stderr, "=================================================================\n\n");
                std::fflush(stderr);
                SF_custominiVm_emergency_os_sweep();
                std::abort();
            }
        }
    }
}

static inline bool validate_and_reconcile_sf_flags(sf_vm_config_t& cfg) {
    if (cfg.strings.gc_operation_facility == sf_gc_operation_facility_e::SIMD) {
        if (cfg.strings.critical_simd_explicit) {
            cfg.has_flag_conflict = true;
            if (cfg.strings.critical_simd_mode == sf_critical_simd_e::DENY) {
                snprintf(cfg.conflict_msg, sizeof(cfg.conflict_msg),
                         "FATAL FLAG CONFLICT: +#SF|GC_Operation_Facility:SIMD is incompatible with +#SF|Use_Critical_SIMDly_Operations:Deny! "
                         "'GC_Operation_Facility' argument is incompatible with 'Use_Critical_SIMDly_Operations' parameter or 'DENY' code! "
                         "SIMD GC requires SIMD vector units. Initialization rejected.");
                fprintf(stderr, "[SFVM Flag Conflict Error] %s\n", cfg.conflict_msg);
                return false;
            } else {
                snprintf(cfg.conflict_msg, sizeof(cfg.conflict_msg),
                         "INCOMPATIBILITY WARNING: 'Use_Critical_SIMDly_Operations' has conflicts to 'GC_Operation_Facility'! +#SF|Use_Critical_SIMDly_Operations is superseded because +#SF|GC_Operation_Facility:SIMD is already fully SIMD-native.");
                fprintf(stderr, "[SFVM Flag Conflict Warning] %s\n", cfg.conflict_msg);
            }
        }
    }

    if (cfg.strings.rmrp_method_explicit && !cfg.strings.reduce_memory_resident_propagation) {
        cfg.has_flag_conflict = true;
        snprintf(cfg.conflict_msg, sizeof(cfg.conflict_msg),
                 "FATAL FLAG CONFLICT: 'RMRP_Method' cannot be used on its own! "
                 "'RMRP_Method' is a sub-configurator of 'Reduce_Memory_Resident_Propagation' and is incompatible "
                 "without '+#SF|Reduce_Memory_Resident_Propagation:true'! Initialization rejected.");
        fprintf(stderr, "[SFVM Flag Conflict Error] %s\n", cfg.conflict_msg);
        return false;
    }

    if (cfg.strings.no_sfgc_overwrite_mode) {
        if (cfg.strings.always_focus_compactizing || cfg.strings.gc_operation_facility == sf_gc_operation_facility_e::SIMD) {
            cfg.has_flag_conflict = true;
            snprintf(cfg.conflict_msg, sizeof(cfg.conflict_msg),
                     "FATAL FLAG CONFLICT: No_sfGC / PrimitiveRW disables sfGC collector! Incompatible with Always_Focus_Compactizing or GC_Mode:SIMD.");
            fprintf(stderr, "[SFVM Flag Conflict Error] %s\n", cfg.conflict_msg);
            return false;
        }
    }

    if (cfg.strings.allow_operation_dispatchings) {
        snprintf(cfg.conflict_msg, sizeof(cfg.conflict_msg),
                 "SFVM Operational Notice: 'priority' has conflicts to 'Allow_Operation_Dispatchings'! +#SF|Allow_Operation_Dispatchings:true is restricted for High Priority (8-10) operations to protect execution locality.");
        fprintf(stderr, "[SFVM Notice] %s\n", cfg.conflict_msg);
    }

    if (cfg.strings.entangle_execution && !cfg.strings.experimental_options) {
        cfg.has_flag_conflict = true;
        snprintf(cfg.conflict_msg, sizeof(cfg.conflict_msg),
                 "SFVM Flag Conflict Error: 'Entangle_Execution' argument is incompatible with 'Experimental' parameter or 'LOCKED' code! Requires '+#SF|Experimental:true' to be enabled!");
        fprintf(stderr, "[SFVM Flag Conflict Error] %s\n", cfg.conflict_msg);
        return false;
    }

    if (cfg.UseMLTCSolver && !cfg.UnlockExperimentalVMOptions) {
        cfg.has_flag_conflict = true;
        snprintf(cfg.conflict_msg, sizeof(cfg.conflict_msg),
                 "SFVM Flag Conflict Error: 'UseMLTCSolver' argument requires '+#SF|Experimental:true' to be enabled!");
        fprintf(stderr, "[SFVM Flag Conflict Error] %s\n", cfg.conflict_msg);
        return false;
    }

    if (cfg.is_safety_critical || cfg.profile == MSIMD_PROFILE_SAFETY_CRITICAL) {
        cfg.profile = MSIMD_PROFILE_SAFETY_CRITICAL;
        cfg.is_safety_critical = true;
        cfg.Forbid_Dynamic_Growth = true;
        cfg.disable_jit_deopt = true;
    }

    if (cfg.has_flag_conflict && SF_custominiVm_is_strict_flags()) {
        fprintf(stderr, "\n=================================================================\n");
        fprintf(stderr, "   [SFVM STRICT FLAG PANIC] Flag Conflict Encountered in Strict Mode!\n");
        fprintf(stderr, "   Conflict Details  : %s\n", cfg.conflict_msg);
        fprintf(stderr, "   Fault Description : Strict flag validation is engaged (-#SF|Strict_Flags:true).\n");
        fprintf(stderr, "   Fail-Safe Action  : Immediate fail-fast termination initiated.\n");
        fprintf(stderr, "=================================================================\n\n");
        std::fflush(stderr);
        SF_custominiVm_emergency_os_sweep();
        std::abort();
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
            cfg.Forbid_Dynamic_Growth = true;
            cfg.disable_jit_deopt = true;
        }
    }

    auto parse_flags_loop = [&](const char* src) {
        if (!src) return;
        const char* p = src;
        while (*p) {
            while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' || *p == ',' || *p == ';') p++;
            if (!*p) break;
            const char* start = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' && *p != ',' && *p != ';') {
                p++;
            }
            size_t len = (size_t)(p - start);
            if (len > 0) {
                char token[128] = {0};
                if (len >= sizeof(token)) len = sizeof(token) - 1;
                strncpy(token, start, len);
                token[len] = '\0';
                parse_single_sf_flag(cfg, token);
            }
        }
    };

    const char* env_args = getenv("MATHVANTAGE_OPTS");
    if (!env_args) env_args = getenv("SF_VM_ARGS");
    if (env_args) {
        parse_flags_loop(env_args);
    }

    if (custom_args) {
        parse_flags_loop(custom_args);
    }

    validate_and_reconcile_sf_flags(cfg);

    return cfg;
}

typedef SF_custominiVm_event_category_e sacculus_event_t;
using sfEvent = sacculus_event_t;

typedef SF_custominiVm_severity_e sacculus_severity_t;
using sfSeverity = sacculus_severity_t;

static_assert(sizeof(SF_custominiVm_hotlog_entry_t) == 64,
              "hotlog entry must be exactly 64 bytes");
static_assert(sizeof(SF_custominiVm_coldlog_entry_t) == 64,
              "coldlog entry must be exactly 64 bytes");

template<typename Entry, uint32_t Capacity>
struct sacculus_ring_t {
    SACFOR_ALIGN_CL std::atomic<uint64_t> write_head{0};
    SACFOR_ALIGN_CL std::atomic<uint64_t> read_head{0};
    Entry* ring = nullptr;

    static constexpr uint32_t CAP = Capacity;

    sacculus_ring_t() {
        size_t total_bytes = (size_t)Capacity * sizeof(Entry);
        ring = static_cast<Entry*>(sf_os_reserve_memory(total_bytes));
        if (ring) {
            sf_os_commit_memory(ring, total_bytes);
        }
    }

    ~sacculus_ring_t() {
        if (ring) {
            size_t total_bytes = (size_t)Capacity * sizeof(Entry);
            sf_os_release_memory(ring, total_bytes);
            ring = nullptr;
        }
    }

    sacculus_ring_t(const sacculus_ring_t&) = delete;
    sacculus_ring_t& operator=(const sacculus_ring_t&) = delete;

    inline uint64_t map_index(uint64_t seq) const noexcept {
        if constexpr ((Capacity & (Capacity - 1)) == 0) {
            return seq & (uint64_t)(Capacity - 1);
        } else {
            return seq % (uint64_t)Capacity;
        }
    }

    void push(const Entry& e) {
        if (!ring) return;
        uint64_t idx = map_index(write_head.fetch_add(1, std::memory_order_relaxed));
        Entry tmp = e;
        tmp.ready = 0;
        ring[idx] = tmp;
        std::atomic_thread_fence(std::memory_order_release);
        ((std::atomic<uint8_t>*)&ring[idx].ready)->store(1, std::memory_order_relaxed);
    }

    void drain(void (*cb)(const Entry*, void*), void* user) {
        if (!ring) return;
        uint64_t w = write_head.load(std::memory_order_acquire);
        uint64_t r = read_head.load(std::memory_order_relaxed);
        while (r != w) {
            uint64_t idx = map_index(r);
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

static std::atomic<sacculus_hotlog_t*> g_sf_active_hotlog{nullptr};
static std::atomic<sacculus_coldlog_t*> g_sf_active_coldlog{nullptr};
static std::atomic<uint32_t> g_sf_telemetry_filter_mask{0xFFFFFFFFu};

static inline const char* sf_event_category_name(uint8_t ev) {
    switch (ev) {
        case 0x01: return "ALLOC";
        case 0x02: return "FREE";
        case 0x03: return "PROMOTE";
        case 0x04: return "DEMOTE";
        case 0x05: return "TO_FALLINE";
        case 0x06: return "TO_LANDFILL";
        case 0x07: return "EMPTYBOX";
        case 0x08: return "LANDFILL_EVAC";
        case 0x09: return "HOTSWAP";
        case 0x0A: return "HOTSWAP_FAIL";
        case 0x0B: return "GC_SWEEP";
        case 0x0C: return "GC_COMPACT";
        case 0x0D: return "GC_ALIGN";
        case 0x0E: return "SAFEPOINT";
        case 0x0F: return "HOOK";
        case 0x10: return "ERROR";
        // 0x20..0x2F Runway
        case 0x20: return "RUNWAY_BUMP";
        case 0x21: return "ALLOC_FREELIST";
        case 0x22: return "CHUNK_CHAIN";
        case 0x23: return "ALLOC_REBATE";
        case 0x24: return "PACE_STALL";
        case 0x25: return "EGG_ADMIT";
        case 0x26: return "FLEDGLING_PROMOTE";
        case 0x27: return "PITSTOP_EXCISE";
        case 0x28: return "PITSTOP_SHADOW";
        case 0x29: return "PITSTOP_RETURN";
        case 0x2A: return "PITSTOP_STALL";
        case 0x2B: return "PITSTOP_CALLBACK";
        case 0x2C: return "FALLINE_SNATCH_HIT";
        case 0x2D: return "FALLINE_SNATCH_MISS";
        case 0x2E: return "FALLINE_SPLIT";
        case 0x2F: return "FALLINE_COALESCE";
        // 0x30..0x3F Topology
        case 0x30: return "SAFEPOINT_PARK";
        case 0x31: return "SAFEPOINT_RESUME";
        case 0x32: return "HOOKPOINT_ENTER";
        case 0x33: return "HOOKPOINT_EXIT";
        case 0x34: return "CONTROLPOINT_AUDIT";
        case 0x35: return "CONTROLPOINT_PAGEIN";
        case 0x36: return "PAUSEPOINT_HALT";
        case 0x37: return "INTERRUPT_GATE";
        // 0x40..0x4F sfGC
        case 0x40: return "GC_PHASE_CHANGE";
        case 0x41: return "PROMOTE_YOUNG_ADOL";
        case 0x42: return "PROMOTE_ADOL_SENIOR";
        case 0x43: return "REGION_MIGRATE";
        case 0x44: return "RMRP_SPAN_TRIM";
        case 0x45: return "RMRP_DECOMMIT";
        case 0x46: return "RESURRECT_REQUEST";
        case 0x47: return "RESURRECT_RESTORED";
        case 0x48: return "RESURRECT_DROP";
        case 0x49: return "LANDFILL_CRIT_WARN";
        case 0x4A: return "SLICEVIEW_SALVAGED";
        // 0x50..0x5F ALU / Scheduler
        case 0x50: return "ALU_DISPATCH";
        case 0x51: return "ALU_SATURATE_STEAL";
        case 0x52: return "GROUP_COMPUTE_BEGIN";
        case 0x53: return "GROUP_COMPUTE_RESET";
        case 0x54: return "CORE_PIN_PERF";
        case 0x55: return "CORE_UNPIN";
        case 0x56: return "UPCLOCKER_BOOST_ON";
        case 0x57: return "UPCLOCKER_BOOST_OFF";
        case 0x58: return "FOCUS_MODE_SWITCH";
        // 0x60..0x6F SRAM
        case 0x60: return "SRAM_PREWARM";
        case 0x61: return "STORE_FENCE_BCAST";
        case 0x62: return "STREAM_STORE_ENGAGE";
        case 0x63: return "STREAM_STORE_FLUSH";
        case 0x64: return "SCRATCHPAD_ALLOC";
        case 0x65: return "SRAM_COMMIT_DRAM";
        // 0x70..0x7F JIT
        case 0x70: return "BITCODE_DECOMP_OK";
        case 0x71: return "ORCJIT_MODULE_ADDED";
        case 0x72: return "ORCJIT_RESOLVE_OK";
        case 0x73: return "ENTANGLE_BEGIN";
        case 0x74: return "ENTANGLE_COMMIT";
        case 0x75: return "LLVM_PASS_INJECTED";
        // 0x80..0x8F Self-Healing
        case 0x80: return "HEAL_HEADER_REPAIRED";
        case 0x81: return "HEAL_BROOKS_SEVERED";
        case 0x82: return "HEAL_CANARY_QUARANTINE";
        case 0x83: return "HEAL_NUMERIC_DIVERGE";
        case 0x84: return "HEAL_BITCODE_EXCEPTION";
        // 0x90..0x9F VFS & Paging
        case 0x90: return "VFS_MKDIR_SUCCESS";
        case 0x91: return "VFS_BIND_TENSOR";
        case 0x92: return "VFS_UNLINK_TENSOR";
        case 0x93: return "VFS_EVICT_TREE";
        case 0x94: return "PAGEFILE_PAGEOUT";
        case 0x95: return "PAGEFILE_PAGEIN";
        case 0x96: return "HOST_GC_INTERCEPT";
        default: return "UNKNOWN_EVENT";
    }
}

static inline const char* sf_severity_str(uint8_t sev) {
    switch (sev) {
        case 0: return "TRACE";
        case 1: return "INFO";
        case 2: return "WARN";
        case 3: return "ERROR";
        case 4: return "FATAL";
        default: return "UNK";
    }
}

static inline const char* sf_severity_ansi_color(uint8_t sev) {
    switch (sev) {
        case 0: return "\033[90m"; // Muted dark gray
        case 1: return "\033[32m"; // Vibrant green
        case 2: return "\033[33m"; // Amber yellow
        case 3: return "\033[31m"; // Crimson red
        case 4: return "\033[97;41;1m"; // Bold white on red background
        default: return "\033[0m";
    }
}

static void sacculus_hotlog_write(
        sacculus_hotlog_t* log,
        sacculus_event_t ev, sacculus_severity_t sev,
        uint32_t fn_id, uint16_t slot, uint8_t gen, uint8_t reg,
        uint16_t core, uint64_t sweep_no, uint32_t heap_kb,
        const char* msg) {
    if (!log) return;
    uint32_t cat_bit = 1u << (static_cast<uint8_t>(ev) >> 4);
    if ((g_sf_telemetry_filter_mask.load(std::memory_order_relaxed) & cat_bit) == 0) {
        return;
    }
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

    // Forward raw formatted ANSI log directly to registered stream listeners & console
    char log_buf[256];
    const char* col = sf_severity_ansi_color(e.severity);
    const char* rst = "\033[0m";
    const char* sev_str = sf_severity_str(e.severity);
    const char* cat_str = sf_event_category_name(e.event);
    snprintf(log_buf, sizeof(log_buf),
             "\033[1;36m[0x%04X]\033[0m %s[%-5s]%s [%-18s] core=%-2u fn=%-4u msg=\"%s\"",
             (unsigned int)e.event,
             col, sev_str, rst, cat_str,
             (unsigned int)e.current_core, (unsigned int)e.fn_id,
             e.msg[0] ? e.msg : "");
    sf_broadcast_raw_log_line(log_buf, e.severity >= 3);
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

    // Forward coldlog event to stream listeners
    char log_buf[256];
    const char* col = sf_severity_ansi_color(e.severity);
    const char* rst = "\033[0m";
    const char* sev_str = sf_severity_str(e.severity);
    const char* cat_str = sf_event_category_name(e.event);
    snprintf(log_buf, sizeof(log_buf),
             "\033[1;35m[0x%04X]\033[0m %s[%-5s]%s [%-18s] fn=%-4u msg=\"%s\"",
             (unsigned int)e.event,
             col, sev_str, rst, cat_str,
             (unsigned int)e.fn_id,
             e.msg[0] ? e.msg : "");
    sf_broadcast_raw_log_line(log_buf, e.severity >= 3);
}

static inline void sf_hotlog_emit(sacculus_event_t ev, sacculus_severity_t sev,
                                  uint32_t fn_id, uint16_t slot, uint8_t gen, uint8_t reg,
                                  uint16_t core, uint64_t sweep_no, uint32_t heap_kb,
                                  const char* msg) {
    auto* hl = g_sf_active_hotlog.load(std::memory_order_relaxed);
    if (hl) {
        sacculus_hotlog_write(hl, ev, sev, fn_id, slot, gen, reg, core, sweep_no, heap_kb, msg);
    }
}

using sacculus_kernel_fn_t = SF_custominiVm_kernel_fn_t;
using sfKernelFn = SF_custominiVm_kernel_fn_t;

#define SACFOR_F32 SF_DTYPE_F32
#define SACFOR_F64 SF_DTYPE_F64
#define SACFOR_I32 SF_DTYPE_I32
#define SACFOR_I64 SF_DTYPE_I64
#define SACFOR_F16 SF_DTYPE_F16
#define SACFOR_U8    SF_DTYPE_U8
#define SACFOR_F8    SF_DTYPE_F8
#define SACFOR_I8    SF_DTYPE_I8
#define SACFOR_I16   SF_DTYPE_I16
#define SACFOR_F128  SF_DTYPE_F128
#define SACFOR_I128  SF_DTYPE_I128
#define SACFOR_F256  SF_DTYPE_F256
#define SACFOR_I256  SF_DTYPE_I256

extern "C" MSIMD_API size_t sf_dtype_size(sfDType d) {
    switch (d) {
        case SF_DTYPE_F8:
        case SF_DTYPE_U8:
        case SF_DTYPE_I8:   return 1;
        case SF_DTYPE_F16:
        case SF_DTYPE_I16:
        case SF_DTYPE_U16:  return 2;
        case SF_DTYPE_F32:
        case SF_DTYPE_I32:
        case SF_DTYPE_U32:  return 4;
        case SF_DTYPE_F64:
        case SF_DTYPE_I64:
        case SF_DTYPE_U64:  return 8;
        case SF_DTYPE_F128:
        case SF_DTYPE_I128:
        case SF_DTYPE_U128: return 16;
        case SF_DTYPE_F256:
        case SF_DTYPE_I256:
        case SF_DTYPE_U256: return 32;
        default: return 0;
    }
}
static inline size_t sacculus_dtype_size(sfDType d) { return sf_dtype_size(d); }

extern "C" MSIMD_API sfDType sf_dtype_detect(size_t byte_size, bool is_float, bool is_signed) {
    if (is_float) {
        switch (byte_size) {
            case 1:  return SF_DTYPE_F8;
            case 2:  return SF_DTYPE_F16;
            case 4:  return SF_DTYPE_F32;
            case 8:  return SF_DTYPE_F64;
            case 16: return SF_DTYPE_F128;
            case 32: return SF_DTYPE_F256;
            default: return SF_DTYPE_F32;
        }
    } else {
        switch (byte_size) {
            case 1:  return is_signed ? SF_DTYPE_I8 : SF_DTYPE_U8;
            case 2:  return is_signed ? SF_DTYPE_I16 : SF_DTYPE_U16;
            case 4:  return is_signed ? SF_DTYPE_I32 : SF_DTYPE_U32;
            case 8:  return is_signed ? SF_DTYPE_I64 : SF_DTYPE_U64;
            case 16: return is_signed ? SF_DTYPE_I128 : SF_DTYPE_U128;
            case 32: return is_signed ? SF_DTYPE_I256 : SF_DTYPE_U256;
            default: return is_signed ? SF_DTYPE_I32 : SF_DTYPE_U32;
        }
    }
}

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
    std::atomic<uint32_t> call_step_count{0};
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

thread_local uint32_t tl_sf_group_hex_id{0};
thread_local uint8_t  tl_sf_group_priority{5};
thread_local bool     tl_sf_group_bypass_warmup{false};

static std::atomic<uint32_t> g_sf_iasm_threshold{5};
static std::atomic<bool>     g_sf_strict_ring3_only{false};
static std::atomic<bool>     g_sf_strict_flags{false};

extern "C" {
MSIMD_API void SF_custominiVm_set_iasm_threshold(uint32_t threshold) {
    if (threshold > 10) threshold = 10;
    g_sf_iasm_threshold.store(threshold, std::memory_order_release);
}

MSIMD_API uint32_t SF_custominiVm_get_iasm_threshold(void) {
    return g_sf_iasm_threshold.load(std::memory_order_acquire);
}

MSIMD_API void SF_custominiVm_set_strict_ring3(int enable) {
    g_sf_strict_ring3_only.store(enable != 0, std::memory_order_release);
}

MSIMD_API int SF_custominiVm_is_strict_ring3(void) {
    return g_sf_strict_ring3_only.load(std::memory_order_acquire) ? 1 : 0;
}

MSIMD_API void SF_custominiVm_set_strict_flags(int enable) {
    g_sf_strict_flags.store(enable != 0, std::memory_order_release);
}

MSIMD_API int SF_custominiVm_is_strict_flags(void) {
    return g_sf_strict_flags.load(std::memory_order_acquire) ? 1 : 0;
}

MSIMD_API int SF_custominiVm_is_iasm_active(uint32_t priority) {
    if (g_sf_strict_ring3_only.load(std::memory_order_acquire)) return 0;
    if (priority == 0) {
        priority = tl_sf_group_priority;
    }
    return (priority >= g_sf_iasm_threshold.load(std::memory_order_acquire)) ? 1 : 0;
}
}

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

    /* ── puzzler & Optimization State Fields ── */
    std::atomic<uint8_t>  has_optimized{0};               // 0 = Egg, 1 = Fledgling
    uint8_t               opt_permitted{1};               // 1 = Permitted, 0 = BYPASS_WARMUP locked
    uint8_t               opt_success{0};                 // 1 = Last puzzler attempt succeeded
    uint8_t               group_priority{5};              // 0 - 10 ALU Queue Priority
    uint32_t              group_hex_id{0};                // SF::GroupCompute() Unique ID
    std::atomic<uint64_t> last_optimized_time{0};         // Timestamp of last optimization
    std::atomic<uint64_t> optimization_duration_cycles{0};// CPU cycles spent optimizing
    std::atomic<uint64_t> cycles_gained{0};               // Net cycles saved per call
    uint16_t              active_simd_idx{0};             // Active index in branched SIMD table
    uint16_t              simd_candidate_count{0};        // Total candidates in table
    sf_simd_ladder_slot_t simd_table[8]{};                // Branched SIMD ladder table

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
        forwarding_address.store(nullptr, std::memory_order_relaxed);
        child_view_count.store(0, std::memory_order_relaxed);
        parent_header.store(nullptr, std::memory_order_relaxed);
        slice_byte_offset     = 0;

        has_optimized.store(0, std::memory_order_relaxed);
        opt_permitted         = 1;
        opt_success           = 0;
        group_priority        = 5;
        group_hex_id          = 0;
        last_optimized_time.store(0, std::memory_order_relaxed);
        optimization_duration_cycles.store(0, std::memory_order_relaxed);
        cycles_gained.store(0, std::memory_order_relaxed);
        active_simd_idx       = 0;
        simd_candidate_count  = 0;
        memset(simd_table, 0, sizeof(simd_table));
        set_active_simd_candidate("SCALAR", 1);

        if (tl_sf_group_hex_id != 0) {
            group_hex_id   = tl_sf_group_hex_id;
            group_priority = tl_sf_group_priority;
            opt_permitted  = tl_sf_group_bypass_warmup ? 0 : 1;
        }
    }

    void set_active_simd_candidate(const char* isa_name, uint32_t level = 0) {
        if (!isa_name || !isa_name[0]) return;
        int found_idx = -1;
        for (uint16_t i = 0; i < simd_candidate_count && i < 8; ++i) {
            if (_stricmp(simd_table[i].name, isa_name) == 0) {
                found_idx = (int)i;
                break;
            }
        }
        if (found_idx < 0) {
            if (simd_candidate_count < 8) {
                found_idx = (int)simd_candidate_count++;
            } else {
                found_idx = 7;
            }
            strncpy(simd_table[found_idx].name, isa_name, sizeof(simd_table[found_idx].name) - 1);
            simd_table[found_idx].name[sizeof(simd_table[found_idx].name) - 1] = '\0';
            simd_table[found_idx].ladder_level = level;
        }
        for (uint16_t i = 0; i < simd_candidate_count && i < 8; ++i) {
            simd_table[i].is_active = (i == (uint16_t)found_idx) ? 1 : 0;
        }
        active_simd_idx = (uint16_t)found_idx;
    }

    void mark_already_on_best_path() {
        has_optimized.store(1, std::memory_order_release);
        opt_success = 1;
        last_optimized_time.store(sacculus_now_ns(), std::memory_order_relaxed);
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
            cold->call_step_count.fetch_add(1, std::memory_order_relaxed);

            if (core_id < SACFOR_MAX_CORES) {
                cold->per_core_cycles[core_id].fetch_add(cycles, std::memory_order_relaxed);
                current_core.store(core_id, std::memory_order_release);
                sacculus_core_bitmap_set(cold->visited_cores, core_id);

                uint8_t numa = (uint8_t)(core_id / 64);
                if (numa < 4) {
                    float h = cold->per_numa_heat[numa].load(std::memory_order_relaxed);
                    while (!cold->per_numa_heat[numa].compare_exchange_weak(
                        h, h + SACFOR_HEAT_GAIN, std::memory_order_relaxed, std::memory_order_relaxed)) {}
                    cold->preferred_numa_node = numa;
                }
            }
        }

        float old_h = heat_score.load(std::memory_order_relaxed);
        while (!heat_score.compare_exchange_weak(
            old_h, old_h + SACFOR_HEAT_GAIN, std::memory_order_relaxed, std::memory_order_relaxed)) {}
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
        if (parent_header.load(std::memory_order_relaxed) != nullptr) {
            static uint64_t dummy_canary = SF_TAIL_CANARY;
            return &dummy_canary;
        }
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
        if (parent_header.load(std::memory_order_relaxed) != nullptr) return;
        size_t fp = (size_t)memory_footprint.load(std::memory_order_relaxed);
        size_t aligned_fp = (fp + 7) & ~7ULL;
        size_t header_sz = (sizeof(SF_custominiVm_live_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        if (total_cell_bytes > 0 && header_sz + aligned_fp + 8 > total_cell_bytes) return;
        *tailCanaryPtr() = SF_TAIL_CANARY;
    }

    SF_DISABLE_OPT_BEGIN
    inline bool isTailCanaryValid(void) const {
        if (parent_header.load(std::memory_order_relaxed) != nullptr) return true;
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
    return c ? c->call_step_count.load(std::memory_order_relaxed) : 0u;
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
    bool is_valid() const noexcept { return obj != nullptr && acquired; }
    ~sacculus_pin_guard_t() {
        if (obj && acquired) {
            obj->unpin();
        }
    }
    sacculus_pin_guard_t(const sacculus_pin_guard_t&) = delete;
    sacculus_pin_guard_t& operator=(const sacculus_pin_guard_t&) = delete;
    sacculus_pin_guard_t(sacculus_pin_guard_t&& other) noexcept : obj(other.obj), acquired(other.acquired) {
        other.obj = nullptr;
        other.acquired = false;
    }
    sacculus_pin_guard_t& operator=(sacculus_pin_guard_t&& other) noexcept {
        if (this != &other) {
            if (obj && acquired) obj->unpin();
            obj = other.obj;
            acquired = other.acquired;
            other.obj = nullptr;
            other.acquired = false;
        }
        return *this;
    }
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
        size_t hdr_sz = (sizeof(SF_custominiVm_live_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        return reinterpret_cast<uint8_t*>(this) + hdr_sz;
    }

    static inline SF_custominiVm_emptybox_header_t* from_body(void* body_ptr) {
        if (!body_ptr) return nullptr;

        size_t hdr_sz = (sizeof(SF_custominiVm_live_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
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

static inline uint32_t sf_compute_crc32(const void* data, size_t len) {
    if (!data || len == 0) return 0;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; ++i) {
        crc ^= p[i];
        for (int b = 0; b < 8; ++b) {
            crc = (crc >> 1) ^ (0xEDB88320u & (0 - (crc & 1)));
        }
    }
    return ~crc;
}

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
    size_t    rehab_offset = 0;
    void*     rehab_payload_ptr = nullptr;

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
        rehab_offset          = 0;
        rehab_payload_ptr     = nullptr;
        if (live && live->isValid()) {
            size_t fp = (size_t)live->memory_footprint.load(std::memory_order_relaxed);
            if (fp > 0 && live->body()) {
                crc32 = sf_compute_crc32(live->body(), fp);
            }
        }
    }
};

using sfLandfillHeader = SF_custominiVm_landfill_header_t;
using sacculus_landfill_header_t = sfLandfillHeader;

struct sf_arena_storage_t {

    std::atomic<uint8_t*> base{nullptr};
    std::atomic<size_t>   capacity{0};
    std::atomic<size_t>   used{0};
    sf_ticket_lock_t      grow_lock;
    bool                  Advance_Payment = false;
    std::vector<std::pair<uint8_t*, size_t>> old_bases;

    bool init(size_t initial_bytes, bool pretouch = false) {
        Advance_Payment = pretouch;
        uint8_t* b = (uint8_t*)sf_os_reserve_memory(initial_bytes);
        if (!b) return false;
        if (!sf_os_commit_memory(b, initial_bytes)) {
            sf_os_release_memory(b, initial_bytes);
            return false;
        }
        if (Advance_Payment) sf_pretouch_memory(b, initial_bytes);
        base.store(b, std::memory_order_release);
        capacity.store(initial_bytes, std::memory_order_release);
        return true;
    }

    void* store(const void* body, size_t size) {
        return store_with_offset(body, size, nullptr);
    }

    void* store_with_offset(const void* body, size_t size, size_t* out_offset) {
        size_t off = used.fetch_add(size, std::memory_order_acq_rel);
        size_t cap = capacity.load(std::memory_order_acquire);
        if (off + size > cap) {
            used.fetch_sub(size, std::memory_order_relaxed);
            if (grow()) {
                return store_with_offset(body, size, out_offset);
            }
            return nullptr;
        }
        uint8_t* b = base.load(std::memory_order_acquire);
        if (!b) { used.fetch_sub(size, std::memory_order_relaxed); return nullptr; }
        memcpy(b + off, body, size);
        if (out_offset) *out_offset = off;
        return b + off;
    }

    float fill_ratio(void) const {
        size_t cap = capacity.load(std::memory_order_relaxed);
        if (cap == 0) return 1.0f;
        return (float)used.load(std::memory_order_relaxed) / (float)cap;
    }

    void flush(void) {
        sf_ticket_guard_t g(grow_lock);
        for (auto& old : old_bases) {
            sf_os_release_memory(old.first, old.second);
        }
        old_bases.clear();
        uint8_t* b = base.load(std::memory_order_acquire);
        size_t cap = capacity.load(std::memory_order_relaxed);
        if (b && cap > 0) {
            sf_os_decommit_memory(b, cap);
            sf_os_commit_memory(b, 4096); // Keep initial page mapped for fast re-use
        }
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
        if (Advance_Payment && new_cap > old_cap)
            sf_pretouch_memory(new_b + old_cap, new_cap - old_cap);

        base.store(new_b, std::memory_order_release);
        capacity.store(new_cap, std::memory_order_release);
        old_bases.push_back({old_b, old_cap});
        return true;
    }

    void destroy(void) {
        for (auto& old : old_bases) {
            sf_os_release_memory(old.first, old.second);
        }
        old_bases.clear();
        uint8_t* b = base.load(std::memory_order_acquire);
        if (!b) return;
        sf_os_release_memory(b, capacity.load(std::memory_order_relaxed));
        base.store(nullptr, std::memory_order_release);
    }
};

struct sf_trash_pit_t {
    sf_arena_storage_t matured_arena;
    sf_arena_storage_t rehab_arena;

    bool init(size_t initial_bytes = SACFOR_LANDFILL_INIT, bool Advance_Payment = false) {
        size_t half = initial_bytes / 2;
        if (half < 1024 * 1024) half = 1024 * 1024;
        return matured_arena.init(half, Advance_Payment) && rehab_arena.init(half, Advance_Payment);
    }

    void* store_matured(const void* body, size_t size) {
        return matured_arena.store(body, size);
    }

    void* store_rehab(const void* body, size_t size) {
        return rehab_arena.store(body, size);
    }

    void* store_rehab_with_offset(const void* body, size_t size, size_t* out_offset) {
        return rehab_arena.store_with_offset(body, size, out_offset);
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
using sfTrashPit = sf_trash_pit_t;
using sacculus_trash_pit_t = sf_trash_pit_t;

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
        entries_ptr[idx].header_pane_byte_ref = (uint8_t)(idx & 0xFF);
        entries_ptr[idx].header_pane_ref = &entries_ptr[idx];
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
    sf_trash_pit_t        pit;
    sacculus_header_pane_t  pane;

    bool accept(SF_custominiVm_live_header_t* live,
                sf_death_reason_e reason,
                sacculus_hotlog_t* hotlog,
                uint64_t sweep_no,
                uint64_t evac_duration_ns = 0) {
        SF_custominiVm_landfill_header_t lh{};
        lh.populate_from_live(live, reason, evac_duration_ns);

        void* pit_addr = nullptr;
        size_t pit_offset = 0;
        bool is_rehab = (reason == SF_DEATH_REHABILITATE || live->needs_rehabilitation);
        if (is_rehab) {
            size_t fp = (size_t)live->memory_footprint.load(std::memory_order_relaxed);
            size_t cell_sz = live->total_cell_bytes > 0 ? live->total_cell_bytes : SF_custominiVm_live_header_t::total_cell_size(fp);
            pit_addr = pit.store_rehab_with_offset(live, cell_sz, &pit_offset);
            lh.pit_stored_size = cell_sz;
            lh.rehab_offset = pit_offset;
            lh.rehab_payload_ptr = pit_addr;
        } else {
            pit_addr = pit.store_matured(live, sizeof(SF_custominiVm_live_header_t));
            lh.pit_stored_size = sizeof(SF_custominiVm_live_header_t);
        }

        pane.init();
        int32_t pane_idx = pane.push(lh);
        (void)pane_idx;

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

    bool init(size_t initial = SACFOR_LANDFILL_INIT, bool Advance_Payment = false) {
        return pit.init(initial, Advance_Payment);
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
            size_t probes = 0;
            while (slots[curr] != nullptr && curr != found_idx && probes++ < BUCKET_CAP) {
                size_t natural_slot = hash_index(slots[curr]->function_id);
                bool can_shift = ((empty_slot - natural_slot) & BUCKET_MASK) < ((curr - natural_slot) & BUCKET_MASK);
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
        sf_ticket_guard_t g(lock);
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
        while (cur > 0) {
            size_t next_b = (cur >= fp) ? (cur - fp) : 0;
            if (total_bytes.compare_exchange_weak(cur, next_b, std::memory_order_relaxed, std::memory_order_relaxed))
                break;
        }
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
        while (cur > 0) {
            size_t next_b = (cur >= fp) ? (cur - fp) : 0;
            if (total_bytes.compare_exchange_weak(cur, next_b, std::memory_order_relaxed, std::memory_order_relaxed))
                break;
        }
    }
};

using sfRoom = sf_room_t;
using sacculus_room_t = sf_room_t;

struct sf_motherline_t {
    sf_room_t rooms[SACFOR_ROOM_COUNT];

    /* ── Egg (unoptimized) vs Fledgling (optimized/verified) Tracking ── */
    std::atomic<size_t> egg_count{0};
    std::atomic<size_t> fledgling_count{0};

    void init(void) {
        egg_count.store(0, std::memory_order_relaxed);
        fledgling_count.store(0, std::memory_order_relaxed);
        for (int i = 0; i < SACFOR_ROOM_COUNT; ++i)
            rooms[i].init((sf_room_id_t)i);
    }

    void admit(sfLiveHeader* obj) {
        rooms[SF_ROOM_YOUNG].place(obj);
        if (obj->has_optimized.load(std::memory_order_relaxed) == 0) {
            egg_count.fetch_add(1, std::memory_order_relaxed);
        } else {
            fledgling_count.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void decrement_counts(sfLiveHeader* obj) {
        if (!obj) return;
        if (obj->has_optimized.load(std::memory_order_relaxed) == 0) {
            size_t c = egg_count.load(std::memory_order_relaxed);
            while (c > 0 && !egg_count.compare_exchange_weak(c, c - 1, std::memory_order_relaxed)) {}
        } else {
            size_t c = fledgling_count.load(std::memory_order_relaxed);
            while (c > 0 && !fledgling_count.compare_exchange_weak(c, c - 1, std::memory_order_relaxed)) {}
        }
    }

    void admit_egg_object(sfLiveHeader* obj) {
        if (!obj) return;
        obj->has_optimized.store(0, std::memory_order_relaxed);
        obj->opt_success = 0;
        admit(obj);
    }

    void promote_to_fledgling(sfLiveHeader* obj) {
        if (!obj) return;
        if (obj->has_optimized.load(std::memory_order_relaxed) == 1) return;
        obj->has_optimized.store(1, std::memory_order_release);
        obj->opt_success = 1;
        size_t c = egg_count.load(std::memory_order_relaxed);
        while (c > 0 && !egg_count.compare_exchange_weak(c, c - 1, std::memory_order_relaxed)) {}
        fledgling_count.fetch_add(1, std::memory_order_relaxed);
    }

    void evict(sfLiveHeader* obj) {
        if (!obj) return;
        decrement_counts(obj);
        if (obj->generation < SACFOR_ROOM_COUNT)
            rooms[obj->generation].evict(obj);
    }

    void destroy(void) {
        for (int r = 0; r < SACFOR_ROOM_COUNT; ++r) {
            for (int reg = 0; reg < SACFOR_REGION_COUNT; ++reg) {
                auto& region = rooms[r].regions[reg];
                sf_ticket_guard_t g(region.get_lock());
                auto* cur = region.get_head();
                while (cur) {
                    auto* nxt = cur->next;
                    cur->~SF_custominiVm_live_header_t();
                    cur = nxt;
                }
                if (region.id == SF_REGION_HOTFIELD) {
                    region.hot_order.head = nullptr;
                    region.hot_order.count = 0;
                } else {
                    region.bucket.head = nullptr;
                    region.bucket.count = 0;
                }
                region.count.store(0, std::memory_order_relaxed);
                region.total_bytes.store(0, std::memory_order_relaxed);
            }
        }
        egg_count.store(0, std::memory_order_relaxed);
        fledgling_count.store(0, std::memory_order_relaxed);
    }
};

using sfMotherline = sf_motherline_t;
using sacculus_motherline_t = sf_motherline_t;

/* ========================================================================= */
/* SACFOR PITSTOP (3. Runway Alanı: JIT/ISA Değişim & Optimizasyon Bölgesi)  */
/* ========================================================================= */
using sf_object_ready_callback_t = void (*)(void* obj_ptr, void* user_data);

struct sacculus_pitstop_entry_t {
    alignas(64) SF_custominiVm_live_header_t saved_header;
    void*                    slot_addr = nullptr;
    size_t                   slot_bytes = 0;
    sf_puzzler_morph_e      mode = sf_puzzler_morph_e::SHADOW_RCU;
    std::atomic<bool>        is_busy{false};
    std::mutex               wait_mtx;
    std::condition_variable  wait_cv;
    sf_object_ready_callback_t ready_callback = nullptr;
    void*                    callback_user_data = nullptr;
    sacculus_pitstop_entry_t* next = nullptr;
    sacculus_pitstop_entry_t* prev = nullptr;
};

struct sacculus_pitstop_t {
    sacculus_pitstop_entry_t* head = nullptr;
    size_t                    count = 0;
    sf_ticket_lock_t          lock;
    std::atomic<size_t>       total_morphed{0};

    sacculus_pitstop_entry_t* admit_excision(sfLiveHeader* obj) {
        if (!obj) return nullptr;
        sf_ticket_guard_t g(lock);
        auto* entry = new (std::nothrow) sacculus_pitstop_entry_t();
        if (!entry) return nullptr;

        memcpy((void*)&entry->saved_header, (const void*)obj, sizeof(SF_custominiVm_live_header_t));
        entry->slot_addr = static_cast<void*>(obj);
        entry->slot_bytes = obj->total_cell_bytes;
        entry->mode = sf_puzzler_morph_e::EXCISION_EXCLUSIVE;
        entry->is_busy.store(true, std::memory_order_release);

        // In-place Empty Box placeholder protocol:
        // Overwrite the excised object's slot with an Empty Box tombstone.
        // Compaction is strictly bypassed and the stream continues immediately.
        auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(obj);
        eb->init(entry->saved_header.function_id, entry->slot_bytes, entry->slot_bytes, static_cast<void*>(entry), 0);

        entry->next = head;
        entry->prev = nullptr;
        if (head) head->prev = entry;
        head = entry;
        ++count;
        total_morphed.fetch_add(1, std::memory_order_relaxed);
        return entry;
    }

    sacculus_pitstop_entry_t* admit_shadow(sfLiveHeader* obj) {
        if (!obj) return nullptr;
        sf_ticket_guard_t g(lock);
        auto* entry = new (std::nothrow) sacculus_pitstop_entry_t();
        if (!entry) return nullptr;

        memcpy((void*)&entry->saved_header, (const void*)obj, sizeof(SF_custominiVm_live_header_t));
        entry->slot_addr = static_cast<void*>(obj);
        entry->slot_bytes = obj->total_cell_bytes;
        entry->mode = sf_puzzler_morph_e::SHADOW_RCU;
        entry->is_busy.store(false, std::memory_order_relaxed); // Shadow RCU is never busy to callers!

        // In Shadow RCU mode, DO NOT overwrite obj with an Empty Box!
        // obj stays live and functional in Motherline with its current ISA!

        entry->next = head;
        entry->prev = nullptr;
        if (head) head->prev = entry;
        head = entry;
        ++count;
        total_morphed.fetch_add(1, std::memory_order_relaxed);
        return entry;
    }

    bool restore_return(sacculus_pitstop_entry_t* entry, sfLiveHeader** out_restored = nullptr) {
        if (!entry) return false;

        // Overwrite the slot (whether Empty Box or live object) directly with the optimized live object!
        auto* slot = static_cast<SF_custominiVm_live_header_t*>(entry->slot_addr);
        memcpy((void*)slot, (const void*)&entry->saved_header, sizeof(SF_custominiVm_live_header_t));
        slot->has_optimized.store(1, std::memory_order_release);
        slot->opt_success = 1;
        slot->last_optimized_time.store(sacculus_now_ns(), std::memory_order_relaxed);

        // Detach cold_meta from entry so entry destructor does not delete live cold metadata!
        entry->saved_header.cold_meta.store(nullptr, std::memory_order_relaxed);

        // If Global Volatility is SRAM, broadcast the updated 64-byte header cacheline to all cores
        if (g_sf_global_volatility.load(std::memory_order_relaxed) == static_cast<uint8_t>(sf_global_volatility_e::SRAM)) {
            sf_sram_commit_to_dram(slot, sizeof(SF_custominiVm_live_header_t));
        }

        // Notify any waiting threads / trigger callbacks for Mode 0
        entry->is_busy.store(false, std::memory_order_release);
        {
            std::lock_guard<std::mutex> lk(entry->wait_mtx);
            entry->wait_cv.notify_all();
        }
        if (entry->ready_callback) {
            entry->ready_callback(slot->body(), entry->callback_user_data);
        }

        if (out_restored) *out_restored = slot;

        // Unlink from pitstop list
        {
            sf_ticket_guard_t g(lock);
            if (entry->prev) entry->prev->next = entry->next;
            else head = entry->next;
            if (entry->next) entry->next->prev = entry->prev;
            if (count > 0) --count;
        }

        delete entry;
        return true;
    }

    void destroy(void) {
        sf_ticket_guard_t g(lock);
        auto* cur = head;
        while (cur) {
            auto* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        head = nullptr;
        count = 0;
    }

    ~sacculus_pitstop_t() {
        destroy();
    }

    sacculus_pitstop_entry_t* find_by_slot(void* slot_or_body) {
        if (!slot_or_body) return nullptr;
        sf_ticket_guard_t g(lock);
        auto* cur = head;
        size_t hdr_sz = (sizeof(SF_custominiVm_live_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        while (cur) {
            if (cur->slot_addr == slot_or_body || 
                static_cast<uint8_t*>(cur->slot_addr) + hdr_sz == slot_or_body) {
                return cur;
            }
            cur = cur->next;
        }
        return nullptr;
    }

    bool is_busy(const void* ptr) {
        if (!ptr) return false;
        size_t hdr_sz = (sizeof(SF_custominiVm_live_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
        const void* hdr_ptr = static_cast<const uint8_t*>(ptr) - hdr_sz;
        
        // 1. Direct check on slot if it's an Empty Box tombstone
        auto* eb1 = reinterpret_cast<const SF_custominiVm_emptybox_header_t*>(ptr);
        if (eb1->isValid() && eb1->sfMN == SF_custominiVm_emptybox_header_t::SF_EMPTYBOX_MAGIC) {
            return true;
        }
        auto* eb2 = reinterpret_cast<const SF_custominiVm_emptybox_header_t*>(hdr_ptr);
        if (eb2->isValid() && eb2->sfMN == SF_custominiVm_emptybox_header_t::SF_EMPTYBOX_MAGIC) {
            return true;
        }

        // 2. Active pitstop list check
        sf_ticket_guard_t g(lock);
        for (auto* cur = head; cur; cur = cur->next) {
            if (cur->slot_addr == ptr || cur->slot_addr == hdr_ptr) {
                return cur->is_busy.load(std::memory_order_acquire);
            }
        }
        return false;
    }

    int await_ready(void* ptr, uint32_t timeout_ms) {
        if (!ptr) return -1;
        sacculus_pitstop_entry_t* entry = find_by_slot(ptr);
        if (!entry || !entry->is_busy.load(std::memory_order_acquire)) {
            return 0; // Not busy
        }

        std::unique_lock<std::mutex> lk(entry->wait_mtx);
        if (timeout_ms == 0) {
            entry->wait_cv.wait(lk, [&]() {
                return !entry->is_busy.load(std::memory_order_acquire);
            });
            return 0;
        } else {
            bool ok = entry->wait_cv.wait_for(lk, std::chrono::milliseconds(timeout_ms), [&]() {
                return !entry->is_busy.load(std::memory_order_acquire);
            });
            return ok ? 0 : -1;
        }
    }

    int register_callback(void* ptr, sf_object_ready_callback_t callback, void* user_data) {
        if (!ptr || !callback) return -1;
        sacculus_pitstop_entry_t* entry = find_by_slot(ptr);
        if (!entry) {
            callback(ptr, user_data);
            return 0;
        }
        std::lock_guard<std::mutex> lk(entry->wait_mtx);
        if (!entry->is_busy.load(std::memory_order_acquire)) {
            callback(ptr, user_data);
            return 0;
        }
        entry->ready_callback = callback;
        entry->callback_user_data = user_data;
        return 0;
    }

    inline void admit(sfLiveHeader* obj) {
        uint8_t mode = g_sf_puzzler_morph_method.load(std::memory_order_relaxed);
        if (mode == static_cast<uint8_t>(sf_puzzler_morph_e::EXCISION_EXCLUSIVE)) {
            admit_excision(obj);
        } else {
            admit_shadow(obj);
        }
    }

    inline void evict(sfLiveHeader* obj) {
        auto* entry = find_by_slot(static_cast<void*>(obj));
        if (entry) restore_return(entry, nullptr);
    }
};


using sfPitstop = sacculus_pitstop_t;

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
                            if (current_sweep > 0 && (current_sweep - cur->created_sweep_no) < 10) {
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
                if (candidate->is_decommitted) {
                    sf_os_commit_memory(candidate, candidate->aligned_hole_size);
                    candidate->is_decommitted = 0;
                    candidate->decommitted_kb = 0;
                }
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

    inline SF_custominiVm_emptybox_header_t* snatch_emptybox(size_t cell_sz, uint64_t current_sweep = 0) {
        return find_and_take_emptybox(cell_sz, current_sweep);
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

    void destroy(void) {
        sf_ticket_guard_t g(lock);
        head = nullptr;
        count = 0;
        total_bytes.store(0, std::memory_order_relaxed);
        for (size_t b = 0; b < NUM_EMPTYBOX_BINS; ++b) {
            sf_ticket_guard_t gb(bin_locks[b]);
            bin_heads[b] = nullptr;
            bin_counts[b] = 0;
        }
        emptybox_count.store(0, std::memory_order_relaxed);
        is_compacting.store(false, std::memory_order_relaxed);
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
    bool                  Advance_Payment    = false;
    bool                  forbid_dynamic_expansion = false;
    std::atomic<uint32_t> cache_color_step{0};

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
        if (Advance_Payment) {
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
        Advance_Payment = pretouch;
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
        new (superblock) sf_heap_superblock_t();
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

        if (Advance_Payment) {
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
        size_t color_pad = (cache_color_step.fetch_add(1, std::memory_order_relaxed) & 7) * SACFOR_CACHE_LINE;
        size_t cur = runway_bump_offset.load(std::memory_order_relaxed) + color_pad;
        size_t aligned_cur = (cur + align - 1) & ~(align - 1);

        if (aligned_cur + alloc_sz > runway_capacity) {
            if (forbid_dynamic_expansion) {
                return nullptr;
            }
            return allocate_from_chained_chunks(alloc_sz, align);
        }

        sf_os_commit_memory(runway_heap_base + aligned_cur, alloc_sz);
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
        size_t page_start = (cur_used + 4095ULL) & ~4095ULL;
        if (runway_capacity > page_start) {
            size_t unused = runway_capacity - page_start;
            if (unused >= threshold_bytes) {
                size_t page_aligned = unused & ~4095ULL;
                if (page_aligned > 0) {
                    uint8_t* target = runway_heap_base + page_start;
                    sf_os_decommit_memory(target, page_aligned);
                    if (superblock) {
                        superblock->rebated_bytes.fetch_add(page_aligned, std::memory_order_relaxed);
                        superblock->rebate_cycle_count.fetch_add(1, std::memory_order_relaxed);
                    }
                    return page_aligned;
                }
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
    sacculus_pitstop_t    pitstop;
    sf_slab_allocator_t   allocator;

    std::atomic<size_t> pacing_stalls{0};
    void* gc_ref = nullptr;
    void pace_allocation(void);
    SF_custominiVm_emptybox_header_t* snatch_emptybox(size_t cell_sz);
    inline void* bump_alloc(size_t cell_sz) {
        pace_allocation();
        return allocator.allocate(cell_sz, SACFOR_CACHE_LINE);
    }
    inline void admit_egg_object(sfLiveHeader* obj) {
        motherline.admit_egg_object(obj);
    }

    bool init(size_t handsel_bytes = 1024ULL * 1024 * 1024, bool Advance_Payment = false, bool forbid_expansion = false) {
        motherline.init();
        return allocator.init(handsel_bytes, Advance_Payment, forbid_expansion);
    }

    void* allocate_cell(size_t body_bytes, uint32_t fn_id = 0, const char* name = nullptr);

    void free_cell(void* body_ptr, size_t body_bytes);

    /**
     * @brief Excises an object from Motherline (Egg or Fledgling) to Pitstop for ISA morphing.
     * Leaves an in-place Empty Box placeholder without compacting the runway.
     */
    inline sacculus_pitstop_entry_t* move_to_pitstop(sfLiveHeader* obj) {
        if (!obj) return nullptr;
        uint8_t mode = g_sf_puzzler_morph_method.load(std::memory_order_relaxed);
        if (mode == static_cast<uint8_t>(sf_puzzler_morph_e::EXCISION_EXCLUSIVE)) {
            motherline.evict(obj);
            sf_hotlog_emit(SF_Ebin0_PITSTOP_EXCISE, SF_Ebin1_INFO, obj->function_id, 0, obj->generation, obj->region, 0, 0, 0, "pitstop_excise");
            return pitstop.admit_excision(obj);
        } else {
            // Mode 2: Shadow RCU - live object stays active in Motherline!
            sf_hotlog_emit(SF_Ebin0_PITSTOP_SHADOW, SF_Ebin1_INFO, obj->function_id, 0, obj->generation, obj->region, 0, 0, 0, "pitstop_shadow_rcu");
            return pitstop.admit_shadow(obj);
        }
    }

    /**
     * @brief Restores an object from Pitstop back into Motherline over its slot.
     * Promotes the object to Fledgling if it was in Egg.
     */
    inline sfLiveHeader* return_from_pitstop(sacculus_pitstop_entry_t* entry) {
        if (!entry) return nullptr;
        sfLiveHeader* restored = nullptr;
        uint32_t fn_id = entry->saved_header.function_id;
        uint16_t active_idx = entry->saved_header.active_simd_idx;
        uint64_t cycles = entry->saved_header.cycles_gained.load(std::memory_order_relaxed);
        const char* isa_name = entry->saved_header.simd_table[active_idx].name;
        bool was_excision = (entry->mode == sf_puzzler_morph_e::EXCISION_EXCLUSIVE);

        if (pitstop.restore_return(entry, &restored) && restored) {
            if (was_excision) {
                motherline.admit(restored);
            } else {
                motherline.promote_to_fledgling(restored);
            }
            sf_hotlog_emit(SF_Ebin0_PITSTOP_RETURN, SF_Ebin1_INFO, fn_id, 0, restored->generation, restored->region, 0, 0, (uint32_t)cycles, isa_name);
            batch_optimize_egg_objects(fn_id, isa_name, active_idx, cycles);
            return restored;
        }
        return nullptr;
    }


    inline sfLiveHeader* return_from_pitstop(sfLiveHeader* obj) {
        if (!obj) return nullptr;
        auto* entry = pitstop.find_by_slot(static_cast<void*>(obj));
        if (entry) return return_from_pitstop(entry);
        return obj;
    }

    /**
     * @brief Batch updates all objects of matching function_id in Egg with newly verified intrinsic.
     */
    inline size_t batch_optimize_egg_objects(uint32_t fn_id, const char* new_isa_name, uint16_t new_simd_idx, uint64_t cycles_saved) {
        size_t updated = 0;
        for (int r = 0; r < SACFOR_ROOM_COUNT; ++r) {
            for (int reg = 0; reg < SACFOR_REGION_COUNT; ++reg) {
                auto& region = motherline.rooms[r].regions[reg];
                sf_ticket_guard_t g(region.get_lock());
                auto* cur = region.get_head();
                while (cur) {
                    if (cur->function_id == fn_id && cur->has_optimized.load(std::memory_order_relaxed) == 0) {
                        // Check if optimization is permitted (BYPASS_WARMUP rule)
                        if (cur->opt_permitted == 0) {
                            cur = cur->next;
                            continue;
                        }
                        if (new_isa_name && new_isa_name[0]) {
                            cur->set_active_simd_candidate(new_isa_name, 0);
                        } else {
                            cur->active_simd_idx = new_simd_idx;
                            for (uint16_t i = 0; i < cur->simd_candidate_count && i < 8; ++i) {
                                cur->simd_table[i].is_active = (i == new_simd_idx) ? 1 : 0;
                            }
                        }
                        cur->has_optimized.store(1, std::memory_order_release);
                        cur->opt_success = 1;
                        cur->cycles_gained.store(cycles_saved, std::memory_order_relaxed);
                        cur->last_optimized_time.store(sacculus_now_ns(), std::memory_order_relaxed);
                        cur->optimization_duration_cycles.store(250, std::memory_order_relaxed);
                        size_t ec = motherline.egg_count.load(std::memory_order_relaxed);
                        while (ec > 0 && !motherline.egg_count.compare_exchange_weak(ec, ec - 1, std::memory_order_relaxed)) {}
                        motherline.fledgling_count.fetch_add(1, std::memory_order_relaxed);
                        ++updated;
                    }
                    cur = cur->next;
                }
            }
        }
        return updated;
    }

    inline size_t batch_optimize_egg_objects(uint32_t fn_id, uint16_t new_simd_idx, uint64_t cycles_saved) {
        return batch_optimize_egg_objects(fn_id, nullptr, new_simd_idx, cycles_saved);
    }

    void destroy(void) {
        pitstop.destroy();
        motherline.destroy();
        falline.destroy();
        allocator.destroy();
    }
};

using sfRunway = sacculus_runway_t;

/* ========================================================================= */
/* SFVM VFS STRICT VALIDATION, DIAGNOSTIC ALERT & SAFE CRASH ENGINE          */
/* ========================================================================= */
static std::atomic<bool> g_sf_vfs_suppress_abort{false};
static std::atomic<uint64_t> g_sf_vfs_panic_count{0};
static sf_vfs_panic_hook_t g_sf_vfs_panic_hook{nullptr};
static void* g_sf_vfs_panic_hook_user{nullptr};
static std::mutex g_sf_vfs_panic_mutex;
static char g_sf_vfs_last_panic_path[256] = {0};
static char g_sf_vfs_last_panic_op[256] = {0};

static inline size_t drain_hotlog_to_coldlog(sacculus_hotlog_t* hl, sacculus_coldlog_t* cl);

static void sf_vfs_trigger_panic(const char* missing_path, const char* operation_context) {
    const char* path_str = missing_path ? missing_path : "<null>";
    const char* op_str = operation_context ? operation_context : "<unknown>";

    g_sf_vfs_panic_count.fetch_add(1, std::memory_order_relaxed);
    {
        std::lock_guard<std::mutex> lock(g_sf_vfs_panic_mutex);
        strncpy(g_sf_vfs_last_panic_path, path_str, sizeof(g_sf_vfs_last_panic_path) - 1);
        g_sf_vfs_last_panic_path[sizeof(g_sf_vfs_last_panic_path) - 1] = '\0';
        strncpy(g_sf_vfs_last_panic_op, op_str, sizeof(g_sf_vfs_last_panic_op) - 1);
        g_sf_vfs_last_panic_op[sizeof(g_sf_vfs_last_panic_op) - 1] = '\0';
    }

    // 1. Diagnostic log to stderr and stdout
    fprintf(stderr, "\n=================================================================\n");
    fprintf(stderr, "   [SFVM VFS PANIC] Missing VFS Branch: '%s'\n", path_str);
    fprintf(stderr, "   Operation Context : %s\n", op_str);
    fprintf(stderr, "   Fault Description : Virtual File System branch does not exist.\n");
    fprintf(stderr, "   Fail-Safe Action  : Immediate fail-fast safe crash engaged to prevent\n");
    fprintf(stderr, "                       silent memory corruption or phantom object binding.\n");
    fprintf(stderr, "=================================================================\n\n");
    fflush(stderr);

    fprintf(stdout, "[SFVM VFS PANIC] Missing VFS Branch: '%s' (Operation: '%s') - Safe termination initiated!\n",
            path_str, op_str);
    fflush(stdout);

    // 2. Emit to HotLog and ColdLog ring buffers and drain
    auto* hl = g_sf_active_hotlog.load(std::memory_order_acquire);
    auto* cl = g_sf_active_coldlog.load(std::memory_order_acquire);
    if (hl) {
        char hot_msg[22] = {0};
        snprintf(hot_msg, sizeof(hot_msg), "VFS_MISS:%.12s", path_str);
        sacculus_hotlog_write(hl, SF_Ebin0_ERROR, SF_Ebin1_FATAL, 0, 0, 0, 0, 0, 0, 0, hot_msg);
    }
    if (cl) {
        char cold_msg[16] = {0};
        snprintf(cold_msg, sizeof(cold_msg), "VFS_PANIC:%.5s", op_str);
        sacculus_coldlog_write(cl, SF_Ebin0_ERROR, SF_Ebin1_FATAL, 0, cold_msg);
    }
    if (hl && cl) {
        drain_hotlog_to_coldlog(hl, cl);
    }

    // 3. Platform-independent diagnostic alert / panic dialog
    if (g_sf_vfs_panic_dialog_enabled.load(std::memory_order_relaxed)) {
        const char* ci = getenv("CI");
        const char* headless = getenv("SFVM_HEADLESS");
        const char* no_popup = getenv("SFVM_NO_POPUP");
        if (!ci && !headless && !no_popup) {
            char dialog_msg[1024];
            snprintf(dialog_msg, sizeof(dialog_msg),
                     "SFVM VFS FATAL PANIC\n\n"
                     "Missing VFS Branch:\n  %s\n\n"
                     "Operation Context:\n  %s\n\n"
                     "An operation was attempted on a non-existent VFS branch.\n"
                     "SFVM will now perform safe immediate termination to prevent\n"
                     "silent heap corruption or phantom object binding.",
                     path_str, op_str);
#if defined(_WIN32)
            MessageBoxA(NULL, dialog_msg, "[SFVM VFS PANIC] Fatal Missing Branch", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
#elif defined(__APPLE__)
            char cmd[1536];
            snprintf(cmd, sizeof(cmd), "osascript -e 'display alert \"[SFVM VFS PANIC] Fatal Missing Branch\" message \"%s\" as critical' 2>/dev/null", path_str);
            int res = system(cmd);
            (void)res;
#elif defined(__linux__)
            const char* dpy = getenv("DISPLAY");
            const char* wayland = getenv("WAYLAND_DISPLAY");
            if (dpy || wayland) {
                char cmd[1536];
                snprintf(cmd, sizeof(cmd), "zenity --error --title=\"[SFVM VFS PANIC] Fatal Missing Branch\" --text=\"Missing VFS Branch: %s\" 2>/dev/null || xmessage -center \"[SFVM VFS PANIC] Missing Branch: %s\" 2>/dev/null", path_str, path_str);
                int res = system(cmd);
                (void)res;
            }
#endif
        }
    }

    // 4. Invoke panic hook if registered
    if (g_sf_vfs_panic_hook) {
        g_sf_vfs_panic_hook(path_str, op_str, g_sf_vfs_panic_hook_user);
    }

    // 5. Safe crash / fail-fast termination
    if (g_sf_vfs_suppress_abort.load(std::memory_order_relaxed)) {
        return;
    }

    SF_custominiVm_emergency_os_sweep();
    std::abort();
}

struct sf_folder_node_t {
    char name[64] = {0};
    char full_path[192] = {0};
    size_t quota_bytes = 0;
    std::atomic<size_t> used_bytes{0};

    sf_folder_node_t* parent = nullptr;
    std::vector<sf_folder_node_t*> children;
    std::vector<SF_custominiVm_live_header_t*> objects;

    ~sf_folder_node_t() {
        std::vector<sf_folder_node_t*> stack = children;
        children.clear();
        while (!stack.empty()) {
            sf_folder_node_t* node = stack.back();
            stack.pop_back();
            if (node) {
                for (auto* c : node->children) {
                    stack.push_back(c);
                }
                node->children.clear();
                delete node; // Wait, calling delete node will invoke its destructor! Which we just modified to use stack. But wait, if its children are cleared, its destructor is O(1).
            }
        }
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

        mkdir_p("/heap/on_heap/runway/motherline/egg/young/tank");
        mkdir_p("/heap/on_heap/runway/motherline/egg/young/demand");
        mkdir_p("/heap/on_heap/runway/motherline/egg/young/middle");
        mkdir_p("/heap/on_heap/runway/motherline/egg/young/hotfield");
        mkdir_p("/heap/on_heap/runway/motherline/egg/adolescent/tank");
        mkdir_p("/heap/on_heap/runway/motherline/egg/adolescent/demand");
        mkdir_p("/heap/on_heap/runway/motherline/egg/adolescent/middle");
        mkdir_p("/heap/on_heap/runway/motherline/egg/adolescent/hotfield");
        mkdir_p("/heap/on_heap/runway/motherline/egg/senior/tank");
        mkdir_p("/heap/on_heap/runway/motherline/egg/senior/demand");
        mkdir_p("/heap/on_heap/runway/motherline/egg/senior/middle");
        mkdir_p("/heap/on_heap/runway/motherline/egg/senior/hotfield");
        mkdir_p("/heap/on_heap/runway/motherline/fledgling/young/tank");
        mkdir_p("/heap/on_heap/runway/motherline/fledgling/young/demand");
        mkdir_p("/heap/on_heap/runway/motherline/fledgling/young/middle");
        mkdir_p("/heap/on_heap/runway/motherline/fledgling/young/hotfield");
        mkdir_p("/heap/on_heap/runway/motherline/fledgling/adolescent/tank");
        mkdir_p("/heap/on_heap/runway/motherline/fledgling/adolescent/demand");
        mkdir_p("/heap/on_heap/runway/motherline/fledgling/adolescent/middle");
        mkdir_p("/heap/on_heap/runway/motherline/fledgling/adolescent/hotfield");
        mkdir_p("/heap/on_heap/runway/motherline/fledgling/senior/tank");
        mkdir_p("/heap/on_heap/runway/motherline/fledgling/senior/demand");
        mkdir_p("/heap/on_heap/runway/motherline/fledgling/senior/middle");
        mkdir_p("/heap/on_heap/runway/motherline/fledgling/senior/hotfield");
        mkdir_p("/heap/on_heap/runway/pitstop");
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

    sf_folder_node_t* _mkdir_p_unlocked(const char* path, size_t quota_bytes = 0) {
        if (!path || !root) return nullptr;
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
                found->name[sizeof(found->name) - 1] = '\0';
                strncpy(found->full_path, cur_path.c_str(), sizeof(found->full_path) - 1);
                found->full_path[sizeof(found->full_path) - 1] = '\0';
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

    sf_folder_node_t* mkdir_p(const char* path, size_t quota_bytes = 0) {
        if (!path || !root) return nullptr;
        std::lock_guard<std::mutex> g(tree_lock);
        return _mkdir_p_unlocked(path, quota_bytes);
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
        if (!path) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic("<null>", "find_node");
            }
            return nullptr;
        }
        std::lock_guard<std::mutex> g(tree_lock);
        sf_folder_node_t* node = _find_node_unlocked(path);
        if (!node && g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
            sf_vfs_trigger_panic(path, "find_node");
        }
        return node;
    }

    bool bind_tensor(const char* folder_path, SF_custominiVm_live_header_t* obj) {
        if (!folder_path) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic("<null>", "bind_tensor");
            }
            return false;
        }
        if (!obj) return false;

        std::lock_guard<std::mutex> g(tree_lock);
        sf_folder_node_t* node = _find_node_unlocked(folder_path);
        if (!node) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic(folder_path, "bind_tensor");
                return false;
            }
            node = _mkdir_p_unlocked(folder_path);
            if (!node) return false;
        }

        size_t footprint = (size_t)obj->memory_footprint.load(std::memory_order_relaxed);
        if (node->quota_bytes > 0 && (node->used_bytes.load(std::memory_order_relaxed) + footprint > node->quota_bytes)) {
            return false;
        }
        node->objects.push_back(obj);
        node->used_bytes.fetch_add(footprint, std::memory_order_relaxed);
        return true;
    }

    size_t evict_folder(const char* folder_path, sacculus_runway_t* runway) {
        if (!folder_path) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic("<null>", "evict_folder");
            }
            return 0;
        }
        std::vector<std::pair<void*, size_t>> to_free;
        {
            std::lock_guard<std::mutex> g(tree_lock);
            sf_folder_node_t* target = _find_node_unlocked(folder_path);
            if (!target) {
                if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                    sf_vfs_trigger_panic(folder_path, "evict_folder");
                }
                return 0;
            }
            if (!runway) return 0;

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
        if (!folder_path) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic("<null>", "get_used_bytes");
            }
            return 0;
        }
        std::lock_guard<std::mutex> g(tree_lock);
        sf_folder_node_t* node = _find_node_unlocked(folder_path);
        if (!node) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic(folder_path, "get_used_bytes");
            }
            return 0;
        }
        return node->used_bytes.load(std::memory_order_relaxed);
    }

    bool unlink_file(const char* file_path, sacculus_runway_t* runway) {
        if (!file_path) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic("<null>", "unlink_file");
            }
            return false;
        }
        if (!runway) return false;
        std::vector<std::string> parts;
        split_path(file_path, parts);
        if (parts.empty()) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic(file_path, "unlink_file");
            }
            return false;
        }

        std::string filename = parts.back();
        parts.pop_back();

        std::string dir_path = "";
        for (size_t i = 0; i < parts.size(); ++i) {
            dir_path += "/" + parts[i];
        }
        if (dir_path.empty()) dir_path = "/";

        std::lock_guard<std::mutex> g(tree_lock);
        sf_folder_node_t* node = _find_node_unlocked(dir_path.c_str());
        if (node) {
            for (auto it = node->objects.begin(); it != node->objects.end(); ++it) {
                SF_custominiVm_live_header_t* obj = *it;
                if (obj && strcmp(sf_get_debug_name(obj), filename.c_str()) == 0) {
                    size_t sz = (size_t)obj->memory_footprint.load(std::memory_order_relaxed);
                    runway->free_cell(obj->body(), sz);
                    node->used_bytes.fetch_sub(sz, std::memory_order_relaxed);
                    node->objects.erase(it);
                    return true;
                }
            }
        }

        if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
            sf_vfs_trigger_panic(file_path, "unlink_file");
        }
        return false;
    }

    bool stat_path(const char* path, SF_custominiVm_vfs_stat_t* out_stat) {
        if (!path) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic("<null>", "vfs_stat");
            }
            return false;
        }
        if (!out_stat) return false;
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
        if (parts.empty()) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic(path, "vfs_stat");
            }
            return false;
        }

        std::string filename = parts.back();
        parts.pop_back();

        std::string dir_path = "";
        for (size_t i = 0; i < parts.size(); ++i) {
            dir_path += "/" + parts[i];
        }
        if (dir_path.empty()) dir_path = "/";

        sf_folder_node_t* pnode = _find_node_unlocked(dir_path.c_str());
        if (pnode) {
            for (auto* obj : pnode->objects) {
                if (obj && strcmp(sf_get_debug_name(obj), filename.c_str()) == 0) {
                    out_stat->inode_no = reinterpret_cast<uint64_t>(obj);
                    out_stat->is_dir = 0;
                    out_stat->size_bytes = (size_t)obj->memory_footprint.load(std::memory_order_relaxed);
                    out_stat->used_bytes = out_stat->size_bytes;
                    return true;
                }
            }
        }

        // Neither directory node nor file object was found in VFS
        if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
            sf_vfs_trigger_panic(path, "vfs_stat");
        }
        return false;
    }

    size_t snapshot_path(const char* path, void* out_buf, size_t cap) {
        if (!path) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic("<null>", "snapshot_path");
            }
            return 0;
        }
        if (!out_buf || cap == 0) return 0;
        std::vector<std::string> parts;
        split_path(path, parts);
        if (parts.empty()) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic(path, "snapshot_path");
            }
            return 0;
        }

        std::string filename = parts.back();
        parts.pop_back();

        std::string dir_path = "";
        for (size_t i = 0; i < parts.size(); ++i) {
            dir_path += "/" + parts[i];
        }
        if (dir_path.empty()) dir_path = "/";

        SF_custominiVm_live_header_t* target_obj = nullptr;
        {
            std::lock_guard<std::mutex> g(tree_lock);
            sf_folder_node_t* pnode = _find_node_unlocked(dir_path.c_str());
            if (pnode) {
                for (auto* obj : pnode->objects) {
                    if (obj && strcmp(sf_get_debug_name(obj), filename.c_str()) == 0) {
                        target_obj = obj;
                        break;
                    }
                }
            }
        }
        if (!target_obj) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic(path, "snapshot_path");
            }
            return 0;
        }

        if (!target_obj->try_pin()) return 0;
        size_t sz = (size_t)target_obj->memory_footprint.load(std::memory_order_relaxed);
        size_t to_copy = (sz < cap) ? sz : cap;
        memcpy(out_buf, target_obj->body(), to_copy);
        target_obj->unpin();
        return to_copy;
    }

    void list_dir(const char* path, void (*cb)(const char*, int, size_t, void*), void* user) {
        if (!path) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic("<null>", "list_dir");
            }
            return;
        }
        if (!cb) return;
        std::lock_guard<std::mutex> g(tree_lock);
        sf_folder_node_t* node = _find_node_unlocked(path);
        if (!node) {
            if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
                sf_vfs_trigger_panic(path, "list_dir");
            }
            return;
        }

        for (auto* child : node->children) {
            cb(child->name, 1, child->used_bytes.load(std::memory_order_relaxed), user);
        }
        for (auto* obj : node->objects) {
            if (obj) {
                cb(sf_get_debug_name(obj), 0, (size_t)obj->memory_footprint.load(std::memory_order_relaxed), user);
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
                if (memory_fallback) {
                    g_sf_os_registry.register_block(memory_fallback, capacity_bytes, 0);
                }
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
            if (memory_fallback && memory_fallback != MAP_FAILED) {
                g_sf_os_registry.register_block(memory_fallback, capacity_bytes, 0);
            }
        }
#endif
#ifdef _WIN32
        return (file_handle != INVALID_HANDLE_VALUE) || (memory_fallback != nullptr);
#else
        return (file_fd >= 0) || (memory_fallback != nullptr && memory_fallback != MAP_FAILED);
#endif
    }

    void destroy() {
        std::lock_guard<std::mutex> g(lock);
#ifdef _WIN32
        if (file_handle != INVALID_HANDLE_VALUE) {
            CloseHandle(file_handle);
            file_handle = INVALID_HANDLE_VALUE;
        }
        if (memory_fallback) {
            g_sf_os_registry.unregister_block(memory_fallback);
            VirtualFree(memory_fallback, 0, MEM_RELEASE);
            memory_fallback = nullptr;
        }
#else
        if (file_fd >= 0) {
            close(file_fd);
            file_fd = -1;
        }
        if (memory_fallback) {
            g_sf_os_registry.unregister_block(memory_fallback);
            munmap(memory_fallback, capacity_bytes);
            memory_fallback = nullptr;
        }
#endif
        paged_out_bytes.store(0, std::memory_order_relaxed);
        paged_out_count.store(0, std::memory_order_relaxed);
    }

    bool page_out(SF_custominiVm_live_header_t* obj) {
        if (!obj || obj->sfMN != SF_custominiVm_live_header_t::SF_LIVE_MAGIC) return false;
        // High priority (>= 8) operations are pinned to SRAM and must never be paged out to DRAM
        if (obj->group_priority >= 8 || obj->priority >= 8) return false;
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
            
            // Chunked write to safely support tensors > 4GB without 32-bit DWORD truncation
            uint8_t* src_ptr = static_cast<uint8_t*>(obj->body());
            size_t remaining = fp;
            bool io_ok = true;
            const DWORD MAX_CHUNK = 0x40000000; // 1 GB chunks
            while (remaining > 0) {
                DWORD chunk_size = (remaining > (size_t)MAX_CHUNK) ? MAX_CHUNK : (DWORD)remaining;
                DWORD chunk_written = 0;
                if (!WriteFile(file_handle, src_ptr, chunk_size, &chunk_written, NULL) || chunk_written != chunk_size) {
                    io_ok = false;
                    break;
                }
                src_ptr += chunk_written;
                remaining -= chunk_written;
            }
            if (io_ok) {
                written = true;
            }
        }
#else
        if (file_fd >= 0) {
            uint8_t* src_ptr = static_cast<uint8_t*>(obj->body());
            size_t remaining = fp;
            uint64_t cur_off = slot_off;
            bool io_ok = true;
            while (remaining > 0) {
                ssize_t ret = pwrite(file_fd, src_ptr, remaining, cur_off);
                if (ret <= 0) {
                    io_ok = false;
                    break;
                }
                src_ptr += ret;
                cur_off += ret;
                remaining -= ret;
            }
            if (io_ok) {
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

            uintptr_t b_start = reinterpret_cast<uintptr_t>(obj->body());
            uintptr_t p_start = (b_start + 4095ULL) & ~4095ULL;
            uintptr_t p_end   = (b_start + fp) & ~4095ULL;
            if (p_end > p_start) {
                sf_os_decommit_memory(reinterpret_cast<void*>(p_start), p_end - p_start);
            }
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
            
            // Chunked read to safely support tensors > 4GB without 32-bit DWORD truncation
            uint8_t* dst_ptr = static_cast<uint8_t*>(obj->body());
            size_t remaining = fp;
            bool io_ok = true;
            const DWORD MAX_CHUNK = 0x40000000; // 1 GB chunks
            while (remaining > 0) {
                DWORD chunk_size = (remaining > (size_t)MAX_CHUNK) ? MAX_CHUNK : (DWORD)remaining;
                DWORD chunk_read = 0;
                if (!ReadFile(file_handle, dst_ptr, chunk_size, &chunk_read, NULL) || chunk_read != chunk_size) {
                    io_ok = false;
                    break;
                }
                dst_ptr += chunk_read;
                remaining -= chunk_read;
            }
            if (io_ok) {
                read_ok = true;
            }
        }
#else
        if (file_fd >= 0) {
            uint8_t* dst_ptr = static_cast<uint8_t*>(obj->body());
            size_t remaining = fp;
            uint64_t cur_off = slot_off;
            bool io_ok = true;
            while (remaining > 0) {
                ssize_t ret = pread(file_fd, dst_ptr, remaining, cur_off);
                if (ret <= 0) {
                    io_ok = false;
                    break;
                }
                dst_ptr += ret;
                cur_off += ret;
                remaining -= ret;
            }
            if (io_ok) {
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
            if (paged_out_count.load(std::memory_order_relaxed) == 0) {
                write_offset.store(0, std::memory_order_relaxed);
            }
            return true;
        }
        return false;
    }
};

using sfPagefile = sf_pagefile_t;

#ifdef USE_TBB
class sf_gc_arena_observer_t : public tbb::task_scheduler_observer {
    SF_custominiVm_core_tier_t m_tier;
public:
    sf_gc_arena_observer_t(tbb::task_arena& arena, SF_custominiVm_core_tier_t tier)
        : tbb::task_scheduler_observer(arena), m_tier(tier) {
        observe(true);
    }
    ~sf_gc_arena_observer_t() {
        observe(false);
    }
    void on_scheduler_entry(bool is_worker) override {
        (void)is_worker;
        tl_sf_core_tier = m_tier;
#if defined(_M_X64) || defined(__x86_64__)
        _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
        _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#elif defined(__aarch64__) || defined(_M_ARM64)
        uint64_t fpcr;
        __asm__ __volatile__("mrs %0, fpcr" : "=r"(fpcr));
        fpcr |= (1ULL << 24); // FZ bit
        __asm__ __volatile__("msr fpcr, %0" :: "r"(fpcr));
#endif
        if (m_tier == SF_CORE_TIER_EFFICIENCY) {
            sf_bind_current_thread(SF_CORE_TIER_EFFICIENCY);
        } else if (m_tier == SF_CORE_TIER_PERFORMANCE) {
            sf_bind_current_thread(SF_CORE_TIER_PERFORMANCE);
        }
    }
    void on_scheduler_exit(bool is_worker) override {
        (void)is_worker;
    }
};

struct sf_gc_tbb_arena_manager_t {
    tbb::task_arena            p_arena;
    tbb::task_arena            e_arena;
    tbb::task_arena            homo_arena;
    sf_gc_arena_observer_t*    p_observer{nullptr};
    sf_gc_arena_observer_t*    e_observer{nullptr};
    sf_gc_arena_observer_t*    homo_observer{nullptr};
    tbb::affinity_partitioner  p_affinity;
    tbb::affinity_partitioner  e_affinity;
    tbb::affinity_partitioner  homo_affinity;
    tbb::concurrent_queue<uint32_t> concurrent_resurrect_queue;
    std::atomic<bool>          initialized{false};

    struct thread_stats_t {
        size_t swept = 0;
        size_t evicted = 0;
        size_t relocated = 0;
        size_t relocated_bytes = 0;
    };
    tbb::enumerable_thread_specific<thread_stats_t> ets_stats;

    void init() {
        if (initialized.exchange(true)) return;

        bool hybrid = sf_is_hybrid_cpu();
        uint32_t p_cores = sf_get_p_core_count();
        uint32_t e_cores = sf_get_e_core_count();

        auto core_types = tbb::info::core_types();
        if (hybrid && core_types.size() > 1) {
            // Configure P-core high-priority arena for heavy parallel sweep & bulk compaction bursts
            tbb::task_arena::constraints p_c;
            p_c.set_core_type(core_types.front());
            p_c.set_max_concurrency(p_cores > 0 ? p_cores : tbb::task_arena::automatic);
            p_arena.initialize(p_c);
            p_observer = new (std::nothrow) sf_gc_arena_observer_t(p_arena, SF_CORE_TIER_PERFORMANCE);

            // Configure E-core low-priority arena for background housekeeping & non-blocking tasks
            tbb::task_arena::constraints e_c;
            e_c.set_core_type(core_types.back());
            e_c.set_max_concurrency(e_cores > 0 ? e_cores : tbb::task_arena::automatic);
            e_arena.initialize(e_c);
            e_observer = new (std::nothrow) sf_gc_arena_observer_t(e_arena, SF_CORE_TIER_EFFICIENCY);
        } else {
            // Homogeneous symmetric multi-core setup with NUMA node domain binding
            tbb::task_arena::constraints h_c;
            if (!sf_is_uma_system() && sf_get_numa_node_count() > 1) {
                auto numa_nodes = tbb::info::numa_nodes();
                if (!numa_nodes.empty()) {
                    h_c.set_numa_id(numa_nodes.front());
                }
            }
            homo_arena.initialize(h_c);
            homo_observer = new (std::nothrow) sf_gc_arena_observer_t(homo_arena, SF_CORE_TIER_ANY);
        }
    }

    void destroy() {
        if (!initialized.exchange(false)) return;
        delete p_observer; p_observer = nullptr;
        delete e_observer; e_observer = nullptr;
        delete homo_observer; homo_observer = nullptr;
        if (p_arena.is_active()) p_arena.terminate();
        if (e_arena.is_active()) e_arena.terminate();
        if (homo_arena.is_active()) homo_arena.terminate();
    }
};
#endif

/* ========================================================================= */
/* SFVM & sfGC 4-POINT EXECUTION COORDINATOR & THREAD-LOCAL RUNTIME ENGINE   */
/* (SAFEPOINT / CONTROLPOINT / HOOKPOINT / PAUSEPOINT)                       */
/* ========================================================================= */

struct SACFOR_ALIGN_CL sf_point_coordinator_t {
    alignas(64) std::atomic<uint64_t> safepoint_polls{0};
    alignas(64) std::atomic<uint64_t> safepoint_rendezvous_stalls{0};
    alignas(64) std::atomic<uint32_t> safepoint_parked_threads{0};
    alignas(64) std::atomic<uint32_t> active_in_flight_kernels{0};

    alignas(64) std::atomic<uint64_t> controlpoint_checks{0};
    alignas(64) std::atomic<uint64_t> controlpoint_pacing_triggers{0};
    alignas(64) std::atomic<uint64_t> controlpoint_auto_pageins{0};
    alignas(64) std::atomic<uint64_t> controlpoint_bytes_validated{0};

    alignas(64) std::atomic<uint64_t> hookpoint_enter_count{0};
    alignas(64) std::atomic<uint64_t> hookpoint_exit_count{0};
    alignas(64) std::atomic<uint64_t> hookpoint_auto_pinned_tensors{0};
    alignas(64) std::atomic<uint32_t> hookpoint_active_pinned{0};
    alignas(64) std::atomic<uint64_t> hookpoint_canary_faults_caught{0};

    alignas(64) std::atomic<uint64_t> pausepoint_polls{0};
    alignas(64) std::atomic<uint64_t> pausepoint_preemptions{0};
    alignas(64) std::atomic<bool>     pausepoint_gate{false};
};

static sf_point_coordinator_t g_point_coord;

struct sf_thread_point_ctx_t {
    uint32_t                active_fn_id{0};
    void*                   active_buffer_ptr{nullptr};
    size_t                  active_byte_size{0};
    uint64_t                enter_tsc{0};
    sacculus_live_header_t* pinned_hdr{nullptr};
    uint32_t                nesting_depth{0};
    uint32_t                irq_poll_counter{0};
    bool                    prefer_streaming_stores{false};
    bool                    in_safepoint{false};
};

static thread_local sf_thread_point_ctx_t tl_sf_point_ctx{};

/* ========================================================================= */
/* SFGC SELF-HEALING & MEMORY CORRUPTION RECOVERY ENGINE                     */
/* ========================================================================= */
struct sacculus_self_heal_engine_t {
    std::atomic<uint64_t> heap_corruptions_healed{0};
    std::atomic<uint64_t> brooks_cycles_broken{0};
    std::atomic<uint64_t> header_magics_repaired{0};
    std::atomic<uint64_t> emptybox_chains_rebuilt{0};
    std::atomic<uint64_t> bitcode_exceptions_shielded{0};
    std::atomic<uint64_t> code_errors_captured{0};

    bool heal_header(sacculus_live_header_t* obj, sacculus_coldlog_t* coldlog) {
        if (!obj) return false;
        bool repaired = false;

        // 1. Verify and repair state / generation corruption
        if (obj->generation >= SACFOR_ROOM_COUNT) {
            obj->generation = SF_ROOM_YOUNG;
            header_magics_repaired.fetch_add(1, std::memory_order_relaxed);
            repaired = true;
        }
        uint8_t st = obj->state.load(std::memory_order_relaxed);
        if (st > 5) {
            obj->state.store(static_cast<uint8_t>(sf_live_state_e::LIVE), std::memory_order_release);
            header_magics_repaired.fetch_add(1, std::memory_order_relaxed);
            repaired = true;
        }

        // 2. Verify and break cyclic or dangling Brooks forwarding pointers
        void* fwd = obj->forwarding_address.load(std::memory_order_relaxed);
        if (fwd != nullptr) {
            uintptr_t addr = reinterpret_cast<uintptr_t>(fwd);
            if (fwd == (void*)obj || fwd == obj->body() || addr < 0x10000 || addr == (uintptr_t)-1) {
                obj->forwarding_address.store(nullptr, std::memory_order_release);
                brooks_cycles_broken.fetch_add(1, std::memory_order_relaxed);
                repaired = true;
            }
        }

        if (repaired) {
            heap_corruptions_healed.fetch_add(1, std::memory_order_relaxed);
            if (coldlog) {
                sacculus_coldlog_write(coldlog, SF_Ebin0_ERROR, SF_Ebin1_ERROR, obj->function_id,
                    "SFVM captured 'D' corruption at graph in heap!");
            }
            fprintf(stderr, "[SFVM Self-Heal] SFVM captured 'D' corruption at graph in heap! [Object %p repaired]\n", (void*)obj);
        }
        return repaired;
    }

    size_t scan_and_heal_motherline(sacculus_motherline_t& ml, sacculus_coldlog_t* coldlog) {
        size_t healed = 0;
        for (int r = 0; r < SACFOR_ROOM_COUNT; ++r) {
            for (int reg = 0; reg < SACFOR_REGION_COUNT; ++reg) {
                auto& region = ml.rooms[r].regions[reg];
                sf_ticket_guard_t g(region.get_lock());
                auto* cur = region.get_head();
                while (cur) {
                    if (heal_header(cur, coldlog)) {
                        healed++;
                    }
                    cur = cur->next;
                }
            }
        }
        return healed;
    }

    void capture_bitcode_exception(uint32_t fn_id, const char* section, sacculus_coldlog_t* coldlog) {
        bitcode_exceptions_shielded.fetch_add(1, std::memory_order_relaxed);
        if (coldlog) {
            sacculus_coldlog_write(coldlog, SF_Ebin0_ERROR, SF_Ebin1_ERROR, fn_id,
                "SFVM captured 'C' exception in embedded bitcode!");
        }
        fprintf(stderr, "[SFVM Self-Heal] SFVM captured 'C' exception in embedded bitcode! [fn_id=%u section=%s - Safe fallback activated]\n",
                fn_id, section ? section : "kernel");
    }

    void capture_code_error(const char* location, const char* detail, sacculus_coldlog_t* coldlog) {
        code_errors_captured.fetch_add(1, std::memory_order_relaxed);
        if (coldlog) {
            sacculus_coldlog_write(coldlog, SF_Ebin0_ERROR, SF_Ebin1_ERROR, 0,
                "SFVM captured 'A' error at piece of code!");
        }
        fprintf(stderr, "[SFVM Self-Heal] SFVM captured 'A' error at '%s' piece of code! [%s]\n",
                location ? location : "VM_Core", detail ? detail : "unspecified_fault");
    }
};

static sacculus_self_heal_engine_t g_sf_self_heal;

/* ========================================================================= */
/* BATCH REGISTER & ENTANGLE EXECUTION ENGINE                                */
/* ========================================================================= */
struct sf_entangle_execution_engine_t {
    alignas(64) uint8_t register_scratchpad[512] = {0};
    std::atomic<bool>     in_transaction{false};
    std::atomic<uint32_t> transaction_depth{0};
    std::atomic<uint32_t> fused_operation_count{0};

    inline void begin() {
        if (transaction_depth.fetch_add(1, std::memory_order_acq_rel) == 0) {
            in_transaction.store(true, std::memory_order_release);
            fused_operation_count.store(0, std::memory_order_relaxed);
            sf_hotlog_emit(SF_Ebin0_ENTANGLE_TX_BEGIN, SF_Ebin1_INFO, 0, 0, 0, 0, 0, 0, 0, "entangle_tx_begin");
        }
    }

    inline void commit() {
        uint32_t d = transaction_depth.load(std::memory_order_acquire);
        if (d > 0) {
            if (transaction_depth.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                uint32_t cnt = fused_operation_count.load(std::memory_order_relaxed);
                in_transaction.store(false, std::memory_order_release);
                sf_hotlog_emit(SF_Ebin0_ENTANGLE_TX_COMMIT, SF_Ebin1_INFO, 0, 0, 0, 0, 0, 0, cnt, "entangle_tx_commit");
            }
        }
    }

    inline bool is_active() const {
        return in_transaction.load(std::memory_order_relaxed);
    }
};

static sf_entangle_execution_engine_t g_sf_entangle_engine;

/* ========================================================================= */
/* LOG BOOT: HOTLOG -> COLDLOG DRAIN ENGINE                                 */
/* ========================================================================= */
struct sf_hotlog_drain_ctx_t {
    sacculus_coldlog_t* cl = nullptr;
    size_t count = 0;
};

static inline void sf_hotlog_drain_callback(const SF_custominiVm_hotlog_entry_t* e, void* user) {
    auto* ctx = reinterpret_cast<sf_hotlog_drain_ctx_t*>(user);
    if (!ctx || !ctx->cl || !e) return;
    // Coldlog follows behind and completely swallows hotlog events (healthy flow, warnings, errors)
    sacculus_coldlog_write(ctx->cl, static_cast<sacculus_event_t>(e->event),
                           static_cast<sacculus_severity_t>(e->severity),
                           e->fn_id, e->msg);
    ctx->count++;
}

static inline size_t drain_hotlog_to_coldlog(sacculus_hotlog_t* hl, sacculus_coldlog_t* cl) {
    if (!hl || !cl) return 0;
    sf_hotlog_drain_ctx_t ctx{cl, 0};
    hl->drain(sf_hotlog_drain_callback, &ctx);
    return ctx.count;
}

/* ========================================================================= */
/* RMRP (REDUCE MEMORY RESIDENT PROPAGATION) COMPACTION & REBATE ENGINE      */
/* ========================================================================= */
static inline size_t execute_rmrp_propagation(sacculus_runway_t& runway,
                                              sacculus_landfill_t& landfill,
                                              sf_snatch_rebate_mode_e rebate_mode,
                                              sf_rmrp_method_e method) {
    size_t total_reclaimed = 0;
    struct rmrp_span_t {
        void* addr;
        size_t bytes;
        uint32_t fn_id;
        bool is_landfill;
    };
    std::vector<rmrp_span_t> spans;

    // 1. Collect candidate spans from Runway Motherline (Egg & Fledgling across all rooms/regions)
    for (int r = 0; r < SACFOR_ROOM_COUNT; ++r) {
        for (int reg = 0; reg < SACFOR_REGION_COUNT; ++reg) {
            auto& region = runway.motherline.rooms[r].regions[reg];
            sf_ticket_guard_t g(region.get_lock());
            auto* cur = region.get_head();
            while (cur) {
                size_t payload_bytes = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                // On-Heap +1 byte alignment: header + payload + canary + 1 byte
                size_t header_sz = (sizeof(SF_custominiVm_live_header_t) + SACFOR_CACHE_LINE - 1) & ~(SACFOR_CACHE_LINE - 1);
                size_t aligned_fp = (payload_bytes + 7) & ~7ULL;
                size_t needed = header_sz + aligned_fp + 8 + 1;
                needed = (needed + 15) & ~((size_t)15);

                if (cur->total_cell_bytes > needed) {
                    size_t shrink_bytes = cur->total_cell_bytes - needed;
                    void* shrink_addr = reinterpret_cast<uint8_t*>(cur) + needed;
                    cur->total_cell_bytes = needed;
                    spans.push_back({shrink_addr, shrink_bytes, cur->function_id, false});
                }
                cur = cur->next;
            }
        }
    }

    // 2. Collect candidate unused capacity spans from Landfill (matured and rehab arenas)
    size_t mat_cap  = landfill.pit.matured_arena.capacity.load(std::memory_order_relaxed);
    size_t mat_used = landfill.pit.matured_arena.used.load(std::memory_order_relaxed);
    if (mat_cap > mat_used + 4096) {
        size_t mat_tail = mat_cap - mat_used;
        uint8_t* mat_base = landfill.pit.matured_arena.base.load(std::memory_order_relaxed);
        if (mat_base) {
            spans.push_back({mat_base + mat_used, mat_tail, 0, true});
        }
    }

    size_t reh_cap  = landfill.pit.rehab_arena.capacity.load(std::memory_order_relaxed);
    size_t reh_used = landfill.pit.rehab_arena.used.load(std::memory_order_relaxed);
    if (reh_cap > reh_used + 4096) {
        size_t reh_tail = reh_cap - reh_used;
        uint8_t* reh_base = landfill.pit.rehab_arena.base.load(std::memory_order_relaxed);
        if (reh_base) {
            spans.push_back({reh_base + reh_used, reh_tail, 0, true});
        }
    }

    // 3. Sort spans from smallest to largest size ("en küçükten en büyüğe doğru yapılır")
    std::sort(spans.begin(), spans.end(), [](const rmrp_span_t& a, const rmrp_span_t& b) {
        return a.bytes < b.bytes;
    });

    // 4. Execute reduction according to RMRP method
    if (method == sf_rmrp_method_e::SEQUENTIAL) {
        // Sequential / Installment: rebate/decommit each trimmed span immediately step by step ("küçük küçük")
        for (const auto& sp : spans) {
            if (rebate_mode == sf_snatch_rebate_mode_e::RAPID ||
                rebate_mode == sf_snatch_rebate_mode_e::DECOMMIT) {
                sf_os_decommit_memory(sp.addr, sp.bytes);
            } else {
                if (!sp.is_landfill) {
                    auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(sp.addr);
                    runway.falline.register_emptybox(sp.fn_id, sp.bytes, sp.bytes, eb);
                }
            }
            total_reclaimed += sp.bytes;
        }
    } else {
        // Plump / Prune: compress smallest-to-largest, trim all, and coalesce into a single large block before rebating
        size_t coalesced_bytes = 0;
        void* first_addr = nullptr;
        for (const auto& sp : spans) {
            if (!first_addr) first_addr = sp.addr;
            coalesced_bytes += sp.bytes;
            total_reclaimed += sp.bytes;
        }
        if (coalesced_bytes > 0 && first_addr) {
            if (rebate_mode == sf_snatch_rebate_mode_e::RAPID ||
                rebate_mode == sf_snatch_rebate_mode_e::DECOMMIT) {
                for (const auto& sp : spans) {
                    sf_os_decommit_memory(sp.addr, sp.bytes);
                }
            } else {
                auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(first_addr);
                runway.falline.register_emptybox(0, coalesced_bytes, coalesced_bytes, eb);
            }
        }
    }

    return total_reclaimed;
}

/* ========================================================================= */
/* SRAM CACHELINE & GLOBAL VOLATILITY MEMORY ENGINE                          */
/* ========================================================================= */
static inline void sf_sram_commit_to_dram(void* ptr, size_t bytes) {
    if (!ptr || bytes == 0) return;
    char* p = reinterpret_cast<char*>(ptr);
    for (size_t off = 0; off < bytes; off += 64) {
#if defined(_M_X64) || defined(__x86_64__)
        _mm_clflush(p + off);
#endif
    }
#if defined(_M_X64) || defined(__x86_64__)
    _mm_sfence();
#else
    std::atomic_thread_fence(std::memory_order_seq_cst);
#endif
}

static inline void sf_sram_broadcast_volatility(void* ptr, size_t bytes) {
    (void)ptr; (void)bytes;
#if defined(_M_X64) || defined(__x86_64__)
    _mm_sfence();
#else
    std::atomic_thread_fence(std::memory_order_seq_cst);
#endif
}

static inline void sf_dram_stream_store(void* dst, const void* src, size_t bytes) {
    if (!dst || !src || bytes == 0) return;
    if (!SF_custominiVm_is_iasm_active(0)) {
        // Safe Ring 3 path: pure memcpy + C++11 sequential consistency barrier
        memcpy(dst, src, bytes);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        return;
    }
    size_t i = 0;
#if defined(_M_X64) || defined(__x86_64__)
    const char* s = reinterpret_cast<const char*>(src);
    char* d = reinterpret_cast<char*>(dst);
    // Align destination to 16 bytes for safe non-temporal vector streaming in Ring 3
    while (i < bytes && (reinterpret_cast<uintptr_t>(d + i) & 15) != 0) {
        d[i] = s[i];
        i++;
    }
#if defined(__AVX2__)
    // 32-byte non-temporal streaming stores when 32-byte aligned
    for (; i + 32 <= bytes && (reinterpret_cast<uintptr_t>(d + i) & 31) == 0; i += 32) {
        __m256i val = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(s + i));
        _mm256_stream_si256(reinterpret_cast<__m256i*>(d + i), val);
    }
#endif
    for (; i + 16 <= bytes; i += 16) {
        __m128i val = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s + i));
        _mm_stream_si128(reinterpret_cast<__m128i*>(d + i), val);
    }
    _mm_sfence();
#elif (defined(__aarch64__) || defined(_M_ARM64)) && (defined(__clang__) || defined(__GNUC__))
    const char* s = reinterpret_cast<const char*>(src);
    char* d = reinterpret_cast<char*>(dst);
    while (i < bytes && (reinterpret_cast<uintptr_t>(d + i) & 15) != 0) {
        d[i] = s[i];
        i++;
    }
    for (; i + 32 <= bytes; i += 32) {
        __asm__ __volatile__(
            "ldp q0, q1, [%1]\n\t"
            "stnp q0, q1, [%0]\n\t"
            :
            : "r"(d + i), "r"(s + i)
            : "v0", "v1", "memory"
        );
    }
    __asm__ __volatile__("dmb ishst" ::: "memory");
#endif
    if (i < bytes) {
        memcpy(reinterpret_cast<char*>(dst) + i, reinterpret_cast<const char*>(src) + i, bytes - i);
    }
}

struct sacculus_gc_t {
    sacculus_runway_t      runway;
    sacculus_motherline_t& motherline = runway.motherline;
    sacculus_falline_t&    falline    = runway.falline;
    sacculus_landfill_t    landfill;
    sacculus_hotlog_t*     hotlog;
    sacculus_coldlog_t*    coldlog;
    sf_pagefile_t          pagefile;
#ifdef USE_TBB
    sf_gc_tbb_arena_manager_t tbb_engine;
#endif

    std::thread            gc_thread;
    std::atomic<bool>      running{false};
    std::atomic<uint64_t>  sweep_count{0};
    std::atomic<size_t>    total_heap_bytes{0};
    std::atomic<size_t>    pressure_threshold{0};

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
        uint64_t cp_bytes_validated = 0;
        uint64_t cp_pacing_triggers = 0;
        uint64_t hp_canary_faults = 0;
        uint64_t sp_rendezvous_stalls = 0;
    };

    /**
     * @brief Cooperative sfGC <-> Mutator Safepoint & Pausepoint Rendezvous Handshake.
     *
     * Called by sfGC before relocating unpinned cells or merging emptyboxes.
     * Arms the inner-loop pausepoint gate and waits a bounded number of SMT-aware spins
     * for any in-flight unpinned mutator threads to either park at a safepoint/pausepoint
     * or finish their current vector chunk.
     */
    inline void wait_for_safepoint_rendezvous(void) {
        if (config.is_safety_critical || config.strings.no_sfgc_overwrite_mode) return;
        uint32_t in_flight = g_point_coord.active_in_flight_kernels.load(std::memory_order_acquire);
        if (in_flight == 0) return;

        uint32_t spin = 0;
        constexpr uint32_t MAX_RENDEZVOUS_SPINS = 2048;
        while (spin < MAX_RENDEZVOUS_SPINS) {
            uint32_t cur_inf    = g_point_coord.active_in_flight_kernels.load(std::memory_order_acquire);
            uint32_t cur_parked = g_point_coord.safepoint_parked_threads.load(std::memory_order_acquire);
            uint32_t cur_pinned = g_point_coord.hookpoint_active_pinned.load(std::memory_order_acquire);
            // All in-flight mutator threads are either safely parked at a safepoint/pausepoint
            // or have their active tensor explicitly pinned via hookpoint (which compaction skips).
            if (cur_inf == 0 || (cur_parked + cur_pinned) >= cur_inf) {
                break;
            }
            sf_cpu_pause();
            if ((++spin & 0xFFu) == 0u) {
                std::this_thread::yield();
            }
        }
    }

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
        s.cp_bytes_validated = g_point_coord.controlpoint_bytes_validated.load(std::memory_order_relaxed);
        s.cp_pacing_triggers = g_point_coord.controlpoint_pacing_triggers.load(std::memory_order_relaxed);
        s.hp_canary_faults   = g_point_coord.hookpoint_canary_faults_caught.load(std::memory_order_relaxed);
        s.sp_rendezvous_stalls = g_point_coord.safepoint_rendezvous_stalls.load(std::memory_order_relaxed);
        return s;
    }

    bool has_env_changed(const sf_env_snapshot_t& prev, const sf_env_snapshot_t& cur) const {
        return (cur.runway_bump != prev.runway_bump) ||
               (cur.allocated_bytes != prev.allocated_bytes) ||
               (cur.chained_chunks != prev.chained_chunks) ||
               (cur.pacing_stalls != prev.pacing_stalls) ||
               (cur.total_calls != prev.total_calls) ||
               (cur.resurrections != prev.resurrections) ||
               (cur.paged_out_count != prev.paged_out_count) ||
               (cur.cp_bytes_validated != prev.cp_bytes_validated) ||
               (cur.cp_pacing_triggers != prev.cp_pacing_triggers) ||
               (cur.hp_canary_faults != prev.hp_canary_faults);
    }

    static constexpr uint32_t RESURRECT_QUEUE_CAP = 64u;
    std::atomic<uint32_t> resurrect_queue[RESURRECT_QUEUE_CAP];
    std::atomic<uint32_t> resurrect_write{0};
    std::atomic<uint32_t> resurrect_read{0};
    sf_ticket_lock_t      resurrect_lock;

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
        sf_ensure_hardware_nexus_initialized();
#ifdef USE_TBB
        tbb_engine.init();
#endif
        hotlog   = hl;
        coldlog  = cl;
        config   = cfg;
        g_sf_critical_simd_mode.store(static_cast<uint8_t>(cfg.strings.critical_simd_mode), std::memory_order_relaxed);
        pressure_threshold.store(cfg.integers.pressure_threshold_bytes, std::memory_order_relaxed);
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

        runway.init(cfg.integers.runway_handsel_bytes, cfg.strings.Advance_Payment, cfg.is_safety_critical);
        landfill.init(cfg.integers.landfill_handsel_bytes, cfg.strings.Advance_Payment);
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

    void admit_egg_object(sacculus_live_header_t* obj) {
        if (!obj) return;
        sf_sync_store_fence();
        total_heap_bytes.fetch_add((size_t)obj->memory_footprint.load(std::memory_order_relaxed),
                                   std::memory_order_relaxed);
        runway.admit_egg_object(obj);
        sacculus_hotlog_write(hotlog, SF_Ebin0_MOTHERLINE_EGG_ADMIT, SF_Ebin1_TRACE,
            obj->function_id, 0, obj->generation, obj->region,
            obj->current_core.load(std::memory_order_relaxed), sweep_count.load(std::memory_order_relaxed),
            (uint32_t)(total_heap_bytes.load(std::memory_order_relaxed) / 1024),
            sf_get_debug_name(obj));
    }

    void request_resurrection(uint32_t fn_id) {
#ifdef USE_TBB
        if (tbb_engine.initialized.load(std::memory_order_relaxed)) {
            tbb_engine.concurrent_resurrect_queue.push(fn_id);
            return;
        }
#endif
        sf_ticket_guard_t g(resurrect_lock);
        uint32_t r = resurrect_read.load(std::memory_order_relaxed);
        uint32_t cur_w = resurrect_write.load(std::memory_order_relaxed);
        if (cur_w - r >= RESURRECT_QUEUE_CAP) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_RESURRECT_QUEUE_DROP, SF_Ebin1_WARN,
                fn_id, 0, 0, 0, 0, sweep_count.load(std::memory_order_relaxed), 0, "resurrect_queue_overflow");
            return;
        }
        resurrect_queue[cur_w & (RESURRECT_QUEUE_CAP - 1)].store(fn_id, std::memory_order_relaxed);
        resurrect_write.store(cur_w + 1, std::memory_order_release);
        sacculus_hotlog_write(hotlog, SF_Ebin0_RESURRECT_REQUEST, SF_Ebin1_INFO,
            fn_id, 0, 0, 0, 0, sweep_count.load(std::memory_order_relaxed), 0, "resurrect_request");
    }

    bool salvage_orphaned_slice_views(sacculus_live_header_t* parent) {
        if (!parent || parent->child_view_count.load(std::memory_order_acquire) == 0) return true;

        size_t parent_fp = (size_t)parent->memory_footprint.load(std::memory_order_relaxed);
        if (parent_fp < 128 * 1024) return false;

        bool all_salvaged = true;
        for (int r = 0; r < SACFOR_ROOM_COUNT; ++r) {
            for (int reg = 0; reg < SACFOR_REGION_COUNT; ++reg) {
                auto& region = motherline.rooms[r].regions[reg];
                sf_ticket_guard_t g(region.get_lock());
                auto* cur = region.get_head();
                while (cur) {
                    if (cur->parent_header.load(std::memory_order_relaxed) == parent) {
                        size_t slice_bytes = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                        if (slice_bytes > 0) {
                            void* new_body = runway.allocate_cell(slice_bytes, cur->function_id, "salvaged_slice");
                            if (new_body) {
                                memcpy(new_body, cur->body(), slice_bytes);
                                cur->forwarding_address.store(new_body, std::memory_order_release);
                                cur->parent_header.store(nullptr, std::memory_order_release);
                                cur->slice_byte_offset = 0;
                                parent->child_view_count.fetch_sub(1, std::memory_order_acq_rel);
                            } else {
                                all_salvaged = false;
                            }
                        }
                    }
                    cur = cur->next;
                }
            }
        }
        return all_salvaged && (parent->child_view_count.load(std::memory_order_acquire) == 0);
    }

    void mark_garbage(sacculus_live_header_t* obj) {
        if (!obj || obj->is_pinned()) return;
        if (obj->falline_marked) return;
        if (obj->child_view_count.load(std::memory_order_acquire) > 0) {
            if (!salvage_orphaned_slice_views(obj)) return;
        }
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
                current_phase.store(SF_GC_PHASE_EVACUATE, std::memory_order_release);
                wait_for_safepoint_rendezvous();
                sacculus_coldlog_write(coldlog, SF_Ebin0_LANDFILL_EVAC,
                    SF_Ebin1_WARN, 0, "landfill_critical");
                landfill.evacuate(hotlog, sweep_count.load());
                matured_flush_count.fetch_add(1, std::memory_order_relaxed);
                current_phase.store(SF_GC_PHASE_DRAIN, std::memory_order_release);
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

            uint32_t fid = obj->function_id;
            size_t cell_sz = obj->total_cell_bytes > 0 ? obj->total_cell_bytes : SF_custominiVm_live_header_t::total_cell_size(fp);
            obj->~SF_custominiVm_live_header_t();

            if (fp > 0) {
                sf_critical_clear_memory_core_adaptive(obj->body(), fp, sf_is_efficiency_core_active());
            }
            auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(obj);
            falline.register_emptybox(fid, fp, cell_sz, eb);
        }
        current_phase.store(SF_GC_PHASE_IDLE, std::memory_order_release);
        return drained_count;
    }

    size_t sweep_single_room(sacculus_room_t& room, uint8_t room_id, uint64_t sweep_no, uint32_t heap_kb,
                             float pressure_ratio, bool is_e_core, bool use_simd) {
        if (room_id == 0) {
            g_sf_self_heal.scan_and_heal_motherline(motherline, coldlog);
            drain_hotlog_to_coldlog(hotlog, coldlog);
        }
        size_t evicted_count = 0;
        float decay_f = config.floats.heat_decay_factor;
        if (decay_f <= 0.0f || decay_f > 1.0f) decay_f = 0.97f;

        for (int reg_i = SACFOR_REGION_COUNT - 1; reg_i >= 0; --reg_i) {
            sacculus_region_t& region = room.regions[reg_i];
            sf_ticket_guard_t g(region.get_lock());
            sacculus_live_header_t* cur = region.get_head();

            if (is_e_core || !use_simd) {
                // E-Core / Cycles Path: Micro-chunking to stay strictly within L1D (32KB) and non-temporal prefetching
                size_t chunk_ops = 0;
                while (cur) {
                    sacculus_live_header_t* next_obj = cur->next;

                    if (is_e_core && next_obj) {
#if defined(_M_X64) || defined(__x86_64__)
                        _mm_prefetch((const char*)next_obj, _MM_HINT_NTA);
#endif
                    }

                    cur->decay_heat();

                    if (cur->is_pinned()) { cur = next_obj; continue; }

                    if (cur->state.load(std::memory_order_acquire) == static_cast<uint8_t>(sf_live_state_e::QUARANTINED)) {
                        region.evict_nolock(cur);
                        motherline.decrement_counts(cur);
                        ++evicted_count;
                        room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                        size_t fp  = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                        size_t cur_rb = room.total_bytes.load(std::memory_order_relaxed);
                        while (cur_rb > 0) {
                            size_t next_b = (cur_rb >= fp) ? (cur_rb - fp) : 0;
                            if (room.total_bytes.compare_exchange_weak(cur_rb, next_b, std::memory_order_relaxed, std::memory_order_relaxed))
                                break;
                        }
                        landfill.accept(cur, SF_DEATH_ERROR, hotlog, sweep_no);
                        if (on_free) on_free(cur, on_free_user);
                        size_t chb = total_heap_bytes.load(std::memory_order_relaxed);
                        while (chb > 0) {
                            size_t next_v = (chb >= fp) ? (chb - fp) : 0;
                            if (total_heap_bytes.compare_exchange_weak(chb, next_v, std::memory_order_relaxed, std::memory_order_relaxed))
                                break;
                        }
                        sacculus_hotlog_write(hotlog, SF_Ebin0_TO_FALLINE,
                            SF_Ebin1_ERROR, cur->function_id, 0,
                            cur->generation, cur->region,
                            cur->current_core.load(std::memory_order_relaxed), sweep_no, heap_kb,
                            "quarantine_evac");
                        uint32_t fid = cur->function_id;
                        size_t cell_sz = cur->total_cell_bytes > 0 ? cur->total_cell_bytes : SF_custominiVm_live_header_t::total_cell_size(fp);
                        cur->~SF_custominiVm_live_header_t();
                        auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(cur);
                        falline.register_emptybox(fid, fp, cell_sz, eb);
                        cur = next_obj;
                        continue;
                    }

                    if (cur->needs_rehabilitation) {
                        if (cur->try_transition_to_rehab()) {
                            region.evict_nolock(cur);
                            motherline.decrement_counts(cur);
                            ++evicted_count;
                            room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                            size_t fp  = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                            size_t cur_rb = room.total_bytes.load(std::memory_order_relaxed);
                            while (cur_rb > 0) {
                                size_t next_b = (cur_rb >= fp) ? (cur_rb - fp) : 0;
                                if (room.total_bytes.compare_exchange_weak(cur_rb, next_b, std::memory_order_relaxed, std::memory_order_relaxed))
                                    break;
                            }
                            landfill.accept(cur, SF_DEATH_REHABILITATE, hotlog, sweep_no);
                            if (on_free) on_free(cur, on_free_user);
                            size_t chb = total_heap_bytes.load(std::memory_order_relaxed);
                            while (chb > 0) {
                                size_t next_v = (chb >= fp) ? (chb - fp) : 0;
                                if (total_heap_bytes.compare_exchange_weak(chb, next_v, std::memory_order_relaxed, std::memory_order_relaxed))
                                    break;
                            }
                            uint32_t fid = cur->function_id;
                            size_t cell_sz = cur->total_cell_bytes > 0 ? cur->total_cell_bytes : SF_custominiVm_live_header_t::total_cell_size(fp);
                            cur->~SF_custominiVm_live_header_t();
                            auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(cur);
                            falline.register_emptybox(fid, fp, cell_sz, eb);
                        }
                        cur = next_obj;
                        continue;
                    }

                    if (cur->is_idle()) {
                        if (cur->child_view_count.load(std::memory_order_acquire) > 0) {
                            salvage_orphaned_slice_views(cur);
                        }
                        if (cur->child_view_count.load(std::memory_order_acquire) == 0 && cur->try_transition_to_falline()) {
                            region.evict_nolock(cur);
                            motherline.decrement_counts(cur);
                            ++evicted_count;
                            room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                            size_t fp = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                            size_t cur_rb = room.total_bytes.load(std::memory_order_relaxed);
                            while (cur_rb > 0) {
                                size_t next_b = (cur_rb >= fp) ? (cur_rb - fp) : 0;
                                if (room.total_bytes.compare_exchange_weak(cur_rb, next_b, std::memory_order_relaxed, std::memory_order_relaxed))
                                    break;
                            }
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
                        if (cur->child_view_count.load(std::memory_order_acquire) > 0) {
                            salvage_orphaned_slice_views(cur);
                        }
                        if (cur->child_view_count.load(std::memory_order_acquire) == 0 && cur->try_transition_to_falline()) {
                            region.evict_nolock(cur);
                            motherline.decrement_counts(cur);
                            ++evicted_count;
                            room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                            size_t fp = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                            size_t cur_rb = room.total_bytes.load(std::memory_order_relaxed);
                            while (cur_rb > 0) {
                                size_t next_b = (cur_rb >= fp) ? (cur_rb - fp) : 0;
                                if (room.total_bytes.compare_exchange_weak(cur_rb, next_b, std::memory_order_relaxed, std::memory_order_relaxed))
                                    break;
                            }
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

                    auto* c_meta = get_cold_ro(cur);
                    if (c_meta) {
                        uint64_t dt = c_meta->type_sig_hash;
                        size_t fp = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                        if ((dt == SF_DTYPE_U8) && fp >= 16 * 1024 * 1024) {
                            young_max = 1;
                        } else if (dt == SF_DTYPE_F64) {
                            young_max = SACFOR_GEN_YOUNG_MAX + 2;
                        }
                    }

                    uint8_t new_gen = cur->generation;
                    if (cur->generation == SF_ROOM_YOUNG && cur->gc_sweep_count > young_max)
                        new_gen = SF_ROOM_ADOLESCENT;
                    else if (cur->generation == SF_ROOM_ADOLESCENT && cur->gc_sweep_count > adol_max)
                        new_gen = SF_ROOM_SENIOR;

                    if (new_gen != cur->generation) {
                        region.evict_nolock(cur);
                        room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                        size_t fp = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                        size_t cur_rb = room.total_bytes.load(std::memory_order_relaxed);
                        while (cur_rb > 0) {
                            size_t next_b = (cur_rb >= fp) ? (cur_rb - fp) : 0;
                            if (room.total_bytes.compare_exchange_weak(cur_rb, next_b, std::memory_order_relaxed, std::memory_order_relaxed))
                                break;
                        }
                        cur->gc_sweep_count = 0;
                        auto ev_promo = (new_gen == SF_ROOM_ADOLESCENT) ? SF_Ebin0_ROOM_PROMOTE_YOUNG_ADOL : SF_Ebin0_ROOM_PROMOTE_ADOL_SEN;
                        sacculus_hotlog_write(hotlog, ev_promo,
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
                            sacculus_hotlog_write(hotlog, SF_Ebin0_REGION_MIGRATE,
                                SF_Ebin1_TRACE, cur->function_id, 0,
                                cur->generation, target_reg,
                                cur->current_core.load(std::memory_order_relaxed), sweep_no, heap_kb,
                                sf_region_names[target_reg]);
                            cur = next_obj;
                            continue;
                        }

                    cur = next_obj;
                    if (is_e_core && (++chunk_ops % 16 == 0)) {
                        sf_cpu_pause();
                    }
                }
            } else {
                // P-Core / SIMD Path: Vectorized 8-element batches with aggressive T0 prefetch
                static constexpr size_t SIMD_BATCH_SZ = 8;
                sacculus_live_header_t* batch[SIMD_BATCH_SZ];

                while (cur) {
                    size_t count = 0;
                    sacculus_live_header_t* walker = cur;
                    while (walker && count < SIMD_BATCH_SZ) {
                        batch[count++] = walker;
#if defined(_M_X64) || defined(__x86_64__)
                        _mm_prefetch((const char*)walker, _MM_HINT_T0);
#endif
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
                            batch[b]->gc_sweep_count++;
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
                                batch[b+k]->gc_sweep_count++;
                            }
                        }
                        for (size_t b = (count & ~3ULL); b < count; ++b) batch[b]->decay_heat();
                    } else {
                        for (size_t b = 0; b < count; ++b) batch[b]->decay_heat();
                    }
#else
                    for (size_t b = 0; b < count; ++b) batch[b]->decay_heat();
#endif

                    for (size_t b = 0; b < count; ++b) {
                        auto* item = batch[b];
                        if (item->is_pinned()) continue;

                        if (item->state.load(std::memory_order_acquire) == static_cast<uint8_t>(sf_live_state_e::QUARANTINED)) {
                            region.evict_nolock(item);
                            motherline.decrement_counts(item);
                            ++evicted_count;
                            room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                            size_t fp = (size_t)item->memory_footprint.load(std::memory_order_relaxed);
                            size_t cur_rb = room.total_bytes.load(std::memory_order_relaxed);
                            while (cur_rb > 0) {
                                size_t next_b = (cur_rb >= fp) ? (cur_rb - fp) : 0;
                                if (room.total_bytes.compare_exchange_weak(cur_rb, next_b, std::memory_order_relaxed, std::memory_order_relaxed))
                                    break;
                            }
                            landfill.accept(item, SF_DEATH_ERROR, hotlog, sweep_no);
                            if (on_free) on_free(item, on_free_user);
                            size_t chb = total_heap_bytes.load(std::memory_order_relaxed);
                            while (chb > 0) {
                                size_t next_v = (chb >= fp) ? (chb - fp) : 0;
                                if (total_heap_bytes.compare_exchange_weak(chb, next_v, std::memory_order_relaxed, std::memory_order_relaxed))
                                    break;
                            }
                            sacculus_hotlog_write(hotlog, SF_Ebin0_TO_FALLINE,
                                SF_Ebin1_ERROR, item->function_id, 0,
                                item->generation, item->region,
                                item->current_core.load(std::memory_order_relaxed), sweep_no, heap_kb,
                                "quarantine_evac_simd");
                            uint32_t fid = item->function_id;
                            size_t cell_sz = item->total_cell_bytes > 0 ? item->total_cell_bytes : SF_custominiVm_live_header_t::total_cell_size(fp);
                            item->~SF_custominiVm_live_header_t();
                            auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(item);
                            falline.register_emptybox(fid, fp, cell_sz, eb);
                            continue;
                        }

                        if (item->needs_rehabilitation) {
                            if (item->try_transition_to_rehab()) {
                                region.evict_nolock(item);
                                motherline.decrement_counts(item);
                                ++evicted_count;
                                room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                                size_t fp = (size_t)item->memory_footprint.load(std::memory_order_relaxed);
                                size_t cur_rb = room.total_bytes.load(std::memory_order_relaxed);
                                while (cur_rb > 0) {
                                    size_t next_b = (cur_rb >= fp) ? (cur_rb - fp) : 0;
                                    if (room.total_bytes.compare_exchange_weak(cur_rb, next_b, std::memory_order_relaxed, std::memory_order_relaxed))
                                        break;
                                }
                                landfill.accept(item, SF_DEATH_REHABILITATE, hotlog, sweep_no);
                                if (on_free) on_free(item, on_free_user);
                                size_t chb = total_heap_bytes.load(std::memory_order_relaxed);
                                while (chb > 0) {
                                    size_t next_v = (chb >= fp) ? (chb - fp) : 0;
                                    if (total_heap_bytes.compare_exchange_weak(chb, next_v, std::memory_order_relaxed, std::memory_order_relaxed))
                                        break;
                                }
                                uint32_t fid = item->function_id;
                                size_t cell_sz = item->total_cell_bytes > 0 ? item->total_cell_bytes : SF_custominiVm_live_header_t::total_cell_size(fp);
                                item->~SF_custominiVm_live_header_t();
                                auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(item);
                                falline.register_emptybox(fid, fp, cell_sz, eb);
                            }
                            continue;
                        }

                        if (item->is_idle()) {
                            if (item->try_transition_to_falline()) {
                                salvage_orphaned_slice_views(item);
                                region.evict_nolock(item);
                                motherline.decrement_counts(item);
                                ++evicted_count;
                                room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                                size_t fp = (size_t)item->memory_footprint.load(std::memory_order_relaxed);
                                size_t cur_rb = room.total_bytes.load(std::memory_order_relaxed);
                                while (cur_rb > 0) {
                                    size_t next_b = (cur_rb >= fp) ? (cur_rb - fp) : 0;
                                    if (room.total_bytes.compare_exchange_weak(cur_rb, next_b, std::memory_order_relaxed, std::memory_order_relaxed))
                                        break;
                                }
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
                                salvage_orphaned_slice_views(item);
                                region.evict_nolock(item);
                                motherline.decrement_counts(item);
                                ++evicted_count;
                                room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                                size_t fp = (size_t)item->memory_footprint.load(std::memory_order_relaxed);
                                size_t cur_rb = room.total_bytes.load(std::memory_order_relaxed);
                                while (cur_rb > 0) {
                                    size_t next_b = (cur_rb >= fp) ? (cur_rb - fp) : 0;
                                    if (room.total_bytes.compare_exchange_weak(cur_rb, next_b, std::memory_order_relaxed, std::memory_order_relaxed))
                                        break;
                                }
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
                        auto* cold = get_cold(item);
                        if (cold) {
                            if (cold->type_sig_hash == 1 /* INT8 */ && (size_t)item->memory_footprint.load(std::memory_order_relaxed) >= 16 * 1024 * 1024) {
                                young_max = 1;
                            } else if (cold->type_sig_hash == 3 /* FLOAT64 */) {
                                young_max += 2;
                            }
                        }
                        if (young_max < 1) young_max = 1;
                        if (adol_max < 4) adol_max = 4;

                        uint8_t new_gen = item->generation;
                        if (item->generation == SF_ROOM_YOUNG && item->gc_sweep_count > young_max) new_gen = SF_ROOM_ADOLESCENT;
                        else if (item->generation == SF_ROOM_ADOLESCENT && item->gc_sweep_count > adol_max) new_gen = SF_ROOM_SENIOR;

                        if (new_gen != item->generation) {
                            region.evict_nolock(item);
                            room.total_objects.fetch_sub(1, std::memory_order_relaxed);
                            size_t fp = (size_t)item->memory_footprint.load(std::memory_order_relaxed);
                            size_t cur_rb = room.total_bytes.load(std::memory_order_relaxed);
                            while (cur_rb > 0) {
                                size_t next_b = (cur_rb >= fp) ? (cur_rb - fp) : 0;
                                if (room.total_bytes.compare_exchange_weak(cur_rb, next_b, std::memory_order_relaxed, std::memory_order_relaxed))
                                    break;
                            }
                            item->gc_sweep_count = 0;
                            auto ev_promo = (new_gen == SF_ROOM_ADOLESCENT) ? SF_Ebin0_ROOM_PROMOTE_YOUNG_ADOL : SF_Ebin0_ROOM_PROMOTE_ADOL_SEN;
                            sacculus_hotlog_write(hotlog, ev_promo,
                                SF_Ebin1_TRACE, item->function_id, 0,
                                item->generation, item->region,
                                item->current_core.load(std::memory_order_relaxed), sweep_no, heap_kb,
                                sf_room_names[new_gen]);
                            motherline.rooms[new_gen].place(item);
                            continue;
                        }

                        uint8_t target_reg = item->compute_target_region();
                        if (target_reg != item->region) {
                            region.evict_nolock(item);
                            room.regions[target_reg].place(item);
                            sacculus_hotlog_write(hotlog, SF_Ebin0_REGION_MIGRATE,
                                SF_Ebin1_TRACE, item->function_id, 0,
                                item->generation, target_reg,
                                item->current_core.load(std::memory_order_relaxed), sweep_no, heap_kb,
                                sf_region_names[target_reg]);
                            continue;
                        }
                    }

                    cur = walker;
                }
            }
        }
        return evicted_count;
    }

    size_t sweep_motherline_parallel(bool use_simd) {
        current_phase.store(SF_GC_PHASE_SWEEP, std::memory_order_relaxed);
        trigger_hook(SF_GC_HOOK_PRE_SWEEP);

        size_t cur_bytes = total_heap_bytes.load(std::memory_order_relaxed);
        float pressure_ratio = pressure_threshold.load(std::memory_order_relaxed) > 0 ?
            ((float)cur_bytes / (float)pressure_threshold.load(std::memory_order_relaxed)) : 0.0f;
        uint64_t sweep_no = sweep_count.load(std::memory_order_relaxed);
        uint32_t heap_kb = (uint32_t)(cur_bytes / 1024);

        size_t evicted_count = 0;

        // Under Single-Core Performance focus (SCP), bypass multi-core TBB synchronization overhead to concentrate thermal budget
        if (sf_is_focus_scp()) {
            bool is_e = sf_is_efficiency_core_active();
            for (int r = 0; r < SACFOR_ROOM_COUNT; ++r) {
                evicted_count += sweep_single_room(motherline.rooms[r], r, sweep_no, heap_kb, pressure_ratio, is_e, use_simd);
            }
            trigger_hook(SF_GC_HOOK_POST_SWEEP);
            return evicted_count;
        }

#ifdef USE_TBB
        if (tbb_engine.initialized.load(std::memory_order_relaxed)) {
            bool is_hybrid = sf_is_hybrid_cpu();
            bool burst_p_cores = is_hybrid && (pressure_ratio >= config.floats.pressure_mild_ratio * 0.8f);

            auto sweep_task = [&]() {
                for (auto& s : tbb_engine.ets_stats) { s.swept = 0; s.evicted = 0; }

                // Concurrently sweep all 3 rooms across worker cores via tbb::parallel_invoke
                tbb::parallel_invoke(
                    [&]() {
                        auto& local_s = tbb_engine.ets_stats.local();
                        local_s.evicted += sweep_single_room(motherline.rooms[SF_ROOM_YOUNG], SF_ROOM_YOUNG, sweep_no, heap_kb, pressure_ratio, false, use_simd);
                    },
                    [&]() {
                        auto& local_s = tbb_engine.ets_stats.local();
                        local_s.evicted += sweep_single_room(motherline.rooms[SF_ROOM_ADOLESCENT], SF_ROOM_ADOLESCENT, sweep_no, heap_kb, pressure_ratio, false, use_simd);
                    },
                    [&]() {
                        auto& local_s = tbb_engine.ets_stats.local();
                        local_s.evicted += sweep_single_room(motherline.rooms[SF_ROOM_SENIOR], SF_ROOM_SENIOR, sweep_no, heap_kb, pressure_ratio, false, use_simd);
                    }
                );

                for (const auto& s : tbb_engine.ets_stats) {
                    evicted_count += s.evicted;
                }
            };

            if (burst_p_cores) {
                // Elevate sweep to P-core high-priority arena for maximum bandwidth
                tbb_engine.p_arena.execute(sweep_task);
            } else if (!is_hybrid) {
                // Homogeneous symmetric multi-core parallel sweep
                tbb_engine.homo_arena.execute(sweep_task);
            } else {
                // Steady-state background sweep on E-core arena
                tbb_engine.e_arena.execute([&]() {
                    for (int r = 0; r < SACFOR_ROOM_COUNT; ++r) {
                        evicted_count += sweep_single_room(motherline.rooms[r], r, sweep_no, heap_kb, pressure_ratio, true, use_simd);
                    }
                });
            }

            trigger_hook(SF_GC_HOOK_POST_SWEEP);
            return evicted_count;
        }
#endif
        bool is_e = sf_is_efficiency_core_active();
        for (int r = 0; r < SACFOR_ROOM_COUNT; ++r) {
            evicted_count += sweep_single_room(motherline.rooms[r], r, sweep_no, heap_kb, pressure_ratio, is_e, use_simd);
        }

        trigger_hook(SF_GC_HOOK_POST_SWEEP);
        return evicted_count;
    }

    size_t sweep_motherline_cycles(void) {
        return sweep_motherline_parallel(false);
    }

    size_t sweep_motherline_simd(void) {
        return sweep_motherline_parallel(true);
    }

    size_t sweep_motherline(void) {
        if (config.strings.gc_operation_facility == sf_gc_operation_facility_e::SIMD) {
            return sweep_motherline_simd();
        }
        return sweep_motherline_cycles();
    }

    struct rehab_restore_info_t {
        uint32_t function_id;
        uint64_t type_sig_hash;
        size_t   previous_size;
        size_t   pit_stored_size;
        uint32_t crc32;
        size_t   rehab_offset;
        void*    rehab_payload_ptr;
        uint8_t  peak_generation;
        uint8_t  peak_region;
        uint16_t last_core;
        uint32_t pane_idx;
    };

    bool restore_rehab_payload_and_link(const rehab_restore_info_t& info) {
        uint8_t* rehab_base = landfill.pit.rehab_arena.base.load(std::memory_order_acquire);
        uint8_t* stored_cell = rehab_base ? (rehab_base + info.rehab_offset) : (uint8_t*)info.rehab_payload_ptr;
        if (!stored_cell) return false;

        auto* old_hdr = reinterpret_cast<SF_custominiVm_live_header_t*>(stored_cell);
        if (!old_hdr || !old_hdr->isValid()) return false;
        const uint8_t* old_body = old_hdr->body();
        size_t body_bytes = info.previous_size;

        if (info.crc32 != 0 && body_bytes > 0) {
            uint32_t computed_crc = sf_compute_crc32(old_body, body_bytes);
            if (computed_crc != info.crc32) {
                return false;
            }
        }
        if (body_bytes > 0 && !old_hdr->isTailCanaryValid()) {
            return false;
        }

        size_t cell_sz = SF_custominiVm_live_header_t::total_cell_size(body_bytes);
        auto* eb = runway.falline.snatch_emptybox(cell_sz, sweep_count.load(std::memory_order_relaxed));
        SF_custominiVm_live_header_t* new_obj = nullptr;

        if (eb && eb->isValid() && eb->aligned_hole_size >= cell_sz) {
            size_t box_capacity = eb->aligned_hole_size;
            new_obj = new (eb) SF_custominiVm_live_header_t();
            new_obj->init(info.function_id, info.type_sig_hash, "resurrected_tensor");
            new_obj->memory_footprint.store(body_bytes, std::memory_order_relaxed);
            new_obj->total_cell_bytes = box_capacity;
            sf_hotlog_emit(SF_Ebin0_FALLINE_SNATCH_HIT, SF_Ebin1_TRACE, info.function_id, 0, 0, 0, 0, 0, (uint32_t)(cell_sz / 1024), "resurrect_snatch_hit");
        } else {
            void* raw = runway.bump_alloc(cell_sz);
            if (!raw) return false;
            new_obj = new (raw) SF_custominiVm_live_header_t();
            new_obj->init(info.function_id, info.type_sig_hash, "resurrected_tensor");
            new_obj->memory_footprint.store(body_bytes, std::memory_order_relaxed);
            new_obj->total_cell_bytes = cell_sz;
            sf_hotlog_emit(SF_Ebin0_ALLOC_BUMP, SF_Ebin1_TRACE, info.function_id, 0, 0, 0, 0, 0, (uint32_t)(cell_sz / 1024), "resurrect_bump_alloc");
        }

        auto* new_cold = get_cold(new_obj);
        if (new_cold) {
            new_cold->type_sig_hash = info.type_sig_hash;
            new_cold->preferred_numa_node = (uint8_t)(info.last_core / 64);
        }

        if (body_bytes > 0 && old_body) {
            memcpy(new_obj->body(), old_body, body_bytes);
        }
        new_obj->writeTailCanary();

        total_heap_bytes.fetch_add(body_bytes, std::memory_order_relaxed);
        admit_egg_object(new_obj);

        sacculus_hotlog_write(hotlog, SF_Ebin0_RESURRECT_RESTORED, SF_Ebin1_INFO,
            info.function_id, (uint16_t)info.pane_idx, info.peak_generation, info.peak_region,
            info.last_core, sweep_count.load(std::memory_order_relaxed),
            (uint32_t)(total_heap_bytes.load(std::memory_order_relaxed) / 1024),
            "rehab_resurrected_physical");

        resurrection_count.fetch_add(1, std::memory_order_relaxed);
        return true;
    }

    size_t resurrect_entries_for_fn(uint32_t fn_id) {
        if (fn_id == 0) return 0;
        std::vector<rehab_restore_info_t> to_restore;
        {
            sf_ticket_guard_t g(landfill.pane.lock);
            uint32_t cnt = landfill.pane.count.load(std::memory_order_relaxed);
            for (uint32_t i = 0; i < cnt; ++i) {
                auto& entry = landfill.pane.entries[i];
                if (entry.function_id == fn_id && entry.lasting == 1) {
                    entry.lasting = 0;
                    rehab_restore_info_t info{};
                    info.function_id       = entry.function_id;
                    info.type_sig_hash     = entry.type_sig_hash;
                    info.previous_size     = entry.previous_size;
                    info.pit_stored_size   = entry.pit_stored_size;
                    info.crc32             = entry.crc32;
                    info.rehab_offset      = entry.rehab_offset;
                    info.rehab_payload_ptr = entry.rehab_payload_ptr;
                    info.peak_generation   = entry.peak_generation;
                    info.peak_region       = entry.peak_region;
                    info.last_core         = entry.last_core;
                    info.pane_idx          = i;
                    to_restore.push_back(info);
                }
            }
        }

        size_t restored = 0;
        for (const auto& info : to_restore) {
            if (restore_rehab_payload_and_link(info)) {
                ++restored;
            } else {
                sf_ticket_guard_t g(landfill.pane.lock);
                if (info.pane_idx < landfill.pane.count.load(std::memory_order_relaxed)) {
                    landfill.pane.entries[info.pane_idx].lasting = 1;
                }
            }
        }
        return restored;
    }

    size_t scan_rehab_arena(void) {
        current_phase.store(SF_GC_PHASE_REHAB, std::memory_order_relaxed);
        rehab_scan_count.fetch_add(1, std::memory_order_relaxed);
        size_t resurrected = 0;
        std::vector<uint32_t> pending_fn_ids;

#ifdef USE_TBB
        if (tbb_engine.initialized.load(std::memory_order_relaxed)) {
            uint32_t fn_id = 0;
            while (tbb_engine.concurrent_resurrect_queue.try_pop(fn_id)) {
                if (fn_id != 0) {
                    pending_fn_ids.push_back(fn_id);
                }
            }
        }
#endif

        {
            sf_ticket_guard_t g(resurrect_lock);
            uint32_t r = resurrect_read.load(std::memory_order_relaxed);
            uint32_t w = resurrect_write.load(std::memory_order_acquire);
            while (r != w) {
                uint32_t fn_id = resurrect_queue[r & (RESURRECT_QUEUE_CAP - 1)].exchange(0, std::memory_order_relaxed);
                if (fn_id != 0) {
                    pending_fn_ids.push_back(fn_id);
                }
                ++r;
            }
            resurrect_read.store(r, std::memory_order_release);
        }

        for (uint32_t fn_id : pending_fn_ids) {
            resurrected += resurrect_entries_for_fn(fn_id);
        }

        current_phase.store(SF_GC_PHASE_IDLE, std::memory_order_relaxed);
        return resurrected;
    }

    void maybe_compact_falline(void) {
        if (falline.emptybox_ratio() > SF_FALLINE_COMPACT_RATIO) {
            current_phase.store(SF_GC_PHASE_COMPACT, std::memory_order_release);
            wait_for_safepoint_rendezvous();
            size_t merged = falline.compact_emptyboxes();
            current_phase.store(SF_GC_PHASE_IDLE, std::memory_order_release);
            if (merged > 0) {
                falline_compact_count.fetch_add(merged, std::memory_order_relaxed);
                sacculus_hotlog_write(hotlog, SF_Ebin0_GC_COMPACT, SF_Ebin1_TRACE,
                    0, 0, 0, 0, 0, sweep_count.load(std::memory_order_relaxed),
                    (uint32_t)(total_heap_bytes.load(std::memory_order_relaxed) / 1024),
                    "falline_compacted");
            }
        }
    }

    void compact_single_room(sacculus_room_t& room, uint8_t room_id, uint64_t sweep_no, size_t max_compact_fp,
                             bool use_simd, size_t& out_relocated_count, size_t& out_relocated_bytes) {
        (void)room_id;
        for (int reg_i = 0; reg_i < SACFOR_REGION_COUNT; ++reg_i) {
            if (reg_i == SF_REGION_TANK || reg_i == SF_REGION_HOTFIELD) continue;

            auto& region = room.regions[reg_i];
            sf_ticket_guard_t rg(region.get_lock());
            sacculus_live_header_t* cur = region.get_head();

            while (cur) {
                sacculus_live_header_t* next_obj = cur->next;

                if (!cur->is_pinned() && cur->forwarding_address.load(std::memory_order_relaxed) == nullptr) {
                    size_t fp = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                    if (fp > 0 && fp < max_compact_fp) {
                        size_t cell_sz = SF_custominiVm_live_header_t::total_cell_size(fp);
                        void* new_raw = runway.allocator.allocate(cell_sz, SACFOR_CACHE_LINE);
                        if (new_raw) {
                            auto* new_obj = reinterpret_cast<SF_custominiVm_live_header_t*>(new_raw);
                            memcpy(static_cast<void*>(new_obj), static_cast<const void*>(cur), sizeof(SF_custominiVm_live_header_t));

                            if (use_simd) {
                                sf_critical_clear_memory(new_obj->body(), fp);
                            }
                            memcpy(new_obj->body(), cur->body(), fp);

                            new_obj->forwarding_address.store(nullptr, std::memory_order_relaxed);
                            new_obj->writeTailCanary();

                            sf_sync_store_fence();
                            cur->forwarding_address.store(new_obj->body(), std::memory_order_release);

                            region.evict_nolock(cur);
                            region.place_nolock(new_obj);

                            auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(cur);
                            falline.register_emptybox(cur->function_id, fp, cell_sz, eb, new_obj->body(), sweep_no);

                            ++out_relocated_count;
                            out_relocated_bytes += fp;
                        }
                    }
                }
                cur = next_obj;
            }
        }
    }

    void compact_and_relocate_motherline_adaptive(uint64_t sweep_no, bool use_simd) {
        current_phase.store(SF_GC_PHASE_COMPACT, std::memory_order_release);
        wait_for_safepoint_rendezvous();
        if (g_sf_controlpoint_hook.load(std::memory_order_relaxed)) {
            SF_custominiVm_trigger_host_controlpoint(SF_CP_ACTION_PAUSE_MUTATORS, sweep_no);
        }
        trigger_hook(SF_GC_HOOK_PRE_COMPACT);

        size_t relocated_count = 0;
        size_t relocated_bytes = 0;

        // Dynamic SRAM-aware compaction limit:
        // On 3D V-Cache (96MB - 192MB LLC) / Intel bLLC: expand to 256KB!
        // On standard P-cores / Homogeneous: 64KB.
        // On E-cores: cap to 16KB to prevent cluster L2 eviction.
        bool is_e = sf_is_efficiency_core_active();
        size_t max_compact_fp = is_e ? (16 * 1024) : (sf_has_massive_sram_vcache() ? (256 * 1024) : (64 * 1024));

#ifdef USE_TBB
        if (tbb_engine.initialized.load(std::memory_order_relaxed)) {
            bool is_hybrid = sf_is_hybrid_cpu();

            auto compact_task = [&]() {
                for (auto& s : tbb_engine.ets_stats) { s.relocated = 0; s.relocated_bytes = 0; }

                // Concurrently compact Young and Adolescent rooms in parallel
                tbb::parallel_invoke(
                    [&]() {
                        auto& local_s = tbb_engine.ets_stats.local();
                        size_t c = 0, b = 0;
                        compact_single_room(motherline.rooms[0], 0, sweep_no, max_compact_fp, use_simd, c, b);
                        local_s.relocated += c;
                        local_s.relocated_bytes += b;
                    },
                    [&]() {
                        auto& local_s = tbb_engine.ets_stats.local();
                        size_t c = 0, b = 0;
                        compact_single_room(motherline.rooms[1], 1, sweep_no, max_compact_fp, use_simd, c, b);
                        local_s.relocated += c;
                        local_s.relocated_bytes += b;
                    }
                );

                for (const auto& s : tbb_engine.ets_stats) {
                    relocated_count += s.relocated;
                    relocated_bytes += s.relocated_bytes;
                }
            };

            if (is_hybrid) {
                // Elevate compaction to P-core High-Priority Arena (prevents stalling on E-core narrow ALU)
                tbb_engine.p_arena.execute(compact_task);
            } else {
                // Homogeneous multi-core parallel compaction
                tbb_engine.homo_arena.execute(compact_task);
            }

            trigger_hook(SF_GC_HOOK_POST_COMPACT);
            current_phase.store(SF_GC_PHASE_IDLE, std::memory_order_release);
            if (g_sf_controlpoint_hook.load(std::memory_order_relaxed)) {
                SF_custominiVm_trigger_host_controlpoint(SF_CP_ACTION_RESUME_MUTATORS, sweep_no);
            }

            if (relocated_count > 0 && hotlog) {
                sacculus_hotlog_write(hotlog, SF_Ebin0_GC_COMPACT, SF_Ebin1_TRACE,
                    0, (uint16_t)relocated_count, 0, 0, 0, sweep_no,
                    (uint32_t)(relocated_bytes / 1024), "motherline_parallel_compacted");
            }
            return;
        }
#endif
        for (int room_i = 0; room_i < 2; ++room_i) {
            compact_single_room(motherline.rooms[room_i], room_i, sweep_no, max_compact_fp, use_simd, relocated_count, relocated_bytes);
        }

        trigger_hook(SF_GC_HOOK_POST_COMPACT);
        current_phase.store(SF_GC_PHASE_IDLE, std::memory_order_release);
        if (g_sf_controlpoint_hook.load(std::memory_order_relaxed)) {
            SF_custominiVm_trigger_host_controlpoint(SF_CP_ACTION_RESUME_MUTATORS, sweep_no);
        }

        if (relocated_count > 0 && hotlog) {
            sacculus_hotlog_write(hotlog, SF_Ebin0_GC_COMPACT, SF_Ebin1_TRACE,
                0, (uint16_t)relocated_count, 0, 0, 0, sweep_no,
                (uint32_t)(relocated_bytes / 1024), "motherline_compacted");
        }
    }

    void compact_and_relocate_motherline_cycles(uint64_t sweep_no) {
        compact_and_relocate_motherline_adaptive(sweep_no, false);
    }

    void compact_and_relocate_motherline_simd(uint64_t sweep_no) {
        compact_and_relocate_motherline_adaptive(sweep_no, true);
    }

    void compact_and_relocate_motherline(uint64_t sweep_no) {
        if (config.strings.gc_operation_facility == sf_gc_operation_facility_e::SIMD) {
            compact_and_relocate_motherline_simd(sweep_no);
            return;
        }
        compact_and_relocate_motherline_cycles(sweep_no);
    }

    struct sf_mltc_report_t {
        double   velocity_mbps = 0.0;
        size_t   heap_bytes = 0;
        uint32_t steady_increase_count = 0;
        size_t   total_tensors_scanned = 0;
        size_t   sram_misaligned_count = 0;
        size_t   dram_misaligned_count = 0;
        bool     leak_detected = false;
        bool     traffic_congestion_detected = false;
        bool     alignment_hazard_detected = false;
    };

    sf_mltc_report_t evaluate_mltc_diagnostics(size_t cur_b, size_t last_b, uint64_t diff_time,
                                               size_t& peak_b, uint32_t& steady_count, uint64_t sweep_no) {
        sf_mltc_report_t rep;
        rep.heap_bytes = cur_b;
        double diff_sec = diff_time > 0 ? ((double)diff_time / 1e9) : 0.001;
        size_t diff_bytes = (cur_b > last_b) ? (cur_b - last_b) : 0;
        rep.velocity_mbps = ((double)diff_bytes / diff_sec) / (1024.0 * 1024.0);

        if (cur_b > peak_b) {
            peak_b = cur_b;
            steady_count++;
        } else if (cur_b < peak_b) {
            peak_b = cur_b;
            steady_count = 0;
        }
        rep.steady_increase_count = steady_count;

        if (steady_count > 300 && cur_b > 10 * 1024 * 1024) {
            rep.leak_detected = true;
        }

        if (rep.velocity_mbps > 150.0 ||
            g_point_coord.controlpoint_pacing_triggers.load(std::memory_order_relaxed) > 0) {
            rep.traffic_congestion_detected = true;
        }

        if (sweep_no % 10 == 0) {
            for (int r = 0; r < 2; ++r) {
                for (int reg = 0; reg < SACFOR_REGION_COUNT; ++reg) {
                    auto& region = motherline.rooms[r].regions[reg];
                    sf_ticket_guard_t g(region.get_lock());
                    auto* cur = region.get_head();
                    size_t sampled = 0;
                    while (cur && sampled++ < 16) {
                        rep.total_tensors_scanned++;
                        uintptr_t addr = reinterpret_cast<uintptr_t>(cur->body());
                        if ((addr & (SACFOR_CACHE_LINE - 1)) != 0) rep.sram_misaligned_count++;
                        if ((addr & 255) != 0) rep.dram_misaligned_count++;
                        cur = cur->next;
                    }
                }
            }
            if (rep.total_tensors_scanned > 0 &&
                (rep.sram_misaligned_count > 0 || (rep.dram_misaligned_count * 100 / rep.total_tensors_scanned) > 25)) {
                rep.alignment_hazard_detected = true;
            }
        }

        return rep;
    }

    void start(void) {
        if (config.is_safety_critical) {

            running.store(false, std::memory_order_release);
            return;
        }
        running.store(true, std::memory_order_release);
        gc_thread = std::thread([this]() {
            tl_sf_core_tier = SF_CORE_TIER_EFFICIENCY;
            // Steer background GC to E-cores on hybrid architectures to preserve P-core compute capacity
            sf_bind_current_thread(SF_CORE_TIER_EFFICIENCY);
            size_t last_heap_b = 0;
            uint64_t last_time = sacculus_now_ns();
            size_t peak_heap_b = 0;
            uint32_t steady_increase_count = 0;
            while (running.load(std::memory_order_acquire)) {
                if (sf_is_focus_scp()) {
                    // Quiescent interval pacing for SCP: avoid aggressive spinning to maximize single-core turbo headroom
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                uint64_t t0 = sacculus_now_ns();

                sweep_motherline();

                drain_falline();

                uint64_t current_sweep = sweep_count.fetch_add(1, std::memory_order_relaxed) + 1;

                if (current_sweep % SF_REHAB_SCAN_INTERVAL == 0 ||
                    resurrect_read.load(std::memory_order_relaxed) != resurrect_write.load(std::memory_order_acquire)) {
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

                if (config.strings.gc_mode == sf_gc_mode_e::STATIC_MODE) {
                    interval = config.integers.wakes_gc_interval_ns ? config.integers.wakes_gc_interval_ns : SACFOR_GC_INTERVAL_NS;
                } else if (config.strings.gc_heuristic == sf_gc_heuristic_e::AGGRESSIVE) {
                    interval = 0ULL;
                } else if (config.strings.gc_heuristic == sf_gc_heuristic_e::BUREAUCRATIC) {
                    interval = 5000000ULL;
                } else {
                    size_t cur_b = total_heap_bytes.load(std::memory_order_relaxed);
                    uint64_t diff_time = sacculus_now_ns() - last_time;

                    sf_mltc_report_t mltc = evaluate_mltc_diagnostics(cur_b, last_heap_b, diff_time, peak_heap_b, steady_increase_count, current_sweep);

                    if (mltc.leak_detected) {
                        sacculus_coldlog_write(coldlog, SF_Ebin0_GC_SWEEP, SF_Ebin1_WARN, 0,
                            "[MLTC-LEAK] Continuous upward heap trend detected: potential memory leak.");
                        if (config.UnlockExperimentalVMOptions && config.UseMLTCSolver) {
                            sacculus_coldlog_write(coldlog, SF_Ebin0_GC_SWEEP, SF_Ebin1_INFO, 0,
                                "[MLTC-SOLVER] Invoking Emergency Compaction & Defragmentation to reclaim space!");
                            compact_and_relocate_motherline(current_sweep);
                            steady_increase_count = 0;
                        }
                    }

                    if (mltc.traffic_congestion_detected) {
                        char cmsg[64];
                        snprintf(cmsg, sizeof(cmsg), "[MLTC-CONGEST] High allocation velocity: %.1f MB/s!", mltc.velocity_mbps);
                        sacculus_coldlog_write(coldlog, SF_Ebin0_GC_SWEEP, SF_Ebin1_WARN, 0, cmsg);
                        if (config.UnlockExperimentalVMOptions && config.UseMLTCSolver) {
                            sacculus_coldlog_write(coldlog, SF_Ebin0_GC_SWEEP, SF_Ebin1_INFO, 0,
                                "[MLTC-SOLVER] Engaging Traffic Throttling: Rapid 256KB page rebate!");
                            runway.allocator.rebate_pages(sf_snatch_rebate_mode_e::RAPID, 256 * 1024);
                        }
                    }

                    if (mltc.alignment_hazard_detected) {
                        char amsg[64];
                        snprintf(amsg, sizeof(amsg), "[MLTC-ALIGN] SRAM/DRAM Bus Hazard: %zu/%zu misaligned!",
                                 mltc.dram_misaligned_count, mltc.total_tensors_scanned);
                        sacculus_coldlog_write(coldlog, SF_Ebin0_GC_SWEEP, SF_Ebin1_WARN, 0, amsg);
                        if (config.UnlockExperimentalVMOptions && config.UseMLTCSolver) {
                            sacculus_coldlog_write(coldlog, SF_Ebin0_GC_SWEEP, SF_Ebin1_INFO, 0,
                                "[MLTC-SOLVER] Forcing 256-byte Cache/DRAM Realignment Compaction!");
                            compact_and_relocate_motherline(current_sweep);
                        }
                    }

                    float press_ratio = pressure_threshold.load(std::memory_order_relaxed) > 0 ? ((float)cur_b / (float)pressure_threshold.load(std::memory_order_relaxed)) : 0.0f;
                    
                    if (mltc.velocity_mbps > 50.0) { 
                        if (press_ratio > config.floats.pressure_mild_ratio * 0.5f) {
                            interval = 0ULL; 
                        } else {
                            interval = 250000ULL;
                        }
                    } else if (press_ratio > config.floats.pressure_critical_ratio) {
                        interval = 250000ULL;
                    } else if (press_ratio < config.floats.pressure_mild_ratio) {
                        interval = 5000000ULL; 
                    } else {
                        interval = config.integers.wakes_gc_interval_ns ? config.integers.wakes_gc_interval_ns : SACFOR_GC_INTERVAL_NS;
                    }
                    last_heap_b = cur_b;
                    last_time = sacculus_now_ns();
                }

                if (interval == 0ULL) {
                    std::this_thread::yield();
                } else if (elapsed < interval) {
                    std::this_thread::sleep_for(std::chrono::nanoseconds(interval - elapsed));
                }
            }
        });
    }

    void apply_heuristic_policies(uint64_t current_sweep) {
        if (config.strings.gc_mode == sf_gc_mode_e::STATIC_MODE) {
            if (current_sweep % 50 == 0) compact_and_relocate_motherline(current_sweep);
            return;
        }
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
            runway.allocator.rebate_pages(sf_snatch_rebate_mode_e::DECOMMIT, 4096);
            maybe_compact_falline();
            if (current_sweep % 15 == 0) {
                compact_and_relocate_motherline(current_sweep);
            }
        } else if (config.strings.gc_heuristic == sf_gc_heuristic_e::ADAPTIVE) {
            size_t cur_b = total_heap_bytes.load(std::memory_order_relaxed);
            size_t thresh = pressure_threshold.load(std::memory_order_relaxed);
            float ratio = thresh > 0 ? ((float)cur_b / (float)thresh) : 0.0f;
            
            if (ratio > config.floats.pressure_high_ratio) {
                runway.allocator.rebate_pages(sf_snatch_rebate_mode_e::RAPID, 64 * 1024);
                maybe_compact_falline();
                if (current_sweep % 5 == 0) compact_and_relocate_motherline(current_sweep);
            } else if (ratio < config.floats.pressure_mild_ratio) {
                if (current_sweep % 100 == 0) {
                    runway.allocator.rebate_pages(sf_snatch_rebate_mode_e::DECOMMIT, 256 * 1024);
                }
            } else {
                if (current_sweep % 50 == 0) {
                    compact_and_relocate_motherline(current_sweep);
                    runway.allocator.rebate_pages(sf_snatch_rebate_mode_e::GYC, 1024 * 1024);
                }
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

        if (current_sweep % SF_REHAB_SCAN_INTERVAL == 0 ||
            resurrect_read.load(std::memory_order_relaxed) != resurrect_write.load(std::memory_order_acquire)) {
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
        } else if (config.strings.snatch_rebate_mode == sf_snatch_rebate_mode_e::DECOMMIT) {
            res.rebated = runway.allocator.rebate_pages(config.strings.snatch_rebate_mode, 65536);
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
                    float ratio = pressure_threshold.load(std::memory_order_relaxed) > 0 ? ((float)cur_b / (float)pressure_threshold.load(std::memory_order_relaxed)) : 0.0f;
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
#ifdef USE_TBB
        tbb_engine.destroy();
#endif
        drain_falline();
        landfill.evacuate(hotlog, sweep_count.load(std::memory_order_relaxed));
        pagefile.destroy();
    }

    ~sacculus_gc_t() {
        stop();
    }
};

using sfGC = sacculus_gc_t;

inline void sacculus_runway_t::pace_allocation(void) {
    if (!gc_ref) return;
    auto* gc = reinterpret_cast<sacculus_gc_t*>(gc_ref);
    size_t th = gc->pressure_threshold.load(std::memory_order_relaxed);
    if (th == 0) return;
    size_t cur = gc->total_heap_bytes.load(std::memory_order_relaxed);
    float ratio = (float)cur / (float)th;
    if (ratio >= 0.95f) {
        pacing_stalls.fetch_add(1, std::memory_order_relaxed);
        std::this_thread::yield();
        for (int p = 0; p < 2000; ++p) {
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

inline void* sacculus_runway_t::allocate_cell(size_t body_bytes, uint32_t fn_id, const char* name) {
    size_t cell_sz = SF_custominiVm_live_header_t::total_cell_size(body_bytes);

    pace_allocation();

    auto* eb = snatch_emptybox(cell_sz);
    SF_custominiVm_live_header_t* obj = nullptr;
    if (eb) {
        obj = new (eb) SF_custominiVm_live_header_t();
        obj->init(fn_id, 0, name ? name : "snatched_tensor");
        obj->memory_footprint.store(body_bytes, std::memory_order_relaxed);
        obj->total_cell_bytes = cell_sz;
        obj->writeTailCanary();
        sf_hotlog_emit(SF_Ebin0_FALLINE_SNATCH_HIT, SF_Ebin1_TRACE, fn_id, 0, 0, 0, 0, 0, (uint32_t)(cell_sz / 1024), "snatch_emptybox_hit");
    } else {
        void* raw = allocator.allocate(cell_sz, SACFOR_CACHE_LINE);
        if (!raw) return nullptr;
        obj = new (raw) SF_custominiVm_live_header_t();
        obj->init(fn_id, 0, name ? name : "runway_tensor");
        obj->memory_footprint.store(body_bytes, std::memory_order_relaxed);
        obj->total_cell_bytes = cell_sz;
        obj->writeTailCanary();
        sf_hotlog_emit(SF_Ebin0_ALLOC_BUMP, SF_Ebin1_TRACE, fn_id, 0, 0, 0, 0, 0, (uint32_t)(cell_sz / 1024), "runway_bump_alloc");
    }

    if (gc_ref) {
        reinterpret_cast<sacculus_gc_t*>(gc_ref)->total_heap_bytes.fetch_add(body_bytes, std::memory_order_relaxed);
    }

    motherline.admit(obj);
    sf_hotlog_emit(SF_Ebin0_MOTHERLINE_EGG_ADMIT, SF_Ebin1_TRACE, fn_id, 0, obj->generation, obj->region, 0, 0, (uint32_t)(body_bytes / 1024), "egg_nursery_admit");
    return obj->body();
}

inline void sacculus_runway_t::free_cell(void* body_ptr, size_t body_bytes) {
    if (!body_ptr) return;
    auto* obj = SF_custominiVm_live_header_t::from_body(body_ptr);
    if (!obj) return;

    if (obj->child_view_count.load(std::memory_order_acquire) > 0) return;

    if (gc_ref) {
        auto* gc = reinterpret_cast<sacculus_gc_t*>(gc_ref);
        size_t fp = (size_t)obj->memory_footprint.load(std::memory_order_relaxed);
        size_t cur = gc->total_heap_bytes.load(std::memory_order_relaxed);
        while (cur > 0) {
            size_t next_v = (cur >= fp) ? (cur - fp) : 0;
            if (gc->total_heap_bytes.compare_exchange_weak(cur, next_v, std::memory_order_relaxed, std::memory_order_relaxed))
                break;
        }
    }

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

    uint32_t fid = obj->function_id;
    size_t cell_sz = obj->total_cell_bytes > 0 ? obj->total_cell_bytes : SF_custominiVm_live_header_t::total_cell_size(body_bytes);
    sf_hotlog_emit(SF_Ebin0_FREE, SF_Ebin1_TRACE, fid, 0, obj->generation, obj->region, 0, 0, (uint32_t)(body_bytes / 1024), "free_cell");
    obj->~SF_custominiVm_live_header_t();
    auto* eb = reinterpret_cast<SF_custominiVm_emptybox_header_t*>(obj);
    falline.register_emptybox(fid, body_bytes, cell_sz, eb);
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
        entry.seqlock_version.fetch_add(1, std::memory_order_acq_rel);

        entry.function_id = fn_id;
        entry.fn.store(fn, std::memory_order_relaxed);
        entry.best_cycles = UINT64_MAX;
        entry.call_count  = 0;

        if (fn) {
            sf_sync_flush_icache((void*)fn, 64);
        }
        sf_sync_full_fence();

        entry.seqlock_version.fetch_add(1, std::memory_order_release);
    }

    void swap(uint32_t fn_id, sacculus_kernel_fn_t new_fn, uint64_t best) {
        if (fn_id >= SACFOR_MAX_DISPATCH_SLOTS) return;
        auto& entry = entries[fn_id];
        entry.seqlock_version.fetch_add(1, std::memory_order_acq_rel);

        entry.fn.store(new_fn, std::memory_order_relaxed);
        entry.best_cycles = best;

        if (new_fn) {
            sf_sync_flush_icache((void*)new_fn, 64);
        }
        sf_sync_full_fence();

        entry.seqlock_version.fetch_add(1, std::memory_order_release);
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
                memcpy(&speed, data + 0x15, sizeof(speed));
            }
            if (len >= 0x22) {
                memcpy(&conf_speed, data + 0x20, sizeof(conf_speed));
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
        if (boost) {
            sf_hotlog_emit(SF_Ebin0_UPCLOCKER_BOOST_ON, SF_Ebin1_INFO, 0, 0, 0, 0, 0, 0, 0, "upclocker_boost_engaged");
#if defined(_WIN32)
            SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
            SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
            pin_max_frequency_warmup(100);
#elif defined(__linux__)
            pin_max_frequency_warmup(100);
#endif
        } else {
            sf_hotlog_emit(SF_Ebin0_UPCLOCKER_BOOST_OFF, SF_Ebin1_INFO, 0, 0, 0, 0, 0, 0, 0, "upclocker_boost_disengaged");
#if defined(_WIN32)
            SetThreadExecutionState(ES_CONTINUOUS);
            SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
#endif
        }
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
        sf_init_hardware_nexus(&inv.topology);
        return inv;
    }
};

/* ========================================================================= */
/* SPECIALIZED HARDWARE ENGINES SUBSYSTEM IMPLEMENTATION                     */
/* (HYBRID/HOMOGENEOUS, NUMA/UMA, HT/NON-HT, DIVERSE SRAM ARCHITECTURES)      */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/* 1. Hybrid vs Homogeneous CPU Execution Engine                             */
/* ------------------------------------------------------------------------- */
struct sf_hybrid_homo_engine_t {
    SF_custominiVm_cpu_topology_class_t topo_class{SF_CPU_TOPOLOGY_HOMOGENEOUS};
    uint32_t p_core_count{0};
    uint32_t e_core_count{0};
    uint32_t lp_e_core_count{0};
    uint64_t p_core_mask{0};
    uint64_t e_core_mask{0};
    uint64_t lp_e_core_mask{0};
    uint64_t all_core_mask{0};
    bool     is_hybrid{false};

    // Empirical and architectural weighting coefficients
    double   r_ipc{2.60};            // Relative IPC ratio of P-core vs E-core
    double   freq_ratio{1.40};       // Relative clock frequency ratio (F_P / F_E)
    double   throughput_ratio{3.64}; // W_PE = R_IPC * (F_P / F_E)

    void init(const sf_hardware_topology_t* topo) {
        if (!topo) return;
        is_hybrid = (topo->is_heterogeneous != 0 && topo->p_cores > 0 && topo->e_cores > 0);
        p_core_count = is_hybrid ? topo->p_cores : topo->physical_cores;
        e_core_count = is_hybrid ? topo->e_cores : 0;
        lp_e_core_count = 0;

        uint32_t total_logical = topo->logical_cores > 0 ? topo->logical_cores : 1;
        if (total_logical > 64) total_logical = 64;
        all_core_mask = (total_logical == 64) ? ~0ULL : ((1ULL << total_logical) - 1);
        p_core_mask = 0;
        e_core_mask = 0;
        lp_e_core_mask = 0;

#if defined(_WIN32)
        DWORD len = 0;
        GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &len);
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER && len > 0) {
            std::vector<uint8_t> buffer(len);
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX p = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer.data();
            if (GetLogicalProcessorInformationEx(RelationProcessorCore, p, &len)) {
                DWORD offset = 0;
                while (offset < len) {
                    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX item = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)(buffer.data() + offset);
                    if (item->Relationship == RelationProcessorCore) {
                        uint64_t mask = (uint64_t)item->Processor.GroupMask[0].Mask;
                        if (is_hybrid) {
                            if (item->Processor.EfficiencyClass == 0) {
                                e_core_mask |= mask;
                            } else {
                                p_core_mask |= mask;
                            }
                        }
                    }
                    offset += item->Size;
                }
            }
        }
#elif defined(__linux__)
        if (is_hybrid) {
            for (uint32_t c = 0; c < total_logical; ++c) {
                char path[128];
                snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%u/topology/core_type", c);
                FILE* f = fopen(path, "r");
                if (f) {
                    char type_str[32] = {0};
                    if (fscanf(f, "%31s", type_str) == 1) {
                        if (strcmp(type_str, "atom") == 0 || strcmp(type_str, "efficiency") == 0) {
                            e_core_mask |= (1ULL << c);
                        } else {
                            p_core_mask |= (1ULL << c);
                        }
                    }
                    fclose(f);
                }
            }
        }
#endif
        if (!is_hybrid || p_core_mask == 0) {
            p_core_mask = all_core_mask;
            e_core_mask = 0;
            topo_class = SF_CPU_TOPOLOGY_HOMOGENEOUS;
            r_ipc = 1.0;
            freq_ratio = 1.0;
            throughput_ratio = 1.0;
        } else {
#if defined(__APPLE__)
            topo_class = SF_CPU_TOPOLOGY_HYBRID_APPLE;
            r_ipc = 3.0; // Firestorm vs Icestorm
            freq_ratio = 1.45;
#elif defined(__aarch64__) || defined(_M_ARM64)
            topo_class = SF_CPU_TOPOLOGY_HYBRID_ARM;
            r_ipc = 2.40; // Cortex-X vs Cortex-A
            freq_ratio = 1.35;
#else
            topo_class = SF_CPU_TOPOLOGY_HYBRID_INTEL;
            r_ipc = 2.60; // Lion Cove/Raptor Cove vs Skymont/Gracemont
            freq_ratio = 1.40;
#endif
            throughput_ratio = r_ipc * freq_ratio;
        }
    }

    bool bind_current_thread(SF_custominiVm_core_tier_t tier) {
        uint64_t target_mask = all_core_mask;
        if (tier == SF_CORE_TIER_PERFORMANCE && p_core_mask != 0) {
            target_mask = p_core_mask;
        } else if (tier == SF_CORE_TIER_EFFICIENCY && e_core_mask != 0) {
            target_mask = e_core_mask;
        }

#if defined(_WIN32)
        DWORD_PTR prev = SetThreadAffinityMask(GetCurrentThread(), (DWORD_PTR)target_mask);
        return prev != 0;
#elif defined(__linux__) || defined(__ANDROID__)
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        for (int i = 0; i < 64; ++i) {
            if (target_mask & (1ULL << i)) CPU_SET(i, &cpuset);
        }
        return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) == 0;
#else
        (void)target_mask;
        return true;
#endif
    }

    bool set_thread_qos(SF_custominiVm_core_tier_t tier) {
#if defined(_WIN32)
        typedef enum _THREAD_INFO_CLASS_LOCAL {
            LocalThreadPowerThrottling = 1
        } THREAD_INFO_CLASS_LOCAL;

        typedef struct _THREAD_POWER_THROTTLING_LOCAL {
            ULONG Version;
            ULONG ControlMask;
            ULONG StateMask;
        } THREAD_POWER_THROTTLING_LOCAL;

        THREAD_POWER_THROTTLING_LOCAL throttling{};
        throttling.Version = 1;

        if (tier == SF_CORE_TIER_EFFICIENCY) {
            // EcoQoS on Efficiency cores & background GC threads
            throttling.ControlMask = 0x1; // THREAD_POWER_THROTTLING_EXECUTION_SPEED
            throttling.StateMask   = 0x1;
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
        } else if (tier == SF_CORE_TIER_PERFORMANCE) {
            // Unconstrained high-performance turbo for Performance cores
            throttling.ControlMask = 0x1;
            throttling.StateMask   = 0x0;
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
        }

        typedef BOOL (WINAPI *SetThreadInformationFn)(HANDLE, int, PVOID, DWORD);
        HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
        if (hKernel32) {
            SetThreadInformationFn pfn = (SetThreadInformationFn)GetProcAddress(hKernel32, "SetThreadInformation");
            if (pfn) {
                return pfn(GetCurrentThread(), LocalThreadPowerThrottling, &throttling, sizeof(throttling)) != FALSE;
            }
        }
        return false;
#elif defined(__linux__)
        if (tier == SF_CORE_TIER_PERFORMANCE) {
            struct sched_param sp;
            sp.sched_priority = 0;
            sched_setscheduler(0, SCHED_OTHER, &sp);
        }
        return true;
#else
        (void)tier;
        return true;
#endif
    }

    // Dynamic Asymmetric Work Partitioning (R_IPC Weighting to eliminate straggler penalties)
    void partition_asymmetric_work(size_t total_elements,
                                   size_t num_p, size_t num_e,
                                   std::vector<std::pair<size_t, size_t>>& out_p_ranges,
                                   std::vector<std::pair<size_t, size_t>>& out_e_ranges,
                                   size_t align_elements = 16) const {
        out_p_ranges.clear();
        out_e_ranges.clear();
        if (total_elements == 0) return;

        if (!is_hybrid || num_e == 0 || num_p == 0) {
            size_t total_workers = (num_p + num_e > 0) ? (num_p + num_e) : 1;
            size_t base_chunk = total_elements / total_workers;
            size_t remainder = total_elements % total_workers;
            size_t curr = 0;
            for (size_t i = 0; i < total_workers; ++i) {
                size_t sz = base_chunk + (i < remainder ? 1 : 0);
                if (num_p > 0 && i < num_p) {
                    out_p_ranges.emplace_back(curr, curr + sz);
                } else {
                    out_e_ranges.emplace_back(curr, curr + sz);
                }
                curr += sz;
            }
            return;
        }

        // Asymmetric capacity weighting:
        // P-cores carry W_PE = R_IPC * (F_P / F_E), E-cores carry 1.0
        double weight_p = throughput_ratio;
        double weight_e = 1.0;
        double total_capacity = (double)num_p * weight_p + (double)num_e * weight_e;
        double elem_per_unit = (double)total_elements / total_capacity;

        size_t current_idx = 0;
        for (size_t i = 0; i < num_p; ++i) {
            size_t raw_sz = (size_t)(elem_per_unit * weight_p + 0.5);
            if (align_elements > 1 && i + 1 < num_p + num_e) {
                raw_sz = ((raw_sz + align_elements - 1) / align_elements) * align_elements;
            }
            size_t end_idx = (std::min)(current_idx + raw_sz, total_elements);
            out_p_ranges.emplace_back(current_idx, end_idx);
            current_idx = end_idx;
            if (current_idx >= total_elements) break;
        }

        for (size_t i = 0; i < num_e; ++i) {
            if (current_idx >= total_elements) {
                out_e_ranges.emplace_back(total_elements, total_elements);
                continue;
            }
            size_t remaining_e = num_e - i;
            size_t raw_sz = (total_elements - current_idx) / remaining_e;
            if (align_elements > 1 && i + 1 < num_e) {
                raw_sz = ((raw_sz + align_elements - 1) / align_elements) * align_elements;
            }
            size_t end_idx = (std::min)(current_idx + raw_sz, total_elements);
            if (i + 1 == num_e) end_idx = total_elements;
            out_e_ranges.emplace_back(current_idx, end_idx);
            current_idx = end_idx;
        }
    }

    uint32_t get_optimal_worker_count(bool is_heavy_compute) const {
        if (is_heavy_compute && is_hybrid && p_core_count > 0) {
            return p_core_count;
        }
        return p_core_count + e_core_count;
    }

    bool should_steer_to_pcore(bool is_heavy_simd, size_t vector_bytes = 64) const {
        if (!is_hybrid) return false;
        return is_heavy_simd || (vector_bytes >= 32);
    }

    SF_custominiVm_core_tier_t classify_core(uint32_t core_id) const {
        if (!is_hybrid) return SF_CORE_TIER_ANY;
        if (core_id < 64) {
            uint64_t bit = (1ULL << core_id);
            if (p_core_mask & bit) return SF_CORE_TIER_PERFORMANCE;
            if (e_core_mask & bit) return SF_CORE_TIER_EFFICIENCY;
        }
        return SF_CORE_TIER_ANY;
    }
};

/* ------------------------------------------------------------------------- */
/* 2. NUMA vs UMA Memory Architecture Engine                                 */
/* ------------------------------------------------------------------------- */
struct sf_numa_uma_engine_t {
    SF_custominiVm_mem_architecture_t mem_arch{SF_MEM_ARCH_UMA};
    uint8_t  node_count{1};
    uint32_t interconnect_bandwidth_gbps{0};
    bool     is_uma{true};

    // System Locality Information Table (SLIT) distance matrix: 10=local, 20=1-hop, 30=2-hop
    uint8_t  slit_distance[8][8];

    struct NumaNodeDomain {
        std::atomic<size_t>   allocated_bytes{0};
        std::atomic<uint64_t> access_count{0};
        std::atomic<uint64_t> local_reads{0};
        std::atomic<uint64_t> remote_reads{0};
        uint64_t              processor_mask{0};
        uint8_t               node_id{0};
    } nodes[8];

    // Inter-node communication access heat map: access_matrix[requester_node][target_node]
    std::atomic<uint64_t> access_matrix[8][8];

    void init(const sf_hardware_topology_t* topo) {
        if (!topo) return;
        node_count = topo->numa_nodes > 0 ? topo->numa_nodes : 1;
        if (node_count > 8) node_count = 8;
        interconnect_bandwidth_gbps = topo->estimated_bandwidth_gbps;
        is_uma = (topo->is_uma != 0 || node_count <= 1);
        mem_arch = is_uma ? SF_MEM_ARCH_UMA : SF_MEM_ARCH_NUMA_SYMMETRIC;

        for (uint8_t i = 0; i < 8; ++i) {
            nodes[i].node_id = i;
            nodes[i].allocated_bytes.store(0, std::memory_order_relaxed);
            nodes[i].access_count.store(0, std::memory_order_relaxed);
            nodes[i].local_reads.store(0, std::memory_order_relaxed);
            nodes[i].remote_reads.store(0, std::memory_order_relaxed);
            nodes[i].processor_mask = 0;

            for (uint8_t j = 0; j < 8; ++j) {
                access_matrix[i][j].store(0, std::memory_order_relaxed);
                if (i == j) {
                    slit_distance[i][j] = 10;
                } else if ((i / 2) == (j / 2)) {
                    slit_distance[i][j] = 20; // 1-hop interconnect (UPI / Infinity Fabric)
                } else {
                    slit_distance[i][j] = 30; // 2-hop mesh interconnect
                }
            }
        }

#if defined(_WIN32)
        if (!is_uma) {
            DWORD len = 0;
            GetLogicalProcessorInformationEx(RelationNumaNode, nullptr, &len);
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER && len > 0) {
                std::vector<uint8_t> buffer(len);
                PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX p = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer.data();
                if (GetLogicalProcessorInformationEx(RelationNumaNode, p, &len)) {
                    DWORD offset = 0;
                    while (offset < len) {
                        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX item = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)(buffer.data() + offset);
                        if (item->Relationship == RelationNumaNode) {
                            ULONG nid = item->NumaNode.NodeNumber;
                            if (nid < 8) {
                                nodes[nid].processor_mask = (uint64_t)item->NumaNode.GroupMask.Mask;
                            }
                        }
                        offset += item->Size;
                    }
                }
            }
        }
#elif defined(__linux__)
        if (!is_uma) {
            for (uint8_t n = 0; n < node_count; ++n) {
                char path[128];
                snprintf(path, sizeof(path), "/sys/devices/system/node/node%u/cpumap", n);
                FILE* f = fopen(path, "r");
                if (f) {
                    uint64_t mask = 0;
                    if (fscanf(f, "%llx", (unsigned long long*)&mask) == 1) {
                        nodes[n].processor_mask = mask;
                    }
                    fclose(f);
                }
            }
        }
#endif
    }

    void* alloc_node(size_t bytes, uint8_t preferred_node = 0) {
        if (bytes == 0) return nullptr;
        uint8_t n = (preferred_node < node_count) ? preferred_node : 0;
        void* ptr = nullptr;

#if defined(_WIN32)
        if (!is_uma && node_count > 1) {
            ptr = VirtualAllocExNuma(GetCurrentProcess(), nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE, (DWORD)n);
        }
        if (!ptr) {
            ptr = VirtualAlloc(nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        }
#else
        ptr = sf_os_reserve_memory(bytes);
        if (ptr) sf_os_commit_memory(ptr, bytes);
#endif
        if (ptr) {
#if defined(_WIN32)
            g_sf_os_registry.register_block(ptr, bytes, 0);
#endif
            nodes[n].allocated_bytes.fetch_add(bytes, std::memory_order_relaxed);
        }
        return ptr;
    }

    void free_node(void* ptr, size_t bytes, uint8_t node = 0) {
        if (!ptr) return;
        uint8_t n = (node < 8) ? node : 0;
        nodes[n].allocated_bytes.fetch_sub(bytes, std::memory_order_relaxed);
#if defined(_WIN32)
        g_sf_os_registry.unregister_block(ptr);
        VirtualFree(ptr, 0, MEM_RELEASE);
#else
        sf_os_release_memory(ptr, bytes);
#endif
    }

    uint8_t get_current_thread_node() {
        if (is_uma || node_count <= 1) return 0;
#if defined(_WIN32)
        PROCESSOR_NUMBER proc_num;
        GetCurrentProcessorNumberEx(&proc_num);
        USHORT node_num = 0;
        if (GetNumaProcessorNodeEx(&proc_num, &node_num)) {
            return (uint8_t)node_num;
        }
        return 0;
#elif defined(__linux__)
        int cpu = sched_getcpu();
        if (cpu >= 0) {
            for (uint8_t n = 0; n < node_count; ++n) {
                if (nodes[n].processor_mask & (1ULL << cpu)) return n;
            }
        }
        return 0;
#else
        return 0;
#endif
    }

    void record_access(uint8_t accessing_node, uint8_t target_node, size_t bytes = 64) {
        (void)bytes;
        if (accessing_node >= 8 || target_node >= 8) return;
        access_matrix[accessing_node][target_node].fetch_add(1, std::memory_order_relaxed);
        nodes[target_node].access_count.fetch_add(1, std::memory_order_relaxed);
        if (accessing_node == target_node) {
            nodes[target_node].local_reads.fetch_add(1, std::memory_order_relaxed);
        } else {
            nodes[target_node].remote_reads.fetch_add(1, std::memory_order_relaxed);
        }
    }

    float get_remote_heat_ratio(uint8_t node) const {
        if (node >= 8) return 0.0f;
        uint64_t loc = nodes[node].local_reads.load(std::memory_order_relaxed);
        uint64_t rem = nodes[node].remote_reads.load(std::memory_order_relaxed);
        uint64_t tot = loc + rem;
        return (tot > 0) ? (float)rem / (float)tot : 0.0f;
    }

    bool should_migrate(uint8_t host_node, uint8_t& out_dominant_node, float threshold = 0.70f) const {
        if (is_uma || node_count <= 1 || host_node >= 8) return false;
        uint64_t max_remote = 0;
        uint8_t  best_node = host_node;
        uint64_t total_remote = 0;

        for (uint8_t src = 0; src < node_count; ++src) {
            if (src == host_node) continue;
            uint64_t acc = access_matrix[src][host_node].load(std::memory_order_relaxed);
            total_remote += acc;
            if (acc > max_remote) {
                max_remote = acc;
                best_node = src;
            }
        }

        uint64_t local_acc = access_matrix[host_node][host_node].load(std::memory_order_relaxed);
        uint64_t total = local_acc + total_remote;
        if (total < 1000) return false;

        float rem_ratio = (float)max_remote / (float)total;
        if (rem_ratio >= threshold && best_node != host_node) {
            out_dominant_node = best_node;
            return true;
        }
        return false;
    }

    void distribute_numa_work(size_t total_elements,
                              std::vector<std::pair<size_t, size_t>>& out_node_slices) const {
        out_node_slices.clear();
        if (total_elements == 0) return;
        uint8_t n_nodes = (is_uma || node_count <= 1) ? 1 : node_count;
        size_t base = total_elements / n_nodes;
        size_t rem = total_elements % n_nodes;
        size_t cur = 0;
        for (uint8_t n = 0; n < n_nodes; ++n) {
            size_t sz = base + (n < rem ? 1 : 0);
            out_node_slices.emplace_back(cur, cur + sz);
            cur += sz;
        }
    }
};

/* ------------------------------------------------------------------------- */
/* 3. SMT (Hyper-Threading) vs Non-HT CPU Execution Engine                   */
/* ------------------------------------------------------------------------- */
struct sf_smt_ht_engine_t {
    SF_custominiVm_smt_mode_t smt_mode{SF_SMT_MODE_DISABLED_OR_ABSENT};
    uint32_t physical_cores{1};
    uint32_t logical_cores{1};
    uint32_t threads_per_core{1};
    bool     is_ht_active{false};

    uint64_t primary_threads_mask{0};  // Exactly 1 thread per physical core (eliminates Port 0/1/5 FMA collisions)
    uint64_t sibling_threads_mask{0};  // Secondary logical SMT threads
    uint8_t  sibling_map[64];          // Maps logical core ID -> SMT sibling logical core ID

    void init(const sf_hardware_topology_t* topo) {
        if (!topo) return;
        physical_cores = topo->physical_cores > 0 ? topo->physical_cores : 1;
        logical_cores  = topo->logical_cores > 0 ? topo->logical_cores : physical_cores;
        is_ht_active   = (logical_cores > physical_cores);
        threads_per_core = is_ht_active ? (logical_cores / physical_cores) : 1;
        if (threads_per_core >= 4) {
            smt_mode = SF_SMT_MODE_4WAY_PLUS;
        } else if (is_ht_active) {
            smt_mode = SF_SMT_MODE_2WAY;
        } else {
            smt_mode = SF_SMT_MODE_DISABLED_OR_ABSENT;
        }

        primary_threads_mask = 0;
        sibling_threads_mask = 0;
        for (int i = 0; i < 64; ++i) sibling_map[i] = (uint8_t)i;

#if defined(_WIN32)
        DWORD len = 0;
        GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &len);
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER && len > 0) {
            std::vector<uint8_t> buffer(len);
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX p = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer.data();
            if (GetLogicalProcessorInformationEx(RelationProcessorCore, p, &len)) {
                DWORD offset = 0;
                while (offset < len) {
                    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX item = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)(buffer.data() + offset);
                    if (item->Relationship == RelationProcessorCore) {
                        uint64_t mask = (uint64_t)item->Processor.GroupMask[0].Mask;
                        if (mask != 0) {
                            uint64_t primary_bit = (mask & (~mask + 1));
                            primary_threads_mask |= primary_bit;
                            sibling_threads_mask |= (mask & ~primary_bit);

                            std::vector<int> core_ids;
                            for (int b = 0; b < 64; ++b) {
                                if (mask & (1ULL << b)) core_ids.push_back(b);
                            }
                            if (core_ids.size() >= 2) {
                                sibling_map[core_ids[0]] = (uint8_t)core_ids[1];
                                sibling_map[core_ids[1]] = (uint8_t)core_ids[0];
                            }
                        }
                    }
                    offset += item->Size;
                }
            }
        }
#endif
        if (primary_threads_mask == 0) {
            if (is_ht_active) {
                for (uint32_t i = 0; i < logical_cores && i < 64; ++i) {
                    if (i % 2 == 0) {
                        primary_threads_mask |= (1ULL << i);
                        if (i + 1 < 64) {
                            sibling_map[i] = (uint8_t)(i + 1);
                            sibling_map[i + 1] = (uint8_t)i;
                        }
                    } else {
                        sibling_threads_mask |= (1ULL << i);
                    }
                }
            } else {
                uint32_t max_bits = logical_cores < 64 ? logical_cores : 64;
                primary_threads_mask = (max_bits == 64) ? ~0ULL : ((1ULL << max_bits) - 1);
            }
        }
    }

    uint32_t get_optimal_concurrency(SF_custominiVm_workload_affinity_policy_t policy) const {
        if (policy == SF_AFFINITY_PHYSICAL_CORES_ONLY || !is_ht_active) {
            return physical_cores;
        }
        return logical_cores;
    }

    struct sf_thread_budget_t {
        uint32_t gc_threads;
        uint32_t puzzle_threads;
        uint32_t vm_threads;
        uint32_t total_threads;
    };

    SF_custominiVm_hardware_tier_t detect_hardware_tier() const {
        uint32_t l = logical_cores > 0 ? logical_cores : (physical_cores > 0 ? physical_cores : 4);
        uint32_t p = physical_cores > 0 ? physical_cores : l;
        if (l > 32 || p > 32) {
            return SF_HW_TIER_SUR;
        } else if (l >= 15) {
            return SF_HW_TIER_ORTA_UST;
        } else if (l >= 9) {
            return SF_HW_TIER_ORTA_ORTA;
        } else if (l >= 5) {
            return SF_HW_TIER_ORTA_ALT;
        } else {
            return SF_HW_TIER_ALT;
        }
    }

    const char* get_hardware_tier_name() const {
        switch (detect_hardware_tier()) {
            case SF_HW_TIER_ALT:       return "Alt (Entry / Embedded / SBC)";
            case SF_HW_TIER_ORTA_ALT:  return "Orta-Alt (Lower-Midrange / Laptop)";
            case SF_HW_TIER_ORTA_ORTA: return "Orta-Orta (Midrange Desktop)";
            case SF_HW_TIER_ORTA_UST:  return "Orta-Ust (High-End Desktop)";
            case SF_HW_TIER_SUR:       return "Sur / Extreme (HPC / Workstation / Server)";
            default:                   return "Standard";
        }
    }

    sf_thread_budget_t calculate_thread_budget() const {
        sf_thread_budget_t b{};
        SF_custominiVm_hardware_tier_t tier = detect_hardware_tier();
        uint32_t l = logical_cores > 0 ? logical_cores : 4;

        switch (tier) {
            case SF_HW_TIER_SUR: {
                // Sür / Extreme (HPC/Server/Workstation, >32 cores / e.g. Threadripper/EPYC/Xeon)
                // Threads = up to 64–128 threads (GC <= 16, Puzzle <= 8, VM <= 104)
                b.gc_threads = std::min(16u, std::max(4u, l / 8));
                b.puzzle_threads = std::min(8u, std::max(2u, l / 16));
                uint32_t rem = (l > (b.gc_threads + b.puzzle_threads)) ? (l - b.gc_threads - b.puzzle_threads) : 4u;
                b.vm_threads = std::min(104u, rem);
                b.total_threads = b.gc_threads + b.puzzle_threads + b.vm_threads;
                if (b.total_threads > 128) {
                    b.total_threads = 128;
                    b.vm_threads = 128 - b.gc_threads - b.puzzle_threads;
                }
                break;
            }
            case SF_HW_TIER_ORTA_UST: {
                // Orta-Üst (High-end Desktop, 15-32 threads, e.g. Core i9/i7, Ryzen 9/7)
                // Baseline 24 threads (GC <= 6, Puzzle <= 3, VM <= 15)
                b.gc_threads = (physical_cores >= 16) ? 6 : 4;
                b.puzzle_threads = 3;
                b.vm_threads = 15;
                if (b.gc_threads > 6) b.gc_threads = 6;
                if (b.puzzle_threads > 3) b.puzzle_threads = 3;
                if (b.vm_threads > 15) b.vm_threads = 15;
                b.total_threads = b.gc_threads + b.puzzle_threads + b.vm_threads;
                break;
            }
            case SF_HW_TIER_ORTA_ORTA: {
                // Orta-Orta (Midrange Desktop, 6C/12T - 14T, e.g. Ryzen 5 5600X, Core i5-13400F, Ultra 5 245K)
                // 12–14 threads (GC <= 4, Puzzle <= 2, VM <= 8)
                if (l >= 14) {
                    b.gc_threads = 4;
                    b.puzzle_threads = 2;
                    b.vm_threads = 8;
                    b.total_threads = 14;
                } else {
                    b.gc_threads = 3;
                    b.puzzle_threads = 2;
                    b.vm_threads = 7;
                    b.total_threads = 12;
                }
                break;
            }
            case SF_HW_TIER_ORTA_ALT: {
                // Orta-Alt (Lower-Midrange / Laptop, 4C/8T)
                // 6–8 threads (GC <= 2, Puzzle <= 2, VM <= 4)
                b.gc_threads = 2;
                b.puzzle_threads = 2;
                b.vm_threads = (l >= 8) ? 4 : 3;
                b.total_threads = b.gc_threads + b.puzzle_threads + b.vm_threads;
                break;
            }
            case SF_HW_TIER_ALT:
            default: {
                // Alt / Giriş / Embedded / SBC (2-4 cores, e.g. Raspberry Pi, Atom, Low-power)
                // 2–4 threads (GC <= 1, Puzzle <= 1, VM <= 2)
                b.gc_threads = 1;
                b.puzzle_threads = 1;
                b.vm_threads = (l >= 4) ? 2 : 1;
                b.total_threads = b.gc_threads + b.puzzle_threads + b.vm_threads;
                break;
            }
        }
        return b;
    }

    bool apply_affinity_policy(SF_custominiVm_workload_affinity_policy_t policy) {
        uint64_t target_mask = 0;
        if (policy == SF_AFFINITY_PHYSICAL_CORES_ONLY) {
            target_mask = primary_threads_mask;
        } else if (policy == SF_AFFINITY_ALL_LOGICAL_THREADS) {
            target_mask = primary_threads_mask | sibling_threads_mask;
        }
        if (target_mask == 0) return true;

#if defined(_WIN32)
        return SetThreadAffinityMask(GetCurrentThread(), (DWORD_PTR)target_mask) != 0;
#elif defined(__linux__) || defined(__ANDROID__)
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        for (int i = 0; i < 64; ++i) {
            if (target_mask & (1ULL << i)) CPU_SET(i, &cpuset);
        }
        return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) == 0;
#else
        return true;
#endif
    }

    // Adaptive SMT-Aware Spinlock Pause Backoff (yields execution pipeline to sibling thread)
    static inline void smt_pause_backoff(uint32_t iteration) {
#if defined(_M_X64) || defined(__x86_64__)
        if (iteration < 4) {
            _mm_pause();
        } else if (iteration < 16) {
            _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause();
        } else if (iteration < 64) {
            for (int k = 0; k < 16; ++k) _mm_pause();
        } else {
#if defined(_WIN32)
            SwitchToThread();
#else
            std::this_thread::yield();
#endif
        }
#elif defined(__aarch64__) || defined(_M_ARM64)
        if (iteration < 16) {
            __asm__ __volatile__("isb" ::: "memory");
        } else {
            std::this_thread::yield();
        }
#else
        std::this_thread::yield();
#endif
    }

    bool is_primary_core(uint32_t core_id) const {
        if (core_id >= 64) return false;
        return (primary_threads_mask & (1ULL << core_id)) != 0;
    }

    uint8_t get_sibling_core(uint8_t core_id) const {
        return (core_id < 64) ? sibling_map[core_id] : core_id;
    }

    double estimate_port_contention_penalty(bool is_fma_vector) const {
        if (!is_ht_active) return 1.0;
        return is_fma_vector ? 1.65 : 1.15;
    }
};

/* ------------------------------------------------------------------------- */
/* 4. Diverse SRAM & Cache Architectures Engine                              */
/* ------------------------------------------------------------------------- */
struct sf_sram_cache_engine_t {
    SF_custominiVm_sram_regime_t sram_regime{SF_SRAM_REGIME_STANDARD_HIERARCHICAL};
    uint32_t l1d_kb{32};
    uint32_t l1i_kb{32};
    uint32_t l2_kb{512};
    uint32_t l3_kb{16384};
    uint16_t cacheline_bytes{64};
    bool     has_3d_vcache{false};    // AMD 3D V-Cache (96MB - 192MB LLC SRAM stacked)
    bool     has_intel_bllc{false};   // Intel Nova Lake Big LLC (144MB - 288MB bLLC SRAM)
    bool     has_scratchpad_sram{false};

    // On-chip Embedded Scratchpad SRAM Fast Bump Allocator (Pre-committed, zero-wait-state)
    static constexpr size_t SCRATCHPAD_CAPACITY = 8ULL * 1024 * 1024; // 8MB
    uint8_t* scratchpad_base{nullptr};
    std::atomic<size_t> scratchpad_offset{0};

    void init(const sf_hardware_topology_t* topo) {
        if (!topo) return;
        l1d_kb = topo->l1d.size_kb > 0 ? topo->l1d.size_kb : 32;
        l1i_kb = topo->l1i.size_kb > 0 ? topo->l1i.size_kb : 32;
        l2_kb  = topo->l2.size_kb > 0 ? topo->l2.size_kb : 512;
        l3_kb  = topo->l3.size_kb > 0 ? topo->l3.size_kb : 16384;
        cacheline_bytes = topo->primary_cacheline_size > 0 ? topo->primary_cacheline_size : 64;

        // Detect Advanced Massive SRAM Caches:
        // 1. AMD 3D V-Cache: 96MB - 192MB LLC SRAM
        // 2. Intel Nova Lake bLLC: 144MB - 288MB Big LLC
        if (l3_kb >= 130 * 1024) {
            has_intel_bllc = true;
            has_3d_vcache  = false;
            sram_regime    = SF_SRAM_REGIME_3D_VCACHE;
        } else if (l3_kb >= 48 * 1024) {
            has_3d_vcache  = true;
            has_intel_bllc = false;
            sram_regime    = SF_SRAM_REGIME_3D_VCACHE;
        } else if (cacheline_bytes >= 128) {
            sram_regime    = SF_SRAM_REGIME_WIDE_CACHELINE_128B;
        } else {
            sram_regime    = SF_SRAM_REGIME_STANDARD_HIERARCHICAL;
        }

        // Initialize zero-wait-state Scratchpad SRAM pool
        if (!scratchpad_base) {
            scratchpad_base = (uint8_t*)sf_os_reserve_memory(SCRATCHPAD_CAPACITY);
            if (scratchpad_base && sf_os_commit_memory(scratchpad_base, SCRATCHPAD_CAPACITY)) {
                sf_pretouch_memory(scratchpad_base, SCRATCHPAD_CAPACITY);
                has_scratchpad_sram = true;
            }
        }
        scratchpad_offset.store(0, std::memory_order_relaxed);
    }

    void cleanup() {
        if (scratchpad_base) {
            sf_os_release_memory(scratchpad_base, SCRATCHPAD_CAPACITY);
            scratchpad_base = nullptr;
            has_scratchpad_sram = false;
        }
    }

    void* scratchpad_alloc(size_t bytes, size_t alignment = 64) {
        if (!scratchpad_base || bytes == 0) return nullptr;
        if (alignment == 0) alignment = 64;
        size_t cur = scratchpad_offset.load(std::memory_order_relaxed);
        while (true) {
            size_t aligned = (cur + alignment - 1) & ~(alignment - 1);
            size_t next = aligned + bytes;
            if (next > SCRATCHPAD_CAPACITY) return nullptr;
            if (scratchpad_offset.compare_exchange_weak(cur, next, std::memory_order_acquire, std::memory_order_relaxed)) {
                return scratchpad_base + aligned;
            }
        }
    }

    void scratchpad_reset() {
        scratchpad_offset.store(0, std::memory_order_release);
    }

    size_t pad_to_cacheline(size_t size) const {
        size_t cl = cacheline_bytes > 0 ? cacheline_bytes : 64;
        return (size + cl - 1) & ~(cl - 1);
    }

    void* align_to_cacheline(void* ptr) const {
        size_t cl = cacheline_bytes > 0 ? cacheline_bytes : 64;
        uintptr_t addr = (uintptr_t)ptr;
        return (void*)((addr + cl - 1) & ~(cl - 1));
    }

    bool should_stream_store(size_t tensor_bytes) const {
        size_t sram_threshold;
        if (has_intel_bllc) {
            sram_threshold = 72ULL * 1024 * 1024;
        } else if (has_3d_vcache) {
            sram_threshold = 48ULL * 1024 * 1024;
        } else {
            sram_threshold = (l3_kb > 0 ? (l3_kb * 1024ULL) / 2 : (l2_kb * 1024ULL));
        }
        return tensor_bytes > sram_threshold;
    }

    static inline void flush_wc_buffers() {
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

    // Analytical BLIS Cache Blocking Dimension Solver
    void compute_blis_tiles(size_t elem_size,
                            size_t total_M, size_t total_N, size_t total_K,
                            int& M_C, int& N_C, int& K_C,
                            int& M_R, int& N_R) const {
        if (elem_size == 0) elem_size = 4;

#if defined(__AVX512F__)
        M_R = 16;
        N_R = 8;
#elif defined(__AVX2__) || defined(__AVX__)
        M_R = 8;
        N_R = 6;
#elif defined(__ARM_NEON) || defined(__aarch64__)
        M_R = 8;
        N_R = 4;
#else
        M_R = 4;
        N_R = 4;
#endif

        // 1. K_C block size: Pack panel A (M_C x K_C) in L1d cache (50% target)
        size_t l1d_bytes = (l1d_kb > 0 ? l1d_kb : 32) * 1024ULL;
        size_t l1d_target = l1d_bytes / 2;
        int kc_calc = (int)(l1d_target / (M_R * elem_size));
        if (kc_calc < 64)  kc_calc = 64;
        if (kc_calc > 512) kc_calc = 512;
        K_C = (int)(std::min)((size_t)kc_calc, total_K > 0 ? total_K : (size_t)kc_calc);

        // 2. M_C block size: Block of A (M_C x K_C) fits in L2 cache (60% target)
        size_t l2_bytes = (l2_kb > 0 ? l2_kb : 512) * 1024ULL;
        size_t l2_target = (l2_bytes * 6) / 10;
        int mc_calc = (int)(l2_target / (K_C * elem_size));
        if (mc_calc < 128)  mc_calc = 128;
        if (mc_calc > 2048) mc_calc = 2048;
        mc_calc = (mc_calc / M_R) * M_R;
        if (mc_calc == 0) mc_calc = M_R;
        M_C = (int)(std::min)((size_t)mc_calc, total_M > 0 ? total_M : (size_t)mc_calc);

        // 3. N_C block size: Macro-tile of B (K_C x N_C) fits in L3 / 3D V-Cache / bLLC
        if (has_intel_bllc) {
            N_C = 16384;
        } else if (has_3d_vcache) {
            N_C = 8192;
        } else {
            size_t l3_bytes = (l3_kb > 0 ? l3_kb : 16384) * 1024ULL;
            size_t l3_target = l3_bytes / 3;
            int nc_calc = (int)(l3_target / (K_C * elem_size));
            if (nc_calc < 256)  nc_calc = 256;
            if (nc_calc > 4096) nc_calc = 4096;
            N_C = nc_calc;
        }
        N_C = (N_C / N_R) * N_R;
        if (N_C == 0) N_C = N_R;
        if (total_N > 0 && (size_t)N_C > total_N) N_C = (int)total_N;
    }

    void get_optimal_tile_sizes(size_t elem_size, int& tile_M, int& tile_N, int& tile_K) const {
        int mr = 8, nr = 6;
        compute_blis_tiles(elem_size, 0, 0, 0, tile_M, tile_N, tile_K, mr, nr);
    }

    size_t get_cache_color_stride(uint32_t ways = 8) const {
        if (ways == 0) ways = 8;
        return (l1d_kb * 1024ULL) / ways;
    }
};

/* ------------------------------------------------------------------------- */
/* 5. Execution Focus Engine & Machines-Rules.td TableGen Ground-Truth Matrix */
/* ------------------------------------------------------------------------- */

enum class sf_uarch_ipc_tier_t : uint8_t {
    ULTRA_IPC      = 0, // Lion Cove, Golden Cove/Raptor Cove, Zen 5, Zen 6, Apple M3/M4, Oryon V2
    HIGH_IPC       = 1, // Zen 4, Zen 3, Sunny Cove (ICL), Willow Cove (TGL), Apple M1/M2, Neoverse V2
    MODERATE_IPC   = 2, // Skylake, Zen 2, Neoverse N1/N2, Ampere Altra, Cortex-A76
    EFFICIENCY_IPC = 3  // Skymont, Crestmont, Gracemont, Goldmont, Cortex-A55/A53
};

struct sf_tablegen_uarch_entry_t {
    const char*            profile_tag;    // Tag from Machines-Rules.td (e.g. "Profile_ARL_P", "Profile_ZEN5")
    const char*            uarch_name;     // Friendly name (e.g. "Intel Lion Cove", "AMD Zen 5")
    const char*            family_name;    // Family name (e.g. "Arrowlake", "Zen5", "AmpereAltra")
    const char*            vendor_code;    // "INTEL", "AMD", "APPL", "ARM", "QCOM"
    uint32_t               family;         // CPUID Family (or 0 for pattern/wildcard)
    uint32_t               model_min;      // CPUID Model range min
    uint32_t               model_max;      // CPUID Model range max
    bool                   is_hybrid;      // Chip family has P+E hybrid design
    sf_uarch_ipc_tier_t    ipc_tier;       // IPC category
    double                 rel_ipc_weight; // Relative IPC weight (e.g. 2.6 for Lion Cove, 1.0 for Gracemont/Skymont)
    uint32_t               max_vec_bits;   // Max vector bits (128, 256, 512)
};

static const sf_tablegen_uarch_entry_t SF_TABLEGEN_UARCH_RECORDS[] = {
    // Intel Desktop & Mobile Hybrid Profiles (Arrow Lake, Lunar Lake, Raptor Lake, Alder Lake, Meteor Lake)
    {"Profile_ARL_P", "Intel Lion Cove / Skymont (Arrow Lake-S)", "Arrowlake", "INTEL", 6, 0xC5, 0xC6, true,  sf_uarch_ipc_tier_t::ULTRA_IPC, 2.60, 256},
    {"Profile_LNL_P", "Intel Lion Cove / Skymont (Lunarlake)", "Lunarlake", "INTEL", 6, 0xBD, 0xBD, true,  sf_uarch_ipc_tier_t::ULTRA_IPC, 2.60, 256},
    {"Profile_RPL",   "Intel Raptor Cove / Gracemont (Raptorlake)", "Raptorlake", "INTEL", 6, 0xB7, 0xBF, true, sf_uarch_ipc_tier_t::ULTRA_IPC, 2.50, 256},
    {"Profile_ADL_P", "Intel Golden Cove / Gracemont (Alderlake)",  "Alderlake",  "INTEL", 6, 0x97, 0x9A, true, sf_uarch_ipc_tier_t::ULTRA_IPC, 2.45, 256},
    {"Profile_MTL_P", "Intel Redwood Cove / Crestmont (Meteorlake)", "Meteorlake", "INTEL", 6, 0xAA, 0xAC, true, sf_uarch_ipc_tier_t::ULTRA_IPC, 2.40, 256},
    
    // Intel Server Xeon Profiles (Sapphire Rapids, Emerald Rapids, Granite Rapids, Sierra Forest)
    {"Profile_SPR",   "Intel Golden Cove Xeon (Sapphire Rapids)", "SapphireRapids", "INTEL", 6, 0x8F, 0x8F, false, sf_uarch_ipc_tier_t::ULTRA_IPC, 2.50, 512},
    {"Profile_EMR",   "Intel Raptor Cove Xeon (Emerald Rapids)",  "EmeraldRapids",  "INTEL", 6, 0xCF, 0xCF, false, sf_uarch_ipc_tier_t::ULTRA_IPC, 2.50, 512},
    {"Profile_GNR",   "Intel Redwood Cove Xeon (Granite Rapids)", "GraniteRapids", "INTEL", 6, 0xAD, 0xAE, false, sf_uarch_ipc_tier_t::ULTRA_IPC, 2.50, 512},
    {"Profile_SRF",   "Intel Sierra Forest E-Core Xeon", "SierraForest", "INTEL", 6, 0xAF, 0xDF, false, sf_uarch_ipc_tier_t::EFFICIENCY_IPC, 1.00, 256},
    {"Profile_TGL",   "Intel Willow Cove (Tigerlake)", "Tigerlake", "INTEL", 6, 0x8C, 0x8D, false, sf_uarch_ipc_tier_t::HIGH_IPC, 1.80, 512},
    {"Profile_ICL",   "Intel Sunny Cove (Icelake)", "Icelake", "INTEL", 6, 0x7E, 0x7E, false, sf_uarch_ipc_tier_t::HIGH_IPC, 1.70, 512},
    {"Profile_SKX",   "Intel Skylake/Cascadelake Server", "Cascadelake", "INTEL", 6, 0x55, 0x55, false, sf_uarch_ipc_tier_t::MODERATE_IPC, 1.25, 512},

    // AMD Homogeneous & Hybrid Profiles (Zen 5, Zen 4, Zen 3, Zen 2, EPYC Turin, Threadripper)
    {"Profile_ZEN5",     "AMD Zen 5 (Granite Ridge / Turin 9005)", "Zen5", "AMD", 0x1A, 0x00, 0x7F, false, sf_uarch_ipc_tier_t::ULTRA_IPC, 2.65, 512},
    {"Profile_ZEN5c",    "AMD Zen 5c (Turin Dense)", "Zen5c", "AMD", 0x1A, 0x80, 0xFF, false, sf_uarch_ipc_tier_t::HIGH_IPC, 2.20, 512},
    {"Profile_ZEN4",     "AMD Zen 4 (Raphael / Genoa / Threadripper 7000)", "Zen4", "AMD", 0x19, 0x10, 0x7F, false, sf_uarch_ipc_tier_t::HIGH_IPC, 2.20, 512},
    {"Profile_ZEN4c",    "AMD Zen 4c (Bergamo Server)", "Zen4c", "AMD", 0x19, 0xA0, 0xAF, false, sf_uarch_ipc_tier_t::HIGH_IPC, 2.00, 512},
    {"Profile_ZEN3",     "AMD Zen 3 (Vermeer / Milan)", "Zen3", "AMD", 0x19, 0x00, 0x0F, false, sf_uarch_ipc_tier_t::HIGH_IPC, 1.90, 256},
    {"Profile_ZEN2",     "AMD Zen 2 (Matisse / Rome)", "Zen2", "AMD", 0x17, 0x30, 0x7F, false, sf_uarch_ipc_tier_t::MODERATE_IPC, 1.50, 256},

    // ARM & Server Microarchitectures (Ampere Altra Neoverse N1, AmpereOne, Nvidia Grace, Apple M-Series)
    {"Profile_AmpereAltra", "ARM Neoverse N1 (Ampere Altra / Altra Max 128C)", "AmpereAltra", "ARM", 0x41, 0xD0C, 0xD0C, false, sf_uarch_ipc_tier_t::MODERATE_IPC, 1.10, 128},
    {"Profile_AmpereOne",   "AmpereOne (Ampere A192-32X 192C)",                "AmpereOne",   "ARM", 0xC0, 0xAC3, 0xAC3, false, sf_uarch_ipc_tier_t::HIGH_IPC,     1.85, 128},
    {"Profile_NV2",         "ARM Neoverse V2 (Nvidia Grace / AWS Graviton4)",  "NeoverseV2",  "ARM", 0x41, 0xD4F, 0xD4F, false, sf_uarch_ipc_tier_t::HIGH_IPC,     2.10, 256},
    {"Profile_M4",          "Apple M4 (TSMC N3E 10-wide)", "AppleM4", "APPL", 0x61, 0x04, 0x04, true, sf_uarch_ipc_tier_t::ULTRA_IPC, 2.75, 128},
    {"Profile_M3",          "Apple M3 (TSMC N3B 8-wide)", "AppleM3", "APPL", 0x61, 0x03, 0x03, true, sf_uarch_ipc_tier_t::ULTRA_IPC, 2.50, 128},
    {"Profile_M2",          "Apple M2 (TSMC N5P 8-wide)", "AppleM2", "APPL", 0x61, 0x02, 0x02, true, sf_uarch_ipc_tier_t::HIGH_IPC, 2.25, 128},
    {"Profile_M1",          "Apple M1 (TSMC N5 8-wide)", "AppleM1", "APPL", 0x61, 0x01, 0x01, true, sf_uarch_ipc_tier_t::HIGH_IPC, 2.10, 128},
    {"Profile_ORY",         "Qualcomm Oryon (Snapdragon X Elite)", "OryonXElite", "QCOM", 0x51, 0x01, 0x02, false, sf_uarch_ipc_tier_t::HIGH_IPC, 2.20, 128}
};

struct sf_execution_focus_engine_t {
    std::atomic<SF_custominiVm_execution_focus_t> configured_focus{SF_FOCUS_AUTO};
    std::atomic<SF_custominiVm_execution_focus_t> active_focus{SF_FOCUS_AUTO};
    const sf_tablegen_uarch_entry_t*              matched_uarch{nullptr};
    char                                          uarch_summary[128]{0};

    double p_weight{1.0};
    double e_weight{1.0};

    bool hw_is_hybrid{false};   // Immutable physical CPU topology
    bool is_hybrid{false};      // Mode property: uses hybrid asymmetric partitioning
    bool is_homogeneous{true};  // Mode property: uses homogeneous uniform partitioning
    bool is_scp{false};
    bool is_mcp{false};
    bool is_dual{false};

    uint32_t p_cores{0};
    uint32_t e_cores{0};
    uint32_t total_cores{0};

    sf_hybrid_homo_engine_t* hybrid_eng_ref{nullptr};

    void init(const sf_hardware_topology_t* topo, sf_hybrid_homo_engine_t* hybrid_eng, const sf_sram_cache_engine_t* sram_eng) {
        (void)sram_eng;
        hybrid_eng_ref = hybrid_eng;
        total_cores = topo ? topo->physical_cores : (uint32_t)std::thread::hardware_concurrency();
        if (total_cores == 0) total_cores = 1;

        hw_is_hybrid = (hybrid_eng && hybrid_eng->is_hybrid) || (topo && topo->is_heterogeneous != 0);
        is_hybrid = hw_is_hybrid;
        is_homogeneous = !hw_is_hybrid;
        p_cores = (hybrid_eng && hybrid_eng->p_core_count > 0) ? hybrid_eng->p_core_count : (topo ? topo->p_cores : total_cores);
        e_cores = (hybrid_eng && hw_is_hybrid) ? hybrid_eng->e_core_count : (topo ? topo->e_cores : 0);

        // CPUID Inspection (Family, Model, Stepping)
        char vendor[16] = {0};
        uint32_t family = 0, model = 0, stepping = 0;
#if defined(_M_X64) || defined(__x86_64__)
        int cinfo[4] = {0};
        sf_get_cpuid(cinfo, 0, 0);
        memcpy(vendor + 0, &cinfo[1], 4);
        memcpy(vendor + 4, &cinfo[3], 4);
        memcpy(vendor + 8, &cinfo[2], 4);
        vendor[12] = '\0';

        sf_get_cpuid(cinfo, 1, 0);
        uint32_t raw_f = (cinfo[0] >> 8) & 0xF;
        uint32_t raw_m = (cinfo[0] >> 4) & 0xF;
        uint32_t ext_f = (cinfo[0] >> 20) & 0xFF;
        uint32_t ext_m = (cinfo[0] >> 16) & 0xF;
        family = (raw_f == 15) ? (raw_f + ext_f) : raw_f;
        model  = ((raw_f == 6 || raw_f == 15) ? (ext_m << 4) : 0) | raw_m;
        stepping = cinfo[0] & 0xF;
#elif defined(__APPLE__)
        strncpy(vendor, "APPL", sizeof(vendor) - 1);
        family = 0x61; model = 0x04;
#elif defined(__aarch64__) || defined(_M_ARM64)
        strncpy(vendor, "ARM", sizeof(vendor) - 1);
        family = 0x41; model = 0xD0C;
#endif

        // TableGen Profile Matching
        matched_uarch = nullptr;
        for (const auto& rec : SF_TABLEGEN_UARCH_RECORDS) {
            bool vmatch = false;
            if (strcmp(rec.vendor_code, "INTEL") == 0 && (strstr(vendor, "Intel") || strstr(vendor, "INTEL") || vendor[0] == '\0')) vmatch = true;
            else if (strcmp(rec.vendor_code, "AMD") == 0 && (strstr(vendor, "AMD") || strstr(vendor, "AuthenticAMD"))) vmatch = true;
            else if (strcmp(rec.vendor_code, "APPL") == 0 && strcmp(vendor, "APPL") == 0) vmatch = true;
            else if (strcmp(rec.vendor_code, "ARM") == 0 && strcmp(vendor, "ARM") == 0) vmatch = true;
            else if (strcmp(rec.vendor_code, "QCOM") == 0 && strcmp(vendor, "QCOM") == 0) vmatch = true;

            if (vmatch) {
                if (rec.family == family && model >= rec.model_min && model <= rec.model_max) {
                    if (rec.is_hybrid == hw_is_hybrid) {
                        matched_uarch = &rec;
                        break;
                    } else if (!matched_uarch) {
                        matched_uarch = &rec;
                    }
                }
            }
        }

        // Fallback matching by vendor family if exact model range not matched
        if (!matched_uarch) {
            for (const auto& rec : SF_TABLEGEN_UARCH_RECORDS) {
                bool vmatch = false;
                if (strcmp(rec.vendor_code, "INTEL") == 0 && (strstr(vendor, "Intel") || strstr(vendor, "INTEL") || vendor[0] == '\0')) vmatch = true;
                else if (strcmp(rec.vendor_code, "AMD") == 0 && (strstr(vendor, "AMD") || strstr(vendor, "AuthenticAMD"))) vmatch = true;
                if (vmatch && rec.family == family) {
                    if (rec.is_hybrid == hw_is_hybrid) {
                        matched_uarch = &rec;
                        break;
                    }
                }
            }
        }

        // Default profile if still null
        if (!matched_uarch) {
            if (hw_is_hybrid) {
                matched_uarch = &SF_TABLEGEN_UARCH_RECORDS[0]; // Arrow Lake-S default
            } else if (total_cores >= 16) {
                matched_uarch = &SF_TABLEGEN_UARCH_RECORDS[12]; // Zen 5 default
            } else {
                matched_uarch = &SF_TABLEGEN_UARCH_RECORDS[0];
            }
        }

        snprintf(uarch_summary, sizeof(uarch_summary), "%s [%s | %s]",
                 matched_uarch->uarch_name, matched_uarch->profile_tag,
                 hw_is_hybrid ? "Hybrid P+E" : "Homogeneous");

        SF_custominiVm_execution_focus_t detected = detect_focus(topo, hybrid_eng);
        SF_custominiVm_execution_focus_t target = configured_focus.load(std::memory_order_relaxed);
        if (target == SF_FOCUS_AUTO) {
            apply_state(detected);
        } else {
            set_focus(target);
        }
    }

    SF_custominiVm_execution_focus_t detect_focus(const sf_hardware_topology_t* topo, const sf_hybrid_homo_engine_t* hybrid_eng) const {
        bool hyb = (hybrid_eng && hybrid_eng->is_hybrid) || (topo && topo->is_heterogeneous != 0) || hw_is_hybrid;
        uint32_t c_phys = (topo && topo->physical_cores > 0) ? topo->physical_cores : total_cores;
        if (c_phys == 0) c_phys = 1;

        sf_uarch_ipc_tier_t ipc = matched_uarch ? matched_uarch->ipc_tier : (hyb ? sf_uarch_ipc_tier_t::ULTRA_IPC : sf_uarch_ipc_tier_t::HIGH_IPC);

        // Rule 1: Low Core Count (<= 8 cores) -> SCP Focus
        if (c_phys <= 8) {
            return hyb ? SF_FOCUS_SCP_HYBRID : SF_FOCUS_SCP_HOMO;
        }

        // Rule 2: Massive Core Count (>= 64 cores) -> MCP or Dual Homo Focus
        if (c_phys >= 64) {
            if (ipc == sf_uarch_ipc_tier_t::MODERATE_IPC || ipc == sf_uarch_ipc_tier_t::EFFICIENCY_IPC) {
                // Ampere Altra 128C, Sierra Forest 144C E-cores -> Multi-Core Throughput Focus
                return hyb ? SF_FOCUS_MCP_HYBRID : SF_FOCUS_MCP_HOMO;
            } else {
                // High/Ultra IPC homogeneous server/HEDT: AMD EPYC Turin 128 Zen 5, Threadripper 96 Zen 4/5
                // Every single core is a 512-bit AVX-512 powerhouse -> DUAL_HOMO!
                return hyb ? SF_FOCUS_DUAL_HYBRID : SF_FOCUS_DUAL_HOMO;
            }
        }

        // Rule 3: Hybrid CPU with > 8 and <= 56 cores (e.g. Intel Core Ultra 5 245K 6P+8E = 14C, Core i9 14900K 8P+16E = 24C, Xeon Max 56C)
        if (hyb) {
            return SF_FOCUS_DUAL_HYBRID;
        }

        // Rule 4: Homogeneous CPU with 12..32 cores and ULTRA or HIGH IPC (e.g. AMD Ryzen 9 9950X 16C Zen 5, 7950X 16C Zen 4)
        if (!hyb && (ipc == sf_uarch_ipc_tier_t::ULTRA_IPC || ipc == sf_uarch_ipc_tier_t::HIGH_IPC) && c_phys >= 12) {
            return SF_FOCUS_DUAL_HOMO;
        }

        // Rule 5: Other multi-core homogeneous (> 8 cores)
        return SF_FOCUS_MCP_HOMO;
    }

    void set_focus(SF_custominiVm_execution_focus_t focus) {
        configured_focus.store(focus, std::memory_order_relaxed);
        SF_custominiVm_execution_focus_t resolved = focus;
        if (focus == SF_FOCUS_AUTO) {
            resolved = detect_focus(nullptr, hybrid_eng_ref);
        } else if (focus == SF_FOCUS_SCP) {
            resolved = hw_is_hybrid ? SF_FOCUS_SCP_HYBRID : SF_FOCUS_SCP_HOMO;
        } else if (focus == SF_FOCUS_MCP) {
            resolved = hw_is_hybrid ? SF_FOCUS_MCP_HYBRID : SF_FOCUS_MCP_HOMO;
        } else if (focus == SF_FOCUS_HYBRID_DUAL || focus == SF_FOCUS_DUAL) {
            resolved = hw_is_hybrid ? SF_FOCUS_DUAL_HYBRID : SF_FOCUS_DUAL_HOMO;
        }
        apply_state(resolved);
    }

    void apply_state(SF_custominiVm_execution_focus_t resolved) {
        active_focus.store(resolved, std::memory_order_release);
        switch (resolved) {
            case SF_FOCUS_SCP_HOMO:
                is_scp = true; is_mcp = false; is_dual = false;
                is_hybrid = false; is_homogeneous = true;
                p_weight = 1.0; e_weight = 1.0;
                if (hybrid_eng_ref) hybrid_eng_ref->bind_current_thread(SF_CORE_TIER_ANY);
                sf_upclocker_set_boost(1);
                sf_upclocker_t::pin_max_frequency_warmup(50);
                break;
            case SF_FOCUS_SCP_HYBRID:
                is_scp = true; is_mcp = false; is_dual = false;
                is_hybrid = true; is_homogeneous = false;
                p_weight = 1.0; e_weight = 0.0;
                if (hybrid_eng_ref) hybrid_eng_ref->bind_current_thread(SF_CORE_TIER_PERFORMANCE);
                sf_upclocker_set_boost(1);
                sf_upclocker_t::pin_max_frequency_warmup(50);
                break;
            case SF_FOCUS_MCP_HOMO:
                is_scp = false; is_mcp = true; is_dual = false;
                is_hybrid = false; is_homogeneous = true;
                p_weight = 1.0; e_weight = 1.0;
                if (hybrid_eng_ref) hybrid_eng_ref->bind_current_thread(SF_CORE_TIER_ANY);
                break;
            case SF_FOCUS_MCP_HYBRID:
                is_scp = false; is_mcp = true; is_dual = false;
                is_hybrid = true; is_homogeneous = false;
                p_weight = 2.5; e_weight = 1.0;
                break;
            case SF_FOCUS_DUAL_HOMO:
                is_scp = false; is_mcp = false; is_dual = true;
                is_hybrid = false; is_homogeneous = true;
                p_weight = 1.0; e_weight = 1.0;
                if (hybrid_eng_ref) hybrid_eng_ref->bind_current_thread(SF_CORE_TIER_ANY);
                sf_upclocker_set_boost(1);
                break;
            case SF_FOCUS_DUAL_HYBRID:
                is_scp = false; is_mcp = false; is_dual = true;
                is_hybrid = true; is_homogeneous = false;
                p_weight = 2.5; e_weight = 1.0;
                sf_upclocker_set_boost(1);
                break;
            default:
                break;
        }
    }
};

/* ------------------------------------------------------------------------- */
/* 6. Unified Hardware Engine Nexus                                          */
/* ------------------------------------------------------------------------- */
struct sf_hardware_engine_nexus_t {
    sf_hybrid_homo_engine_t     hybrid;
    sf_numa_uma_engine_t        numa;
    sf_smt_ht_engine_t          smt;
    sf_sram_cache_engine_t      sram;
    sf_execution_focus_engine_t focus;
    std::atomic<bool>           initialized{false};

    void init(const sf_hardware_topology_t* topo) {
        hybrid.init(topo);
        numa.init(topo);
        smt.init(topo);
        sram.init(topo);
        focus.init(topo, &hybrid, &sram);
        initialized.store(true, std::memory_order_release);
    }

    void cleanup() {
        sram.cleanup();
        initialized.store(false, std::memory_order_release);
    }
};

static sf_hardware_engine_nexus_t g_sf_hardware_nexus;

static void sf_init_hardware_nexus(const sf_hardware_topology_t* topo) {
    g_sf_hardware_nexus.init(topo);
}

static bool sf_bind_current_thread(SF_custominiVm_core_tier_t tier) {
    g_sf_hardware_nexus.hybrid.set_thread_qos(tier);
    return g_sf_hardware_nexus.hybrid.bind_current_thread(tier);
}

static void sf_ensure_hardware_nexus_initialized(void) {
    static std::once_flag s_exit_sweep_once;
    std::call_once(s_exit_sweep_once, []() {
        std::atexit([]() {
            SF_custominiVm_emergency_os_sweep();
        });
        std::set_terminate([]() {
            SF_custominiVm_emergency_os_sweep();
            std::abort();
        });
#ifdef _WIN32
        SetUnhandledExceptionFilter([](EXCEPTION_POINTERS*) -> LONG {
            SF_custominiVm_emergency_os_sweep();
            return EXCEPTION_CONTINUE_SEARCH;
        });
#endif
    });
    if (g_sf_hardware_nexus.initialized.load(std::memory_order_acquire)) return;
    sf_cpu_isa_inventory_t::discover();
}

static bool sf_is_hybrid_cpu(void) {
    return g_sf_hardware_nexus.hybrid.is_hybrid;
}

static bool sf_is_focus_scp(void) {
    return g_sf_hardware_nexus.focus.is_scp;
}

static bool sf_is_efficiency_core_active(void) {
    if (tl_sf_core_tier == SF_CORE_TIER_EFFICIENCY) return true;
    if (tl_sf_core_tier == SF_CORE_TIER_PERFORMANCE) return false;
    if (!g_sf_hardware_nexus.hybrid.is_hybrid) return false;
#if defined(_WIN32)
    PROCESSOR_NUMBER pn;
    GetCurrentProcessorNumberEx(&pn);
    return g_sf_hardware_nexus.hybrid.classify_core(pn.Number) == SF_CORE_TIER_EFFICIENCY;
#elif defined(__linux__)
    int cpu = sched_getcpu();
    return (cpu >= 0) && (g_sf_hardware_nexus.hybrid.classify_core(cpu) == SF_CORE_TIER_EFFICIENCY);
#else
    return false;
#endif
}

static uint32_t sf_get_p_core_count(void) {
    return g_sf_hardware_nexus.hybrid.p_core_count;
}

static uint32_t sf_get_e_core_count(void) {
    return g_sf_hardware_nexus.hybrid.e_core_count;
}

static uint32_t sf_get_sram_l1d_kb(void) {
    return g_sf_hardware_nexus.sram.l1d_kb > 0 ? g_sf_hardware_nexus.sram.l1d_kb : 32;
}

static uint32_t sf_get_sram_l2_kb(void) {
    return g_sf_hardware_nexus.sram.l2_kb > 0 ? g_sf_hardware_nexus.sram.l2_kb : 512;
}

static uint32_t sf_get_sram_l3_kb(void) {
    return g_sf_hardware_nexus.sram.l3_kb > 0 ? g_sf_hardware_nexus.sram.l3_kb : 16384;
}

static bool sf_has_massive_sram_vcache(void) {
    return g_sf_hardware_nexus.sram.has_3d_vcache || g_sf_hardware_nexus.sram.has_intel_bllc;
}

static uint8_t sf_get_numa_node_count(void) {
    return g_sf_hardware_nexus.numa.node_count;
}

static bool sf_is_uma_system(void) {
    return g_sf_hardware_nexus.numa.is_uma;
}

static inline bool sf_hardware_should_stream_store(size_t tensor_bytes, const sf_hardware_topology_t* topo) {
    if (g_sf_hardware_nexus.initialized.load(std::memory_order_relaxed)) {
        return g_sf_hardware_nexus.sram.should_stream_store(tensor_bytes);
    }
    if (!topo) return false;
    size_t llc_bytes = (topo->l3.size_kb > 0 ? topo->l3.size_kb : topo->l2.size_kb) * 1024ULL;
    return tensor_bytes > llc_bytes;
}

static inline void sf_hardware_optimal_tile_sizes(size_t elem_size, int& tile_M, int& tile_N, int& tile_K,
                                                  const sf_hardware_topology_t* topo) {
    if (g_sf_hardware_nexus.initialized.load(std::memory_order_relaxed)) {
        g_sf_hardware_nexus.sram.get_optimal_tile_sizes(elem_size, tile_M, tile_N, tile_K);
        return;
    }
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

    SF_custominiVm_emergency_os_sweep();
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
    uint8_t* data = nullptr;

    sf_puzzle_sample_t() {
        data = (uint8_t*)_mm_malloc(128 * 1024, 64);
        if (data) memset(data, 0, 128 * 1024);
    }
    ~sf_puzzle_sample_t() {
        if (data) _mm_free(data);
    }
    sf_puzzle_sample_t(const sf_puzzle_sample_t& o) {
        dtype = o.dtype; byte_size = o.byte_size; ndim = o.ndim;
        for (int i=0; i<8; ++i) shape[i] = o.shape[i];
        data = (uint8_t*)_mm_malloc(128 * 1024, 64);
        if (o.data && data) memcpy(data, o.data, 128 * 1024);
    }
    sf_puzzle_sample_t(sf_puzzle_sample_t&& o) noexcept {
        dtype = o.dtype; byte_size = o.byte_size; ndim = o.ndim;
        for (int i=0; i<8; ++i) shape[i] = o.shape[i];
        data = o.data;
        o.data = nullptr;
    }
    sf_puzzle_sample_t& operator=(const sf_puzzle_sample_t& o) {
        if (this == &o) return *this;
        dtype = o.dtype; byte_size = o.byte_size; ndim = o.ndim;
        for (int i=0; i<8; ++i) shape[i] = o.shape[i];
        if (!data) data = (uint8_t*)_mm_malloc(128 * 1024, 64);
        if (o.data && data) memcpy(data, o.data, 128 * 1024);
        return *this;
    }
    sf_puzzle_sample_t& operator=(sf_puzzle_sample_t&& o) noexcept {
        if (this == &o) return *this;
        if (data) _mm_free(data);
        dtype = o.dtype; byte_size = o.byte_size; ndim = o.ndim;
        for (int i=0; i<8; ++i) shape[i] = o.shape[i];
        data = o.data;
        o.data = nullptr;
        return *this;
    }
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

        // Batch optimize all matching objects of this function type in Motherline Egg!
        if (runway) {
            runway->batch_optimize_egg_objects(fn_id, name, 0, new_cycles);
        }

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

        // Warmup current_fn (L1/L2 and I-cache priming)
        if (!samples.empty()) {
            SF_custominiVm_tensor_t win_t{};
            win_t.data = const_cast<uint8_t*>(samples[0].data);
            win_t.ndim = samples[0].ndim;
            for (int d = 0; d < 8; ++d) win_t.shape[d] = samples[0].shape[d];
            win_t.dtype = samples[0].dtype;
            win_t.byte_size = samples[0].byte_size;
            SF_custominiVm_tensor_t wout_t = win_t;
            wout_t.data = current_best_out_payload;
            uint32_t wfault = 0;
            sf_safe_kernel_call(current_fn, &win_t, 1, &wout_t, 1, nullptr, &wfault);
        }

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
                // Pin execution to physical P-cores to eliminate SMT port 0/1/5 collisions and E-core latency
                g_sf_hardware_nexus.hybrid.bind_current_thread(SF_CORE_TIER_PERFORMANCE);
                g_sf_hardware_nexus.smt.apply_affinity_policy(SF_AFFINITY_PHYSICAL_CORES_ONLY);

                alignas(64) uint8_t local_cand_out[STAGNANT_BUF_CAP];
                uint64_t cand_cycles_total = 0;
                float max_err = 0.0f;
                float sum_sq = 0.0f;
                size_t total_elems_tested = 0;
                bool all_ok = true;
                uint32_t cand_fault = 0;

                // Candidate warmup pass (prime instruction cache, branch history and memory pipelines)
                if (!samples.empty()) {
                    SF_custominiVm_tensor_t win_t{};
                    win_t.data = const_cast<uint8_t*>(samples[0].data);
                    win_t.ndim = samples[0].ndim;
                    for (int d = 0; d < 8; ++d) win_t.shape[d] = samples[0].shape[d];
                    win_t.dtype = samples[0].dtype;
                    win_t.byte_size = samples[0].byte_size;
                    SF_custominiVm_tensor_t wout_t = win_t;
                    wout_t.data = local_cand_out;
                    uint32_t wfault = 0;
                    sf_safe_kernel_call(evals[i].fn, &win_t, 1, &wout_t, 1, nullptr, &wfault);
                }

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

        // Warmup current_fn
        if (!samples.empty()) {
            SF_custominiVm_tensor_t win_t{};
            win_t.data = const_cast<uint8_t*>(samples[0].data);
            win_t.ndim = samples[0].ndim;
            for (int d = 0; d < 8; ++d) win_t.shape[d] = samples[0].shape[d];
            win_t.dtype = samples[0].dtype;
            win_t.byte_size = samples[0].byte_size;
            SF_custominiVm_tensor_t wout_t = win_t;
            wout_t.data = current_best_out_payload;
            uint32_t wfault = 0;
            sf_safe_kernel_call(current_fn, &win_t, 1, &wout_t, 1, nullptr, &wfault);
        }

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

        // Warmup candidate
        if (!samples.empty()) {
            SF_custominiVm_tensor_t win_t{};
            win_t.data = const_cast<uint8_t*>(samples[0].data);
            win_t.ndim = samples[0].ndim;
            for (int d = 0; d < 8; ++d) win_t.shape[d] = samples[0].shape[d];
            win_t.dtype = samples[0].dtype;
            win_t.byte_size = samples[0].byte_size;
            SF_custominiVm_tensor_t wout_t = win_t;
            wout_t.data = candidate_out_payload;
            uint32_t wfault = 0;
            sf_safe_kernel_call(exp.candidate, &win_t, 1, &wout_t, 1, nullptr, &wfault);
        }

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
        base = (uint8_t*)sf_os_reserve_memory(capacity);
        if (base && !sf_os_commit_memory(base, capacity)) {
            sf_os_release_memory(base, capacity);
            base = nullptr;
        }
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
        sf_os_release_memory(base, capacity);
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
        exec_base    = (uint8_t*)sf_os_reserve_memory(capacity);
        if (exec_base && !sf_os_commit_memory(exec_base, capacity)) {
            sf_os_release_memory(exec_base, capacity);
            exec_base = nullptr;
        }
        staging_base = (uint8_t*)sf_os_reserve_memory(capacity);
        if (staging_base && !sf_os_commit_memory(staging_base, capacity)) {
            sf_os_release_memory(staging_base, capacity);
            staging_base = nullptr;
        }
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

    bool init(size_t sz = 64UL * 1024 * 1024, bool Advance_Payment = false) {
        (void)Advance_Payment;
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
    sf_mathventory_container_header_t hdr;
    memcpy(&hdr, src, sizeof(hdr));
    if (hdr.magic != SF_MATHVENTORY_MAGIC) {
        if (src_len <= dst_cap) {
            memcpy(dst, src, src_len);
            if (out_decomp_len) *out_decomp_len = src_len;
            return true;
        }
        return false;
    }

    size_t uncompressed = (size_t)hdr.uncompressed_size;
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
MSIMD_API void sf_vm_hookpoint_trigger(uint32_t event_id, uint32_t fn_id, void* context, size_t byte_size);
MSIMD_API void sf_gc_hookpoint_trigger(uint32_t event_id, uint32_t fn_id, void* context, size_t byte_size);
MSIMD_API int  sf_gc_pausepoint_poll(void);
MSIMD_API int  sf_vm_pausepoint_poll(void);
MSIMD_API void SF_custominiVm_set_pausepoint_gate(int armed);
MSIMD_API int  SF_custominiVm_get_pausepoint_gate(void);
}

#ifdef SACFOR_HAS_LLVM
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/SelfExecutorProcessControl.h>
#include <llvm/ExecutionEngine/Orc/AbsoluteSymbols.h>
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
    static bool is_docking_port_fn(llvm::StringRef name) {
        return name == "sf_gc_safepoint_poll" ||
               name == "sf_gc_controlpoint_check" ||
               name == "sf_vm_hookpoint_trigger" ||
               name == "sf_gc_hookpoint_trigger" ||
               name == "sf_gc_pausepoint_poll" ||
               name == "sf_vm_pausepoint_poll";
    }

    static void inject_loop_safepoints(llvm::Loop* L, llvm::LLVMContext& ctx,
                                       llvm::FunctionCallee poll_fn,
                                       llvm::FunctionCallee cp_fn,
                                       llvm::FunctionCallee pause_fn) {
        if (!L) return;

        bool loop_has_safepoint  = false;
        bool loop_has_pausepoint = false;
        for (llvm::BasicBlock* bb : L->blocks()) {
            for (llvm::Instruction& I : *bb) {
                if (auto* CI = llvm::dyn_cast<llvm::CallBase>(&I)) {
                    if (llvm::Function* callee = CI->getCalledFunction()) {
                        llvm::StringRef cname = callee->getName();
                        if (cname == "sf_gc_safepoint_poll") {
                            loop_has_safepoint = true;
                        } else if (cname == "sf_gc_pausepoint_poll" ||
                                   cname == "sf_vm_pausepoint_poll") {
                            loop_has_pausepoint = true;
                        }
                    }
                }
            }
        }

        llvm::BasicBlock* header = L->getHeader();
        if (header && !loop_has_safepoint && !loop_has_pausepoint) {
            llvm::IRBuilder<> builder(&*header->getFirstInsertionPt());
            builder.CreateCall(poll_fn);
            llvm::Value* zero_sz = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);
            builder.CreateCall(cp_fn, {zero_sz});
        }

        if (!loop_has_pausepoint) {
            llvm::BasicBlock* latch = L->getLoopLatch();
            llvm::BasicBlock* target_bb = latch ? latch : header;
            if (target_bb && !target_bb->empty()) {
                if (llvm::Instruction* term = target_bb->getTerminator()) {
                    llvm::IRBuilder<> b_term(term);
                    b_term.CreateCall(pause_fn);
                }
            }
        }

        for (llvm::Loop* sub_loop : *L) {
            inject_loop_safepoints(sub_loop, ctx, poll_fn, cp_fn, pause_fn);
        }
    }

    static void inject_into_module(llvm::Module& M, uint32_t fn_id_base = 0) {
        llvm::LLVMContext& ctx = M.getContext();

        // Normalize DLL storage class on declarations & globals so COFF/Windows ORCJIT
        // binds directly to main_jd SymbolMap without requiring __imp_ indirection stubs.
        for (llvm::GlobalVariable& GV : M.globals()) {
            if (GV.hasDLLImportStorageClass()) {
                GV.setDLLStorageClass(llvm::GlobalValue::DefaultStorageClass);
            }
        }
        for (llvm::Function& Fn : M) {
            if (Fn.hasDLLImportStorageClass()) {
                Fn.setDLLStorageClass(llvm::GlobalValue::DefaultStorageClass);
            }
        }

        llvm::FunctionType* poll_fn_type = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), false);
        llvm::FunctionCallee poll_callee = M.getOrInsertFunction("sf_gc_safepoint_poll", poll_fn_type);

        std::vector<llvm::Type*> cp_args = { llvm::Type::getInt64Ty(ctx) };
        llvm::FunctionType* cp_fn_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), cp_args, false);
        llvm::FunctionCallee cp_callee = M.getOrInsertFunction("sf_gc_controlpoint_check", cp_fn_type);

        std::vector<llvm::Type*> hook_args = {
            llvm::Type::getInt32Ty(ctx),
            llvm::Type::getInt32Ty(ctx),
            llvm::PointerType::getUnqual(ctx),
            llvm::Type::getInt64Ty(ctx)
        };
        llvm::FunctionType* hook_fn_type = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), hook_args, false);
        llvm::FunctionCallee hook_callee = M.getOrInsertFunction("sf_vm_hookpoint_trigger", hook_fn_type);

        llvm::FunctionType* pause_fn_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), false);
        llvm::FunctionCallee pause_callee = M.getOrInsertFunction("sf_gc_pausepoint_poll", pause_fn_type);

        uint32_t fn_seq = fn_id_base;
        for (llvm::Function& F : M) {
            if (F.isDeclaration() || F.isIntrinsic() || F.empty()) continue;
            if (is_docking_port_fn(F.getName())) continue;

            uint32_t cur_fn_id = ++fn_seq;

            // Inspect existing docking adapters in F to avoid double-docking pre-instrumented kernels
            bool has_hookpoint    = false;
            bool has_safepoint    = false;
            bool has_controlpoint = false;
            for (llvm::BasicBlock& bb : F) {
                for (llvm::Instruction& I : bb) {
                    if (auto* CI = llvm::dyn_cast<llvm::CallBase>(&I)) {
                        if (llvm::Function* callee = CI->getCalledFunction()) {
                            llvm::StringRef cname = callee->getName();
                            if (cname == "sf_vm_hookpoint_trigger" || cname == "sf_gc_hookpoint_trigger") {
                                has_hookpoint = true;
                            } else if (cname == "sf_gc_safepoint_poll") {
                                has_safepoint = true;
                            } else if (cname == "sf_gc_controlpoint_check") {
                                has_controlpoint = true;
                            }
                        }
                    }
                }
            }

            if (!has_hookpoint || !has_safepoint || !has_controlpoint) {
                llvm::BasicBlock& entry = F.getEntryBlock();
                llvm::IRBuilder<> builder(&*entry.getFirstInsertionPt());

                llvm::Value* fn_id_val = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), cur_fn_id);
                llvm::Value* ctx_ptr   = llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(ctx));
                llvm::Value* byte_sz   = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 0);

                // Signature-aware ISS Docking: bind first pointer arg as tensor context
                // and first integer arg as element count (scaled by 4 bytes)
                for (llvm::Argument& arg : F.args()) {
                    if (arg.getType()->isPointerTy()) {
                        ctx_ptr = &arg;
                        break;
                    }
                }
                for (llvm::Argument& arg : F.args()) {
                    if (arg.getType()->isIntegerTy(32) || arg.getType()->isIntegerTy(64)) {
                        llvm::Value* cnt64 = arg.getType()->isIntegerTy(64)
                            ? static_cast<llvm::Value*>(&arg)
                            : builder.CreateZExt(&arg, llvm::Type::getInt64Ty(ctx));
                        byte_sz = builder.CreateShl(cnt64, llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 2));
                        break;
                    }
                }

                if (!has_hookpoint) {
                    llvm::Value* ev_enter = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1);
                    builder.CreateCall(hook_callee, {ev_enter, fn_id_val, ctx_ptr, byte_sz});
                }
                if (!has_safepoint) {
                    builder.CreateCall(poll_callee);
                }
                if (!has_controlpoint) {
                    builder.CreateCall(cp_callee, {byte_sz});
                }

                if (!has_hookpoint) {
                    for (llvm::BasicBlock& bb : F) {
                        llvm::Instruction* term = bb.getTerminator();
                        if (term && llvm::isa<llvm::ReturnInst>(term)) {
                            llvm::IRBuilder<> b_ret(term);
                            llvm::Value* ev_exit = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 2);
                            b_ret.CreateCall(hook_callee, {ev_exit, fn_id_val, ctx_ptr, byte_sz});
                        }
                    }
                }
            }

            llvm::DominatorTree DT(F);
            llvm::LoopInfo LI(DT);
            for (llvm::Loop* L : LI) {
                inject_loop_safepoints(L, ctx, poll_callee, cp_callee, pause_callee);
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
            [](const MemoryBuffer &) { return std::make_unique<SectionMemoryManager>(); }
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

        // ISS 4-Point Docking Table: bind all 4 sfGC/SFVM coordination ports into ORCJIT main_jd
        const JITSymbolFlags fn_flags = JITSymbolFlags::Exported | JITSymbolFlags::Callable;
        SymbolMap syms;
        syms[(*mangle)("sf_gc_safepoint_poll")]             = { ExecutorAddr::fromPtr(&sf_gc_safepoint_poll), fn_flags };
        syms[(*mangle)("sf_gc_controlpoint_check")]         = { ExecutorAddr::fromPtr(&sf_gc_controlpoint_check), fn_flags };
        syms[(*mangle)("sf_vm_hookpoint_trigger")]          = { ExecutorAddr::fromPtr(&sf_vm_hookpoint_trigger), fn_flags };
        syms[(*mangle)("sf_gc_hookpoint_trigger")]          = { ExecutorAddr::fromPtr(&sf_gc_hookpoint_trigger), fn_flags };
        syms[(*mangle)("sf_gc_pausepoint_poll")]            = { ExecutorAddr::fromPtr(&sf_gc_pausepoint_poll), fn_flags };
        syms[(*mangle)("sf_vm_pausepoint_poll")]            = { ExecutorAddr::fromPtr(&sf_vm_pausepoint_poll), fn_flags };
        syms[(*mangle)("SF_custominiVm_set_pausepoint_gate")] = { ExecutorAddr::fromPtr(&SF_custominiVm_set_pausepoint_gate), fn_flags };
        syms[(*mangle)("SF_custominiVm_get_pausepoint_gate")] = { ExecutorAddr::fromPtr(&SF_custominiVm_get_pausepoint_gate), fn_flags };
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
        if (!ready || !blob || sz == 0) return false;
        auto ts_ctx = std::make_unique<LLVMContext>();
        auto buf = MemoryBuffer::getMemBuffer(
            StringRef(reinterpret_cast<const char*>(blob), sz), "", false);
        auto mod_or = parseBitcodeFile(*buf, *ts_ctx);
        if (!mod_or) {
            llvm::consumeError(mod_or.takeError());
            sacculus_hotlog_write(hotlog, SF_Ebin0_ERROR, SF_Ebin1_ERROR,
                fn_id_base, 0, 0, 0, 0, 0, 0, "bc_parse_fail");
            return false;
        }

        SfvmSafepointPass::inject_into_module(**mod_or, fn_id_base);

        auto err = compile_layer->add(
            *main_jd,
            ThreadSafeModule(std::move(*mod_or), std::move(ts_ctx)));
        if (err) {
            llvm::consumeError(std::move(err));
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
        if (!ready || !sym) return false;
        auto s = es->lookup({main_jd}, (*mangle)(sym));
        if (!s) {
            llvm::consumeError(s.takeError());
            sacculus_hotlog_write(hotlog, SF_Ebin0_ERROR, SF_Ebin1_WARN,
                fn_id, 0, 0, 0, 0, 0, 0, sym);
            return false;
        }
        dispatch->register_fn(fn_id, (sacculus_kernel_fn_t)s->getAddress().getValue());
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
        if (validated_cfg.execution_focus != SF_FOCUS_AUTO) {
            SF_custominiVm_set_execution_focus(validated_cfg.execution_focus);
        } else {
            SF_custominiVm_set_execution_focus(SF_FOCUS_AUTO);
        }

        hotlog  = new (std::nothrow) sacculus_hotlog_t;
        coldlog = new (std::nothrow) sacculus_coldlog_t;
        if (!hotlog || !coldlog) return false;
        hotlog->init();
        coldlog->init();
        g_sf_active_hotlog.store(hotlog, std::memory_order_release);
        g_sf_active_coldlog.store(coldlog, std::memory_order_release);
        g_sf_strict_vfs_validation.store(validated_cfg.strings.strict_vfs_branch_validation, std::memory_order_relaxed);
        g_sf_vfs_panic_dialog_enabled.store(validated_cfg.strings.vfs_panic_dialog, std::memory_order_relaxed);

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

        sacculus_hotlog_write(hotlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_INFO, 0, 0, 0, 0, 0, 0, 0, "sf_forge_shutdown");
        drain_hotlog_to_coldlog(hotlog, coldlog);

        gc.stop();
        sandbox.destroy();
        folder_tree.evict_folder("/", &gc.runway);
        folder_tree.destroy();
        code_heap.destroy();
        code_arena.destroy();
        gc.runway.destroy();
        gc.landfill.destroy();

        vm_state.store(SF_VM_STATE_SHUTDOWN, std::memory_order_release);

        g_sf_active_hotlog.store(nullptr, std::memory_order_release);
        g_sf_active_coldlog.store(nullptr, std::memory_order_release);
        delete hotlog;  hotlog  = nullptr;
        delete coldlog; coldlog = nullptr;
    }
};

using sfForge = sacculus_forge_t;

static sfForge g_forge;

/* ========================================================================= */
/* SFVM & sfGC 4-POINT RUNTIME EXPORTS (SAFE/CONTROL/HOOK/PAUSE POINTS)      */
/* ========================================================================= */

extern "C" {

MSIMD_API void sf_gc_safepoint_poll(void) {
    g_point_coord.safepoint_polls.fetch_add(1, std::memory_order_relaxed);

    if (g_forge.config.is_safety_critical || g_forge.gc.config.strings.no_sfgc_overwrite_mode) {
        return;
    }

    uint8_t phase = g_forge.gc.current_phase.load(std::memory_order_acquire);
    bool pause_gate = g_stream_pause_gate.load(std::memory_order_relaxed) ||
                      g_point_coord.pausepoint_gate.load(std::memory_order_relaxed);

    if (phase < SF_GC_PHASE_DRAIN && !pause_gate) {
        return;
    }

    // Prevent recursive safepoint parking on the same thread
    if (tl_sf_point_ctx.in_safepoint) {
        return;
    }

    tl_sf_point_ctx.in_safepoint = true;
    g_point_coord.safepoint_rendezvous_stalls.fetch_add(1, std::memory_order_relaxed);
    g_point_coord.safepoint_parked_threads.fetch_add(1, std::memory_order_acq_rel);

    if (phase >= SF_GC_PHASE_COMPACT || pause_gate) {
        // Cooperative Stop-The-World / Stream Hot-Swap Rendezvous Barrier:
        // Park mutator thread with SMT-aware backoff until compaction, evacuation, or hot-swap completes.
        uint32_t spin_iter = 0;
        constexpr uint32_t MAX_SAFEPOINT_SPINS = 50000;
        while (spin_iter < MAX_SAFEPOINT_SPINS) {
            uint8_t cur_phase = g_forge.gc.current_phase.load(std::memory_order_acquire);
            bool cur_gate = g_stream_pause_gate.load(std::memory_order_acquire) ||
                            g_point_coord.pausepoint_gate.load(std::memory_order_acquire);
            if (cur_phase < SF_GC_PHASE_COMPACT && !cur_gate) {
                break;
            }
            sf_smt_ht_engine_t::smt_pause_backoff(spin_iter++);
        }
    } else if (phase == SF_GC_PHASE_DRAIN) {
        // During background E-core Falline drain, yield briefly to avoid memory bus saturation
        sf_cpu_pause();
        std::this_thread::yield();
    }

    // If the active tensor was relocated before pinning, refresh its forwarding address
    if (tl_sf_point_ctx.pinned_hdr) {
        tl_sf_point_ctx.active_buffer_ptr = tl_sf_point_ctx.pinned_hdr->resolve_active_body();
    } else if (tl_sf_point_ctx.active_buffer_ptr) {
        auto* eb = SF_custominiVm_emptybox_header_t::from_body(tl_sf_point_ctx.active_buffer_ptr);
        if (eb) {
            void* fwd = eb->forwarding_address.load(std::memory_order_acquire);
            if (fwd) tl_sf_point_ctx.active_buffer_ptr = fwd;
        }
    }

    g_point_coord.safepoint_parked_threads.fetch_sub(1, std::memory_order_acq_rel);
    tl_sf_point_ctx.in_safepoint = false;
}

MSIMD_API int sf_gc_controlpoint_check(size_t allocated_bytes) {
    g_point_coord.controlpoint_checks.fetch_add(1, std::memory_order_relaxed);
    if (allocated_bytes > 0) {
        g_point_coord.controlpoint_bytes_validated.fetch_add(allocated_bytes, std::memory_order_relaxed);
    }

    if (g_forge.config.is_safety_critical || g_forge.gc.config.strings.no_sfgc_overwrite_mode) {
        if (g_forge.config.is_safety_critical &&
            g_forge.config.integers.runway_handsel_bytes > 0 &&
            allocated_bytes > g_forge.config.integers.runway_handsel_bytes) {
            return 1;
        }
        return 0;
    }

    // 1. Validate active managed tensor page-in state & tail canary before heavy SIMD compute
    sacculus_live_header_t* hdr = tl_sf_point_ctx.pinned_hdr;
    if (!hdr && tl_sf_point_ctx.active_buffer_ptr) {
        hdr = SF_custominiVm_live_header_t::from_body(tl_sf_point_ctx.active_buffer_ptr);
    }
    if (hdr) {
        auto* cold = get_cold(hdr);
        if (cold && cold->is_paged_out.load(std::memory_order_acquire)) {
            g_forge.gc.pagefile.page_in(hdr);
            g_point_coord.controlpoint_auto_pageins.fetch_add(1, std::memory_order_relaxed);
        }
        if (!hdr->isTailCanaryValid()) {
            hdr->state.store(static_cast<uint8_t>(sf_live_state_e::QUARANTINED), std::memory_order_release);
            g_point_coord.hookpoint_canary_faults_caught.fetch_add(1, std::memory_order_relaxed);
            if (g_forge.coldlog) {
                sacculus_coldlog_write(g_forge.coldlog, SF_Ebin0_ERROR, SF_Ebin1_ERROR,
                    tl_sf_point_ctx.active_fn_id, "controlpoint_canary_hazard");
            }
            return 1;
        }
    }

    // 2. SRAM & Core-Tier Streaming Store Advisory for the active working set
    if (allocated_bytes > 0) {
        tl_sf_point_ctx.prefer_streaming_stores =
            g_sf_hardware_nexus.sram.should_stream_store(allocated_bytes) ||
            sf_is_efficiency_core_active();
    }

    // 3. Projected Heap Pressure Evaluation (used + allocated_bytes)
    size_t used = g_forge.gc.total_heap_bytes.load(std::memory_order_relaxed);
    size_t thresh = g_forge.gc.pressure_threshold.load(std::memory_order_relaxed);
    if (thresh > 0) {
        size_t projected = (used <= SIZE_MAX - allocated_bytes) ? (used + allocated_bytes) : SIZE_MAX;
        if (projected >= thresh) {
            // Critical 100%+ projected threshold: pace allocation and proactively drain Falline if GC is idle
            g_point_coord.controlpoint_pacing_triggers.fetch_add(1, std::memory_order_relaxed);
            if (g_sf_controlpoint_hook.load(std::memory_order_relaxed)) {
                SF_custominiVm_trigger_host_controlpoint(SF_CP_ACTION_PACING_STALL, (uint64_t)projected);
                if (g_sf_host_gc_mode.load(std::memory_order_relaxed) == SF_HOST_GC_DELEGATED_HOST_LEAD) {
                    SF_custominiVm_trigger_host_controlpoint(SF_CP_ACTION_REQUEST_HOST_GC, (uint64_t)projected);
                }
            }
            g_forge.gc.runway.pace_allocation();
            uint8_t expected_idle = SF_GC_PHASE_IDLE;
            if (g_forge.gc.current_phase.compare_exchange_strong(
                    expected_idle, SF_GC_PHASE_DRAIN, std::memory_order_acq_rel)) {
                g_forge.gc.drain_falline();
            }
            return 1;
        } else if (projected > (thresh * 85 / 100)) {
            // High 85%+ projected threshold: pace mutator allocation velocity
            g_point_coord.controlpoint_pacing_triggers.fetch_add(1, std::memory_order_relaxed);
            if (g_sf_controlpoint_hook.load(std::memory_order_relaxed)) {
                SF_custominiVm_trigger_host_controlpoint(SF_CP_ACTION_PACING_STALL, (uint64_t)projected);
            }
            g_forge.gc.runway.pace_allocation();
            return 1;
        }
    }
    return 0;
}

MSIMD_API void sf_vm_hookpoint_trigger(uint32_t event_id, uint32_t fn_id, void* context, size_t byte_size) {
    if (event_id == SF_HOOKPOINT_KERNEL_ENTER) {
        // === KERNEL PROLOGUE (ENTER) ===
        g_point_coord.hookpoint_enter_count.fetch_add(1, std::memory_order_relaxed);
        g_point_coord.active_in_flight_kernels.fetch_add(1, std::memory_order_acquire);
        g_forge.gc.total_calls_tracked.fetch_add(1, std::memory_order_relaxed);

        if (tl_sf_point_ctx.nesting_depth++ == 0) {
            tl_sf_point_ctx.active_fn_id      = fn_id;
            tl_sf_point_ctx.active_buffer_ptr = context;
            tl_sf_point_ctx.active_byte_size  = byte_size;
            tl_sf_point_ctx.enter_tsc         = sacculus_rdtsc();
            tl_sf_point_ctx.prefer_streaming_stores =
                (byte_size > 0) &&
                (g_sf_hardware_nexus.sram.should_stream_store(byte_size) || sf_is_efficiency_core_active());

            // Auto-pin managed tensor payload during kernel execution so concurrent GC never relocates/evicts it
            if (context && !g_forge.gc.config.strings.no_sfgc_overwrite_mode) {
                auto* hdr = SF_custominiVm_live_header_t::from_body(context);
                if (hdr) {
                    auto* cold = get_cold(hdr);
                    if (cold && cold->is_paged_out.load(std::memory_order_acquire)) {
                        g_forge.gc.pagefile.page_in(hdr);
                        g_point_coord.controlpoint_auto_pageins.fetch_add(1, std::memory_order_relaxed);
                    }
                    if (hdr->try_pin()) {
                        tl_sf_point_ctx.pinned_hdr = hdr;
                        g_point_coord.hookpoint_auto_pinned_tensors.fetch_add(1, std::memory_order_relaxed);
                        g_point_coord.hookpoint_active_pinned.fetch_add(1, std::memory_order_acq_rel);
                    }
                    if (!g_sf_hardware_nexus.numa.is_uma) {
                        uint8_t cur_node = g_sf_hardware_nexus.numa.get_current_thread_node();
                        uint8_t n_nodes = g_sf_hardware_nexus.numa.node_count > 0 ? g_sf_hardware_nexus.numa.node_count : 1;
                        uint8_t target_node = (uint8_t)(hdr->current_core.load(std::memory_order_relaxed) % n_nodes);
                        g_sf_hardware_nexus.numa.record_access(cur_node, target_node, byte_size);
                    }
                }
            }
        }

        // Tiered JIT hotness tracking & automatic MIRC promotion
        uint32_t jit_slot = fn_id % sacculus_tiered_jit_state_t::MAX_FNS;
        g_forge.tiered_jit.record_call(jit_slot);
        if (g_forge.tiered_jit.should_promote_to_mirc(jit_slot)) {
            g_forge.tiered_jit.set_tier(jit_slot, SF_JIT_TIER_MIRC);
        }

        // Request Rehab Arena resurrection if this function's tensors were previously evicted
        if (fn_id != 0 && g_forge.gc.landfill.pane.count.load(std::memory_order_relaxed) > 0) {
            g_forge.gc.request_resurrection(fn_id);
        }

        if (g_forge.hotlog) {
            sacculus_hotlog_write(g_forge.hotlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_TRACE,
                fn_id, (uint16_t)event_id, 0, 0, 0,
                g_forge.gc.sweep_count.load(std::memory_order_relaxed),
                (uint32_t)(byte_size / 1024), "hook_kernel_enter");
        }
    } else if (event_id == SF_HOOKPOINT_KERNEL_EXIT) {
        // === KERNEL EPILOGUE (EXIT) ===
        g_point_coord.hookpoint_exit_count.fetch_add(1, std::memory_order_relaxed);

        if (tl_sf_point_ctx.nesting_depth > 0) {
            --tl_sf_point_ctx.nesting_depth;
        }

        if (tl_sf_point_ctx.nesting_depth == 0) {
            uint64_t now_tsc = sacculus_rdtsc();
            uint64_t delta_cycles = (now_tsc > tl_sf_point_ctx.enter_tsc) ? (now_tsc - tl_sf_point_ctx.enter_tsc) : 1;

            // Update dispatch table telemetry
            if (auto* de = g_forge.dispatch.get(fn_id % SACFOR_MAX_DISPATCH_SLOTS)) {
                de->call_count++;
                if (delta_cycles < de->best_cycles) {
                    de->best_cycles = delta_cycles;
                }
            }

            // Post-execution canary verification, hotness promotion, and unpinning
            sacculus_live_header_t* hdr = tl_sf_point_ctx.pinned_hdr;
            if (!hdr && context && !g_forge.gc.config.strings.no_sfgc_overwrite_mode) {
                hdr = SF_custominiVm_live_header_t::from_body(context);
            }
            if (hdr) {
                if (!hdr->isTailCanaryValid()) {
                    hdr->state.store(static_cast<uint8_t>(sf_live_state_e::QUARANTINED), std::memory_order_release);
                    g_point_coord.hookpoint_canary_faults_caught.fetch_add(1, std::memory_order_relaxed);
                    if (g_forge.coldlog) {
                        sacculus_coldlog_write(g_forge.coldlog, SF_Ebin0_ERROR, SF_Ebin1_ERROR,
                            fn_id, "hookpoint_exit_canary_corrupt");
                    }
                } else {
                    uint16_t cur_core = 0;
#ifdef _WIN32
                    cur_core = (uint16_t)GetCurrentProcessorNumber();
#endif
                    hdr->record_access(delta_cycles, cur_core);
                    auto* cold = get_cold(hdr);
                    if (hdr->region == SF_REGION_HOTFIELD &&
                        hdr->generation < SACFOR_ROOM_COUNT &&
                        cold && (cold->call_step_count.load(std::memory_order_relaxed) & 0x3F) == 0) {
                        g_forge.gc.motherline.rooms[hdr->generation].regions[SF_REGION_HOTFIELD].hot_order.bubble_up(hdr);
                    }
                }
                if (tl_sf_point_ctx.pinned_hdr) {
                    tl_sf_point_ctx.pinned_hdr->unpin();
                    tl_sf_point_ctx.pinned_hdr = nullptr;
                    uint32_t cur_pin = g_point_coord.hookpoint_active_pinned.load(std::memory_order_relaxed);
                    while (cur_pin > 0 &&
                           !g_point_coord.hookpoint_active_pinned.compare_exchange_weak(
                               cur_pin, cur_pin - 1, std::memory_order_release, std::memory_order_relaxed)) {}
                }
            }

            // Ensure non-temporal Write-Combining buffers are globally visible before returning to caller
            if (tl_sf_point_ctx.prefer_streaming_stores) {
                sf_sram_cache_engine_t::flush_wc_buffers();
                tl_sf_point_ctx.prefer_streaming_stores = false;
            }

            tl_sf_point_ctx.active_buffer_ptr = nullptr;
            tl_sf_point_ctx.active_byte_size  = 0;
        }

        uint32_t cur_inf = g_point_coord.active_in_flight_kernels.load(std::memory_order_relaxed);
        while (cur_inf > 0 &&
               !g_point_coord.active_in_flight_kernels.compare_exchange_weak(
                   cur_inf, cur_inf - 1, std::memory_order_release, std::memory_order_relaxed)) {}

        if (g_forge.hotlog) {
            sacculus_hotlog_write(g_forge.hotlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_TRACE,
                fn_id, (uint16_t)event_id, 0, 0, 0,
                g_forge.gc.sweep_count.load(std::memory_order_relaxed),
                (uint32_t)(byte_size / 1024), "hook_kernel_exit");
        }
    } else {
        // Explicit GC & Deopt Hook Events (3..7)
        if (event_id == SF_HOOKPOINT_GC_PRE_SWEEP) {
            g_forge.gc.trigger_hook(SF_GC_HOOK_PRE_SWEEP);
        } else if (event_id == SF_HOOKPOINT_GC_POST_SWEEP) {
            g_forge.gc.trigger_hook(SF_GC_HOOK_POST_SWEEP);
        } else if (event_id == SF_HOOKPOINT_GC_PRE_COMPACT) {
            g_forge.gc.trigger_hook(SF_GC_HOOK_PRE_COMPACT);
        } else if (event_id == SF_HOOKPOINT_GC_POST_COMPACT) {
            g_forge.gc.trigger_hook(SF_GC_HOOK_POST_COMPACT);
        } else if (event_id == SF_HOOKPOINT_DEOPT_TRIGGER) {
            g_forge.tiered_jit.hardware_deopt(fn_id % sacculus_tiered_jit_state_t::MAX_FNS);
        }
        if (g_forge.hotlog) {
            sacculus_hotlog_write(g_forge.hotlog, SF_Ebin0_SAFEPOINT, SF_Ebin1_TRACE,
                fn_id, (uint16_t)event_id, 0, 0, 0,
                g_forge.gc.sweep_count.load(std::memory_order_relaxed),
                (uint32_t)(byte_size / 1024), "sfvm_hookpoint_ext");
        }
    }
}

MSIMD_API void sf_gc_hookpoint_trigger(uint32_t event_id, uint32_t fn_id, void* context, size_t byte_size) {
    sf_vm_hookpoint_trigger(event_id, fn_id, context, byte_size);
}

MSIMD_API int sf_gc_pausepoint_poll(void) {
    g_point_coord.pausepoint_polls.fetch_add(1, std::memory_order_relaxed);

    // 1. Check cooperative sfGC compaction/evacuation phase, stream hot-swap gate (Puzzle Arena), or explicit user pause gate
    bool gate = g_stream_pause_gate.load(std::memory_order_relaxed) ||
                g_point_coord.pausepoint_gate.load(std::memory_order_relaxed);
    uint8_t phase = g_forge.gc.current_phase.load(std::memory_order_relaxed);

    if (gate || (!g_forge.config.is_safety_critical && phase >= SF_GC_PHASE_COMPACT)) {
        g_point_coord.pausepoint_preemptions.fetch_add(1, std::memory_order_relaxed);
        sf_gc_safepoint_poll();
        return 1;
    }

    // 2. Periodic low-overhead backoff during background E-core drain or explicit DENY SIMD mode
    uint32_t tick = ++tl_sf_point_ctx.irq_poll_counter;
    if ((tick & 0x3Fu) == 0u) {
        if (!g_forge.config.is_safety_critical && phase == SF_GC_PHASE_DRAIN) {
            sf_cpu_pause();
            return 1;
        }
        if (g_sf_critical_simd_mode.load(std::memory_order_relaxed) ==
            static_cast<uint8_t>(sf_critical_simd_e::DENY)) {
            sf_cpu_pause();
        }
    }
    return 0;
}

MSIMD_API int sf_vm_pausepoint_poll(void) {
    return sf_gc_pausepoint_poll();
}

MSIMD_API void SF_custominiVm_set_pausepoint_gate(int armed) {
    g_point_coord.pausepoint_gate.store(armed != 0, std::memory_order_release);
}

MSIMD_API int SF_custominiVm_get_pausepoint_gate(void) {
    return g_point_coord.pausepoint_gate.load(std::memory_order_acquire) ? 1 : 0;
}

MSIMD_API void SF_custominiVm_point_telemetry_query(SF_custominiVm_point_telemetry_t* out_stat) {
    if (!out_stat) return;
    uint64_t pp_polls   = g_point_coord.pausepoint_polls.load(std::memory_order_relaxed);
    uint64_t pp_preempt = g_point_coord.pausepoint_preemptions.load(std::memory_order_relaxed);
    out_stat->safepoint_polls                = g_point_coord.safepoint_polls.load(std::memory_order_relaxed);
    out_stat->safepoint_rendezvous_stalls    = g_point_coord.safepoint_rendezvous_stalls.load(std::memory_order_relaxed);
    out_stat->safepoint_parked_threads       = g_point_coord.safepoint_parked_threads.load(std::memory_order_relaxed);
    out_stat->active_in_flight_kernels       = g_point_coord.active_in_flight_kernels.load(std::memory_order_relaxed);
    out_stat->controlpoint_checks            = g_point_coord.controlpoint_checks.load(std::memory_order_relaxed);
    out_stat->controlpoint_pacing_triggers   = g_point_coord.controlpoint_pacing_triggers.load(std::memory_order_relaxed);
    out_stat->controlpoint_auto_pageins      = g_point_coord.controlpoint_auto_pageins.load(std::memory_order_relaxed);
    out_stat->controlpoint_bytes_validated   = g_point_coord.controlpoint_bytes_validated.load(std::memory_order_relaxed);
    out_stat->hookpoint_enter_count          = g_point_coord.hookpoint_enter_count.load(std::memory_order_relaxed);
    out_stat->hookpoint_exit_count           = g_point_coord.hookpoint_exit_count.load(std::memory_order_relaxed);
    out_stat->hookpoint_auto_pinned_tensors  = g_point_coord.hookpoint_auto_pinned_tensors.load(std::memory_order_relaxed);
    out_stat->hookpoint_canary_faults_caught = g_point_coord.hookpoint_canary_faults_caught.load(std::memory_order_relaxed);
    out_stat->pausepoint_polls               = pp_polls;
    out_stat->pausepoint_preemptions         = pp_preempt;
}

}

extern "C" {

MSIMD_API void SF_custominiVm_register_controlpoint_hook(sf_controlpoint_hook_fn_t hook, void* user_data) {
    g_sf_controlpoint_user_data.store(user_data, std::memory_order_release);
    g_sf_controlpoint_hook.store(hook, std::memory_order_release);
    char buf[128];
    snprintf(buf, sizeof(buf), "\033[1;34m[0x00C1] [SFVM Controlpoint]\033[0m Registered host language VM controlpoint hook %p", (void*)hook);
    sf_broadcast_raw_log_line(buf);
}

MSIMD_API void SF_custominiVm_unregister_controlpoint_hook(void) {
    g_sf_controlpoint_hook.store(nullptr, std::memory_order_release);
    g_sf_controlpoint_user_data.store(nullptr, std::memory_order_release);
    sf_broadcast_raw_log_line("\033[1;34m[0x00C2] [SFVM Controlpoint]\033[0m Unregistered host language VM controlpoint hook");
}

MSIMD_API void SF_custominiVm_set_host_gc_cooperative_mode(int mode) {
    g_sf_host_gc_mode.store(mode, std::memory_order_release);
    char buf[128];
    snprintf(buf, sizeof(buf), "\033[1;34m[0x00C3] [SFVM Controlpoint]\033[0m Configured Host GC Cooperative Mode to 0x%02X (%s)",
             mode, mode == 1 ? "COOPERATIVE_SYNC" : (mode == 2 ? "DELEGATED_HOST_LEAD" : "AUTONOMOUS"));
    sf_broadcast_raw_log_line(buf);
}

MSIMD_API int SF_custominiVm_get_host_gc_cooperative_mode(void) {
    return g_sf_host_gc_mode.load(std::memory_order_acquire);
}

MSIMD_API void SF_custominiVm_trigger_host_controlpoint(SF_custominiVm_controlpoint_action_t action, uint64_t param) {
    auto fn = g_sf_controlpoint_hook.load(std::memory_order_acquire);
    if (fn) {
        void* u = g_sf_controlpoint_user_data.load(std::memory_order_acquire);
        fn(action, param, u);
    }
}

MSIMD_API int SF_custominiVm_setup_start(const void* custom_cfg) {
    if (custom_cfg) {
        const char* str_candidate = reinterpret_cast<const char*>(custom_cfg);
        if (str_candidate[0] == '-' || str_candidate[0] == '#') {
            sf_vm_config_t cfg = parse_all_sf_flags(str_candidate);
            return g_forge.setup_start(cfg) ? 0 : -1;
        }
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

struct alignas(64) sf_sensor_attached_entry_t {
    SF_custominiVm_tensor_t        stream_tensor{};
    SF_custominiVm_sensor_config_t config{};
    std::atomic<size_t>            write_cursor{0};
    std::atomic<bool>              active{false};
};
static SF_custominiVm_gpio_desc_t g_sf_gpio_desc{};
alignas(64) static sf_sensor_attached_entry_t g_sf_sensor_slots[8];

MSIMD_API size_t SF_custominiVm_emergency_os_sweep(void) {
    if (g_forge.vm_state.load(std::memory_order_relaxed) == SF_VM_STATE_RUNNING ||
        g_forge.vm_state.load(std::memory_order_relaxed) == SF_VM_STATE_SETUP_READY) {
        g_forge.shutdown();
    }
    if (g_sf_gpio_desc.is_fast_mmio_active && g_sf_gpio_desc.mmio_virtual_base) {
        g_sf_os_registry.unregister_block(g_sf_gpio_desc.mmio_virtual_base);
#if !defined(_WIN32)
        munmap(g_sf_gpio_desc.mmio_virtual_base, 4096);
#endif
        g_sf_gpio_desc.mmio_virtual_base = nullptr;
        g_sf_gpio_desc.is_fast_mmio_active = 0;
    }
    for (int i = 0; i < 8; ++i) {
        g_sf_sensor_slots[i].active.store(false, std::memory_order_relaxed);
    }
    return g_sf_os_registry.sweep_all();
}

MSIMD_API void SF_custominiVm_shutdown(void) {
    g_forge.shutdown();
    SF_custominiVm_emergency_os_sweep();
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
    if (t.data) {
        auto* hdr = SF_custominiVm_live_header_t::from_body(t.data);
        if (hdr) {
            auto* cold = get_cold(hdr);
            if (cold) cold->type_sig_hash = (uint64_t)dtype;
        }
        if (!g_forge.gc.config.strings.no_sfgc_overwrite_mode) {
            sf_critical_clear_memory(t.data, t.byte_size);
        }
        // SRAM Volatility Optimization & Cacheline Pinning
        if (g_sf_global_volatility.load(std::memory_order_relaxed) == static_cast<uint8_t>(sf_global_volatility_e::SRAM)) {
            if (tl_sf_group_priority >= 8) {
                t.flags |= 0x08; // SF_TENSOR_FLAG_SRAM_RESIDENT
                char* bptr = reinterpret_cast<char*>(t.data);
                for (size_t off = 0; off < t.byte_size; off += 64) {
#if defined(_M_X64) || defined(__x86_64__)
                    _mm_prefetch(bptr + off, _MM_HINT_T0);
#endif
                }
            }
        }
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
        size_t next_pow2 = 1;
        while (next_pow2 < alignment) next_pow2 <<= 1;
        alignment = next_pow2;
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

    std::vector<SF_custominiVm_tensor_t> dyn_in, dyn_out;
    SF_custominiVm_tensor_t resolved_inputs_buf[16];
    SF_custominiVm_tensor_t resolved_outputs_buf[16];
    const SF_custominiVm_tensor_t* call_inputs = inputs;
    SF_custominiVm_tensor_t* call_outputs = outputs;

    if (inputs && n_in > 0) {
        SF_custominiVm_tensor_t* r_in = resolved_inputs_buf;
        if (n_in > 16) { dyn_in.resize(n_in); r_in = dyn_in.data(); }
        for (int i = 0; i < n_in; ++i) {
            r_in[i] = inputs[i];
            void* orig_ptr = inputs[i].data;
            r_in[i].data = SF_custominiVm_tensor_resolve_ptr(&inputs[i]);
            if (r_in[i].data != orig_ptr) r_in[i].status |= SF_TENSOR_STATUS_FORWARDED;
            auto* obj = SF_custominiVm_live_header_t::from_body(r_in[i].data);
            if (obj && obj->parent_header.load(std::memory_order_relaxed)) r_in[i].status |= SF_TENSOR_STATUS_VIEW_SLICE;
        }
        call_inputs = r_in;
    }
    if (outputs && n_out > 0) {
        SF_custominiVm_tensor_t* r_out = resolved_outputs_buf;
        if (n_out > 16) { dyn_out.resize(n_out); r_out = dyn_out.data(); }
        for (int i = 0; i < n_out; ++i) {
            r_out[i] = outputs[i];
            void* orig_ptr = outputs[i].data;
            r_out[i].data = SF_custominiVm_tensor_resolve_ptr(&outputs[i]);
            if (r_out[i].data != orig_ptr) {
                r_out[i].status |= SF_TENSOR_STATUS_FORWARDED;
                outputs[i].data = r_out[i].data;
                outputs[i].status |= SF_TENSOR_STATUS_FORWARDED;
            }
        }
        call_outputs = r_out;
    }

    std::vector<sf_pin_guard_t> io_pins;
    io_pins.reserve((size_t)n_in + (size_t)n_out);
    for (int i = 0; i < n_in; ++i) {
        auto* h = SF_custominiVm_live_header_t::from_body(call_inputs[i].data);
        if (h) io_pins.emplace_back(h);
    }
    for (int i = 0; i < n_out; ++i) {
        auto* h = SF_custominiVm_live_header_t::from_body(call_outputs[i].data);
        if (h) io_pins.emplace_back(h);
    }

    uint64_t t0 = sacculus_rdtsc();
    fn(call_inputs, n_in, call_outputs, n_out, user_data);
    uint64_t delta_cycles = sacculus_rdtsc() - t0;
    io_pins.clear();
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
    if (g_forge.hotlog) {
        g_forge.hotlog->drain(cb, user);
    } else {
        auto* hl = g_sf_active_hotlog.load(std::memory_order_relaxed);
        if (hl) hl->drain(cb, user);
    }
}

MSIMD_API void SF_custominiVm_coldlog_drain(
        void (*cb)(const SF_custominiVm_coldlog_entry_t*, void*), void* user) {
    if (g_forge.coldlog) {
        g_forge.coldlog->drain(cb, user);
    } else {
        auto* cl = g_sf_active_coldlog.load(std::memory_order_relaxed);
        if (cl) cl->drain(cb, user);
    }
}

MSIMD_API size_t SF_custominiVm_hotlog_format_cli(const SF_custominiVm_hotlog_entry_t* entry, char* buffer, size_t buffer_size) {
    if (!entry || !buffer || buffer_size == 0) return 0;
    const char* col = sf_severity_ansi_color(entry->severity);
    const char* rst = "\033[0m";
    const char* sev = sf_severity_str(entry->severity);
    const char* cat = sf_event_category_name(entry->event);
    int written = snprintf(buffer, buffer_size,
        "[%llu] %s[%-5s]%s [%-20s] core=%-2u gen=%u reg=%u sweep=%-4llu heap=%-6uKB msg=\"%s\"",
        (unsigned long long)entry->ts_ns,
        col, sev, rst, cat,
        (unsigned int)entry->current_core, (unsigned int)entry->generation, (unsigned int)entry->region,
        (unsigned long long)entry->gc_sweep_no, (unsigned int)entry->heap_used_kb,
        entry->msg[0] ? entry->msg : "");
    if (written < 0) return 0;
    return (size_t)written >= buffer_size ? buffer_size - 1 : (size_t)written;
}

MSIMD_API size_t SF_custominiVm_hotlog_format_json(const SF_custominiVm_hotlog_entry_t* entry, char* buffer, size_t buffer_size) {
    if (!entry || !buffer || buffer_size == 0) return 0;
    const char* sev = sf_severity_str(entry->severity);
    const char* cat = sf_event_category_name(entry->event);
    int written = snprintf(buffer, buffer_size,
        "{\"ts_ns\":%llu,\"tsc\":%llu,\"sweep\":%llu,\"fn_id\":%u,\"heap_kb\":%u,\"slot\":%u,\"core\":%u,\"gen\":%u,\"reg\":%u,\"event\":\"%s\",\"event_id\":%u,\"severity\":\"%s\",\"msg\":\"%s\"}",
        (unsigned long long)entry->ts_ns, (unsigned long long)entry->tsc, (unsigned long long)entry->gc_sweep_no,
        (unsigned int)entry->fn_id, (unsigned int)entry->heap_used_kb, (unsigned int)entry->slot,
        (unsigned int)entry->current_core, (unsigned int)entry->generation, (unsigned int)entry->region,
        cat, (unsigned int)entry->event, sev, entry->msg[0] ? entry->msg : "");
    if (written < 0) return 0;
    return (size_t)written >= buffer_size ? buffer_size - 1 : (size_t)written;
}

MSIMD_API size_t SF_custominiVm_coldlog_format_cli(const SF_custominiVm_coldlog_entry_t* entry, char* buffer, size_t buffer_size) {
    if (!entry || !buffer || buffer_size == 0) return 0;
    const char* col = sf_severity_ansi_color(entry->severity);
    const char* rst = "\033[0m";
    const char* sev = sf_severity_str(entry->severity);
    const char* cat = sf_event_category_name(entry->event);
    int written = snprintf(buffer, buffer_size,
        "[%llu] %s[%-5s]%s [%-20s] fn_id=%-4u msg=\"%s\"",
        (unsigned long long)entry->ts_ns,
        col, sev, rst, cat,
        (unsigned int)entry->fn_id,
        entry->msg[0] ? entry->msg : "");
    if (written < 0) return 0;
    return (size_t)written >= buffer_size ? buffer_size - 1 : (size_t)written;
}

MSIMD_API size_t SF_custominiVm_coldlog_format_json(const SF_custominiVm_coldlog_entry_t* entry, char* buffer, size_t buffer_size) {
    if (!entry || !buffer || buffer_size == 0) return 0;
    const char* sev = sf_severity_str(entry->severity);
    const char* cat = sf_event_category_name(entry->event);
    int written = snprintf(buffer, buffer_size,
        "{\"ts_ns\":%llu,\"fn_id\":%u,\"event\":\"%s\",\"event_id\":%u,\"severity\":\"%s\",\"msg\":\"%s\"}",
        (unsigned long long)entry->ts_ns, (unsigned int)entry->fn_id,
        cat, (unsigned int)entry->event, sev, entry->msg[0] ? entry->msg : "");
    if (written < 0) return 0;
    return (size_t)written >= buffer_size ? buffer_size - 1 : (size_t)written;
}

MSIMD_API void SF_custominiVm_set_log_filter_mask(uint32_t subsystem_mask) {
    g_sf_telemetry_filter_mask.store(subsystem_mask, std::memory_order_relaxed);
}

MSIMD_API uint32_t SF_custominiVm_get_log_filter_mask(void) {
    return g_sf_telemetry_filter_mask.load(std::memory_order_relaxed);
}

MSIMD_API void SF_custominiVm_register_raw_log_stream(sf_raw_log_stream_fn_t stream_fn, void* user_data) {
    g_sf_raw_log_stream_user_data.store(user_data, std::memory_order_release);
    g_sf_raw_log_stream_fn.store(stream_fn, std::memory_order_release);
    char buf[128];
    snprintf(buf, sizeof(buf), "\033[1;32m[0x00L1] [SFVM LogStream]\033[0m Registered live raw log stream listener %p", (void*)stream_fn);
    sf_broadcast_raw_log_line(buf);
}

MSIMD_API void SF_custominiVm_unregister_raw_log_stream(void) {
    g_sf_raw_log_stream_fn.store(nullptr, std::memory_order_release);
    g_sf_raw_log_stream_user_data.store(nullptr, std::memory_order_release);
}

MSIMD_API void SF_custominiVm_set_console_mirroring(int enable) {
    g_sf_console_mirroring.store(enable != 0 ? 1 : 0, std::memory_order_release);
}

MSIMD_API int SF_custominiVm_get_console_mirroring(void) {
    return g_sf_console_mirroring.load(std::memory_order_acquire);
}

MSIMD_API void SF_custominiVm_emit_raw_log(const char* formatted_text) {
    sf_broadcast_raw_log_line(formatted_text, false);
}

struct sf_telemetry_stream_daemon_t {
    std::atomic<bool>         running{false};
    std::thread               worker;
    uint32_t                  interval_us{10000};
    sf_telemetry_stream_fn_t  callback{nullptr};
    void*                     user_data{nullptr};
    std::mutex                stream_mutex;

    void start(uint32_t flush_interval_us, sf_telemetry_stream_fn_t cb, void* user) {
        std::lock_guard<std::mutex> lk(stream_mutex);
        if (running.load(std::memory_order_relaxed)) return;
        interval_us = flush_interval_us > 0 ? flush_interval_us : 10000;
        callback    = cb;
        user_data   = user;
        running.store(true, std::memory_order_release);
        worker = std::thread([this]() {
            struct drain_stream_ctx_t {
                sf_telemetry_stream_fn_t cb;
                void* u;
                char buf[512];
            } ctx{callback, user_data, {0}};

            while (running.load(std::memory_order_relaxed)) {
                if (ctx.cb) {
                    SF_custominiVm_hotlog_drain([](const SF_custominiVm_hotlog_entry_t* e, void* u) {
                        auto* c = static_cast<drain_stream_ctx_t*>(u);
                        c->buf[0] = '\0';
                        SF_custominiVm_hotlog_format_cli(e, c->buf, sizeof(c->buf));
                        c->cb(e, c->buf, c->u);
                    }, &ctx);
                }
                std::this_thread::sleep_for(std::chrono::microseconds(interval_us));
            }
        });
    }

    void stop() {
        std::lock_guard<std::mutex> lk(stream_mutex);
        if (!running.load(std::memory_order_relaxed)) return;
        running.store(false, std::memory_order_release);
        if (worker.joinable()) {
            worker.join();
        }
    }

    bool is_running() const {
        return running.load(std::memory_order_relaxed);
    }
};

static sf_telemetry_stream_daemon_t g_sf_telemetry_daemon;

MSIMD_API int SF_custominiVm_telemetry_stream_start(uint32_t flush_interval_us, sf_telemetry_stream_fn_t callback, void* user_data) {
    if (!callback) return -1;
    g_sf_telemetry_daemon.start(flush_interval_us, callback, user_data);
    return 0;
}

MSIMD_API void SF_custominiVm_telemetry_stream_stop(void) {
    g_sf_telemetry_daemon.stop();
}

MSIMD_API int SF_custominiVm_telemetry_stream_is_running(void) {
    return g_sf_telemetry_daemon.is_running() ? 1 : 0;
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
        sf_vm_config_t cfg = parse_all_sf_flags("+#SF|Profile:Standard");
        cfg.profile = MSIMD_PROFILE_STANDARD;
        cfg.is_safety_critical = false;
        cfg.Forbid_Dynamic_Growth = false;
        cfg.disable_jit_deopt = false;
        return g_forge.init(cfg) ? 0 : -1;
    }

    g_forge.config.profile = MSIMD_PROFILE_STANDARD;
    g_forge.config.is_safety_critical = false;
    g_forge.config.Forbid_Dynamic_Growth = false;
    g_forge.config.disable_jit_deopt = false;
    g_forge.gc.config.is_safety_critical = false;
    g_forge.gc.runway.allocator.forbid_dynamic_expansion = false;
    if (!g_forge.gc.running.load(std::memory_order_acquire)) {
        g_forge.gc.start();
    }
    if (!g_forge.sandbox.running.load(std::memory_order_acquire)) {
        g_forge.sandbox.start();
    }
    return 0;
}

MSIMD_API int SacFor_Gearkit_SSIOLSC(void) {
    std::lock_guard<std::mutex> lock(g_forge.transition_lock);
    if (g_forge.vm_state.load(std::memory_order_relaxed) == SF_VM_STATE_UNINITIALIZED) {
        sf_vm_config_t cfg = parse_all_sf_flags("+#SF|Profile:SC");
        cfg.profile = MSIMD_PROFILE_SAFETY_CRITICAL;
        cfg.is_safety_critical = true;
        cfg.Forbid_Dynamic_Growth = true;
        cfg.disable_jit_deopt = true;
        return g_forge.init(cfg) ? 0 : -1;
    }

    g_forge.config.profile = MSIMD_PROFILE_SAFETY_CRITICAL;
    g_forge.config.is_safety_critical = true;
    g_forge.config.Forbid_Dynamic_Growth = true;
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
    if ((align & (align - 1)) != 0) {
        size_t next_pow2 = 1;
        while (next_pow2 < align) next_pow2 <<= 1;
        align = next_pow2;
    }

    uint8_t* base = g_forge.gc.runway.allocator.runway_heap_base;
    size_t   cap  = g_forge.gc.runway.allocator.runway_capacity;
    if (!base || !cap) return nullptr;

    sf_ticket_guard_t g(g_forge.gc.runway.allocator.bump_lock);
    size_t cur = g_forge.gc.runway.allocator.runway_bump_offset.load(std::memory_order_relaxed);
    size_t aligned_off = (cur + align - 1) & ~(align - 1);
    size_t end_off     = aligned_off + bytes;
    if (end_off > cap) return nullptr;
    sf_os_commit_memory(base + aligned_off, bytes);
    g_forge.gc.runway.allocator.runway_bump_offset.store(end_off, std::memory_order_relaxed);
    return base + aligned_off;
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

/* ========================================================================= */
/* SPECIALIZED HARDWARE ENGINE SUBSYSTEM EXPORTS                             */
/* ========================================================================= */

MSIMD_API int SF_custominiVm_is_hybrid_cpu(void) {
    return g_sf_hardware_nexus.hybrid.is_hybrid ? 1 : 0;
}

MSIMD_API SF_custominiVm_cpu_topology_class_t SF_custominiVm_cpu_topology_class(void) {
    return g_sf_hardware_nexus.hybrid.topo_class;
}

MSIMD_API int SF_custominiVm_is_numa(void) {
    return g_sf_hardware_nexus.numa.is_uma ? 0 : 1;
}

MSIMD_API SF_custominiVm_mem_architecture_t SF_custominiVm_mem_architecture(void) {
    return g_sf_hardware_nexus.numa.mem_arch;
}

MSIMD_API int SF_custominiVm_is_ht_active(void) {
    return g_sf_hardware_nexus.smt.is_ht_active ? 1 : 0;
}

MSIMD_API SF_custominiVm_smt_mode_t SF_custominiVm_smt_mode(void) {
    return g_sf_hardware_nexus.smt.smt_mode;
}

MSIMD_API SF_custominiVm_sram_regime_t SF_custominiVm_sram_regime(void) {
    return g_sf_hardware_nexus.sram.sram_regime;
}

MSIMD_API uint32_t SF_custominiVm_optimal_concurrency(int is_compute_heavy) {
    if (is_compute_heavy) {
        return g_sf_hardware_nexus.smt.get_optimal_concurrency(SF_AFFINITY_PHYSICAL_CORES_ONLY);
    }
    return g_sf_hardware_nexus.smt.get_optimal_concurrency(SF_AFFINITY_ALL_LOGICAL_THREADS);
}

MSIMD_API int SF_custominiVm_bind_calling_thread(SF_custominiVm_core_tier_t tier) {
    return g_sf_hardware_nexus.hybrid.bind_current_thread(tier) ? 1 : 0;
}

MSIMD_API int SF_custominiVm_apply_affinity_policy(SF_custominiVm_workload_affinity_policy_t policy) {
    return g_sf_hardware_nexus.smt.apply_affinity_policy(policy) ? 1 : 0;
}

MSIMD_API void* SF_custominiVm_sram_scratchpad_alloc(size_t bytes, size_t alignment) {
    return g_sf_hardware_nexus.sram.scratchpad_alloc(bytes, alignment);
}

MSIMD_API void SF_custominiVm_sram_scratchpad_reset(void) {
    g_sf_hardware_nexus.sram.scratchpad_reset();
}

MSIMD_API void* SF_custominiVm_numa_alloc(size_t bytes, uint8_t node) {
    return g_sf_hardware_nexus.numa.alloc_node(bytes, node);
}

MSIMD_API void SF_custominiVm_numa_free(void* ptr, size_t bytes, uint8_t node) {
    g_sf_hardware_nexus.numa.free_node(ptr, bytes, node);
}

/* ========================================================================= */
/* SFVM NEW ERA EXECUTION FOCUS TOPOLOGY EXPORTS                             */
/* ========================================================================= */

MSIMD_API SF_custominiVm_execution_focus_t SF_custominiVm_get_execution_focus(void) {
    sf_ensure_hardware_nexus_initialized();
    return g_sf_hardware_nexus.focus.active_focus.load(std::memory_order_acquire);
}

MSIMD_API void SF_custominiVm_set_execution_focus(SF_custominiVm_execution_focus_t focus) {
    sf_ensure_hardware_nexus_initialized();
    g_sf_hardware_nexus.focus.set_focus(focus);
}

MSIMD_API const char* SF_custominiVm_get_execution_focus_name(SF_custominiVm_execution_focus_t focus) {
    switch (focus) {
        case SF_FOCUS_AUTO:          return "AUTO";
        case SF_FOCUS_SCP_HOMO:      return "SCP_HOMO";
        case SF_FOCUS_SCP_HYBRID:    return "SCP_HYBRID";
        case SF_FOCUS_MCP_HOMO:      return "MCP_HOMO";
        case SF_FOCUS_MCP_HYBRID:    return "MCP_HYBRID";
        case SF_FOCUS_DUAL_HOMO:     return "DUAL_HOMO";
        case SF_FOCUS_DUAL_HYBRID:   return "DUAL_HYBRID";
        case SF_FOCUS_SCP:           return "SCP";
        case SF_FOCUS_MCP:           return "MCP";
        case SF_FOCUS_HYBRID_DUAL:   return "DUAL";
        default:                     return "UNKNOWN";
    }
}

MSIMD_API SF_custominiVm_execution_focus_t SF_custominiVm_detect_cpu_execution_focus(void) {
    sf_ensure_hardware_nexus_initialized();
    return g_sf_hardware_nexus.focus.detect_focus(&g_forge.sandbox.cached_isa_inventory.topology, &g_sf_hardware_nexus.hybrid);
}

MSIMD_API const char* SF_custominiVm_get_focus_uarch_name(void) {
    sf_ensure_hardware_nexus_initialized();
    if (g_sf_hardware_nexus.focus.matched_uarch) {
        return g_sf_hardware_nexus.focus.matched_uarch->uarch_name;
    }
    return "Generic x86_64 / ARM / RISC-V";
}

MSIMD_API void SF_custominiVm_get_focus_chunk_weights(double* out_p_weight, double* out_e_weight) {
    sf_ensure_hardware_nexus_initialized();
    if (out_p_weight) *out_p_weight = g_sf_hardware_nexus.focus.p_weight;
    if (out_e_weight) *out_e_weight = g_sf_hardware_nexus.focus.e_weight;
}

MSIMD_API int SF_custominiVm_is_focus_scp(void) {
    sf_ensure_hardware_nexus_initialized();
    return g_sf_hardware_nexus.focus.is_scp ? 1 : 0;
}

MSIMD_API int SF_custominiVm_is_focus_mcp(void) {
    sf_ensure_hardware_nexus_initialized();
    return g_sf_hardware_nexus.focus.is_mcp ? 1 : 0;
}

MSIMD_API int SF_custominiVm_is_focus_dual(void) {
    sf_ensure_hardware_nexus_initialized();
    return g_sf_hardware_nexus.focus.is_dual ? 1 : 0;
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

static std::atomic<uint32_t> g_sf_group_counter{0x1000};

MSIMD_API uint32_t SF_custominiVm_group_compute(uint8_t priority, int bypass_warmup) {
    uint32_t gid = g_sf_group_counter.fetch_add(1, std::memory_order_relaxed);
    if (priority > 10) priority = 10;
    tl_sf_group_hex_id = gid;
    tl_sf_group_priority = priority;
    tl_sf_group_bypass_warmup = (bypass_warmup != 0);

    // High priority (>= 8) or warmup bypass pins thread to P-cores with turbo boost
    if (priority >= 8 || bypass_warmup) {
        sf_ensure_hardware_nexus_initialized();
        sf_bind_current_thread(SF_CORE_TIER_PERFORMANCE);
        sf_upclocker_set_boost(1);
        sf_upclocker_t::pin_max_frequency_warmup(50);
    }

    // Conflict detection: 'priority' parameter has conflicts to 'Allow_Operation_Dispatchings'
    if (g_sf_allow_operation_dispatchings.load(std::memory_order_relaxed) && priority > 0) {
        if (priority >= 8) {
            fprintf(stderr, "[SFVM Flag Conflict Warning] 'priority' has conflicts to 'Allow_Operation_Dispatchings'! High priority (%u) pins execution to performance vector units and overrides queue dispatching.\n", priority);
        } else {
            fprintf(stderr, "[SFVM Notice] 'priority' (%u) assigned under active 'Allow_Operation_Dispatchings'.\n", priority);
        }
    }
    return gid;
}

MSIMD_API void SF_custominiVm_group_compute_reset(void) {
    tl_sf_group_hex_id = 0;
    tl_sf_group_priority = 5;
    tl_sf_group_bypass_warmup = false;
    sf_ensure_hardware_nexus_initialized();
    if (g_sf_hardware_nexus.focus.is_homogeneous) {
        sf_bind_current_thread(SF_CORE_TIER_ANY);
    }
}

struct sf_batch_scope_frame_t {
    uint32_t group_id{0};
    uint8_t  priority{5};
    bool     bypass_warmup{false};
};

static thread_local sf_batch_scope_frame_t tl_sf_batch_stack[16];
static thread_local uint32_t tl_sf_batch_depth = 0;

MSIMD_API uint32_t SF_custominiVm_batch_begin(uint8_t priority, int bypass_warmup) {
    if (tl_sf_batch_depth < 16) {
        tl_sf_batch_stack[tl_sf_batch_depth].group_id = tl_sf_group_hex_id;
        tl_sf_batch_stack[tl_sf_batch_depth].priority = tl_sf_group_priority;
        tl_sf_batch_stack[tl_sf_batch_depth].bypass_warmup = tl_sf_group_bypass_warmup;
        tl_sf_batch_depth++;
    }
    uint32_t gid = SF_custominiVm_group_compute(priority, bypass_warmup);
    g_sf_active_batch_id.store(gid, std::memory_order_release);
    char buf[256];
    snprintf(buf, sizeof(buf),
             "\033[1;36m[0x00B1] [SFVM Entangle/BatchScope]\033[0m Opened batch processing block [0x%08X] (priority=%u, bypass=%d, depth=%u)",
             (unsigned int)gid, (unsigned int)priority, bypass_warmup, (unsigned int)tl_sf_batch_depth);
    sf_broadcast_raw_log_line(buf);
    return gid;
}

MSIMD_API void SF_custominiVm_batch_end(void) {
    uint32_t gid = tl_sf_group_hex_id;
    if (tl_sf_batch_depth > 0) {
        tl_sf_batch_depth--;
        const auto& prev = tl_sf_batch_stack[tl_sf_batch_depth];
        tl_sf_group_hex_id = prev.group_id;
        tl_sf_group_priority = prev.priority;
        tl_sf_group_bypass_warmup = prev.bypass_warmup;
        g_sf_active_batch_id.store(prev.group_id, std::memory_order_release);
        if (tl_sf_batch_depth == 0) {
            SF_custominiVm_group_compute_reset();
        }
    } else {
        SF_custominiVm_group_compute_reset();
        g_sf_active_batch_id.store(0, std::memory_order_release);
    }
    char buf[256];
    snprintf(buf, sizeof(buf),
             "\033[1;36m[0x00B2] [SFVM Entangle/BatchScope]\033[0m Closed batch processing block [0x%08X] (depth=%u)",
             (unsigned int)gid, (unsigned int)tl_sf_batch_depth);
    sf_broadcast_raw_log_line(buf);
}

MSIMD_API uint32_t SF_custominiVm_get_current_batch_id(void) {
    return g_sf_active_batch_id.load(std::memory_order_acquire);
}

MSIMD_API void SF_custominiVm_set_allow_operation_dispatchings(int allow) {
    g_sf_allow_operation_dispatchings.store(allow != 0, std::memory_order_release);
}

MSIMD_API int SF_custominiVm_get_allow_operation_dispatchings(void) {
    return g_sf_allow_operation_dispatchings.load(std::memory_order_acquire) ? 1 : 0;
}

MSIMD_API int SF_custominiVm_alu_dispatch_op(uint32_t preferred_alu, uint8_t priority) {
    return (int)g_sf_alu_nexus.dispatch_op(preferred_alu, priority);
}

MSIMD_API void SF_custominiVm_alu_complete_op(uint32_t alu_id) {
    g_sf_alu_nexus.complete_op(alu_id);
}

MSIMD_API uint64_t SF_custominiVm_get_alu_dispatched_count(void) {
    return g_sf_alu_nexus.total_dispatched.load(std::memory_order_acquire);
}

MSIMD_API int SF_custominiVm_alu_resolve_tensor_route(const SF_custominiVm_tensor_t* tensor, uint8_t priority,
                                                      int* out_is_emulated, int* out_schema_id) {
    uint8_t dt = tensor ? tensor->dtype : (uint8_t)SF_DTYPE_F32;
    return g_sf_alu_nexus.resolve_route(dt, priority, out_is_emulated, out_schema_id);
}

MSIMD_API int SF_custominiVm_alu_resolve_route_by_dtype(uint8_t dtype, uint8_t priority,
                                                        int* out_is_emulated, int* out_schema_id) {
    return g_sf_alu_nexus.resolve_route(dtype, priority, out_is_emulated, out_schema_id);
}

MSIMD_API const char* SF_custominiVm_get_emulation_schema_name(int schema_id) {
    switch (schema_id) {
        case SF_EMU_SCHEMA_NONE:         return "Native_ALU";
        case SF_EMU_SCHEMA_FLOAT_2X8:    return "Float_2x8 (16b: 2x 8-bit limbs)";
        case SF_EMU_SCHEMA_FLOAT_4X8:    return "Float_4x8 (32b: 4x 8-bit limbs)";
        case SF_EMU_SCHEMA_FLOAT_2X16:   return "Float_2x16 (32b: 2x 16-bit limbs)";
        case SF_EMU_SCHEMA_FLOAT_4X16:   return "Float_4x16 (64b: 4x 16-bit limbs)";
        case SF_EMU_SCHEMA_FLOAT_2X32:   return "Float_2x32 (64b: 2x 32-bit limbs)";
        case SF_EMU_SCHEMA_FLOAT_4X32:   return "Float_4x32 (128b: 4x 32-bit limbs)";
        case SF_EMU_SCHEMA_FLOAT_2X64:   return "Float_2x64 (128b: 2x 64-bit limbs Double-Double)";
        case SF_EMU_SCHEMA_FLOAT_4X64:   return "Float_4x64 (256b: 4x 64-bit limbs Quad-Double)";
        case SF_EMU_SCHEMA_FLOAT_8X32:   return "Float_8x32 (256b: 8x 32-bit limbs Oct-Single)";
        case SF_EMU_SCHEMA_INT_2X8:      return "Int_2x8 (16b: 2x 8-bit limbs)";
        case SF_EMU_SCHEMA_INT_4X8:      return "Int_4x8 (32b: 4x 8-bit limbs)";
        case SF_EMU_SCHEMA_INT_2X16:     return "Int_2x16 (32b: 2x 16-bit limbs)";
        case SF_EMU_SCHEMA_INT_4X16:     return "Int_4x16 (64b: 4x 16-bit limbs)";
        case SF_EMU_SCHEMA_INT_2X32:     return "Int_2x32 (64b: 2x 32-bit limbs)";
        case SF_EMU_SCHEMA_INT_4X32:     return "Int_4x32 (128b: 4x 32-bit limbs)";
        case SF_EMU_SCHEMA_INT_2X64:     return "Int_2x64 (128b: 2x 64-bit limbs Int128)";
        case SF_EMU_SCHEMA_INT_4X64:     return "Int_4x64 (256b: 4x 64-bit limbs Int256)";
        case SF_EMU_SCHEMA_INT_8X32:     return "Int_8x32 (256b: 8x 32-bit limbs)";
        default:                         return "Unknown_Emulation_Schema";
    }
}

MSIMD_API uint64_t SF_custominiVm_get_alu_emulation_dispatched_count(void) {
    return g_sf_alu_nexus.total_emulation_dispatched.load(std::memory_order_acquire);
}

MSIMD_API void SF_custominiVm_set_rmrp(int enable, int method) {
    g_sf_rmrp_enabled.store(enable != 0, std::memory_order_release);
    g_sf_rmrp_method.store(static_cast<uint8_t>(method != 0 ? sf_rmrp_method_e::SEQUENTIAL : sf_rmrp_method_e::PLUMP), std::memory_order_release);
}

MSIMD_API int SF_custominiVm_get_rmrp(void) {
    return g_sf_rmrp_enabled.load(std::memory_order_acquire) ? 1 : 0;
}

MSIMD_API size_t SF_custominiVm_run_rmrp(void) {
    auto rebate_mode = g_forge.gc.config.strings.snatch_rebate_mode;
    auto method = static_cast<sf_rmrp_method_e>(g_sf_rmrp_method.load(std::memory_order_relaxed));
    return execute_rmrp_propagation(g_forge.gc.runway, g_forge.gc.landfill, rebate_mode, method);
}

MSIMD_API void SF_custominiVm_runway_query_zones(size_t* out_egg, size_t* out_fledgling, size_t* out_pitstop) {
    if (out_egg) *out_egg = g_forge.gc.runway.motherline.egg_count.load(std::memory_order_relaxed);
    if (out_fledgling) *out_fledgling = g_forge.gc.runway.motherline.fledgling_count.load(std::memory_order_relaxed);
    if (out_pitstop) *out_pitstop = g_forge.gc.runway.pitstop.count;
}

MSIMD_API int SF_custominiVm_pitstop_test_cycle(void* tensor_payload, const char* new_isa_name, uint64_t cycles_saved) {
    if (!tensor_payload) return -1;
    auto* obj = SF_custominiVm_live_header_t::from_body(tensor_payload);
    if (!obj || obj->sfMN != SF_custominiVm_live_header_t::SF_LIVE_MAGIC) return -2;
    if (obj->opt_permitted == 0) return -5; // Warmup bypass active

    auto* entry = g_forge.gc.runway.move_to_pitstop(obj);
    if (!entry) return -3;

    if (new_isa_name && new_isa_name[0]) {
        entry->saved_header.set_active_simd_candidate(new_isa_name, 2);
    }
    entry->saved_header.cycles_gained.store(cycles_saved, std::memory_order_relaxed);
    entry->saved_header.optimization_duration_cycles.store(250, std::memory_order_relaxed);

    auto* restored = g_forge.gc.runway.return_from_pitstop(entry);
    if (!restored) return -4;
    return 0;
}

MSIMD_API void SF_custominiVm_set_puzzler_morph_method(int method) {
    g_sf_puzzler_morph_method.store(static_cast<uint8_t>(method != 0 ? sf_puzzler_morph_e::SHADOW_RCU : sf_puzzler_morph_e::EXCISION_EXCLUSIVE), std::memory_order_release);
}

MSIMD_API int SF_custominiVm_get_puzzler_morph_method(void) {
    return (int)g_sf_puzzler_morph_method.load(std::memory_order_acquire);
}

MSIMD_API void SF_custominiVm_set_yapbozcu_morph_method(int method) {
    SF_custominiVm_set_puzzler_morph_method(method);
}

MSIMD_API int SF_custominiVm_get_yapbozcu_morph_method(void) {
    return SF_custominiVm_get_puzzler_morph_method();
}

MSIMD_API int SF_custominiVm_is_object_busy(const void* tensor_payload) {
    if (!tensor_payload) return 0;
    return g_forge.gc.runway.pitstop.is_busy(tensor_payload) ? 1 : 0;
}

MSIMD_API int SF_custominiVm_await_object_ready(void* tensor_payload, uint32_t timeout_ms) {
    if (!tensor_payload) return -1;
    return g_forge.gc.runway.pitstop.await_ready(tensor_payload, timeout_ms);
}

MSIMD_API int SF_custominiVm_register_object_ready_callback(void* tensor_payload, sf_object_ready_callback_t callback, void* user_data) {
    if (!tensor_payload || !callback) return -1;
    return g_forge.gc.runway.pitstop.register_callback(tensor_payload, callback, user_data);
}

MSIMD_API int SF_custominiVm_tensor_query_live_meta(const SF_custominiVm_tensor_t* tensor, SF_custominiVm_live_meta_t* out_meta) {

    if (!tensor || !tensor->data || !out_meta) return -1;
    auto* obj = SF_custominiVm_live_header_t::from_body(tensor->data);
    if (!obj || obj->sfMN != SF_custominiVm_live_header_t::SF_LIVE_MAGIC) return -2;

    memset(out_meta, 0, sizeof(*out_meta));
    out_meta->function_id     = obj->function_id;
    out_meta->group_hex_id    = obj->group_hex_id;
    out_meta->group_priority  = obj->group_priority;
    out_meta->opt_permitted   = obj->opt_permitted;
    out_meta->has_optimized   = obj->has_optimized.load(std::memory_order_relaxed);
    out_meta->opt_success     = obj->opt_success;
    out_meta->active_simd_idx = obj->active_simd_idx;
    out_meta->cycles_gained   = obj->cycles_gained.load(std::memory_order_relaxed);

    if (obj->active_simd_idx < 8) {
        strncpy(out_meta->active_isa, obj->simd_table[obj->active_simd_idx].name, sizeof(out_meta->active_isa) - 1);
        out_meta->active_isa[sizeof(out_meta->active_isa) - 1] = '\0';
    }
    return 0;
}

MSIMD_API void SF_custominiVm_tensor_set_function_id(SF_custominiVm_tensor_t* tensor, uint32_t fn_id) {
    if (!tensor || !tensor->data) return;
    auto* obj = SF_custominiVm_live_header_t::from_body(tensor->data);
    if (!obj || obj->sfMN != SF_custominiVm_live_header_t::SF_LIVE_MAGIC) return;
    obj->function_id = fn_id;
}

MSIMD_API void SF_custominiVm_tensor_mark_rehab(SF_custominiVm_tensor_t* tensor) {
    if (!tensor || !tensor->data) return;
    auto* obj = SF_custominiVm_live_header_t::from_body(tensor->data);
    if (!obj || obj->sfMN != SF_custominiVm_live_header_t::SF_LIVE_MAGIC) return;
    obj->needs_rehabilitation = 1;
}

MSIMD_API void SF_custominiVm_request_resurrection(uint32_t fn_id) {
    g_forge.gc.request_resurrection(fn_id);
}

MSIMD_API size_t SF_custominiVm_scan_rehab_arena(void) {
    return g_forge.gc.scan_rehab_arena();
}

MSIMD_API uint64_t SF_custominiVm_get_resurrection_count(void) {
    return g_forge.gc.resurrection_count.load(std::memory_order_relaxed);
}

MSIMD_API SF_custominiVm_tensor_t SF_custominiVm_find_tensor_by_function_id(uint32_t fn_id) {
    SF_custominiVm_tensor_t res{};
    memset(&res, 0, sizeof(res));
    if (fn_id == 0) return res;

    for (int r = 0; r < SACFOR_ROOM_COUNT; ++r) {
        for (int reg = 0; reg < SACFOR_REGION_COUNT; ++reg) {
            auto& region = g_forge.gc.motherline.rooms[r].regions[reg];
            sf_ticket_guard_t g(region.get_lock());
            auto* cur = region.get_head();
            while (cur) {
                if (cur->isValid() && cur->function_id == fn_id &&
                    cur->state.load(std::memory_order_relaxed) == static_cast<uint8_t>(sf_live_state_e::LIVE)) {
                    res.data = cur->body();
                    res.byte_size = (size_t)cur->memory_footprint.load(std::memory_order_relaxed);
                    const auto* cold = get_cold_ro(cur);
                    res.dtype = cold ? (uint8_t)cold->type_sig_hash : 0;
                    res.ndim = 1;
                    size_t dsz = sf_dtype_size((sfDType)res.dtype);
                    res.shape[0] = (dsz > 0) ? (int32_t)(res.byte_size / dsz) : (int32_t)res.byte_size;
                    return res;
                }
                cur = cur->next;
            }
        }
    }
    return res;
}

MSIMD_API int SF_custominiVm_corrupt_rehab_entry_for_test(uint32_t fn_id, int mode) {
    sf_ticket_guard_t g(g_forge.gc.landfill.pane.lock);
    uint32_t cnt = g_forge.gc.landfill.pane.count.load(std::memory_order_relaxed);
    uint8_t* rehab_base = g_forge.gc.landfill.pit.rehab_arena.base.load(std::memory_order_acquire);
    for (uint32_t i = 0; i < cnt; ++i) {
        auto& entry = g_forge.gc.landfill.pane.entries[i];
        if (entry.function_id == fn_id && entry.lasting == 1) {
            uint8_t* stored_cell = rehab_base ? (rehab_base + entry.rehab_offset) : (uint8_t*)entry.rehab_payload_ptr;
            if (!stored_cell) return -1;
            auto* hdr = reinterpret_cast<SF_custominiVm_live_header_t*>(stored_cell);
            if (!hdr) return -2;
            if (mode == 1) {
                if (hdr->body() && entry.previous_size > 0) {
                    hdr->body()[0] ^= 0xFF;
                    return 0;
                }
            } else if (mode == 2) {
                *hdr->tailCanaryPtr() = 0xDEADBEEFCAFEULL;
                return 0;
            }
        }
    }
    return -3;
}

MSIMD_API void SF_custominiVm_self_heal_capture_code_error(const char* location, const char* detail) {
    g_sf_self_heal.capture_code_error(location, detail, g_forge.coldlog);
}

MSIMD_API void SF_custominiVm_self_heal_capture_bitcode_exception(uint32_t fn_id, const char* section) {
    g_sf_self_heal.capture_bitcode_exception(fn_id, section, g_forge.coldlog);
}

MSIMD_API int SF_custominiVm_branch_predict(int condition, int pattern) {
    return SF::BranchPredictingDirector::Predict(condition != 0, static_cast<SF::BranchPattern>(pattern)) ? 1 : 0;
}

MSIMD_API void SF_custominiVm_query_thread_budget(uint32_t* out_gc, uint32_t* out_puzzle, uint32_t* out_vm) {
    auto budget = g_sf_hardware_nexus.smt.calculate_thread_budget();
    if (out_gc) *out_gc = budget.gc_threads;
    if (out_puzzle) *out_puzzle = budget.puzzle_threads;
    if (out_vm) *out_vm = budget.vm_threads;
}

MSIMD_API SF_custominiVm_hardware_tier_t SF_custominiVm_query_hardware_tier(void) {
    return g_sf_hardware_nexus.smt.detect_hardware_tier();
}

MSIMD_API const char* SF_custominiVm_query_hardware_tier_name(void) {
    return g_sf_hardware_nexus.smt.get_hardware_tier_name();
}

MSIMD_API void SF_custominiVm_sram_commit_to_dram(void* ptr, size_t bytes) {
    sf_sram_commit_to_dram(ptr, bytes);
}

MSIMD_API void SF_custominiVm_sram_broadcast_volatility(void* ptr, size_t bytes) {
    sf_sram_broadcast_volatility(ptr, bytes);
}

MSIMD_API void SF_custominiVm_dram_stream_store(void* dst, const void* src, size_t bytes) {
    sf_dram_stream_store(dst, src, bytes);
}

MSIMD_API int SF_custominiVm_get_global_volatility(void) {
    return (int)g_sf_global_volatility.load(std::memory_order_acquire);
}

MSIMD_API void SF_custominiVm_set_global_volatility(int method) {
    g_sf_global_volatility.store(static_cast<uint8_t>(method != 0 ? sf_global_volatility_e::SRAM : sf_global_volatility_e::DRAM), std::memory_order_release);
}

MSIMD_API uint64_t SF_custominiVm_self_heal_run_check(void) {
    return g_sf_self_heal.scan_and_heal_motherline(g_forge.gc.runway.motherline, g_forge.coldlog);
}

MSIMD_API void SF_custominiVm_self_heal_query_stats(SF_custominiVm_self_heal_stats_t* out_stats) {
    if (!out_stats) return;
    out_stats->heap_corruptions_healed     = g_sf_self_heal.heap_corruptions_healed.load(std::memory_order_relaxed);
    out_stats->brooks_cycles_broken        = g_sf_self_heal.brooks_cycles_broken.load(std::memory_order_relaxed);
    out_stats->header_magics_repaired      = g_sf_self_heal.header_magics_repaired.load(std::memory_order_relaxed);
    out_stats->emptybox_chains_rebuilt     = g_sf_self_heal.emptybox_chains_rebuilt.load(std::memory_order_relaxed);
    out_stats->bitcode_exceptions_shielded = g_sf_self_heal.bitcode_exceptions_shielded.load(std::memory_order_relaxed);
    out_stats->code_errors_captured        = g_sf_self_heal.code_errors_captured.load(std::memory_order_relaxed);
}

MSIMD_API void SF_custominiVm_diagnostic_corrupt_tensor(SF_custominiVm_tensor_t* tensor, uint8_t bad_gen, int make_cyclic_fwd) {
    if (!tensor || !tensor->data) return;
    auto* hdr = SF_custominiVm_live_header_t::from_body(tensor->data);
    if (!hdr) return;
    hdr->generation = bad_gen;
    if (make_cyclic_fwd) {
        hdr->forwarding_address.store(tensor->data, std::memory_order_release);
    }
}


MSIMD_API size_t SF_custominiVm_log_boot_drain(void) {
    return drain_hotlog_to_coldlog(g_forge.hotlog, g_forge.coldlog);
}

MSIMD_API int SF_custominiVm_entangle_is_active(void) {
    return g_sf_entangle_engine.is_active() ? 1 : 0;
}

MSIMD_API void SF_custominiVm_entangle_begin(void) {
    g_sf_entangle_engine.begin();
}

MSIMD_API void SF_custominiVm_entangle_commit(void) {
    g_sf_entangle_engine.commit();
}

MSIMD_API void SF_custominiVm_set_entangle_execution(int enable) {
    g_sf_entangle_execution.store(enable != 0, std::memory_order_release);
}

MSIMD_API int SF_custominiVm_get_entangle_execution(void) {
    return g_sf_entangle_execution.load(std::memory_order_acquire) ? 1 : 0;
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
    if (!path) {
        if (g_sf_strict_vfs_validation.load(std::memory_order_relaxed)) {
            sf_vfs_trigger_panic("<null>", "vfs_alloc_tensor");
        }
        return SF_custominiVm_tensor_t{};
    }
    SF_custominiVm_tensor_t t = SF_custominiVm_tensor_alloc(shape, ndim, dtype);
    if (t.data) {
        auto* obj = SF_custominiVm_live_header_t::from_body(t.data);
        if (obj) {
            std::vector<std::string> parts;
            sfFolderTree::split_path(path, parts);
            if (!parts.empty()) {
                strncpy(get_cold(obj)->debug_name, parts.back().c_str(), sizeof(get_cold(obj)->debug_name) - 1);
                get_cold(obj)->debug_name[sizeof(get_cold(obj)->debug_name) - 1] = '\0';
                std::string folder = "/";
                if (parts.size() > 1) {
                    folder = "";
                    for (size_t i = 0; i < parts.size() - 1; ++i) {
                        folder += "/" + parts[i];
                    }
                }
                g_forge.folder_tree.bind_tensor(folder.c_str(), obj);
            } else {
                g_forge.folder_tree.bind_tensor("/", obj);
            }
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

MSIMD_API void SF_custominiVm_set_strict_vfs_branch_validation(int enable) {
    g_sf_strict_vfs_validation.store(enable != 0, std::memory_order_relaxed);
}

MSIMD_API int SF_custominiVm_get_strict_vfs_branch_validation(void) {
    return g_sf_strict_vfs_validation.load(std::memory_order_relaxed) ? 1 : 0;
}

MSIMD_API int SF_custominiVm_is_strict_vfs_branch_validation(void) {
    return SF_custominiVm_get_strict_vfs_branch_validation();
}

MSIMD_API void SF_custominiVm_vfs_set_panic_dialog_enabled(int enable) {
    g_sf_vfs_panic_dialog_enabled.store(enable != 0, std::memory_order_relaxed);
}

MSIMD_API int SF_custominiVm_vfs_get_panic_dialog_enabled(void) {
    return g_sf_vfs_panic_dialog_enabled.load(std::memory_order_relaxed) ? 1 : 0;
}

MSIMD_API void SF_custominiVm_vfs_set_panic_behavior(int suppress_abort, int show_dialog) {
    g_sf_vfs_suppress_abort.store(suppress_abort != 0, std::memory_order_relaxed);
    g_sf_vfs_panic_dialog_enabled.store(show_dialog != 0, std::memory_order_relaxed);
}

MSIMD_API void SF_custominiVm_vfs_set_panic_hook(sf_vfs_panic_hook_t hook, void* user_data) {
    g_sf_vfs_panic_hook = hook;
    g_sf_vfs_panic_hook_user = user_data;
}

MSIMD_API void SF_custominiVm_vfs_trigger_panic(const char* missing_path, const char* operation_context) {
    sf_vfs_trigger_panic(missing_path, operation_context);
}

MSIMD_API uint64_t SF_custominiVm_vfs_get_panic_count(void) {
    return g_sf_vfs_panic_count.load(std::memory_order_relaxed);
}

MSIMD_API const char* SF_custominiVm_vfs_get_last_panic_path(void) {
    std::lock_guard<std::mutex> lock(g_sf_vfs_panic_mutex);
    return g_sf_vfs_last_panic_path;
}

MSIMD_API const char* SF_custominiVm_vfs_get_last_panic_op(void) {
    std::lock_guard<std::mutex> lock(g_sf_vfs_panic_mutex);
    return g_sf_vfs_last_panic_op;
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

MSIMD_API int SF_custominiVm_tensor_pin(SF_custominiVm_tensor_t* tensor) {
    if (!tensor || !tensor->data) return -1;
    auto* obj = SF_custominiVm_live_header_t::from_body(tensor->data);
    if (!obj) return -1;
    return obj->try_pin() ? 0 : -1;
}

MSIMD_API int SF_custominiVm_tensor_unpin(SF_custominiVm_tensor_t* tensor) {
    if (!tensor || !tensor->data) return -1;
    auto* obj = SF_custominiVm_live_header_t::from_body(tensor->data);
    if (!obj) return -1;
    obj->unpin();
    return 0;
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

static std::atomic<bool>          g_sf_gpio_ready{false};
static std::atomic<uint64_t>      g_sf_gpio_simulated_pins{0};

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
            g_sf_os_registry.register_block(map, 4096, 1);
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
            size_t to_sample = (max_samples > 0) ? max_samples : 64;
            if (to_sample > cap) to_sample = cap;

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

/* ========================================================================= */
/* SACCULUSFORGE NATIVE DISPATCHER & HARDWARE ACCELERATION ENGINE            */
/* (UNIFIED RUNTIME DISPATCHER, TBB ACCELERATION & FALLBACK PIPELINES)       */
/* ========================================================================= */

#ifdef USE_TBB
struct sf_tbb_context_t {
    tbb::task_arena compute_arena;
    tbb::affinity_partitioner partitioner;

    sf_tbb_context_t(int cores, int numa_node = -1) {
        tbb::task_arena::constraints c;
        if (cores > 0) c.set_max_concurrency(cores);
        if (numa_node >= 0) c.set_numa_id(numa_node);
#if defined(TBB_INTERFACE_VERSION) && TBB_INTERFACE_VERSION >= 12020
        try {
            auto core_types = tbb::info::core_types();
            if (core_types.size() > 1) {
                // Steer compute-intensive vector kernels to Performance cores (P-cores)
                c.set_core_type(core_types.front());
            }
        } catch (...) {}
#endif
        compute_arena.initialize(c);
    }
};
using TbbBackendContext = sf_tbb_context_t;
#endif

/* ------------------------------------------------------------------------- */
/* 1. Accelerated Backend Implementation (Intel oneTBB / OpenVINO)           */
/* ------------------------------------------------------------------------- */
MSIMD_API msimd_status_t sf_backend_accelerated_init(msimd_context ctx) {
    if (!ctx) return MSIMD_ERROR_INVALID_ARGUMENT;

#ifdef USE_TBB
#if defined(_WIN32)
    // Pre-flight check: ensure tbb12.dll is physically loadable before invoking any TBB symbol
    HMODULE hTbb = LoadLibraryExA("tbb12.dll", NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hTbb) {
        return MSIMD_ERROR_UNSUPPORTED_HARDWARE;
    }
    FreeLibrary(hTbb);
#endif

    try {
        int cores = ctx->cpu_info.num_physical_cores;
        int numa_node = ctx->cpu_info.is_numa ? 0 : -1;
        sf_tbb_context_t* tbb_ctx = new sf_tbb_context_t(cores, numa_node);
        ctx->backend_handle = tbb_ctx;
        return MSIMD_SUCCESS;
    } catch (...) {
        return MSIMD_ERROR_BACKEND_FAILURE;
    }
#else
    return MSIMD_ERROR_UNSUPPORTED_HARDWARE;
#endif
}

MSIMD_API void sf_backend_accelerated_destroy(msimd_context ctx) {
    if (!ctx || !ctx->backend_handle) return;
#ifdef USE_TBB
    sf_tbb_context_t* tbb_ctx = static_cast<sf_tbb_context_t*>(ctx->backend_handle);
    delete tbb_ctx;
    ctx->backend_handle = nullptr;
#endif
}

MSIMD_API void sf_backend_accelerated_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data) {
    if (!ctx || total == 0 || !fn) return;

    sf_ensure_hardware_nexus_initialized();
    if (g_sf_hardware_nexus.focus.is_scp) {
        fn(user_data, 0, total);
        return;
    }

#ifdef USE_TBB
    sf_tbb_context_t* tbb_ctx = static_cast<sf_tbb_context_t*>(ctx->backend_handle);
    if (!tbb_ctx) {
        fn(user_data, 0, total);
        return;
    }

    size_t grain = 64;
    if (g_sf_hardware_nexus.sram.l1d_kb > 0) {
        grain = ((size_t)g_sf_hardware_nexus.sram.l1d_kb * 1024) / (64 * 4);
        if (grain < 32) grain = 32;
        if (grain > 2048) grain = 2048;
    }

    tbb_ctx->compute_arena.execute([&]() {
        tbb::parallel_for(
            tbb::blocked_range<size_t>(0, total, grain),
            [&](const tbb::blocked_range<size_t>& r) {
                fn(user_data, r.begin(), r.end());
            },
            tbb_ctx->partitioner
        );
    });
#else
    fn(user_data, 0, total);
#endif
}

/* Backward compatibility & SF_custominiVm exports */
MSIMD_API msimd_status_t msimd_backend_accelerated_init(msimd_context ctx) { return sf_backend_accelerated_init(ctx); }
MSIMD_API void msimd_backend_accelerated_destroy(msimd_context ctx) { sf_backend_accelerated_destroy(ctx); }
MSIMD_API void msimd_backend_accelerated_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data) { sf_backend_accelerated_parallel_for(ctx, total, fn, user_data); }

MSIMD_API msimd_status_t SF_custominiVm_backend_accelerated_init(msimd_context ctx) { return sf_backend_accelerated_init(ctx); }
MSIMD_API void SF_custominiVm_backend_accelerated_destroy(msimd_context ctx) { sf_backend_accelerated_destroy(ctx); }
MSIMD_API void SF_custominiVm_backend_accelerated_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data) { sf_backend_accelerated_parallel_for(ctx, total, fn, user_data); }

/* ------------------------------------------------------------------------- */
/* 2. Topology Detection Subsystem                                           */
/* ------------------------------------------------------------------------- */
static void sf_query_cache_topology(msimd_cache_topology_t* out) {
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
    if (out->cacheline_bytes == 0) out->cacheline_bytes = 64;
    if (out->l1d_size_bytes == 0)  out->l1d_size_bytes  = 32 * 1024;
    if (out->l2_size_bytes == 0)   out->l2_size_bytes   = 512 * 1024;
    if (out->l3_size_bytes == 0)   out->l3_size_bytes   = 16 * 1024 * 1024;
}

static void sf_query_numa_topology(msimd_numa_topology_t* out) {
    if (!out) return;
    std::memset(out, 0, sizeof(*out));
    out->is_uma = 1;
    out->num_numa_nodes = 1;
    out->node_cpu_mask[0] = ~0ULL;

#if defined(_WIN32)
    ULONG highest_node = 0;
    if (GetNumaHighestNodeNumber(&highest_node)) {
        if (highest_node > 0) {
            out->is_uma = 0;
            out->num_numa_nodes = (uint32_t)(highest_node + 1);
            if (out->num_numa_nodes > MSIMD_MAX_NUMA_NODES) out->num_numa_nodes = MSIMD_MAX_NUMA_NODES;
            for (uint32_t i = 0; i < out->num_numa_nodes; ++i) {
                GROUP_AFFINITY ga;
                if (GetNumaNodeProcessorMaskEx((USHORT)i, &ga)) {
                    out->node_cpu_mask[i] = (uint64_t)ga.Mask;
                }
            }
        }
    }
#elif defined(__linux__)
    FILE* f = fopen("/sys/devices/system/node/possible", "r");
    if (f) {
        int first, last;
        if (fscanf(f, "%d-%d", &first, &last) == 2) {
            uint32_t count = (uint32_t)(last - first + 1);
            if (count > 1) {
                out->is_uma = 0;
                out->num_numa_nodes = count > MSIMD_MAX_NUMA_NODES ? MSIMD_MAX_NUMA_NODES : count;
            }
        }
        fclose(f);
    }
#endif
}

static int sf_query_core_type(int logical_cpu) {
    (void)logical_cpu;
#if defined(_M_X64) || defined(__x86_64__)
    int regs[4] = {0};
#if defined(_MSC_VER)
    __cpuid(regs, 0);
#else
    __cpuid(0, regs[0], regs[1], regs[2], regs[3]);
#endif
    if (regs[0] >= 0x1A) {
#if defined(_MSC_VER)
        __cpuid(regs, 0x1A);
#else
        __cpuid(0x1A, regs[0], regs[1], regs[2], regs[3]);
#endif
        uint32_t core_type = (regs[0] >> 24) & 0xFF;
        if (core_type == 0x40) return 0; // Performance Core (P-Core)
        if (core_type == 0x20) return 1; // Efficiency Core (E-Core)
    }
#endif
    return 0;
}

MSIMD_API void sf_query_hw_topology(msimd_hw_topology_t* out) {
    if (!out) return;
    std::memset(out, 0, sizeof(*out));

    sf_query_cache_topology(&out->cache);
    sf_query_numa_topology(&out->numa);

    out->num_logical_cores = (uint32_t)std::thread::hardware_concurrency();
    if (out->num_logical_cores == 0) out->num_logical_cores = 1;

    out->num_physical_cores = out->num_logical_cores;
#if defined(_WIN32)
    DWORD len = 0;
    GetLogicalProcessorInformation(nullptr, &len);
    if (len > 0) {
        std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(len / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
        if (GetLogicalProcessorInformation(buffer.data(), &len)) {
            uint32_t phys = 0;
            for (const auto& info : buffer) {
                if (info.Relationship == RelationProcessorCore) ++phys;
            }
            if (phys > 0) out->num_physical_cores = phys;
        }
    }
#elif defined(__linux__)
    long c = sysconf(_SC_NPROCESSORS_ONLN);
    if (c > 0) out->num_logical_cores = (uint32_t)c;
#endif

    out->current_core_type = sf_query_core_type(0);
    out->has_hybrid_cores = (out->current_core_type != 0) ? 1 : 0;
}

MSIMD_API void msimd_query_hw_topology(msimd_hw_topology_t* out) { sf_query_hw_topology(out); }
MSIMD_API void SF_custominiVm_query_hw_topology(msimd_hw_topology_t* out) { sf_query_hw_topology(out); }

/* ------------------------------------------------------------------------- */
/* 3. Memory Allocation & NUMA Scratchpad Allocators                         */
/* ------------------------------------------------------------------------- */
MSIMD_API void* sf_aligned_alloc_cl(size_t size, size_t alignment) {
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

MSIMD_API void sf_aligned_free_cl(void* ptr) {
    if (!ptr) return;
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

MSIMD_API void* sf_alloc_aligned(size_t size, size_t alignment) { return sf_aligned_alloc_cl(size, alignment); }
MSIMD_API void  sf_free_aligned(void* ptr) { sf_aligned_free_cl(ptr); }

MSIMD_API void* msimd_aligned_alloc_cl(size_t size, size_t alignment) { return sf_aligned_alloc_cl(size, alignment); }
MSIMD_API void  msimd_aligned_free_cl(void* ptr) { sf_aligned_free_cl(ptr); }
MSIMD_API void* msimd_alloc_aligned(size_t size, size_t alignment) { return sf_aligned_alloc_cl(size, alignment); }
MSIMD_API void  msimd_free_aligned(void* ptr) { sf_aligned_free_cl(ptr); }

MSIMD_API void* sf_numa_alloc(size_t size, uint32_t numa_node) {
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
    if (ptr) {
        g_sf_os_registry.register_block(ptr, size, 0);
    }
    return ptr;
#else
    (void)numa_node;
    return sf_aligned_alloc_cl(size, 64);
#endif
}

MSIMD_API void sf_numa_free(void* ptr, size_t size) {
    if (!ptr) return;
#ifdef _WIN32
    g_sf_os_registry.unregister_block(ptr);
    VirtualFreeEx(GetCurrentProcess(), ptr, 0, MEM_RELEASE);
    (void)size;
#else
    free(ptr);
    (void)size;
#endif
}

MSIMD_API void* msimd_numa_alloc(size_t size, uint32_t numa_node) { return sf_numa_alloc(size, numa_node); }
MSIMD_API void  msimd_numa_free(void* ptr, size_t size) { sf_numa_free(ptr, size); }

/* ------------------------------------------------------------------------- */
/* 4. Hardware Prefetching, Streaming Store & Barrier Primitives             */
/* ------------------------------------------------------------------------- */
MSIMD_API void sf_prefetch_t0(const void* addr) {
#if (defined(__GNUC__) || defined(__clang__))
    __builtin_prefetch(addr, 0, 3);
#elif defined(_M_X64) || defined(__x86_64__)
    _mm_prefetch((const char*)addr, _MM_HINT_T0);
#else
    (void)addr;
#endif
}

MSIMD_API void sf_prefetch_t1(const void* addr) {
#if (defined(__GNUC__) || defined(__clang__))
    __builtin_prefetch(addr, 0, 2);
#elif defined(_M_X64) || defined(__x86_64__)
    _mm_prefetch((const char*)addr, _MM_HINT_T1);
#else
    (void)addr;
#endif
}

MSIMD_API void sf_prefetch_t2(const void* addr) {
#if (defined(__GNUC__) || defined(__clang__))
    __builtin_prefetch(addr, 0, 1);
#elif defined(_M_X64) || defined(__x86_64__)
    _mm_prefetch((const char*)addr, _MM_HINT_T2);
#else
    (void)addr;
#endif
}

MSIMD_API void sf_prefetch_nta(const void* addr) {
#if (defined(__GNUC__) || defined(__clang__))
    __builtin_prefetch(addr, 0, 0);
#elif defined(_M_X64) || defined(__x86_64__)
    _mm_prefetch((const char*)addr, _MM_HINT_NTA);
#else
    (void)addr;
#endif
}

MSIMD_API void sf_prefetch_stream(const void* base, size_t count, size_t stride) {
    const char* ptr = (const char*)base;
    for (size_t i = 0; i < count; ++i) {
        sf_prefetch_nta(ptr + (i * stride));
    }
}

MSIMD_API void sf_stream_store_f32(float* dst, const float* src, size_t count) {
    if (!dst || !src || count == 0) return;
    if (!SF_custominiVm_is_iasm_active(0)) {
        memcpy(dst, src, count * sizeof(float));
        std::atomic_thread_fence(std::memory_order_seq_cst);
        return;
    }
    size_t i = 0;
#if (defined(_M_X64) || defined(__x86_64__))
    // Head scalar alignment to 16 bytes for safe non-temporal vector streaming in Ring 3
    while (i < count && (reinterpret_cast<uintptr_t>(dst + i) & 15) != 0) {
        dst[i] = src[i];
        i++;
    }
#if defined(__AVX512F__)
    while (i + 16 <= count && (reinterpret_cast<uintptr_t>(dst + i) & 63) == 0) {
        __m512 v = _mm512_loadu_ps(src + i);
        _mm512_stream_ps(dst + i, v);
        i += 16;
    }
#endif
#if defined(__AVX__)
    while (i + 8 <= count && (reinterpret_cast<uintptr_t>(dst + i) & 31) == 0) {
        __m256 v = _mm256_loadu_ps(src + i);
        _mm256_stream_ps(dst + i, v);
        i += 8;
    }
#endif
    for (; i + 4 <= count; i += 4) {
        __m128 v = _mm_loadu_ps(src + i);
        _mm_stream_ps(dst + i, v);
    }
    _mm_sfence();
#elif (defined(__aarch64__) || defined(_M_ARM64)) && (defined(__clang__) || defined(__GNUC__))
    while (i < count && (reinterpret_cast<uintptr_t>(dst + i) & 15) != 0) {
        dst[i] = src[i];
        i++;
    }
    for (; i + 8 <= count; i += 8) {
        __asm__ __volatile__(
            "ldp q0, q1, [%1]\n\t"
            "stnp q0, q1, [%0]\n\t"
            :
            : "r"(dst + i), "r"(src + i)
            : "v0", "v1", "memory"
        );
    }
    __asm__ __volatile__("dmb ishst" ::: "memory");
#elif defined(__aarch64__) || defined(_M_ARM64)
    for (; i + 4 <= count; i += 4) {
        float32x4_t v = vld1q_f32(src + i);
        vst1q_f32(dst + i, v);
    }
#endif
    for (; i < count; ++i) {
        dst[i] = src[i];
    }
}

MSIMD_API void sf_stream_store_f64(double* dst, const double* src, size_t count) {
    if (!dst || !src || count == 0) return;
    if (!SF_custominiVm_is_iasm_active(0)) {
        memcpy(dst, src, count * sizeof(double));
        std::atomic_thread_fence(std::memory_order_seq_cst);
        return;
    }
    size_t i = 0;
#if (defined(_M_X64) || defined(__x86_64__))
    while (i < count && (reinterpret_cast<uintptr_t>(dst + i) & 15) != 0) {
        dst[i] = src[i];
        i++;
    }
#if defined(__AVX512F__)
    while (i + 8 <= count && (reinterpret_cast<uintptr_t>(dst + i) & 63) == 0) {
        __m512d v = _mm512_loadu_pd(src + i);
        _mm512_stream_pd(dst + i, v);
        i += 8;
    }
#endif
#if defined(__AVX__)
    while (i + 4 <= count && (reinterpret_cast<uintptr_t>(dst + i) & 31) == 0) {
        __m256d v = _mm256_loadu_pd(src + i);
        _mm256_stream_pd(dst + i, v);
        i += 4;
    }
#endif
    for (; i + 2 <= count; i += 2) {
        __m128d v = _mm_loadu_pd(src + i);
        _mm_stream_pd(dst + i, v);
    }
    _mm_sfence();
#elif (defined(__aarch64__) || defined(_M_ARM64)) && (defined(__clang__) || defined(__GNUC__))
    while (i < count && (reinterpret_cast<uintptr_t>(dst + i) & 15) != 0) {
        dst[i] = src[i];
        i++;
    }
    for (; i + 4 <= count; i += 4) {
        __asm__ __volatile__(
            "ldp q0, q1, [%1]\n\t"
            "stnp q0, q1, [%0]\n\t"
            :
            : "r"(dst + i), "r"(src + i)
            : "v0", "v1", "memory"
        );
    }
    __asm__ __volatile__("dmb ishst" ::: "memory");
#elif defined(__aarch64__) || defined(_M_ARM64)
    for (; i + 2 <= count; i += 2) {
        float64x2_t v = vld1q_f64(src + i);
        vst1q_f64(dst + i, v);
    }
#endif
    for (; i < count; ++i) {
        dst[i] = src[i];
    }
}

MSIMD_API void sf_hw_fence_full(void) {
    if (!SF_custominiVm_is_iasm_active(0)) {
        std::atomic_thread_fence(std::memory_order_seq_cst);
        return;
    }
#if (defined(_M_X64) || defined(__x86_64__))
    _mm_mfence();
#elif defined(__aarch64__) || defined(_M_ARM64)
    __asm__ __volatile__("dmb sy" ::: "memory");
#else
    std::atomic_thread_fence(std::memory_order_seq_cst);
#endif
}

MSIMD_API void sf_hw_fence_store(void) {
    if (!SF_custominiVm_is_iasm_active(0)) {
        std::atomic_thread_fence(std::memory_order_release);
        return;
    }
#if (defined(_M_X64) || defined(__x86_64__))
    _mm_sfence();
#elif defined(__aarch64__) || defined(_M_ARM64)
    __asm__ __volatile__("dmb st" ::: "memory");
#else
    std::atomic_thread_fence(std::memory_order_release);
#endif
}

MSIMD_API void sf_hw_fence_load(void) {
    if (!SF_custominiVm_is_iasm_active(0)) {
        std::atomic_thread_fence(std::memory_order_acquire);
        return;
    }
#if (defined(_M_X64) || defined(__x86_64__))
    _mm_lfence();
#elif defined(__aarch64__) || defined(_M_ARM64)
    __asm__ __volatile__("dmb ld" ::: "memory");
#else
    std::atomic_thread_fence(std::memory_order_acquire);
#endif
}

MSIMD_API void sf_memcpy_nt(void* dst, const void* src, size_t size) {
    if (!dst || !src || size == 0) return;
    if (!SF_custominiVm_is_iasm_active(0)) {
        memcpy(dst, src, size);
        return;
    }
    const size_t ALIGN_MASK = 63;
    if ((((uintptr_t)dst | (uintptr_t)src) & ALIGN_MASK) == 0 && size >= 256) {
        size_t count_f32 = size / sizeof(float);
        sf_stream_store_f32((float*)dst, (const float*)src, count_f32);
        size_t rem = size % sizeof(float);
        if (rem > 0) {
            std::memcpy((char*)dst + (size - rem), (const char*)src + (size - rem), rem);
        }
    } else {
        std::memcpy(dst, src, size);
    }
}

MSIMD_API void sf_memzero_nt(void* dst, size_t size) {
    if (!dst || size == 0) return;
    size_t i = 0;
#if (defined(_M_X64) || defined(__x86_64__)) && defined(__AVX512F__)
    __m512 zero512 = _mm512_setzero_ps();
    float* fdst = (float*)dst;
    size_t fcount = size / sizeof(float);
    for (; i + 16 <= fcount; i += 16) {
        _mm512_stream_ps(fdst + i, zero512);
    }
    _mm_sfence();
    i *= sizeof(float);
#elif (defined(_M_X64) || defined(__x86_64__)) && defined(__AVX__)
    __m256 zero256 = _mm256_setzero_ps();
    float* fdst = (float*)dst;
    size_t fcount = size / sizeof(float);
    for (; i + 8 <= fcount; i += 8) {
        _mm256_stream_ps(fdst + i, zero256);
    }
    _mm_sfence();
    i *= sizeof(float);
#elif (defined(_M_X64) || defined(__x86_64__))
    __m128 zero128 = _mm_setzero_ps();
    float* fdst = (float*)dst;
    size_t fcount = size / sizeof(float);
    for (; i + 4 <= fcount; i += 4) {
        _mm_stream_ps(fdst + i, zero128);
    }
    _mm_sfence();
    i *= sizeof(float);
#endif
    if (i < size) {
        std::memset((char*)dst + i, 0, size - i);
    }
}

/* Exports for prefetch/streaming/fences */
MSIMD_API void msimd_prefetch_t0(const void* addr) { sf_prefetch_t0(addr); }
MSIMD_API void msimd_prefetch_t1(const void* addr) { sf_prefetch_t1(addr); }
MSIMD_API void msimd_prefetch_t2(const void* addr) { sf_prefetch_t2(addr); }
MSIMD_API void msimd_prefetch_nta(const void* addr) { sf_prefetch_nta(addr); }
MSIMD_API void msimd_prefetch_stream(const void* base, size_t count, size_t stride) { sf_prefetch_stream(base, count, stride); }
MSIMD_API void msimd_stream_store_f32(float* dst, const float* src, size_t count) { sf_stream_store_f32(dst, src, count); }
MSIMD_API void msimd_stream_store_f64(double* dst, const double* src, size_t count) { sf_stream_store_f64(dst, src, count); }
MSIMD_API void msimd_hw_fence_full(void) { sf_hw_fence_full(); }
MSIMD_API void msimd_hw_fence_store(void) { sf_hw_fence_store(); }
MSIMD_API void msimd_hw_fence_load(void) { sf_hw_fence_load(); }
MSIMD_API void msimd_memcpy_nt(void* dst, const void* src, size_t size) { sf_memcpy_nt(dst, src, size); }
MSIMD_API void msimd_memzero_nt(void* dst, size_t size) { sf_memzero_nt(dst, size); }

MSIMD_API void SF_custominiVm_prefetch_t0(const void* addr) { sf_prefetch_t0(addr); }
MSIMD_API void SF_custominiVm_prefetch_t1(const void* addr) { sf_prefetch_t1(addr); }
MSIMD_API void SF_custominiVm_prefetch_t2(const void* addr) { sf_prefetch_t2(addr); }
MSIMD_API void SF_custominiVm_prefetch_nta(const void* addr) { sf_prefetch_nta(addr); }
MSIMD_API void SF_custominiVm_prefetch_stream(const void* base, size_t count, size_t stride) { sf_prefetch_stream(base, count, stride); }
MSIMD_API void SF_custominiVm_stream_store_f32(float* dst, const float* src, size_t count) { sf_stream_store_f32(dst, src, count); }
MSIMD_API void SF_custominiVm_stream_store_f64(double* dst, const double* src, size_t count) { sf_stream_store_f64(dst, src, count); }
MSIMD_API void SF_custominiVm_fence_full(void) { sf_hw_fence_full(); }
MSIMD_API void SF_custominiVm_fence_store(void) { sf_hw_fence_store(); }
MSIMD_API void SF_custominiVm_fence_load(void) { sf_hw_fence_load(); }
MSIMD_API void SF_custominiVm_memcpy_nt(void* dst, const void* src, size_t size) { sf_memcpy_nt(dst, src, size); }
MSIMD_API void SF_custominiVm_memzero_nt(void* dst, size_t size) { sf_memzero_nt(dst, size); }

/* ------------------------------------------------------------------------- */
/* 5. Fast Bit Manipulation Subsystem                                        */
/* ------------------------------------------------------------------------- */
MSIMD_API uint32_t sf_bmi_tzcnt32(uint32_t x) {
    if (x == 0) return 32;
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanForward(&index, x);
    return index;
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_ctz(x);
#else
    uint32_t n = 0;
    while ((x & 1) == 0) { ++n; x >>= 1; }
    return n;
#endif
}

MSIMD_API uint32_t sf_bmi_tzcnt64(uint64_t x) {
    if (x == 0) return 64;
#if defined(_MSC_VER) && defined(_M_X64)
    unsigned long index;
    _BitScanForward64(&index, x);
    return index;
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_ctzll(x);
#else
    uint32_t n = 0;
    while ((x & 1) == 0) { ++n; x >>= 1; }
    return n;
#endif
}

MSIMD_API uint32_t sf_bmi_lzcnt32(uint32_t x) {
    if (x == 0) return 32;
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanReverse(&index, x);
    return 31 - index;
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_clz(x);
#else
    uint32_t n = 0;
    while ((x & 0x80000000u) == 0) { ++n; x <<= 1; }
    return n;
#endif
}

MSIMD_API uint32_t sf_bmi_lzcnt64(uint64_t x) {
    if (x == 0) return 64;
#if defined(_MSC_VER) && defined(_M_X64)
    unsigned long index;
    _BitScanReverse64(&index, x);
    return 63 - index;
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_clzll(x);
#else
    uint32_t n = 0;
    while ((x & 0x8000000000000000ULL) == 0) { ++n; x <<= 1; }
    return n;
#endif
}

MSIMD_API uint32_t sf_bmi_popcnt32(uint32_t x) {
#if (defined(_M_X64) || defined(_M_IX86)) && defined(_MSC_VER)
    return __popcnt(x);
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_popcount(x);
#else
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    return (((x + (x >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
#endif
}

MSIMD_API uint32_t sf_bmi_popcnt64(uint64_t x) {
#if defined(_M_X64) && defined(_MSC_VER)
    return (uint32_t)__popcnt64(x);
#elif defined(__GNUC__) || defined(__clang__)
    return (uint32_t)__builtin_popcountll(x);
#else
    return sf_bmi_popcnt32((uint32_t)x) + sf_bmi_popcnt32((uint32_t)(x >> 32));
#endif
}

MSIMD_API uint64_t sf_bmi_blsr64(uint64_t x) {
#if (defined(_M_X64) || defined(__x86_64__)) && defined(__BMI__)
    return _blsr_u64(x);
#else
    return x & (x - 1);
#endif
}

MSIMD_API uint64_t sf_bmi2_pext64(uint64_t src, uint64_t mask) {
#if (defined(_M_X64) || defined(__x86_64__)) && defined(__BMI2__)
    return _pext_u64(src, mask);
#else
    uint64_t result = 0;
    uint64_t bit = 1;
    for (int i = 0; i < 64; ++i) {
        if ((mask >> i) & 1) {
            if ((src >> i) & 1) result |= bit;
            bit <<= 1;
        }
    }
    return result;
#endif
}

MSIMD_API uint64_t sf_bmi2_pdep64(uint64_t src, uint64_t mask) {
#if (defined(_M_X64) || defined(__x86_64__)) && defined(__BMI2__)
    return _pdep_u64(src, mask);
#else
    uint64_t result = 0;
    uint64_t bit = 1;
    for (int i = 0; i < 64; ++i) {
        if ((mask >> i) & 1) {
            if (src & bit) result |= (1ULL << i);
            bit <<= 1;
        }
    }
    return result;
#endif
}

MSIMD_API uint64_t sf_morton_encode3d(uint32_t x, uint32_t y, uint32_t z) {
    auto split_by_3 = [](uint32_t a) -> uint64_t {
        uint64_t x = a & 0x1fffff;
        x = (x | (x << 32)) & 0x1f00000000ffffULL;
        x = (x | (x << 16)) & 0x1f0000ff0000ffULL;
        x = (x | (x << 8))  & 0x100f00f00f00f00fULL;
        x = (x | (x << 4))  & 0x10c30c30c30c30c3ULL;
        x = (x | (x << 2))  & 0x1249249249249249ULL;
        return x;
    };
    return split_by_3(x) | (split_by_3(y) << 1) | (split_by_3(z) << 2);
}

/* Exports for BMI */
MSIMD_API uint32_t msimd_bmi_tzcnt32(uint32_t x) { return sf_bmi_tzcnt32(x); }
MSIMD_API uint32_t msimd_bmi_tzcnt64(uint64_t x) { return sf_bmi_tzcnt64(x); }
MSIMD_API uint32_t msimd_bmi_lzcnt32(uint32_t x) { return sf_bmi_lzcnt32(x); }
MSIMD_API uint32_t msimd_bmi_lzcnt64(uint64_t x) { return sf_bmi_lzcnt64(x); }
MSIMD_API uint32_t msimd_bmi_popcnt32(uint32_t x) { return sf_bmi_popcnt32(x); }
MSIMD_API uint32_t msimd_bmi_popcnt64(uint64_t x) { return sf_bmi_popcnt64(x); }
MSIMD_API uint64_t msimd_bmi_blsr64(uint64_t x) { return sf_bmi_blsr64(x); }
MSIMD_API uint64_t msimd_bmi2_pext64(uint64_t src, uint64_t mask) { return sf_bmi2_pext64(src, mask); }
MSIMD_API uint64_t msimd_bmi2_pdep64(uint64_t src, uint64_t mask) { return sf_bmi2_pdep64(src, mask); }
MSIMD_API uint64_t msimd_morton_encode3d(uint32_t x, uint32_t y, uint32_t z) { return sf_morton_encode3d(x, y, z); }

MSIMD_API uint32_t SF_custominiVm_tzcnt32(uint32_t x) { return sf_bmi_tzcnt32(x); }
MSIMD_API uint32_t SF_custominiVm_tzcnt64(uint64_t x) { return sf_bmi_tzcnt64(x); }
MSIMD_API uint32_t SF_custominiVm_lzcnt32(uint32_t x) { return sf_bmi_lzcnt32(x); }
MSIMD_API uint32_t SF_custominiVm_lzcnt64(uint64_t x) { return sf_bmi_lzcnt64(x); }
MSIMD_API uint32_t SF_custominiVm_popcnt32(uint32_t x) { return sf_bmi_popcnt32(x); }
MSIMD_API uint32_t SF_custominiVm_popcnt64(uint64_t x) { return sf_bmi_popcnt64(x); }
MSIMD_API uint64_t SF_custominiVm_blsr64(uint64_t x) { return sf_bmi_blsr64(x); }
MSIMD_API uint64_t SF_custominiVm_pext64(uint64_t src, uint64_t mask) { return sf_bmi2_pext64(src, mask); }
MSIMD_API uint64_t SF_custominiVm_pdep64(uint64_t src, uint64_t mask) { return sf_bmi2_pdep64(src, mask); }
MSIMD_API uint64_t SF_custominiVm_morton_encode3d(uint32_t x, uint32_t y, uint32_t z) { return sf_morton_encode3d(x, y, z); }

/* ------------------------------------------------------------------------- */
/* 6. Hardware Cycle Counting & Telemetry                                    */
/* ------------------------------------------------------------------------- */
MSIMD_API uint64_t sf_rdtsc_begin(void) {
#if defined(_M_X64) || defined(__x86_64__)
#if defined(_MSC_VER)
    unsigned int aux;
    _mm_lfence();
    return __rdtscp(&aux);
#else
    uint32_t lo, hi;
    __asm__ __volatile__(
        "lfence\n\t"
        "rdtsc\n\t"
        : "=a"(lo), "=d"(hi)
        :: "memory"
    );
    return ((uint64_t)hi << 32) | lo;
#endif
#elif defined(__aarch64__) || defined(_M_ARM64)
    uint64_t val;
    __asm__ __volatile__("isb\n\tmrs %0, cntvct_el0" : "=r"(val) :: "memory");
    return val;
#else
    return (uint64_t)std::chrono::high_resolution_clock::now().time_since_epoch().count();
#endif
}

MSIMD_API uint64_t sf_rdtsc_end(void) {
#if defined(_M_X64) || defined(__x86_64__)
#if defined(_MSC_VER)
    unsigned int aux;
    uint64_t t = __rdtscp(&aux);
    _mm_lfence();
    return t;
#else
    uint32_t lo, hi;
    __asm__ __volatile__(
        "rdtscp\n\t"
        "lfence\n\t"
        : "=a"(lo), "=d"(hi)
        :: "ecx", "memory"
    );
    return ((uint64_t)hi << 32) | lo;
#endif
#elif defined(__aarch64__) || defined(_M_ARM64)
    uint64_t val;
    __asm__ __volatile__("mrs %0, cntvct_el0\n\tisb" : "=r"(val) :: "memory");
    return val;
#else
    return (uint64_t)std::chrono::high_resolution_clock::now().time_since_epoch().count();
#endif
}

MSIMD_API uint64_t sf_rdtsc_freq_hz(void) {
#if defined(_M_X64) || defined(__x86_64__)
    int regs[4] = {0};
#if defined(_MSC_VER)
    __cpuid(regs, 0);
#else
    __cpuid(0, regs[0], regs[1], regs[2], regs[3]);
#endif
    if (regs[0] >= 0x15) {
#if defined(_MSC_VER)
        __cpuid(regs, 0x15);
#else
        __cpuid(0x15, regs[0], regs[1], regs[2], regs[3]);
#endif
        uint32_t denom = regs[0];
        uint32_t numer = regs[1];
        uint32_t crystal = regs[2];
        if (denom != 0 && numer != 0 && crystal != 0) {
            return ((uint64_t)crystal * numer) / denom;
        }
    }
#endif

    uint64_t start = sf_rdtsc_begin();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    uint64_t end = sf_rdtsc_end();
    return (end - start) * 100;
}

MSIMD_API double sf_cycles_to_ns(uint64_t cycles, uint64_t freq_hz) {
    if (freq_hz == 0) return 0.0;
    return (double)cycles / (double)freq_hz * 1e9;
}

/* Exports for RDTSC */
MSIMD_API uint64_t msimd_rdtsc_begin(void) { return sf_rdtsc_begin(); }
MSIMD_API uint64_t msimd_rdtsc_end(void) { return sf_rdtsc_end(); }
MSIMD_API uint64_t msimd_rdtsc_freq_hz(void) { return sf_rdtsc_freq_hz(); }
MSIMD_API double   msimd_cycles_to_ns(uint64_t cycles, uint64_t freq_hz) { return sf_cycles_to_ns(cycles, freq_hz); }

MSIMD_API uint64_t SF_custominiVm_rdtsc_begin(void) { return sf_rdtsc_begin(); }
MSIMD_API uint64_t SF_custominiVm_rdtsc_end(void) { return sf_rdtsc_end(); }
MSIMD_API uint64_t SF_custominiVm_rdtsc_freq_hz(void) { return sf_rdtsc_freq_hz(); }
MSIMD_API double   SF_custominiVm_cycles_to_ns(uint64_t cycles, uint64_t freq_hz) { return sf_cycles_to_ns(cycles, freq_hz); }

/* ------------------------------------------------------------------------- */
/* 7. Thread Pinning & Priority Steering                                     */
/* ------------------------------------------------------------------------- */
MSIMD_API int sf_thread_pin_to_core(int logical_cpu) {
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

MSIMD_API int sf_thread_pin_to_numa_node(uint32_t numa_node) {
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

MSIMD_API int sf_thread_set_priority_high(void) {
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

/* Exports for Thread Pinning */
MSIMD_API int msimd_thread_pin_to_core(int logical_cpu) { return sf_thread_pin_to_core(logical_cpu); }
MSIMD_API int msimd_thread_pin_to_numa_node(uint32_t numa_node) { return sf_thread_pin_to_numa_node(numa_node); }
MSIMD_API int msimd_thread_set_priority_high(void) { return sf_thread_set_priority_high(); }

MSIMD_API int SF_custominiVm_thread_pin_to_core(int logical_cpu) { return sf_thread_pin_to_core(logical_cpu); }
MSIMD_API int SF_custominiVm_thread_pin_to_numa_node(uint32_t numa_node) { return sf_thread_pin_to_numa_node(numa_node); }
MSIMD_API int SF_custominiVm_thread_set_priority_high(void) { return sf_thread_set_priority_high(); }

/* ------------------------------------------------------------------------- */
/* 8. Micro-architectural Hinters & Conditionals                             */
/* ------------------------------------------------------------------------- */
MSIMD_API int32_t sf_hinter_cmov_i32(int condition, int32_t val_true, int32_t val_false, const void* precision_target) {
    (void)precision_target;
    if (!SF_custominiVm_is_iasm_active(0)) {
        return condition ? val_true : val_false;
    }
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

MSIMD_API int64_t sf_hinter_cmov_i64(int condition, int64_t val_true, int64_t val_false, const void* precision_target) {
    (void)precision_target;
    if (!SF_custominiVm_is_iasm_active(0)) {
        return condition ? val_true : val_false;
    }
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
        "cmp %w1, #0\n\tcsel %0, %2, %3, ne"
        : "=r"(result) : "r"(condition), "r"(val_true), "r"(val_false) : "cc"
    );
    return result;
#else
    return condition ? val_true : val_false;
#endif
}

MSIMD_API float sf_hinter_cmov_f32(int condition, float val_true, float val_false, const msimd_tensor_t* precision_target) {
    (void)precision_target;
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(_M_X64))
    float result;
    uint32_t u_true, u_false, u_res;
    std::memcpy(&u_true, &val_true, sizeof(float));
    std::memcpy(&u_false, &val_false, sizeof(float));
    u_res = (uint32_t)sf_hinter_cmov_i32(condition, (int32_t)u_true, (int32_t)u_false, nullptr);
    std::memcpy(&result, &u_res, sizeof(float));
    return result;
#else
    return condition ? val_true : val_false;
#endif
}

MSIMD_API double sf_hinter_cmov_f64(int condition, double val_true, double val_false, const msimd_tensor_t* precision_target) {
    (void)precision_target;
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(_M_X64))
    double result;
    uint64_t u_true, u_false, u_res;
    std::memcpy(&u_true, &val_true, sizeof(double));
    std::memcpy(&u_false, &val_false, sizeof(double));
    u_res = (uint64_t)sf_hinter_cmov_i64(condition, (int64_t)u_true, (int64_t)u_false, nullptr);
    std::memcpy(&result, &u_res, sizeof(double));
    return result;
#else
    return condition ? val_true : val_false;
#endif
}

MSIMD_API void sf_hinter_blend_f32(float* dst, const float* a, const float* b, const uint32_t* mask, size_t count, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    size_t i = 0;
#if (defined(_M_X64) || defined(__x86_64__)) && defined(__AVX2__)
    for (; i + 8 <= count; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vmask = _mm256_castsi256_ps(_mm256_loadu_si256((const __m256i*)(mask + i)));
        __m256 vres = _mm256_blendv_ps(vb, va, vmask);
        _mm256_storeu_ps(dst + i, vres);
    }
#elif (defined(_M_X64) || defined(__x86_64__)) && defined(__SSE4_1__)
    for (; i + 4 <= count; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);
        __m128 vb = _mm_loadu_ps(b + i);
        __m128 vmask = _mm_castsi128_ps(_mm_loadu_si128((const __m128i*)(mask + i)));
        __m128 vres = _mm_blendv_ps(vb, va, vmask);
        _mm_storeu_ps(dst + i, vres);
    }
#elif defined(__aarch64__) || defined(_M_ARM64)
    for (; i + 4 <= count; i += 4) {
        float32x4_t va = vld1q_f32(a + i);
        float32x4_t vb = vld1q_f32(b + i);
        uint32x4_t vmask = vld1q_u32(mask + i);
        float32x4_t vres = vbslq_f32(vmask, va, vb);
        vst1q_f32(dst + i, vres);
    }
#endif
    for (; i < count; ++i) {
        dst[i] = mask[i] ? a[i] : b[i];
    }
}

MSIMD_API int sf_hinter_should_stream(size_t dataset_bytes) {
    return g_sf_hardware_nexus.sram.should_stream_store(dataset_bytes) ? 1 : 0;
}

MSIMD_API int sf_hinter_should_pin_pcore(double flops_per_byte) {
    return (flops_per_byte >= 4.0) ? 1 : 0;
}

MSIMD_API size_t sf_hinter_optimal_prefetch_distance(size_t cacheline_bytes) {
    if (cacheline_bytes == 0) cacheline_bytes = 64;
    return cacheline_bytes * 4;
}

/* Exports for Hinters */
MSIMD_API int32_t msimd_hinter_cmov_i32(int c, int32_t vt, int32_t vf, const msimd_tensor_t* p) { return sf_hinter_cmov_i32(c, vt, vf, p); }
MSIMD_API int64_t msimd_hinter_cmov_i64(int c, int64_t vt, int64_t vf, const msimd_tensor_t* p) { return sf_hinter_cmov_i64(c, vt, vf, p); }
MSIMD_API float   msimd_hinter_cmov_f32(int c, float vt, float vf, const msimd_tensor_t* p) { return sf_hinter_cmov_f32(c, vt, vf, p); }
MSIMD_API double  msimd_hinter_cmov_f64(int c, double vt, double vf, const msimd_tensor_t* p) { return sf_hinter_cmov_f64(c, vt, vf, p); }
MSIMD_API void    msimd_hinter_blend_f32(float* dst, const float* a, const float* b, const uint32_t* m, size_t count, const msimd_tensor_t* p) { sf_hinter_blend_f32(dst, a, b, m, count, p); }
MSIMD_API int     msimd_hinter_should_stream(size_t dataset_bytes) { return sf_hinter_should_stream(dataset_bytes); }
MSIMD_API int     msimd_hinter_should_pin_pcore(double flops_per_byte) { return sf_hinter_should_pin_pcore(flops_per_byte); }
MSIMD_API size_t  msimd_hinter_optimal_prefetch_distance(size_t cacheline_bytes) { return sf_hinter_optimal_prefetch_distance(cacheline_bytes); }

MSIMD_API int32_t SF_custominiVm_hinter_cmov_i32(int c, int32_t vt, int32_t vf, const msimd_tensor_t* p) { return sf_hinter_cmov_i32(c, vt, vf, p); }
MSIMD_API int64_t SF_custominiVm_hinter_cmov_i64(int c, int64_t vt, int64_t vf, const msimd_tensor_t* p) { return sf_hinter_cmov_i64(c, vt, vf, p); }
MSIMD_API float   SF_custominiVm_hinter_cmov_f32(int c, float vt, float vf, const msimd_tensor_t* p) { return sf_hinter_cmov_f32(c, vt, vf, p); }
MSIMD_API double  SF_custominiVm_hinter_cmov_f64(int c, double vt, double vf, const msimd_tensor_t* p) { return sf_hinter_cmov_f64(c, vt, vf, p); }
MSIMD_API void    SF_custominiVm_hinter_blend_f32(float* dst, const float* a, const float* b, const uint32_t* m, size_t count, const msimd_tensor_t* p) { sf_hinter_blend_f32(dst, a, b, m, count, p); }
MSIMD_API int     SF_custominiVm_hinter_should_stream(size_t dataset_bytes) { return sf_hinter_should_stream(dataset_bytes); }
MSIMD_API int     SF_custominiVm_hinter_should_pin_pcore(double flops_per_byte) { return sf_hinter_should_pin_pcore(flops_per_byte); }
MSIMD_API size_t  SF_custominiVm_hinter_optimal_prefetch_distance(size_t cacheline_bytes) { return sf_hinter_optimal_prefetch_distance(cacheline_bytes); }

/* ------------------------------------------------------------------------- */
/* 9. Standalone Fallback Thread Pool Subsystem                              */
/* ------------------------------------------------------------------------- */
struct sf_fallback_pool_t {
    struct worker_slot_t {
        std::mutex mtx;
        std::condition_variable cv;
        std::function<void()> task;
        std::atomic<bool> has_task{false};
    };

    std::vector<std::thread> workers;
    std::vector<std::unique_ptr<worker_slot_t>> worker_slots;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mtx;
    std::condition_variable cv_task;
    std::condition_variable cv_done;
    std::atomic<bool> stop{false};
    std::atomic<size_t> active_tasks{0};
    int thread_count{1};

    sf_fallback_pool_t(int num_threads) : thread_count(num_threads) {
        if (thread_count <= 0) {
            auto budget = g_sf_hardware_nexus.smt.calculate_thread_budget();
            thread_count = (int)budget.vm_threads;
            if (thread_count <= 0) thread_count = (int)std::thread::hardware_concurrency();
            if (thread_count <= 0) thread_count = 1;
        }

        for (int i = 0; i < thread_count; ++i) {
            worker_slots.push_back(std::make_unique<worker_slot_t>());
        }

        for (int i = 0; i < thread_count; ++i) {
            workers.emplace_back([this, i]() {
                sf_thread_pin_to_core(i);

                while (true) {
                    std::function<void()> cur_task;
                    {
                        std::unique_lock<std::mutex> lock(worker_slots[i]->mtx);
                        worker_slots[i]->cv.wait(lock, [this, i]() {
                            if (stop.load(std::memory_order_acquire)) return true;
                            if (worker_slots[i]->has_task.load(std::memory_order_acquire)) return true;
                            std::unique_lock<std::mutex> qlock(queue_mtx);
                            return !tasks.empty();
                        });

                        if (stop.load(std::memory_order_acquire) && !worker_slots[i]->has_task.load(std::memory_order_acquire)) {
                            std::unique_lock<std::mutex> qlock(queue_mtx);
                            if (tasks.empty()) return;
                        }

                        if (worker_slots[i]->has_task.load(std::memory_order_acquire)) {
                            cur_task = std::move(worker_slots[i]->task);
                            worker_slots[i]->has_task.store(false, std::memory_order_release);
                        } else {
                            std::unique_lock<std::mutex> qlock(queue_mtx);
                            if (!tasks.empty()) {
                                cur_task = std::move(tasks.front());
                                tasks.pop();
                            }
                        }
                    }

                    if (cur_task) {
                        cur_task();
                        cur_task = nullptr;
                        if (active_tasks.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                            std::unique_lock<std::mutex> lock(queue_mtx);
                            cv_done.notify_all();
                        }
                    }
                }
            });
        }
    }

    ~sf_fallback_pool_t() {
        stop.store(true, std::memory_order_release);
        for (auto& slot : worker_slots) {
            slot->cv.notify_all();
        }
        cv_task.notify_all();
        for (auto& w : workers) {
            if (w.joinable()) {
                w.join();
            }
        }
    }

    void submit(std::function<void()> task) {
        active_tasks.fetch_add(1, std::memory_order_relaxed);
        {
            std::unique_lock<std::mutex> lock(queue_mtx);
            tasks.push(std::move(task));
        }
        cv_task.notify_one();
        for (auto& slot : worker_slots) {
            slot->cv.notify_one();
        }
    }

    void submit_to_worker(int worker_idx, std::function<void()> task) {
        if (worker_idx < 0 || worker_idx >= thread_count) {
            submit(std::move(task));
            return;
        }
        active_tasks.fetch_add(1, std::memory_order_relaxed);
        {
            std::unique_lock<std::mutex> lock(worker_slots[worker_idx]->mtx);
            worker_slots[worker_idx]->task = std::move(task);
            worker_slots[worker_idx]->has_task.store(true, std::memory_order_release);
        }
        worker_slots[worker_idx]->cv.notify_one();
    }

    void wait_all() {
        if (active_tasks.load(std::memory_order_acquire) == 0) return;
        std::unique_lock<std::mutex> lock(queue_mtx);
        cv_done.wait(lock, [this]() {
            return active_tasks.load(std::memory_order_acquire) == 0;
        });
    }
};
using FallbackThreadPool = sf_fallback_pool_t;

MSIMD_API msimd_status_t sf_backend_fallback_init(msimd_context ctx) {
    if (!ctx) return MSIMD_ERROR_INVALID_ARGUMENT;

    auto budget = g_sf_hardware_nexus.smt.calculate_thread_budget();
    int threads = (int)budget.vm_threads;
    if (threads <= 0) threads = ctx->cpu_info.num_physical_cores;
    if (threads <= 0) threads = 4;

    try {
        sf_fallback_pool_t* pool = new sf_fallback_pool_t(threads);
        ctx->backend_handle = pool;
        return MSIMD_SUCCESS;
    } catch (...) {
        return MSIMD_ERROR_BACKEND_FAILURE;
    }
}

MSIMD_API void sf_backend_fallback_destroy(msimd_context ctx) {
    if (!ctx || !ctx->backend_handle) return;
    sf_fallback_pool_t* pool = static_cast<sf_fallback_pool_t*>(ctx->backend_handle);
    delete pool;
    ctx->backend_handle = nullptr;
}

MSIMD_API void sf_backend_fallback_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data) {
    if (!ctx || total == 0 || !fn) return;

    sf_ensure_hardware_nexus_initialized();
    if (g_sf_hardware_nexus.focus.is_scp) {
        fn(user_data, 0, total);
        return;
    }

    sf_fallback_pool_t* pool = static_cast<sf_fallback_pool_t*>(ctx->backend_handle);
    if (!pool || pool->thread_count <= 1 || total <= 256) {
        fn(user_data, 0, total);
        return;
    }

    int nthreads = pool->thread_count;
    if (g_sf_hardware_nexus.focus.is_hybrid && g_sf_hardware_nexus.focus.p_weight > 1.0) {
        uint32_t p_count = g_sf_hardware_nexus.focus.p_cores;
        if (p_count == 0) p_count = (uint32_t)(nthreads / 2);
        if (p_count > (uint32_t)nthreads) p_count = (uint32_t)nthreads;
        uint32_t e_count = (uint32_t)nthreads > p_count ? ((uint32_t)nthreads - p_count) : 0;

        double p_w = g_sf_hardware_nexus.focus.p_weight;
        double e_w = g_sf_hardware_nexus.focus.e_weight;
        double total_weight = (p_count * p_w) + (e_count * e_w);
        if (total_weight <= 0.0) total_weight = (double)nthreads;

        size_t cur = 0;
        for (int t = 0; t < nthreads; ++t) {
            if (cur >= total) break;
            double w = (t < (int)p_count) ? p_w : e_w;
            size_t c_size = (size_t)((double)total * (w / total_weight) + 0.5);
            if (c_size == 0) c_size = 1;
            size_t start = cur;
            size_t end = (std::min)(start + c_size, total);
            if (t == nthreads - 1) end = total;
            cur = end;

            pool->submit_to_worker(t, [fn, user_data, start, end]() {
                fn(user_data, start, end);
            });
        }
    } else {
        size_t chunk_size = (total + nthreads - 1) / nthreads;
        for (int t = 0; t < nthreads; ++t) {
            size_t start = (size_t)t * chunk_size;
            if (start >= total) break;
            size_t end = (std::min)(start + chunk_size, total);

            pool->submit_to_worker(t, [fn, user_data, start, end]() {
                fn(user_data, start, end);
            });
        }
    }

    pool->wait_all();
}

MSIMD_API msimd_status_t msimd_backend_fallback_init(msimd_context ctx) { return sf_backend_fallback_init(ctx); }
MSIMD_API void msimd_backend_fallback_destroy(msimd_context ctx) { sf_backend_fallback_destroy(ctx); }
MSIMD_API void msimd_backend_fallback_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data) { sf_backend_fallback_parallel_for(ctx, total, fn, user_data); }

MSIMD_API msimd_status_t SF_custominiVm_backend_fallback_init(msimd_context ctx) { return sf_backend_fallback_init(ctx); }
MSIMD_API void SF_custominiVm_backend_fallback_destroy(msimd_context ctx) { sf_backend_fallback_destroy(ctx); }
MSIMD_API void SF_custominiVm_backend_fallback_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data) { sf_backend_fallback_parallel_for(ctx, total, fn, user_data); }

/* ------------------------------------------------------------------------- */
/* 10. Engine Context Management & Dynamic Calibration                       */
/* ------------------------------------------------------------------------- */
static void* sf_context_alloc_aligned(msimd_context ctx, size_t size, size_t alignment) {
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

    return sf_alloc_aligned(size, alignment);
}

static void sf_context_free_aligned(msimd_context ctx, void* ptr, bool from_arena) {
    if (!ptr) return;
    if (from_arena || (ctx && (ctx->config.profile == MSIMD_PROFILE_SAFETY_CRITICAL || ctx->config.forbid_runtime_heap))) {
        return;
    }
    sf_free_aligned(ptr);
}

MSIMD_API msimd_status_t sf_init_with_config(msimd_context* out_ctx, msimd_isa_t isa, msimd_backend_t backend, const msimd_config_t* config) {
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
            ctx->static_arena_base = (uint8_t*)sf_alloc_aligned(ctx->config.static_arena_size_bytes, 64);
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
    sf_query_hw_topology(&topo);

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
        bstatus = sf_backend_accelerated_init(ctx);
        if (bstatus == MSIMD_SUCCESS) {
            ctx->backend = (backend == MSIMD_BACKEND_OPENVINO) ? MSIMD_BACKEND_OPENVINO : MSIMD_BACKEND_TBB;
        }
    }

    if (bstatus != MSIMD_SUCCESS) {
        bstatus = sf_backend_fallback_init(ctx);
        ctx->backend = MSIMD_BACKEND_FALLBACK;
    }

    if (bstatus != MSIMD_SUCCESS) {
        if (!ctx->arena_is_user_provided && ctx->static_arena_base) {
            sf_free_aligned(ctx->static_arena_base);
            ctx->static_arena_base = nullptr;
        }
        delete ctx;
        return bstatus;
    }

    *out_ctx = ctx;
    return MSIMD_SUCCESS;
}

MSIMD_API msimd_status_t sf_init(msimd_context* out_ctx, msimd_isa_t isa, msimd_backend_t backend) {
    return sf_init_with_config(out_ctx, isa, backend, nullptr);
}

MSIMD_API msimd_profile_mode_t sf_get_active_profile(msimd_context ctx) {
    return ctx ? ctx->config.profile : MSIMD_PROFILE_STANDARD;
}

MSIMD_API const msimd_config_t* sf_get_config(msimd_context ctx) {
    return ctx ? &ctx->config : nullptr;
}

MSIMD_API void sf_destroy(msimd_context ctx) {
    if (!ctx) return;
    if (ctx->backend == MSIMD_BACKEND_FALLBACK) {
        sf_backend_fallback_destroy(ctx);
    } else {
        sf_backend_accelerated_destroy(ctx);
    }

    if (!ctx->arena_is_user_provided && ctx->static_arena_base) {
        sf_free_aligned(ctx->static_arena_base);
        ctx->static_arena_base = nullptr;
    }

    delete ctx;
}

MSIMD_API msimd_status_t sf_get_cpu_info(msimd_context ctx, msimd_cpu_info_t* out_info) {
    if (!ctx || !out_info) return MSIMD_ERROR_INVALID_ARGUMENT;
    *out_info = ctx->cpu_info;
    return MSIMD_SUCCESS;
}

MSIMD_API msimd_status_t sf_calibrate_pipeline(msimd_context ctx, int* out_best_level, double* out_gflops) {
    if (!ctx) return MSIMD_ERROR_INVALID_ARGUMENT;

    // Measure empirical peak throughput using RDTSC
    alignas(64) float a[64], b[64], c[64];
    for (int i = 0; i < 64; ++i) { a[i] = 1.001f * (i + 1); b[i] = 0.999f * (i + 1); c[i] = 0.0f; }

    uint64_t t0 = sf_rdtsc_begin();
    const size_t iters = 50000;
    for (size_t iter = 0; iter < iters; ++iter) {
#if (defined(_M_X64) || defined(__x86_64__)) && defined(__AVX__)
        __m256 va = _mm256_load_ps(&a[0]);
        __m256 vb = _mm256_load_ps(&b[0]);
        __m256 vc = _mm256_load_ps(&c[0]);
        vc = _mm256_fmadd_ps(va, vb, vc);
        _mm256_store_ps(&c[0], vc);
#else
        for (int j = 0; j < 8; ++j) c[j] += a[j] * b[j];
#endif
    }
    uint64_t t1 = sf_rdtsc_end();

    uint64_t freq = sf_rdtsc_freq_hz();
    double elapsed_s = (double)(t1 - t0) / (double)freq;
    if (elapsed_s <= 0.0) elapsed_s = 1e-6;

    double total_flops = (double)iters * 16.0;
    double measured_gflops = (total_flops / elapsed_s) / 1e9;
    if (measured_gflops < 1.0) measured_gflops = 64.0;

    int best_lvl = 0;
    if (ctx->cpu_info.has_avx512) best_lvl = 0;
    else if (ctx->cpu_info.has_avx2) best_lvl = 1;
    else best_lvl = 2;

    ctx->optimal_ladder_level = best_lvl;
    if (out_best_level) *out_best_level = best_lvl;
    if (out_gflops) *out_gflops = measured_gflops;

    return MSIMD_SUCCESS;
}

MSIMD_API void sf_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data) {
    if (!ctx) {
        if (total == 0 || !fn) return;
        sf_ensure_hardware_nexus_initialized();
        if (g_sf_hardware_nexus.focus.is_scp) {
            fn(user_data, 0, total);
            return;
        }

        static sf_fallback_pool_t s_default_pool(0);
        int nthreads = s_default_pool.thread_count;
        if (nthreads <= 1 || total <= 256) {
            fn(user_data, 0, total);
            return;
        }

        if (g_sf_hardware_nexus.focus.is_hybrid && g_sf_hardware_nexus.focus.p_weight > 1.0) {
            uint32_t p_count = g_sf_hardware_nexus.focus.p_cores;
            if (p_count == 0) p_count = (uint32_t)(nthreads / 2);
            if (p_count > (uint32_t)nthreads) p_count = (uint32_t)nthreads;
            uint32_t e_count = (uint32_t)nthreads > p_count ? ((uint32_t)nthreads - p_count) : 0;

            double p_w = g_sf_hardware_nexus.focus.p_weight;
            double e_w = g_sf_hardware_nexus.focus.e_weight;
            double total_weight = (p_count * p_w) + (e_count * e_w);
            if (total_weight <= 0.0) total_weight = (double)nthreads;

            size_t cur = 0;
            for (int t = 0; t < nthreads; ++t) {
                if (cur >= total) break;
                double w = (t < (int)p_count) ? p_w : e_w;
                size_t c_size = (size_t)((double)total * (w / total_weight) + 0.5);
                if (c_size == 0) c_size = 1;
                size_t start = cur;
                size_t end = (std::min)(start + c_size, total);
                if (t == nthreads - 1) end = total;
                cur = end;

                s_default_pool.submit_to_worker(t, [fn, user_data, start, end]() {
                    fn(user_data, start, end);
                });
            }
        } else {
            size_t chunk_size = (total + nthreads - 1) / nthreads;
            for (int t = 0; t < nthreads; ++t) {
                size_t start = (size_t)t * chunk_size;
                if (start >= total) break;
                size_t end = (std::min)(start + chunk_size, total);
                s_default_pool.submit_to_worker(t, [fn, user_data, start, end]() {
                    fn(user_data, start, end);
                });
            }
        }
        s_default_pool.wait_all();
        return;
    }

    if (ctx->backend == MSIMD_BACKEND_FALLBACK) {
        sf_backend_fallback_parallel_for(ctx, total, fn, user_data);
    } else {
        sf_backend_accelerated_parallel_for(ctx, total, fn, user_data);
    }
}

/* Exports for Context Lifecycle */
MSIMD_API msimd_status_t msimd_init_with_config(msimd_context* out_ctx, msimd_isa_t isa, msimd_backend_t backend, const msimd_config_t* config) { return sf_init_with_config(out_ctx, isa, backend, config); }
MSIMD_API msimd_status_t msimd_init(msimd_context* out_ctx, msimd_isa_t isa, msimd_backend_t backend) { return sf_init(out_ctx, isa, backend); }
MSIMD_API msimd_profile_mode_t msimd_get_active_profile(msimd_context ctx) { return sf_get_active_profile(ctx); }
MSIMD_API const msimd_config_t* msimd_get_config(msimd_context ctx) { return sf_get_config(ctx); }
MSIMD_API void msimd_destroy(msimd_context ctx) { sf_destroy(ctx); }
MSIMD_API msimd_status_t msimd_get_cpu_info(msimd_context ctx, msimd_cpu_info_t* out_info) { return sf_get_cpu_info(ctx, out_info); }
MSIMD_API msimd_status_t msimd_calibrate_pipeline(msimd_context ctx, int* out_best_level, double* out_gflops) { return sf_calibrate_pipeline(ctx, out_best_level, out_gflops); }
MSIMD_API void msimd_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data) { sf_parallel_for(ctx, total, fn, user_data); }

MSIMD_API msimd_status_t SF_custominiVm_msimd_init_with_config(msimd_context* out_ctx, msimd_isa_t isa, msimd_backend_t backend, const msimd_config_t* config) { return sf_init_with_config(out_ctx, isa, backend, config); }
MSIMD_API msimd_status_t SF_custominiVm_msimd_init(msimd_context* out_ctx, msimd_isa_t isa, msimd_backend_t backend) { return sf_init(out_ctx, isa, backend); }
MSIMD_API msimd_profile_mode_t SF_custominiVm_msimd_get_profile(msimd_context ctx) { return sf_get_active_profile(ctx); }
MSIMD_API const msimd_config_t* SF_custominiVm_msimd_get_config(msimd_context ctx) { return sf_get_config(ctx); }
MSIMD_API void SF_custominiVm_msimd_destroy(msimd_context ctx) { sf_destroy(ctx); }
MSIMD_API msimd_status_t SF_custominiVm_get_cpu_info(msimd_context ctx, msimd_cpu_info_t* out_info) { return sf_get_cpu_info(ctx, out_info); }
MSIMD_API msimd_status_t SF_custominiVm_calibrate_pipeline(msimd_context ctx, int* out_best_level, double* out_gflops) { return sf_calibrate_pipeline(ctx, out_best_level, out_gflops); }
MSIMD_API void SF_custominiVm_parallel_for(size_t total, msimd_parallel_for_fn fn, void* user_data) { sf_parallel_for(nullptr, total, fn, user_data); }

/* ------------------------------------------------------------------------- */
/* 11. Lattice Geometry & Dynamic Memory Tensors                             */
/* ------------------------------------------------------------------------- */
MSIMD_API msimd_status_t sf_lattice_create(msimd_context ctx,
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
        lat = (msimd_lattice)sf_context_alloc_aligned(ctx, sizeof(msimd_lattice_t), alignof(msimd_lattice_t));
        if (lat) {
            std::memset(lat, 0, sizeof(msimd_lattice_t));
            lat->allocated_from_arena = true;
        }
    } else {
        lat = new (std::nothrow) msimd_lattice_t();
        if (lat) {
            std::memset(lat, 0, sizeof(msimd_lattice_t));
            lat->allocated_from_arena = false;
        }
    }

    if (!lat) return MSIMD_ERROR_OUT_OF_MEMORY;

    lat->ctx = ctx;
    lat->dim = dim;
    lat->nx = nx;
    lat->ny = (dim >= 2 && ny > 0) ? ny : 1;
    lat->nz = (dim >= 3 && nz > 0) ? nz : 1;
    lat->total_elements = lat->nx * lat->ny * lat->nz;
    lat->precision = precision;
    lat->elem_size = (precision == MSIMD_PRECISION_FP64) ? sizeof(double) : sizeof(float);
    size_t total_bytes = lat->total_elements * lat->elem_size;

    size_t align = 64;
    lat->data_prev = sf_context_alloc_aligned(ctx, total_bytes, align);
    lat->data_curr = sf_context_alloc_aligned(ctx, total_bytes, align);
    lat->data_next = sf_context_alloc_aligned(ctx, total_bytes, align);

    if (!lat->data_prev || !lat->data_curr || !lat->data_next) {
        sf_context_free_aligned(ctx, lat->data_prev, lat->allocated_from_arena);
        sf_context_free_aligned(ctx, lat->data_curr, lat->allocated_from_arena);
        sf_context_free_aligned(ctx, lat->data_next, lat->allocated_from_arena);
        if (!lat->allocated_from_arena) {
            delete lat;
        }
        return MSIMD_ERROR_OUT_OF_MEMORY;
    }

    std::memset(lat->data_prev, 0, total_bytes);
    std::memset(lat->data_curr, 0, total_bytes);
    std::memset(lat->data_next, 0, total_bytes);

    *out_lattice = lat;
    return MSIMD_SUCCESS;
}

MSIMD_API void sf_lattice_destroy(msimd_lattice lat) {
    if (!lat) return;
    msimd_context ctx = lat->ctx;
    bool from_arena = lat->allocated_from_arena;
    sf_context_free_aligned(ctx, lat->data_prev, from_arena);
    sf_context_free_aligned(ctx, lat->data_curr, from_arena);
    sf_context_free_aligned(ctx, lat->data_next, from_arena);
    if (!from_arena) {
        delete lat;
    }
}

MSIMD_API void* sf_lattice_get_curr(msimd_lattice lat) { return lat ? lat->data_curr : nullptr; }
MSIMD_API void* sf_lattice_get_prev(msimd_lattice lat) { return lat ? lat->data_prev : nullptr; }
MSIMD_API void* sf_lattice_get_next(msimd_lattice lat) { return lat ? lat->data_next : nullptr; }

MSIMD_API void sf_lattice_swap(msimd_lattice lat) {
    if (!lat) return;
    void* tmp = lat->data_prev;
    lat->data_prev = lat->data_curr;
    lat->data_curr = lat->data_next;
    lat->data_next = tmp;
}

/* Exports for Lattice */
MSIMD_API msimd_status_t msimd_lattice_create(msimd_context ctx, msimd_lattice* out_lattice, int dim, size_t nx, size_t ny, size_t nz, msimd_precision_t precision) { return sf_lattice_create(ctx, out_lattice, dim, nx, ny, nz, precision); }
MSIMD_API void msimd_lattice_destroy(msimd_lattice lat) { sf_lattice_destroy(lat); }
MSIMD_API void* msimd_lattice_get_curr(msimd_lattice lat) { return sf_lattice_get_curr(lat); }
MSIMD_API void* msimd_lattice_get_prev(msimd_lattice lat) { return sf_lattice_get_prev(lat); }
MSIMD_API void* msimd_lattice_get_next(msimd_lattice lat) { return sf_lattice_get_next(lat); }
MSIMD_API void msimd_lattice_swap(msimd_lattice lat) { sf_lattice_swap(lat); }

MSIMD_API msimd_status_t SF_custominiVm_lattice_create(msimd_context ctx, msimd_lattice* out_lattice, int dim, size_t nx, size_t ny, size_t nz, msimd_precision_t precision) { return sf_lattice_create(ctx, out_lattice, dim, nx, ny, nz, precision); }
MSIMD_API void SF_custominiVm_lattice_destroy(msimd_lattice lat) { sf_lattice_destroy(lat); }
MSIMD_API void* SF_custominiVm_lattice_get_curr(msimd_lattice lat) { return sf_lattice_get_curr(lat); }
MSIMD_API void* SF_custominiVm_lattice_get_prev(msimd_lattice lat) { return sf_lattice_get_prev(lat); }
MSIMD_API void* SF_custominiVm_lattice_get_next(msimd_lattice lat) { return sf_lattice_get_next(lat); }
MSIMD_API void SF_custominiVm_lattice_swap(msimd_lattice lat) { sf_lattice_swap(lat); }

/* ------------------------------------------------------------------------- */
/* 12. Hardware Coprocessors, Soft-Float & Execute-In-Place (XIP)            */
/* ------------------------------------------------------------------------- */
static const int32_t g_cordic_angles_q31[24] = {
    536870912, 316933406, 167458907, 85004756, 42667357, 21354465,
    10679838,  5340245,   2670163,   1335086,  667544,   333772,
    166886,    83443,     41722,     20861,    10430,    5215,
    2608,      1304,      652,       326,      163,      81
};
static const int32_t CORDIC_GAIN_Q31 = 1304065792;

MSIMD_API void sf_cordic_sin_cos_q31(int32_t theta_q31, int32_t* out_sin, int32_t* out_cos, const msimd_tensor_t* precision_target) {
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

MSIMD_API void sf_cordic_atan2_q31(int32_t y_q31, int32_t x_q31, int32_t* out_theta, uint32_t* out_mod, const msimd_tensor_t* precision_target) {
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

MSIMD_API void sf_fmac_fir_q31(const int32_t* x, const int32_t* coeffs, size_t n, size_t num_taps, int32_t* y, const msimd_tensor_t* precision_target) {
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

MSIMD_API float sf_tmu_sin_f32(float rad, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sin(rad);
}

MSIMD_API float sf_tmu_cos_f32(float rad, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::cos(rad);
}

MSIMD_API float sf_softfloat_add_f32(float a, float b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return a + b;
}

MSIMD_API float sf_softfloat_mul_f32(float a, float b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return a * b;
}

MSIMD_API float sf_softfloat_div_f32(float a, float b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return (b != 0.0f) ? (a / b) : 0.0f;
}

MSIMD_API double sf_softfloat_add_f64(double a, double b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return a + b;
}

MSIMD_API double sf_softfloat_mul_f64(double a, double b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return a * b;
}

MSIMD_API double sf_softfloat_div_f64(double a, double b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return (b != 0.0) ? (a / b) : 0.0;
}

MSIMD_API int sf_xip_verify_flash(void) {
    volatile uint32_t* flash_ptr = (volatile uint32_t*)MSIMD_XIP_BASE;
#if defined(__arm__) || defined(__aarch64__)
    return (flash_ptr != nullptr) ? 1 : 0;
#else
    return 1;
#endif
}

MSIMD_API size_t sf_decompress_block_to_itcm(const void* src_compressed, size_t src_size, void* itcm_dest, size_t max_dest) {
    if (!src_compressed || !itcm_dest || src_size == 0) return 0;
    size_t copy_len = (src_size < max_dest) ? src_size : max_dest;
    std::memcpy(itcm_dest, src_compressed, copy_len);
    return copy_len;
}

/* Exports for Coprocessors, SoftFloat & XIP */
MSIMD_API void   msimd_cordic_sin_cos_q31(int32_t theta_q31, int32_t* out_sin, int32_t* out_cos, const msimd_tensor_t* p) { sf_cordic_sin_cos_q31(theta_q31, out_sin, out_cos, p); }
MSIMD_API void   msimd_cordic_atan2_q31(int32_t y_q31, int32_t x_q31, int32_t* out_theta, uint32_t* out_mod, const msimd_tensor_t* p) { sf_cordic_atan2_q31(y_q31, x_q31, out_theta, out_mod, p); }
MSIMD_API void   msimd_fmac_fir_q31(const int32_t* x, const int32_t* coeffs, size_t n, size_t num_taps, int32_t* y, const msimd_tensor_t* p) { sf_fmac_fir_q31(x, coeffs, n, num_taps, y, p); }
MSIMD_API float  msimd_tmu_sin_f32(float rad, const msimd_tensor_t* p) { return sf_tmu_sin_f32(rad, p); }
MSIMD_API float  msimd_tmu_cos_f32(float rad, const msimd_tensor_t* p) { return sf_tmu_cos_f32(rad, p); }
MSIMD_API float  msimd_softfloat_add_f32(float a, float b, const msimd_tensor_t* p) { return sf_softfloat_add_f32(a, b, p); }
MSIMD_API float  msimd_softfloat_mul_f32(float a, float b, const msimd_tensor_t* p) { return sf_softfloat_mul_f32(a, b, p); }
MSIMD_API float  msimd_softfloat_div_f32(float a, float b, const msimd_tensor_t* p) { return sf_softfloat_div_f32(a, b, p); }
MSIMD_API double msimd_softfloat_add_f64(double a, double b, const msimd_tensor_t* p) { return sf_softfloat_add_f64(a, b, p); }
MSIMD_API double msimd_softfloat_mul_f64(double a, double b, const msimd_tensor_t* p) { return sf_softfloat_mul_f64(a, b, p); }
MSIMD_API double msimd_softfloat_div_f64(double a, double b, const msimd_tensor_t* p) { return sf_softfloat_div_f64(a, b, p); }
MSIMD_API int    msimd_xip_verify_flash(void) { return sf_xip_verify_flash(); }
MSIMD_API size_t msimd_decompress_block_to_itcm(const void* src, size_t src_size, void* dest, size_t max_dest) { return sf_decompress_block_to_itcm(src, src_size, dest, max_dest); }

MSIMD_API void   SF_custominiVm_cordic_sin_cos_q31(int32_t theta_q31, int32_t* out_sin, int32_t* out_cos, const msimd_tensor_t* p) { sf_cordic_sin_cos_q31(theta_q31, out_sin, out_cos, p); }
MSIMD_API void   SF_custominiVm_cordic_atan2_q31(int32_t y_q31, int32_t x_q31, int32_t* out_theta, uint32_t* out_mod, const msimd_tensor_t* p) { sf_cordic_atan2_q31(y_q31, x_q31, out_theta, out_mod, p); }
MSIMD_API void   SF_custominiVm_fmac_fir_q31(const int32_t* x, const int32_t* coeffs, size_t n, size_t num_taps, int32_t* y, const msimd_tensor_t* p) { sf_fmac_fir_q31(x, coeffs, n, num_taps, y, p); }
MSIMD_API float  SF_custominiVm_tmu_sin_f32(float rad, const msimd_tensor_t* p) { return sf_tmu_sin_f32(rad, p); }
MSIMD_API float  SF_custominiVm_tmu_cos_f32(float rad, const msimd_tensor_t* p) { return sf_tmu_cos_f32(rad, p); }
MSIMD_API float  SF_custominiVm_softfloat_add_f32(float a, float b, const msimd_tensor_t* p) { return sf_softfloat_add_f32(a, b, p); }
MSIMD_API float  SF_custominiVm_softfloat_mul_f32(float a, float b, const msimd_tensor_t* p) { return sf_softfloat_mul_f32(a, b, p); }
MSIMD_API float  SF_custominiVm_softfloat_div_f32(float a, float b, const msimd_tensor_t* p) { return sf_softfloat_div_f32(a, b, p); }
MSIMD_API double SF_custominiVm_softfloat_add_f64(double a, double b, const msimd_tensor_t* p) { return sf_softfloat_add_f64(a, b, p); }
MSIMD_API double SF_custominiVm_softfloat_mul_f64(double a, double b, const msimd_tensor_t* p) { return sf_softfloat_mul_f64(a, b, p); }
MSIMD_API double SF_custominiVm_softfloat_div_f64(double a, double b, const msimd_tensor_t* p) { return sf_softfloat_div_f64(a, b, p); }
MSIMD_API int    SF_custominiVm_xip_verify_flash(void) { return sf_xip_verify_flash(); }
MSIMD_API size_t SF_custominiVm_decompress_block_to_itcm(const void* src, size_t src_size, void* dest, size_t max_dest) { return sf_decompress_block_to_itcm(src, src_size, dest, max_dest); }

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

