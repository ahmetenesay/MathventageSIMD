# -*- coding: utf-8 -*-
"""
Module providing Doxygen replacements for Sections 28 to 46 (lines 1886 to 2068) in include/MathventageSIMD.h.
"""

SECTIONS_28_TO_46_DOCS = '''/* ========================================================================= */
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
 * @brief Computes high-precision double-precision sum across N elements using Neumaier\\'s compensated summation algorithm.
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
 * @brief Computes high-precision single-precision sum across N elements using Neumaier\\'s compensated summation algorithm.
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
 * @brief Evaluates an N-th degree polynomial via Horner\\'s synthetic division scheme: P(x) = c_0 + x*(c_1 + x*(c_2 + ...)).
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
 * @brief Evaluates an 8th-degree polynomial using Estrin\\'s parallel tree evaluation scheme.
 *
 * Reorganizes polynomial terms into a balanced binary tree of fused multiply-adds, breaking
 * the sequential dependency chain of Horner\\'s method and enabling instruction-level parallelism (ILP).
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
 * @brief Computes Euler\\'s Gamma function Gamma(x) using the Lanczos approximation with g = 7.
 *
 * Computes Gamma(x) for all real x except non-positive integers where poles exist.
 *
 * @param[in] x                Real argument.
 * @param[in] precision_target Optional precision descriptor (default: nullptr).
 * @return Value of Gamma(x).
 */
MSIMD_API double msimd_minimax_gamma(double x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Evaluates the Legendre polynomial P_n(x) of degree n at point x using Bonnet\\'s three-term recurrence relation.
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
 * @brief Brown\\'s fictitious play learning dynamics and two-player zero-sum game value evaluations.
 * @{
 */

/**
 * @brief Advances one iteration of George W. Brown\\'s fictitious play learning dynamical system for bimatrix games.
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
 * @brief Computes single-source shortest paths on a directed weighted graph using Dijkstra\\'s algorithm with min-heap priority queue.
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

/** @} */ /* End of MSIMD_Section46 */'''
