/**
 * =========================================================================
 * MathventageSIMD: High-Performance Discrete Lattice & SIMD Acceleration Engine
 * =========================================================================
 * Pure C99/C11 Public API Header (MathventageSIMD.h)
 *
 * Exposes opaque handles and C-ABI stable interfaces.
 * Zero C++ templates, classes, or compiler intrinsics are exposed.
 */

#ifndef MATHVANTAGE_SIMD_C_API_H
#define MATHVANTAGE_SIMD_C_API_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef _WIN32
    #ifdef MATHVANTAGE_EXPORTS
        #define MSIMD_API __declspec(dllexport)
    #else
        #define MSIMD_API __declspec(dllimport)
    #endif
#else
    #define MSIMD_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* ========================================================================= */
/* FUNDAMENTAL MATHEMATICAL CONSTANTS                                        */
/* ========================================================================= */
#define MSIMD_PI                  3.141592653589793238462643383279502884
#define MSIMD_TWO_PI              6.283185307179586476925286766559005768
#define MSIMD_HALF_PI             1.570796326794896619231321691639751442
#define MSIMD_INV_PI              0.318309886183790671537767526745028724
#define MSIMD_SQRT_PI             1.772453850905516027298167483341145182
#define MSIMD_E                   2.718281828459045235360287471352662497  /* Euler's number e */
#define MSIMD_EULER_MASCHERONI    0.577215664901532860606512090082402431  /* Euler-Mascheroni constant gamma */
#define MSIMD_GOLDEN_RATIO        1.618033988749894848204586834365638117  /* Golden ratio phi */
#define MSIMD_SQRT_2              1.414213562373095048801688724209698078
#define MSIMD_INV_SQRT_2          0.707106781186547524400844362104849039
#define MSIMD_SQRT_3              1.732050807568877293527446341505872366
#define MSIMD_LN_2                0.693147180559945309417232121458176568
#define MSIMD_LN_10               2.302585092994045684017991454684364207
#define MSIMD_EPSILON             1e-12

/* ========================================================================= */
/* UNIVERSAL PHYSICAL CONSTANTS (NIST / CODATA 2022+ EXACT SI DEFINITIONS)   */
/* ========================================================================= */
#define MSIMD_LIGHT_SPEED         299792458.0              /* c: Speed of light in vacuum (m/s, exact SI definition) */
#define MSIMD_PLANCK_H            6.62607015e-34           /* h: Planck constant (J*s, exact SI definition) */
#define MSIMD_HBAR                1.0545718176461565e-34   /* hbar = h / (2*pi): Reduced Planck constant (J*s) */
#define MSIMD_GRAV_CONST          6.67430e-11              /* G: Newtonian gravitational constant (m^3 / (kg*s^2)) */
#define MSIMD_BOLTZMANN_K         1.380649e-23             /* k_B: Boltzmann constant (J/K, exact SI definition) */
#define MSIMD_AVOGADRO_N          6.02214076e+23           /* N_A: Avogadro constant (1/mol, exact SI definition) */
#define MSIMD_ELEMENTARY_CHARGE   1.602176634e-19          /* e: Elementary charge (Coulombs, exact SI definition) */
#define MSIMD_VACUUM_PERMEABILITY 1.25663706212e-6         /* mu_0: Vacuum magnetic permeability (N/A^2) */
#define MSIMD_VACUUM_PERMITTIVITY 8.8541878128e-12         /* eps_0 = 1/(mu_0*c^2): Vacuum electric permittivity (F/m) */
#define MSIMD_ELECTRON_MASS       9.1093837015e-31         /* m_e: Electron rest mass (kg) */
#define MSIMD_PROTON_MASS         1.67262192369e-27        /* m_p: Proton rest mass (kg) */
#define MSIMD_NEUTRON_MASS        1.67492749804e-27        /* m_n: Neutron rest mass (kg) */
#define MSIMD_FINE_STRUCTURE      7.2973525693e-3          /* alpha = e^2 / (4*pi*eps_0*hbar*c) (~1/137.035999) */
#define MSIMD_RYDBERG_CONST       10973731.568160          /* R_inf: Rydberg constant (1/m) */
#define MSIMD_STEFAN_BOLTZMANN    5.670374419e-8           /* sigma: Stefan-Boltzmann constant (W / (m^2*K^4)) */

/* ========================================================================= */
/* PLANCK NATURAL UNITS SYSTEM (CODATA / PLANCK SCALE QUANTIZATION)          */
/* ========================================================================= */
/* --- Base Planck Units --- */
#define MSIMD_PLANCK_LENGTH         1.616255e-35           /* ell_P = sqrt(hbar*G/c^3) (meters) */
#define MSIMD_PLANCK_TIME           5.391247e-44           /* t_P = sqrt(hbar*G/c^5) (seconds) */
#define MSIMD_PLANCK_MASS           2.176434e-08           /* m_P = sqrt(hbar*c/G) (kg) */
#define MSIMD_PLANCK_TEMP           1.416784e+32           /* T_P = (m_P*c^2)/k_B (Kelvin) */
#define MSIMD_PLANCK_CHARGE         1.875546e-18           /* q_P = sqrt(4*pi*eps_0*hbar*c) (Coulombs) */

/* --- Derived Planck Units --- */
#define MSIMD_PLANCK_AREA           2.612280e-70           /* A_P = ell_P^2 (m^2) */
#define MSIMD_PLANCK_VOLUME         4.222111e-105          /* V_P = ell_P^3 (m^3) */
#define MSIMD_PLANCK_MOMENTUM       6.524786               /* p_P = m_P * c (kg*m/s) */
#define MSIMD_PLANCK_ENERGY         1.956081e+09           /* E_P = m_P*c^2 (Joules, ~1.22e19 GeV) */
#define MSIMD_PLANCK_FORCE          1.210256e+44           /* F_P = c^4 / G (Newtons) */
#define MSIMD_PLANCK_POWER          3.628255e+52           /* P_P = c^5 / G (Watts, peak luminosity) */
#define MSIMD_PLANCK_DENSITY        5.15500e+96            /* rho_P = m_P / ell_P^3 (kg/m^3) */
#define MSIMD_PLANCK_ENERGY_DENSITY 4.63298e+113           /* rho_E,P = E_P / ell_P^3 (J/m^3) */
#define MSIMD_PLANCK_FREQUENCY      1.854859e+43           /* omega_P = 1 / t_P (s^-1 or rad/s) */
#define MSIMD_PLANCK_PRESSURE       4.63298e+113           /* P_P = c^7 / (hbar*G^2) (Pascals) */
#define MSIMD_PLANCK_ACCELERATION   5.56063e+51            /* a_P = c / t_P (m/s^2) */
#define MSIMD_PLANCK_CURRENT        3.478873e+25           /* I_P = q_P / t_P (Amperes) */
#define MSIMD_PLANCK_VOLTAGE        1.042940e+27           /* V_P = E_P / q_P (Volts) */
#define MSIMD_PLANCK_IMPEDANCE      29.9792458             /* Z_P = 1 / (4*pi*eps_0*c) = Z_0 / (4*pi) (Ohms) */

/* ========================================================================= */
/* Opaque Handles                                                            */
/* ========================================================================= */

typedef struct msimd_context_t* msimd_context;
typedef struct msimd_lattice_t* msimd_lattice;

/* ========================================================================= */
/* Enums & Return Codes                                                      */
/* ========================================================================= */

typedef enum msimd_status_t {
    MSIMD_SUCCESS                    =  0,
    MSIMD_ERROR_INVALID_ARGUMENT     = -1,
    MSIMD_ERROR_OUT_OF_MEMORY        = -2,
    MSIMD_ERROR_UNSUPPORTED_HARDWARE = -3,
    MSIMD_ERROR_BACKEND_FAILURE      = -4,
    MSIMD_ERROR_NOT_IMPLEMENTED      = -5,
    MSIMD_ERROR_HARDWARE_FAULT       = -6,
    MSIMD_ERROR_FLAG_CONFLICT        = -7
} msimd_status_t;

typedef enum msimd_isa_t {
    MSIMD_ISA_AUTO          = 0,
    MSIMD_ISA_AVX2          = 1,
    MSIMD_ISA_AVX512        = 2,
    MSIMD_ISA_NEON          = 3,
    MSIMD_ISA_SCALAR        = 4,
    MSIMD_ISA_SVE           = 5,
    MSIMD_ISA_SVE2          = 6,
    MSIMD_ISA_AMX           = 7,
    MSIMD_ISA_SME           = 8,
    MSIMD_ISA_RVV10         = 9,
    MSIMD_ISA_RVV071        = 10,
    MSIMD_ISA_LASX          = 11,
    MSIMD_ISA_LSX           = 12,
    MSIMD_ISA_ELBRUS_E2K    = 13,
    MSIMD_ISA_SHENWEI       = 14,
    MSIMD_ISA_ARM_DSP       = 15,
    MSIMD_ISA_XTENSA_PIE    = 16,
    MSIMD_ISA_ST_CORDIC     = 17,
    MSIMD_ISA_ST_FMAC       = 18,
    MSIMD_ISA_TI_TMU_VCU    = 19,
    MSIMD_ISA_RP2350_DP     = 20,
    MSIMD_ISA_SOFTFLOAT_Q31 = 21
} msimd_isa_t;

/* ========================================================================= */
/* MSC2020 MATHEMATICAL DISCIPLINES & HARDWARE EXECUTION TIERS               */
/* ========================================================================= */

/**
 * @enum msimd_msc2020_domain_t
 * @brief Mathematical Subject Classification (MSC2020) domain mapping for CPU kernel specializations.
 */
typedef enum msimd_msc2020_domain_t {
    MSIMD_MSC_NUMBER_THEORY     = 11, /* 11-XX, 14-XX: Number Theory and Arithmetic Geometry */
    MSIMD_MSC_LINEAR_ALGEBRA    = 15, /* 15-XX, 65Fxx: Linear Algebra and Matrix Theory */
    MSIMD_MSC_SPECIAL_FUNCTIONS = 33, /* 33-XX, 41-XX: Special Functions and Approximations */
    MSIMD_MSC_DIFFERENTIAL_EQ   = 34, /* 34-XX, 35-XX: Ordinary and Partial Differential Equations */
    MSIMD_MSC_COMBINATORICS     = 5,  /* 05-XX, 68Rxx: Combinatorics and Graph Theory */
    MSIMD_MSC_PROBABILITY       = 60, /* 60-XX, 65Cxx: Probability Theory and Stochastic Processes */
    MSIMD_MSC_CRYPTOLOGY        = 94, /* 94A60: Cryptology and Information Security */
    MSIMD_MSC_EMBEDDED_MCU      = 0   /* Low-Power / FPU-less Microcontrollers */
} msimd_msc2020_domain_t;

/* 6-Tier Hardware Execution Hierarchy (Tiers 0-5) */
#define MSIMD_TIER_0_MATRIX_TILES   0 /* Intel AMX, Arm SME, Apple AMX Matrix Coprocessors */
#define MSIMD_TIER_1_WIDE_VECTOR    1 /* AVX-512, SVE2, RVV 1.0, LASX (256/512-bit vectors) */
#define MSIMD_TIER_2_BASIC_SIMD     2 /* AVX2, ARM NEON, RVV 0.7.1, LSX, Elbrus E2K (128/256-bit) */
#define MSIMD_TIER_3_COPROCESSORS   3 /* CORDIC, FMAC, TMU, VCU, DSP, PIE Fixed-Function Coprocessors */
#define MSIMD_TIER_4_SCALAR_FPU     4 /* Standard IEEE 754 FP32/FP64 Scalar Floating-Point Hardware */
#define MSIMD_TIER_5_SOFTFLOAT_ALU  5 /* Pure Integer ALU Soft-Float & Q15/Q31 Fixed-Point Emulation */

typedef enum msimd_backend_t {
    MSIMD_BACKEND_AUTO     = 0,
    MSIMD_BACKEND_TBB      = 1,
    MSIMD_BACKEND_OPENVINO = 2,
    MSIMD_BACKEND_FALLBACK = 3
} msimd_backend_t;


/* ========================================================================= */
/* Tensor & Precision Target Structure for Computations & Puzzler             */
/* ========================================================================= */

/*
 * precision_target (msimd_tensor_t) describes the desired floating-point rounding precision.
 * It enables the puzzler to specify a target precision (FP32, FP64, etc.) to balance
 * performance against rounding loss. Passing nullptr (the default) uses the library's
 * internal default precision.
 */
#ifdef __cplusplus
#define MSIMD_DEFAULT_PRECISION = nullptr
#else
#define MSIMD_DEFAULT_PRECISION
#endif

typedef struct msimd_tensor_t {
    void*    data;
    int32_t  shape[8];
    int32_t  ndim;
    uint8_t  dtype;     /* 0: FP32, 1: FP64, 2: DUAL64, 3: INT32, 4: INT64, 5: FP128 */
    uint8_t  flags;
    uint8_t  status;
    uint8_t  _reserved;
    size_t   byte_size;
} msimd_tensor_t;

typedef msimd_tensor_t msimd_precision_target_t;

typedef enum msimd_precision_t {
    MSIMD_PRECISION_FP32   = 0,
    MSIMD_PRECISION_FP64   = 1,
    MSIMD_PRECISION_DUAL64 = 2,
    MSIMD_PRECISION_INT32  = 3,
    MSIMD_PRECISION_INT64  = 4,
    MSIMD_PRECISION_FP128  = 5
} msimd_precision_t;

/* Operational Profiles: Standard Dynamic vs Safety-Critical SC (Khronos Vulkan SC Model) */
typedef enum msimd_profile_mode_t {
    MSIMD_PROFILE_STANDARD        = 0, /* Full multi-gen sfGC, dynamic JIT sandbox, page swapping */
    MSIMD_PROFILE_SAFETY_CRITICAL = 1  /* SFVM-SC: Zero runtime allocations, static AOT dispatch, WCET bounded */
} msimd_profile_mode_t;

/* Unified Engine Configuration (Single Standard Distribution for Embedded & Desktop) */
typedef struct msimd_config_t {
    msimd_profile_mode_t profile;                 /* Standard (0) vs Safety-Critical SC (1) */
    size_t               static_arena_size_bytes; /* Pre-allocated arena size for SC mode (0 = default 16MB) */
    uint32_t             max_loop_iterations;     /* Upper bound for loops/stencils (0 = natural memory bound, no artificial ceiling; >0 = strict WCET cap) */
    bool                 forbid_runtime_heap;     /* Disallow malloc/new/mmap after initialization */
    void*                user_static_arena_buf;   /* Optional user-provided static buffer (e.g. TCM/SRAM on MCU) */
} msimd_config_t;

/* ========================================================================= */
/* Hardware Information                                                      */
/* ========================================================================= */

typedef struct msimd_cpu_info_t {
    char     vendor[16];
    char     brand[64];
    int      num_physical_cores;
    int      num_logical_cores;
    int      is_numa;
    int      has_avx2;
    int      has_avx512;
    int      has_amx;
    int      has_sve;
    int      has_sve2;
    int      has_sme;
    int      has_rvv;
    int      has_lasx;
    int      has_cordic;
    int      has_fmac;
    int      has_tmu;
    int      has_dsp;
    int      has_avx_vnni;
    int      has_fp128_hw;
    char     uops_arch[32];
    int      tier;
    int      l1_data_kb;
    int      l2_kb;
    int      l3_mb;
    int      is_hybrid;
    int      p_cores;
    int      e_cores;
    int      has_fma;
    int      has_avx512_vnni;
    int      has_avx512_bf16;
    int      has_avx512_fp16;
    int      optimal_fallback_level;
    double   calibrated_gflops;
} msimd_cpu_info_t;

/* =========================================================================
 * HW Topology Structs (HW-1: Cache / NUMA / Core Type)
 * ========================================================================= */

#define MSIMD_MAX_NUMA_NODES  16

/* Core type constants */
#define MSIMD_CORE_TYPE_UNKNOWN  0
#define MSIMD_CORE_TYPE_PCORE    1   /* Intel Performance core (Golden Cove) */
#define MSIMD_CORE_TYPE_ECORE    2   /* Intel Efficiency core (Gracemont)    */

/* L1/L2/L3 cache hierarchy -- CPUID leaf 0x04 / ARM CTR_EL0 */
typedef struct msimd_cache_topology_t {
    uint64_t l1d_size_bytes;
    uint64_t l2_size_bytes;
    uint64_t l3_size_bytes;
    uint32_t l1d_line_bytes;
    uint32_t l2_line_bytes;
    uint32_t l3_line_bytes;
    uint32_t cacheline_bytes;   /* Active cache line size (64 or 128) */
} msimd_cache_topology_t;

/* NUMA node topology -- Windows NUMA API / Linux sysfs */
typedef struct msimd_numa_topology_t {
    uint32_t num_numa_nodes;
    int      is_uma;                            /* 1 = UMA, 0 = multi-socket NUMA */
    uint64_t node_cpu_mask[MSIMD_MAX_NUMA_NODES];  /* Logical CPU bitmask per node */
} msimd_numa_topology_t;

/* Full hardware topology result */
typedef struct msimd_hw_topology_t {
    msimd_cache_topology_t cache;
    msimd_numa_topology_t  numa;
    uint32_t            num_logical_cores;
    uint32_t            num_physical_cores;
    int                 current_core_type;    /* MSIMD_CORE_TYPE_PCORE / ECORE */
    int                 has_hybrid_cores;     /* 1 = Intel Hybrid P+E */
} msimd_hw_topology_t;

/* ========================================================================= */
/* 128-Bit & Sub-128-Bit Packed Vector Types                                 */
/* ========================================================================= */

/* 1. Hexadecal 8-bit (16 x 8-bit = 128-bit) */
typedef struct { int8_t  v[16]; } msimd_i8x16_t;
typedef struct { uint8_t v[16]; } msimd_u8x16_t;

/* 1b. Octal 8-bit (8 x 8-bit = 64-bit) */
typedef struct { int8_t  v[8]; } msimd_i8x8_t;
typedef struct { uint8_t v[8]; } msimd_u8x8_t;

/* 1c. Hexal 8-bit (6 x 8-bit = 48-bit) */
typedef struct { int8_t  v[6]; } msimd_i8x6_t;
typedef struct { uint8_t v[6]; } msimd_u8x6_t;

/* 1d. Quad 8-bit (4 x 8-bit = 32-bit) */
typedef struct { int8_t  v[4]; } msimd_i8x4_t;
typedef struct { uint8_t v[4]; } msimd_u8x4_t;

/* 1e. Dual 8-bit (2 x 8-bit = 16-bit) */
typedef struct { int8_t  v[2]; } msimd_i8x2_t;
typedef struct { uint8_t v[2]; } msimd_u8x2_t;

/* 1f. 8-bit Floating Point (FP8: E4M3 & E5M2) */
typedef struct { uint8_t raw; } msimd_fp8_e4m3_t;
typedef struct { uint8_t raw; } msimd_fp8_e5m2_t;

/* 1g. Hexadecal 8-bit Float (16 x 8-bit FP8 = 128-bit) */
typedef struct { msimd_fp8_e4m3_t v[16]; } msimd_fp8x16_e4m3_t;
typedef struct { msimd_fp8_e5m2_t v[16]; } msimd_fp8x16_e5m2_t;

/* 2. Octal 16-bit (8 x 16-bit = 128-bit) */
typedef struct { int16_t  v[8]; } msimd_i16x8_t;
typedef struct { uint16_t v[8]; } msimd_u16x8_t;

/* 3. Quad 16-bit (4 x 16-bit = 64-bit) */
typedef struct { int16_t  v[4]; } msimd_i16x4_t;
typedef struct { uint16_t v[4]; } msimd_u16x4_t;

/* 4. Dual 16-bit (2 x 16-bit = 32-bit) */
typedef struct { int16_t  v[2]; } msimd_i16x2_t;
typedef struct { uint16_t v[2]; } msimd_u16x2_t;

/* 5. Quad 32-bit (4 x 32-bit = 128-bit) */
typedef struct { int32_t  v[4]; } msimd_i32x4_t;
typedef struct { uint32_t v[4]; } msimd_u32x4_t;
typedef struct { float    v[4]; } msimd_f32x4_t;

/* 6. Dual 32-bit (2 x 32-bit = 64-bit) */
typedef struct { int32_t  v[2]; } msimd_i32x2_t;
typedef struct { uint32_t v[2]; } msimd_u32x2_t;
typedef struct { float    v[2]; } msimd_f32x2_t;

/* 7. Dual 64-bit (2 x 64-bit = 128-bit) */
typedef struct { int64_t  v[2]; } msimd_i64x2_t;
typedef struct { uint64_t v[2]; } msimd_u64x2_t;
typedef struct { double   v[2]; } msimd_f64x2_t;

/* 8. Dual Complex FP32 (2 x Complex FP32 = 128-bit) */
typedef struct { float r0, i0, r1, i1; } msimd_cpx32x2_t;

/* 9. Single 128-bit Integer (Multi-limb / BigInt carry chain) */
typedef struct { uint64_t lo; uint64_t hi; } msimd_u128_t;
typedef struct { int64_t  lo; int64_t  hi; } msimd_i128_t;

/* 10. True IEEE 754-2008 binary128 Quad Precision (1 sign, 15 exp, 112 mantissa) */
typedef struct { uint64_t lo; uint64_t hi; } msimd_fp128_t;

/* Complex Number (Single Precision) */
typedef struct MSIMDComplex {
    float r, i;
} MSIMDComplex;

/* Axis-Aligned Bounding Box (AABB) */
typedef struct MSIMDAABB {
    float min_pt[3];
    float max_pt[3];
} MSIMDAABB;

/* Rigid Body Dynamics State */
typedef struct MSIMDRigidBody {
    float pos[3];
    float vel[3];
    float quat[4];
    float ang_vel[3];
    float mass;
    float inv_mass;
    float inertia[9];
    float inv_inertia_world[9];
    float force[3];
    float torque[3];
} MSIMDRigidBody;

/* Projected Gauss-Seidel (PGS) Contact Constraint */
typedef struct MSIMDContactConstraint {
    MSIMDRigidBody* bodyA;
    MSIMDRigidBody* bodyB;
    float rA[3];
    float rB[3];
    float normal[3];
    float tangent1[3];
    float tangent2[3];
    float penetration;
    float friction_coeff;
    float restitution;
    float normal_mass;
    float tangent1_mass;
    float tangent2_mass;
    float accumulated_normal_impulse;
    float accumulated_tangent1_impulse;
    float accumulated_tangent2_impulse;
    float bias;
} MSIMDContactConstraint;

/* Smoothed Particle Hydrodynamics (SPH) Particle State */
typedef struct MSIMDSPHParticle {
    float pos[3];
    float vel[3];
    float force[3];
    float density;
    float pressure;
    float mass;
} MSIMDSPHParticle;

/* ========================================================================= */
/* Context & Memory Lifecycle                                                */
/* ========================================================================= */

/**
 * @brief Initializes a primary MathventageSIMD engine execution context.
 *
 * Allocates and configures the core runtime context, probes host CPU instruction sets,
 * verifies vector register width capabilities, and establishes default thread worker pools.
 *
 * The selected ISA and backend determine code dispatch paths throughout the library\'s lifetime.
 * If MSIMD_ISA_AUTO is requested, the library executes runtime CPUID/CTR_EL0 feature detection
 * and selects the highest performing vector tier available on the processor.
 *
 * @param[out] out_ctx Pointer to an opaque context handle populated with the newly created instance.
 * @param[in]  isa     Target instruction set architecture (e.g. MSIMD_ISA_AUTO, AVX512, NEON, SVE2).
 * @param[in]  backend Target execution threading backend (e.g. MSIMD_BACKEND_AUTO, TBB, FALLBACK).
 * @return MSIMD_SUCCESS (0) on success, or an MSIMD negative error status code on failure.
 */
MSIMD_API msimd_status_t        msimd_init(msimd_context* out_ctx, msimd_isa_t isa, msimd_backend_t backend);

/**
 * @brief Initializes a MathventageSIMD context with an explicit operational configuration profile.
 *
 * Permits configuration of deterministic Safety-Critical (Khronos Vulkan SC / ISO 26262 ASIL-D)
 * profiles, static pre-allocated memory arenas, loop iteration bounds (WCET ceilings), and strict
 * heap allocation bans post-initialization.
 *
 * @param[out] out_ctx Pointer to an opaque context handle populated with the newly created instance.
 * @param[in]  isa     Target vector instruction set architecture.
 * @param[in]  backend Target execution threading backend.
 * @param[in]  config  Pointer to an msimd_config_t structure specifying operational profile settings.
 * @return MSIMD_SUCCESS (0) on success, or an MSIMD negative error status code on invalid settings.
 */
MSIMD_API msimd_status_t        msimd_init_with_config(msimd_context* out_ctx, msimd_isa_t isa, msimd_backend_t backend, const msimd_config_t* config);

/**
 * @brief Queries the active operational profile mode of an initialized engine context.
 *
 * @param[in] ctx Valid initialized MathventageSIMD engine context.
 * @return MSIMD_PROFILE_STANDARD (0) or MSIMD_PROFILE_SAFETY_CRITICAL (1).
 */
MSIMD_API msimd_profile_mode_t  msimd_get_active_profile(msimd_context ctx);

/**
 * @brief Retrieves a read-only pointer to the active engine configuration structure.
 *
 * @param[in] ctx Valid initialized MathventageSIMD engine context.
 * @return Pointer to the active msimd_config_t structure, or NULL if ctx is invalid.
 */
MSIMD_API const msimd_config_t* msimd_get_config(msimd_context ctx);

/**
 * @brief Destroys an active MathventageSIMD engine context and releases all associated resources.
 *
 * Terminates worker thread pools, releases aligned scratchpad buffers, unmaps static arenas,
 * and invalidates the context handle.
 *
 * @param[in] ctx Valid engine context handle to destroy.
 */
MSIMD_API void                  msimd_destroy(msimd_context ctx);

/**
 * @brief Queries comprehensive CPU capabilities, vector extensions, and cache dimensions.
 *
 * @param[in]  ctx      Valid engine context.
 * @param[out] out_info Pointer to an msimd_cpu_info_t structure populated with detected hardware features.
 * @return MSIMD_SUCCESS (0) on success, or an error status code if querying fails.
 */
MSIMD_API msimd_status_t        msimd_get_cpu_info(msimd_context ctx, msimd_cpu_info_t* out_info);

/**
 * @brief Calibrates computational throughput across available vector execution levels.
 *
 * Executes empirical micro-benchmarks across candidate vector instruction sets, measuring
 * real-time GFLOPS and determining the optimal fallback tier for mixed-workload processing.
 *
 * @param[in]  ctx            Valid engine context.
 * @param[out] out_best_level Pointer receiving the optimal hardware execution tier level index.
 * @param[out] out_gflops     Pointer receiving the peak measured floating-point throughput in GFLOPS.
 * @return MSIMD_SUCCESS (0) on success, or an error status code.
 */
MSIMD_API msimd_status_t        msimd_calibrate_pipeline(msimd_context ctx, int* out_best_level, double* out_gflops);

/**
 * @brief Allocates cacheline-aligned memory satisfying processor SIMD vector requirements.
 *
 * Guarantees strict address boundary alignment (e.g. 64 bytes for AVX-512 and ARM SVE),
 * preventing unaligned memory access penalties and page boundary split locks.
 *
 * @param[in] size      Number of contiguous bytes to allocate.
 * @param[in] alignment Memory boundary alignment in bytes (must be a power of two, >= sizeof(void*)).
 * @return Pointer to the aligned memory buffer, or NULL on allocation failure.
 */
MSIMD_API void* msimd_alloc_aligned(size_t size, size_t alignment);

/**
 * @brief Releases a memory block previously allocated via msimd_alloc_aligned().
 *
 * @param[in] ptr Pointer to the aligned memory buffer returned by msimd_alloc_aligned().
 */
MSIMD_API void  msimd_free_aligned(void* ptr);

/* ========================================================================= */
/* Lattice Management                                                        */
/* ========================================================================= */

/**
 * @brief Creates a multi-buffered spatial lattice for partial differential equation simulations.
 *
 * Allocates a structured grid with triple-buffering (previous, current, next) supporting
 * finite-difference time-domain (FDTD) wave mechanics, Lattice Boltzmann Methods (LBM),
 * elastodynamics, and reaction-diffusion kinetics.
 *
 * @param[in]  ctx         Valid engine context.
 * @param[out] out_lattice Pointer receiving the created opaque lattice handle.
 * @param[in]  dim         Spatial dimensionality of the lattice (1, 2, or 3).
 * @param[in]  nx          Number of grid cells along the x-axis.
 * @param[in]  ny          Number of grid cells along the y-axis (1 for 1D).
 * @param[in]  nz          Number of grid cells along the z-axis (1 for 1D/2D).
 * @param[in]  precision   Numerical precision format for field data (FP32, FP64, etc.).
 * @return MSIMD_SUCCESS (0) on success, or an error status code on failure.
 */
MSIMD_API msimd_status_t msimd_lattice_create(msimd_context ctx,
                                     msimd_lattice* out_lattice,
                                     int dim,
                                     size_t nx,
                                     size_t ny,
                                     size_t nz,
                                     msimd_precision_t precision);

/**
 * @brief Destroys an active simulation lattice and releases its field buffers.
 *
 * @param[in] lat Lattice handle to destroy.
 */
MSIMD_API void  msimd_lattice_destroy(msimd_lattice lat);

/**
 * @brief Retrieves the pointer to the current-time-step field buffer of the lattice.
 *
 * @param[in] lat Valid lattice handle.
 * @return Raw pointer to the contiguous current field array.
 */
MSIMD_API void* msimd_lattice_get_curr(msimd_lattice lat);

/**
 * @brief Retrieves the pointer to the previous-time-step field buffer of the lattice.
 *
 * @param[in] lat Valid lattice handle.
 * @return Raw pointer to the contiguous previous field array.
 */
MSIMD_API void* msimd_lattice_get_prev(msimd_lattice lat);

/**
 * @brief Retrieves the pointer to the next-time-step field buffer of the lattice.
 *
 * @param[in] lat Valid lattice handle.
 * @return Raw pointer to the contiguous next field array.
 */
MSIMD_API void* msimd_lattice_get_next(msimd_lattice lat);

/**
 * @brief Rotates the lattice time buffers in a circular swap (prev <- curr <- next).
 *
 * Advances the simulation timeline by one time step with zero data copying.
 *
 * @param[in] lat Valid lattice handle.
 */
MSIMD_API void  msimd_lattice_swap(msimd_lattice lat);

/* ========================================================================= */
/* SECTION 1: PURE LINEAR ALGEBRA & TENSOR MATHEMATICS (C-API)              */
/* ========================================================================= */

/**
 * @defgroup MSIMD_LinearAlgebra Pure Linear Algebra & Vector Mathematics
 * @brief High-performance SIMD-accelerated 2D, 3D, and 4D vector, matrix, and quaternion operations.
 * @{
 */

/* ------------------------------------------------------------------------- */
/* 2D Vectors (Single and Double Precision)                                  */
/* ------------------------------------------------------------------------- */

/**
 * @brief Adds two 2D single-precision vectors: out = a + b.
 * @param[in]  a                 Pointer to the first 2D vector [x, y].
 * @param[in]  b                 Pointer to the second 2D vector [x, y].
 * @param[out] out               Pointer to the destination 2D vector buffer [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 */
MSIMD_API void msimd_vec2f_add(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Subtracts two 2D single-precision vectors: out = a - b.
 * @param[in]  a                 Pointer to the minuend 2D vector [x, y].
 * @param[in]  b                 Pointer to the subtrahend 2D vector [x, y].
 * @param[out] out               Pointer to the destination 2D vector buffer [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 */
MSIMD_API void msimd_vec2f_sub(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Scales a 2D single-precision vector by a scalar factor: out = a * s.
 * @param[in]  a                 Pointer to the input 2D vector [x, y].
 * @param[in]  s                 Scalar scaling multiplier.
 * @param[out] out               Pointer to the destination 2D vector buffer [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 */
MSIMD_API void msimd_vec2f_scale(const float* a, float s, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Euclidean dot product of two 2D single-precision vectors: a . b.
 * @param[in]  a                 Pointer to the first 2D vector [x, y].
 * @param[in]  b                 Pointer to the second 2D vector [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 * @return Scalar dot product result (a.x * b.x + a.y * b.y).
 */
MSIMD_API float msimd_vec2f_dot(const float* a, const float* b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the squared Euclidean length of a 2D single-precision vector: ||a||^2.
 * @param[in]  a                 Pointer to the input 2D vector [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 * @return Squared vector length (a.x^2 + a.y^2).
 */
MSIMD_API float msimd_vec2f_length_sq(const float* a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Euclidean length of a 2D single-precision vector: ||a||.
 * @param[in]  a                 Pointer to the input 2D vector [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 * @return Vector length sqrt(a.x^2 + a.y^2).
 */
MSIMD_API float msimd_vec2f_length(const float* a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Normalizes a 2D single-precision vector to unit length: out = a / ||a||.
 * @param[in]  a                 Pointer to the input 2D vector [x, y].
 * @param[out] out               Pointer to the destination unit vector [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 */
MSIMD_API void msimd_vec2f_normalize(const float* a, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the linear interpolation between two 2D single-precision vectors: out = a + t * (b - a).
 * @param[in]  a                 Starting vector [x, y] at t = 0.
 * @param[in]  b                 Ending vector [x, y] at t = 1.
 * @param[in]  t                 Interpolation parameter in range [0, 1].
 * @param[out] out               Pointer to the destination 2D vector buffer [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 */
MSIMD_API void msimd_vec2f_lerp(const float* a, const float* b, float t, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Reflects an incident 2D single-precision vector across a surface normal: out = v - 2 * (v . n) * n.
 * @param[in]  v                 Incident vector [x, y].
 * @param[in]  n                 Surface unit normal vector [x, y].
 * @param[out] out               Reflected vector buffer [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 */
MSIMD_API void msimd_vec2f_reflect(const float* v, const float* n, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Adds two 2D double-precision vectors: out = a + b.
 * @param[in]  a                 Pointer to the first 2D vector [x, y].
 * @param[in]  b                 Pointer to the second 2D vector [x, y].
 * @param[out] out               Pointer to the destination 2D vector buffer [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 */
MSIMD_API void msimd_vec2d_add(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Subtracts two 2D double-precision vectors: out = a - b.
 * @param[in]  a                 Pointer to the minuend 2D vector [x, y].
 * @param[in]  b                 Pointer to the subtrahend 2D vector [x, y].
 * @param[out] out               Pointer to the destination 2D vector buffer [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 */
MSIMD_API void msimd_vec2d_sub(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Scales a 2D double-precision vector by a scalar factor: out = a * s.
 * @param[in]  a                 Pointer to the input 2D vector [x, y].
 * @param[in]  s                 Scalar scaling multiplier.
 * @param[out] out               Pointer to the destination 2D vector buffer [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 */
MSIMD_API void msimd_vec2d_scale(const double* a, double s, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Euclidean dot product of two 2D double-precision vectors: a . b.
 * @param[in]  a                 Pointer to the first 2D vector [x, y].
 * @param[in]  b                 Pointer to the second 2D vector [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 * @return Scalar dot product result (a.x * b.x + a.y * b.y).
 */
MSIMD_API double msimd_vec2d_dot(const double* a, const double* b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the squared Euclidean length of a 2D double-precision vector: ||a||^2.
 * @param[in]  a                 Pointer to the input 2D vector [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 * @return Squared vector length (a.x^2 + a.y^2).
 */
MSIMD_API double msimd_vec2d_length_sq(const double* a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Euclidean length of a 2D double-precision vector: ||a||.
 * @param[in]  a                 Pointer to the input 2D vector [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 * @return Vector length sqrt(a.x^2 + a.y^2).
 */
MSIMD_API double msimd_vec2d_length(const double* a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Normalizes a 2D double-precision vector to unit length: out = a / ||a||.
 * @param[in]  a                 Pointer to the input 2D vector [x, y].
 * @param[out] out               Pointer to the destination unit vector [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 */
MSIMD_API void msimd_vec2d_normalize(const double* a, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the linear interpolation between two 2D double-precision vectors: out = a + t * (b - a).
 * @param[in]  a                 Starting vector [x, y] at t = 0.
 * @param[in]  b                 Ending vector [x, y] at t = 1.
 * @param[in]  t                 Interpolation parameter in range [0, 1].
 * @param[out] out               Pointer to the destination 2D vector buffer [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 */
MSIMD_API void msimd_vec2d_lerp(const double* a, const double* b, double t, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Reflects an incident 2D double-precision vector across a surface normal: out = v - 2 * (v . n) * n.
 * @param[in]  v                 Incident vector [x, y].
 * @param[in]  n                 Surface unit normal vector [x, y].
 * @param[out] out               Reflected vector buffer [x, y].
 * @param[in]  precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 */
MSIMD_API void msimd_vec2d_reflect(const double* v, const double* n, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* ------------------------------------------------------------------------- */
/* 3D Vectors (Single and Double Precision)                                  */
/* ------------------------------------------------------------------------- */

/**
 * @brief Adds two 3D single-precision vectors: out = a + b.
 * @param[in]  a                 Pointer to first 3D vector [x, y, z].
 * @param[in]  b                 Pointer to second 3D vector [x, y, z].
 * @param[out] out               Pointer to destination 3D vector buffer [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3f_add(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Subtracts two 3D single-precision vectors: out = a - b.
 * @param[in]  a                 Pointer to minuend 3D vector [x, y, z].
 * @param[in]  b                 Pointer to subtrahend 3D vector [x, y, z].
 * @param[out] out               Pointer to destination 3D vector buffer [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3f_sub(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Scales a 3D single-precision vector by a scalar factor: out = a * s.
 * @param[in]  a                 Pointer to input 3D vector [x, y, z].
 * @param[in]  s                 Scalar scaling multiplier.
 * @param[out] out               Pointer to destination 3D vector buffer [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3f_scale(const float* a, float s, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Euclidean dot product of two 3D single-precision vectors: a . b.
 * @param[in]  a                 Pointer to first 3D vector [x, y, z].
 * @param[in]  b                 Pointer to second 3D vector [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Scalar dot product result (a.x*b.x + a.y*b.y + a.z*b.z).
 */
MSIMD_API float msimd_vec3f_dot(const float* a, const float* b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the squared Euclidean length of a 3D single-precision vector: ||a||^2.
 * @param[in]  a                 Pointer to input 3D vector [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Squared vector length (a.x^2 + a.y^2 + a.z^2).
 */
MSIMD_API float msimd_vec3f_length_sq(const float* a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Euclidean length of a 3D single-precision vector: ||a||.
 * @param[in]  a                 Pointer to input 3D vector [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Vector length sqrt(a.x^2 + a.y^2 + a.z^2).
 */
MSIMD_API float msimd_vec3f_length(const float* a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Normalizes a 3D single-precision vector to unit length: out = a / ||a||.
 * @param[in]  a                 Pointer to input 3D vector [x, y, z].
 * @param[out] out               Pointer to destination unit vector [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3f_normalize(const float* a, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the cross product of two 3D single-precision vectors: out = a x b.
 * @param[in]  a                 Pointer to first 3D vector [x, y, z].
 * @param[in]  b                 Pointer to second 3D vector [x, y, z].
 * @param[out] out               Pointer to destination cross product vector [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3f_cross(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes linear interpolation between two 3D single-precision vectors: out = a + t * (b - a).
 * @param[in]  a                 Starting vector [x, y, z] at t = 0.
 * @param[in]  b                 Ending vector [x, y, z] at t = 1.
 * @param[in]  t                 Interpolation parameter in range [0, 1].
 * @param[out] out               Pointer to destination 3D vector buffer [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3f_lerp(const float* a, const float* b, float t, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Reflects an incident 3D single-precision vector across a surface normal: out = v - 2 * (v . n) * n.
 * @param[in]  v                 Incident 3D vector [x, y, z].
 * @param[in]  n                 Surface unit normal vector [x, y, z].
 * @param[out] out               Reflected 3D vector buffer [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3f_reflect(const float* v, const float* n, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Adds two 3D double-precision vectors: out = a + b.
 * @param[in]  a                 Pointer to first 3D vector [x, y, z].
 * @param[in]  b                 Pointer to second 3D vector [x, y, z].
 * @param[out] out               Pointer to destination 3D vector buffer [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3d_add(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Subtracts two 3D double-precision vectors: out = a - b.
 * @param[in]  a                 Pointer to minuend 3D vector [x, y, z].
 * @param[in]  b                 Pointer to subtrahend 3D vector [x, y, z].
 * @param[out] out               Pointer to destination 3D vector buffer [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3d_sub(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Scales a 3D double-precision vector by a scalar factor: out = a * s.
 * @param[in]  a                 Pointer to input 3D vector [x, y, z].
 * @param[in]  s                 Scalar scaling multiplier.
 * @param[out] out               Pointer to destination 3D vector buffer [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3d_scale(const double* a, double s, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Euclidean dot product of two 3D double-precision vectors: a . b.
 * @param[in]  a                 Pointer to first 3D vector [x, y, z].
 * @param[in]  b                 Pointer to second 3D vector [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Scalar dot product result (a.x*b.x + a.y*b.y + a.z*b.z).
 */
MSIMD_API double msimd_vec3d_dot(const double* a, const double* b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the squared Euclidean length of a 3D double-precision vector: ||a||^2.
 * @param[in]  a                 Pointer to input 3D vector [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Squared vector length (a.x^2 + a.y^2 + a.z^2).
 */
MSIMD_API double msimd_vec3d_length_sq(const double* a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Euclidean length of a 3D double-precision vector: ||a||.
 * @param[in]  a                 Pointer to input 3D vector [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Vector length sqrt(a.x^2 + a.y^2 + a.z^2).
 */
MSIMD_API double msimd_vec3d_length(const double* a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Normalizes a 3D double-precision vector to unit length: out = a / ||a||.
 * @param[in]  a                 Pointer to input 3D vector [x, y, z].
 * @param[out] out               Pointer to destination unit vector [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3d_normalize(const double* a, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the cross product of two 3D double-precision vectors: out = a x b.
 * @param[in]  a                 Pointer to first 3D vector [x, y, z].
 * @param[in]  b                 Pointer to second 3D vector [x, y, z].
 * @param[out] out               Pointer to destination cross product vector [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3d_cross(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes linear interpolation between two 3D double-precision vectors: out = a + t * (b - a).
 * @param[in]  a                 Starting vector [x, y, z] at t = 0.
 * @param[in]  b                 Ending vector [x, y, z] at t = 1.
 * @param[in]  t                 Interpolation parameter in range [0, 1].
 * @param[out] out               Pointer to destination 3D vector buffer [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3d_lerp(const double* a, const double* b, double t, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Reflects an incident 3D double-precision vector across a surface normal: out = v - 2 * (v . n) * n.
 * @param[in]  v                 Incident 3D vector [x, y, z].
 * @param[in]  n                 Surface unit normal vector [x, y, z].
 * @param[out] out               Reflected 3D vector buffer [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec3d_reflect(const double* v, const double* n, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* ------------------------------------------------------------------------- */
/* 4D Vectors (Single and Double Precision)                                  */
/* ------------------------------------------------------------------------- */

/**
 * @brief Adds two 4D single-precision vectors: out = a + b.
 * @param[in]  a                 Pointer to first 4D vector [x, y, z, w].
 * @param[in]  b                 Pointer to second 4D vector [x, y, z, w].
 * @param[out] out               Pointer to destination 4D vector buffer [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec4f_add(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Subtracts two 4D single-precision vectors: out = a - b.
 * @param[in]  a                 Pointer to minuend 4D vector [x, y, z, w].
 * @param[in]  b                 Pointer to subtrahend 4D vector [x, y, z, w].
 * @param[out] out               Pointer to destination 4D vector buffer [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec4f_sub(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Scales a 4D single-precision vector by a scalar factor: out = a * s.
 * @param[in]  a                 Pointer to input 4D vector [x, y, z, w].
 * @param[in]  s                 Scalar scaling multiplier.
 * @param[out] out               Pointer to destination 4D vector buffer [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec4f_scale(const float* a, float s, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Euclidean dot product of two 4D single-precision vectors: a . b.
 * @param[in]  a                 Pointer to first 4D vector [x, y, z, w].
 * @param[in]  b                 Pointer to second 4D vector [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Scalar dot product result (a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w).
 */
MSIMD_API float msimd_vec4f_dot(const float* a, const float* b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the squared Euclidean length of a 4D single-precision vector: ||a||^2.
 * @param[in]  a                 Pointer to input 4D vector [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Squared vector length (a.x^2 + a.y^2 + a.z^2 + a.w^2).
 */
MSIMD_API float msimd_vec4f_length_sq(const float* a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Euclidean length of a 4D single-precision vector: ||a||.
 * @param[in]  a                 Pointer to input 4D vector [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Vector length sqrt(a.x^2 + a.y^2 + a.z^2 + a.w^2).
 */
MSIMD_API float msimd_vec4f_length(const float* a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Normalizes a 4D single-precision vector to unit length: out = a / ||a||.
 * @param[in]  a                 Pointer to input 4D vector [x, y, z, w].
 * @param[out] out               Pointer to destination unit vector [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec4f_normalize(const float* a, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes linear interpolation between two 4D single-precision vectors: out = a + t * (b - a).
 * @param[in]  a                 Starting vector [x, y, z, w] at t = 0.
 * @param[in]  b                 Ending vector [x, y, z, w] at t = 1.
 * @param[in]  t                 Interpolation parameter in range [0, 1].
 * @param[out] out               Pointer to destination 4D vector buffer [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec4f_lerp(const float* a, const float* b, float t, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Adds two 4D double-precision vectors: out = a + b.
 * @param[in]  a                 Pointer to first 4D vector [x, y, z, w].
 * @param[in]  b                 Pointer to second 4D vector [x, y, z, w].
 * @param[out] out               Pointer to destination 4D vector buffer [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec4d_add(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Subtracts two 4D double-precision vectors: out = a - b.
 * @param[in]  a                 Pointer to minuend 4D vector [x, y, z, w].
 * @param[in]  b                 Pointer to subtrahend 4D vector [x, y, z, w].
 * @param[out] out               Pointer to destination 4D vector buffer [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec4d_sub(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Scales a 4D double-precision vector by a scalar factor: out = a * s.
 * @param[in]  a                 Pointer to input 4D vector [x, y, z, w].
 * @param[in]  s                 Scalar scaling multiplier.
 * @param[out] out               Pointer to destination 4D vector buffer [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec4d_scale(const double* a, double s, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Euclidean dot product of two 4D double-precision vectors: a . b.
 * @param[in]  a                 Pointer to first 4D vector [x, y, z, w].
 * @param[in]  b                 Pointer to second 4D vector [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Scalar dot product result (a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w).
 */
MSIMD_API double msimd_vec4d_dot(const double* a, const double* b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the squared Euclidean length of a 4D double-precision vector: ||a||^2.
 * @param[in]  a                 Pointer to input 4D vector [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Squared vector length (a.x^2 + a.y^2 + a.z^2 + a.w^2).
 */
MSIMD_API double msimd_vec4d_length_sq(const double* a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Euclidean length of a 4D double-precision vector: ||a||.
 * @param[in]  a                 Pointer to input 4D vector [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Vector length sqrt(a.x^2 + a.y^2 + a.z^2 + a.w^2).
 */
MSIMD_API double msimd_vec4d_length(const double* a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Normalizes a 4D double-precision vector to unit length: out = a / ||a||.
 * @param[in]  a                 Pointer to input 4D vector [x, y, z, w].
 * @param[out] out               Pointer to destination unit vector [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec4d_normalize(const double* a, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes linear interpolation between two 4D double-precision vectors: out = a + t * (b - a).
 * @param[in]  a                 Starting vector [x, y, z, w] at t = 0.
 * @param[in]  b                 Ending vector [x, y, z, w] at t = 1.
 * @param[in]  t                 Interpolation parameter in range [0, 1].
 * @param[out] out               Pointer to destination 4D vector buffer [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_vec4d_lerp(const double* a, const double* b, double t, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* ------------------------------------------------------------------------- */
/* 2x2 Matrices (Row-Major, FP32 and FP64)                                   */
/* ------------------------------------------------------------------------- */

/**
 * @brief Multiplies two 2x2 single-precision matrices: out = a * b (row-major).
 * @param[in]  a                 Pointer to 2x2 left-hand matrix (4 floats).
 * @param[in]  b                 Pointer to 2x2 right-hand matrix (4 floats).
 * @param[out] out               Pointer to 2x2 output matrix buffer (4 floats).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat2f_mul(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Multiplies a 2x2 single-precision matrix by a 2D vector: out = m * v.
 * @param[in]  m                 Pointer to 2x2 matrix (4 floats).
 * @param[in]  v                 Pointer to 2D vector (2 floats).
 * @param[out] out               Pointer to 2D output vector buffer (2 floats).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat2f_mul_vec(const float* m, const float* v, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the determinant of a 2x2 single-precision matrix: det(m) = m00*m11 - m01*m10.
 * @param[in]  m                 Pointer to 2x2 matrix (4 floats).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Determinant scalar value.
 */
MSIMD_API float msimd_mat2f_det(const float* m, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Inverts a 2x2 single-precision matrix: out = m^-1.
 * @param[in]  m                 Pointer to 2x2 input matrix (4 floats).
 * @param[out] out               Pointer to 2x2 inverted matrix buffer (4 floats).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return 0 on success, non-zero if matrix is singular (det near zero).
 */
MSIMD_API int msimd_mat2f_inv(const float* m, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Multiplies two 2x2 double-precision matrices: out = a * b (row-major).
 * @param[in]  a                 Pointer to 2x2 left-hand matrix (4 doubles).
 * @param[in]  b                 Pointer to 2x2 right-hand matrix (4 doubles).
 * @param[out] out               Pointer to 2x2 output matrix buffer (4 doubles).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat2d_mul(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Multiplies a 2x2 double-precision matrix by a 2D vector: out = m * v.
 * @param[in]  m                 Pointer to 2x2 matrix (4 doubles).
 * @param[in]  v                 Pointer to 2D vector (2 doubles).
 * @param[out] out               Pointer to 2D output vector buffer (2 doubles).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat2d_mul_vec(const double* m, const double* v, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the determinant of a 2x2 double-precision matrix: det(m) = m00*m11 - m01*m10.
 * @param[in]  m                 Pointer to 2x2 matrix (4 doubles).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Determinant scalar value.
 */
MSIMD_API double msimd_mat2d_det(const double* m, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Inverts a 2x2 double-precision matrix: out = m^-1.
 * @param[in]  m                 Pointer to 2x2 input matrix (4 doubles).
 * @param[out] out               Pointer to 2x2 inverted matrix buffer (4 doubles).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return 0 on success, non-zero if matrix is singular (det near zero).
 */
MSIMD_API int msimd_mat2d_inv(const double* m, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* ------------------------------------------------------------------------- */
/* 3x3 Matrices (Row-Major, FP32 and FP64)                                   */
/* ------------------------------------------------------------------------- */

/**
 * @brief Multiplies two 3x3 single-precision matrices: out = a * b (row-major).
 * @param[in]  a                 Pointer to 3x3 left-hand matrix (9 floats).
 * @param[in]  b                 Pointer to 3x3 right-hand matrix (9 floats).
 * @param[out] out               Pointer to 3x3 output matrix buffer (9 floats).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat3f_mul(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Multiplies a 3x3 single-precision matrix by a 3D vector: out = m * v.
 * @param[in]  m                 Pointer to 3x3 matrix (9 floats).
 * @param[in]  v                 Pointer to 3D vector (3 floats).
 * @param[out] out               Pointer to 3D output vector buffer (3 floats).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat3f_mul_vec(const float* m, const float* v, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the determinant of a 3x3 single-precision matrix via Sarrus/Laplace rule.
 * @param[in]  m                 Pointer to 3x3 matrix (9 floats).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Determinant scalar value.
 */
MSIMD_API float msimd_mat3f_det(const float* m, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Inverts a 3x3 single-precision matrix via cofactor expansion: out = m^-1.
 * @param[in]  m                 Pointer to 3x3 input matrix (9 floats).
 * @param[out] out               Pointer to 3x3 inverted matrix buffer (9 floats).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return 0 on success, non-zero if matrix is singular.
 */
MSIMD_API int msimd_mat3f_inv(const float* m, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Multiplies two 3x3 double-precision matrices: out = a * b (row-major).
 * @param[in]  a                 Pointer to 3x3 left-hand matrix (9 doubles).
 * @param[in]  b                 Pointer to 3x3 right-hand matrix (9 doubles).
 * @param[out] out               Pointer to 3x3 output matrix buffer (9 doubles).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat3d_mul(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Multiplies a 3x3 double-precision matrix by a 3D vector: out = m * v.
 * @param[in]  m                 Pointer to 3x3 matrix (9 doubles).
 * @param[in]  v                 Pointer to 3D vector (3 doubles).
 * @param[out] out               Pointer to 3D output vector buffer (3 doubles).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat3d_mul_vec(const double* m, const double* v, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the determinant of a 3x3 double-precision matrix via Sarrus/Laplace rule.
 * @param[in]  m                 Pointer to 3x3 matrix (9 doubles).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Determinant scalar value.
 */
MSIMD_API double msimd_mat3d_det(const double* m, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Inverts a 3x3 double-precision matrix via cofactor expansion: out = m^-1.
 * @param[in]  m                 Pointer to 3x3 input matrix (9 doubles).
 * @param[out] out               Pointer to 3x3 inverted matrix buffer (9 doubles).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return 0 on success, non-zero if matrix is singular.
 */
MSIMD_API int msimd_mat3d_inv(const double* m, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* ------------------------------------------------------------------------- */
/* 4x4 Matrices (Row-Major, FP32 and FP64)                                   */
/* ------------------------------------------------------------------------- */

/**
 * @brief Multiplies two 4x4 single-precision matrices: out = a * b (row-major).
 * @param[in]  a                 Pointer to 4x4 left-hand matrix (16 floats).
 * @param[in]  b                 Pointer to 4x4 right-hand matrix (16 floats).
 * @param[out] out               Pointer to 4x4 output matrix buffer (16 floats).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat4f_mul(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Multiplies a 4x4 single-precision matrix by a 4D vector: out = m * v.
 * @param[in]  m                 Pointer to 4x4 matrix (16 floats).
 * @param[in]  v                 Pointer to 4D vector (4 floats).
 * @param[out] out               Pointer to 4D output vector buffer (4 floats).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat4f_mul_vec(const float* m, const float* v, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the determinant of a 4x4 single-precision matrix.
 * @param[in]  m                 Pointer to 4x4 matrix (16 floats).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Determinant scalar value.
 */
MSIMD_API float msimd_mat4f_det(const float* m, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Inverts a 4x4 single-precision matrix via 2x2 block matrix decomposition: out = m^-1.
 * @param[in]  m                 Pointer to 4x4 input matrix (16 floats).
 * @param[out] out               Pointer to 4x4 inverted matrix buffer (16 floats).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return 0 on success, non-zero if matrix is singular.
 */
MSIMD_API int msimd_mat4f_inv(const float* m, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Multiplies two 4x4 double-precision matrices: out = a * b (row-major).
 * @param[in]  a                 Pointer to 4x4 left-hand matrix (16 doubles).
 * @param[in]  b                 Pointer to 4x4 right-hand matrix (16 doubles).
 * @param[out] out               Pointer to 4x4 output matrix buffer (16 doubles).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat4d_mul(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Multiplies a 4x4 double-precision matrix by a 4D vector: out = m * v.
 * @param[in]  m                 Pointer to 4x4 matrix (16 doubles).
 * @param[in]  v                 Pointer to 4D vector (4 doubles).
 * @param[out] out               Pointer to 4D output vector buffer (4 doubles).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat4d_mul_vec(const double* m, const double* v, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the determinant of a 4x4 double-precision matrix.
 * @param[in]  m                 Pointer to 4x4 matrix (16 doubles).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return Determinant scalar value.
 */
MSIMD_API double msimd_mat4d_det(const double* m, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Inverts a 4x4 double-precision matrix via 2x2 block matrix decomposition: out = m^-1.
 * @param[in]  m                 Pointer to 4x4 input matrix (16 doubles).
 * @param[out] out               Pointer to 4x4 inverted matrix buffer (16 doubles).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 * @return 0 on success, non-zero if matrix is singular.
 */
MSIMD_API int msimd_mat4d_inv(const double* m, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* ------------------------------------------------------------------------- */
/* Unit Quaternions & Spatial Rotations (FP32)                               */
/* ------------------------------------------------------------------------- */

/**
 * @brief Multiplies two unit quaternions (Hamilton product): out = q1 * q2 [x, y, z, w].
 * @param[in]  q1                First quaternion [x, y, z, w].
 * @param[in]  q2                Second quaternion [x, y, z, w].
 * @param[out] out               Product quaternion buffer [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quat_mul(const float* q1, const float* q2, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Converts a unit quaternion [x, y, z, w] to a 3x3 rotation matrix (row-major).
 * @param[in]  q                 Unit rotation quaternion [x, y, z, w].
 * @param[out] m                 Pointer to 3x3 row-major rotation matrix buffer (9 floats).
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quat_to_mat3(const float* q, float* m, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Rotates a 3D vector by a unit quaternion: out = q * v * q^-1.
 * @param[in]  q                 Unit rotation quaternion [x, y, z, w].
 * @param[in]  v                 Input 3D vector [x, y, z].
 * @param[out] out               Rotated 3D vector buffer [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quat_rotate_vec(const float* q, const float* v, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes spherical linear interpolation (SLERP) between two quaternions: out = slerp(q0, q1, t).
 * @param[in]  q0                Starting quaternion [x, y, z, w] at t = 0.
 * @param[in]  q1                Ending quaternion [x, y, z, w] at t = 1.
 * @param[in]  t                 Interpolation factor in range [0, 1].
 * @param[out] out               Interpolated quaternion buffer [x, y, z, w].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quat_slerp(const float* q0, const float* q1, float t, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Transforms a 3D point using a dual quaternion (SE(3) rigid body transformation).
 * @param[in]  real              Real/rotation quaternion [x, y, z, w].
 * @param[in]  dual              Dual/translation quaternion [x, y, z, w].
 * @param[in]  p                 Input 3D point [x, y, z].
 * @param[out] out               Transformed 3D point buffer [x, y, z].
 * @param[in]  precision_target  Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_dual_quat_transform_point_f32(const float* real, const float* dual, const float* p, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_LinearAlgebra */

/* ========================================================================= */
/* Wave Physics Solvers                                                      */
/* ========================================================================= */

/**
 * @defgroup MSIMD_WavePhysics Wave Equation & Staggered Lattice Physics Solvers
 * @brief Discrete finite-difference time-domain (FDTD) wave solvers on multi-dimensional lattices.
 * @{
 */

/**
 * @brief Solves one time step of the 1D acoustic/electromagnetic scalar wave equation:
 *        d^2 u / dt^2 = c^2 * (d^2 u / dx^2).
 * @param[in]     ctx               Valid initialized MathventageSIMD engine context.
 * @param[in,out] lat               Opaque handle to the 1D discrete lattice buffer.
 * @param[in]     c                 Wave phase velocity propagation speed (m/s).
 * @param[in]     dt                Time step interval (seconds). Must satisfy Courant stability: c*dt/dx <= 1.
 * @param[in]     dx                Spatial grid spacing along the x-axis (meters).
 * @param[in]     precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 * @return MSIMD_SUCCESS on successful step computation, or error status code.
 */
MSIMD_API msimd_status_t msimd_wave_step_1d(msimd_context ctx,
                                   msimd_lattice lat,
                                   double c,
                                   double dt,
                                   double dx, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Solves one time step of the 3D scalar wave equation on an orthogonal grid:
 *        d^2 u / dt^2 = c^2 * nabla^2 u.
 * @param[in]     ctx               Valid initialized MathventageSIMD engine context.
 * @param[in,out] lat               Opaque handle to the 3D discrete lattice buffer.
 * @param[in]     c                 Wave phase velocity propagation speed (m/s).
 * @param[in]     dt                Time step interval (seconds). Must satisfy 3D CFL condition.
 * @param[in]     dx                Spatial grid spacing along x-axis (meters).
 * @param[in]     dy                Spatial grid spacing along y-axis (meters).
 * @param[in]     dz                Spatial grid spacing along z-axis (meters).
 * @param[in]     precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 * @return MSIMD_SUCCESS on successful step computation, or error status code.
 */
MSIMD_API msimd_status_t msimd_wave_step_3d(msimd_context ctx,
                                   msimd_lattice lat,
                                   double c,
                                   double dt,
                                   double dx,
                                   double dy,
                                   double dz, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Dispatches multiple consecutive simulation steps across all available CPU threads in parallel.
 * @param[in]     ctx               Valid initialized MathventageSIMD engine context.
 * @param[in,out] lat               Opaque handle to the discrete lattice buffer.
 * @param[in]     num_steps         Number of time steps to iterate.
 * @param[in]     precision_target  Optional precision descriptor for Puzzler SIMD dispatch (default: nullptr).
 * @return MSIMD_SUCCESS on completion, or error status code.
 */
MSIMD_API msimd_status_t msimd_dispatch_step_parallel(msimd_context ctx,
                                             msimd_lattice lat,
                                             int num_steps, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_WavePhysics */

/* ========================================================================= */
/* Golden Seeds: Domain-Specific Leaf Kernels                                */
/* ========================================================================= */

/**
 * @defgroup MSIMD_GoldenSeeds Golden Seeds: High-Density Domain-Specific Leaf Kernels
 * @brief Specialized hardware-optimized leaf compute kernels for electrodynamics, CFD, raytracing, and spectral FFT.
 * @{
 */

/**
 * @brief Performs one 3D Maxwell finite-difference time-domain (FDTD) staggered Yee lattice step.
 *        Updates the magnetic field H from curl(E) and electric field E from curl(H).
 * @param[in]     ctx               Valid initialized MathventageSIMD engine context.
 * @param[in]     nx                Grid cell count along the x-axis.
 * @param[in]     ny                Grid cell count along the y-axis.
 * @param[in]     nz                Grid cell count along the z-axis.
 * @param[in,out] Ex                Electric field x-component array [nx * ny * nz].
 * @param[in,out] Ey                Electric field y-component array [nx * ny * nz].
 * @param[in,out] Ez                Electric field z-component array [nx * ny * nz].
 * @param[in,out] Hx                Magnetic field x-component array [nx * ny * nz].
 * @param[in,out] Hy                Magnetic field y-component array [nx * ny * nz].
 * @param[in,out] Hz                Magnetic field z-component array [nx * ny * nz].
 * @param[in]     dt                Time step size (seconds).
 * @param[in]     dx                Spatial cell size along x (meters).
 * @param[in]     dy                Spatial cell size along y (meters).
 * @param[in]     dz                Spatial cell size along z (meters).
 * @param[in]     mu                Magnetic permeability of the medium (H/m).
 * @param[in]     eps               Electric permittivity of the medium (F/m).
 * @param[in]     precision_target  Optional precision descriptor (default: nullptr).
 * @return MSIMD_SUCCESS on success, or error status code.
 */
MSIMD_API msimd_status_t msimd_maxwell_yee_3d_step(msimd_context ctx,
                                          size_t nx, size_t ny, size_t nz,
                                          float* Ex, float* Ey, float* Ez,
                                          float* Hx, float* Hy, float* Hz,
                                          float dt, float dx, float dy, float dz,
                                          float mu, float eps, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Performs one 2D Lattice Boltzmann Method (LBM) D2Q9 fluid dynamics collision and streaming step.
 *        Applies single-relaxation-time Bhatnagar-Gross-Krook (BGK) collision followed by neighbor streaming.
 * @param[in]     ctx               Valid initialized MathventageSIMD engine context.
 * @param[in]     nx                Lattice width along the x-axis.
 * @param[in]     ny                Lattice height along the y-axis.
 * @param[in]     f_in              Incoming particle distribution functions [9 * nx * ny].
 * @param[out]    f_out             Outgoing post-collision and streamed distribution functions [9 * nx * ny].
 * @param[out]    rho               Macroscopic fluid density field [nx * ny].
 * @param[out]    ux                Macroscopic fluid velocity x-component field [nx * ny].
 * @param[out]    uy                Macroscopic fluid velocity y-component field [nx * ny].
 * @param[in]     tau               Dimensionless BGK relaxation time parameter (related to kinematic viscosity).
 * @param[in]     precision_target  Optional precision descriptor (default: nullptr).
 * @return MSIMD_SUCCESS on success, or error status code.
 */
MSIMD_API msimd_status_t msimd_lbm_d2q9_step(msimd_context ctx,
                                    size_t nx, size_t ny,
                                    const float* f_in,
                                    float* f_out,
                                    float* rho,
                                    float* ux,
                                    float* uy,
                                    float tau, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized Moller-Trumbore ray-triangle batch intersection test for 8 rays in parallel.
 * @param[in]     ctx               Valid initialized MathventageSIMD engine context.
 * @param[in]     ox                Ray origin x-coordinates (8 floats).
 * @param[in]     oy                Ray origin y-coordinates (8 floats).
 * @param[in]     oz                Ray origin z-coordinates (8 floats).
 * @param[in]     dx                Ray direction x-coordinates (8 floats).
 * @param[in]     dy                Ray direction y-coordinates (8 floats).
 * @param[in]     dz                Ray direction z-coordinates (8 floats).
 * @param[in]     v0                First triangle vertex position [x, y, z].
 * @param[in]     v1                Second triangle vertex position [x, y, z].
 * @param[in]     v2                Third triangle vertex position [x, y, z].
 * @param[out]    hit               Hit boolean flags array (8 ints, 1 if hit, 0 otherwise).
 * @param[out]    t                 Ray intersection hit distances (8 floats).
 * @param[out]    u                 Barycentric u-coordinates at intersection (8 floats).
 * @param[out]    v                 Barycentric v-coordinates at intersection (8 floats).
 * @param[in]     precision_target  Optional precision descriptor (default: nullptr).
 * @return MSIMD_SUCCESS on success, or error status code.
 */
MSIMD_API msimd_status_t msimd_ray_triangle_batch8(msimd_context ctx,
                                          const float ox[8], const float oy[8], const float oz[8],
                                          const float dx[8], const float dy[8], const float dz[8],
                                          const float v0[3], const float v1[3], const float v2[3],
                                          int hit[8], float t[8], float u[8], float v[8], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes one Radix-2 Stockham discrete Fourier transform butterfly stage without bit-reversal.
 * @param[in]     ctx               Valid initialized MathventageSIMD engine context.
 * @param[in]     n                 Total number of complex frequency/time domain samples (must be a power of 2).
 * @param[in]     in_real           Input real component array of length n.
 * @param[in]     in_imag           Input imaginary component array of length n.
 * @param[out]    out_real          Output real component array of length n.
 * @param[out]    out_imag          Output imaginary component array of length n.
 * @param[in]     inverse           Direction flag: 0 for forward FFT, 1 for inverse FFT (IFFT).
 * @param[in]     precision_target  Optional precision descriptor (default: nullptr).
 * @return MSIMD_SUCCESS on success, or error status code.
 */
MSIMD_API msimd_status_t msimd_stockham_fft_radix2(msimd_context ctx,
                                          size_t n,
                                          const float* in_real,
                                          const float* in_imag,
                                          float* out_real,
                                          float* out_imag,
                                          int inverse, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_GoldenSeeds */

/* ========================================================================= */
/* 128-Bit & Sub-128-Bit Vector Acceleration Operations                      */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Vector128 128-Bit & Sub-128-Bit Vector Acceleration Primitives
 * @brief Vectorized arithmetic, saturation, shuffling, and dot products across 8-bit, 16-bit, 32-bit, and 64-bit lanes.
 *
 * Designed to map directly to hardware vector intrinsics (Intel SSE/AVX, ARM NEON, RISC-V Vector,
 * and WebAssembly 128-bit SIMD), these functions provide deterministic single-instruction multiple-data
 * throughput for packed integer and floating-point computations.
 * @{
 */

/* --- Hexadecal 8-bit (16x8) --- */

/**
 * @brief Vectorized 16-lane unsigned 8-bit integer addition with modulo wrapping: out[i] = (a[i] + b[i]) mod 256.
 *
 * Operates across sixteen independent 8-bit integer lanes simultaneously. Employs hardware
 * vector instructions (`_mm_add_epi8` on x86, `vaddq_u8` on ARM NEON). If overflow occurs within any
 * lane, the result wraps symmetrically according to two's-complement modular arithmetic without
 * affecting adjacent lanes.
 *
 * @param[in] a                First 16-byte packed vector operand.
 * @param[in] b                Second 16-byte packed vector operand.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Resulting 16-byte packed vector.
 */
MSIMD_API msimd_u8x16_t msimd_vec128_u8_add(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 16-lane unsigned 8-bit integer subtraction with modulo wrapping: out[i] = (a[i] - b[i]) mod 256.
 *
 * Performs parallel lane-wise subtraction across 16 bytes. Wraps on borrow. Maps directly to
 * `_mm_sub_epi8` or `vsubq_u8`.
 *
 * @param[in] a                Minuend 16-byte packed vector.
 * @param[in] b                Subtrahend 16-byte packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Difference 16-byte packed vector.
 */
MSIMD_API msimd_u8x16_t msimd_vec128_u8_sub(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 16-lane unsigned 8-bit addition with upper saturation: out[i] = min(a[i] + b[i], 255).
 *
 * Clamps each 8-bit result to the maximum representable unsigned byte value (0xFF = 255).
 * Ideal for image pixel blending, color composition, and quantized signal processing where
 * overflow wrapping causes catastrophic visual inversion artifacts.
 *
 * @param[in] a                First 16-byte packed vector operand.
 * @param[in] b                Second 16-byte packed vector operand.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Saturated 16-byte packed vector.
 */
MSIMD_API msimd_u8x16_t msimd_vec128_u8_adds_sat(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 16-lane unsigned 8-bit subtraction with lower saturation: out[i] = max(a[i] - b[i], 0).
 *
 * Clamps each byte result to zero upon underflow. Eliminates negative wrapping in color processing
 * and threshold subtraction algorithms.
 *
 * @param[in] a                Minuend 16-byte packed vector.
 * @param[in] b                Subtrahend 16-byte packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Saturated 16-byte packed vector.
 */
MSIMD_API msimd_u8x16_t msimd_vec128_u8_subs_sat(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes lane-wise minimum across sixteen unsigned 8-bit integer elements: out[i] = min(a[i], b[i]).
 *
 * Compares corresponding bytes in parallel and selects the lesser unsigned value for each lane.
 *
 * @param[in] a                First 16-byte packed vector.
 * @param[in] b                Second 16-byte packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Vector containing the minimum value for each lane.
 */
MSIMD_API msimd_u8x16_t msimd_vec128_u8_min(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes lane-wise maximum across sixteen unsigned 8-bit integer elements: out[i] = max(a[i], b[i]).
 *
 * @param[in] a                First 16-byte packed vector.
 * @param[in] b                Second 16-byte packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Vector containing the maximum value for each lane.
 */
MSIMD_API msimd_u8x16_t msimd_vec128_u8_max(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Permutes bytes in vector a according to indices specified in the mask vector.
 *
 * Maps to hardware byte shuffle instructions (`_mm_shuffle_epi8` / SSSE3 `pshufb` on x86,
 * `vtbl1_u8` on ARM NEON). For each lane i, if mask[i] has its high bit set (>= 128),
 * the output lane is cleared to 0; otherwise, out[i] = a[mask[i] & 0x0F].
 *
 * @param[in] a                Source 16-byte vector containing data bytes to permute.
 * @param[in] mask             Control mask specifying source byte indices for each destination lane.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Shuffled 16-byte packed vector.
 */
MSIMD_API msimd_u8x16_t msimd_vec128_u8_shuffle(msimd_u8x16_t a, msimd_u8x16_t mask, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* --- Octal 16-bit (8x16) --- */

/**
 * @brief Vectorized 8-lane signed 16-bit integer addition: out[i] = (a[i] + b[i]) mod 65536.
 *
 * Performs parallel two's-complement 16-bit addition across eight lanes simultaneously.
 *
 * @param[in] a                First 8-word packed vector.
 * @param[in] b                Second 8-word packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Sum 8-word packed vector.
 */
MSIMD_API msimd_i16x8_t msimd_vec128_i16_add(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 8-lane signed 16-bit integer subtraction: out[i] = (a[i] - b[i]) mod 65536.
 *
 * @param[in] a                Minuend 8-word packed vector.
 * @param[in] b                Subtrahend 8-word packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Difference 8-word packed vector.
 */
MSIMD_API msimd_i16x8_t msimd_vec128_i16_sub(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 8-lane signed 16-bit multiplication retaining lower 16 bits of the 32-bit products.
 *
 * Multiplies eight pairs of signed 16-bit integers and stores the low 16 bits of each 32-bit product.
 * Maps to `_mm_mullo_epi16` on x86 and `vmulq_s16` on ARM NEON.
 *
 * @param[in] a                First 8-word packed vector.
 * @param[in] b                Second 8-word packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Truncated product 8-word packed vector.
 */
MSIMD_API msimd_i16x8_t msimd_vec128_i16_mullo(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 8-lane signed 16-bit addition with signed saturation to [-32768, 32767].
 *
 * @param[in] a                First 8-word packed vector.
 * @param[in] b                Second 8-word packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Saturated 8-word packed vector.
 */
MSIMD_API msimd_i16x8_t msimd_vec128_i16_adds_sat(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 8-lane signed 16-bit subtraction with signed saturation to [-32768, 32767].
 *
 * @param[in] a                Minuend 8-word packed vector.
 * @param[in] b                Subtrahend 8-word packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Saturated 8-word packed vector.
 */
MSIMD_API msimd_i16x8_t msimd_vec128_i16_subs_sat(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Multiplies signed 16-bit integers in a and b, horizontally adds adjacent 32-bit products, and packs into four 32-bit integers.
 *
 * Computes: out[0] = a[0]*b[0] + a[1]*b[1]; out[1] = a[2]*b[2] + a[3]*b[3];
 * out[2] = a[4]*b[4] + a[5]*b[5]; out[3] = a[6]*b[6] + a[7]*b[7].
 * Maps directly to `_mm_madd_epi16` on x86, offering high-throughput fused multiply-add for dot products.
 *
 * @param[in] a                First 8-word packed vector.
 * @param[in] b                Second 8-word packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Vector of four 32-bit signed integers.
 */
MSIMD_API msimd_i32x4_t msimd_vec128_i16_madd(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* --- Quad 16-bit (4x16 in 64-bit) --- */

/**
 * @brief Vectorized 4-lane signed 16-bit addition in a 64-bit sub-vector: out[i] = a[i] + b[i].
 *
 * @param[in] a                First 4-lane 16-bit vector.
 * @param[in] b                Second 4-lane 16-bit vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Resulting 4-lane vector.
 */
MSIMD_API msimd_i16x4_t msimd_vec64_i16_add(msimd_i16x4_t a, msimd_i16x4_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 4-lane signed 16-bit subtraction in a 64-bit sub-vector: out[i] = a[i] - b[i].
 *
 * @param[in] a                Minuend 4-lane vector.
 * @param[in] b                Subtrahend 4-lane vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Difference 4-lane vector.
 */
MSIMD_API msimd_i16x4_t msimd_vec64_i16_sub(msimd_i16x4_t a, msimd_i16x4_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 4-lane signed 16-bit addition with saturation to [-32768, 32767].
 *
 * @param[in] a                First 4-lane vector.
 * @param[in] b                Second 4-lane vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Saturated 4-lane vector.
 */
MSIMD_API msimd_i16x4_t msimd_vec64_i16_adds_sat(msimd_i16x4_t a, msimd_i16x4_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* --- Dual 16-bit (2x16 in 32-bit) --- */

/**
 * @brief Vectorized 2-lane signed 16-bit addition in a packed 32-bit register.
 *
 * @param[in] a                First 2-lane 16-bit vector.
 * @param[in] b                Second 2-lane 16-bit vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Sum 2-lane vector.
 */
MSIMD_API msimd_i16x2_t msimd_vec32_i16_add(msimd_i16x2_t a, msimd_i16x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 2-lane signed 16-bit subtraction in a packed 32-bit register.
 *
 * @param[in] a                Minuend 2-lane vector.
 * @param[in] b                Subtrahend 2-lane vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Difference 2-lane vector.
 */
MSIMD_API msimd_i16x2_t msimd_vec32_i16_sub(msimd_i16x2_t a, msimd_i16x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* --- Quad 32-bit (4x32 FP32 & INT32) --- */

/**
 * @brief Vectorized 4-lane single-precision floating-point addition: out[i] = a[i] + b[i].
 *
 * Executes four parallel IEEE 754 single-precision floating-point additions simultaneously.
 * Maps directly to `_mm_add_ps` on x86 and `vaddq_f32` on ARM NEON.
 *
 * @param[in] a                First 4-float packed vector.
 * @param[in] b                Second 4-float packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Resulting 4-float vector.
 */
MSIMD_API msimd_f32x4_t msimd_vec128_f32_add(msimd_f32x4_t a, msimd_f32x4_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 4-lane single-precision floating-point subtraction: out[i] = a[i] - b[i].
 *
 * @param[in] a                Minuend 4-float packed vector.
 * @param[in] b                Subtrahend 4-float packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Difference 4-float vector.
 */
MSIMD_API msimd_f32x4_t msimd_vec128_f32_sub(msimd_f32x4_t a, msimd_f32x4_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 4-lane single-precision floating-point multiplication: out[i] = a[i] * b[i].
 *
 * @param[in] a                First 4-float packed vector.
 * @param[in] b                Second 4-float packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Product 4-float vector.
 */
MSIMD_API msimd_f32x4_t msimd_vec128_f32_mul(msimd_f32x4_t a, msimd_f32x4_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 4-lane fused multiply-addition: out[i] = (a[i] * b[i]) + c[i].
 *
 * Evaluates product and sum with only a single rounding stage when hardware FMA3/FMA4 is present,
 * maintaining maximum numerical precision and doubling effective floating-point throughput.
 *
 * @param[in] a                Multiplicand 4-float vector.
 * @param[in] b                Multiplier 4-float vector.
 * @param[in] c                Addend 4-float vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Fused result 4-float vector.
 */
MSIMD_API msimd_f32x4_t msimd_vec128_f32_fmadd(msimd_f32x4_t a, msimd_f32x4_t b, msimd_f32x4_t c, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes lane-wise square root across four single-precision floating-point elements: out[i] = sqrt(a[i]).
 *
 * Employs hardware square-root instructions (`_mm_sqrt_ps` on x86, `vsqrtq_f32` on ARM).
 *
 * @param[in] a                Source 4-float vector (elements must be >= 0.0f).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Square root 4-float vector.
 */
MSIMD_API msimd_f32x4_t msimd_vec128_f32_sqrt(msimd_f32x4_t a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the scalar dot product of two 4-component single-precision vectors: dot = sum(a[i] * b[i]).
 *
 * @param[in] a                First 4-float vector.
 * @param[in] b                Second 4-float vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Scalar dot product floating-point value.
 */
MSIMD_API float      msimd_vec128_f32_dot(msimd_f32x4_t a, msimd_f32x4_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 4-lane signed 32-bit integer addition: out[i] = a[i] + b[i].
 *
 * @param[in] a                First 4-integer vector.
 * @param[in] b                Second 4-integer vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Sum 4-integer vector.
 */
MSIMD_API msimd_i32x4_t msimd_vec128_i32_add(msimd_i32x4_t a, msimd_i32x4_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 4-lane signed 32-bit integer subtraction: out[i] = a[i] - b[i].
 *
 * @param[in] a                Minuend 4-integer vector.
 * @param[in] b                Subtrahend 4-integer vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Difference 4-integer vector.
 */
MSIMD_API msimd_i32x4_t msimd_vec128_i32_sub(msimd_i32x4_t a, msimd_i32x4_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 4-lane signed 32-bit integer multiplication retaining the lower 32 bits: out[i] = a[i] * b[i].
 *
 * @param[in] a                First 4-integer vector.
 * @param[in] b                Second 4-integer vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Product 4-integer vector.
 */
MSIMD_API msimd_i32x4_t msimd_vec128_i32_mullo(msimd_i32x4_t a, msimd_i32x4_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* --- Dual 32-bit (2x32 in 64-bit) --- */

/**
 * @brief Vectorized 2-lane single-precision floating-point addition in a 64-bit vector: out[i] = a[i] + b[i].
 *
 * @param[in] a                First 2-float vector.
 * @param[in] b                Second 2-float vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Sum 2-float vector.
 */
MSIMD_API msimd_f32x2_t msimd_vec64_f32_add(msimd_f32x2_t a, msimd_f32x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 2-lane single-precision floating-point subtraction in a 64-bit vector: out[i] = a[i] - b[i].
 *
 * @param[in] a                Minuend 2-float vector.
 * @param[in] b                Subtrahend 2-float vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Difference 2-float vector.
 */
MSIMD_API msimd_f32x2_t msimd_vec64_f32_sub(msimd_f32x2_t a, msimd_f32x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 2-lane single-precision floating-point multiplication in a 64-bit vector: out[i] = a[i] * b[i].
 *
 * @param[in] a                First 2-float vector.
 * @param[in] b                Second 2-float vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Product 2-float vector.
 */
MSIMD_API msimd_f32x2_t msimd_vec64_f32_mul(msimd_f32x2_t a, msimd_f32x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* --- Dual 64-bit (2x64 FP64 & INT64) --- */

/**
 * @brief Vectorized 2-lane double-precision floating-point addition: out[i] = a[i] + b[i].
 *
 * Maps to `_mm_add_pd` on x86 and `vaddq_f64` on ARM64 NEON.
 *
 * @param[in] a                First 2-double packed vector.
 * @param[in] b                Second 2-double packed vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Sum 2-double vector.
 */
MSIMD_API msimd_f64x2_t msimd_vec128_f64_add(msimd_f64x2_t a, msimd_f64x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 2-lane double-precision floating-point subtraction: out[i] = a[i] - b[i].
 *
 * @param[in] a                Minuend 2-double vector.
 * @param[in] b                Subtrahend 2-double vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Difference 2-double vector.
 */
MSIMD_API msimd_f64x2_t msimd_vec128_f64_sub(msimd_f64x2_t a, msimd_f64x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 2-lane double-precision floating-point multiplication: out[i] = a[i] * b[i].
 *
 * @param[in] a                First 2-double vector.
 * @param[in] b                Second 2-double vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Product 2-double vector.
 */
MSIMD_API msimd_f64x2_t msimd_vec128_f64_mul(msimd_f64x2_t a, msimd_f64x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 2-lane double-precision fused multiply-addition: out[i] = (a[i] * b[i]) + c[i].
 *
 * @param[in] a                Multiplicand 2-double vector.
 * @param[in] b                Multiplier 2-double vector.
 * @param[in] c                Addend 2-double vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Fused result 2-double vector.
 */
MSIMD_API msimd_f64x2_t msimd_vec128_f64_fmadd(msimd_f64x2_t a, msimd_f64x2_t b, msimd_f64x2_t c, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 2-lane double-precision square root: out[i] = sqrt(a[i]).
 *
 * @param[in] a                Source 2-double vector (elements >= 0.0).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Square root 2-double vector.
 */
MSIMD_API msimd_f64x2_t msimd_vec128_f64_sqrt(msimd_f64x2_t a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 2-lane signed 64-bit integer addition: out[i] = a[i] + b[i].
 *
 * @param[in] a                First 2-integer vector.
 * @param[in] b                Second 2-integer vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Sum 2-integer vector.
 */
MSIMD_API msimd_i64x2_t msimd_vec128_i64_add(msimd_i64x2_t a, msimd_i64x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized 2-lane signed 64-bit integer subtraction: out[i] = a[i] - b[i].
 *
 * @param[in] a                Minuend 2-integer vector.
 * @param[in] b                Subtrahend 2-integer vector.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Difference 2-integer vector.
 */
MSIMD_API msimd_i64x2_t msimd_vec128_i64_sub(msimd_i64x2_t a, msimd_i64x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* --- Dual Complex FP32 (2x Complex in 128-bit) --- */

/**
 * @brief Vectorized addition of two pairs of single-precision complex numbers.
 *
 * Evaluates: (r0_a + r0_b) + i*(i0_a + i0_b) and (r1_a + r1_b) + i*(i1_a + i1_b).
 *
 * @param[in] a                First packed pair of complex numbers.
 * @param[in] b                Second packed pair of complex numbers.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Sum of complex pairs.
 */
MSIMD_API msimd_cpx32x2_t msimd_vec128_cpx32_add(msimd_cpx32x2_t a, msimd_cpx32x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized subtraction of two pairs of single-precision complex numbers.
 *
 * @param[in] a                Minuend complex pair.
 * @param[in] b                Subtrahend complex pair.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Difference of complex pairs.
 */
MSIMD_API msimd_cpx32x2_t msimd_vec128_cpx32_sub(msimd_cpx32x2_t a, msimd_cpx32x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Vectorized complex multiplication for two pairs of single-precision complex numbers.
 *
 * Evaluates: out_r = a_r*b_r - a_i*b_i; out_i = a_r*b_i + a_i*b_r for both complex lanes simultaneously.
 *
 * @param[in] a                First complex pair.
 * @param[in] b                Second complex pair.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Product complex pair.
 */
MSIMD_API msimd_cpx32x2_t msimd_vec128_cpx32_mul(msimd_cpx32x2_t a, msimd_cpx32x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the complex conjugate of two packed single-precision complex numbers.
 *
 * Inverts the algebraic sign of the imaginary components: out = [r0, -i0, r1, -i1].
 *
 * @param[in] a                Input complex pair.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Conjugated complex pair.
 */
MSIMD_API msimd_cpx32x2_t msimd_vec128_cpx32_conj(msimd_cpx32x2_t a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the squared magnitude for both complex lanes: out = [r0^2 + i0^2, r1^2 + i1^2].
 *
 * @param[in] a                Input complex pair.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Packed 2-float vector containing the squared magnitudes.
 */
MSIMD_API msimd_f32x2_t   msimd_vec128_cpx32_mag2(msimd_cpx32x2_t a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* --- Single 128-bit Integer Carry Chain & Extended ISA --- */

/**
 * @brief Performs 128-bit unsigned integer addition with multi-limb carry propagation.
 *
 * Evaluates sum across two 64-bit limbs (lo, hi), propagating the overflow carry bit from the
 * lower limb into the higher limb using ADC (Add with Carry) processor instructions.
 *
 * @param[in] a                First 128-bit unsigned integer.
 * @param[in] b                Second 128-bit unsigned integer.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return 128-bit sum integer.
 */
MSIMD_API msimd_u128_t msimd_u128_add(msimd_u128_t a, msimd_u128_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Performs 128-bit unsigned integer subtraction with multi-limb borrow propagation.
 *
 * @param[in] a                Minuend 128-bit unsigned integer.
 * @param[in] b                Subtrahend 128-bit unsigned integer.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return 128-bit difference integer.
 */
MSIMD_API msimd_u128_t msimd_u128_sub(msimd_u128_t a, msimd_u128_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes 128-bit unsigned integer multiplication retaining the lower 128 bits of the product.
 *
 * Employs hardware 64x64->128 bit widening multiply instructions (`_mulx_u64` / `mulq`) to evaluate
 * full cross-limb partial products.
 *
 * @param[in] a                First 128-bit unsigned integer.
 * @param[in] b                Second 128-bit unsigned integer.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Lower 128 bits of the product.
 */
MSIMD_API msimd_u128_t msimd_u128_mul(msimd_u128_t a, msimd_u128_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Performs a logical left shift on a 128-bit unsigned integer: out = a << shift.
 *
 * @param[in] a                128-bit operand to shift.
 * @param[in] shift            Bit shift count in the range [0, 127].
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Shifted 128-bit integer.
 */
MSIMD_API msimd_u128_t msimd_u128_shl(msimd_u128_t a, int shift, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Performs a logical right shift on a 128-bit unsigned integer: out = a >> shift.
 *
 * @param[in] a                128-bit operand to shift.
 * @param[in] shift            Bit shift count in the range [0, 127].
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Shifted 128-bit integer.
 */
MSIMD_API msimd_u128_t msimd_u128_shr(msimd_u128_t a, int shift, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes 128-bit unsigned integer division and remainder: quotient = n / d; *rem = n % d.
 *
 * Uses Knuth\'s Algorithm D multi-precision division to ensure exact quotient and remainder computation.
 *
 * @param[in]  n                128-bit dividend integer.
 * @param[in]  d                128-bit divisor integer (must be non-zero).
 * @param[out] rem              Optional pointer receiving the 128-bit remainder (may be NULL).
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 * @return 128-bit quotient integer.
 */
MSIMD_API msimd_u128_t msimd_u128_div(msimd_u128_t n, msimd_u128_t d, msimd_u128_t* rem, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Counts the number of leading zero bits in a 128-bit integer.
 *
 * @param[in] a                128-bit integer to inspect.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Count of leading zero bits in the range [0, 128].
 */
MSIMD_API int       msimd_u128_clz(msimd_u128_t a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the population count (total number of set 1-bits) in a 128-bit integer.
 *
 * @param[in] a                128-bit integer to inspect.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Number of set bits in the range [0, 128].
 */
MSIMD_API int       msimd_u128_popcnt(msimd_u128_t a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Vector128 */

/* ========================================================================= */
/* True IEEE 754-2008 binary128 Quad Precision (FP128)                       */
/* Hardware ISA Accelerated (IBM z15, POWER10, RISC-V Q) with FMA Fallbacks  */
/* ========================================================================= */

/**
 * @defgroup MSIMD_FP128 IEEE 754-2008 binary128 Quad Precision Arithmetic
 * @brief Exact 128-bit floating-point arithmetic (1 sign bit, 15 exponent bits, 112 significand bits).
 *
 * Provides approximately 34 decimal digits of precision for ultra-high-precision scientific
 * simulations, orbital ephemeris calculations, and general relativity spacetime metrics.
 * Accelerated natively on IBM z15/z16, IBM POWER10, and RISC-V Q-extension hardware.
 * @{
 */

/**
 * @brief Converts a 64-bit double-precision float to an exact IEEE 754 binary128 quad-precision float.
 *
 * @param[in] x                Source double-precision value.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Exact binary128 quad-precision representation.
 */
MSIMD_API msimd_fp128_t msimd_fp128_from_f64(double x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Converts an IEEE 754 binary128 quad-precision float to a 64-bit double-precision float.
 *
 * Rounds according to round-to-nearest, ties-to-even semantics.
 *
 * @param[in] x                Source binary128 quad-precision value.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Rounded 64-bit double-precision value.
 */
MSIMD_API double     msimd_fp128_to_f64(msimd_fp128_t x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Performs quad-precision addition: out = a + b.
 *
 * @param[in] a                First binary128 operand.
 * @param[in] b                Second binary128 operand.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Quad-precision sum.
 */
MSIMD_API msimd_fp128_t msimd_fp128_add(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Performs quad-precision subtraction: out = a - b.
 *
 * @param[in] a                Minuend binary128 operand.
 * @param[in] b                Subtrahend binary128 operand.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Quad-precision difference.
 */
MSIMD_API msimd_fp128_t msimd_fp128_sub(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Performs quad-precision multiplication: out = a * b.
 *
 * @param[in] a                Multiplicand binary128 operand.
 * @param[in] b                Multiplier binary128 operand.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Quad-precision product.
 */
MSIMD_API msimd_fp128_t msimd_fp128_mul(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Performs quad-precision division: out = a / b.
 *
 * @param[in] a                Dividend binary128 operand.
 * @param[in] b                Divisor binary128 operand (must be non-zero).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Quad-precision quotient.
 */
MSIMD_API msimd_fp128_t msimd_fp128_div(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the quad-precision square root: out = sqrt(a).
 *
 * @param[in] a                Source binary128 value (must be >= 0).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Quad-precision square root.
 */
MSIMD_API msimd_fp128_t msimd_fp128_sqrt(msimd_fp128_t a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the quad-precision fused multiply-add: out = (a * b) + c with single rounding.
 *
 * @param[in] a                Multiplicand binary128 operand.
 * @param[in] b                Multiplier binary128 operand.
 * @param[in] c                Addend binary128 operand.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Fused multiply-add quad-precision result.
 */
MSIMD_API msimd_fp128_t msimd_fp128_fma(msimd_fp128_t a, msimd_fp128_t b, msimd_fp128_t c, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Compares two binary128 quad-precision values.
 *
 * @param[in] a                First binary128 operand.
 * @param[in] b                Second binary128 operand.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return -1 if a < b, 0 if a == b, +1 if a > b (or NaN indicator code).
 */
MSIMD_API int        msimd_fp128_cmp(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_FP128 */

/* ========================================================================= */
/* 8-Bit Sub-Vectors (Hexal 6x8, Octal 8x8, Quad 4x8, Dual 2x8) & FP8       */
/* ========================================================================= */

/**
 * @defgroup MSIMD_SubVectors8 8-Bit Sub-Vectors & FP8 Formats
 * @brief Micro-vector operations and 8-bit floating-point (E4M3, E5M2) tensor quantization.
 * @{
 */

/* Octal 8-bit (8x8) */
MSIMD_API msimd_i8x8_t msimd_vec64_i8_add(msimd_i8x8_t a, msimd_i8x8_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API msimd_i8x8_t msimd_vec64_i8_sub(msimd_i8x8_t a, msimd_i8x8_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API msimd_u8x8_t msimd_vec64_u8_adds_sat(msimd_u8x8_t a, msimd_u8x8_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API msimd_u8x8_t msimd_vec64_u8_subs_sat(msimd_u8x8_t a, msimd_u8x8_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* Hexal 8-bit (6x8) */
MSIMD_API msimd_i8x6_t msimd_vec48_i8_add(msimd_i8x6_t a, msimd_i8x6_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API msimd_i8x6_t msimd_vec48_i8_sub(msimd_i8x6_t a, msimd_i8x6_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* Quad 8-bit (4x8) */
MSIMD_API msimd_i8x4_t msimd_vec32_i8_add(msimd_i8x4_t a, msimd_i8x4_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API msimd_i8x4_t msimd_vec32_i8_sub(msimd_i8x4_t a, msimd_i8x4_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* Dual 8-bit (2x8) */
MSIMD_API msimd_i8x2_t msimd_vec16_i8_add(msimd_i8x2_t a, msimd_i8x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API msimd_i8x2_t msimd_vec16_i8_sub(msimd_i8x2_t a, msimd_i8x2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* FP8 Conversions (E4M3 & E5M2) */

/**
 * @brief Converts a 32-bit single-precision float to an 8-bit floating-point value in E4M3 format.
 *
 * Format: 1 sign bit, 4 exponent bits, 3 mantissa bits (bias = 7).
 * Optimized for neural network activations and inference quantization.
 *
 * @param[in] x                Input single-precision float.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Quantized 8-bit FP8 E4M3 byte representation.
 */
MSIMD_API msimd_fp8_e4m3_t msimd_fp8_e4m3_from_f32(float x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Converts an 8-bit floating-point value in E4M3 format back to an IEEE 754 single-precision float.
 *
 * @param[in] x                Quantized FP8 E4M3 value.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Dequantized 32-bit float.
 */
MSIMD_API float         msimd_fp8_e4m3_to_f32(msimd_fp8_e4m3_t x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Converts a 32-bit single-precision float to an 8-bit floating-point value in E5M2 format.
 *
 * Format: 1 sign bit, 5 exponent bits, 2 mantissa bits (bias = 15).
 * Optimized for gradient backward passes due to dynamic range matching IEEE 754 FP16.
 *
 * @param[in] x                Input single-precision float.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Quantized 8-bit FP8 E5M2 byte representation.
 */
MSIMD_API msimd_fp8_e5m2_t msimd_fp8_e5m2_from_f32(float x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Converts an 8-bit floating-point value in E5M2 format back to an IEEE 754 single-precision float.
 *
 * @param[in] x                Quantized FP8 E5M2 value.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Dequantized 32-bit float.
 */
MSIMD_API float         msimd_fp8_e5m2_to_f32(msimd_fp8_e5m2_t x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* 16x8-bit FP8 Packed Vectors (128-bit) */
MSIMD_API msimd_fp8x16_e4m3_t msimd_vec128_fp8_e4m3_from_f32_16(const float in[16], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API void             msimd_vec128_fp8_e4m3_to_f32_16(msimd_fp8x16_e4m3_t in, float out[16], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API msimd_fp8x16_e4m3_t msimd_vec128_fp8_e4m3_add(msimd_fp8x16_e4m3_t a, msimd_fp8x16_e4m3_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API msimd_fp8x16_e4m3_t msimd_vec128_fp8_e4m3_sub(msimd_fp8x16_e4m3_t a, msimd_fp8x16_e4m3_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API float            msimd_vec128_fp8_e4m3_dot(msimd_fp8x16_e4m3_t a, msimd_fp8x16_e4m3_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

MSIMD_API msimd_fp8x16_e5m2_t msimd_vec128_fp8_e5m2_from_f32_16(const float in[16], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API void             msimd_vec128_fp8_e5m2_to_f32_16(msimd_fp8x16_e5m2_t in, float out[16], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API msimd_fp8x16_e5m2_t msimd_vec128_fp8_e5m2_add(msimd_fp8x16_e5m2_t a, msimd_fp8x16_e5m2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API msimd_fp8x16_e5m2_t msimd_vec128_fp8_e5m2_sub(msimd_fp8x16_e5m2_t a, msimd_fp8x16_e5m2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API float            msimd_vec128_fp8_e5m2_dot(msimd_fp8x16_e5m2_t a, msimd_fp8x16_e5m2_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_SubVectors8 */

/* ========================================================================= */
/* Multi-Limb & Multi-Precision Emulation (Pure C API)                       */
/* ========================================================================= */

/**
 * @defgroup MSIMD_MultiLimb Multi-Limb & Double-Double Precision Emulation
 * @brief Extended multi-limb integer arithmetic and double-double quad-precision floating-point emulation.
 * @{
 */

/**
 * @brief Adds two 128-bit dual-limb (2x64-bit) integers with carry propagation.
 *
 * @param[in]  a                First 128-bit integer array [lo, hi].
 * @param[in]  b                Second 128-bit integer array [lo, hi].
 * @param[out] out              Output 128-bit integer array [lo, hi].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_dual64_add(const uint64_t* a, const uint64_t* b, uint64_t* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Subtracts two 128-bit dual-limb (2x64-bit) integers with borrow propagation.
 *
 * @param[in]  a                Minuend 128-bit integer array [lo, hi].
 * @param[in]  b                Subtrahend 128-bit integer array [lo, hi].
 * @param[out] out              Output 128-bit integer array [lo, hi].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_dual64_sub(const uint64_t* a, const uint64_t* b, uint64_t* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Multiplies a 128-bit dual-limb integer by a 64-bit scalar: out = a * scalar.
 *
 * @param[in]  a                128-bit integer array [lo, hi].
 * @param[in]  scalar           64-bit unsigned multiplier.
 * @param[out] out              Output 128-bit integer array [lo, hi].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_dual64_mul_scalar(const uint64_t* a, uint64_t scalar, uint64_t* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Propagates carries sequentially across an arbitrary array of 64-bit limbs.
 *
 * Normalizes an unreduced multi-limb integer by resolving carries from limb 0 through limb count-1.
 *
 * @param[in,out] limbs            Array of 64-bit integer limbs to normalize in-place.
 * @param[in]     count            Number of limbs in the array.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_dual64_propagate_carries(uint64_t* limbs, size_t count, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

MSIMD_API void msimd_quad32_add(const uint32_t* a, const uint32_t* b, uint32_t* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API void msimd_quad32_sub(const uint32_t* a, const uint32_t* b, uint32_t* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

MSIMD_API void msimd_octal16_add(const uint16_t* a, const uint16_t* b, uint16_t* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);
MSIMD_API void msimd_octal16_sub(const uint16_t* a, const uint16_t* b, uint16_t* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Error-free transformation (EFT) of two floating-point numbers: s + e = a + b exactly.
 *
 * Uses Knuth\'s TwoSum algorithm (6 floating-point operations) to compute the exact sum s
 * and roundoff error e without requiring higher-precision hardware registers.
 *
 * @param[in]  a                First floating-point operand.
 * @param[in]  b                Second floating-point operand.
 * @param[out] s                Pointer receiving the rounded sum (a + b).
 * @param[out] e                Pointer receiving the exact roundoff error.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_two_sum(double a, double b, double* s, double* e, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Error-free transformation (EFT) of floating-point product: p + e = a * b exactly.
 *
 * Uses hardware FMA (Fused Multiply-Add) to compute p = a * b and e = fma(a, b, -p).
 *
 * @param[in]  a                Multiplicand floating-point operand.
 * @param[in]  b                Multiplier floating-point operand.
 * @param[out] p                Pointer receiving the rounded product (a * b).
 * @param[out] e                Pointer receiving the exact roundoff error.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_two_prod(double a, double b, double* p, double* e, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Adds two double-double numbers yielding an unevaluated sum with ~106 bits of significand precision.
 *
 * Each double-double value is represented as a high part and a low part (hi + lo).
 *
 * @param[in]  hi_a             High part of first operand.
 * @param[in]  lo_a             Low part of first operand.
 * @param[in]  hi_b             High part of second operand.
 * @param[in]  lo_b             Low part of second operand.
 * @param[out] out_hi           Pointer receiving the high part of the sum.
 * @param[out] out_lo           Pointer receiving the low part of the sum.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_doubledouble_add(double hi_a, double lo_a, double hi_b, double lo_b, double* out_hi, double* out_lo, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Multiplies two double-double numbers yielding a double-double product (~106 bits of precision).
 *
 * @param[in]  hi_a             High part of first operand.
 * @param[in]  lo_a             Low part of first operand.
 * @param[in]  hi_b             High part of second operand.
 * @param[in]  lo_b             Low part of second operand.
 * @param[out] out_hi           Pointer receiving the high part of the product.
 * @param[out] out_lo           Pointer receiving the low part of the product.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_doubledouble_mul(double hi_a, double lo_a, double hi_b, double lo_b, double* out_hi, double* out_lo, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_MultiLimb */

/* ========================================================================= */
/* Tier 3 Coprocessors, Embedded XiP & Tier 5 Soft-Float ALU Operations      */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Coprocessors Tier 3 Coprocessors, XiP & Tier 5 Soft-Float ALU
 * @brief Hardware coprocessor accelerators (CORDIC, FMAC, TMU) and IEEE 754 software emulation.
 * @{
 */

/* STMicroelectronics STM32 CORDIC Coprocessor (Q1.31 Fixed Point) */

/**
 * @brief Computes sine and cosine simultaneously in Q1.31 fixed-point format using the CORDIC algorithm.
 *
 * Designed to interface directly with hardware CORDIC accelerators (e.g. STM32G4 / STM32H7 CORDIC peripheral)
 * or execute an optimized unrolled software CORDIC pipeline. Angle theta is normalized to [-1.0, 1.0) representing [-pi, pi).
 *
 * @param[in]  theta_q31        Angle in Q1.31 format where 0x40000000 represents +pi/2.
 * @param[out] out_sin          Pointer receiving the Q1.31 sine result.
 * @param[out] out_cos          Pointer receiving the Q1.31 cosine result.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void  msimd_cordic_sin_cos_q31(int32_t theta_q31, int32_t* out_sin, int32_t* out_cos, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the four-quadrant arctangent (atan2) and vector modulus in Q1.31 format using CORDIC.
 *
 * @param[in]  y_q31            Y-coordinate in Q1.31 fixed-point format.
 * @param[in]  x_q31            X-coordinate in Q1.31 fixed-point format.
 * @param[out] out_theta        Pointer receiving the calculated angle in Q1.31 format.
 * @param[out] out_mod          Pointer receiving the vector magnitude in Q1.31 format.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void  msimd_cordic_atan2_q31(int32_t y_q31, int32_t x_q31, int32_t* out_theta, uint32_t* out_mod, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* STMicroelectronics STM32 FMAC (Filter-Math Accelerator) */

/**
 * @brief Computes an N-point Finite Impulse Response (FIR) filter across Q1.31 samples using hardware FMAC.
 *
 * Interfaces with the STM32 Filter-Math Accelerator (FMAC) or unrolled SIMD MAC pipelines to evaluate
 * y[n] = sum_{k=0}^{num_taps-1} coeffs[k] * x[n - k].
 *
 * @param[in]  x                Input sample buffer [n samples] in Q1.31 fixed-point format.
 * @param[in]  coeffs           Filter coefficient array [num_taps] in Q1.31 format.
 * @param[in]  n                Number of output samples to compute.
 * @param[in]  num_taps         Number of filter taps / coefficients.
 * @param[out] y                Output filtered sample buffer [n samples].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void  msimd_fmac_fir_q31(const int32_t* x, const int32_t* coeffs, size_t n, size_t num_taps, int32_t* y, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* Texas Instruments C28x TMU (Trigonometric Math Unit) */

/**
 * @brief Computes single-precision sine via Texas Instruments C28x TMU hardware instructions or polynomial minimax.
 *
 * @param[in] rad              Angle in radians.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Sine of rad.
 */
MSIMD_API float msimd_tmu_sin_f32(float rad, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes single-precision cosine via Texas Instruments C28x TMU hardware instructions or polynomial minimax.
 *
 * @param[in] rad              Angle in radians.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Cosine of rad.
 */
MSIMD_API float msimd_tmu_cos_f32(float rad, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* Software Floating-Point IEEE 754 Emulation (Tier 5 / Embedded MCUs) */

/**
 * @brief Software emulation of IEEE 754 single-precision addition on integer-only ALUs (Tier 5).
 *
 * @param[in] a                First 32-bit float operand.
 * @param[in] b                Second 32-bit float operand.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Emulated float sum.
 */
MSIMD_API float  msimd_softfloat_add_f32(float a, float b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Software emulation of IEEE 754 single-precision multiplication on integer-only ALUs (Tier 5).
 *
 * @param[in] a                First 32-bit float operand.
 * @param[in] b                Second 32-bit float operand.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Emulated float product.
 */
MSIMD_API float  msimd_softfloat_mul_f32(float a, float b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Software emulation of IEEE 754 single-precision division on integer-only ALUs (Tier 5).
 *
 * @param[in] a                Dividend 32-bit float.
 * @param[in] b                Divisor 32-bit float.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Emulated float quotient.
 */
MSIMD_API float  msimd_softfloat_div_f32(float a, float b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Software emulation of IEEE 754 double-precision addition on integer-only ALUs (Tier 5).
 *
 * @param[in] a                First 64-bit double operand.
 * @param[in] b                Second 64-bit double operand.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Emulated double sum.
 */
MSIMD_API double msimd_softfloat_add_f64(double a, double b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Software emulation of IEEE 754 double-precision multiplication on integer-only ALUs (Tier 5).
 *
 * @param[in] a                First 64-bit double operand.
 * @param[in] b                Second 64-bit double operand.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Emulated double product.
 */
MSIMD_API double msimd_softfloat_mul_f64(double a, double b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Software emulation of IEEE 754 double-precision division on integer-only ALUs (Tier 5).
 *
 * @param[in] a                Dividend 64-bit double.
 * @param[in] b                Divisor 64-bit double.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Emulated double quotient.
 */
MSIMD_API double msimd_softfloat_div_f64(double a, double b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/* Embedded XiP NOR Flash & TCM Decompression (100MB+ Footprint Architecture) */

/**
 * @def MSIMD_XIP_BASE
 * @brief Base physical memory address of external execute-in-place (XiP) Octo-SPI / Quad-SPI NOR Flash.
 */
#define MSIMD_XIP_BASE 0x90000000U

/**
 * @brief Verifies integrity and mapping status of the execute-in-place (XiP) external NOR flash memory.
 * @return MSIMD_SUCCESS (0) if flash mapping is verified and valid, or an error status code.
 */
MSIMD_API int    msimd_xip_verify_flash(void);

/**
 * @brief Decompresses a compressed bitcode block directly into high-speed Instruction Tightly-Coupled Memory (ITCM).
 *
 * @param[in]  src_compressed Compressed bitcode data in external flash.
 * @param[in]  src_size       Size of the compressed block in bytes.
 * @param[out] itcm_dest      Destination memory pointer within the ITCM address range (e.g. 0x00000000).
 * @param[in]  max_dest       Maximum available capacity in ITCM in bytes.
 * @return Number of decompressed bytes successfully written to ITCM.
 */
MSIMD_API size_t msimd_decompress_block_to_itcm(const void* src_compressed, size_t src_size, void* itcm_dest, size_t max_dest);

/** @} */ /* End of MSIMD_Coprocessors */

/* ========================================================================= */
/* Fast Exponentiation & Powers                                              */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Powers Fast Exponentiation & Powers
 * @brief High-performance integer exponentiation and arbitrary power evaluation.
 * @{
 */

/**
 * @brief Computes base raised to an integer exponent using binary exponentiation (exponentiation by squaring).
 *
 * Evaluates: out = base^{exp} with O(log2(|exp|)) multiplications. Handles negative exponents
 * via multiplicative inverse (1.0 / base^{-exp}). Handles 0^0 = 1.0 according to standard convention.
 *
 * @param[in] base             Base floating-point value.
 * @param[in] exp              Signed 64-bit integer power exponent.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Computed power value base^exp.
 */
MSIMD_API double msimd_ipow(double base, int64_t exp, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes base raised to an arbitrary real floating-point power: out = base^{exp} = exp(exp * ln(base)).
 *
 * Vectorized implementation of real exponentiation with full IEEE 754 special-case handling
 * (NaN, infinities, subnormals, and negative base fractional root detection).
 *
 * @param[in] base             Base floating-point value (must be >= 0 if exp is non-integer).
 * @param[in] exp              Real floating-point exponent.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Computed real power base^exp.
 */
MSIMD_API double msimd_pow(double base, double exp, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Powers */

/* ========================================================================= */
/* Classical Mechanics: Analytical & Symplectic Systems                      */
/* ========================================================================= */

/**
 * @defgroup MSIMD_ClassicalMechanics Classical Mechanics & Symplectic Integrators
 * @brief Lagrangian dynamics, Hamiltonian phase-space flows, Yoshida 4th-order symplectic stepping, and Kepler orbits.
 * @{
 */

/**
 * @brief Advances a Lagrangian dynamical system by one time step: q(t+dt), q_dot(t+dt).
 *
 * Integrates Euler-Lagrange equations d/dt (dL/dq_dot) - dL/dq = 0 for standard kinetic energy
 * T = 0.5 * sum m_i q_dot_i^2 and potential V(q). Accelerations are derived as a_i = -grad_v_i / m_i.
 *
 * @param[in,out] q                Generalized coordinate array [n] updated in-place.
 * @param[in,out] q_dot            Generalized velocity array [n] updated in-place.
 * @param[in]     mass             Particle mass array [n].
 * @param[in]     grad_v           Gradient of the potential energy array dV/dq [n].
 * @param[in]     n                Degrees of freedom (number of coordinates).
 * @param[in]     dt               Time step duration (seconds).
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_lagrangian_step(double* q, double* q_dot, const double* mass, const double* grad_v, size_t n, double dt, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Advances a Hamiltonian system in 2n-dimensional phase space (q, p) via symplectic stepping.
 *
 * Evolves canonical Hamilton equations dq/dt = dH/dp and dp/dt = -dH/dq preserving the canonical
 * symplectic 2-form omega = sum dq_i wedge dp_i, ensuring long-term energy and phase-space volume conservation.
 *
 * @param[in,out] q                Canonical position coordinate array [n].
 * @param[in,out] p                Canonical momentum coordinate array [n].
 * @param[in]     inv_m            Inverse mass array [n] representing dH/dp = p_i / m_i.
 * @param[in]     grad_v           Potential gradient array dV/dq [n].
 * @param[in]     n                Phase-space dimensionality.
 * @param[in]     dt               Time step increment.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_hamiltonian_step(double* q, double* p, const double* inv_m, const double* grad_v, size_t n, double dt, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Integrates separable Hamiltonian dynamics using Yoshida\'s 4th-order symplectic integrator.
 *
 * Applies the symmetric composition of four leapfrog substeps with weights w1 = w3 = 1/(2 - 2^{1/3})
 * and w0 = -2^{1/3}/(2 - 2^{1/3}), achieving O(dt^4) global error with exact preservation of symplectic invariants.
 *
 * @param[in,out] q                Position vector array [n].
 * @param[in,out] p                Momentum vector array [n].
 * @param[in]     inv_m            Inverse mass array [n].
 * @param[in]     force_fn         User callback evaluating forces force = -grad(V) given position q.
 * @param[in]     n                Degrees of freedom.
 * @param[in]     dt               Time step size.
 * @param[in,out] user_data        User context pointer passed directly to force_fn.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_symplectic_yoshida4_step(double* q, double* p, const double* inv_m, void (*force_fn)(const double*, double*, void*), size_t n, double dt, void* user_data, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Solves Kepler\'s orbital equation M = E - e*sin(E) and determines Cartesian orbital coordinates.
 *
 * Uses Newton-Raphson iteration accelerated by Halley\'s method to solve transcendental Kepler\'s equation
 * for eccentric anomaly E, then computes true anomaly nu and orbital orbital coordinates (x, y).
 *
 * @param[in]  a                Semi-major axis of the elliptical orbit (meters or AU).
 * @param[in]  e                Orbital eccentricity (0 <= e < 1 for bound ellipses).
 * @param[in]  M                Mean anomaly (radians).
 * @param[out] r                Pointer receiving radial orbital distance.
 * @param[out] nu               Pointer receiving true anomaly (radians).
 * @param[out] x                Pointer receiving in-plane Cartesian x-coordinate.
 * @param[out] y                Pointer receiving in-plane Cartesian y-coordinate.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_kepler_orbit_solve(double a, double e, double M, double* r, double* nu, double* x, double* y, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_ClassicalMechanics */

/* ========================================================================= */
/* Continuum Mechanics & Elastic Solids                                      */
/* ========================================================================= */

/**
 * @defgroup MSIMD_ContinuumMechanics Continuum Mechanics & Elastic Solids
 * @brief Cauchy stress-strain tensors, 3D elastodynamics, and Euler-Bernoulli beam flexure.
 * @{
 */

/**
 * @brief Computes 3D Cauchy stress and infinitesimal strain tensors under isotropic linear elasticity (Hooke\'s Law).
 *
 * Evaluates linearized strain epsilon_{ij} = 0.5 * (grad_u_{ij} + grad_u_{ji}) and Cauchy stress
 * sigma_{ij} = lambda * tr(epsilon) * delta_{ij} + 2 * mu * epsilon_{ij}, where lambda and mu
 * are Lame\'s first and second elastic parameters.
 *
 * @param[in]  grad_u           Displacement gradient tensor [3][3] = d u_i / d x_j.
 * @param[in]  lambda           Lame\'s first parameter (Pa).
 * @param[in]  mu               Shear modulus / Lame\'s second parameter (Pa).
 * @param[out] stress           Resulting 3x3 symmetric Cauchy stress tensor [Pa].
 * @param[out] strain           Resulting 3x3 symmetric infinitesimal strain tensor.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_cauchy_stress_strain_3d(const double grad_u[3][3], double lambda, double mu, double stress[3][3], double strain[3][3], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Advances the 3D Navier-Cauchy elastodynamics wave equation across a structured lattice.
 *
 * Solves: rho * d^2 u / dt^2 = (lambda + mu) * grad(div u) + mu * laplacian(u) using centered
 * finite differences in space and time.
 *
 * @param[in]     nx, ny, nz       Lattice grid dimensions along x, y, and z axes.
 * @param[in]     u_prev           Displacement vector field at time t - dt [3 * nx * ny * nz].
 * @param[in]     u_curr           Displacement vector field at time t [3 * nx * ny * nz].
 * @param[out]    u_next           Output displacement vector field at time t + dt [3 * nx * ny * nz].
 * @param[in]     lambda           Lame\'s first parameter.
 * @param[in]     mu               Lame\'s second parameter (shear modulus).
 * @param[in]     rho              Material mass density (kg/m^3).
 * @param[in]     dt               Time step duration.
 * @param[in]     dx               Uniform spatial grid spacing.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_navier_cauchy_elastodynamics_step(size_t nx, size_t ny, size_t nz, const float* u_prev, const float* u_curr, float* u_next, float lambda, float mu, float rho, float dt, float dx, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Advances a 1D Euler-Bernoulli beam flexural vibration equation: rho*A * d^2 w / dt^2 + EI * d^4 w / dx^4 = q.
 *
 * Integrates transverse deflection w(x, t) using 4th-order spatial central differences for the bending term.
 *
 * @param[in]  n                Number of spatial discretization nodes along the beam.
 * @param[in]  w_prev           Transverse deflection array at t - dt [n].
 * @param[in]  w_curr           Transverse deflection array at t [n].
 * @param[out] w_next           Output deflection array at t + dt [n].
 * @param[in]  EI               Flexural rigidity (Young\'s modulus E * area moment of inertia I).
 * @param[in]  rhoA             Mass per unit length (density * cross-sectional area).
 * @param[in]  q_load           Transverse distributed external load (N/m).
 * @param[in]  dt               Time step size.
 * @param[in]  dx               Spatial grid node spacing.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_euler_bernoulli_beam_step(size_t n, const float* w_prev, const float* w_curr, float* w_next, float EI, float rhoA, float q_load, float dt, float dx, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_ContinuumMechanics */

/* ========================================================================= */
/* Thermodynamics & Statistical Physics                                      */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Thermodynamics Thermodynamics & Statistical Physics
 * @brief Van der Waals real gas equation of state, Stefan-Boltzmann radiative transfer, and 2D Ising lattice Monte Carlo.
 * @{
 */

/**
 * @brief Computes pressure from the Van der Waals real gas equation: P = (R * T) / (Vm - b) - a / Vm^2.
 *
 * @param[in] T                Absolute temperature in Kelvin.
 * @param[in] Vm               Molar volume (m^3 / mol). Must be > b.
 * @param[in] a                Cohesion factor parameter (Pa * m^6 / mol^2).
 * @param[in] b                Co-volume parameter (m^3 / mol).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Calculated thermodynamic pressure in Pascals.
 */
MSIMD_API double msimd_thermo_van_der_waals_p(double T, double Vm, double a, double b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes total radiative flux emitted by a greybody: j* = emissivity * sigma * T^4.
 *
 * @param[in] emissivity       Surface emissivity in the range [0.0, 1.0].
 * @param[in] T                Thermodynamic temperature in Kelvin.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Emitted radiant emittance in Watts per square meter (W/m^2).
 */
MSIMD_API double msimd_thermo_stefan_boltzmann(double emissivity, double T, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Executes one full Metropolis Monte Carlo sweep across a 2D Ising spin lattice.
 *
 * Updates discrete spins sigma_i in {-1, +1} subject to Hamiltonian H = -J * sum_{<i,j>} sigma_i*sigma_j - B * sum_i sigma_i
 * using the Metropolis acceptance probability min(1, exp(-Delta E / (k_B * T))).
 *
 * @param[in,out] spin             2D spin array of signed bytes [nx * ny], each element +1 or -1.
 * @param[in]     nx, ny           Lattice grid width and height.
 * @param[in]     J                Spin coupling exchange constant (J > 0: ferromagnetic, J < 0: antiferromagnetic).
 * @param[in]     B                External magnetic field strength.
 * @param[in]     T                Lattice temperature in Kelvin (T > 0).
 * @param[in,out] rng_state        64-bit PRNG seed state updated across sweeps.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_ising_model_2d_step(int8_t* spin, size_t nx, size_t ny, double J, double B, double T, uint64_t* rng_state, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Thermodynamics */

/* ========================================================================= */
/* Physical & Geometric Optics                                               */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Optics Physical & Geometric Optics
 * @brief Fresnel interface reflection/transmission coefficients, Brewster angle, Jones polarization matrices, and Airy diffraction.
 * @{
 */

/**
 * @brief Computes Fresnel reflection and transmission amplitudes and Brewster\'s angle for an optical dielectric interface.
 *
 * Evaluates amplitude reflection and transmission coefficients for s-polarized (TE) and p-polarized (TM) light:
 * r_s = (n1*cos(theta_i) - n2*cos(theta_t)) / (n1*cos(theta_i) + n2*cos(theta_t))
 * r_p = (n2*cos(theta_i) - n1*cos(theta_t)) / (n2*cos(theta_i) + n1*cos(theta_t))
 *
 * @param[in]  n1             Refractive index of initial incident medium.
 * @param[in]  n2             Refractive index of transmitted dielectric medium.
 * @param[in]  theta_i        Incident ray angle measured from surface normal (radians).
 * @param[out] r_s            Pointer receiving s-polarized reflection amplitude coefficient.
 * @param[out] r_p            Pointer receiving p-polarized reflection amplitude coefficient.
 * @param[out] t_s            Pointer receiving s-polarized transmission amplitude coefficient.
 * @param[out] t_p            Pointer receiving p-polarized transmission amplitude coefficient.
 * @param[out] brewster_angle Pointer receiving Brewster\'s polarization angle: atan(n2 / n1).
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_optics_fresnel(double n1, double n2, double theta_i, double* r_s, double* r_p, double* t_s, double* t_p, double* brewster_angle, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Transforms an optical polarization state vector by applying a 2x2 complex Jones matrix: out = J * in.
 *
 * @param[in]  jones_mat_re     Real components of 2x2 Jones optical element matrix.
 * @param[in]  jones_mat_im     Imaginary components of 2x2 Jones matrix.
 * @param[in]  in_re            Real components of input 2-vector Jones state [Ex, Ey].
 * @param[in]  in_im            Imaginary components of input Jones state.
 * @param[out] out_re           Output real components of transformed polarization state.
 * @param[out] out_im           Output imaginary components of transformed state.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_optics_jones_apply(const double jones_mat_re[2][2], const double jones_mat_im[2][2], const double in_re[2], const double in_im[2], double out_re[2], double out_im[2], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes normalized Fraunhofer diffraction intensity for a circular aperture (Airy disk profile).
 *
 * Evaluates: I(r) / I(0) = (2 * J1(x) / x)^2 where x = k * a * r / f = (2*pi/wavelength) * aperture_radius * (r / focal_length),
 * using polynomial approximations for the Bessel function J1(x).
 *
 * @param[in] r                Radial distance from the optical axis in the observation plane (meters).
 * @param[in] wavelength       Monochromatic light wavelength in vacuum (meters).
 * @param[in] aperture_radius  Radius of the circular aperture (meters).
 * @param[in] focal_length     Distance from aperture to image observation plane (meters).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Normalized diffraction intensity in the range [0.0, 1.0].
 */
MSIMD_API double msimd_optics_fraunhofer_airy(double r, double wavelength, double aperture_radius, double focal_length, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Optics */

/* ========================================================================= */
/* Special Relativity & 4-Vectors                                            */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Relativity Special Relativity & 4-Vectors
 * @brief Lorentz spacetime boosts, Compton scattering shifts, and relativistic Doppler shifts.
 * @{
 */

/**
 * @brief Applies an arbitrary 3D Lorentz boost to a Minkowski spacetime 4-vector: x^mu_out = Lambda^mu_nu * x^nu_in.
 *
 * Employs standard metric signature eta = diag(-1, +1, +1, +1) (or (+1, -1, -1, -1)). Boost vector beta = v / c
 * has magnitude |beta| < 1. Lorentz factor gamma = 1 / sqrt(1 - |beta|^2).
 *
 * @param[in]  beta             Relative velocity boost vector beta = [vx/c, vy/c, vz/c]. Magnitude must be < 1.0.
 * @param[in]  four_vec_in      Input 4-vector [c*t, x, y, z] (or [E, px, py, pz]).
 * @param[out] four_vec_out     Boosted 4-vector transformed into the co-moving frame.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_relativity_lorentz_boost(const double beta[3], const double four_vec_in[4], double four_vec_out[4], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Calculates the Compton scattering wavelength shift: Delta lambda = (h / (m_e * c)) * (1 - cos(theta)).
 *
 * @param[in] theta            Photon scattering deflection angle (radians).
 * @param[in] wavelength_in    Incident photon wavelength (meters).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Shifted photon wavelength lambda_out = wavelength_in + Delta lambda.
 */
MSIMD_API double msimd_relativity_compton_shift(double theta, double wavelength_in, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the relativistic Doppler shifted frequency: f_obs = f_source * sqrt(1 - beta^2) / (1 - beta * cos(theta)).
 *
 * Accounts for both longitudinal and transverse relativistic time dilation effects.
 *
 * @param[in] f_source         Rest-frame source emission frequency (Hz).
 * @param[in] beta             Source velocity normalized to light speed (v / c).
 * @param[in] theta_rad        Angle between velocity vector and line of sight from receiver (radians).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Observed frequency f_obs in Hz.
 */
MSIMD_API double msimd_relativity_doppler(double f_source, double beta, double theta_rad, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Relativity */

/* ========================================================================= */
/* Advanced Linear Algebra & Spectral Decompositions                         */
/* ========================================================================= */

/**
 * @defgroup MSIMD_SpectralDecomp Advanced Linear Algebra & Spectral Decompositions
 * @brief Jacobi eigensolvers, 3x3 SVD, Cholesky factorization, LU decomposition, and Pade matrix exponentials.
 * @{
 */

/**
 * @brief Computes all eigenvalues and eigenvectors of a symmetric NxN matrix using cyclic Jacobi rotations.
 *
 * Eliminates off-diagonal elements via successive orthogonal similarity transformations A_{k+1} = J_k^T * A_k * J_k.
 *
 * @param[in]  n                Matrix dimension (NxN).
 * @param[in]  A                Symmetric input matrix array [n * n] in row-major layout.
 * @param[out] eigenvalues      Output array [n] receiving the sorted real eigenvalues.
 * @param[out] eigenvectors     Output array [n * n] receiving column eigenvectors.
 * @param[in]  max_iter         Maximum rotation iterations permitted before timeout.
 * @param[in]  tol              Tolerance threshold for off-diagonal residual norm.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 * @return Iteration count required for convergence, or negative error code on divergence.
 */
MSIMD_API int  msimd_mat_eigen_jacobi(size_t n, const double* A, double* eigenvalues, double* eigenvectors, int max_iter, double tol, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Singular Value Decomposition (SVD) of a 3x3 real matrix: A = U * diag(S) * V^T.
 *
 * Evaluates left singular vectors U (3x3 orthogonal), singular values S = [s0, s1, s2] (sorted descending),
 * and right singular vectors V (3x3 orthogonal) using a two-sided Jacobi rotation algorithm.
 *
 * @param[in]  A                Input 3x3 real matrix.
 * @param[out] U                Output 3x3 left orthogonal matrix.
 * @param[out] S                Output 3-vector of singular values (s0 >= s1 >= s2 >= 0).
 * @param[out] V                Output 3x3 right orthogonal matrix.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat_svd_3x3(const double A[3][3], double U[3][3], double S[3], double V[3][3], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Cholesky factorization of a symmetric positive-definite NxN matrix: A = L * L^T.
 *
 * @param[in]  n                Matrix dimension.
 * @param[in]  A                Symmetric positive-definite matrix [n * n] (row-major).
 * @param[out] L                Output lower triangular matrix [n * n] (row-major).
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 * @return MSIMD_SUCCESS (0) on success, or MSIMD_ERROR_INVALID_ARGUMENT if matrix is not positive-definite.
 */
MSIMD_API int  msimd_mat_cholesky(size_t n, const double* A, double* L, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Doolittle LU decomposition with partial row pivoting: P * A = L * U.
 *
 * @param[in]  n                Matrix dimension.
 * @param[in]  A                Input square matrix [n * n].
 * @param[out] LU               Combined output matrix [n * n] storing L (unit lower) and U (upper).
 * @param[out] p                Permutation vector array [n] storing pivot row exchanges.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 * @return 0 on success, non-zero if matrix is singular.
 */
MSIMD_API int  msimd_mat_lu_decompose(size_t n, const double* A, double* LU, int* p, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the matrix exponential exp(A) via scaling and squaring with [6/6] Pade approximation.
 *
 * Scales matrix by A / 2^s such that ||A / 2^s|| < 0.5, computes rational Pade polynomial ratio N_6(A)/D_6(A),
 * and squares the result s times.
 *
 * @param[in]  n                Matrix dimension.
 * @param[in]  A                Input square matrix [n * n].
 * @param[out] expA             Output matrix exponential result [n * n].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_mat_exp_pade(size_t n, const double* A, double* expA, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_SpectralDecomp */

/* ========================================================================= */
/* Geometric Algebra & Lie Groups                                            */
/* ========================================================================= */

/**
 * @defgroup MSIMD_LieGroups Geometric Algebra & Lie Groups
 * @brief Clifford Cl(3) multivector geometric product and Special Euclidean Lie group SE(3) exponential/logarithm maps.
 * @{
 */

/**
 * @brief Computes the 8-component geometric product in Clifford Algebra Cl(3, 0): out = a * b.
 *
 * Multiplies two full multivectors: scalar, 3 vectors (e1, e2, e3), 3 bivectors (e12, e23, e31),
 * and 1 pseudoscalar (e123) according to e_i^2 = +1.
 *
 * @param[in]  a                First multivector array [8].
 * @param[in]  b                Second multivector array [8].
 * @param[out] out              Output multivector array [8].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_clifford_cl3_mul(const double a[8], const double b[8], double out[8], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Lie group exponential map for Special Euclidean group SE(3): T = exp(xi^wedge).
 *
 * Maps a 6D twist coordinate vector xi = [omega; v] (angular velocity omega in R^3, linear velocity v in R^3)
 * to a 4x4 homogeneous transformation matrix T in SE(3) using Rodrigues\' formula.
 *
 * @param[in]  xi               Lie algebra se(3) twist vector [6] = [omega_x, omega_y, omega_z, v_x, v_y, v_z].
 * @param[out] T                Resulting 4x4 homogeneous transformation matrix.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_lie_se3_exp(const double xi[6], double T[4][4], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Lie group logarithm map for Special Euclidean group SE(3): xi = log(T).
 *
 * Inverts a 4x4 rigid transformation matrix T in SE(3) back to its corresponding 6D se(3) twist coordinates.
 *
 * @param[in]  T                Input 4x4 homogeneous transformation matrix in SE(3).
 * @param[out] xi               Output 6D twist coordinates array [6].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_lie_se3_log(const double T[4][4], double xi[6], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_LieGroups */

/* ========================================================================= */
/* Numerical Analysis & Optimization                                         */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Optimization Numerical Analysis & Non-Linear Optimization
 * @brief Gauss-Legendre quadrature, Romberg integration, Nelder-Mead simplex, and L-BFGS unconstrained optimization.
 * @{
 */

/**
 * @brief Evaluates definite integral integral_a^b fn(x) dx using N-point Gauss-Legendre quadrature.
 *
 * Maps nodes and weights to [a, b] achieving exact polynomial integration up to degree 2*n_points - 1.
 *
 * @param[in] fn               User callback function evaluating integrand f(x, user_data).
 * @param[in] a                Lower integration bound.
 * @param[in] b                Upper integration bound.
 * @param[in] n_points         Number of quadrature nodes (e.g. 5, 8, 16, 32).
 * @param[in] user_data        Arbitrary user context pointer passed to fn.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Evaluated definite integral value.
 */
MSIMD_API double msimd_num_gauss_legendre_quad(double (*fn)(double, void*), double a, double b, int n_points, void* user_data, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes high-precision definite integral via Romberg integration using Richardson extrapolation on trapezoidal sums.
 *
 * @param[in] fn               Integrand function pointer.
 * @param[in] a                Lower bound.
 * @param[in] b                Upper bound.
 * @param[in] max_steps        Maximum Romberg table refinement steps.
 * @param[in] tol              Convergence tolerance threshold.
 * @param[in] user_data        User context pointer.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Evaluated integral value.
 */
MSIMD_API double msimd_num_romberg_integral(double (*fn)(double, void*), double a, double b, int max_steps, double tol, void* user_data, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Minimizes an arbitrary multi-dimensional scalar objective function using the Nelder-Mead downhill simplex algorithm.
 *
 * Derivative-free optimization operating through geometric reflections, expansions, contractions, and shrinks.
 *
 * @param[in]     fn               Objective function pointer evaluating f(x, user_data).
 * @param[in]     dim              Dimensionality of the parameter space.
 * @param[in]     x_start          Initial guess parameter vector [dim].
 * @param[in]     step             Initial simplex edge characteristic length.
 * @param[in]     max_iter         Maximum iteration count.
 * @param[in]     tol              Simplex function value variance tolerance.
 * @param[out]    x_opt            Output vector receiving optimized parameters [dim].
 * @param[out]    f_opt            Pointer receiving minimum objective function value.
 * @param[in,out] user_data        User context pointer.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 * @return 0 on convergence, -1 on iteration exhaustion.
 */
MSIMD_API int    msimd_opt_nelder_mead(double (*fn)(const double*, void*), size_t dim, double* x_start, double step, int max_iter, double tol, double* x_opt, double* f_opt, void* user_data, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the unconstrained descent direction using the L-BFGS two-loop recursion.
 *
 * Approximates H_k * grad using m historical displacement and gradient difference vectors (s_k, y_k).
 *
 * @param[in]  n                Parameter vector dimension.
 * @param[in]  m                History memory depth.
 * @param[in]  grad             Current objective gradient vector [n].
 * @param[in]  s_history        Array of m previous parameter step vectors s_k = x_{k+1} - x_k.
 * @param[in]  y_history        Array of m previous gradient difference vectors y_k = g_{k+1} - g_k.
 * @param[in]  rho_history      Array of m scalar values rho_k = 1 / (y_k^T * s_k).
 * @param[in]  history_count    Number of currently active history vectors in memory (<= m).
 * @param[out] dir              Output calculated descent search direction vector [n].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_opt_lbfgs_two_loop(size_t n, size_t m, const double* grad, const double* const* s_history, const double* const* y_history, const double* rho_history, size_t history_count, double* dir, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Optimization */

/* ========================================================================= */
/* Probability Distributions & Estimation                                    */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Probability Probability Distributions & State Estimation
 * @brief Gaussian, Gamma, Poisson, Chi-square distributions, sample covariance matrices, and discrete Kalman filters.
 * @{
 */

/**
 * @brief Evaluates Gaussian (Normal) Probability Density Function (PDF): f(x) = (1 / (sigma * sqrt(2*pi))) * exp(-0.5 * ((x-mu)/sigma)^2).
 *
 * @param[in] x                Evaluation point.
 * @param[in] mu               Distribution mean.
 * @param[in] sigma            Standard deviation (must be > 0).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Probability density value.
 */
MSIMD_API double msimd_dist_normal_pdf(double x, double mu, double sigma, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates Gaussian Cumulative Distribution Function (CDF): Phi(x) = 0.5 * (1 + erf((x - mu) / (sigma * sqrt(2)))).
 *
 * @param[in] x                Evaluation point.
 * @param[in] mu               Distribution mean.
 * @param[in] sigma            Standard deviation (sigma > 0).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Cumulative probability in the range [0.0, 1.0].
 */
MSIMD_API double msimd_dist_normal_cdf(double x, double mu, double sigma, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates the Gamma distribution probability density function: f(x) = (beta^alpha / Gamma(alpha)) * x^{alpha-1} * exp(-beta * x).
 *
 * @param[in] x                Evaluation value (x >= 0).
 * @param[in] alpha            Shape parameter (alpha > 0).
 * @param[in] beta             Rate parameter (beta > 0).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Probability density.
 */
MSIMD_API double msimd_dist_gamma_pdf(double x, double alpha, double beta, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates Poisson Probability Mass Function (PMF): P(X = k) = (lambda^k * exp(-lambda)) / k!.
 *
 * @param[in] k                Discrete integer occurrence count (k >= 0).
 * @param[in] lambda           Expected arrival rate parameter (lambda > 0).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Discrete probability value.
 */
MSIMD_API double msimd_dist_poisson_pmf(int k, double lambda, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates the Chi-Square cumulative distribution function for k degrees of freedom.
 *
 * @param[in] x                Evaluation point (x >= 0).
 * @param[in] k                Degrees of freedom (positive integer).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Cumulative probability.
 */
MSIMD_API double msimd_dist_chisquare_cdf(double x, int k, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the sample covariance matrix across multi-variate observations.
 *
 * Evaluates: Cov(i, j) = sum_{s=1}^N (X_{s,i} - bar{X}_i) * (X_{s,j} - bar{X}_j) / (N - 1).
 *
 * @param[in]  num_samples      Number of sample observation rows (N >= 2).
 * @param[in]  num_vars         Number of variables (columns, P).
 * @param[in]  data             Observation matrix data array [num_samples * num_vars] in row-major layout.
 * @param[out] cov_matrix       Output symmetric covariance matrix array [num_vars * num_vars].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_stat_covariance_matrix(size_t num_samples, size_t num_vars, const double* data, double* cov_matrix, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Executes one discrete linear Kalman filter prediction and correction cycle.
 *
 * State model: x_{k|k-1} = F * x_{k-1}; P_{k|k-1} = F * P * F^T + Q
 * Measurement update: K = P * H^T * (H * P * H^T + R)^{-1}; x = x + K * (z - H * x); P = (I - K * H) * P.
 *
 * @param[in]     dim_x            State vector dimension.
 * @param[in]     dim_z            Measurement vector dimension.
 * @param[in,out] x                State estimate vector [dim_x] updated in-place.
 * @param[in,out] P                State error covariance matrix [dim_x * dim_x] updated in-place.
 * @param[in]     F                State transition matrix [dim_x * dim_x].
 * @param[in]     Q                Process noise covariance matrix [dim_x * dim_x].
 * @param[in]     H                Measurement observation matrix [dim_z * dim_x].
 * @param[in]     R                Measurement noise covariance matrix [dim_z * dim_z].
 * @param[in]     z                Observed measurement vector [dim_z].
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_filter_kalman_step(size_t dim_x, size_t dim_z, double* x, double* P, const double* F, const double* Q, const double* H, const double* R, const double* z, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Probability */

/* ========================================================================= */
/* Fuzzy Logic (Zadeh T-Norm / S-Norm Fuzzy Inference Systems)               */
/* ========================================================================= */

/**
 * @defgroup MSIMD_FuzzyLogic Fuzzy Logic & Inference Systems
 * @brief Membership functions (Triangular, Trapezoidal, Gaussian), T-Norms, S-Norms, and Centroid Defuzzification.
 * @{
 */

/**
 * @brief Computes triangular fuzzy membership degree mu(x; a, b, c).
 *
 * @param[in] x                Input value.
 * @param[in] a                Lower foot parameter.
 * @param[in] b                Peak parameter (a <= b <= c).
 * @param[in] c                Upper foot parameter.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Membership degree in the range [0.0, 1.0].
 */
MSIMD_API double msimd_fuzzy_triangular(double x, double a, double b, double c, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes trapezoidal fuzzy membership degree mu(x; a, b, c, d).
 *
 * @param[in] x                Input value.
 * @param[in] a, b, c, d       Trapezoidal boundary parameters (a <= b <= c <= d).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Membership degree in [0.0, 1.0].
 */
MSIMD_API double msimd_fuzzy_trapezoidal(double x, double a, double b, double c, double d, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes Gaussian fuzzy membership degree: mu(x) = exp(-0.5 * ((x - c) / sigma)^2).
 *
 * @param[in] x                Input value.
 * @param[in] c                Gaussian center peak.
 * @param[in] sigma            Standard deviation width parameter (sigma > 0).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Membership degree in [0.0, 1.0].
 */
MSIMD_API double msimd_fuzzy_gaussian(double x, double c, double sigma, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates Godel / Zadeh standard T-norm intersection: T(a, b) = min(a, b).
 *
 * @param[in] a, b             Fuzzy membership degrees in [0.0, 1.0].
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Minimum membership degree.
 */
MSIMD_API double msimd_fuzzy_tnorm_min(double a, double b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates Godel / Zadeh standard S-norm union: S(a, b) = max(a, b).
 *
 * @param[in] a, b             Fuzzy membership degrees in [0.0, 1.0].
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Maximum membership degree.
 */
MSIMD_API double msimd_fuzzy_snorm_max(double a, double b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes centroid (center of gravity) defuzzification: x* = sum(x_i * mu_i) / sum(mu_i).
 *
 * @param[in] x_vals           Domain discretization values array [n].
 * @param[in] mu_vals          Membership degree values array [n].
 * @param[in] n                Number of discretization samples.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Defuzzified crisp scalar output value.
 */
MSIMD_API double msimd_fuzzy_defuzzify_centroid(const double* x_vals, const double* mu_vals, size_t n, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_FuzzyLogic */

/* ========================================================================= */
/* Galois Field GF(2^8) Arithmetic                                           */
/* ========================================================================= */

/**
 * @defgroup MSIMD_GaloisField Galois Field GF(2^8) Arithmetic
 * @brief Finite field GF(2^8) multiplication and multiplicative inversion under Rijndael irreducible polynomial x^8 + x^4 + x^3 + x + 1 (0x11B).
 * @{
 */

/**
 * @brief Multiplies two elements in Galois Field GF(2^8) modulo Rijndael irreducible polynomial 0x11B.
 *
 * @param[in] a                First field element (8-bit byte).
 * @param[in] b                Second field element (8-bit byte).
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Product element in GF(2^8).
 */
MSIMD_API uint8_t msimd_gf256_mul(uint8_t a, uint8_t b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes multiplicative inverse in Galois Field GF(2^8) such that a * a^{-1} = 1 (with 0^{-1} = 0).
 *
 * @param[in] a                Field element byte.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Multiplicative inverse in GF(2^8).
 */
MSIMD_API uint8_t msimd_gf256_inv(uint8_t a, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_GaloisField */

/* ========================================================================= */
/* Gravitational Physics: Poisson Potential & General Relativistic Metric    */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Gravitation Gravitational Physics & Spacetime Metrics
 * @brief Newtonian Poisson 3D field solvers and Schwarzschild curved spacetime metric geodesics.
 * @{
 */

/**
 * @brief Advances a 3D gravitational Poisson equation solver: laplacian(phi) = 4 * pi * G * rho using Successive Over-Relaxation (SOR).
 *
 * @param[in]     nx, ny, nz       Grid spatial dimensions.
 * @param[in]     rho_mass         Mass density distribution field array [nx * ny * nz].
 * @param[in]     phi_curr         Current gravitational potential field [nx * ny * nz].
 * @param[out]    phi_next         Updated gravitational potential field [nx * ny * nz].
 * @param[in]     G_const          Gravitational constant G.
 * @param[in]     dx               Uniform cell size.
 * @param[in]     relax_factor     Over-relaxation parameter omega in the range (1.0, 2.0).
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_gravity_poisson_step(size_t nx, size_t ny, size_t nz, const float* rho_mass, const float* phi_curr, float* phi_next, float G_const, float dx, float relax_factor, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates Schwarzschild metric gravitational time dilation, horizon radius, and photon bending deflection.
 *
 * Computes Schwarzschild radius r_s = 2 * G * M / c^2, static observer time dilation d tau / dt = sqrt(1 - r_s / r),
 * and weak-field light deflection angle Delta phi = 4 * G * M / (c^2 * b).
 *
 * @param[in]  M_mass           Central body mass (kg).
 * @param[in]  r_dist           Radial distance from singularity (meters, must be > r_s).
 * @param[in]  impact_b         Photon impact parameter (meters).
 * @param[out] time_dilation    Pointer receiving gravitational time dilation factor.
 * @param[out] rs_radius        Pointer receiving Schwarzschild event horizon radius (meters).
 * @param[out] deflection_angle Pointer receiving total photon deflection angle (radians).
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_gravity_schwarzschild_metric(double M_mass, double r_dist, double impact_b, double* time_dilation, double* rs_radius, double* deflection_angle, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Gravitation */

/* ========================================================================= */
/* Quantum Mechanics: Relativistic Dirac Spinor & Pauli Precession           */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Quantum Relativistic Quantum Mechanics
 * @brief 4-component Dirac spinors in electromagnetic 4-potentials and Pauli spin-1/2 precession.
 * @{
 */

/**
 * @brief Advances a 4-component complex relativistic Dirac spinor psi under an electromagnetic 4-potential A_mu.
 *
 * Solves: (i * gamma^mu * D_mu - m) * psi = 0 where D_mu = partial_mu + i * e * A_mu.
 *
 * @param[in]  psi_re_in        Input spinor real components [4].
 * @param[in]  psi_im_in        Input spinor imaginary components [4].
 * @param[in]  A_pot            Electromagnetic 4-potential A = [phi/c, Ax, Ay, Az].
 * @param[in]  mass             Fermion mass m.
 * @param[in]  dt               Time step duration.
 * @param[out] psi_re_out       Updated spinor real components [4].
 * @param[out] psi_im_out       Updated spinor imaginary components [4].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quantum_dirac_spinor_step(const double psi_re_in[4], const double psi_im_in[4], const double A_pot[4], double mass, double dt, double psi_re_out[4], double psi_im_out[4], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Advances spin magnetic dipole moment precession in an external magnetic field B: dS/dt = gamma * (S x B).
 *
 * @param[in,out] spin             Spin vector S = [Sx, Sy, Sz] updated in-place.
 * @param[in]     B_field          External magnetic flux density vector B = [Bx, By, Bz] (Tesla).
 * @param[in]     gamma_gyro       Gyromagnetic ratio (rad / (s * T)).
 * @param[in]     dt               Time increment.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quantum_pauli_spin_precession(double spin[3], const double B_field[3], double gamma_gyro, double dt, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Quantum */

/* ========================================================================= */
/* High-Energy & Nuclear Physics: Quarks, Gluons, Pions & Hadronic Bonds     */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Nuclear High-Energy & Nuclear Physics
 * @brief QCD Cornell quark-confinement potential, Yukawa nucleon-nucleon exchange force, and SU(3) Gell-Mann color algebra.
 * @{
 */

/**
 * @brief Evaluates the Cornell quarkonium potential and radial force: V(r) = -4/3 * (alpha_s / r) + sigma * r.
 *
 * Represents Coulomb-like one-gluon exchange at short distances and linear string tension confinement at long distances.
 *
 * @param[in]  r_dist           Inter-quark separation distance (meters or fm).
 * @param[in]  alpha_s          Strong coupling constant.
 * @param[in]  sigma_string     QCD string tension constant (~1 GeV/fm).
 * @param[out] potential        Pointer receiving computed potential energy V(r).
 * @param[out] force            Pointer receiving radial force magnitude -dV/dr.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_qcd_cornell_potential(double r_dist, double alpha_s, double sigma_string, double* potential, double* force, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates the Yukawa meson-exchange nuclear potential and force: V(r) = -g^2 * exp(-m_pion * r) / r.
 *
 * @param[in]  r_dist           Nucleon separation distance.
 * @param[in]  g_coupling       Meson-nucleon coupling constant.
 * @param[in]  pion_mass        Mass/screening parameter of mediating pion.
 * @param[out] potential        Pointer receiving potential energy V(r).
 * @param[out] force            Pointer receiving attractive force magnitude.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_nuclear_yukawa_force(double r_dist, double g_coupling, double pion_mass, double* potential, double* force, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Applies an SU(3) Gell-Mann matrix generator lambda_a to a 3-component complex quark color state vector.
 *
 * @param[in]  index            Gell-Mann matrix index in the range [1, 8].
 * @param[in]  color_in_re      Input color triplet real components [3] = [red, green, blue].
 * @param[in]  color_in_im      Input color triplet imaginary components [3].
 * @param[out] color_out_re     Output color triplet real components [3].
 * @param[out] color_out_im     Output color triplet imaginary components [3].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_qcd_gellmann_apply(int index, const double color_in_re[3], const double color_in_im[3], double color_out_re[3], double color_out_im[3], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Nuclear */

/* ========================================================================= */
/* Higgs Mechanism & Electroweak Symmetry Breaking                           */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Higgs Higgs Mechanism & Electroweak Symmetry Breaking
 * @brief Higgs scalar potential V(phi) = -mu^2 * phi^2 + lambda * phi^4 and fermion Yukawa mass generation.
 * @{
 */

/**
 * @brief Evaluates the Mexican-hat Higgs scalar field potential and gradient: V(phi) = -mu_sq * phi^2 + lambda_self * phi^4.
 *
 * @param[in]  phi              Higgs scalar field expectation amplitude.
 * @param[in]  mu_sq            Tachyon mass squared parameter (mu^2 > 0).
 * @param[in]  lambda_self      Higgs quartic self-coupling parameter (lambda > 0).
 * @param[out] potential        Pointer receiving computed potential energy V(phi).
 * @param[out] dV_dphi          Pointer receiving potential derivative dV/dphi.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_higgs_potential(double phi, double mu_sq, double lambda_self, double* potential, double* dV_dphi, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes fermion mass generated via spontaneous symmetry breaking: m_f = y_f * v / sqrt(2).
 *
 * @param[in] yukawa_coupling   Dimensionless Yukawa coupling constant y_f for the specific fermion.
 * @param[in] vev_v             Higgs vacuum expectation value v (~246 GeV).
 * @param[in] precision_target  Optional precision descriptor (default: nullptr).
 * @return Generated fermion rest mass.
 */
MSIMD_API double msimd_higgs_fermion_mass(double yukawa_coupling, double vev_v, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Higgs */

/* ========================================================================= */
/* Weak Nuclear Force & Fermi Transition Rates                               */
/* ========================================================================= */

/**
 * @defgroup MSIMD_WeakForce Weak Nuclear Force & Decay Rates
 * @brief Fermi\'s Golden Rule beta decay rates and Massive W/Z vector boson Proca-Yukawa potentials.
 * @{
 */

/**
 * @brief Computes nuclear beta-decay transition rate using Fermi\'s Golden Rule with phase space integration.
 *
 * Evaluates: dGamma / dE_e proportional to G_fermi^2 * |M_fi|^2 * p_e * E_e * (E_0 - E_e)^2 * F(Z, E_e).
 *
 * @param[in]  delta_E          Total available decay reaction Q-value energy.
 * @param[in]  G_fermi          Fermi weak coupling constant (~1.1663787e-5 GeV^-2).
 * @param[out] decay_rate       Pointer receiving transition rate / decay width Gamma.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_weak_fermi_decay_rate(double delta_E, double G_fermi, double* decay_rate, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes massive vector boson interaction potential: V(r) = (g_weak^2 / (4*pi*r)) * exp(-m_w * r).
 *
 * @param[in]  r_dist           Interaction separation distance.
 * @param[in]  g_weak           Weak SU(2)_L gauge coupling constant.
 * @param[in]  m_w_boson        Mass of mediating W/Z weak vector boson (~80.4 GeV/c^2).
 * @param[out] potential        Pointer receiving computed weak interaction potential.
 * @param[out] force            Pointer receiving weak interaction force magnitude.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_weak_boson_potential(double r_dist, double g_weak, double m_w_boson, double* potential, double* force, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_WeakForce */

/* ========================================================================= */
/* 1st Generation Antimatter Dynamics: Positron & Anti-Quarks (u_bar, d_bar) */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Antimatter Antimatter Dynamics & Charge Conjugation
 * @brief Charge conjugation operators C = i * gamma^2 * gamma^0, pair annihilation energy, and anti-quark color inversion.
 * @{
 */

/**
 * @brief Applies the charge conjugation operator C to a Dirac spinor: psi_c = C * bar{psi}^T = i * gamma^2 * psi^*.
 *
 * Maps a fermion wavefunction into its corresponding antifermion state (e.g. electron -> positron).
 *
 * @param[in]  psi_re_in        Input fermion spinor real components [4].
 * @param[in]  psi_im_in        Input fermion spinor imaginary components [4].
 * @param[out] psi_re_out       Output antifermion spinor real components [4].
 * @param[out] psi_im_out       Output antifermion spinor imaginary components [4].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quantum_dirac_charge_conjugate(const double psi_re_in[4], const double psi_im_in[4], double psi_re_out[4], double psi_im_out[4], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes total rest-mass annihilation energy and resulting characteristic two-photon frequency: E = (m1 + m2) * c^2; nu = E / (2 * h).
 *
 * @param[in]  m1                   Particle rest mass (kg).
 * @param[in]  m2                   Antiparticle rest mass (kg).
 * @param[out] total_energy_joules  Pointer receiving total released energy in Joules.
 * @param[out] photon_frequency     Pointer receiving annihilation photon frequency in Hz.
 * @param[in]  precision_target     Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_particle_annihilation_energy(double m1, double m2, double* total_energy_joules, double* photon_frequency, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Conjugates an SU(3) color triplet representation into an anti-color triplet representation: c_bar = c^*.
 *
 * @param[in]  color_in_re      Quark color vector real components [3] = [r, g, b].
 * @param[in]  color_in_im      Quark color vector imaginary components [3].
 * @param[out] color_out_re     Antiquark anti-color real components [3] = [anti-r, anti-g, anti-b].
 * @param[out] color_out_im     Antiquark anti-color imaginary components [3].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_qcd_antiquark_color_conjugate(const double color_in_re[3], const double color_in_im[3], double color_out_re[3], double color_out_im[3], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Antimatter */

/* ========================================================================= */
/* Hardware True Random Number Generator (TRNG: Fast & Hard)                 */
/* ========================================================================= */

/**
 * @defgroup MSIMD_TRNG Hardware True Random Number Generation (TRNG)
 * @brief Hardware entropy harvesting using NIST SP 800-90A/B compliant instructions (RDRAND, RDSEED).
 * @{
 */

/**
 * @brief Harvests a 64-bit cryptographically secure pseudorandom value from hardware (RDRAND instruction).
 *
 * Utilizes on-chip hardware thermal noise entropy sources passing through an on-die AES-CTR conditioning circuit.
 *
 * @param[out] out_val          Pointer receiving the harvested 64-bit random integer.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 * @return 1 on successful entropy generation, 0 if hardware generator underflows.
 */
MSIMD_API int msimd_trng_fast64(uint64_t* out_val, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Harvests a 64-bit true hardware non-deterministic physical entropy seed (RDSEED instruction).
 *
 * Directly samples thermal entropy without passing through an algorithmic PRNG expander.
 *
 * @param[out] out_val          Pointer receiving the 64-bit hardware seed.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 * @return 1 on successful seed extraction, 0 if hardware noise source is depleted.
 */
MSIMD_API int msimd_trng_hard64(uint64_t* out_val, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Fills an arbitrary memory buffer with hardware random entropy bytes.
 *
 * @param[out] buffer           Destination memory buffer to fill with random data.
 * @param[in]  num_bytes        Number of random bytes to generate.
 * @param[in]  hard_entropy     If non-zero, uses RDSEED physical entropy; otherwise uses RDRAND fast TRNG.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_trng_fill(void* buffer, size_t num_bytes, int hard_entropy, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_TRNG */

#ifdef __cplusplus
} // extern "C"

/* ========================================================================= */
/* C++ Mathematical Wrapper with Natural Exponentiation Operator ^           */
/* ========================================================================= */

/**
 * @struct msimd_scalar
 * @brief Expressive C++ scalar wrapper providing natural mathematical syntax with the `^` power operator.
 *
 * In standard C/C++, the `^` operator represents bitwise XOR. The msimd_scalar struct overloads
 * `operator^` to represent true mathematical exponentiation (calling msimd_pow or msimd_ipow),
 * enabling natural algebraic formulas like `msimd_scalar(x) ^ 2.5` or `x ^ 3`.
 */
struct msimd_scalar {
    double val;
    msimd_scalar() : val(0.0) {}
    msimd_scalar(double v) : val(v) {}
    operator double() const { return val; }

    inline msimd_scalar operator^(double exponent) const {
        return msimd_scalar(msimd_pow(val, exponent));
    }
    inline msimd_scalar operator^(int exponent) const {
        return msimd_scalar(msimd_ipow(val, exponent));
    }
    inline msimd_scalar operator^(int64_t exponent) const {
        return msimd_scalar(msimd_ipow(val, exponent));
    }
};

inline msimd_scalar operator^(double base, const msimd_scalar& exp) {
    return msimd_scalar(msimd_pow(base, exp.val));
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
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

/** @} */ /* End of MSIMD_HW_Hinter */

/* ========================================================================= */
/* SECTION 28: PLANCK-SCALE QUANTIZED ARITHMETIC & COMPENSATED SUMMATION     */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section28 Section 28: Planck-Scale Quantized Arithmetic & Compensated Summation
 * @brief Discrete spacetime lattice quantization and Neumaier error-compensated summation.
 * @{
 */

/**
 * @brief Quantizes a continuous physical length in meters into discrete integer Planck length units (ell_P).
 *
 * Formula: l_{discrete} = round(meters / ell_P) where ell_P = sqrt(hbar * G / c^3) ~ 1.616255e-35 m.
 *
 * @param[in] meters           Physical distance in meters.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Discrete length expressed in Planck units as a 64-bit integer.
 */
MSIMD_API int64_t msimd_planck_quantize_length(double meters, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Converts a discrete Planck length unit count back to continuous meters.
 *
 * @param[in] planck_units     Discrete length in Planck units.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Continuous length in meters.
 */
MSIMD_API double  msimd_planck_dequantize_length(int64_t planck_units, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Quantizes a continuous time duration in seconds into discrete Planck time units (t_P).
 *
 * Formula: t_{discrete} = round(seconds / t_P) where t_P = sqrt(hbar * G / c^5) ~ 5.391247e-44 s.
 *
 * @param[in] seconds          Continuous time interval in seconds.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Discrete time duration in Planck units.
 */
MSIMD_API int64_t msimd_planck_quantize_time(double seconds, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Converts a discrete Planck time unit count back to continuous seconds.
 *
 * @param[in] planck_units     Discrete time duration in Planck units.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Continuous time in seconds.
 */
MSIMD_API double  msimd_planck_dequantize_time(int64_t planck_units, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Advances discrete kinematic positions on a Planck-scale integer spatial lattice.
 *
 * Evaluates: pos_out[i] = pos_in[i] + vel_in[i] * dt_planck with exact integer arithmetic,
 * preventing floating-point accumulation drift over astronomical time scales.
 *
 * @param[in]  pos_in           Current discrete positions array [n].
 * @param[in]  vel_in           Discrete velocity array [n] (Planck units per Planck time).
 * @param[in]  dt_planck        Time step in Planck units.
 * @param[out] pos_out          Updated discrete positions array [n].
 * @param[in]  n                Number of particles / lattice coordinates.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void    msimd_planck_discrete_step(const int64_t* pos_in, const int64_t* vel_in, int64_t dt_planck, int64_t* pos_out, size_t n, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes high-precision double-precision sum across N elements using Neumaier\'s compensated summation algorithm.
 *
 * Enhanced version of Kahan summation that properly tracks roundoff error even when the current term
 * exceeds the running sum in magnitude: |term| > |running_sum|. Reduces numerical accumulation
 * error from O(N * eps) to O(eps).
 *
 * @param[in]  values           Array of double-precision floating-point numbers [n].
 * @param[in]  n                Number of elements in values.
 * @param[out] sum_out          Pointer receiving the compensated sum.
 * @param[out] comp_out         Pointer receiving the residual compensation term.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void    msimd_compensated_sum_neumaier(const double* values, size_t n, double* sum_out, double* comp_out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes high-precision single-precision sum across N elements using Neumaier\'s compensated summation algorithm.
 *
 * @param[in]  values           Array of single-precision floating-point numbers [n].
 * @param[in]  n                Number of elements.
 * @param[out] sum_out          Pointer receiving compensated float sum.
 * @param[out] comp_out         Pointer receiving float compensation term.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void    msimd_compensated_sum_neumaier_f32(const float* values, size_t n, float* sum_out, float* comp_out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section28 */

/* ========================================================================= */
/* SECTION 29: TRANSCENDENTAL MINIMAX & SPECIAL FUNCTIONS                    */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section29 Section 29: Transcendental Minimax & Special Functions
 * @brief Horner and Estrin polynomial evaluation, Remez minimax error function, Gamma function, and Legendre polynomials.
 * @{
 */

/**
 * @brief Evaluates an N-th degree polynomial via Horner\'s synthetic division scheme: P(x) = c_0 + x*(c_1 + x*(c_2 + ...)).
 *
 * Evaluates polynomial coefficients with N additions and N multiplications, maximizing numerical stability.
 *
 * @param[in] coeffs           Array of polynomial coefficients [degree + 1], where coeffs[i] is the coefficient for x^i.
 * @param[in] degree           Polynomial degree N.
 * @param[in] x                Evaluation point.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Evaluated polynomial value P(x).
 */
MSIMD_API double msimd_horner_eval(const double* coeffs, size_t degree, double x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates an 8th-degree polynomial using Estrin\'s parallel tree evaluation scheme.
 *
 * Reorganizes polynomial terms into a balanced binary tree of fused multiply-adds, breaking
 * the sequential dependency chain of Horner\'s method and enabling instruction-level parallelism (ILP).
 *
 * @param[in] c                Array of 9 coefficients [c0, c1, ..., c8].
 * @param[in] x                Evaluation point.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Evaluated polynomial value.
 */
MSIMD_API double msimd_estrin_eval_8(const double* c, double x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Gauss error function erf(x) via Remez minimax rational polynomial approximation.
 *
 * Achieves near machine-epsilon accuracy (|error| < 1.5e-16) across the entire real line (-inf, +inf).
 *
 * @param[in] x                Input value.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Evaluated error function erf(x) in [-1.0, 1.0].
 */
MSIMD_API double msimd_minimax_erf(double x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes Euler\'s Gamma function Gamma(x) using the Lanczos approximation with g = 7.
 *
 * Computes Gamma(x) for all real x except non-positive integers where poles exist.
 *
 * @param[in] x                Real argument.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Value of Gamma(x).
 */
MSIMD_API double msimd_minimax_gamma(double x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates the Legendre polynomial P_n(x) of degree n at point x using Bonnet\'s three-term recurrence relation.
 *
 * Recurrence: (n + 1) * P_{n+1}(x) = (2n + 1) * x * P_n(x) - n * P_{n-1}(x) starting from P_0(x) = 1, P_1(x) = x.
 *
 * @param[in] n                Polynomial degree (n >= 0).
 * @param[in] x                Evaluation point in [-1.0, 1.0].
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Legendre polynomial value P_n(x).
 */
MSIMD_API double msimd_legendre_polynomial(int n, double x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section29 */

/* ========================================================================= */
/* SECTION 30: WINOGRAD MINIMAL FILTERING & FAST CONVOLUTION                 */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section30 Section 30: Winograd Minimal Filtering & Fast Convolution
 * @brief Winograd F(2x2, 3x3) minimal algorithm reducing 2D convolution multiplications by 2.25x.
 * @{
 */

/**
 * @brief Computes the Winograd filter domain transformation U = G * g * G^T for a 3x3 kernel.
 *
 * Maps a 3x3 spatial filter kernel g into a 4x4 Winograd frequency domain representation U.
 *
 * @param[in]  g                Spatial 3x3 filter kernel array [9] in row-major layout.
 * @param[out] U                Output transformed 4x4 Winograd filter matrix array [16].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_winograd_f2x2_3x3_filter_transform(const float g[9], float U[16], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Winograd input tile transformation V = B^T * d * B for a 4x4 image patch.
 *
 * @param[in]  d                Spatial 4x4 input image patch tile [16].
 * @param[out] V                Output transformed 4x4 Winograd input matrix array [16].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_winograd_f2x2_3x3_input_transform(const float d[16], float V[16], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Winograd output domain transformation Y = A^T * M * A for a 2x2 output tile.
 *
 * @param[in]  M                Element-wise product matrix M = U (*) V array [16].
 * @param[out] Y                Output 2x2 filtered convolution result tile [4].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_winograd_f2x2_3x3_output_transform(const float M[16], float Y[4], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Executes one complete Winograd F(2x2, 3x3) minimal filtering step on a single 4x4 tile: Y = A^T * [(G*g*G^T) (*) (B^T*d*B)] * A.
 *
 * Reduces required floating-point multiplications from 36 (direct convolution) to 16.
 *
 * @param[in]  input_tile4x4    Input image patch 4x4 array [16].
 * @param[in]  filter_trans16   Pre-transformed 4x4 Winograd filter matrix U array [16].
 * @param[out] output_tile2x2   Computed 2x2 output feature map tile [4].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_winograd_f2x2_3x3_exec(const float input_tile4x4[16], const float filter_trans16[16], float output_tile2x2[4], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Performs full 2D convolution of an image with a 3x3 kernel using tiled Winograd F(2x2, 3x3) acceleration.
 *
 * Tiles the input image into overlapping 4x4 patches with stride 2 and aggregates 2x2 output tiles.
 *
 * @param[in]  image            Input image array [height * width].
 * @param[in]  width            Image width in pixels.
 * @param[in]  height           Image height in pixels.
 * @param[in]  kernel           3x3 convolution filter kernel array [9].
 * @param[out] output           Output filtered image array [(height - 2) * (width - 2)].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_winograd_conv2d_3x3(const float* image, int width, int height, const float kernel[9], float* output, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section30 */

/* ========================================================================= */
/* SECTION 31: 3D PROJECTIVE GEOMETRIC ALGEBRA (3D PGA R_{3,0,1}^*)          */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section31 Section 31: 3D Projective Geometric Algebra (PGA)
 * @brief Dual projective geometric algebra R_{3,0,1}^* unifying points, lines, planes, and rigid body motors.
 * @{
 */

/** @brief 3D PGA Plane primitive: a*e1 + b*e2 + c*e3 + d*e0. */
typedef struct msimd_pga_plane_t { double a, b, c, d; } msimd_pga_plane_t;
/** @brief 3D PGA Line primitive: Plucker line coordinates (direction d, moment m). */
typedef struct msimd_pga_line_t  { double dx, dy, dz, mx, my, mz; } msimd_pga_line_t;
/** @brief 3D PGA Point primitive: homogeneous coordinates x*e032 + y*e013 + z*e021 + w*e123. */
typedef struct msimd_pga_point_t { double x, y, z, w; } msimd_pga_point_t;
/** @brief 3D PGA Even multivector Motor (rotor + translator) representing general rigid body transforms in SE(3). */
typedef struct msimd_pga_motor_t { double m[8]; } msimd_pga_motor_t;

/**
 * @brief Computes the meet (intersection) of two planes yielding a 3D line: line = p1 wedge p2.
 *
 * @param[in]  p1               First plane coefficients [a, b, c, d].
 * @param[in]  p2               Second plane coefficients [a, b, c, d].
 * @param[out] line_out         Resulting intersection Plucker line [dx, dy, dz, mx, my, mz].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_pga3d_plane_meet_plane(const double p1[4], const double p2[4], double line_out[6], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the meet (intersection) of a plane and a line yielding a 3D homogeneous point: pt = p wedge l.
 *
 * @param[in]  p                Plane coefficients [a, b, c, d].
 * @param[in]  l                Plucker line coordinates [dx, dy, dz, mx, my, mz].
 * @param[out] pt_out           Output intersection point [x, y, z, w].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_pga3d_plane_meet_line(const double p[4], const double l[6], double pt_out[4], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the join (spanning line) connecting two homogeneous points: line = pt1 vee pt2.
 *
 * @param[in]  pt1              First homogeneous point [x, y, z, w].
 * @param[in]  pt2              Second homogeneous point [x, y, z, w].
 * @param[out] line_out         Output joining line [dx, dy, dz, mx, my, mz].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_pga3d_point_join_point(const double pt1[4], const double pt2[4], double line_out[6], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Composes two rigid body motion motors via the PGA geometric product: m_out = m1 * m2.
 *
 * Combines successive rotations and translations into a single 8-component even multivector motor.
 *
 * @param[in]  m1               First motor [8].
 * @param[in]  m2               Second motor [8].
 * @param[out] m_out            Composed motor [8].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_pga3d_motor_compose(const double m1[8], const double m2[8], double m_out[8], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Transforms a 3D homogeneous point using a PGA motor sandwich product: pt_out = m * pt_in * ~m.
 *
 * Executes simultaneous rigid rotation and translation without gimbal lock or matrix drift.
 *
 * @param[in]  m                8-component motor multivector.
 * @param[in]  pt_in            Input homogeneous point [x, y, z, w].
 * @param[out] pt_out           Transformed homogeneous point [x, y, z, w].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_pga3d_motor_sandwich_point(const double m[8], const double pt_in[4], double pt_out[4], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Transforms a 3D plane using a PGA motor sandwich product: pl_out = m * pl_in * ~m.
 *
 * @param[in]  m                8-component motor multivector.
 * @param[in]  pl_in            Input plane [a, b, c, d].
 * @param[out] pl_out           Transformed plane [a, b, c, d].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_pga3d_motor_sandwich_plane(const double m[8], const double pl_in[4], double pl_out[4], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section31 */

/* ========================================================================= */
/* SECTION 32: DUAL QUATERNIONS (SE(3)) & DUAL LINEAR BLENDING (DLB)         */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section32 Section 32: Dual Quaternions & Dual Linear Blending
 * @brief Unit dual quaternions in SE(3) and Dual Linear Blending (DLB) skeletal mesh skinning.
 * @{
 */

/** @brief 8-component Dual Quaternion representation: q = q_real + epsilon * q_dual where epsilon^2 = 0. */
typedef struct msimd_dual_quat_t { double q[8]; } msimd_dual_quat_t;

/**
 * @brief Constructs a unit dual quaternion from a rotational quaternion and a 3D translation vector.
 *
 * Evaluates: q_real = q_rot; q_dual = 0.5 * (trans * q_rot).
 *
 * @param[in]  q_rot            Unit rotation quaternion [w, x, y, z].
 * @param[in]  trans            Translation displacement vector [tx, ty, tz].
 * @param[out] dq_out           Output 8-component dual quaternion [8].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_dual_quat_from_rotation_translation(const double q_rot[4], const double trans[3], double dq_out[8], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Multiplies two dual quaternions: dq_out = dq1 * dq2.
 *
 * @param[in]  dq1              First dual quaternion [8].
 * @param[in]  dq2              Second dual quaternion [8].
 * @param[out] dq_out           Product dual quaternion [8].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_dual_quat_multiply(const double dq1[8], const double dq2[8], double dq_out[8], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Transforms a 3D point using a unit dual quaternion: p_out = dq * (1 + epsilon * p_in) * dq*.
 *
 * @param[in]  dq               Unit dual quaternion [8].
 * @param[in]  pt_in            Input 3D point [x, y, z].
 * @param[out] pt_out           Transformed 3D point [x, y, z].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_dual_quat_transform_point(const double dq[8], const double pt_in[3], double pt_out[3], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes Dual Linear Blending (DLB) across weighted dual quaternions: dq_out = normalize(sum w_i * dq_i).
 *
 * Eliminates skin-collapsing artifacts (candy-wrapper joint collapses) in skeletal mesh skinning.
 *
 * @param[in]  dq_array         Array of dual quaternions [count * 8].
 * @param[in]  weights          Array of convex skinning weights [count] (summing to 1.0).
 * @param[in]  count            Number of bone influences to blend.
 * @param[out] dq_out           Output normalized blended dual quaternion [8].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_dual_quat_dlb(const double* dq_array, const double* weights, size_t count, double dq_out[8], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section32 */

/* ========================================================================= */
/* SECTION 33: DISCRETE EXTERIOR CALCULUS (DEC) & CELLULAR MANIFOLDS         */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section33 Section 33: Discrete Exterior Calculus (DEC)
 * @brief Cotangent weights, Laplace-Beltrami mesh operators, and 0-form / 1-form Hodge star diagonal matrices.
 * @{
 */

/**
 * @brief Computes cotangent weights for the three interior angles of a 3D triangle.
 *
 * Cotangent formula: cot(alpha) = (u . v) / |u x v|.
 *
 * @param[in]  p0, p1, p2       3D coordinates of triangle vertices [3].
 * @param[out] cot0, cot1, cot2 Pointers receiving the cotangent weights at vertices 0, 1, and 2.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_dec_triangle_cotan_weights(const double p0[3], const double p1[3], const double p2[3], double* cot0, double* cot1, double* cot2, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Applies the discrete Laplace-Beltrami operator to a scalar field over a 2D triangular manifold mesh.
 *
 * Discretization: (Delta f)_i = (1 / 2*A_i) * sum_{j in N(i)} (cot alpha_{ij} + cot beta_{ij}) * (f_j - f_i).
 *
 * @param[in]  vertices         Vertex coordinates array [num_vertices * 3].
 * @param[in]  num_vertices     Total vertex count.
 * @param[in]  triangles        Triangle index array [num_triangles * 3].
 * @param[in]  num_triangles    Total triangle count.
 * @param[in]  scalar_field_in  Input discrete scalar field at vertices [num_vertices].
 * @param[out] laplacian_out    Output discrete Laplacian evaluated at vertices [num_vertices].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_dec_laplace_beltrami_mesh(const double* vertices, size_t num_vertices, const uint32_t* triangles, size_t num_triangles, const double* scalar_field_in, double* laplacian_out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes diagonal entries of the discrete Hodge star operator for 0-forms: (*_0)_{ii} = |dual sigma_i| / |primal sigma_i|.
 *
 * @param[in]  dual_cell_areas  Voronoi dual cell areas for vertices [n].
 * @param[in]  primal_weights   Primal vertex weights [n] (typically 1.0).
 * @param[in]  n                Number of mesh vertices.
 * @param[out] hodge_star_diag  Output diagonal elements of Hodge star matrix [n].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_dec_hodge_star_0form(const double* dual_cell_areas, const double* primal_weights, size_t n, double* hodge_star_diag, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes diagonal entries of the discrete Hodge star operator for 1-forms: (*_1)_{ee} = |dual edge| / |primal edge|.
 *
 * @param[in]  dual_edge_lengths   Lengths of dual Voronoi edges [num_edges].
 * @param[in]  primal_edge_lengths Lengths of primal mesh edges [num_edges].
 * @param[in]  num_edges           Total edge count.
 * @param[out] hodge_diag          Output diagonal Hodge star entries [num_edges].
 * @param[in]  precision_target    Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_dec_hodge_star_1form(const double* dual_edge_lengths, const double* primal_edge_lengths, size_t num_edges, double* hodge_diag, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section33 */

/* ========================================================================= */
/* SECTION 34: DISCRETE VARIATIONAL MECHANICS & SYMPLECTIC INTEGRATORS       */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section34 Section 34: Discrete Variational Mechanics & Symplectic Integrators
 * @brief Symplectic Euler, Velocity Verlet, Discrete Euler-Lagrange stepping, and symplectic energy drift metrics.
 * @{
 */

/**
 * @brief Advances phase-space coordinates by one time step using the first-order Symplectic Euler method.
 *
 * Evaluates: v_{n+1} = v_n + dt * M^{-1} * F(x_n); x_{n+1} = x_n + dt * v_{n+1}.
 *
 * @param[in,out] pos              Particle position coordinate array [n] updated in-place.
 * @param[in,out] vel              Particle velocity array [n] updated in-place.
 * @param[in]     forces           Applied force vector array [n].
 * @param[in]     inv_mass         Inverse mass array [n].
 * @param[in]     n                Number of degrees of freedom.
 * @param[in]     dt               Time step duration.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_symplectic_euler_step(double* pos, double* vel, const double* forces, const double* inv_mass, size_t n, double dt, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Advances dynamics using the second-order Velocity Verlet symplectic integrator.
 *
 * Evaluates: x_{n+1} = x_n + dt * v_n + 0.5 * dt^2 * a_n; v_{n+1} = v_n + 0.5 * dt * (a_n + a_{n+1}).
 *
 * @param[in,out] pos              Particle position array [n].
 * @param[in,out] vel              Particle velocity array [n].
 * @param[in,out] forces           Force array [n] updated with newly computed forces at x_{n+1}.
 * @param[in]     inv_mass         Inverse mass array [n].
 * @param[in]     compute_forces   Callback function recomputing forces given new positions.
 * @param[in]     n                Degrees of freedom.
 * @param[in]     dt               Time step size.
 * @param[in,out] ctx              User context pointer passed to compute_forces.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_velocity_verlet_step(double* pos, double* vel, double* forces, const double* inv_mass, void (*compute_forces)(const double* pos, double* forces, void* ctx), size_t n, double dt, void* ctx, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Advances a trajectory using the Discrete Euler-Lagrange equations: D_2 L_d(q_{k-1}, q_k) + D_1 L_d(q_k, q_{k+1}) = 0.
 *
 * @param[in]  q_prev           Positions at time step k - 1 [n].
 * @param[in]  q_curr           Positions at time step k [n].
 * @param[in]  d_L_d_q_curr     Derivative of discrete action sum with respect to q_k [n].
 * @param[in]  dt               Discrete time step.
 * @param[in]  n                Degrees of freedom.
 * @param[out] q_next           Output calculated position vector at step k + 1 [n].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_discrete_euler_lagrange_step(const double* q_prev, const double* q_curr, const double* d_L_d_q_curr, double dt, size_t n, double* q_next, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Measures relative energy drift in a numerical simulation: drift = |E(t) - E(0)| / |E(0)|.
 *
 * Used to audit symplectic conservation of invariants over millions of simulation steps.
 *
 * @param[in] initial_energy   Total mechanical energy at step 0.
 * @param[in] current_energy   Total mechanical energy at current step.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Normalized non-negative fractional energy drift.
 */
MSIMD_API double msimd_symplectic_energy_drift(double initial_energy, double current_energy, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section34 */

/* ========================================================================= */
/* SECTION 35: EXTENDED POSITION-BASED DYNAMICS (XPBD)                       */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section35 Section 35: Extended Position-Based Dynamics (XPBD)
 * @brief Elastic distance constraints, tetrahedral volume conservation, and contact plane constraints.
 * @{
 */

/**
 * @brief Enforces an elastic distance constraint between two particles using Extended Position-Based Dynamics (XPBD).
 *
 * Constraint: C(p1, p2) = |p1 - p2| - rest_len = 0.
 * Solves: Delta lambda = (-C - compliance_tilde * lambda) / (sum w_i * |grad C_i|^2 + compliance_tilde),
 * where compliance_tilde = compliance / dt^2.
 *
 * @param[in,out] p1, p2           Particle 3D positions [3] updated in-place.
 * @param[in]     inv_m1, inv_m2   Inverse masses of particles.
 * @param[in]     rest_len         Unstretched rest distance.
 * @param[in]     compliance       Physical compliance alpha (inverse stiffness, m/N).
 * @param[in]     dt               Substep time duration.
 * @param[in,out] lambda           Accumulated Lagrange multiplier updated in-place.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_xpbd_distance_constraint(double* p1, double* p2, double inv_m1, double inv_m2, double rest_len, double compliance, double dt, double* lambda, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Enforces hydrostatic volume conservation across a deformable tetrahedral element in XPBD.
 *
 * Constraint: C(p1, p2, p3, p4) = (1/6) * ((p2 - p1) x (p3 - p1)) . (p4 - p1) - rest_volume = 0.
 *
 * @param[in,out] p1, p2, p3, p4   Tetrahedron 3D vertex positions [3] updated in-place.
 * @param[in]     inv_m1..4        Inverse masses of the 4 vertices.
 * @param[in]     rest_volume      Initial undeformed tetrahedral volume.
 * @param[in]     compliance       Hydrostatic volume compliance.
 * @param[in]     dt               Substep time step.
 * @param[in,out] lambda           Accumulated Lagrange multiplier updated in-place.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_xpbd_tetrahedron_volume_constraint(double* p1, double* p2, double* p3, double* p4, double inv_m1, double inv_m2, double inv_m3, double inv_m4, double rest_volume, double compliance, double dt, double* lambda, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Enforces an inequality non-penetration contact constraint against a static half-space plane in XPBD.
 *
 * Constraint: C(p) = (p - plane_point) . plane_normal >= 0.
 *
 * @param[in,out] p                Particle 3D position [3] updated if penetrating plane.
 * @param[in]     inv_m            Inverse mass of particle.
 * @param[in]     plane_point      Point on the contact plane [3].
 * @param[in]     plane_normal     Unit normal pointing into free space [3].
 * @param[in]     compliance       Contact compliance (soft contact stiffness).
 * @param[in]     dt               Substep time duration.
 * @param[in,out] lambda           Accumulated normal contact impulse multiplier.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_xpbd_contact_plane_constraint(double* p, double inv_m, const double plane_point[3], const double plane_normal[3], double compliance, double dt, double* lambda, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section35 */

/* ========================================================================= */
/* SECTION 36: CELLULAR & SPH FLUID DYNAMICS (WENDLAND C2 & LBM D3Q19 BGK)   */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section36 Section 36: SPH & LBM Fluid Dynamics
 * @brief Wendland C2 smoothing kernels, pairwise SPH pressure/viscosity forces, and 3D Lattice Boltzmann D3Q19 BGK flows.
 * @{
 */

/**
 * @brief Evaluates the Wendland C2 quintic smoothing kernel and its radial spatial derivative in 3D SPH.
 *
 * Kernel: W(r, h) = alpha_d * (1 - q)^4 * (4q + 1) for q = r / h <= 1, with normalization alpha_d = 21 / (2 * pi * h^3).
 *
 * @param[in]  r                Inter-particle radial distance (r >= 0).
 * @param[in]  h                Smoothing kernel support radius (smoothing length).
 * @param[out] w                Pointer receiving evaluated smoothing kernel weight W(r, h).
 * @param[out] dw_dr            Pointer receiving kernel radial derivative dW/dr.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_sph_wendland_c2_kernel(double r, double h, double* w, double* dw_dr, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes symmetric pairwise SPH pressure and artificial viscosity forces between two fluid particles.
 *
 * @param[in]  p_i, p_j         Positions of particles i and j [3].
 * @param[in]  v_i, v_j         Velocities of particles i and j [3].
 * @param[in]  m_j              Mass of neighbor particle j.
 * @param[in]  rho_i, rho_j     Fluid densities at particles i and j.
 * @param[in]  pressure_i, j    Fluid pressures at particles i and j.
 * @param[in]  h                Smoothing length.
 * @param[in]  visc_eta         Dynamic shear viscosity coefficient.
 * @param[out] f_out            Output interaction force vector acting on particle i [3].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_sph_wendland_pair_force(const double p_i[3], const double p_j[3], const double v_i[3], const double v_j[3], double m_j, double rho_i, double rho_j, double pressure_i, double pressure_j, double h, double visc_eta, double f_out[3], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Advances a 3D Lattice Boltzmann Method (LBM) D3Q19 single-relaxation-time (BGK) collision and streaming step.
 *
 * Evolves 19 discrete velocity distribution functions across an [nx * ny * nz] grid with bounce-back boundary conditions.
 *
 * @param[in]  f_in             Incoming distribution functions array [19 * nx * ny * nz].
 * @param[out] f_out            Outgoing collided and streamed distribution functions [19 * nx * ny * nz].
 * @param[in]  nx, ny, nz       Lattice spatial dimensions.
 * @param[in]  tau              Dimensionless BGK relaxation time parameter (kinematic viscosity nu = (tau - 0.5) / 3).
 * @param[in]  obstacle_mask    Boolean obstacle boundary mask array [nx * ny * nz] (1 for solid wall, 0 for fluid).
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_lbm_d3q19_bgk_collision_and_stream(const float* f_in, float* f_out, int nx, int ny, int nz, float tau, const uint8_t* obstacle_mask, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section36 */

/* ========================================================================= */
/* SECTION 37: QUANTUM STATEVECTOR & UNITARY GATE DYNAMICS                   */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section37 Section 37: Quantum Statevector & Unitary Gate Dynamics
 * @brief Universal quantum quantum circuit simulation: Pauli X/Y/Z, Hadamard, Phase, CNOT, and Toffoli gates.
 * @{
 */

/**
 * @brief Applies the single-qubit Pauli-X (NOT / bit-flip) unitary gate: X = [[0, 1], [1, 0]] to a statevector.
 *
 * Permutes statevector amplitudes: |...0...> <-> |...1...> across 2^{num_qubits} complex amplitudes.
 *
 * @param[in,out] psi_re           Statevector real amplitudes array [2^{num_qubits}] updated in-place.
 * @param[in,out] psi_im           Statevector imaginary amplitudes array [2^{num_qubits}] updated in-place.
 * @param[in]     num_qubits       Total number of entangled qubits in the system.
 * @param[in]     target_qubit     Target qubit index (0 to num_qubits - 1).
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quantum_apply_pauli_x(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Applies the single-qubit Pauli-Y unitary gate: Y = [[0, -i], [i, 0]] to a statevector.
 *
 * @param[in,out] psi_re, psi_im   Statevector real and imaginary amplitude arrays.
 * @param[in]     num_qubits       Total qubit count.
 * @param[in]     target_qubit     Target qubit index.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quantum_apply_pauli_y(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Applies the single-qubit Pauli-Z (phase-flip) unitary gate: Z = [[1, 0], [0, -1]] to a statevector.
 *
 * @param[in,out] psi_re, psi_im   Statevector real and imaginary amplitude arrays.
 * @param[in]     num_qubits       Total qubit count.
 * @param[in]     target_qubit     Target qubit index.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quantum_apply_pauli_z(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Applies the single-qubit Hadamard superposition gate: H = (1 / sqrt(2)) * [[1, 1], [1, -1]].
 *
 * Maps basis states |0> -> (|0> + |1>)/sqrt(2) and |1> -> (|0> - |1>)/sqrt(2).
 *
 * @param[in,out] psi_re, psi_im   Statevector real and imaginary amplitude arrays.
 * @param[in]     num_qubits       Total qubit count.
 * @param[in]     target_qubit     Target qubit index.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quantum_apply_hadamard(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Applies an arbitrary single-qubit phase shift gate: R(theta) = [[1, 0], [0, exp(i * theta)]].
 *
 * @param[in,out] psi_re, psi_im   Statevector amplitude arrays.
 * @param[in]     num_qubits       Total qubit count.
 * @param[in]     target_qubit     Target qubit index.
 * @param[in]     theta            Phase rotation angle in radians.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quantum_apply_phase(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, double theta, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Applies the two-qubit Controlled-NOT (CNOT / CX) entangling gate to a statevector.
 *
 * Inverts the target qubit amplitude if and only if the control qubit is in state |1>.
 *
 * @param[in,out] psi_re, psi_im   Statevector amplitude arrays.
 * @param[in]     num_qubits       Total qubit count.
 * @param[in]     control_qubit    Control qubit index.
 * @param[in]     target_qubit     Target qubit index.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quantum_apply_cnot(double* psi_re, double* psi_im, size_t num_qubits, size_t control_qubit, size_t target_qubit, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Applies the three-qubit Toffoli (Controlled-Controlled-NOT / CCX) reversible logic gate.
 *
 * @param[in,out] psi_re, psi_im   Statevector amplitude arrays.
 * @param[in]     num_qubits       Total qubit count.
 * @param[in]     ctrl1, ctrl2     First and second control qubit indices.
 * @param[in]     target_qubit     Target qubit index.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quantum_apply_toffoli(double* psi_re, double* psi_im, size_t num_qubits, size_t ctrl1, size_t ctrl2, size_t target_qubit, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes the Born rule measurement probability distribution for a target qubit: P(0) and P(1).
 *
 * Sums squared magnitudes |psi_k|^2 across all basis states where target_qubit is 0 or 1.
 *
 * @param[in]  psi_re, psi_im   Statevector amplitude arrays.
 * @param[in]  num_qubits       Total qubit count.
 * @param[in]  target_qubit     Qubit index to measure.
 * @param[out] prob_0           Pointer receiving probability of measuring state |0>.
 * @param[out] prob_1           Pointer receiving probability of measuring state |1>.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_quantum_qubit_probability(const double* psi_re, const double* psi_im, size_t num_qubits, size_t target_qubit, double* prob_0, double* prob_1, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section37 */

/* ========================================================================= */
/* SECTION 38: DISCRETE SPACETIME, REGGE CALCULUS & WILSON LOOPS             */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section38 Section 38: Discrete Spacetime & Lattice Gauge Theory
 * @brief Regge calculus curvature deficit angles, Wilson gauge plaquettes, and 4D lattice Wilson-Dirac fermion operators.
 * @{
 */

/**
 * @brief Computes the gravitational curvature deficit angle epsilon around a vertex in 2D Regge simplicial spacetime.
 *
 * Deficit angle: epsilon = 2*pi - sum_{t in Star(v)} theta_t, where theta_t is the dihedral/interior angle in simplex t.
 *
 * @param[in]  edge_lengths_around_vertex Array of edge lengths defining triangles incident to vertex.
 * @param[in]  num_triangles              Number of triangles sharing the vertex.
 * @param[out] deficit_angle              Pointer receiving curvature deficit angle (radians).
 * @param[in]  precision_target           Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_regge_deficit_angle_2d(const double* edge_lengths_around_vertex, size_t num_triangles, double* deficit_angle, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes average Wilson gauge action across all elementary 1x1 plaquettes in a 4D U(1) lattice: S = sum (1 - cos(theta_{mu,nu})).
 *
 * @param[in]  theta_links      Gauge link angle array [4 * nx * ny * nz * nt] representing U_mu(x) = exp(i * theta).
 * @param[in]  nx, ny, nz, nt   Lattice spacetime dimensions.
 * @param[out] average_action   Pointer receiving average action per plaquette.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_wilson_plaquette_u1(const double* theta_links, int nx, int ny, int nz, int nt, double* average_action, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Applies the 4D lattice Wilson-Dirac fermion operator D_W to a fermion spinor field.
 *
 * Eliminates fermion doubling species using the Wilson parameter r_wilson:
 * D_W psi(x) = (m_0 + 4*r)*psi(x) - 0.5 * sum_mu [(r - gamma_mu)*U_mu(x)*psi(x+mu) + (r + gamma_mu)*U_mu^dag(x-mu)*psi(x-mu)].
 *
 * @param[in]  psi_in           Input 4-component complex fermion field array.
 * @param[out] psi_out          Output fermion field after operator application.
 * @param[in]  gauge_u1         U(1) gauge link field array.
 * @param[in]  nx, ny, nz, nt   4D lattice dimensions.
 * @param[in]  mass_m0          Bare fermion mass parameter m_0.
 * @param[in]  r_wilson         Wilson parameter (typically 1.0).
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_wilson_dirac_operator_4d(const double* psi_in, double* psi_out, const double* gauge_u1, int nx, int ny, int nz, int nt, double mass_m0, double r_wilson, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section38 */

/* ========================================================================= */
/* SECTION 39: ABSTRACT CONTINUOUS RELATIVITY & GEODESIC TRACING             */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section39 Section 39: Differential Geometry & Geodesic Tracing
 * @brief Christoffel symbols of the second kind, geodesic numerical integration, Riemann curvature tensor, and Ricci scalar.
 * @{
 */

/**
 * @brief Computes Christoffel symbols of the second kind from metric inverse and derivatives: Gamma^lambda_{mu,nu} = 0.5 * g^{lambda,sigma} * (d_mu g_{nu,sigma} + d_nu g_{mu,sigma} - d_sigma g_{mu,nu}).
 *
 * @param[in]  g_inv            Inverse metric tensor g^{mu,nu} [4][4].
 * @param[in]  d_g              Partial derivatives of metric tensor d_alpha g_{beta,gamma} [4][4][4].
 * @param[out] gamma            Resulting Christoffel symbols Gamma^lambda_{mu,nu} [4][4][4].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_relativity_christoffel_symbols(const double g_inv[4][4], const double d_g[4][4][4], double gamma[4][4][4], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Advances a particle geodesic trajectory by one affine step d_lambda: d^2 x^mu / d lambda^2 + Gamma^mu_{alpha,beta} * u^alpha * u^beta = 0.
 *
 * @param[in]  x                Current 4-position coordinate [4] = [ct, x, y, z].
 * @param[in]  u                Current 4-velocity vector [4] = dx/d_lambda.
 * @param[in]  gamma            Christoffel symbols of 2nd kind [4][4][4].
 * @param[in]  d_lambda         Affine parameter increment.
 * @param[out] x_next           Updated 4-position coordinates [4].
 * @param[out] u_next           Updated 4-velocity vector [4].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_relativity_geodesic_step(const double x[4], const double u[4], const double gamma[4][4][4], double d_lambda, double x_next[4], double u_next[4], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates the Riemann curvature tensor R^rho_{sigma,mu,nu}, Ricci tensor R_{mu,nu}, and Ricci scalar R from Christoffel symbols.
 *
 * @param[in]  gamma            Christoffel symbols [4][4][4].
 * @param[in]  d_gamma          Partial derivatives of Christoffel symbols d_alpha Gamma^beta_{mu,nu} [4][4][4][4].
 * @param[out] riemann          Resulting Riemann curvature tensor [4][4][4][4].
 * @param[out] ricci            Resulting 4x4 Ricci curvature tensor [4][4].
 * @param[out] ricci_scalar     Pointer receiving scalar Ricci curvature R = g^{mu,nu} * R_{mu,nu}.
 * @param[in]  g_inv            Inverse metric tensor [4][4].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_relativity_riemann_tensor(const double gamma[4][4][4], const double d_gamma[4][4][4][4], double riemann[4][4][4][4], double ricci[4][4], double* ricci_scalar, const double g_inv[4][4], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section39 */

/* ========================================================================= */
/* SECTION 40: COMPUTATIONAL ALGEBRAIC GEOMETRY & HOMOTOPY CONTINUATION      */
/* (MSC 14: Algebraic Geometry)                                              */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section40 Section 40: Computational Algebraic Geometry & Homotopy Continuation
 * @brief Predictor-corrector homotopy path trackers, surface singularity detection, and elliptic curve group laws.
 * @{
 */

/**
 * @brief Advances a polynomial system root tracker along a homotopy path H(x, t) = (1 - t)*G(x) + t*F(x) = 0 using Euler predictor + Newton corrector.
 *
 * @param[in]     x                Current root coordinate vector [n].
 * @param[in]     t                Current continuation parameter in [0.0, 1.0].
 * @param[in]     dt               Continuation step size along parameter t.
 * @param[in]     eval_H           Callback evaluating homotopy residual vector H(x, t).
 * @param[in]     eval_JH          Callback evaluating Jacobian matrix J_H(x, t) [n * n].
 * @param[in]     eval_dH_dt       Callback evaluating parameter derivative dH/dt [n].
 * @param[in]     n                Polynomial system dimension.
 * @param[out]    x_next           Updated root coordinate vector [n].
 * @param[in,out] ctx              User context pointer.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_nag_homotopy_step(const double* x, double t, double dt, void (*eval_H)(const double* x, double t, double* H_val, void* ctx), void (*eval_JH)(const double* x, double t, double* J_val, void* ctx), void (*eval_dH_dt)(const double* x, double t, double* dH_val, void* ctx), size_t n, double* x_next, void* ctx, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates gradient, 3x3 Hessian matrix, Hessian determinant, and singularity status for an implicit algebraic surface F(x, y, z) = 0.
 *
 * @param[in]  x, y, z          Spatial evaluation point.
 * @param[in]  F_fn             Implicit function callback evaluating F(x, y, z, ctx).
 * @param[out] grad             Output spatial gradient vector [3].
 * @param[out] hessian          Output 3x3 Hessian matrix of second partial derivatives [3][3].
 * @param[out] hessian_det      Pointer receiving determinant of the 3x3 Hessian matrix.
 * @param[out] is_singular      Pointer receiving 1 if |grad| < 1e-7 (singular point), 0 if smooth regular point.
 * @param[in]  ctx              User context pointer.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_nag_surface_normal_and_singularity(double x, double y, double z, double (*F_fn)(double x, double y, double z, void* ctx), double grad[3], double hessian[3][3], double* hessian_det, int* is_singular, void* ctx, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes point addition on a Weierstrass elliptic curve: y^2 = x^3 + a*x + b: P3 = P1 + P2.
 *
 * Implements the chord-and-tangent group law over real coordinates. Handles point doubling when P1 == P2.
 *
 * @param[in]  x1, y1           Cartesian coordinates of point P1.
 * @param[in]  x2, y2           Cartesian coordinates of point P2.
 * @param[in]  a                Curve coefficient 'a'.
 * @param[out] x3, y3           Pointers receiving coordinates of resulting sum point P3.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_nag_elliptic_curve_add(double x1, double y1, double x2, double y2, double a, double* x3, double* y3, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section40 */

/* ========================================================================= */
/* SECTION 41: SYSTEMS THEORY, KALMAN FILTERING & OPTIMAL CONTROL            */
/* (MSC 93: Systems Theory; Control)                                         */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section41 Section 41: Systems Theory, Filtering & Optimal Control
 * @brief State-space Kalman predictors with control inputs, anti-windup PID controllers, and Discrete Algebraic Riccati LQR solvers.
 * @{
 */

/**
 * @brief Advances a continuous/discrete state-space Kalman filter with explicit control inputs: x_{k+1} = F*x + B*u + w; z = H*x + v.
 *
 * @param[in,out] x                State vector [state_dim] updated in-place.
 * @param[in,out] P                Error covariance matrix [state_dim * state_dim] updated in-place.
 * @param[in]     F                State transition dynamics matrix [state_dim * state_dim].
 * @param[in]     B                Control input matrix [state_dim * ctrl_dim].
 * @param[in]     u                Control input vector [ctrl_dim].
 * @param[in]     Q                Process noise covariance matrix [state_dim * state_dim].
 * @param[in]     z                Sensor measurement observation vector [obs_dim].
 * @param[in]     H                Measurement observation matrix [obs_dim * state_dim].
 * @param[in]     R                Measurement noise covariance matrix [obs_dim * obs_dim].
 * @param[in]     state_dim        State space dimensionality.
 * @param[in]     obs_dim          Measurement space dimensionality.
 * @param[in]     ctrl_dim         Control input dimensionality.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_control_kalman_filter_step(double* x, double* P, const double* F, const double* B, const double* u, const double* Q, const double* z, const double* H, const double* R, size_t state_dim, size_t obs_dim, size_t ctrl_dim, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes one step of a Proportional-Integral-Derivative (PID) controller with derivative low-pass filtering and anti-windup clamping.
 *
 * @param[in]     setpoint         Target reference setpoint value.
 * @param[in]     measurement      Current measured process variable value.
 * @param[in]     kp, ki, kd       Proportional, integral, and derivative gains.
 * @param[in]     dt               Sampling time step.
 * @param[in]     min_out, max_out Saturation output clamping limits.
 * @param[in]     filter_alpha     First-order derivative low-pass filter coefficient in [0.0, 1.0].
 * @param[in,out] integral_state   Pointer to accumulated integrator state variable.
 * @param[in,out] prev_meas        Pointer to previous measurement value (for derivative kick prevention).
 * @param[in,out] prev_deriv       Pointer to previous filtered derivative state.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 * @return Computed control output command in [min_out, max_out].
 */
MSIMD_API double msimd_control_pid_step(double setpoint, double measurement, double kp, double ki, double kd, double dt, double min_out, double max_out, double filter_alpha, double* integral_state, double* prev_meas, double* prev_deriv, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Executes one fixed-point iteration of the Discrete-time Algebraic Riccati Equation (DARE) for Linear-Quadratic Regulators (LQR).
 *
 * Iterates: P_{k+1} = Q + A^T * P_k * A - (A^T * P_k * B) * (R + B^T * P_k * B)^{-1} * (B^T * P_k * A) and updates feedback gain K.
 *
 * @param[in]  A                State dynamic matrix [n * n].
 * @param[in]  B                Input actuation matrix [n * m].
 * @param[in]  Q                State cost weighting matrix [n * n].
 * @param[in]  R                Control effort cost weighting matrix [m * m].
 * @param[in]  n                State dimension.
 * @param[in]  m                Control input dimension.
 * @param[out] P                Riccati solution matrix [n * n] updated in-place.
 * @param[out] K                LQR optimal feedback gain matrix [m * n].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_control_lqr_riccati_iteration(const double* A, const double* B, const double* Q, const double* R, size_t n, size_t m, double* P, double* K, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section41 */

/* ========================================================================= */
/* SECTION 42: OPERATIONS RESEARCH, SIMPLEX & INTERIOR-POINT METHODS         */
/* (MSC 90: Operations Research, Mathematical Programming)                   */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section42 Section 42: Operations Research & Simplex Linear Programming
 * @brief Gauss-Jordan simplex tableau pivot transformations and Dantzig linear programming steps.
 * @{
 */

/**
 * @brief Performs an in-place Gauss-Jordan pivot operation on a linear programming simplex tableau.
 *
 * Normalizes pivot row by pivot element tableau[pivot_row][pivot_col], then eliminates all other
 * entries in pivot_col across all tableau rows.
 *
 * @param[in,out] tableau          2D Simplex tableau matrix array [num_rows * num_cols] updated in-place.
 * @param[in]     num_rows         Number of rows in tableau (constraints + objective).
 * @param[in]     num_cols         Number of columns in tableau (variables + slacks + RHS).
 * @param[in]     pivot_row        Row index of the selected pivot element.
 * @param[in]     pivot_col        Column index of the entering variable.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_opt_simplex_pivot(double* tableau, size_t num_rows, size_t num_cols, size_t pivot_row, size_t pivot_col, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Executes one complete Dantzig simplex step: chooses entering column, performs minimum ratio test, and pivots.
 *
 * @param[in,out] tableau          Simplex tableau matrix [num_rows * num_cols].
 * @param[in]     num_rows         Tableau row count.
 * @param[in]     num_cols         Tableau column count.
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 * @return 0 if optimal solution is found, 1 if step was performed successfully, -1 if problem is unbounded.
 */
MSIMD_API int  msimd_opt_simplex_step(double* tableau, size_t num_rows, size_t num_cols, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section42 */

/* ========================================================================= */
/* SECTION 43: GAME THEORY, NASH EQUILIBRIUM & MINIMAX STRATEGIES            */
/* (MSC 91: Game Theory, Economics, Social and Behavioral Sciences)          */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section43 Section 43: Game Theory & Minimax Strategies
 * @brief Brown\'s fictitious play learning dynamics and two-player zero-sum game value evaluations.
 * @{
 */

/**
 * @brief Advances one iteration of George W. Brown\'s fictitious play learning dynamical system for bimatrix games.
 *
 * Both players observe historical empirical frequencies of opponent actions and select best-response pure strategies,
 * converging toward a mixed Nash equilibrium.
 *
 * @param[in]     A                Payoff matrix for Player 1 [m * n].
 * @param[in]     B                Payoff matrix for Player 2 [m * n].
 * @param[in]     m                Action count for Player 1.
 * @param[in]     n                Action count for Player 2.
 * @param[in,out] p1_counts        Cumulative action count histogram for Player 1 [m].
 * @param[in,out] p2_counts        Cumulative action count histogram for Player 2 [n].
 * @param[out]    p1_strategy      Empirical mixed strategy probability vector for Player 1 [m].
 * @param[out]    p2_strategy      Empirical mixed strategy probability vector for Player 2 [n].
 * @param[in]     iteration        Current iteration index (>= 1).
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_game_fictitious_play_step(const double* A, const double* B, size_t m, size_t n, double* p1_counts, double* p2_counts, double* p1_strategy, double* p2_strategy, size_t iteration, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates expected game value for a zero-sum game under mixed strategies: V = p1^T * A * p2.
 *
 * @param[in] payoff_matrix    Payoff matrix A [m * n].
 * @param[in] p1_strat         Player 1 mixed strategy probability distribution [m].
 * @param[in] p2_strat         Player 2 mixed strategy probability distribution [n].
 * @param[in] m                Action count for Player 1.
 * @param[in] n                Action count for Player 2.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Expected payoff value V to Player 1.
 */
MSIMD_API double msimd_game_zero_sum_value(const double* payoff_matrix, const double* p1_strat, const double* p2_strat, size_t m, size_t n, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section43 */

/* ========================================================================= */
/* SECTION 44: MATHEMATICAL BIOLOGY & POPULATION DYNAMICS                    */
/* (MSC 92: Biology and Other Natural Sciences)                              */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section44 Section 44: Mathematical Biology & Population Dynamics
 * @brief Lotka-Volterra predator-prey dynamics, SEIR epidemic progression, and Michaelis-Menten enzyme kinetics.
 * @{
 */

/**
 * @brief Advances the continuous non-linear Lotka-Volterra predator-prey differential equations by one time step dt.
 *
 * System: d(prey)/dt = alpha * prey - beta * prey * predator; d(predator)/dt = delta * prey * predator - gamma * predator.
 *
 * @param[in]  prey, predator   Current prey and predator population densities.
 * @param[in]  alpha            Prey natural reproduction growth rate.
 * @param[in]  beta             Predation kill rate.
 * @param[in]  gamma            Predator natural mortality rate.
 * @param[in]  delta            Predator reproduction efficiency per prey consumed.
 * @param[in]  dt               Time step duration.
 * @param[out] next_prey        Pointer receiving updated prey density.
 * @param[out] next_predator    Pointer receiving updated predator density.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_bio_lotka_volterra_step(double prey, double predator, double alpha, double beta, double gamma, double delta, double dt, double* next_prey, double* next_predator, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Advances a four-compartment SEIR (Susceptible-Exposed-Infectious-Recovered) epidemiological model by one time step dt.
 *
 * Equations: dS/dt = -beta*S*I; dE/dt = beta*S*I - sigma*E; dI/dt = sigma*E - gamma*I; dR/dt = gamma*I.
 *
 * @param[in]  S, E, I, R       Current compartment population fractions (summing to 1.0).
 * @param[in]  beta             Transmission contact infection rate.
 * @param[in]  sigma            Incubation transition rate (1 / latent period).
 * @param[in]  gamma            Recovery removal rate (1 / infectious period).
 * @param[in]  dt               Time increment.
 * @param[out] next_S..next_R   Pointers receiving updated compartment population fractions.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_bio_seir_step(double S, double E, double I, double R, double beta, double sigma, double gamma, double dt, double* next_S, double* next_E, double* next_I, double* next_R, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes enzyme reaction catalysis rate via Michaelis-Menten kinetics: v = (vmax * [S]) / (Km + [S]).
 *
 * @param[in]  substrate_conc   Substrate chemical concentration [S] (mol/L).
 * @param[in]  vmax             Maximum reaction velocity at enzyme saturation.
 * @param[in]  km               Michaelis constant (substrate concentration at half vmax).
 * @param[out] rate             Pointer receiving catalyzed reaction velocity v.
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_bio_michaelis_menten_rate(double substrate_conc, double vmax, double km, double* rate, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section44 */

/* ========================================================================= */
/* SECTION 45: TOPOLOGICAL DATA ANALYSIS & PERSISTENT HOMOLOGY               */
/* (MSC 55/68: Algebraic Topology & Topological Data Analysis)               */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section45 Section 45: Topological Data Analysis & Persistent Homology
 * @brief Pairwise Euclidean distance matrices for Vietoris-Rips complexes and boundary matrix Gaussian reduction for barcodes.
 * @{
 */

/**
 * @brief Computes an N x N symmetric pairwise Euclidean metric distance matrix across multi-dimensional point cloud samples.
 *
 * Evaluates D_{ij} = ||p_i - p_j||_2 accelerated via SIMD dot products.
 *
 * @param[in]  points           Point coordinate array [num_points * dim] in row-major layout.
 * @param[in]  num_points       Total point count (N).
 * @param[in]  dim              Spatial dimensionality of each point.
 * @param[out] dist_matrix      Output N x N symmetric pairwise distance matrix [num_points * num_points].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_tda_pairwise_distance_matrix(const double* points, size_t num_points, size_t dim, double* dist_matrix, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Performs column reduction on a binary simplicial boundary matrix modulo 2 to compute persistent homology barcode intervals.
 *
 * Reduces boundary matrix using left-to-right column additions over GF(2) such that no two columns have the same lowest non-zero row index (pivot).
 *
 * @param[in,out] boundary_matrix  Binary boundary matrix array [num_simplices * num_simplices] updated in-place.
 * @param[in]     num_simplices    Number of simplices (rows and columns) in the filtration.
 * @param[out]    pivots           Array [num_simplices] receiving lowest row pivot index for each column (-1 if empty).
 * @param[in]     precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void msimd_tda_boundary_matrix_reduce(uint8_t* boundary_matrix, size_t num_simplices, int64_t* pivots, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section45 */

/* ========================================================================= */
/* SECTION 46: GRAPH ALGORITHMS & NETWORK FLOW                               */
/* (MSC 68: Computer Science, Discrete Mathematics & Graph Algorithms)       */
/* ========================================================================= */

/**
 * @defgroup MSIMD_Section46 Section 46: Graph Algorithms & Network Flow
 * @brief Dijkstra shortest paths, Bellman-Ford negative cycle detection, and Edmonds-Karp maximum network flow.
 * @{
 */

/**
 * @brief Computes single-source shortest paths on a directed weighted graph using Dijkstra\'s algorithm with min-heap priority queue.
 *
 * @param[in]  adj_matrix       Directed adjacency weight matrix [n * n] (infinity or < 0 indicates no edge).
 * @param[in]  n                Vertex count.
 * @param[in]  src              Source root vertex index.
 * @param[out] distances        Output array [n] receiving shortest path distances from src.
 * @param[out] predecessors     Output array [n] receiving predecessor vertex indices along shortest paths (-1 for src/unreachable).
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 */
MSIMD_API void   msimd_graph_dijkstra_shortest_paths(const double* adj_matrix, size_t n, size_t src, double* distances, int64_t* predecessors, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Solves single-source shortest paths and detects negative-weight cycles using the Bellman-Ford algorithm.
 *
 * Relaxes all edges N-1 times and checks for further distance reductions.
 *
 * @param[in]  adj_matrix       Directed edge weight matrix [n * n].
 * @param[in]  n                Vertex count.
 * @param[in]  src              Source vertex index.
 * @param[out] distances        Output shortest path distance array [n].
 * @param[out] predecessors     Output predecessor array [n].
 * @param[in]  precision_target Optional precision descriptor (default: nullptr).
 * @return 0 on success, or -1 if a reachable negative-weight cycle is detected.
 */
MSIMD_API int    msimd_graph_bellman_ford(const double* adj_matrix, size_t n, size_t src, double* distances, int64_t* predecessors, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computes maximum network flow from source to sink using the Edmonds-Karp algorithm (Ford-Fulkerson with BFS augmenting paths).
 *
 * Finds augmenting paths in O(V * E^2) time and constructs the final residual capacity network.
 *
 * @param[in]  capacity_matrix      Edge capacity matrix [n * n].
 * @param[in]  n                    Vertex count.
 * @param[in]  source               Source vertex index.
 * @param[in]  sink                 Sink vertex index.
 * @param[out] residual_matrix_out  Output residual capacity matrix [n * n].
 * @param[in]  precision_target     Optional precision descriptor (default: nullptr).
 * @return Total maximum flow volume from source to sink.
 */
MSIMD_API double msimd_graph_max_flow_edmonds_karp(const double* capacity_matrix, size_t n, size_t source, size_t sink, double* residual_matrix_out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section46 */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MATHVANTAGE_SIMD_C_API_H */
