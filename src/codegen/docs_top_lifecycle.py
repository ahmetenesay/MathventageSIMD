# -*- coding: utf-8 -*-
"""
Module providing Doxygen replacements for top constants, enums, structs,
and context/lattice lifecycle functions (lines 1 to 460) in include/MathventageSIMD.h.
"""

TOP_CONSTANTS_ENGLISH = '''/* ========================================================================= */
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
#define MSIMD_PLANCK_IMPEDANCE      29.9792458             /* Z_P = 1 / (4*pi*eps_0*c) = Z_0 / (4*pi) (Ohms) */'''

MSC_AND_TIERS_ENGLISH = '''/* ========================================================================= */
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
#define MSIMD_TIER_5_SOFTFLOAT_ALU  5 /* Pure Integer ALU Soft-Float & Q15/Q31 Fixed-Point Emulation */'''

LIFECYCLE_FUNCTIONS_DOC = '''/* ========================================================================= */
/* Context & Memory Lifecycle                                                */
/* ========================================================================= */

/**
 * @brief Initializes a primary MathventageSIMD engine execution context.
 *
 * Allocates and configures the core runtime context, probes host CPU instruction sets,
 * verifies vector register width capabilities, and establishes default thread worker pools.
 *
 * The selected ISA and backend determine code dispatch paths throughout the library\\'s lifetime.
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
MSIMD_API void  msimd_lattice_swap(msimd_lattice lat);'''
