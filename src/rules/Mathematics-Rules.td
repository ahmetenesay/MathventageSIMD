// =============================================================================
// MathventageSIMD — Mathematics-Rules.td
// LLVM TableGen Rulebook for Pure Mathematical Specifications & Derivations
// -----------------------------------------------------------------------------
// STRICT PHILOSOPHY:
// 1. ZERO HARDWARE, ZERO SIMD, ZERO INTRINSICS, ZERO COMPILER FLAGS.
// 2. EXPLICIT DESCRIPTIVE NAMING: Every single waveform, soliton, wavelet,
//    and special function in the catalog carries an explicit, human-readable
//    DisplayName and a unique, semantically meaningful ExportName (msimd_wave_*).
//    NO ANONYMOUS GENERICS. Every derived function in the synthesized 300k LOC
//    C++ codebase is uniquely identifiable, readable, and self-documenting.
// =============================================================================

// =============================================================================
// SECTION 1: PURE MATHEMATICAL NUMBER FIELDS & ALGEBRAIC TYPES
// =============================================================================

class MathNumberField<string name, string symbol, bit is_ordered, bit is_complete, bit is_division_algebra> {
  string Name = name;
  string Symbol = symbol;
  bit IsOrdered = is_ordered;
  bit IsComplete = is_complete;
  bit IsDivisionAlgebra = is_division_algebra;
}

def Field_N : MathNumberField<"Natural",           "\\mathbb{N}", 1, 0, 0>;
def Field_Z : MathNumberField<"Integer",           "\\mathbb{Z}", 1, 0, 0>;
def Field_Q : MathNumberField<"Rational",          "\\mathbb{Q}", 1, 0, 1>;
def Field_R : MathNumberField<"Real",              "\\mathbb{R}", 1, 1, 1>;
def Field_C : MathNumberField<"Complex",           "\\mathbb{C}", 0, 1, 1>;
def Field_H : MathNumberField<"Quaternion",        "\\mathbb{H}", 0, 1, 1>;
def Field_O : MathNumberField<"Octonion",          "\\mathbb{O}", 0, 1, 0>;
def Field_S : MathNumberField<"Sedenion",          "\\mathbb{S}", 0, 1, 0>;

class MathDataType<string name, MathNumberField num_field, int dimension, string ctype> {
  string Name = name;
  MathNumberField Field = num_field;
  int Dimension = dimension; // Dimension over base field (e.g. C=2 over R, H=4 over R)
  string CType = ctype;
}

def MathReal32         : MathDataType<"Real32",         Field_R, 1,  "float">;
def MathReal64         : MathDataType<"Real64",         Field_R, 1,  "double">;
def MathInteger32      : MathDataType<"Integer32",      Field_Z, 1,  "int32_t">;
def MathInteger64      : MathDataType<"Integer64",      Field_Z, 1,  "int64_t">;
def MathComplex32      : MathDataType<"Complex32",      Field_C, 2,  "MSIMDComplex">;
def MathComplex64      : MathDataType<"Complex64",      Field_C, 2,  "MSIMDComplex64">;
def MathQuaternion32   : MathDataType<"Quaternion32",   Field_H, 4,  "float[4]">;
def MathQuaternion64   : MathDataType<"Quaternion64",   Field_H, 4,  "double[4]">;
def MathDualQuat32     : MathDataType<"DualQuat32",     Field_H, 8,  "float[8]">;
def MathDualQuat64     : MathDataType<"DualQuat64",     Field_H, 8,  "double[8]">;
def MathOctonion64     : MathDataType<"Octonion64",     Field_O, 8,  "double[8]">;

// =============================================================================
// SECTION 2: TENSOR GEOMETRY, MANIFOLD SPACES & CONSERVATION LAWS
// =============================================================================

class ManifoldSpace<string name, string symbol, int dimension, string metric_signature> {
  string Name = name;
  string Symbol = symbol;
  int Dimension = dimension;
  string MetricSignature = metric_signature;
}

def Space_Euclidean1D : ManifoldSpace<"Euclidean1D", "\\mathbb{E}^1",         1, "(+)">;
def Space_Euclidean2D : ManifoldSpace<"Euclidean2D", "\\mathbb{E}^2",         2, "(+,+)">;
def Space_Euclidean3D : ManifoldSpace<"Euclidean3D", "\\mathbb{E}^3",         3, "(+,+,+)">;
def Space_Minkowski4D : ManifoldSpace<"Minkowski4D", "\\mathbb{M}^{1,3}",     4, "(-,+,+,+)">;
def Space_PGA3D       : ManifoldSpace<"PGA3D",       "\\mathbb{R}_{3,0,1}^*", 4, "(+,+,+,0)">;

class ConservationLaw<string name, string preserved_quantity, string noether_symmetry> {
  string Name = name;
  string PreservedQuantity = preserved_quantity;
  string NoetherSymmetry = noether_symmetry;
}

def Law_EnergyConservation   : ConservationLaw<"Energy",   "Hamiltonian H",    "Time Translation Invariance">;
def Law_LinearMomentum       : ConservationLaw<"Momentum", "Total Momentum P", "Spatial Translation Invariance">;
def Law_AngularMomentum      : ConservationLaw<"AngularP", "Angular Momentum L","Spatial Rotation Invariance (SO(3))">;
def Law_Symplecticity        : ConservationLaw<"Symplectic","dp ^ dq 2-form",  "Symplectic Phase Space Volume (Liouville)">;
def Law_ProbabilityNorm      : ConservationLaw<"Unitary",  "<psi|psi> = 1",    "U(1) Global Phase Gauge Invariance">;
def Law_MassConservation     : ConservationLaw<"Continuity","int rho dV",      "Continuity Equation div(rho v) + d_t rho = 0">;

// =============================================================================
// SECTION 3: MATHEMATICAL CLASSIFICATION (MSC2020 DOMAINS)
// =============================================================================

class MathDomain<string msc_code, string name, string description> {
  string MSCCode = msc_code;
  string Name = name;
  string Description = description;
}

def Dom_LinearAlgebra      : MathDomain<"15A", "LinearAndMultilinearAlgebra", "Matrices, Tensors, Spectral Theory">;
def Dom_RealFunctions      : MathDomain<"26A", "RealFunctions",              "Derivatives, Integrals, Special Curves">;
def Dom_SpecialFunctions   : MathDomain<"33C", "SpecialFunctions",          "Orthogonal Polynomials, Bessel, Airy, Hypergeometric">;
def Dom_ODEs               : MathDomain<"34A", "OrdinaryDifferentialEq",    "Symplectic Integrators, Dynamical Systems, Flows">;
def Dom_PDEs               : MathDomain<"35L", "PartialDifferentialEq",     "Hyperbolic Wave Equations, Solitons, Dispersive Systems">;
def Dom_FourierAnalysis    : MathDomain<"42A", "HarmonicAnalysis",          "Fourier Series, Wavelets, Spectral Leakage">;
def Dom_NumericalAnalysis  : MathDomain<"65M", "NumericalAnalysis",         "Finite Difference, Stencils, Collocation, DEC">;
def Dom_QuantumTheory      : MathDomain<"81P", "QuantumTheory",             "Statevectors, Spinors, Unitary Dynamics, Hamiltonians">;
def Dom_Relativity         : MathDomain<"83C", "RelativityAndGravitation",  "Geodesics, Curvature, Spacetime Metrics, Christoffel">;
def Dom_ClassicalMechanics : MathDomain<"70H", "MechanicsOfParticles",      "Hamiltonian & Lagrangian Mechanics, XPBD, Multibody">;
def Dom_ContinuumMechanics : MathDomain<"74B", "MechanicsOfDeformableSolids","Elasticity, Stress-Strain, Lamé Parameters">;
def Dom_FluidMechanics     : MathDomain<"76D", "FluidMechanics",            "Navier-Stokes, SPH, Lattice Boltzmann BGK">;
def Dom_OpticsEM           : MathDomain<"78A", "OpticsAndElectromagnetics", "Maxwell Equations, Staggered Yee FDTD">;
def Dom_SignalTheory       : MathDomain<"94A", "InformationAndSignalTheory","Modulation, Filtering, Windows, Detection">;
def Dom_GeometricAlgebra   : MathDomain<"15A66","CliffordAndGeometricAlg",  "Exterior Calculus, Projective GA, Lie Groups">;
def Dom_NumberTheory       : MathDomain<"11A", "NumberTheoryAndArithmeticGeom", "Multi-precision integer pipelines, ADCX/ADOX, Bit Manipulation">;
def Dom_Combinatorics      : MathDomain<"05A", "CombinatoricsAndGraphTheory", "Discrete graphs, Morton order, Spatial Trees, Gather/Scatter">;
def Dom_Probability        : MathDomain<"60A", "ProbabilityAndStochastic", "RNG, normal/gamma distributions, Monte Carlo, entropy">;
def Dom_Cryptology         : MathDomain<"94A60", "CryptologyAndSecurity", "Galois Fields, AES-NI, GFNI, SHA, Hash lattices">;
def Dom_EmbeddedSoftFloat  : MathDomain<"00A", "EmbeddedLowPower", "Single-cycle integer ALU, Fixed-point Q15/Q31, Soft-float emu">;

// =============================================================================
// SECTION 4: BASE MATHEMATICAL WAVEFORM & OPERATOR SCHEMA
// =============================================================================

class WaveformRule<string name, int id, MathDomain domain, MathDataType dtype> {
  string Name = name;
  int RuleID = id;
  MathDomain Domain = domain;
  MathDataType DataType = dtype;

  // Explicit Semantic Identifiers for Clean C++ Code Generation & Export
  string ExportName = "msimd_wave_" # name;
  string DisplayName = name;
  string Description = "";

  // Pure mathematical formalization
  string AnalyticalFormula = "";
  string FourierSeries = "";
  string HarmonicDecay = "none";      // e.g. "1/n", "1/n^2", "exponential", "pure"
  string DiscontinuityType = "Smooth";// "Smooth", "Jump", "Corner", "Essential"
  string GoverningEquation = "";
  string DiscreteRecurrence = "";
  int StencilRadius = 0;
  int MathematicalOrder = 1;
  list<ConservationLaw> Invariants = [];
}

// =============================================================================
// PILLAR 1: HARMONIC & FUNDAMENTAL SYNTHESIS WAVEFORMS (EXPLICIT CATALOG)
// =============================================================================

class FundamentalWave<string name, int id, string export_fn, string display_title, string formula, string decay, string disc, string desc>
    : WaveformRule<name, id, Dom_FourierAnalysis, MathReal32> {
  let ExportName = export_fn;
  let DisplayName = display_title;
  let AnalyticalFormula = formula;
  let HarmonicDecay = decay;
  let DiscontinuityType = disc;
  let Description = desc;
}

// 1. Pure & Elementary Periodic Waveforms
def Wave_Sine : FundamentalWave<
    "harmonic_sine", 101, "msimd_wave_harmonic_sine",
    "Pure Sinusoidal Oscillation",
    "\\sin(t)", "Pure (no upper harmonics)", "Smooth",
    "Pure fundamental harmonic oscillator with zero spectral leakage.">;

def Wave_Cosine : FundamentalWave<
    "harmonic_cosine", 102, "msimd_wave_harmonic_cosine",
    "Pure Co-sinusoidal Oscillation",
    "\\cos(t)", "Pure (no upper harmonics)", "Smooth",
    "Orthogonal quadrature harmonic oscillator with phase shift pi/2.">;

def Wave_SymmetricSquare : FundamentalWave<
    "symmetric_square_duty50", 103, "msimd_wave_symmetric_square_duty50",
    "Symmetric 50% Duty Square Wave",
    "\\operatorname{sgn}(\\sin(t))", "1/n (-6 dB/oct, odd harmonics only)", "Jump",
    "Piecewise constant bipolar waveform with jump discontinuities at half periods.">;

def Wave_SymmetricTriangle : FundamentalWave<
    "symmetric_triangle", 104, "msimd_wave_symmetric_triangle",
    "Symmetric Triangle Wave",
    "\\frac{2}{\\pi}\\arcsin(\\sin(t))", "1/n^2 (-12 dB/oct, odd harmonics only)", "Corner",
    "Continuous piecewise linear wave with alternating slopes and sharp peak corners.">;

def Wave_SawtoothRampUp : FundamentalWave<
    "sawtooth_ramp_up", 105, "msimd_wave_sawtooth_ramp_up",
    "Ramp-Up Sawtooth Wave",
    "2(t - \\lfloor t + 0.5 \\rfloor)", "1/n (-6 dB/oct, all harmonics)", "Jump",
    "Linearly rising sawtooth profile with periodic instantaneous downward jump.">;

def Wave_SawtoothRampDown : FundamentalWave<
    "sawtooth_ramp_down", 106, "msimd_wave_sawtooth_ramp_down",
    "Ramp-Down Inverted Sawtooth Wave",
    "-2(t - \\lfloor t + 0.5 \\rfloor)", "1/n (-6 dB/oct, all harmonics)", "Jump",
    "Linearly descending sawtooth profile with periodic instantaneous upward jump.">;

def Wave_Trapezoidal : FundamentalWave<
    "trapezoidal_linear_transition", 107, "msimd_wave_trapezoidal_transition",
    "Trapezoidal Waveform with Finite Slew",
    "\\operatorname{clamp}(2\\operatorname{tri}(t), -1, 1)", "1/n^2", "Corner",
    "Slew-rate limited square wave with finite ramp transitions avoiding infinite slope.">;

def Wave_Staircase : FundamentalWave<
    "staircase_quantized_ramp", 108, "msimd_wave_staircase_quantized_ramp",
    "Uniform Staircase Quantized Wave",
    "\\frac{\\lfloor k t \\rfloor}{k}", "1/n", "Jump",
    "Monotonically stepping staircase wave with uniform discrete riser increments.">;

def Wave_DiracImpulseTrain : FundamentalWave<
    "dirac_impulse_train", 109, "msimd_wave_dirac_impulse_train",
    "Periodic Dirac Comb / Shah Distribution",
    "\\sum_{n=-\\infty}^\\infty \\delta(t - nT)", "Flat Spectrum (equal amplitude all n)", "Singular",
    "Idealized sampling distribution producing an infinite flat comb of harmonic frequencies.">;

// 2. Rounded & Band-Limited Emulated Waves
def Wave_RoundedSine : FundamentalWave<
    "rounded_sine_cubic_softened", 110, "msimd_wave_rounded_sine_cubic",
    "Cubic Softened Sine Wave",
    "\\sin(t) - \\frac{1}{6}\\sin^3(t)", "1/n^3", "Smooth",
    "Sine wave softened by 3rd-order harmonic suppression.">;

def Wave_RoundedSquare : FundamentalWave<
    "rounded_square_hyperbolic_tangent", 111, "msimd_wave_rounded_square_tanh",
    "Hyperbolic Tangent Rounded Square Wave",
    "\\tanh(\\beta \\sin(t))", "Exponential roll-off", "Smooth",
    "Infinitely differentiable smooth square wave emulated via tanh steepness parameter beta.">;

def Wave_RoundedTriangle : FundamentalWave<
    "rounded_triangle_log_cosh", 112, "msimd_wave_rounded_triangle_log_cosh",
    "Log-Cosh Rounded Triangle Wave",
    "\\frac{1}{\\beta}\\ln(\\cosh(\\beta \\operatorname{tri}(t)))", "Exponential roll-off", "Smooth",
    "C-infinity smooth triangle wave with rounded extremum crests and troughs.">;

// 3. Clipped & Saturated Waves
def Wave_SymmetricClippedSine : FundamentalWave<
    "clipped_sine_symmetric", 114, "msimd_wave_clipped_sine_symmetric",
    "Symmetrically Clipped Sine Wave",
    "\\operatorname{clamp}(\\sin(t), -\\theta, \\theta)", "1/n^2", "Corner",
    "Sine wave hard-clipped symmetrically at threshold theta.">;

def Wave_SoftClippedSine : FundamentalWave<
    "soft_clipped_sine_algebraic", 115, "msimd_wave_soft_clipped_sine_algebraic",
    "Algebraic Soft-Clipped Sine Wave",
    "\\frac{\\sin(t)}{\\sqrt{1 + \\sin^2(t)}}", "Exponential roll-off", "Smooth",
    "Smooth overdrive saturation wave with continuous algebraic asymptote.">;

// 4. Polarity Variants (Unipolar & Bipolar)
def Wave_UnipolarSquare : FundamentalWave<
    "unipolar_square_zero_to_one", 120, "msimd_wave_unipolar_square",
    "Unipolar [0, 1] Square Wave",
    "0.5(1 + \\operatorname{sgn}(\\sin(t)))", "1/n + DC", "Jump",
    "Digital logic clock profile shifting between ground (0) and Vdd (1).">;

def Wave_BipolarSquare : FundamentalWave<
    "bipolar_square_neg_one_to_one", 121, "msimd_wave_bipolar_square",
    "Bipolar [-1, 1] Square Wave",
    "\\operatorname{sgn}(\\sin(t))", "1/n", "Jump",
    "Zero-mean symmetric bipolar square wave switching between -1 and +1.">;

def Wave_UnipolarSawtooth : FundamentalWave<
    "unipolar_sawtooth_ramp", 122, "msimd_wave_unipolar_sawtooth",
    "Unipolar [0, 1] Sawtooth Wave",
    "t \\pmod 1", "1/n + DC", "Jump",
    "Ramp generator waveform rising from 0 to 1 with sharp reset to 0.">;

def Wave_BipolarSawtooth : FundamentalWave<
    "bipolar_sawtooth_ramp", 123, "msimd_wave_bipolar_sawtooth",
    "Bipolar [-1, 1] Sawtooth Wave",
    "2(t \\pmod 1) - 1", "1/n", "Jump",
    "Zero-mean bipolar sawtooth oscillating linearly across [-1, 1].">;

// 5. Multi-Oscillator Detuned Waveform Bundles
def Wave_Supersaw7 : FundamentalWave<
    "supersaw_7_oscillator_ensemble", 130, "msimd_wave_supersaw_7",
    "Roland JP-8000 Style 7-Oscillator Supersaw",
    "\\frac{1}{7}\\sum_{k=1}^7 \\operatorname{saw}(\\omega_k t + \\phi_k)", "Rich Multi-Saw Ensemble", "Jump",
    "Dense chorus-like waveform resulting from 7 linearly detuned sawtooth oscillators.">;

def Wave_Hypersaw9 : FundamentalWave<
    "hypersaw_9_oscillator_ensemble", 131, "msimd_wave_hypersaw_9",
    "Hypersaw 9-Oscillator Hyper-Ensemble",
    "\\frac{1}{9}\\sum_{k=1}^9 \\operatorname{saw}(\\omega_k t + \\phi_k)", "Hyper-Dense Multi-Saw", "Jump",
    "Ultra-dense trance waveform synthesized from 9 prime-ratio detuned sawtooth generators.">;

// 6. Explicit Parametric Pulse Waves (%1 to %99 -> 99 Distinct Waveforms)
class ExplicitPulseWave<int duty> : WaveformRule<
    "pulse_duty_" # duty # "_percent",
    !add(10000, duty),
    Dom_FourierAnalysis,
    MathReal32> {
  int DutyPercent = duty;
  let ExportName = "msimd_wave_pulse_duty_" # duty # "_pct";
  let DisplayName = "Pulse Wave Duty Cycle " # duty # "%";
  let Description = "Asymmetric rectangular pulse wave with exactly " # duty # "% high state and " # !sub(100, duty) # "% low state.";
  let AnalyticalFormula = "\\mathbf{1}_{[0, " # duty # "/100)}(t \\pmod 1) - \\mathbf{1}_{[" # duty # "/100, 1)}(t \\pmod 1)";
  let HarmonicDecay = "1/n";
  let DiscontinuityType = "Jump";
}

foreach D = 1-99 in {
  def Wave_Pulse_Duty_ # D : ExplicitPulseWave<D>;
}

// 7. Explicit Asymmetric Triangle Waves (Peak Shift %1 to %99 -> 98 Distinct Waveforms)
class ExplicitAsymTriangleWave<int peak> : WaveformRule<
    "asym_triangle_peak_" # peak # "_percent",
    !add(11000, peak),
    Dom_FourierAnalysis,
    MathReal32> {
  int PeakPercent = peak;
  let ExportName = "msimd_wave_asym_triangle_peak_" # peak # "_pct";
  let DisplayName = "Asymmetric Triangle Peak Shift " # peak # "%";
  let Description = "Asymmetric triangle wave with crest vertex shifted to " # peak # "% of the period.";
  let AnalyticalFormula = "\\begin{cases} \\frac{t}{d} & 0 \\le t < d \\\\ \\frac{1 - t}{1 - d} & d \\le t < 1 \\end{cases}, \\quad d = " # peak # "/100";
  let HarmonicDecay = "1/n^2";
  let DiscontinuityType = "Corner";
}

foreach P = {1-49, 51-99} in {
  def Wave_AsymTri_Peak_ # P : ExplicitAsymTriangleWave<P>;
}

// =============================================================================
// PILLAR 2: TRIGONOMETRIC, HYPERBOLIC & FRACTIONAL DERIVATIVE MANIFOLDS
// =============================================================================

class PureTrigWave<string name, int id, string export_fn, string display_title, string formula, string disc, string desc>
    : WaveformRule<name, id, Dom_RealFunctions, MathReal32> {
  let ExportName = export_fn;
  let DisplayName = display_title;
  let AnalyticalFormula = formula;
  let DiscontinuityType = disc;
  let Description = desc;
}

// 1. Classical Circular & Historical Trigonometric Waves
def Wave_Tan : PureTrigWave<
    "circular_tangent", 201, "msimd_wave_trig_tan",
    "Circular Tangent Wave", "\\tan(t)", "Vertical Asymptote",
    "Ratio of sine to cosine producing periodic vertical asymptotes at odd multiples of pi/2.">;

def Wave_Cot : PureTrigWave<
    "circular_cotangent", 202, "msimd_wave_trig_cot",
    "Circular Cotangent Wave", "\\cot(t)", "Vertical Asymptote",
    "Reciprocal of tangent producing periodic vertical asymptotes at multiples of pi.">;

def Wave_Sec : PureTrigWave<
    "circular_secant", 203, "msimd_wave_trig_sec",
    "Circular Secant Wave", "\\sec(t)", "Vertical Asymptote",
    "Reciprocal of cosine possessing local extrema at +1 and -1 and asymptotes at odd multiples of pi/2.">;

def Wave_Csc : PureTrigWave<
    "circular_cosecant", 204, "msimd_wave_trig_csc",
    "Circular Cosecant Wave", "\\csc(t)", "Vertical Asymptote",
    "Reciprocal of sine possessing local extrema at +1 and -1 and asymptotes at multiples of pi.">;

def Wave_Haversine : PureTrigWave<
    "haversine_formula", 205, "msimd_wave_trig_haversine",
    "Haversine Great-Circle Metric Wave", "\\frac{1 - \\cos(t)}{2}", "Smooth",
    "Half-versine wave essential in spherical navigational geodesic distance calculations.">;

// 2. Hyperbolic Waves
def Wave_Sinh : PureTrigWave<
    "hyperbolic_sine", 206, "msimd_wave_hyperbolic_sinh",
    "Hyperbolic Sine Wave", "\\sinh(t)", "Exponential Divergence",
    "Odd hyperbolic function representing catenary tension gradients and relativistic boost velocities.">;

def Wave_Cosh : PureTrigWave<
    "hyperbolic_cosine", 207, "msimd_wave_hyperbolic_cosh",
    "Hyperbolic Cosine (Catenary) Wave", "\\cosh(t)", "Exponential Divergence",
    "Even hyperbolic catenary curve modeling suspended elastic cables in uniform gravitational fields.">;

def Wave_Tanh : PureTrigWave<
    "hyperbolic_tangent", 208, "msimd_wave_hyperbolic_tanh",
    "Hyperbolic Tangent Topological Sigmoid", "\\tanh(t)", "Smooth Asymptotic Saturation",
    "Smooth sigmoid transition curve modeling domain walls, kinks, and neural activation profiles.">;

def Wave_Sech : PureTrigWave<
    "hyperbolic_secant", 209, "msimd_wave_hyperbolic_sech",
    "Hyperbolic Secant Soliton Profile", "\\operatorname{sech}(t)", "Smooth Exponential Decay",
    "Fundamental profile of optical solitons in Kerr nonlinear media and KdV shallow water waves.">;

// 3. Inverse Trigonometric Waves
def Wave_Arcsin : PureTrigWave<
    "inverse_sine_arcsin", 210, "msimd_wave_inv_arcsin",
    "Inverse Sine (Arcsin) Wave", "\\arcsin(t)", "Bounded [-1, 1]",
    "Multivalued branch-cut principal angle curve bounded strictly over the domain [-1, 1].">;

def Wave_Arctan : PureTrigWave<
    "inverse_tangent_arctan", 211, "msimd_wave_inv_arctan",
    "Inverse Tangent (Arctan) Wave", "\\arctan(t)", "Asymptotic [-pi/2, pi/2]",
    "Continuous bounded transition curve connecting -pi/2 to +pi/2 without singularities.">;

// 4. Memory-Kernel Fractional Calculus Waves
class ExplicitFractionalWave<string name, int id, string export_fn, string display_title, string order_str, string desc>
    : WaveformRule<name, id, Dom_RealFunctions, MathReal32> {
  string FractionalOrder = order_str;
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
  let GoverningEquation = "{}^C D_t^\\alpha f(t) = \\frac{1}{\\Gamma(1-\\alpha)} \\int_0^t (t-\\tau)^{-\\alpha} f'(\\tau) d\\tau";
}

def Wave_Sine_Caputo_Quarter : ExplicitFractionalWave<
    "sine_caputo_quarter_derivative", 220, "msimd_wave_frac_caputo_sine_025",
    "Caputo Fractional 1/4 Derivative of Sine", "0.25",
    "Quarter-order fractional derivative of sine exhibiting weak memory and pi/8 phase advance.">;

def Wave_Sine_Caputo_Half : ExplicitFractionalWave<
    "sine_caputo_half_derivative", 221, "msimd_wave_frac_caputo_sine_050",
    "Caputo Fractional Semi-Derivative of Sine", "0.50 (Semi-Derivative)",
    "Half-order fractional derivative exhibiting exact square-root memory diffusion and pi/4 phase advance.">;

def Wave_Sine_Caputo_ThreeQuarter : ExplicitFractionalWave<
    "sine_caputo_three_quarter_derivative", 222, "msimd_wave_frac_caputo_sine_075",
    "Caputo Fractional 3/4 Derivative of Sine", "0.75",
    "Three-quarter order fractional derivative interpolating between semi-derivative and full cosine.">;

// =============================================================================
// PILLAR 3: RECTIFIED, SPLINE & QUANTIZED WAVEFORMS
// =============================================================================

def Wave_FullRect_Sine : FundamentalWave<
    "full_wave_rectified_sine", 301, "msimd_wave_full_rect_sine",
    "Full-Wave Rectified Sine Wave", "|\\sin(t)|", "1/(4n^2 - 1) (even harmonics)", "Corner",
    "Absolute value folded sinusoidal wave doubling fundamental frequency and preserving only even harmonics.">;

def Wave_HalfRect_Sine : FundamentalWave<
    "half_wave_rectified_sine", 302, "msimd_wave_half_rect_sine",
    "Half-Wave Rectified Sine Wave", "\\max(0, \\sin(t))", "1/n + DC", "Corner",
    "Single-polarity diode rectified sinusoidal wave with positive pulses separated by zero intervals.">;

def Wave_FullRect_BesselJ0 : FundamentalWave<
    "full_wave_rectified_bessel_j0", 303, "msimd_wave_full_rect_bessel_j0",
    "Full-Wave Rectified Bessel J0 Wave", "|J_0(t)|", "Decaying even lobes", "Corner",
    "Rectified zero-order cylindrical wave modeling non-negative optical Airy disk intensity rings.">;

def Wave_Smoothstep : FundamentalWave<
    "smoothstep_hermite_interpolation", 304, "msimd_wave_spline_smoothstep",
    "C1 Smoothstep Hermite Interpolation Wave", "3t^2 - 2t^3", "Finite Polynomial", "Smooth",
    "Classic Hermite S-curve ensuring zero 1st derivatives at boundaries t=0 and t=1.">;

def Wave_Smootherstep : FundamentalWave<
    "smootherstep_quintic_interpolation", 305, "msimd_wave_spline_smootherstep",
    "C2 Smootherstep Quintic Interpolation Wave", "6t^5 - 15t^4 + 10t^3", "Finite Polynomial", "Smooth",
    "Ken Perlin's quintic S-curve ensuring zero 1st and 2nd derivatives at boundaries.">;

// =============================================================================
// PILLAR 4: TIME-VARYING, SÖNÜMLÜ & MODÜLASYONLU CHIRP SİSTEMLERİ
// =============================================================================

class ExplicitChirpWave<string name, int id, string export_fn, string display_title, string freq_law, string desc>
    : WaveformRule<name, id, Dom_SignalTheory, MathReal32> {
  string FrequencyLaw = freq_law;
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
}

def Wave_LinearUpChirp : ExplicitChirpWave<
    "linear_up_chirp", 401, "msimd_wave_chirp_linear_up",
    "Linear Frequency Up-Chirp", "f(t) = f_0 + k t",
    "Radar pulse compression waveform with instantaneous frequency linearly sweeping upwards.">;

def Wave_LinearDownChirp : ExplicitChirpWave<
    "linear_down_chirp", 402, "msimd_wave_chirp_linear_down",
    "Linear Frequency Down-Chirp", "f(t) = f_0 - k t",
    "Radar pulse compression waveform with instantaneous frequency linearly sweeping downwards.">;

def Wave_ExponentialChirp : ExplicitChirpWave<
    "exponential_geometric_chirp", 403, "msimd_wave_chirp_exponential",
    "Exponential (Geometric) Chirp", "f(t) = f_0 k^t",
    "Wideband acoustic measurement waveform sweeping across equal octaves in equal time intervals.">;

def Wave_GaussianChirplet : ExplicitChirpWave<
    "gaussian_chirplet_packet", 404, "msimd_wave_chirp_gaussian_chirplet",
    "Gaussian Windowed Chirplet Wavepacket", "e^{-\\alpha (t - t_0)^2} \\sin(2\\pi (f_0 t + 0.5 k t^2))",
    "Time-frequency localized wavepacket combining Gaussian envelope with linear quadratic phase chirp.">;

def Wave_ExpDampedSine : FundamentalWave<
    "exponentially_damped_sine", 405, "msimd_wave_damped_sine_exp",
    "Exponentially Damped Sine Oscillation", "e^{-\\gamma t} \\sin(\\omega t)", "Continuous Cauchy Lorentzian", "Smooth",
    "Harmonic oscillator response under viscous damping losing energy exponentially per radian cycle.">;

def Wave_GaussianDampedSine : FundamentalWave<
    "gaussian_damped_sine", 406, "msimd_wave_damped_sine_gaussian",
    "Gaussian Damped Sine Wavepacket", "e^{-\\alpha t^2} \\sin(\\omega t)", "Gaussian Envelope", "Smooth",
    "Minimum uncertainty wavepacket achieving the theoretical Heisenberg-Gabor time-frequency limit.">;

// =============================================================================
// PILLAR 5: İLERİ MATEMATİKSEL FONKSİYONLAR, BESSEL, SOLİTONLAR & HİDRODİNAMİK
// =============================================================================

// 1. Silindirik Bessel & Hankel Aileleri (n=0..10 -> Tam 88 Dalga Formu)
class ExplicitBesselWave<string kind_tag, string export_pfx, string kind_title, int order> : WaveformRule<
    "bessel_" # kind_tag # "_" # order,
    !add(13000, order),
    Dom_SpecialFunctions,
    MathReal64> {
  string BesselKind = kind_tag;
  int Order = order;
  let ExportName = "msimd_wave_bessel_" # export_pfx # "_" # order;
  let DisplayName = kind_title # " of Order " # order;
  let Description = "Cylindrical wave solution to Bessel's differential equation representing radial harmonic modes.";
  let GoverningEquation = "x^2 y'' + x y' + (x^2 - n^2) y = 0";
}

foreach N = 0-10 in {
  def Wave_Bessel_J_ # N   : ExplicitBesselWave<"first_kind_J", "j", "Bessel Function 1st Kind J", N>;
  def Wave_Bessel_Y_ # N   : ExplicitBesselWave<"second_kind_Y", "y", "Neumann Function 2nd Kind Y", N>;
  def Wave_Bessel_Mod_I_ # N : ExplicitBesselWave<"modified_I", "mod_i", "Modified Bessel 1st Kind I", N>;
  def Wave_Bessel_Mod_K_ # N : ExplicitBesselWave<"modified_K", "mod_k", "Modified Bessel 2nd Kind K", N>;
  def Wave_Hankel_H1_ # N  : ExplicitBesselWave<"hankel_1st_H1", "h1", "Hankel Function 1st Kind H1", N>;
  def Wave_Hankel_H2_ # N  : ExplicitBesselWave<"hankel_2nd_H2", "h2", "Hankel Function 2nd Kind H2", N>;
  def Wave_Bessel_Sph_j_ # N : ExplicitBesselWave<"spherical_j", "sph_j", "Spherical Bessel 1st Kind j", N>;
  def Wave_Bessel_Sph_y_ # N : ExplicitBesselWave<"spherical_y", "sph_y", "Spherical Neumann 2nd Kind y", N>;
}

// 2. Ortogonal Polinom Dalga Aileleri (d=1..8)
class ExplicitPolyWave<string family_tag, string export_pfx, string family_title, int degree> : WaveformRule<
    "poly_" # family_tag # "_deg" # degree,
    !add(14000, degree),
    Dom_SpecialFunctions,
    MathReal64> {
  string FamilyName = family_tag;
  int Degree = degree;
  let ExportName = "msimd_wave_poly_" # export_pfx # "_deg" # degree;
  let DisplayName = family_title # " of Degree " # degree;
  let Description = "Orthogonal polynomial wave satisfying three-term Sturm-Liouville recurrence relations.";
}

foreach D = 1-8 in {
  def Wave_Legendre_ # D    : ExplicitPolyWave<"legendre_P", "legendre_p", "Legendre Polynomial P", D>;
  def Wave_Hermite_ # D     : ExplicitPolyWave<"hermite_H", "hermite_h", "Hermite Polynomial H", D>;
  def Wave_ChebyshevT_ # D  : ExplicitPolyWave<"chebyshev_T", "chebyshev_t", "Chebyshev Polynomial 1st Kind T", D>;
  def Wave_ChebyshevU_ # D  : ExplicitPolyWave<"chebyshev_U", "chebyshev_u", "Chebyshev Polynomial 2nd Kind U", D>;
  def Wave_Laguerre_ # D    : ExplicitPolyWave<"laguerre_L", "laguerre_l", "Laguerre Polynomial L", D>;
  def Wave_Gegenbauer_ # D  : ExplicitPolyWave<"gegenbauer_C", "gegenbauer_c", "Gegenbauer Polynomial C", D>;
}

// 3. Jacobi Eliptik Dalgaları (12 Kanonik Form)
class ExplicitJacobiWave<string symbol, string export_sym, string display_title, string desc>
    : WaveformRule<"jacobi_elliptic_" # symbol, 501, Dom_SpecialFunctions, MathReal64> {
  string FunctionSymbol = symbol;
  let ExportName = "msimd_wave_jacobi_" # export_sym;
  let DisplayName = "Jacobi Elliptic " # display_title # " (" # symbol # ")";
  let Description = desc;
  let GoverningEquation = "\\left(\\frac{d}{du} \\mathrm{" # symbol # "}(u,m)\\right)^2 = P_4(\\mathrm{" # symbol # "})";
}

def Wave_Jacobi_SN : ExplicitJacobiWave<"sn", "sn", "Sine Amplitude", "Sinusoidal amplitude elliptic function sn(u,m).">;
def Wave_Jacobi_CN : ExplicitJacobiWave<"cn", "cn", "Cosine Amplitude", "Co-sinusoidal amplitude elliptic function cn(u,m).">;
def Wave_Jacobi_DN : ExplicitJacobiWave<"dn", "dn", "Delta Amplitude", "Delta amplitude elliptic function dn(u,m).">;
def Wave_Jacobi_CD : ExplicitJacobiWave<"cd", "cd", "Ratio cn/dn", "Elliptic quotient cn(u,m) / dn(u,m).">;
def Wave_Jacobi_SD : ExplicitJacobiWave<"sd", "sd", "Ratio sn/dn", "Elliptic quotient sn(u,m) / dn(u,m).">;
def Wave_Jacobi_ND : ExplicitJacobiWave<"nd", "nd", "Reciprocal 1/dn", "Reciprocal elliptic delta 1 / dn(u,m).">;
def Wave_Jacobi_DC : ExplicitJacobiWave<"dc", "dc", "Ratio dn/cn", "Elliptic quotient dn(u,m) / cn(u,m).">;
def Wave_Jacobi_NC : ExplicitJacobiWave<"nc", "nc", "Reciprocal 1/cn", "Reciprocal elliptic cosine 1 / cn(u,m).">;
def Wave_Jacobi_SC : ExplicitJacobiWave<"sc", "sc", "Ratio sn/cn", "Elliptic tangent quotient sn(u,m) / cn(u,m).">;
def Wave_Jacobi_NS : ExplicitJacobiWave<"ns", "ns", "Reciprocal 1/sn", "Reciprocal elliptic sine 1 / sn(u,m).">;
def Wave_Jacobi_DS : ExplicitJacobiWave<"ds", "ds", "Ratio dn/sn", "Elliptic quotient dn(u,m) / sn(u,m).">;
def Wave_Jacobi_CS : ExplicitJacobiWave<"cs", "cs", "Ratio cn/sn", "Elliptic cotangent quotient cn(u,m) / sn(u,m).">;

// 4. Doğrusal Olmayan Solitonlar & Okyanusal Canavar Dalgalar
class ExplicitSolitonWave<string name, int id, string export_fn, string display_title, string pde, string profile, string desc>
    : WaveformRule<name, id, Dom_PDEs, MathReal32> {
  let ExportName = export_fn;
  let DisplayName = display_title;
  let GoverningEquation = pde;
  let AnalyticalFormula = profile;
  let Description = desc;
  let Invariants = [Law_EnergyConservation, Law_LinearMomentum];
}

def Wave_KdV_BrightSoliton : ExplicitSolitonWave<
    "kdv_bright_soliton", 520, "msimd_soliton_kdv_bright",
    "Korteweg-de Vries (KdV) Bright Soliton",
    "u_t + 6 u u_x + u_{xxx} = 0",
    "\\frac{v}{2}\\operatorname{sech}^2\\left(\\frac{\\sqrt{v}}{2}(x - vt)\\right)",
    "Stable solitary elevation wave where nonlinear steepening exactly cancels third-order dispersion.">;

def Wave_NLS_BrightSoliton : ExplicitSolitonWave<
    "nls_bright_soliton", 521, "msimd_soliton_nls_bright",
    "Nonlinear Schrödinger (NLS) Bright Soliton",
    "i \\psi_t + \\frac{1}{2}\\psi_{xx} + |\\psi|^2 \\psi = 0",
    "\\eta \\operatorname{sech}(\\eta(x - vt)) e^{i(v x - \\omega t)}",
    "Fundamental envelope soliton propagating without dispersion in anomalous dispersion optical fiber.">;

def Wave_NLS_DarkSoliton : ExplicitSolitonWave<
    "nls_dark_soliton", 522, "msimd_soliton_nls_dark",
    "Nonlinear Schrödinger (NLS) Dark Soliton",
    "i \\psi_t + \\frac{1}{2}\\psi_{xx} - |\\psi|^2 \\psi = 0",
    "\\psi_0 \\tanh(\\eta(x - vt)) e^{i(v x - \\omega t)}",
    "Localized intensity dip (black hole analog) propagating stably in normal dispersion nonlinear media.">;

def Wave_Peregrine_RogueSoliton : ExplicitSolitonWave<
    "peregrine_rogue_wave", 523, "msimd_soliton_peregrine_rogue",
    "Peregrine Rogue Wave (Spatio-Temporal Singularity)",
    "NLSE Rational Spatio-Temporal Singularity",
    "\\left(1 - \\frac{4(1 + 2i t)}{1 + 4x^2 + 4t^2}\\right) e^{it}",
    "Prototype of extreme oceanic freak waves appearing suddenly from nowhere and vanishing without trace.">;

def Wave_AkhmedievBreather : ExplicitSolitonWave<
    "akhmediev_breather", 524, "msimd_soliton_akhmediev_breather",
    "Akhmediev Breather Soliton",
    "NLSE Spatially Periodic Breather",
    "Periodic modulation wave in space growing and decaying symmetrically in time.",
    "Exact breather solution modeling the Fermi-Pasta-Ulam recurrence and modulational instability onset.">;

def Wave_SineGordon_Kink : ExplicitSolitonWave<
    "sine_gordon_topological_kink", 525, "msimd_soliton_sine_gordon_kink",
    "Sine-Gordon Relativistic Topological Kink",
    "u_{tt} - u_{xx} + \\sin(u) = 0",
    "4\\arctan\\left(e^{(x - vt)/\\sqrt{1 - v^2}}\\right)",
    "Lorentz-invariant topological twist interpolating between vacuum states 0 and 2pi carrying topological charge.">;

// 5. Hidrodinamik & Okyanusal Dalga Dinamikleri
class ExplicitHydroWave<string name, int id, string export_fn, string display_title, string dispersion, string desc>
    : WaveformRule<name, id, Dom_FluidMechanics, MathReal32> {
  string DispersionRelation = dispersion;
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
  let Invariants = [Law_MassConservation, Law_EnergyConservation];
}

def Wave_GerstnerTrochoidal : ExplicitHydroWave<
    "gerstner_trochoidal_ocean_wave", 530, "msimd_hydro_gerstner_trochoidal",
    "Gerstner Trochoidal Surface Gravity Wave",
    "\\omega^2 = g k",
    "Exact nonlinear solution for deep water ocean waves where fluid particles trace circular orbits forming sharp cusps.">;

def Wave_Stokes_5thOrder : ExplicitHydroWave<
    "stokes_5th_order_wave", 531, "msimd_hydro_stokes_5th_order",
    "Stokes 5th-Order Perturbation Ocean Wave",
    "\\eta(x,t) = \\sum_{n=1}^5 A_n \\cos(n(kx - \\omega t))",
    "High-order perturbation theory wave profile showing steep crests and broad flat troughs.">;

def Wave_CnoidalWave : ExplicitHydroWave<
    "cnoidal_shallow_water_wave", 532, "msimd_hydro_cnoidal_shallow",
    "Cnoidal Periodic Shallow Water Wave",
    "\\eta(x) = \\eta_2 + H \\operatorname{cn}^2\\left(2 K(m) \\frac{x}{\\lambda}, m\\right)",
    "Exact periodic solution to the Korteweg-de Vries equation describing ocean swell in shallow coastal regions.">;

def Wave_KelvinEquatorial : ExplicitHydroWave<
    "kelvin_equatorial_oceanic_wave", 533, "msimd_hydro_kelvin_equatorial",
    "Equatorial Kelvin Wave (El Niño Mechanism)",
    "v = 0, \\quad \\omega = c k_x",
    "Non-dispersive equatorially trapped eastward-propagating ocean wave acting as the driver of ENSO cycles.">;

def Wave_RossbyPlanetary : ExplicitHydroWave<
    "rossby_planetary_atmospheric_wave", 534, "msimd_hydro_rossby_planetary",
    "Rossby Planetary Wave (Coriolis Beta Effect)",
    "\\omega = -\\frac{\\beta k_x}{k_x^2 + k_y^2 + 1/R_d^2}",
    "Giant meanders in high-altitude jet streams and ocean currents driven by the meridional variation of Coriolis force.">;

// =============================================================================
// PILLAR 6: DISCRETE WAVELETS, WINDOWING FUNCTIONS & CHAOTIC ATTRACTORS
// =============================================================================

// 1. Daubechies Dalgacıkları (db1 to db44 -> Tam 44 Ortogonal Form!)
class ExplicitDaubechiesWavelet<int index> : WaveformRule<
    "wavelet_daubechies_db_" # index,
    !add(15000, index),
    Dom_FourierAnalysis,
    MathReal64> {
  int VanishingMoments = index;
  int FilterLength = !mul(index, 2);
  let ExportName = "msimd_wavelet_daubechies_db" # index;
  let DisplayName = "Daubechies Wavelet db" # index # " (N=" # index # ")";
  let Description = "Maximal phase orthogonal compact wavelet with " # index # " vanishing moments and " # !mul(index, 2) # " filter taps.";
  let AnalyticalFormula = "\\phi(t) = \\sqrt{2}\\sum_{k=0}^{2N-1} h_k \\phi(2t - k)";
  let Invariants = [Law_EnergyConservation];
}

foreach W = 1-44 in {
  def Wavelet_Daubechies_ # W : ExplicitDaubechiesWavelet<W>;
}

// 2. Symlets (sym2 to sym20 -> Tam 19 Neredeyse Simetrik Form!)
class ExplicitSymletWavelet<int index> : WaveformRule<
    "wavelet_symlet_sym_" # index,
    !add(16000, index),
    Dom_FourierAnalysis,
    MathReal64> {
  int VanishingMoments = index;
  let ExportName = "msimd_wavelet_symlet_sym" # index;
  let DisplayName = "Symlet Wavelet sym" # index;
  let Description = "Least asymmetric modification of Daubechies wavelets minimizing linear phase distortion.";
}

foreach S = 2-20 in {
  def Wavelet_Symlet_ # S : ExplicitSymletWavelet<S>;
}

// 3. Coiflets (coif1 to coif5 -> Tam 5 Form!)
class ExplicitCoifletWavelet<int index> : WaveformRule<
    "wavelet_coiflet_coif_" # index,
    !add(17000, index),
    Dom_FourierAnalysis,
    MathReal64> {
  int Index = index;
  let ExportName = "msimd_wavelet_coiflet_coif" # index;
  let DisplayName = "Coiflet Wavelet coif" # index;
  let Description = "Wavelet with vanishing moments for both scaling and wavelet functions designed for numerical PDE schemes.";
}

foreach C = 1-5 in {
  def Wavelet_Coiflet_ # C : ExplicitCoifletWavelet<C>;
}

// 4. Spektral Pencereleme Fonksiyonları
class ExplicitWindow<string name, int id, string export_fn, string display_title, string formula, string sidelobe_db, string desc>
    : WaveformRule<name, id, Dom_SignalTheory, MathReal32> {
  string SidelobeSuppression = sidelobe_db;
  let ExportName = export_fn;
  let DisplayName = display_title;
  let AnalyticalFormula = formula;
  let Description = desc;
}

def Window_Hann : ExplicitWindow<
    "window_hann_raised_cosine", 601, "msimd_window_hann",
    "Hann (Raised Cosine) Window",
    "0.5 - 0.5\\cos\\left(\\frac{2\\pi n}{N}\\right)", "-31.5 dB",
    "Classic raised cosine window with smooth zero endpoints and -18 dB/octave sidelobe decay rate.">;

def Window_Hamming : ExplicitWindow<
    "window_hamming_equiripple", 602, "msimd_window_hamming",
    "Hamming Optimal Sidelobe Window",
    "0.54 - 0.46\\cos\\left(\\frac{2\\pi n}{N}\\right)", "-42.7 dB",
    "Optimized raised cosine window canceling the first sidelobe peak at the cost of slower asymptotic decay.">;

def Window_BlackmanHarris4 : ExplicitWindow<
    "window_blackman_harris_4term", 603, "msimd_window_blackman_harris_4term",
    "4-Term Blackman-Harris High-Dynamic Window",
    "a_0 - a_1\\cos + a_2\\cos - a_3\\cos", "-92.0 dB",
    "Precision spectral analysis window providing extreme 92 dB dynamic range sidelobe suppression.">;

def Window_FlatTop : ExplicitWindow<
    "window_flat_top_amplitude_calibration", 604, "msimd_window_flat_top",
    "Flat-Top Calibration Window",
    "\\sum_{k=0}^4 a_k \\cos\\left(\\frac{2\\pi k n}{N}\\right)", "-93.0 dB",
    "Zero scallop loss window designed for high-accuracy harmonic amplitude calibration in spectral analyzers.">;

// 5. Deterministik Kaotik Çekiciler (Chaotic Attractors)
class ExplicitAttractor<string name, int id, string export_fn, string display_title, string odes, string desc>
    : WaveformRule<name, id, Dom_ODEs, MathReal32> {
  string DynamicEquations = odes;
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
}

def Attractor_LorenzX : ExplicitAttractor<
    "lorenz_chaotic_attractor_x", 620, "msimd_attractor_lorenz_x",
    "Lorenz Butterfly Attractor X-Component",
    "\\dot{x} = \\sigma(y - x)",
    "Convective overturning coordinate displaying deterministic chaotic butterfly wing trajectories.">;

def Attractor_VanDerPol : ExplicitAttractor<
    "van_der_pol_relaxation_oscillator", 621, "msimd_attractor_van_der_pol",
    "Van der Pol Relaxation Oscillator",
    "\\ddot{x} - \\mu (1 - x^2) \\dot{x} + x = 0",
    "Nonlinear limit-cycle system modeling heartbeat pacing, vacuum tube oscillation, and relaxation jumping.">;

def Attractor_MackeyGlass : ExplicitAttractor<
    "mackey_glass_delay_differential", 622, "msimd_attractor_mackey_glass",
    "Mackey-Glass Delayed Physiological Attractor",
    "\\dot{x}(t) = \\frac{\\beta x(t - \\tau)}{1 + x(t-\\tau)^n} - \\gamma x(t)",
    "Infinite-dimensional chaotic delay differential equation modeling physiological white blood cell regeneration.">;

// 6. Sayısal CAZAC ve Radar Kod Dizileri
class ExplicitDigitalSequence<string name, int id, string export_fn, string display_title, string property, string desc>
    : WaveformRule<name, id, Dom_SignalTheory, MathComplex32> {
  string SequenceProperty = property;
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
}

def Seq_ZadoffChu_CAZAC : ExplicitDigitalSequence<
    "zadoff_chu_cazac_sequence", 640, "msimd_sequence_zadoff_chu_cazac",
    "Zadoff-Chu Polyphase CAZAC Sequence (LTE/5G PSS)",
    "Constant Amplitude Zero Autocorrelation (CAZAC)",
    "Optimal radar and mobile primary synchronization signal possessing strictly zero cyclic autocorrelation at non-zero shifts.">;

def Seq_BarkerCode13 : ExplicitDigitalSequence<
    "barker_13_pulse_compression_code", 641, "msimd_sequence_barker_13",
    "Barker-13 Optimal Biphase Radar Code",
    "Sidelobe levels strictly <= 1/13",
    "Biphase radar modulation sequence achieving the maximum possible compression peak-to-sidelobe ratio.">;

def Seq_GoldCode : ExplicitDigitalSequence<
    "gold_pseudo_random_cdma_sequence", 642, "msimd_sequence_gold_cdma",
    "Gold Sequence for Asynchronous CDMA",
    "Bounded three-valued cross-correlation spectrum",
    "Pseudo-random binary sequence generated from preferred pairs of m-sequences used in GPS and UMTS telecommunications.">;

// =============================================================================
// PILLAR 4: TRANSCENDENTAL MINIMAX & HORNER POLYNOMIAL APPROXIMATIONS
// =============================================================================

class PolynomialApproximationRule<string name, int id, string export_fn, string display_title,
                                  string formula, string interval, string max_err, string desc>
    : WaveformRule<name, id, Dom_RealFunctions, MathReal32> {
  string MinimaxInterval = interval;
  string MaxAbsoluteError = max_err;
  let ExportName = export_fn;
  let DisplayName = display_title;
  let AnalyticalFormula = formula;
  let Description = desc;
}

def Poly_FastSin_F32 : PolynomialApproximationRule<
    "fast_sin_f32", 701, "msimd_poly_fast_sin_f32",
    "Chebyshev-Horner Fast Vector Sine",
    "x * (1.0 - 0.16666654*x^2 + 0.00833216*x^4 - 0.00019515*x^6)",
    "[-pi/2, pi/2]", "2.3e-7",
    "Single-precision 7th-order minimax polynomial evaluating sin(x) with 2 ULP precision.">;

def Poly_FastCos_F32 : PolynomialApproximationRule<
    "fast_cos_f32", 702, "msimd_poly_fast_cos_f32",
    "Chebyshev-Horner Fast Vector Cosine",
    "1.0 - 0.49999999*x^2 + 0.04166663*x^4 - 0.00138883*x^6",
    "[-pi/2, pi/2]", "2.1e-7",
    "Single-precision 6th-order minimax polynomial evaluating cos(x) with zero BPU branching.">;

def Poly_FastExp_F32 : PolynomialApproximationRule<
    "fast_exp_f32", 703, "msimd_poly_fast_exp_f32",
    "Range-Reduced Fast Vector Exponential",
    "2^k * P(r), r = x - k*ln(2)", "[-87.3, 88.7]", "1.8e-7",
    "Range-reduced exponential approximation utilizing integer bitshift for 2^k and 5th-order Horner polynomial for residual.">;

def Poly_FastLn_F32 : PolynomialApproximationRule<
    "fast_ln_f32", 704, "msimd_poly_fast_ln_f32",
    "Fast Vector Natural Logarithm",
    "k*ln(2) + 2*s * P(s^2), s = (m-1)/(m+1)", "[1e-37, 1e38]", "3.2e-7",
    "Vectorized natural logarithm splitting IEEE-754 exponent and mantissa with 5th-order Padé approximant.">;

def Poly_FastInvSqrt_F32 : PolynomialApproximationRule<
    "fast_inv_sqrt_f32", 705, "msimd_poly_fast_inv_sqrt_f32",
    "Newton-Raphson Fast Reciprocal Square Root",
    "x * (1.5 - 0.5 * a * x^2)", "(0, +inf)", "1.0e-7",
    "Initial hardware seed (rsqrtps) refined via quadratic Newton-Raphson step to full single-precision accuracy.">;

def Poly_FastTanh_F32 : PolynomialApproximationRule<
    "fast_tanh_f32", 706, "msimd_poly_fast_tanh_f32",
    "Padé Rational Fast Hyperbolic Tangent",
    "x * (105 + 10*x^2) / (105 + 45*x^2 + x^4)", "[-4.0, 4.0]", "4.5e-7",
    "Branchless rational Padé [3/2] approximation for hyperbolic tangent neural activations.">;

def Poly_FastSigmoid_F32 : PolynomialApproximationRule<
    "fast_sigmoid_f32", 707, "msimd_poly_fast_sigmoid_f32",
    "Fast Logistic Sigmoid Activation",
    "0.5 + 0.5 * tanh(0.5 * x)", "[-8.0, 8.0]", "5.0e-7",
    "Branchless vectorized logistic activation function derived from fast tanh.">;

// =============================================================================
// PILLAR 5: DISCRETE DIFFERENTIAL STENCILS & WAVE EQUATION OPERATORS
// =============================================================================

class DifferentialStencilRule<string name, int id, string export_fn, string display_title,
                              int dim, int radius, int accuracy, string pde_name, string desc>
    : WaveformRule<name, id, Dom_NumericalAnalysis, MathReal32> {
  int Dimension = dim;
  let StencilRadius = radius;
  let MathematicalOrder = accuracy;
  string GoverningPDE = pde_name;
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
}

def Stencil_Laplacian1D_3pt : DifferentialStencilRule<
    "stencil_laplacian_1d_3pt", 720, "msimd_stencil_laplacian_1d_3pt",
    "1D Standard 3-Point Laplacian Stencil",
    1, 1, 2, "Heat / Wave 1D",
    "(u[i-1] - 2*u[i] + u[i+1]) / dx^2 second-order accurate central difference.">;

def Stencil_Laplacian2D_5pt : DifferentialStencilRule<
    "stencil_laplacian_2d_5pt", 721, "msimd_stencil_laplacian_2d_5pt",
    "2D 5-Point Cross Laplacian Stencil",
    2, 1, 2, "Poisson / Membrane 2D",
    "Central 5-point difference stencil on uniform Cartesian 2D grid.">;

def Stencil_Laplacian2D_9pt : DifferentialStencilRule<
    "stencil_laplacian_2d_9pt", 722, "msimd_stencil_laplacian_2d_9pt",
    "2D 9-Point Isotropic Compact Laplacian Stencil",
    2, 1, 4, "Helmholtz 2D",
    "Fourth-order compact Mehrstellen isotropic stencil eliminating diagonal grid anisotropy.">;

def Stencil_Laplacian3D_7pt : DifferentialStencilRule<
    "stencil_laplacian_3d_7pt", 723, "msimd_stencil_laplacian_3d_7pt",
    "3D 7-Point Star Laplacian Stencil",
    3, 1, 2, "Acoustic Wave / Gravitational Potential 3D",
    "Standard 7-point orthogonal cross stencil for 3D scalar field diffusion and wave propagation.">;

def Stencil_Laplacian3D_27pt : DifferentialStencilRule<
    "stencil_laplacian_3d_27pt", 724, "msimd_stencil_laplacian_3d_27pt",
    "3D 27-Point Isotropic Cube Laplacian Stencil",
    3, 1, 4, "High-Order Acoustic / Seismic Wave",
    "Isotropic 27-point stencil minimizing grid dispersion in high-frequency wave propagation.">;

def Stencil_Biharmonic3D_13pt : DifferentialStencilRule<
    "stencil_biharmonic_3d_13pt", 725, "msimd_stencil_biharmonic_3d_13pt",
    "3D 13-Point Biharmonic Operator (nabla^4)",
    3, 2, 2, "Plate Bending / Stokes Creeping Flow",
    "Fourth-order biharmonic spatial differential operator modeling thin-plate flexure and elasticity.">;

def Stencil_Yee_FDTD_3D : DifferentialStencilRule<
    "stencil_yee_fdtd_curl_3d", 726, "msimd_stencil_yee_fdtd_3d",
    "Yee Staggered Electromagnetic FDTD Stencil",
    3, 1, 2, "Maxwell Curl Equations",
    "Staggered grid Yee cell evaluating curl(E) and curl(H) with rigorous symplectic conservation of charge.">;

// =============================================================================
// PILLAR 6: HIGH-DIMENSIONAL LINEAR ALGEBRA & ROTOR GEOMETRY
// =============================================================================

class LinearAlgebraStencil<string name, int id, string export_fn, string display_title,
                           string category, int dim, string desc>
    : WaveformRule<name, id, Dom_LinearAlgebra, MathReal32> {
  string Category = category;
  int MatrixDimension = dim;
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
}

def LA_Mat4_Mul_F32 : LinearAlgebraStencil<
    "la_mat4_mul_f32", 740, "msimd_la_mat4_mul_f32",
    "4x4 Matrix Multiplication FMA Stencil",
    "MatrixMultiplication", 4,
    "Vectorized 4x4 affine transform matrix product unrolled into 16 FMA broadcast lanes.">;

def LA_Mat4_Inv_F32 : LinearAlgebraStencil<
    "la_mat4_inv_f32", 741, "msimd_la_mat4_inv_f32",
    "4x4 Matrix Inversion Cramer SIMD",
    "MatrixInversion", 4,
    "Branchless 4x4 matrix inversion using SIMD sub-determinant Cramer evaluation.">;

def LA_Vec3_Cross_F32 : LinearAlgebraStencil<
    "la_vec3_cross_f32", 742, "msimd_la_vec3_cross_f32",
    "3D Vector Cross Product Stencil",
    "VectorAlgebra", 3,
    "Two-instruction permutation cross product (a x b) via shuffle and fused multiply-subtract.">;

def LA_Vec4_Dot_F32 : LinearAlgebraStencil<
    "la_vec4_dot_f32", 743, "msimd_la_vec4_dot_f32",
    "4D Vector Dot Product Stencil",
    "VectorAlgebra", 4,
    "Single-cycle dot product accumulating 4 elements into a scalar lane.">;

def LA_Quat_Hamilton_F32 : LinearAlgebraStencil<
    "la_quat_hamilton_f32", 744, "msimd_la_quat_hamilton_f32",
    "Quaternion Hamilton Product Stencil",
    "QuaternionAlgebra", 4,
    "Fused 4D hypercomplex quaternion product maintaining SO(3) 3D rotation group structure.">;

def LA_Quat_SLERP_F32 : LinearAlgebraStencil<
    "la_quat_slerp_f32", 745, "msimd_la_quat_slerp_f32",
    "Spherical Linear Quaternion Interpolation (SLERP)",
    "QuaternionInterpolation", 4,
    "Constant-velocity geodesic interpolation on the unit 3-sphere S^3.">;

// =============================================================================
// PILLAR 7: SYMPLECTIC & GEOMETRIC NUMERICAL INTEGRATORS
// =============================================================================

class SymplecticIntegratorRule<string name, int id, string export_fn, string display_title,
                               int order, int stages, string symm, string desc>
    : WaveformRule<name, id, Dom_ODEs, MathReal64> {
  int IntegratorOrder = order;
  int StageCount = stages;
  string SymmetryType = symm;
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
}

def Integrator_VelocityVerlet : SymplecticIntegratorRule<
    "integrator_velocity_verlet", 760, "msimd_integrator_velocity_verlet",
    "2nd-Order Symplectic Velocity Verlet",
    2, 2, "Time-Reversible",
    "Standard phase space volume preserving integrator for molecular dynamics and orbital mechanics.">;

def Integrator_Ruth4 : SymplecticIntegratorRule<
    "integrator_ruth4_symplectic", 761, "msimd_integrator_ruth4",
    "4th-Order 3-Stage Ruth Symplectic Integrator",
    4, 3, "Symplectic",
    "Ronald Ruth 4th-order partitioned Runge-Kutta conserving energy invariants without numerical drift.">;

def Integrator_Yoshida6 : SymplecticIntegratorRule<
    "integrator_yoshida6_symplectic", 762, "msimd_integrator_yoshida6",
    "6th-Order 7-Stage Haruo Yoshida Symplectic Integrator",
    6, 7, "Symmetric-Symplectic",
    "High-precision composition integrator preserving Poincaré 2-form invariants across billions of cycles.">;

def Integrator_RK4_Classical : SymplecticIntegratorRule<
    "integrator_rk4_classical", 763, "msimd_integrator_rk4",
    "4th-Order Classical Runge-Kutta (RK4)",
    4, 4, "Non-Symplectic",
    "Universal four-stage predictor-corrector integrator for general non-conservative dynamical systems.">;
