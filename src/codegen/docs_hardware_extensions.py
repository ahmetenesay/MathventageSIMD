# -*- coding: utf-8 -*-
"""
Module providing Doxygen replacements for Hardware Architecture Extensions HW-1 to HW-8 (lines 1811 to 1885) in include/MathventageSIMD.h.
"""

HARDWARE_EXTENSIONS_DOCS = '''/* =========================================================================
 * Hardware Topology & CPU Functions (HW-1)
 * ========================================================================= */

/**
 * @defgroup MSIMD_HW_Topology HW-1: Hardware Topology & CPU Architectural Geometry
 * @brief Dynamic probing of CPU cache geometry (L1/L2/L3), NUMA node affinity masks, and hybrid core classifications.
 * @{
 */

/**
 * @brief Probes host processor hardware and populates an exhaustive hardware topology descriptor.
 *
 * Traverses CPUID leaves 0x04/0x18/0x1A (x86_64) or system registers CTR_EL0/MPIDR_EL1 (ARM64),
 * enumerating cache capacities, cacheline boundaries, NUMA node bitmasks, and identifying hybrid
 * Performance (P) versus Efficiency (E) core types.
 *
 * @param[out] out Pointer to an msimd_hw_topology_t structure populated with detected topology data.
 */
MSIMD_API void     msimd_query_hw_topology(msimd_hw_topology_t* out);

/** @} */ /* End of MSIMD_HW_Topology */

/* =========================================================================
 * Cache-Line Aligned Memory (HW-2)
 * ========================================================================= */

/**
 * @defgroup MSIMD_HW_Memory HW-2: Cache-Line Aligned & NUMA-Local Memory Management
 * @brief Zero-overhead cacheline-aligned and NUMA-node-pinned physical memory allocation.
 * @{
 */

/**
 * @brief Allocates dynamic heap memory strictly aligned to hardware CPU cacheline boundaries (typically 64 or 128 bytes).
 *
 * Eliminates false sharing across multi-threaded cores and prevents vector instruction split-load penalties.
 *
 * @param[in] size      Allocation size in bytes.
 * @param[in] alignment Cacheline alignment constraint in bytes (must be a power of two, >= 64).
 * @return Pointer to the aligned memory buffer, or NULL on allocation exhaustion.
 */
MSIMD_API void*    msimd_aligned_alloc_cl(size_t size, size_t alignment);

/**
 * @brief Frees a cacheline-aligned memory buffer previously allocated by msimd_aligned_alloc_cl().
 *
 * @param[in] ptr Pointer to the cacheline-aligned buffer to release.
 */
MSIMD_API void     msimd_aligned_free_cl(void* ptr);

/**
 * @brief Allocates physical memory pages bound specifically to a designated NUMA node.
 *
 * Uses `VirtualAllocExNuma` (Windows) or `numa_alloc_onnode` (Linux) to ensure physical RAM pages
 * are mapped directly into the memory channels attached to the specified NUMA socket.
 *
 * @param[in] size      Number of bytes to allocate.
 * @param[in] numa_node Non-Uniform Memory Access (NUMA) node index.
 * @return Pointer to the allocated NUMA-local memory buffer, or NULL on failure.
 */
MSIMD_API void*    msimd_numa_alloc(size_t size, uint32_t numa_node);

/**
 * @brief Releases a NUMA-bound memory buffer previously allocated via msimd_numa_alloc().
 *
 * @param[in] ptr  Pointer to the NUMA buffer.
 * @param[in] size Allocation size in bytes.
 */
MSIMD_API void     msimd_numa_free(void* ptr, size_t size);

/** @} */ /* End of MSIMD_HW_Memory */

/* =========================================================================
 * Hardware Prefetch & Non-Temporal Streaming Stores (HW-3)
 * ========================================================================= */

/**
 * @defgroup MSIMD_HW_Prefetch HW-3: Hardware Prefetching & Non-Temporal Streaming Stores
 * @brief Cache hierarchy prefetching hints (T0, T1, T2, NTA) and cache-bypassing streaming writes.
 * @{
 */

/**
 * @brief Prefetches a cacheline into all levels of the hardware cache hierarchy (L1/L2/L3).
 *
 * Emits `_mm_prefetch(addr, _MM_HINT_T0)` on x86, or `PRFM PLDL1KEEP` on ARM64.
 *
 * @param[in] addr Memory address to prefetch into L1 cache.
 */
MSIMD_API void     msimd_prefetch_t0(const void* addr);

/**
 * @brief Prefetches a cacheline into Level 2 and Level 3 caches, bypassing L1 Data cache.
 *
 * @param[in] addr Memory address to prefetch into L2 cache.
 */
MSIMD_API void     msimd_prefetch_t1(const void* addr);

/**
 * @brief Prefetches a cacheline into Level 3 (Last-Level) cache only.
 *
 * @param[in] addr Memory address to prefetch into L3 cache.
 */
MSIMD_API void     msimd_prefetch_t2(const void* addr);

/**
 * @brief Prefetches a cacheline with non-temporal hint (transient data, minimal cache pollution).
 *
 * @param[in] addr Memory address of transient buffer.
 */
MSIMD_API void     msimd_prefetch_nta(const void* addr);

/**
 * @brief Emits hardware prefetch hints across a contiguous memory stream with a specified stride.
 *
 * @param[in] base   Starting memory address.
 * @param[in] count  Number of elements to prefetch.
 * @param[in] stride Byte distance between consecutive elements.
 */
MSIMD_API void     msimd_prefetch_stream(const void* base, size_t count, size_t stride);

/**
 * @brief Stores an array of single-precision floats using non-temporal streaming instructions (bypassing CPU cache).
 *
 * Emits `_mm_stream_ps` or `VMOVNTSPS` instructions, writing data directly to main DRAM without
 * evicting active cachelines. Requires destination buffer to be 64-byte aligned.
 *
 * @param[out] dst   Destination buffer in physical RAM (must be 64-byte aligned).
 * @param[in]  src   Source float array.
 * @param[in]  count Number of float elements to stream write.
 */
MSIMD_API void     msimd_stream_store_f32(float* dst, const float* src, size_t count);

/**
 * @brief Stores an array of double-precision floats using non-temporal streaming stores.
 *
 * @param[out] dst   Destination buffer in physical RAM (must be 64-byte aligned).
 * @param[in]  src   Source double array.
 * @param[in]  count Number of double elements to stream write.
 */
MSIMD_API void     msimd_stream_store_f64(double* dst, const double* src, size_t count);

/**
 * @brief Enforces a full hardware memory barrier (`mfence` on x86, `DMB ISH` on ARM64).
 *
 * Serializes all prior and subsequent load and store memory operations across cores.
 */
MSIMD_API void     msimd_hw_fence_full(void);

/**
 * @brief Enforces a hardware store fence (`sfence` on x86, `DMB ISHST` on ARM64).
 *
 * Guarantees that all preceding non-temporal streaming stores are globally visible in memory.
 */
MSIMD_API void     msimd_hw_fence_store(void);

/**
 * @brief Enforces a hardware load fence (`lfence` on x86, `DMB ISHLD` on ARM64).
 */
MSIMD_API void     msimd_hw_fence_load(void);

/** @} */ /* End of MSIMD_HW_Prefetch */

/* =========================================================================
 * BMI2 Bit Manipulation (HW-4)
 * ========================================================================= */

/**
 * @defgroup MSIMD_HW_BitManip HW-4: BMI2 Bit Manipulation & Morton Space-Filling Curves
 * @brief Single-cycle bit counting, trailing/leading zero counts, parallel bit extract/deposit, and 3D Morton Z-curves.
 * @{
 */

/**
 * @brief Counts the number of trailing zero bits in a 32-bit unsigned integer (TZCNT instruction).
 *
 * @param[in] x 32-bit integer operand.
 * @return Number of trailing zero bits in the range [0, 32].
 */
MSIMD_API uint32_t msimd_bmi_tzcnt32(uint32_t x);

/**
 * @brief Counts the number of trailing zero bits in a 64-bit unsigned integer (TZCNT instruction).
 *
 * @param[in] x 64-bit integer operand.
 * @return Number of trailing zero bits in the range [0, 64].
 */
MSIMD_API uint32_t msimd_bmi_tzcnt64(uint64_t x);

/**
 * @brief Counts the number of leading zero bits in a 32-bit unsigned integer (LZCNT instruction).
 *
 * @param[in] x 32-bit integer operand.
 * @return Number of leading zero bits in the range [0, 32].
 */
MSIMD_API uint32_t msimd_bmi_lzcnt32(uint32_t x);

/**
 * @brief Counts the number of leading zero bits in a 64-bit unsigned integer (LZCNT instruction).
 *
 * @param[in] x 64-bit integer operand.
 * @return Number of leading zero bits in the range [0, 64].
 */
MSIMD_API uint32_t msimd_bmi_lzcnt64(uint64_t x);

/**
 * @brief Computes the population count (number of set 1-bits) in a 32-bit integer (POPCNT instruction).
 *
 * @param[in] x 32-bit integer operand.
 * @return Number of set bits.
 */
MSIMD_API uint32_t msimd_bmi_popcnt32(uint32_t x);

/**
 * @brief Computes the population count in a 64-bit integer (POPCNT instruction).
 *
 * @param[in] x 64-bit integer operand.
 * @return Number of set bits.
 */
MSIMD_API uint32_t msimd_bmi_popcnt64(uint64_t x);

/**
 * @brief Clears the lowest set 1-bit in a 64-bit integer (BLSR instruction): out = x & (x - 1).
 *
 * @param[in] x 64-bit integer operand.
 * @return Integer with the least-significant set bit cleared.
 */
MSIMD_API uint64_t msimd_bmi_blsr64(uint64_t x);

/**
 * @brief Parallel bit extract: gathers bits from source using bits specified in mask (PEXT instruction).
 *
 * @param[in] src  Source 64-bit integer.
 * @param[in] mask Bitmask specifying bits to extract.
 * @return Extracted contiguous bits packed toward the least significant bit.
 */
MSIMD_API uint64_t msimd_bmi2_pext64(uint64_t src, uint64_t mask);

/**
 * @brief Parallel bit deposit: scatters contiguous bits from source into positions specified in mask (PDEP instruction).
 *
 * @param[in] src  Contiguous bits to scatter.
 * @param[in] mask Bitmask specifying target bit positions.
 * @return Deposited 64-bit integer.
 */
MSIMD_API uint64_t msimd_bmi2_pdep64(uint64_t src, uint64_t mask);

/**
 * @brief Encodes 3D Cartesian grid coordinates into a 64-bit Morton Z-order curve index.
 *
 * Interleaves the bits of x, y, and z coordinates using BMI2 PDEP instructions to preserve spatial locality
 * in Barnes-Hut N-body trees, bounding volume hierarchies (BVH), and octrees.
 *
 * @param[in] x 21-bit integer x-coordinate.
 * @param[in] y 21-bit integer y-coordinate.
 * @param[in] z 21-bit integer z-coordinate.
 * @return 63-bit interleaved Morton space-filling curve index.
 */
MSIMD_API uint64_t msimd_morton_encode3d(uint32_t x, uint32_t y, uint32_t z);

/** @} */ /* End of MSIMD_HW_BitManip */

/* =========================================================================
 * RDTSC Cycle-Accurate Profiling (HW-5)
 * ========================================================================= */

/**
 * @defgroup MSIMD_HW_RDTSC HW-5: RDTSC Cycle-Accurate Profiling & Timers
 * @brief Serialized processor cycle counters (RDTSCP/LFENCE) and calibrated nanosecond converters.
 * @{
 */

/**
 * @brief Captures the starting timestamp counter with execution serialization (LFENCE + RDTSC).
 *
 * @return Current 64-bit processor time-stamp counter value.
 */
MSIMD_API uint64_t msimd_rdtsc_begin(void);

/**
 * @brief Captures the ending timestamp counter with execution serialization (RDTSCP + LFENCE).
 *
 * @return Current 64-bit processor time-stamp counter value.
 */
MSIMD_API uint64_t msimd_rdtsc_end(void);

/**
 * @brief Queries the calibrated base frequency of the invariant processor timestamp counter in Hertz.
 *
 * @return TSC frequency in Hz.
 */
MSIMD_API uint64_t msimd_rdtsc_freq_hz(void);

/**
 * @brief Converts an elapsed processor cycle delta into elapsed wall-clock nanoseconds.
 *
 * @param[in] cycles  Elapsed cycle count (end - begin).
 * @param[in] freq_hz Calibrated TSC base frequency in Hz.
 * @return Elapsed time in nanoseconds.
 */
MSIMD_API double   msimd_cycles_to_ns(uint64_t cycles, uint64_t freq_hz);

/** @} */ /* End of MSIMD_HW_RDTSC */

/* =========================================================================
 * Thread Affinity & P/E Core Pinning (HW-6)
 * ========================================================================= */

/**
 * @defgroup MSIMD_HW_Affinity HW-6: Thread Affinity & P/E Hybrid Core Pinning
 * @brief Thread affinity locking to physical cores, NUMA nodes, and real-time thread priority elevation.
 * @{
 */

/**
 * @brief Pins the calling thread exclusively to a designated logical processor core.
 *
 * @param[in] logical_cpu Logical CPU core index.
 * @return MSIMD_SUCCESS (0) on success, or an operating system error status code.
 */
MSIMD_API int      msimd_thread_pin_to_core(int logical_cpu);

/**
 * @brief Pins the calling thread to execute across all cores belonging to a designated NUMA node.
 *
 * @param[in] numa_node Target NUMA node index.
 * @return MSIMD_SUCCESS (0) on success, or an error status code.
 */
MSIMD_API int      msimd_thread_pin_to_numa_node(uint32_t numa_node);

/**
 * @brief Elevates the calling thread priority to high/real-time scheduling priority.
 *
 * @return MSIMD_SUCCESS (0) on success, or an error status code.
 */
MSIMD_API int      msimd_thread_set_priority_high(void);

/** @} */ /* End of MSIMD_HW_Affinity */

/* =========================================================================
 * Non-Temporal Bulk memcpy / memzero (HW-7)
 * ========================================================================= */

/**
 * @defgroup MSIMD_HW_BulkCopy HW-7: Non-Temporal Bulk Memory Transfer & Zeroing
 * @brief Cache-bypassing non-temporal streaming bulk memory copy and memory clear.
 * @{
 */

/**
 * @brief Copies large memory blocks using non-temporal streaming load and store instructions.
 *
 * Bypasses CPU caches to maximize DRAM memory bus saturation without evicting hot working sets.
 *
 * @param[out] dst  Destination memory pointer (must be 64-byte aligned).
 * @param[in]  src  Source memory pointer (must be 64-byte aligned).
 * @param[in]  size Number of bytes to copy (recommended > 256 KB).
 */
MSIMD_API void     msimd_memcpy_nt(void* dst, const void* src, size_t size);

/**
 * @brief Clears a large memory buffer to zero using non-temporal streaming stores.
 *
 * @param[out] dst  Destination memory buffer to zero (must be 64-byte aligned).
 * @param[in]  size Number of bytes to zero.
 */
MSIMD_API void     msimd_memzero_nt(void* dst, size_t size);

/** @} */ /* End of MSIMD_HW_BulkCopy */

/* =========================================================================
 * Ring 3 Inline Assembly & Predication Hinter Engine (HW-8)
 * Implementation of Machines-Rules.td Section 12 Rules in Userspace
 * ========================================================================= */

/**
 * @defgroup MSIMD_HW_Hinter HW-8: Branchless Predication & Memory Hinter Engine
 * @brief Branchless conditional moves (CMOV), vectorized lane blending, and memory access streaming hints.
 * @{
 */

/**
 * @brief Branchless conditional move for single-precision floats: out = condition ? val_true : val_false.
 *
 * Eliminates branch mispredictions using hardware conditional moves or blend instructions.
 *
 * @param[in] condition        Boolean condition (non-zero for true, zero for false).
 * @param[in] val_true         Value selected if condition is true.
 * @param[in] val_false        Value selected if condition is false.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Selected floating-point value.
 */
MSIMD_API float    msimd_hinter_cmov_f32(int condition, float val_true, float val_false, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Branchless conditional move for double-precision floats.
 *
 * @param[in] condition        Boolean condition.
 * @param[in] val_true         True branch value.
 * @param[in] val_false        False branch value.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Selected double value.
 */
MSIMD_API double   msimd_hinter_cmov_f64(int condition, double val_true, double val_false, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Branchless conditional move for 32-bit signed integers.
 *
 * @param[in] condition        Boolean condition.
 * @param[in] val_true         True branch value.
 * @param[in] val_false        False branch value.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Selected integer value.
 */
MSIMD_API int32_t  msimd_hinter_cmov_i32(int condition, int32_t val_true, int32_t val_false, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Branchless conditional move for 64-bit signed integers.
 *
 * @param[in] condition        Boolean condition.
 * @param[in] val_true         True branch value.
 * @param[in] val_false        False branch value.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Selected 64-bit integer value.
 */
MSIMD_API int64_t  msimd_hinter_cmov_i64(int condition, int64_t val_true, int64_t val_false, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized element blending across float arrays based on a bitmask: dst[i] = mask[i] ? a[i] : b[i].
 *
 * @param[out] dst              Destination float array.
 * @param[in]  a                First source float array.
 * @param[in]  b                Second source float array.
 * @param[in]  mask             Selection mask array (non-zero selects a, zero selects b).
 * @param[in]  count            Number of elements to blend.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void     msimd_hinter_blend_f32(float* dst, const float* a, const float* b, const uint32_t* mask, size_t count, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates whether a dataset size warrants using non-temporal streaming stores instead of standard cached writes.
 *
 * Compares dataset size against Level 3 cache capacity; returns true if dataset exceeds 50% of LLC.
 *
 * @param[in] dataset_bytes Total working footprint of the dataset in bytes.
 * @return 1 to stream, 0 to use standard cached writes.
 */
MSIMD_API int      msimd_hinter_should_stream(size_t dataset_bytes);

/**
 * @brief Evaluates whether an algorithmic computational density warrants pinning the thread to a Performance core (P-core).
 *
 * @param[in] flops_per_byte Arithmetic intensity (FLOPs executed per byte transferred from DRAM).
 * @return 1 to pin to P-core, 0 if E-core is sufficient.
 */
MSIMD_API int      msimd_hinter_should_pin_pcore(double flops_per_byte);

/**
 * @brief Calculates the optimal hardware prefetch distance in bytes for streaming array traversal.
 *
 * @param[in] cacheline_bytes System cacheline size in bytes.
 * @return Calculated optimal prefetch lookahead distance in bytes.
 */
MSIMD_API size_t   msimd_hinter_optimal_prefetch_distance(size_t cacheline_bytes);

/** @} */ /* End of MSIMD_HW_Hinter */'''
