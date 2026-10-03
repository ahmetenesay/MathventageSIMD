#ifndef MSIMD_INTERNAL_H
#define MSIMD_INTERNAL_H

#include "MathventageSIMD.h"
#include <cstddef>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 * @typedef msimd_parallel_for_fn
 * @brief Callback function prototype for internal chunked parallel loop execution.
 *
 * @param[in,out] user_data Pointer to user context passed to the backend parallel_for runner.
 * @param[in]     start     Zero-based starting element index of the assigned chunk range.
 * @param[in]     end       Non-inclusive ending element index of the assigned chunk range.
 */
typedef void (*msimd_parallel_for_fn)(void* user_data, size_t start, size_t end);

/* Backend hooks */
msimd_status_t msimd_backend_fallback_init(msimd_context ctx);
void           msimd_backend_fallback_destroy(msimd_context ctx);
void           msimd_backend_fallback_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data);

msimd_status_t msimd_backend_accelerated_init(msimd_context ctx);
void           msimd_backend_accelerated_destroy(msimd_context ctx);
void           msimd_backend_accelerated_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data);

/* Leaf Math Kernels (Implemented in sf_kernel_math.cpp / SF_Native_Dispatcher.cpp) */
void msimd_kernel_wave_step_1d_fp32(float* next, const float* curr, const float* prev, size_t nx, float r2, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
void msimd_kernel_wave_step_3d_fp32(float* next, const float* curr, const float* prev, size_t nx, size_t ny, size_t nz, float rx, float ry, float rz, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

void msimd_kernel_maxwell_yee_3d(size_t nx, size_t ny, size_t nz,
                                 float* Ex, float* Ey, float* Ez,
                                 float* Hx, float* Hy, float* Hz,
                                 float dt, float dx, float dy, float dz,
                                 float mu, float eps, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

void msimd_kernel_lbm_d2q9(size_t nx, size_t ny,
                           const float* f_in, float* f_out,
                           float* rho, float* ux, float* uy,
                           float tau, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

void msimd_kernel_ray_triangle_batch8(const float ox[8], const float oy[8], const float oz[8],
                                      const float dx[8], const float dy[8], const float dz[8],
                                      const float v0[3], const float v1[3], const float v2[3],
                                      int hit[8], float t[8], float u[8], float v[8], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

void msimd_kernel_stockham_fft_radix2(size_t n,
                                      const float* in_real, const float* in_imag,
                                      float* out_real, float* out_imag,
                                      int inverse, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

#ifdef __cplusplus
}
#endif

#endif // MSIMD_INTERNAL_H
