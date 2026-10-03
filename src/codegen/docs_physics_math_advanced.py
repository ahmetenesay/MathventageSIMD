# -*- coding: utf-8 -*-
"""
Module providing Doxygen replacements for physics, advanced math, statistics,
fuzzy logic, relativity, quantum mechanics, and TRNG (lines 1621 to 1805) in include/MathventageSIMD.h.
"""

PHYSICS_MATH_DOCS = '''/* ========================================================================= */
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
 * @brief Integrates separable Hamiltonian dynamics using Yoshida\\'s 4th-order symplectic integrator.
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
 * @brief Solves Kepler\\'s orbital equation M = E - e*sin(E) and determines Cartesian orbital coordinates.
 *
 * Uses Newton-Raphson iteration accelerated by Halley\\'s method to solve transcendental Kepler\\'s equation
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
 * @brief Computes 3D Cauchy stress and infinitesimal strain tensors under isotropic linear elasticity (Hooke\\'s Law).
 *
 * Evaluates linearized strain epsilon_{ij} = 0.5 * (grad_u_{ij} + grad_u_{ji}) and Cauchy stress
 * sigma_{ij} = lambda * tr(epsilon) * delta_{ij} + 2 * mu * epsilon_{ij}, where lambda and mu
 * are Lame\\'s first and second elastic parameters.
 *
 * @param[in]  grad_u           Displacement gradient tensor [3][3] = d u_i / d x_j.
 * @param[in]  lambda           Lame\\'s first parameter (Pa).
 * @param[in]  mu               Shear modulus / Lame\\'s second parameter (Pa).
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
 * @param[in]     lambda           Lame\\'s first parameter.
 * @param[in]     mu               Lame\\'s second parameter (shear modulus).
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
 * @param[in]  EI               Flexural rigidity (Young\\'s modulus E * area moment of inertia I).
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
 * @brief Computes Fresnel reflection and transmission amplitudes and Brewster\\'s angle for an optical dielectric interface.
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
 * @param[out] brewster_angle Pointer receiving Brewster\\'s polarization angle: atan(n2 / n1).
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
 * to a 4x4 homogeneous transformation matrix T in SE(3) using Rodrigues\\' formula.
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
 * @brief Fermi\\'s Golden Rule beta decay rates and Massive W/Z vector boson Proca-Yukawa potentials.
 * @{
 */

/**
 * @brief Computes nuclear beta-decay transition rate using Fermi\\'s Golden Rule with phase space integration.
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
#endif'''
