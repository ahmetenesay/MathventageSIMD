# -*- coding: utf-8 -*-
"""
Script to decouple SacculusForge.h into:
1. include/SacculusForge.h (pure public API: Args feeding, sfGC, Gearkit, C++ Sfvm runtime)
2. src/sacfor_internal.h (internal headers, telemetry, VFS, pagefile, GPIO, sensor, ring0, upclocker, puzzle, JIT, primitiva, host GC)
"""

import re

with open("include/SacculusForge.h", "r", encoding="utf-8") as f:
    text = f.read()

with open("src/codegen/extracted_sfvm_cpp.inc", "r", encoding="utf-8") as f:
    sfvm_cpp = f.read()

# Clean up sfvm_cpp: remove #include "SacculusForge.h"
sfvm_cpp = sfvm_cpp.replace('#include "SacculusForge.h"\n', '')

# Parse sections
parts = re.split(r'(/\* ========================================================================= \*/\n/\* [^\n*]+ \*/\n/\* ========================================================================= \*/)', text)

preamble = parts[0]
sections = {}
for i in range(1, len(parts), 2):
    header = parts[i]
    content = parts[i+1]
    title = header.splitlines()[1].strip('/* ')
    sections[title] = (header, content)

# Section 5: OPAQUE INTERNAL RUNTIME HEADERS & KERNEL FUNCTION PROTOTYPES
s5_header, s5_content = sections["OPAQUE INTERNAL RUNTIME HEADERS & KERNEL FUNCTION PROTOTYPES"]
k_idx = s5_content.find("/**\n * @typedef SF_custominiVm_kernel_fn_t")
s5_internal = s5_content[:k_idx].strip()
s5_public = s5_content[k_idx:]

# Section 10: GENERATIONAL sfGC CONTROL & OBJECT ADMISSION API
s10_header, s10_content = sections["GENERATIONAL sfGC CONTROL & OBJECT ADMISSION API"]
c_idx = s10_content.find("/**\n * @brief Triggers an explicit, synchronous garbage collection sweep")
s10_internal = s10_content[:c_idx].strip()
s10_public = s10_content[c_idx:]

# Section 21: TENSOR SLICING & ANONYMOUS DISK PAGEFILE PAGING SPECIFICATION
s21_header, s21_content = sections["TENSOR SLICING & ANONYMOUS DISK PAGEFILE PAGING SPECIFICATION"]
p_idx = s21_content.find("/**\n * @enum SF_custominiVm_paging_policy_t")
s21_public = s21_content[:p_idx].rstrip() + "\n\n"
s21_internal = s21_content[p_idx:]

# Extract SF_custominiVm_scgc_type_t definition
scgc_type_decl = """/**
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
"""

# Section 27: GEARKIT TRANSMISSION ENGINE & DUAL-PROFILE API SPECIFICATION
gk_header, gk_content = sections["GEARKIT TRANSMISSION ENGINE & DUAL-PROFILE API SPECIFICATION"]

ext_idx = gk_content.find('#ifdef __cplusplus\n} // extern')
if ext_idx == -1:
    ext_idx = gk_content.find('} // extern')
assert ext_idx != -1, "Failed to find extern C closing in Gearkit section"

gk_c_part = gk_content[:ext_idx].strip()
gk_clean_c = scgc_type_decl + "\n\n" + gk_c_part + "\n\n#ifdef __cplusplus\n} // extern \"C\"\n#endif\n\n"

gk_clean_cpp = """#ifdef __cplusplus

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

""" + sfvm_cpp.rstrip() + "\n\n#endif // __cplusplus\n\n#endif // SACCULUS_FORGE_H\n"

# BUILD PUBLIC SacculusForge.h
pub_out = [preamble]
for title in [
    "CORE ARCHITECTURE CONSTANTS & AFFINITY BITMAP SPECIFICATION",
    "TENSOR DATA TYPES & BUFFER FLAGS",
    "TENSOR DESCRIPTOR (CACHE-ALIGNED C ABI COMPLIANT HANDLE)",
]:
    hdr, cnt = sections[title]
    pub_out.append(hdr + "\n" + cnt)

# Add Public Kernel Function Prototype
pub_out.append("""/* ========================================================================= */
/* KERNEL FUNCTION PROTOTYPE SPECIFICATION                                   */
/* ========================================================================= */
""" + s5_public)

for title in [
    "SFVM PUBLIC MANAGEMENT & INITIALIZATION API",
    "TENSOR LIFECYCLE & SLAB ALLOCATOR API",
    "KERNEL REGISTRATION, SYMBOL BINDING & DISPATCH API",
]:
    hdr, cnt = sections[title]
    pub_out.append(hdr + "\n" + cnt)

# Add Public Tensor Slicing API
pub_out.append("""/* ========================================================================= */
/* ZERO-COPY TENSOR SLICING (SUB-VIEW CREATION)                              */
/* ========================================================================= */
""" + s21_public)

# Add Public sfGC API
pub_out.append("""/* ========================================================================= */
/* GENERATIONAL sfGC CONTROL & LIFECYCLE SWEEP API                           */
/* ========================================================================= */
""" + s10_public)

for title in [
    "UNIFIED MEMORY ANATOMY & SYSTEM METRICS SPECIFICATION",
    "ADVANCED GC EXECUTION MODES, OWP & MTP CONTROL SPECIFICATION",
    "DETERMINISTIC CONSCIENTIOUS CARETAKER MANIFEST (\"SHAKE THE BED & SLEEP\")",
    "AUTOMATIC GC HEURISTICS & EXECUTION ARCHITECTURE PROFILES",
    "TWO-STAGE LIFECYCLE INITIALIZATION SPECIFICATION",
]:
    hdr, cnt = sections[title]
    pub_out.append(hdr + "\n" + cnt)

pub_out.append(gk_header + "\n" + gk_clean_c + gk_clean_cpp)

with open("include/SacculusForge.h", "w", encoding="utf-8") as f:
    f.write("".join(pub_out))
print("Wrote public include/SacculusForge.h! Size:", len("".join(pub_out)))

# BUILD INTERNAL src/sacfor_internal.h
# For Section 17: remove duplicate scgc_type definition
s17_hdr, s17_cnt = sections["SCGC SECONDARY TYPING & RAW PRIMITIVA SECTOR I/O API"]
t_end = s17_cnt.find("typedef SF_custominiVm_scgc_type_t sfScgcType;")
if t_end != -1:
    s17_cnt = s17_cnt[t_end + len("typedef SF_custominiVm_scgc_type_t sfScgcType;"):].strip()

internal_sections_to_add = [
    ("TELEMETRY & EVENT LOGGING STRUCTURES (OFF-HEAP CACHELINE-ISOLATED)", sections["TELEMETRY & EVENT LOGGING STRUCTURES (OFF-HEAP CACHELINE-ISOLATED)"]),
    ("LOCKLESS TELEMETRY LOG DRAINING API", sections["LOCKLESS TELEMETRY LOG DRAINING API"]),
    ("JIT BITCODE LOADING, SYMBOL RESOLUTION & SANDBOX TESTING", sections["JIT BITCODE LOADING, SYMBOL RESOLUTION & SANDBOX TESTING"]),
    ("INDEPENDENT PUZZLE ARENA & DYNAMIC ISA CANDIDATE EVALUATION", sections["INDEPENDENT PUZZLE ARENA & DYNAMIC ISA CANDIDATE EVALUATION"]),
    ("SCGC SECONDARY TYPING & RAW PRIMITIVA SECTOR I/O API", (s17_hdr, s17_cnt)),
    ("PUZZLE SUBMISSION & ISA CANDIDATE TOURNAMENT API", sections["PUZZLE SUBMISSION & ISA CANDIDATE TOURNAMENT API"]),
    ("HARDWARE TOPOLOGY, CACHE HIERARCHY & DRAM PROFILING SPECIFICATION", sections["HARDWARE TOPOLOGY, CACHE HIERARCHY & DRAM PROFILING SPECIFICATION"]),
    ("VIRTUAL FILE SYSTEM (VFS) & DIRECTORY-HEAP STORAGE API", sections["VIRTUAL FILE SYSTEM (VFS) & DIRECTORY-HEAP STORAGE API"]),
    ("SINGLE-BOARD COMPUTER (SBC) MMIO & ZERO-SYSCALL GPIO HARDWARE SPECIFICATION", sections["SINGLE-BOARD COMPUTER (SBC) MMIO & ZERO-SYSCALL GPIO HARDWARE SPECIFICATION"]),
    ("CONTINUOUS REAL-TIME SENSOR STREAMING SPECIFICATION", sections["CONTINUOUS REAL-TIME SENSOR STREAMING SPECIFICATION"]),
    ("RING-0 KERNEL BRIDGE (SFVM_K.sys / sfvm_k.ko) SPECIFICATION", sections["RING-0 KERNEL BRIDGE (SFVM_K.sys / sfvm_k.ko) SPECIFICATION"]),
    ("MULTI-LANGUAGE BINDING & HOST GC SYNCHRONIZATION API", sections["MULTI-LANGUAGE BINDING & HOST GC SYNCHRONIZATION API"]),
]

int_out = ["""#ifndef SACFOR_INTERNAL_H
#define SACFOR_INTERNAL_H

#include "SacculusForge.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */
/* OPAQUE INTERNAL RUNTIME HEADERS                                           */
/* ========================================================================= */
""" + s5_internal + "\n\n",
"""/* ========================================================================= */
/* INTERNAL sfGC OBJECT ADMISSION & GENERATIONAL HOOKS                       */
/* ========================================================================= */
""" + s10_internal + "\n\n",
"""/* ========================================================================= */
/* ANONYMOUS DISK PAGEFILE & VIRTUAL MEMORY SWAP SPECIFICATION               */
/* ========================================================================= */
""" + s21_internal + "\n\n"]

for title, (hdr, cnt) in internal_sections_to_add:
    int_out.append(hdr + "\n" + cnt + "\n\n")

int_out.append("""
#ifdef __cplusplus
}

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
#endif

#endif // SACFOR_INTERNAL_H
""")

with open("src/sacfor_internal.h", "w", encoding="utf-8") as f:
    f.write("".join(int_out))
print("Wrote internal src/sacfor_internal.h! Size:", len("".join(int_out)))
