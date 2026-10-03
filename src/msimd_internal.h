#ifndef MSIMD_INTERNAL_H
#define MSIMD_INTERNAL_H

#include "MathventageSIMD.h"
#include <cstddef>
#include <cstdint>

/**
 * =========================================================================
 * MathventageSIMD: Internal Engine Context & Lattice Data Structures
 * =========================================================================
 */

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

#endif // MSIMD_INTERNAL_H
