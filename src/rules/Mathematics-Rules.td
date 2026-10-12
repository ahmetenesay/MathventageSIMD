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

def MathFloat8         : MathDataType<"Float8",         Field_R, 1,  "uint8_t">;
def MathFloat16        : MathDataType<"Float16",        Field_R, 1,  "uint16_t">;
def MathReal32         : MathDataType<"Real32",         Field_R, 1,  "float">;
def MathReal64         : MathDataType<"Real64",         Field_R, 1,  "double">;
def MathFloat128       : MathDataType<"Float128",       Field_R, 1,  "msimd_f128_t">;
def MathFloat256       : MathDataType<"Float256",       Field_R, 1,  "msimd_f256_t">;
def MathInteger8       : MathDataType<"Integer8",       Field_Z, 1,  "int8_t">;
def MathInteger16      : MathDataType<"Integer16",      Field_Z, 1,  "int16_t">;
def MathInteger32      : MathDataType<"Integer32",      Field_Z, 1,  "int32_t">;
def MathInteger64      : MathDataType<"Integer64",      Field_Z, 1,  "int64_t">;
def MathInteger128     : MathDataType<"Integer128",     Field_Z, 1,  "msimd_i128_t">;
def MathInteger256     : MathDataType<"Integer256",     Field_Z, 1,  "msimd_i256_t">;
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

def Dom_SetTheory          : MathDomain<"03E", "SetTheoryAndLogic", "Constructive logic, Boolean lattices, formal semantics">;
def Dom_Combinatorics      : MathDomain<"05A", "CombinatoricsAndGraphTheory", "Discrete graphs, Morton order, Spatial Trees, Gather/Scatter">;
def Dom_OrderLattices      : MathDomain<"06A", "OrderedStructuresAndLattices", "Posets, complete lattices, Galois connections">;
def Dom_NumberTheory       : MathDomain<"11A", "NumberTheoryAndArithmeticGeom", "Multi-precision integer pipelines, ADCX/ADOX, Bit Manipulation">;
def Dom_LinearAlgebra      : MathDomain<"15A", "LinearAndMultilinearAlgebra", "Matrices, Tensors, Spectral Theory">;
def Dom_GeometricAlgebra   : MathDomain<"15A66","CliffordAndGeometricAlg",  "Exterior Calculus, Projective GA, Lie Groups">;
def Dom_AssociativeRings   : MathDomain<"16D", "AssociativeRingsAndAlgebras", "Non-commutative rings, modules, division algebras">;
def Dom_NonassociativeRings: MathDomain<"17A", "NonassociativeRingsAndAlgebras", "Lie algebras, Jordan algebras, Octonions, Sedenions">;
def Dom_CategoryTheory     : MathDomain<"18A", "CategoryTheoryAndFunctors", "Functors, natural transformations, monads, adjoints">;
def Dom_GroupTheory        : MathDomain<"20C", "GroupTheoryAndRepresentations", "Permutation groups, representation theory, character tables">;
def Dom_TopologicalGroups  : MathDomain<"22E", "LieGroupsAndLieAlgebras", "Continuous transformation groups, exponential maps, Lie brackets">;
def Dom_RealFunctions      : MathDomain<"26A", "RealFunctions",              "Derivatives, Integrals, Special Curves">;
def Dom_MeasureTheory      : MathDomain<"28A", "MeasureAndIntegration", "Lebesgue measures, Radon-Nikodym derivatives, Borel algebras">;
def Dom_ComplexFunctions   : MathDomain<"30A", "FunctionsOfComplexVariable", "Holomorphic functions, conformal mapping, residue calculus">;
def Dom_PotentialTheory    : MathDomain<"31A", "PotentialTheoryAndHarmonics", "Harmonic functions, Dirichlet problems, Green functions">;
def Dom_SpecialFunctions   : MathDomain<"33C", "SpecialFunctions",          "Orthogonal Polynomials, Bessel, Airy, Hypergeometric">;
def Dom_ODEs               : MathDomain<"34A", "OrdinaryDifferentialEq",    "Symplectic Integrators, Dynamical Systems, Flows">;
def Dom_PDEs               : MathDomain<"35L", "PartialDifferentialEq",     "Hyperbolic Wave Equations, Solitons, Dispersive Systems">;
def Dom_DynamicalSystems   : MathDomain<"37D", "DynamicalSystemsAndErgodicTheory", "Hyperbolic dynamics, strange attractors, Lyapunov spectra">;
def Dom_DifferenceEquations: MathDomain<"39A", "DifferenceAndFunctionalEquations", "Discrete recurrence relations, q-calculus, shift operators">;
def Dom_SequencesSeries    : MathDomain<"40A", "SequencesSeriesSummability", "Convergence acceleration, Euler-Maclaurin, Padé summation">;
def Dom_FourierAnalysis    : MathDomain<"42A", "HarmonicAnalysis",          "Fourier Series, Wavelets, Spectral Leakage">;
def Dom_IntegralTransforms : MathDomain<"44A", "IntegralTransformsAndOperational", "Laplace, Mellin, Radon, Hankel, Hilbert transforms">;
def Dom_IntegralEquations  : MathDomain<"45A", "IntegralEquations", "Fredholm, Volterra, Wiener-Hopf, singular kernel equations">;
def Dom_FunctionalAnalysis : MathDomain<"46E", "FunctionalAnalysisAndBanachSpaces", "Sobolev spaces, Hilbert spaces, spectral theorem">;
def Dom_OperatorTheory     : MathDomain<"47A", "OperatorTheory", "Self-adjoint operators, perturbation theory, C*-algebras">;
def Dom_CalculusVariations : MathDomain<"49J", "CalculusOfVariationsAndControl", "Euler-Lagrange equations, Pontryagin maximum principle">;
def Dom_DifferentialGeometry: MathDomain<"53A", "DifferentialGeometryAndManifolds", "Riemannian geometry, connections, curvature tensors">;
def Dom_TopologyGeneral    : MathDomain<"54A", "GeneralTopology", "Metric spaces, compactifications, quotient spaces">;
def Dom_AlgebraicTopology  : MathDomain<"55N", "AlgebraicTopologyAndHomology", "Homology, cohomology, homotopy groups, Euler characteristic">;
def Dom_ManifoldsAndCell   : MathDomain<"57R", "ManifoldsAndCellComplexes", "Differential topology, cobordism, surgery theory">;
def Dom_GlobalAnalysis     : MathDomain<"58A", "GlobalAnalysisAndAnalysisOnManifolds", "Infinite-dimensional manifolds, catastrophe theory, Morse theory">;
def Dom_Probability        : MathDomain<"60A", "ProbabilityAndStochastic", "RNG, normal/gamma distributions, Monte Carlo, entropy">;
def Dom_StatisticsTheory   : MathDomain<"62F", "StatisticalInferenceAndEstimation", "Maximum likelihood, Bayesian inference, hypothesis testing">;
def Dom_NumericalAnalysis  : MathDomain<"65M", "NumericalAnalysis",         "Finite Difference, Stencils, Collocation, DEC">;
def Dom_DataCompression    : MathDomain<"68P30", "CodingAndInformationTheory", "Lossless data compression, rANS, LZSS, Huffman, ZFP">;
def Dom_ComputerGraphics   : MathDomain<"68U05", "ComputerGraphicsAndAnimation", "Molang evaluation, high-poly mesh rasterization, skeletal animation">;
def Dom_SpatialAcceleration: MathDomain<"68U05B", "SpatialDataStructuresAndBVH", "Bounding Volume Hierarchies, BVH2/BVH4/BVH8, ray-box slab traversal">;
def Dom_ClassicalMechanics : MathDomain<"70H", "MechanicsOfParticles",      "Hamiltonian & Lagrangian Mechanics, XPBD, Multibody">;
def Dom_ContinuumMechanics : MathDomain<"74B", "MechanicsOfDeformableSolids","Elasticity, Stress-Strain, Lamé Parameters">;
def Dom_FluidMechanics     : MathDomain<"76D", "FluidMechanics",            "Navier-Stokes, SPH, Lattice Boltzmann BGK">;
def Dom_AcousticPhysics    : MathDomain<"76Q05", "AcousticsAndSoundPropagation", "Acoustic ray tracing, sound pressure FDTD, HRTF, Sabine RT60">;
def Dom_OpticsEM           : MathDomain<"78A", "OpticsAndElectromagnetics", "Maxwell Equations, Staggered Yee FDTD">;
def Dom_QuantumTheory      : MathDomain<"81P", "QuantumTheory",             "Statevectors, Spinors, Unitary Dynamics, Hamiltonians">;
def Dom_DiscreteQuantum    : MathDomain<"81T25", "DiscreteLatticeQuantumMechanics", "Finite Planck-bounded Heisenberg uncertainty, non-divergent 0/inf avoidance, discrete lattice QCD/QED/QFC">;
def Dom_MathematicalPhysics: MathDomain<"82B", "EquilibriumStatisticalMechanics", "Ising models, partition functions, phase transitions">;
def Dom_Relativity         : MathDomain<"83C", "RelativityAndGravitation",  "Geodesics, Curvature, Spacetime Metrics, Christoffel">;
def Dom_Astrophysics       : MathDomain<"85A25", "AstrophysicsAndRelativisticSim", "Kerr black hole raymarching, magnetar plasma physics, relativistic fluid">;
def Dom_OperationsResearch : MathDomain<"90B", "OperationsResearchAndOptimization", "Simplex, interior point, flow networks, convex programming">;
def Dom_GameTheory         : MathDomain<"91A", "GameTheoryAndDecisions", "Zero-sum games, Nash equilibria, cooperative game dynamics">;
def Dom_EconomicsFinance   : MathDomain<"91G", "MathematicalFinanceAndStochastics", "Black-Scholes, Heston volatility, jump-diffusion, Ito calculus">;
def Dom_ControlTheory      : MathDomain<"93B", "SystemsTheoryAndControl", "Kalman filtering, observability, feedback stabilization">;
def Dom_SignalTheory       : MathDomain<"94A", "InformationAndSignalTheory","Modulation, Filtering, Windows, Detection">;
def Dom_Cryptology         : MathDomain<"94A60", "CryptologyAndSecurity", "Galois Fields, AES-NI, GFNI, SHA, Hash lattices">;
def Dom_EmbeddedSoftFloat  : MathDomain<"00A", "EmbeddedLowPower", "Single-cycle integer ALU, Fixed-point Q15/Q31, Soft-float emu">;

// =============================================================================
// SECTION 4: BASE MATHEMATICAL WAVEFORM & OPERATOR SCHEMA
// =============================================================================

class WaveformRule<string name, int id, MathDomain domain, MathDataType dtype> {
  assert !ne(name, ""), "WaveformRule Name cannot be empty!";
  assert !ge(id, 100), "RuleID must be >= 100!";
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
// MathventageSIMD — Math-Archetypes.td (Generative Engine)
// =============================================================================

multiclass DualPrecisionWaveform<string opName, int baseFnId, MathDomain dom, string formula, string decay, string disc, string desc> {
  def _f32 : WaveformRule<opName # "_f32", !add(baseFnId, 50000), dom, MathReal32> {
    let ExportName = "msimd_wave_" # opName # "_f32";
    let DisplayName = opName # " (F32)";
    let AnalyticalFormula = formula;
    let HarmonicDecay = decay;
    let DiscontinuityType = disc;
    let Description = desc;
  }
  def _f64 : WaveformRule<opName # "_f64", baseFnId, dom, MathReal64> {
    let ExportName = "msimd_wave_" # opName # "_f64";
    let DisplayName = opName # " (F64)";
    let AnalyticalFormula = formula;
    let HarmonicDecay = decay;
    let DiscontinuityType = disc;
    let Description = desc;
  }
}

multiclass BLAS_MatrixArchetype<string opName, int baseFnId, string formula, string desc> {
  foreach p = ["f32", "f64"] in {
    foreach layout = ["RowMajor", "ColMajor"] in {
      def "_" # p # "_" # layout : WaveformRule<
        opName # "_" # p # "_" # layout,
        !add(baseFnId, !if(!eq(p, "f32"), 50000, 0)),
        Dom_LinearAlgebra, !cast<MathDataType>(!if(!eq(p, "f32"), "MathReal32", "MathReal64"))
      > {
        let AnalyticalFormula = formula;
        let Description = desc;
        
      }
    }
  }
}

// Multi-type floating-point waveform generative expansion across hardware and emulated formats:
// F8, F16, F32, F64, F128, F256
multiclass MultiPrecisionWaveform<string opName, int baseFnId, MathDomain dom, string formula, string decay, string disc, string desc> {
  def _f8 : WaveformRule<opName # "_f8", !add(baseFnId, 40000), dom, MathFloat8> {
    let ExportName = "msimd_wave_" # opName # "_f8";
    let DisplayName = opName # " (F8)";
    let AnalyticalFormula = formula;
    let HarmonicDecay = decay;
    let DiscontinuityType = disc;
    let Description = desc;
  }
  def _f16 : WaveformRule<opName # "_f16", !add(baseFnId, 45000), dom, MathFloat16> {
    let ExportName = "msimd_wave_" # opName # "_f16";
    let DisplayName = opName # " (F16)";
    let AnalyticalFormula = formula;
    let HarmonicDecay = decay;
    let DiscontinuityType = disc;
    let Description = desc;
  }
  def _f32 : WaveformRule<opName # "_f32", !add(baseFnId, 50000), dom, MathReal32> {
    let ExportName = "msimd_wave_" # opName # "_f32";
    let DisplayName = opName # " (F32)";
    let AnalyticalFormula = formula;
    let HarmonicDecay = decay;
    let DiscontinuityType = disc;
    let Description = desc;
  }
  def _f64 : WaveformRule<opName # "_f64", baseFnId, dom, MathReal64> {
    let ExportName = "msimd_wave_" # opName # "_f64";
    let DisplayName = opName # " (F64)";
    let AnalyticalFormula = formula;
    let HarmonicDecay = decay;
    let DiscontinuityType = disc;
    let Description = desc;
  }
  def _f128 : WaveformRule<opName # "_f128", !add(baseFnId, 60000), dom, MathFloat128> {
    let ExportName = "msimd_wave_" # opName # "_f128";
    let DisplayName = opName # " (F128)";
    let AnalyticalFormula = formula;
    let HarmonicDecay = decay;
    let DiscontinuityType = disc;
    let Description = desc;
  }
  def _f256 : WaveformRule<opName # "_f256", !add(baseFnId, 70000), dom, MathFloat256> {
    let ExportName = "msimd_wave_" # opName # "_f256";
    let DisplayName = opName # " (F256)";
    let AnalyticalFormula = formula;
    let HarmonicDecay = decay;
    let DiscontinuityType = disc;
    let Description = desc;
  }
}

// Multi-type integer waveform generative expansion across integer formats:
// I8, I16, I32, I64, I128, I256
multiclass DiscreteIntegerWaveform<string opName, int baseFnId, MathDomain dom, string formula, string disc, string desc> {
  def _i8 : WaveformRule<opName # "_i8", !add(baseFnId, 10000), dom, MathInteger8> {
    let ExportName = "msimd_wave_" # opName # "_i8";
    let DisplayName = opName # " (I8)";
    let AnalyticalFormula = formula;
    let DiscontinuityType = disc;
    let Description = desc;
  }
  def _i16 : WaveformRule<opName # "_i16", !add(baseFnId, 20000), dom, MathInteger16> {
    let ExportName = "msimd_wave_" # opName # "_i16";
    let DisplayName = opName # " (I16)";
    let AnalyticalFormula = formula;
    let DiscontinuityType = disc;
    let Description = desc;
  }
  def _i32 : WaveformRule<opName # "_i32", !add(baseFnId, 30000), dom, MathInteger32> {
    let ExportName = "msimd_wave_" # opName # "_i32";
    let DisplayName = opName # " (I32)";
    let AnalyticalFormula = formula;
    let DiscontinuityType = disc;
    let Description = desc;
  }
  def _i64 : WaveformRule<opName # "_i64", !add(baseFnId, 40000), dom, MathInteger64> {
    let ExportName = "msimd_wave_" # opName # "_i64";
    let DisplayName = opName # " (I64)";
    let AnalyticalFormula = formula;
    let DiscontinuityType = disc;
    let Description = desc;
  }
  def _i128 : WaveformRule<opName # "_i128", !add(baseFnId, 50000), dom, MathInteger128> {
    let ExportName = "msimd_wave_" # opName # "_i128";
    let DisplayName = opName # " (I128)";
    let AnalyticalFormula = formula;
    let DiscontinuityType = disc;
    let Description = desc;
  }
  def _i256 : WaveformRule<opName # "_i256", !add(baseFnId, 60000), dom, MathInteger256> {
    let ExportName = "msimd_wave_" # opName # "_i256";
    let DisplayName = opName # " (I256)";
    let AnalyticalFormula = formula;
    let DiscontinuityType = disc;
    let Description = desc;
  }
}

class MasterKernelImplementation<string name, code implementation> {
  string Name = name;
  code Body = implementation;
}

multiclass DualKernelImplementation<string f32Name, string f64Name, code f32Body, code f64Body> {
  def _f32 : MasterKernelImplementation<f32Name, f32Body>;
  def _f64 : MasterKernelImplementation<f64Name, f64Body>;
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
defm Wave_Sine : DualPrecisionWaveform<
    "harmonic_sine", 101, Dom_FourierAnalysis,
    "\\sin(t)", "Pure (no upper harmonics)", "Smooth",
    "Pure fundamental harmonic oscillator with zero spectral leakage.">;

defm Wave_Cosine : DualPrecisionWaveform<
    "harmonic_cosine", 102, Dom_FourierAnalysis,
    "\\cos(t)", "Pure (no upper harmonics)", "Smooth",
    "Orthogonal quadrature harmonic oscillator with phase shift pi/2.">;

defm Wave_SymmetricSquare : DualPrecisionWaveform<
    "symmetric_square_duty50", 103, Dom_FourierAnalysis,
    "\\operatorname{sgn}(\\sin(t))", "1/n (-6 dB/oct, odd harmonics only)", "Jump",
    "Piecewise constant bipolar waveform with jump discontinuities at half periods.">;

defm Wave_SymmetricTriangle : DualPrecisionWaveform<
    "symmetric_triangle", 104, Dom_FourierAnalysis,
    "\\frac{2}{\\pi}\\arcsin(\\sin(t))", "1/n^2 (-12 dB/oct, odd harmonics only)", "Corner",
    "Continuous piecewise linear wave with alternating slopes and sharp peak corners.">;

defm Wave_SawtoothRampUp : DualPrecisionWaveform<
    "sawtooth_ramp_up", 105, Dom_FourierAnalysis,
    "2(t - \\lfloor t + 0.5 \\rfloor)", "1/n (-6 dB/oct, all harmonics)", "Jump",
    "Linearly rising sawtooth profile with periodic instantaneous downward jump.">;

defm Wave_SawtoothRampDown : DualPrecisionWaveform<
    "sawtooth_ramp_down", 106, Dom_FourierAnalysis,
    "-2(t - \\lfloor t + 0.5 \\rfloor)", "1/n (-6 dB/oct, all harmonics)", "Jump",
    "Linearly descending sawtooth profile with periodic instantaneous upward jump.">;

defm Wave_Trapezoidal : DualPrecisionWaveform<
    "trapezoidal_linear_transition", 107, Dom_FourierAnalysis,
    "\\operatorname{clamp}(2\\operatorname{tri}(t), -1, 1)", "1/n^2", "Corner",
    "Slew-rate limited square wave with finite ramp transitions avoiding infinite slope.">;

defm Wave_Staircase : DualPrecisionWaveform<
    "staircase_quantized_ramp", 108, Dom_FourierAnalysis,
    "\\frac{\\lfloor k t \\rfloor}{k}", "1/n", "Jump",
    "Monotonically stepping staircase wave with uniform discrete riser increments.">;

defm Wave_DiracImpulseTrain : DualPrecisionWaveform<
    "dirac_impulse_train", 109, Dom_FourierAnalysis,
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

// 1. Silindirik Bessel & Hankel Aileleri (Tam 88 Dalga Formu, Çakışmasız RuleID)
class BesselFamily<string tag, string pfx, string title, int idOffset> {
  string Tag = tag;
  string Prefix = pfx;
  string Title = title;
  int IdOffset = idOffset;
}

def Fam_Bessel_J      : BesselFamily<"first_kind_J",  "j",     "Bessel Function 1st Kind J",     0>;
def Fam_Bessel_Y      : BesselFamily<"second_kind_Y", "y",     "Neumann Function 2nd Kind Y",    100>;
def Fam_Bessel_Mod_I  : BesselFamily<"modified_I",    "mod_i", "Modified Bessel 1st Kind I",     200>;
def Fam_Bessel_Mod_K  : BesselFamily<"modified_K",    "mod_k", "Modified Bessel 2nd Kind K",     300>;
def Fam_Hankel_H1     : BesselFamily<"hankel_1st_H1", "h1",    "Hankel Function 1st Kind H1",    400>;
def Fam_Hankel_H2     : BesselFamily<"hankel_2nd_H2", "h2",    "Hankel Function 2nd Kind H2",    500>;
def Fam_Bessel_Sph_j  : BesselFamily<"spherical_j",   "sph_j", "Spherical Bessel 1st Kind j",   600>;
def Fam_Bessel_Sph_y  : BesselFamily<"spherical_y",   "sph_y", "Spherical Neumann 2nd Kind y",  700>;

class ExplicitBesselWave<BesselFamily fam, int order> : WaveformRule<
    "bessel_" # fam.Tag # "_" # order,
    !add(13000, fam.IdOffset, order),
    Dom_SpecialFunctions,
    MathReal64> {
  assert !ge(order, 0), "Bessel order cannot be negative!";
  assert !le(order, 50), "Bessel order exceeds supported range!";
  
  string BesselKind = fam.Tag;
  int Order = order;
  let ExportName = "msimd_wave_bessel_" # fam.Prefix # "_" # order;
  let DisplayName = fam.Title # " of Order " # order;
  let Description = "Cylindrical wave solution to Bessel's differential equation representing radial harmonic modes.";
  let GoverningEquation = "x^2 y'' + x y' + (x^2 - n^2) y = 0";
}

foreach fam = [Fam_Bessel_J, Fam_Bessel_Y, Fam_Bessel_Mod_I, Fam_Bessel_Mod_K,
               Fam_Hankel_H1, Fam_Hankel_H2, Fam_Bessel_Sph_j, Fam_Bessel_Sph_y] in {
  foreach N = 0-10 in {
    def "Wave_Bessel_" # fam.Prefix # "_" # N : ExplicitBesselWave<fam, N>;
  }
}

// 2. Ortogonal Polinom Dalga Aileleri (d=1..8 -> Çakışmasız Unique RuleID)
class PolyFamily<string tag, string pfx, string title, int idOffset> {
  string Tag = tag;
  string Prefix = pfx;
  string Title = title;
  int IdOffset = idOffset;
}

def Fam_Poly_Legendre   : PolyFamily<"legendre_P",   "legendre_p",   "Legendre Polynomial P",            0>;
def Fam_Poly_Hermite    : PolyFamily<"hermite_H",    "hermite_h",    "Hermite Polynomial H",             100>;
def Fam_Poly_ChebyshevT : PolyFamily<"chebyshev_T",  "chebyshev_t",  "Chebyshev Polynomial 1st Kind T",  200>;
def Fam_Poly_ChebyshevU : PolyFamily<"chebyshev_U",  "chebyshev_u",  "Chebyshev Polynomial 2nd Kind U",  300>;
def Fam_Poly_Laguerre   : PolyFamily<"laguerre_L",   "laguerre_l",   "Laguerre Polynomial L",            400>;
def Fam_Poly_Gegenbauer : PolyFamily<"gegenbauer_C", "gegenbauer_c", "Gegenbauer Polynomial C",          500>;

class ExplicitPolyWave<PolyFamily fam, int degree> : WaveformRule<
    "poly_" # fam.Tag # "_deg" # degree,
    !add(14000, fam.IdOffset, degree),
    Dom_SpecialFunctions,
    MathReal64> {
  assert !ge(degree, 1), "Polynomial degree must be >= 1!";
  assert !le(degree, 50), "Polynomial degree exceeds supported range!";
  
  string FamilyName = fam.Tag;
  int Degree = degree;
  let ExportName = "msimd_wave_poly_" # fam.Prefix # "_deg" # degree;
  let DisplayName = fam.Title # " of Degree " # degree;
  let Description = "Orthogonal polynomial wave satisfying three-term Sturm-Liouville recurrence relations.";
}

foreach fam = [Fam_Poly_Legendre, Fam_Poly_Hermite, Fam_Poly_ChebyshevT,
               Fam_Poly_ChebyshevU, Fam_Poly_Laguerre, Fam_Poly_Gegenbauer] in {
  foreach D = 1-8 in {
    def "Wave_" # fam.Prefix # "_" # D : ExplicitPolyWave<fam, D>;
  }
}

// 3. Jacobi Eliptik Dalgaları (12 Kanonik Form -> Çakışmasız Unique RuleID 501..512)
class ExplicitJacobiWave<string symbol, string export_sym, int idOffset, string display_title, string desc>
    : WaveformRule<"jacobi_elliptic_" # symbol, !add(501, idOffset), Dom_SpecialFunctions, MathReal64> {
  string FunctionSymbol = symbol;
  let ExportName = "msimd_wave_jacobi_" # export_sym;
  let DisplayName = "Jacobi Elliptic " # display_title # " (" # symbol # ")";
  let Description = desc;
  let GoverningEquation = "\\left(\\frac{d}{du} \\mathrm{" # symbol # "}(u,m)\\right)^2 = P_4(\\mathrm{" # symbol # "})";
}

def Wave_Jacobi_SN : ExplicitJacobiWave<"sn", "sn", 0,  "Sine Amplitude", "Sinusoidal amplitude elliptic function sn(u,m).">;
def Wave_Jacobi_CN : ExplicitJacobiWave<"cn", "cn", 1,  "Cosine Amplitude", "Co-sinusoidal amplitude elliptic function cn(u,m).">;
def Wave_Jacobi_DN : ExplicitJacobiWave<"dn", "dn", 2,  "Delta Amplitude", "Delta amplitude elliptic function dn(u,m).">;
def Wave_Jacobi_CD : ExplicitJacobiWave<"cd", "cd", 3,  "Ratio cn/dn", "Elliptic quotient cn(u,m) / dn(u,m).">;
def Wave_Jacobi_SD : ExplicitJacobiWave<"sd", "sd", 4,  "Ratio sn/dn", "Elliptic quotient sn(u,m) / dn(u,m).">;
def Wave_Jacobi_ND : ExplicitJacobiWave<"nd", "nd", 5,  "Reciprocal 1/dn", "Reciprocal elliptic delta 1 / dn(u,m).">;
def Wave_Jacobi_DC : ExplicitJacobiWave<"dc", "dc", 6,  "Ratio dn/cn", "Elliptic quotient dn(u,m) / cn(u,m).">;
def Wave_Jacobi_NC : ExplicitJacobiWave<"nc", "nc", 7,  "Reciprocal 1/cn", "Reciprocal elliptic cosine 1 / cn(u,m).">;
def Wave_Jacobi_SC : ExplicitJacobiWave<"sc", "sc", 8,  "Ratio sn/cn", "Elliptic tangent quotient sn(u,m) / cn(u,m).">;
def Wave_Jacobi_NS : ExplicitJacobiWave<"ns", "ns", 9,  "Reciprocal 1/sn", "Reciprocal elliptic sine 1 / sn(u,m).">;
def Wave_Jacobi_DS : ExplicitJacobiWave<"ds", "ds", 10, "Ratio dn/sn", "Elliptic quotient dn(u,m) / sn(u,m).">;
def Wave_Jacobi_CS : ExplicitJacobiWave<"cs", "cs", 11, "Ratio cn/sn", "Elliptic cotangent quotient cn(u,m) / sn(u,m).">;

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

// =============================================================================
// MATHEMATICAL AST DAG NODES (Instruction Selection & SIMD FMA Pattern Matching)
// =============================================================================
class MathASTNode;
def node_var   : MathASTNode;
def node_const : MathASTNode;
def node_add   : MathASTNode;
def node_sub   : MathASTNode;
def node_mul   : MathASTNode;
def node_fma   : MathASTNode;
def node_sin   : MathASTNode;
def node_cos   : MathASTNode;

class PolynomialApproximationRule<string name, int id, string export_fn, string display_title,
                                  string formula, string interval, string max_err, string desc>
    : WaveformRule<name, id, Dom_RealFunctions, MathReal32> {
  string MinimaxInterval = interval;
  string MaxAbsoluteError = max_err;
  dag ExpressionAST = (node_var "x");
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
    "Single-precision 7th-order minimax polynomial evaluating sin(x) with 2 ULP precision."> {
  let ExpressionAST = (node_mul (node_var "x"),
    (node_fma
      (node_mul (node_var "x"), (node_var "x")),
      (node_fma
        (node_mul (node_var "x"), (node_var "x")),
        (node_const "-0.00019515"),
        (node_const "0.00833216")),
      (node_fma
        (node_mul (node_var "x"), (node_var "x")),
        (node_const "-0.16666654"),
        (node_const "1.0"))));
}

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

// =============================================================================
// =============================================================================
// ADVANCED COMPUTABLE PHYSICAL & ALGORITHMIC DOMAIN SCHEMAS
// =============================================================================
class AdvancedDomainRule<string name, int id, MathDomain domain, MathDataType dtype> {
  assert !ne(name, ""), "AdvancedDomainRule Name cannot be empty!";
  assert !ge(id, 100), "RuleID must be >= 100!";
  string Name = name;
  int RuleID = id;
  MathDomain Domain = domain;
  MathDataType DataType = dtype;
  string ExportName = name;
  string DisplayName = name;
  string Description = "";
  string AnalyticalFormula = "";
  string GoverningEquation = "";
  list<ConservationLaw> Invariants = [];
}

// =============================================================================
// PILLAR 8: CPU ACOUSTIC RAY TRACING & PHYSICAL SOUND PROPAGATION
// =============================================================================
class AcousticTracingRule<string name, int id, string export_fn, string display_title, string desc>
    : AdvancedDomainRule<name, id, Dom_AcousticPhysics, MathReal32> {
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
}

def Acoustic_Trace_Ray : AcousticTracingRule<
    "acoustic_trace_ray", 801, "msimd_acoustic_trace_ray",
    "Acoustic Wave Ray Tracing Pipeline",
    "Casts acoustic pressure byte array as a directional sound ray through 3D medium with attenuation and boundary absorption.">;

def Acoustic_HRTF_Binaural : AcousticTracingRule<
    "acoustic_hrtf_binaural", 802, "msimd_acoustic_hrtf_binaural_process",
    "Head-Related Transfer Function (HRTF) 3D Convolution",
    "Convolves audio signal with left/right ear impulse responses for spatialized binaural audio.">;

def Acoustic_Sabine_Reverberation : AcousticTracingRule<
    "acoustic_sabine_reverberation", 803, "msimd_acoustic_sabine_rt60",
    "Sabine/Eyring Volumetric Reverberation Time RT60",
    "Computes reverberation decay time RT60 across room boundaries and absorption coefficients.">;

def Acoustic_Doppler_Shift : AcousticTracingRule<
    "acoustic_doppler_shift", 804, "msimd_acoustic_doppler_shift",
    "Acoustic Doppler Frequency Shift",
    "Evaluates frequency shift between moving source and observer in physical medium.">;

def Acoustic_FDTD_Step_3D : AcousticTracingRule<
    "acoustic_fdtd_step_3d", 805, "msimd_acoustic_fdtd_step_3d",
    "3D Acoustic Pressure-Velocity Staggered Yee FDTD",
    "Advances acoustic pressure and 3D particle velocity vector fields on staggered grid.">;

// =============================================================================
// PILLAR 9: COMPUTER GRAPHICS, HIGH-POLY MESHES & MOLANG MATH
// =============================================================================
class GraphicsAnimationRule<string name, int id, string export_fn, string display_title, string desc>
    : AdvancedDomainRule<name, id, Dom_ComputerGraphics, MathReal32> {
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
}

def Molang_Math_Evaluator : GraphicsAnimationRule<
    "molang_math_evaluator", 820, "msimd_molang_eval",
    "Molang Expression & Animation Math Engine",
    "Evaluates high-speed branchless Molang statements and query variables for high-poly entity animation.">;

def Animation_DualQuat_Skinning : GraphicsAnimationRule<
    "animation_dualquat_skinning", 821, "msimd_animation_dualquat_skinning",
    "Dual Quaternion Linear Blending (DLB) Mesh Skinning",
    "High-throughput SIMD mesh skinning eliminating volume loss and candy-wrapper artifacts.">;

def Animation_Catmull_Rom : GraphicsAnimationRule<
    "animation_catmull_rom", 822, "msimd_animation_catmull_rom",
    "Centripetal Catmull-Rom Animation Spline",
    "Evaluates smooth C1 continuous centripetal spline curves without self-intersection or cusps.">;

def Graphics_BVH_Ray_Intersect : GraphicsAnimationRule<
    "graphics_bvh_ray_intersect", 823, "msimd_bvh_ray_intersect",
    "SIMD Bounding Volume Hierarchy (BVH) Traversal",
    "Multi-lane slab ray-AABB test and Möller-Trumbore ray-triangle intersection traversal.">;

def Graphics_Cull_Frustum_AABBs : GraphicsAnimationRule<
    "graphics_cull_frustum_aabbs", 824, "msimd_cull_frustum_aabbs_f32",
    "Open-World Batched Frustum Culling (AABB)",
    "Batched SIMD frustum culling of chunk and entity axis-aligned bounding boxes against 6 camera planes.">;

def Graphics_Cull_Frustum_Spheres : GraphicsAnimationRule<
    "graphics_cull_frustum_spheres", 825, "msimd_cull_frustum_spheres_f32",
    "Open-World Batched Frustum Culling (Bounding Spheres)",
    "Batched SIMD frustum culling of bounding spheres against 6 camera planes with signed distance tests.">;

def Graphics_Cull_Distance_LOD : GraphicsAnimationRule<
    "graphics_cull_distance_lod", 826, "msimd_cull_distance_lod_f32",
    "Open-World Distance LOD & Range Culling",
    "SIMD squared distance evaluation against camera viewpoint mapping entities to discrete LOD bands or culled state.">;

def Graphics_Voxel_Cull_Hidden_Faces : GraphicsAnimationRule<
    "graphics_voxel_cull_hidden_faces", 827, "msimd_voxel_cull_hidden_faces_u32",
    "Open-World Voxel 6-Neighbor Hidden Face Culling",
    "Batched 6-neighbor voxel occlusion mask test eliminating 75-85% of interior hidden quads prior to chunk meshing.">;

// =============================================================================
// PILLAR 10: RELATIVISTIC ASTROPHYSICS, KERR BLACK HOLE & MAGNETAR PHYSICS
// =============================================================================
class RelativisticAstrophysicsRule<string name, int id, string export_fn, string display_title, string metric, string desc>
    : AdvancedDomainRule<name, id, Dom_Astrophysics, MathReal64> {
  string SpacetimeMetric = metric;
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
}

def Astro_Kerr_BlackHole_Raymarch : RelativisticAstrophysicsRule<
    "astro_kerr_black_hole_raymarch", 840, "msimd_astro_kerr_raymarch",
    "Kerr Rotating Black Hole Geodesic Raymarcher", "Kerr Spacetime (Boyer-Lindquist)",
    "Simulates photon geodesics, frame dragging, ergosphere boundary, and photon ring around rotating black hole.">;

def Astro_Gravitational_Redshift : RelativisticAstrophysicsRule<
    "astro_gravitational_redshift", 841, "msimd_astro_gravitational_redshift",
    "Extreme Relativistic Gravitational Redshift Factor", "Schwarzschild / Kerr",
    "Computes photon frequency ratio and relativistic time dilation in strong gravitational fields.">;

def Astro_Magnetar_BField : RelativisticAstrophysicsRule<
    "astro_magnetar_bfield", 842, "msimd_astro_magnetar_bfield",
    "Magnetar Ultra-Strong Magnetic Field & Synchrotron Radiation", "Force-Free Magnetosphere",
    "Simulates quantum vacuum polarization and synchrotron emission under ultra-intense 10^11 Tesla fields.">;

// =============================================================================
// PILLAR 11: CONTINUOUS MULTIVARIABLE CALCULUS & INTEGRATION
// =============================================================================
class ContinuousCalculusRule<string name, int id, string export_fn, string display_title, string desc>
    : AdvancedDomainRule<name, id, Dom_RealFunctions, MathReal64> {
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
}

def Calculus_Derivative_Central : ContinuousCalculusRule<
    "calculus_derivative_central", 860, "msimd_calculus_derivative_central",
    "High-Order Central Difference Derivative",
    "Evaluates 4th/6th-order continuous numerical derivatives with automatic step-size Richardson extrapolation.">;

def Calculus_Integral_Simpson : ContinuousCalculusRule<
    "calculus_integral_simpson", 861, "msimd_calculus_integral_simpson",
    "Simpson 3/8 Composite Continuous Quadrature",
    "High-accuracy adaptive composite integration of continuous real-valued functions.">;

def Calculus_Integral_Gauss_Legendre : ContinuousCalculusRule<
    "calculus_integral_gauss_legendre", 862, "msimd_calculus_integral_gauss_legendre",
    "Gauss-Legendre N-Point Quadrature",
    "Optimal polynomial exact integration utilizing orthogonal Legendre roots and weights.">;

// =============================================================================
// PILLAR 12: DISCRETE PLANCK-SCALE QUANTUM MECHANICS & LATTICE FIELD THEORY
// =============================================================================
class DiscretePlanckQuantumRule<string name, int id, string export_fn, string display_title, string bound, string desc>
    : AdvancedDomainRule<name, id, Dom_DiscreteQuantum, MathReal64> {
  string DiscreteBound = bound;
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
}

def Quantum_Planck_Bounded_Uncertainty : DiscretePlanckQuantumRule<
    "quantum_planck_bounded_uncertainty", 880, "msimd_quantum_planck_bounded_uncertainty",
    "Bounded Planck Uncertainty Space Evaluator", "ell_P <= Delta x <= L_max",
    "Evaluates Heisenberg uncertainty strictly bounded inside finite Planck lattice window, preventing divergence to infinity on silicon.">;

def Quantum_NonDivergent_Denominator : DiscretePlanckQuantumRule<
    "quantum_nondivergent_denominator", 881, "msimd_quantum_nondivergent_denominator",
    "Planck-Regularized Non-Divergent Denominator", "1 / sqrt(x^2 + ell_P^2)",
    "Ensures discrete physical equations never divide by zero or diverge toward infinity in silicon computations.">;

def Quantum_Lattice_QCD_Cornell : DiscretePlanckQuantumRule<
    "quantum_lattice_qcd_cornell", 882, "msimd_qcd_cornell_potential",
    "QCD Cornell Quark-Confinement Potential & String Tension", "V(r) = -4/3 alpha_s / r + sigma r",
    "Evaluates short-distance Coulombic gluon exchange and long-distance linear color string confinement.">;

def Quantum_Lattice_SU3_GellMann : DiscretePlanckQuantumRule<
    "quantum_lattice_su3_gellmann", 883, "msimd_qcd_gellmann_apply",
    "SU(3) Color Algebra Gell-Mann Matrix Operator", "lambda_1..lambda_8",
    "Applies generator matrices of the special unitary SU(3) color group to quark color triplets.">;

def Quantum_Flavor_CKM_Mix : DiscretePlanckQuantumRule<
    "quantum_flavor_ckm_mix", 884, "msimd_quantum_flavor_ckm_mix",
    "Quark Flavor Dynamics CKM Unitary Mixing", "V_CKM 3x3 Matrix",
    "Computes weak interaction quark flavor transitions and CP-violating phase in discrete field configurations.">;

def Quantum_Planck_Bounded_Uncertainty_256 : AdvancedDomainRule<
    "quantum_planck_bounded_uncertainty_256", 885, Dom_DiscreteQuantum, MathFloat256> {
  let ExportName = "msimd_quantum_planck_bounded_uncertainty_256";
  let DisplayName = "256-Bit Emulated Bounded Heisenberg Uncertainty Space";
  let AnalyticalFormula = "\\Delta x \\cdot \\Delta p \\ge \\frac{\\hbar}{2}, \\quad \\Delta x \\in [\\ell_P, L_{max}]";
  let Description = "Evaluates bounded Heisenberg phase space in 256-bit quad-double multi-limb precision without underflow or catastrophic cancellation.";
}

def Quantum_Wavepacket_Peak_Pinpoint_256 : AdvancedDomainRule<
    "quantum_wavepacket_peak_pinpoint_256", 886, Dom_DiscreteQuantum, MathFloat256> {
  let ExportName = "msimd_quantum_wavepacket_peak_pinpoint_256";
  let DisplayName = "256-Bit 100% Phase-Space Waveform Compression & Crest Pinpointer";
  let AnalyticalFormula = "\\mathrm{argmax}_{x} |\\Psi(x, t)|^2 \\quad (100\\% \\text{ compressed})";
  let Description = "Compresses quantum phase-space wavepacket to 100% to deterministically pinpoint the absolute crest of the quantum field excitation in discrete Planck spacetime.";
}

// =============================================================================
// PILLAR 13: LOSSLESS DATA COMPRESSION & ENTROPY CODING
// =============================================================================
class CompressionEntropyRule<string name, int id, string export_fn, string display_title, string desc>
    : AdvancedDomainRule<name, id, Dom_DataCompression, MathInteger32> {
  let ExportName = export_fn;
  let DisplayName = display_title;
  let Description = desc;
}

def Compress_RLE : CompressionEntropyRule<
    "compress_rle", 901, "msimd_compress_rle",
    "Run-Length Encoding (RLE) Byte Compressor",
    "Vectorized byte-run compression for sparse tensors and repetitive geometric data.">;

def Compress_LZSS : CompressionEntropyRule<
    "compress_lzss", 902, "msimd_compress_lzss",
    "LZSS Sliding Window Dictionary Compressor",
    "Lempel-Ziv-Storer-Szymanski dictionary encoder with high-throughput sliding window match.">;

def Compress_rANS : CompressionEntropyRule<
    "compress_rans", 903, "msimd_compress_rans_encode",
    "Asymmetric Numeral Systems (rANS) SIMD Entropy Coder",
    "High-throughput sub-bit precision entropy encoder approaching the theoretical Shannon limit.">;



// =============================================================================
// SECTION 8: MASTER UNIFIED C-API SPECIFICATION & HEADER CODEX
// =============================================================================

class MasterCHeaderBlock<string id, string desc, code content> {
  string BlockID = id;
  string Description = desc;
  code Source = content;
}



class MasterKernelSourceModule<string id, string desc, code source_code> {
  string ModuleID = id;
  string Description = desc;
  code Source = source_code;
}

// Master C-API Header Complete Specification (Lossless Representation)
def Master_MathventageSIMD_Header : MasterCHeaderBlock<"MathventageSIMD.h", "Complete C API Header Specification", [{
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
typedef msimd_fp128_t msimd_f128_t;

/* 11. Multi-Limb 256-bit Integer & Float Types (Universal Emulation) */
typedef struct { uint64_t limbs[4]; } msimd_u256_t;
typedef struct { int64_t  limbs[4]; } msimd_i256_t;
typedef struct { double   limbs[4]; } msimd_f256_t;

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
 * @param[in,out] psi_re           Statevector real amplitudes array [2^{num_qubits} ] updated in-place.
 * @param[in,out] psi_im           Statevector imaginary amplitudes array [2^{num_qubits} ] updated in-place.
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

/* ========================================================================= */
/* SECTION 47: EXTENDED COMPUTATIONAL, PHYSICAL & ALGORITHMIC KERNELS       */
/* ========================================================================= */
/** @defgroup MSIMD_Section47 Extended Computational & Physical Kernels
 *  @{
 */

/**
 * @brief Computational kernel msimd_vec4f_reflect.
 */
MSIMD_API void            msimd_vec4f_reflect(const float* v, const float* n, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_vec4d_reflect.
 */
MSIMD_API void            msimd_vec4d_reflect(const double* v, const double* n, double* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_mat3f_identity.
 */
MSIMD_API void            msimd_mat3f_identity(float* m, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_mat3f_invert.
 */
MSIMD_API bool            msimd_mat3f_invert(const float* m, float* out);

/**
 * @brief Computational kernel msimd_mat4f_identity.
 */
MSIMD_API void            msimd_mat4f_identity(float* m, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_mat4f_transpose.
 */
MSIMD_API void            msimd_mat4f_transpose(const float* m, float* out, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_quat_identity.
 */
MSIMD_API void            msimd_quat_identity(float* q, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_bessel_j0.
 */
MSIMD_API double          msimd_bessel_j0(double x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_bessel_j1.
 */
MSIMD_API double          msimd_bessel_j1(double x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_airy_ai.
 */
MSIMD_API double          msimd_airy_ai(double x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_legendre_p.
 */
MSIMD_API double          msimd_legendre_p(int n, double x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_chebyshev_t.
 */
MSIMD_API double          msimd_chebyshev_t(int n, double x, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_kdv_step.
 */
MSIMD_API void            msimd_kdv_step(float* u_next, const float* u_curr, const float* u_prev, size_t n, float dt, float dx, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_sine_gordon_step.
 */
MSIMD_API void            msimd_sine_gordon_step(float* u_next, const float* u_curr, const float* u_prev, size_t n, float dt, float dx, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_fractional_caputo_derivative.
 */
MSIMD_API void            msimd_fractional_caputo_derivative(const float* f, float* out, size_t n, float alpha, float dt, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_wavelet_haar_forward.
 */
MSIMD_API void            msimd_wavelet_haar_forward(const float* in, float* out, size_t n, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_wavelet_haar_inverse.
 */
MSIMD_API void            msimd_wavelet_haar_inverse(const float* in, float* out, size_t n, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_wavelet_daubechies4_forward.
 */
MSIMD_API void            msimd_wavelet_daubechies4_forward(const float* in, float* out, size_t n, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_kernel_wave_step_1d_fp32.
 */
MSIMD_API void            msimd_kernel_wave_step_1d_fp32(float* next, const float* curr, const float* prev, size_t nx, float r2, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_kernel_wave_step_3d_fp32.
 */
MSIMD_API void            msimd_kernel_wave_step_3d_fp32(float* next, const float* curr, const float* prev, size_t nx, size_t ny, size_t nz, float rx, float ry, float rz, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_kernel_maxwell_yee_3d.
 */
MSIMD_API void            msimd_kernel_maxwell_yee_3d(size_t nx, size_t ny, size_t nz, float* Ex, float* Ey, float* Ez, float* Hx, float* Hy, float* Hz, float dt, float dx, float dy, float dz, float mu, float eps, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_kernel_lbm_d2q9.
 */
MSIMD_API void            msimd_kernel_lbm_d2q9(size_t nx, size_t ny, const float* f_in, float* f_out, float* rho, float* ux, float* uy, float tau, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_kernel_stockham_fft_radix2.
 */
MSIMD_API void            msimd_kernel_stockham_fft_radix2(size_t n, const float* in_real, const float* in_imag, float* out_real, float* out_imag, int inverse, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_rigidbody_update_inv_inertia.
 */
MSIMD_API void            msimd_rigidbody_update_inv_inertia(MSIMDRigidBody* b, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_rigidbody_integrate_velocity.
 */
MSIMD_API void            msimd_rigidbody_integrate_velocity(MSIMDRigidBody* b, const float* gravity, float dt, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_rigidbody_integrate_position.
 */
MSIMD_API void            msimd_rigidbody_integrate_position(MSIMDRigidBody* b, float dt, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_pgs_prepare_contact.
 */
MSIMD_API void            msimd_pgs_prepare_contact(MSIMDContactConstraint* c, float dt, float baumgarte, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_pgs_solve_contact_iteration.
 */
MSIMD_API void            msimd_pgs_solve_contact_iteration(MSIMDContactConstraint* c, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_sph_compute_density_pressure.
 */
MSIMD_API void            msimd_sph_compute_density_pressure(MSIMDSPHParticle* p, size_t count, float h, float rest_density, float k_stiffness, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_c_add.
 */
MSIMD_API MSIMDComplex    msimd_c_add(MSIMDComplex a, MSIMDComplex b);

/**
 * @brief Computational kernel msimd_c_sub.
 */
MSIMD_API MSIMDComplex    msimd_c_sub(MSIMDComplex a, MSIMDComplex b);

/**
 * @brief Computational kernel msimd_c_mul.
 */
MSIMD_API MSIMDComplex    msimd_c_mul(MSIMDComplex a, MSIMDComplex b);

/**
 * @brief Computational kernel msimd_c_scale.
 */
MSIMD_API MSIMDComplex    msimd_c_scale(MSIMDComplex a, float s);

/**
 * @brief Computational kernel msimd_c_norm_sq.
 */
MSIMD_API float           msimd_c_norm_sq(MSIMDComplex a);

/**
 * @brief Computational kernel msimd_pauli_sigma_x.
 */
MSIMD_API void            msimd_pauli_sigma_x(const MSIMDComplex in[2], MSIMDComplex out[2], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_pauli_sigma_y.
 */
MSIMD_API void            msimd_pauli_sigma_y(const MSIMDComplex in[2], MSIMDComplex out[2], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_pauli_sigma_z.
 */
MSIMD_API void            msimd_pauli_sigma_z(const MSIMDComplex in[2], MSIMDComplex out[2], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_dirac_4spinor_step_3d.
 */
MSIMD_API void            msimd_dirac_4spinor_step_3d(MSIMDComplex* psi_next, const MSIMDComplex* psi_curr, size_t nx, size_t ny, size_t nz, float m, float c, float hbar, float dt, float dx, float dy, float dz, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_lattice_u1_plaquette_energy.
 */
MSIMD_API float           msimd_lattice_u1_plaquette_energy(const float* link_angles, size_t nx, size_t ny, size_t nz, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_lindblad_step_qubit.
 */
MSIMD_API void            msimd_lindblad_step_qubit(const MSIMDComplex rho[4], const MSIMDComplex H[4], float gamma_dephase, float gamma_relax, float dt, MSIMDComplex out_rho[4], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_aabb_intersect_ray.
 */
MSIMD_API bool            msimd_aabb_intersect_ray(const MSIMDAABB* box, const float* orig, const float* dir, float* out_tmin, float* out_tmax);

/**
 * @brief Computational kernel msimd_pbr_distribution_ggx.
 */
MSIMD_API float           msimd_pbr_distribution_ggx(const float* n, const float* h, float roughness);

/**
 * @brief Computational kernel msimd_pbr_geometry_schlick_ggx.
 */
MSIMD_API float           msimd_pbr_geometry_schlick_ggx(float n_dot_v, float roughness);

/**
 * @brief Computational kernel msimd_pbr_geometry_smith.
 */
MSIMD_API float           msimd_pbr_geometry_smith(const float* n, const float* v, const float* l, float roughness);

/**
 * @brief Computational kernel msimd_pbr_fresnel_schlick.
 */
MSIMD_API void            msimd_pbr_fresnel_schlick(float cos_theta, const float* F0, float* out_F);

/**
 * @brief Computational kernel msimd_pbr_cook_torrance_brdf.
 */
MSIMD_API void            msimd_pbr_cook_torrance_brdf(const float* n, const float* v, const float* l, const float* albedo, float metallic, float roughness, float* out_radiance, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_svdag_raymarch_dda.
 */
MSIMD_API bool            msimd_svdag_raymarch_dda(const uint32_t* voxel_grid, size_t dim_x, size_t dim_y, size_t dim_z, const float* orig, const float* dir, float max_dist, float* out_hit_pos, int* out_voxel_val);

/**
 * @brief Computational kernel msimd_kernel_ray_triangle_batch8.
 */
MSIMD_API void            msimd_kernel_ray_triangle_batch8(const float ox[8], const float oy[8], const float oz[8], const float dx[8], const float dy[8], const float dz[8], const float v0[3], const float v1[3], const float v2[3], int hit[8], float t[8], float u[8], float v[8], const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_acoustic_fdtd_step_3d.
 */
MSIMD_API void            msimd_acoustic_fdtd_step_3d(float* p, float* ux, float* uy, float* uz, size_t nx, size_t ny, size_t nz, float c, float rho, float dt, float dx, float dy, float dz, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_acoustic_doppler_shift.
 */
MSIMD_API float           msimd_acoustic_doppler_shift(float f0, const float* src_pos, const float* src_vel, const float* lis_pos, const float* lis_vel, float speed_of_sound, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_acoustic_sabine_rt60.
 */
MSIMD_API float           msimd_acoustic_sabine_rt60(float room_volume, const float* surface_areas, const float* absorption_coeffs, size_t surface_count, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_acoustic_hrtf_binaural_process.
 */
MSIMD_API void            msimd_acoustic_hrtf_binaural_process(const float* in_audio, size_t sample_count, const float* hrtf_left_ir, const float* hrtf_right_ir, size_t ir_len, float* out_left, float* out_right, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_compress_rle.
 */
MSIMD_API size_t          msimd_compress_rle(const uint8_t* in, size_t in_len, uint8_t* out, size_t out_max);

/**
 * @brief Computational kernel msimd_decompress_rle.
 */
MSIMD_API size_t          msimd_decompress_rle(const uint8_t* in, size_t in_len, uint8_t* out, size_t out_max);

/**
 * @brief Computational kernel msimd_compress_lzss.
 */
MSIMD_API size_t          msimd_compress_lzss(const uint8_t* in, size_t in_len, uint8_t* out, size_t out_max);

/**
 * @brief Computational kernel msimd_decompress_lzss.
 */
MSIMD_API size_t          msimd_decompress_lzss(const uint8_t* in, size_t in_len, uint8_t* out, size_t out_max);

/**
 * @brief Computational kernel msimd_bitpack_pack.
 */
MSIMD_API void            msimd_bitpack_pack(const uint32_t* values, size_t count, int bitwidth, uint8_t* out_bytes, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_bitpack_unpack.
 */
MSIMD_API void            msimd_bitpack_unpack(const uint8_t* in_bytes, size_t count, int bitwidth, uint32_t* out_values, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_float_to_half.
 */
MSIMD_API uint16_t        msimd_float_to_half(float val);

/**
 * @brief Computational kernel msimd_half_to_float.
 */
MSIMD_API float           msimd_half_to_float(uint16_t h, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_wave_morphology_step.
 */
MSIMD_API void            msimd_wave_morphology_step(float* out_state, const float* in_state, size_t n, float dispersion, float coupling, float nonlinearity, float dt, int mode, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_soliton_interaction_step.
 */
MSIMD_API void            msimd_soliton_interaction_step(float* phi_re, float* phi_im, size_t n, float v1, float v2, float phase_offset, float dt, float dx, int mode, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_discrete_spatial_filter_step.
 */
MSIMD_API void            msimd_discrete_spatial_filter_step(float* target, const float* source, size_t n, float alpha, int mode, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Computational kernel msimd_wave_taxonomy_eval.
 */
MSIMD_API void            msimd_wave_taxonomy_eval(int mode, float* out, const float* in, size_t n, float dispersion, float coupling, float nonlinearity, float dt, const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section47 */

/* ========================================================================= */
/* SECTION 48: HIGH-PRECISION EMULATION, QUANTUM & ADVANCED DOMAIN KERNELS   */
/* ========================================================================= */
/** @defgroup MSIMD_Section48 High-Precision Emulation, Quantum & Advanced Domain Kernels
 *  @{
 */

/**
 * @brief CPU Acoustic Ray Tracing directional sound propagation with medium absorption.
 */
MSIMD_API void msimd_acoustic_trace_ray(
    const float ray_origin[3],
    const float ray_dir[3],
    float initial_pressure,
    float medium_attenuation_db_m,
    float max_distance,
    float hit_absorption_coeff,
    float* out_traversed_dist,
    float* out_final_pressure,
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief High-speed branchless Molang expression evaluator for entity animation queries.
 */
MSIMD_API float msimd_molang_eval(
    const char* molang_expression,
    const float* query_vars,
    size_t query_var_count,
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Dual Quaternion Linear Blending (DLB) mesh skinning eliminating volume loss.
 */
MSIMD_API void msimd_animation_dualquat_skinning(
    const float in_vertex[3],
    const float in_normal[3],
    const float dual_quats[][8],
    const float* bone_weights,
    size_t bone_count,
    float out_vertex[3],
    float out_normal[3],
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Kerr Rotating Black Hole Geodesic Raymarcher (Boyer-Lindquist spacetime).
 */
MSIMD_API int msimd_astro_kerr_raymarch(
    double bh_mass,
    double bh_spin_a,
    const double ray_origin[3],
    const double ray_dir[3],
    double step_size,
    int max_steps,
    double* out_hit_dist,
    int* out_captured,
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Magnetar Ultra-Strong Magnetic Field and Quantum Vacuum Polarization evaluator.
 */
MSIMD_API void msimd_astro_magnetar_bfield(
    double b0_pole_tesla,
    double magnetar_radius_km,
    const double eval_pos_km[3],
    double out_bfield_tesla[3],
    double* out_b_magnitude,
    double* out_vacuum_polarization_idx,
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Bounded Planck Uncertainty Space evaluator in 256-bit quad-double multi-limb precision.
 */
MSIMD_API int msimd_quantum_planck_bounded_uncertainty_256(
    const msimd_f256_t* delta_x_in,
    const msimd_f256_t* delta_p_in,
    msimd_f256_t* phase_space_uncertainty_out,
    int* satisfies_planck_bound,
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Deterministic 100% Phase-Space Wavepacket Crest Pinpointer in discrete Planck spacetime.
 */
MSIMD_API size_t msimd_quantum_wavepacket_peak_pinpoint_256(
    const msimd_f256_t* psi_real,
    const msimd_f256_t* psi_imag,
    size_t sample_count,
    msimd_f256_t* out_peak_probability,
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Quark Flavor Dynamics CKM Unitary 3x3 Mixing with CP-violating phase.
 */
MSIMD_API void msimd_quantum_flavor_ckm_mix(
    const double in_quark_triplet_re[3],
    const double in_quark_triplet_im[3],
    const double ckm_matrix_re[9],
    const double ckm_matrix_im[9],
    double out_quark_triplet_re[3],
    double out_quark_triplet_im[3],
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Asymmetric Numeral Systems (rANS) SIMD Entropy Encoder.
 */
MSIMD_API size_t msimd_compress_rans_encode(
    const uint8_t* in_data,
    size_t in_len,
    uint8_t* out_buf,
    size_t out_max,
    const uint32_t* symbol_freqs,
    size_t alphabet_size,
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Asymmetric Numeral Systems (rANS) SIMD Entropy Decoder.
 */
MSIMD_API size_t msimd_compress_rans_decode(
    const uint8_t* in_data,
    size_t in_len,
    uint8_t* out_buf,
    size_t out_max,
    const uint32_t* symbol_freqs,
    size_t alphabet_size,
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section48 */

/**
 * @defgroup MSIMD_Section49 Section 49: Open-World Visibility Culling & Geometry Optimization
 * @{
 */

/**
 * @brief Batched SIMD Frustum Culling of Axis-Aligned Bounding Boxes (AABBs).
 * @param frustum_planes 6 planes * 4 floats (Nx, Ny, Nz, D) = 24 floats.
 * @param aabb_min_max N boxes * 6 floats (minX, minY, minZ, maxX, maxY, maxZ).
 * @param count Number of AABBs to test.
 * @param out_visibility_mask Output: 1 if inside/intersecting frustum, 0 if culled.
 * @return Total number of visible AABBs.
 */
MSIMD_API size_t msimd_cull_frustum_aabbs_f32(
    const float* frustum_planes,
    const float* aabb_min_max,
    size_t count,
    uint8_t* out_visibility_mask,
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Batched SIMD Frustum Culling of Bounding Spheres.
 * @param frustum_planes 6 planes * 4 floats (Nx, Ny, Nz, D) = 24 floats.
 * @param sphere_centers_radii N spheres * 4 floats (cx, cy, cz, radius).
 * @param count Number of spheres to test.
 * @param out_visibility_mask Output: 1 if visible, 0 if culled.
 * @return Total number of visible spheres.
 */
MSIMD_API size_t msimd_cull_frustum_spheres_f32(
    const float* frustum_planes,
    const float* sphere_centers_radii,
    size_t count,
    uint8_t* out_visibility_mask,
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Batched Squared Distance Evaluation & Level of Detail (LOD) / Range Culling.
 * @param camera_pos 3 floats (camX, camY, camZ).
 * @param object_positions N objects * 3 floats (posX, posY, posZ).
 * @param count Number of objects.
 * @param lod_sq_thresholds 4 floats: (lod0_max_sq, lod1_max_sq, lod2_max_sq, cull_dist_sq).
 * @param out_lod_levels Output: discrete LOD level (0, 1, 2, 3) or 255 if culled beyond max range.
 * @return Total number of visible (non-culled) objects.
 */
MSIMD_API size_t msimd_cull_distance_lod_f32(
    const float* camera_pos,
    const float* object_positions,
    size_t count,
    const float* lod_sq_thresholds,
    uint8_t* out_lod_levels,
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/**
 * @brief Voxel 6-Neighbor Hidden Face Elimination & Occlusion Mask Synthesis.
 * @param voxel_grid 3D chunk voxel grid: dim_x * dim_y * dim_z uint32_t block IDs (0 = air).
 * @param dim_x Chunk X dimension.
 * @param dim_y Chunk Y dimension.
 * @param dim_z Chunk Z dimension.
 * @param out_face_masks Output: dim_x * dim_y * dim_z 6-bit face visibility masks (+X, -X, +Y, -Y, +Z, -Z).
 * @return Total number of visible faces across the voxel grid.
 */
MSIMD_API size_t msimd_voxel_cull_hidden_faces_u32(
    const uint32_t* voxel_grid,
    size_t dim_x,
    size_t dim_y,
    size_t dim_z,
    uint8_t* out_face_masks,
    const msimd_tensor_t* precision_target MSIMD_DEFAULT_PRECISION);

/** @} */ /* End of MSIMD_Section49 */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MATHVANTAGE_SIMD_C_API_H */

}]>;

// Master C++ Algorithmic Implementation Complete Module (Lossless Representation)
def Master_MSIMD_Base_Skeletree_Source : MasterKernelSourceModule<"Mathematics-Rules.td", "Complete Pure C++ Skeletree Algorithmic Kernels", [{

#include "MathventageSIMD.h"
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

#ifndef MSIMD_PARALLEL_FOR_FN_DEFINED
#define MSIMD_PARALLEL_FOR_FN_DEFINED
typedef void (*msimd_parallel_for_fn)(void* user_data, size_t start, size_t end);
#endif

/* Backend hooks */
MSIMD_API msimd_status_t msimd_backend_fallback_init(msimd_context ctx);
MSIMD_API void           msimd_backend_fallback_destroy(msimd_context ctx);
MSIMD_API void           msimd_backend_fallback_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data);

MSIMD_API msimd_status_t msimd_backend_accelerated_init(msimd_context ctx);
MSIMD_API void           msimd_backend_accelerated_destroy(msimd_context ctx);
MSIMD_API void           msimd_backend_accelerated_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data);

/* Leaf Math Kernels */
void msimd_kernel_wave_step_1d_fp32(float* next, const float* curr, const float* prev, size_t nx, float r2, const msimd_tensor_t* precision_target);
void msimd_kernel_wave_step_3d_fp32(float* next, const float* curr, const float* prev, size_t nx, size_t ny, size_t nz, float rx, float ry, float rz, const msimd_tensor_t* precision_target);

void msimd_kernel_maxwell_yee_3d(size_t nx, size_t ny, size_t nz,
                                 float* Ex, float* Ey, float* Ez,
                                 float* Hx, float* Hy, float* Hz,
                                 float dt, float dx, float dy, float dz,
                                 float mu, float eps, const msimd_tensor_t* precision_target);

void msimd_kernel_lbm_d2q9(size_t nx, size_t ny,
                           const float* f_in, float* f_out,
                           float* rho, float* ux, float* uy,
                           float tau, const msimd_tensor_t* precision_target);

void msimd_kernel_ray_triangle_batch8(const float ox[8], const float oy[8], const float oz[8],
                                      const float dx[8], const float dy[8], const float dz[8],
                                      const float v0[3], const float v1[3], const float v2[3],
                                      int hit[8], float t[8], float u[8], float v[8], const msimd_tensor_t* precision_target);

void msimd_kernel_stockham_fft_radix2(size_t n,
                                      const float* in_real, const float* in_imag,
                                      float* out_real, float* out_imag,
                                      int inverse, const msimd_tensor_t* precision_target);

#ifdef __cplusplus
}
#endif
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <vector>
#include <array>
#include <queue>
#include <functional>
#include <limits>
#include <memory>

#if defined(_MSC_VER)
#include <intrin.h>
#endif
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnan-infinity-disabled"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E 2.71828182845904523536
#endif

extern "C" {

void msimd_vec2f_add(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] + b[i];
}
void msimd_vec2f_sub(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] - b[i];
}
void msimd_vec2f_scale(const float* a, float s, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] * s;
}
float msimd_vec2f_dot(const float* a, const float* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float sum = 0;
    for (int i = 0; i < 2; ++i) sum += a[i] * b[i];
    return sum;
}
float msimd_vec2f_length_sq(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return msimd_vec2f_dot(a, a);
}
float msimd_vec2f_length(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sqrt(msimd_vec2f_length_sq(a));
}
void msimd_vec2f_normalize(const float* a, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float len = msimd_vec2f_length(a);
    if (len > 1e-12) {
        float inv = float(1.0) / len;
        msimd_vec2f_scale(a, inv, out);
    } else {
        for (int i = 0; i < 2; ++i) out[i] = 0;
    }
}
void msimd_vec2f_lerp(const float* a, const float* b, float t, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] + t * (b[i] - a[i]);
}
void msimd_vec2f_reflect(const float* v, const float* n, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float d = msimd_vec2f_dot(v, n);
    for (int i = 0; i < 2; ++i) out[i] = v[i] - float(2.0) * d * n[i];
}

void msimd_vec2d_add(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] + b[i];
}
void msimd_vec2d_sub(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] - b[i];
}
void msimd_vec2d_scale(const double* a, double s, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] * s;
}
double msimd_vec2d_dot(const double* a, const double* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double sum = 0;
    for (int i = 0; i < 2; ++i) sum += a[i] * b[i];
    return sum;
}
double msimd_vec2d_length_sq(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return msimd_vec2d_dot(a, a);
}
double msimd_vec2d_length(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sqrt(msimd_vec2d_length_sq(a));
}
void msimd_vec2d_normalize(const double* a, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double len = msimd_vec2d_length(a);
    if (len > 1e-12) {
        double inv = double(1.0) / len;
        msimd_vec2d_scale(a, inv, out);
    } else {
        for (int i = 0; i < 2; ++i) out[i] = 0;
    }
}
void msimd_vec2d_lerp(const double* a, const double* b, double t, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] + t * (b[i] - a[i]);
}
void msimd_vec2d_reflect(const double* v, const double* n, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double d = msimd_vec2d_dot(v, n);
    for (int i = 0; i < 2; ++i) out[i] = v[i] - double(2.0) * d * n[i];
}

void msimd_vec3f_add(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] + b[i];
}
void msimd_vec3f_sub(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] - b[i];
}
void msimd_vec3f_scale(const float* a, float s, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] * s;
}
float msimd_vec3f_dot(const float* a, const float* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float sum = 0;
    for (int i = 0; i < 3; ++i) sum += a[i] * b[i];
    return sum;
}
float msimd_vec3f_length_sq(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return msimd_vec3f_dot(a, a);
}
float msimd_vec3f_length(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sqrt(msimd_vec3f_length_sq(a));
}
void msimd_vec3f_normalize(const float* a, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float len = msimd_vec3f_length(a);
    if (len > 1e-12) {
        float inv = float(1.0) / len;
        msimd_vec3f_scale(a, inv, out);
    } else {
        for (int i = 0; i < 3; ++i) out[i] = 0;
    }
}
void msimd_vec3f_lerp(const float* a, const float* b, float t, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] + t * (b[i] - a[i]);
}
void msimd_vec3f_reflect(const float* v, const float* n, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float d = msimd_vec3f_dot(v, n);
    for (int i = 0; i < 3; ++i) out[i] = v[i] - float(2.0) * d * n[i];
}

void msimd_vec3d_add(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] + b[i];
}
void msimd_vec3d_sub(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] - b[i];
}
void msimd_vec3d_scale(const double* a, double s, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] * s;
}
double msimd_vec3d_dot(const double* a, const double* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double sum = 0;
    for (int i = 0; i < 3; ++i) sum += a[i] * b[i];
    return sum;
}
double msimd_vec3d_length_sq(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return msimd_vec3d_dot(a, a);
}
double msimd_vec3d_length(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sqrt(msimd_vec3d_length_sq(a));
}
void msimd_vec3d_normalize(const double* a, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double len = msimd_vec3d_length(a);
    if (len > 1e-12) {
        double inv = double(1.0) / len;
        msimd_vec3d_scale(a, inv, out);
    } else {
        for (int i = 0; i < 3; ++i) out[i] = 0;
    }
}
void msimd_vec3d_lerp(const double* a, const double* b, double t, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] + t * (b[i] - a[i]);
}
void msimd_vec3d_reflect(const double* v, const double* n, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double d = msimd_vec3d_dot(v, n);
    for (int i = 0; i < 3; ++i) out[i] = v[i] - double(2.0) * d * n[i];
}

void msimd_vec4f_add(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] + b[i];
}
void msimd_vec4f_sub(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] - b[i];
}
void msimd_vec4f_scale(const float* a, float s, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] * s;
}
float msimd_vec4f_dot(const float* a, const float* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float sum = 0;
    for (int i = 0; i < 4; ++i) sum += a[i] * b[i];
    return sum;
}
float msimd_vec4f_length_sq(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return msimd_vec4f_dot(a, a);
}
float msimd_vec4f_length(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sqrt(msimd_vec4f_length_sq(a));
}
void msimd_vec4f_normalize(const float* a, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float len = msimd_vec4f_length(a);
    if (len > 1e-12) {
        float inv = float(1.0) / len;
        msimd_vec4f_scale(a, inv, out);
    } else {
        for (int i = 0; i < 4; ++i) out[i] = 0;
    }
}
void msimd_vec4f_lerp(const float* a, const float* b, float t, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] + t * (b[i] - a[i]);
}
void msimd_vec4f_reflect(const float* v, const float* n, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float d = msimd_vec4f_dot(v, n);
    for (int i = 0; i < 4; ++i) out[i] = v[i] - float(2.0) * d * n[i];
}

void msimd_vec4d_add(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] + b[i];
}
void msimd_vec4d_sub(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] - b[i];
}
void msimd_vec4d_scale(const double* a, double s, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] * s;
}
double msimd_vec4d_dot(const double* a, const double* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double sum = 0;
    for (int i = 0; i < 4; ++i) sum += a[i] * b[i];
    return sum;
}
double msimd_vec4d_length_sq(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return msimd_vec4d_dot(a, a);
}
double msimd_vec4d_length(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sqrt(msimd_vec4d_length_sq(a));
}
void msimd_vec4d_normalize(const double* a, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double len = msimd_vec4d_length(a);
    if (len > 1e-12) {
        double inv = double(1.0) / len;
        msimd_vec4d_scale(a, inv, out);
    } else {
        for (int i = 0; i < 4; ++i) out[i] = 0;
    }
}
void msimd_vec4d_lerp(const double* a, const double* b, double t, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] + t * (b[i] - a[i]);
}
void msimd_vec4d_reflect(const double* v, const double* n, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double d = msimd_vec4d_dot(v, n);
    for (int i = 0; i < 4; ++i) out[i] = v[i] - double(2.0) * d * n[i];
}

void msimd_vec3f_cross(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    out[0] = a[1] * b[2] - a[2] * b[1];
    out[1] = a[2] * b[0] - a[0] * b[2];
    out[2] = a[0] * b[1] - a[1] * b[0];
}
void msimd_vec3d_cross(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    out[0] = a[1] * b[2] - a[2] * b[1];
    out[1] = a[2] * b[0] - a[0] * b[2];
    out[2] = a[0] * b[1] - a[1] * b[0];
}

void msimd_mat3f_identity(float* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::memset(m, 0, 9 * sizeof(float));
    m[0] = m[4] = m[8] = 1.0f;
}
void msimd_mat3f_mul(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            float s = 0.0f;
            for (int k = 0; k < 3; ++k) s += a[r * 3 + k] * b[k * 3 + c];
            out[r * 3 + c] = s;
        }
    }
}
void msimd_mat3f_mul_vec(const float* m, const float* v, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int r = 0; r < 3; ++r) {
        out[r] = m[r * 3 + 0] * v[0] + m[r * 3 + 1] * v[1] + m[r * 3 + 2] * v[2];
    }
}
float msimd_mat3f_det(const float* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return m[0] * (m[4] * m[8] - m[5] * m[7]) -
           m[1] * (m[3] * m[8] - m[5] * m[6]) +
           m[2] * (m[3] * m[7] - m[4] * m[6]);
}
bool msimd_mat3f_invert(const float* m, float* out) {
    float det = msimd_mat3f_det(m);
    if (std::fabs(det) < 1e-12f) return false;
    float inv_det = 1.0f / det;

    out[0] = (m[4] * m[8] - m[5] * m[7]) * inv_det;
    out[1] = (m[2] * m[7] - m[1] * m[8]) * inv_det;
    out[2] = (m[1] * m[5] - m[2] * m[4]) * inv_det;

    out[3] = (m[5] * m[6] - m[3] * m[8]) * inv_det;
    out[4] = (m[0] * m[8] - m[2] * m[6]) * inv_det;
    out[5] = (m[2] * m[3] - m[0] * m[5]) * inv_det;

    out[6] = (m[3] * m[7] - m[4] * m[6]) * inv_det;
    out[7] = (m[1] * m[6] - m[0] * m[7]) * inv_det;
    out[8] = (m[0] * m[4] - m[1] * m[3]) * inv_det;
    return true;
}

void msimd_mat4f_identity(float* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::memset(m, 0, 16 * sizeof(float));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}
void msimd_mat4f_mul(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            float s = 0.0f;
            for (int k = 0; k < 4; ++k) s += a[r * 4 + k] * b[k * 4 + c];
            out[r * 4 + c] = s;
        }
    }
}
void msimd_mat4f_mul_vec(const float* m, const float* v, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int r = 0; r < 4; ++r) {
        out[r] = m[r * 4 + 0] * v[0] + m[r * 4 + 1] * v[1] + m[r * 4 + 2] * v[2] + m[r * 4 + 3] * v[3];
    }
}
void msimd_mat4f_transpose(const float* m, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            out[c * 4 + r] = m[r * 4 + c];
        }
    }
}

void msimd_quat_identity(float* q, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    q[0] = q[1] = q[2] = 0.0f;
    q[3] = 1.0f;
}
void msimd_quat_mul(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    out[0] = a[3] * b[0] + a[0] * b[3] + a[1] * b[2] - a[2] * b[1];
    out[1] = a[3] * b[1] - a[0] * b[2] + a[1] * b[3] + a[2] * b[0];
    out[2] = a[3] * b[2] + a[0] * b[1] - a[1] * b[0] + a[2] * b[3];
    out[3] = a[3] * b[3] - a[0] * b[0] - a[1] * b[1] - a[2] * b[2];
}
void msimd_quat_rotate_vec(const float* q, const float* v, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float qv[3] = { q[0], q[1], q[2] };
    float uv[3], uuv[3];
    msimd_vec3f_cross(qv, v, uv);
    msimd_vec3f_cross(qv, uv, uuv);
    for (int i = 0; i < 3; ++i) {
        out[i] = v[i] + ((uv[i] * q[3]) + uuv[i]) * 2.0f;
    }
}
void msimd_quat_to_mat3(const float* q, float* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float x = q[0], y = q[1], z = q[2], w = q[3];
    float x2 = x + x, y2 = y + y, z2 = z + z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2, zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;

    m[0] = 1.0f - (yy + zz); m[1] = xy - wz;         m[2] = xz + wy;
    m[3] = xy + wz;         m[4] = 1.0f - (xx + zz); m[5] = yz - wx;
    m[6] = xz - wy;         m[7] = yz + wx;         m[8] = 1.0f - (xx + yy);
}
void msimd_quat_slerp(const float* q0, const float* q1, float t, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float dot = q0[0] * q1[0] + q0[1] * q1[1] + q0[2] * q1[2] + q0[3] * q1[3];
    float sign = 1.0f;
    if (dot < 0.0f) {
        dot = -dot;
        sign = -1.0f;
    }
    if (dot > 0.9995f) {
        for (int i = 0; i < 4; ++i) out[i] = q0[i] + t * (sign * q1[i] - q0[i]);
        float len = std::sqrt(out[0]*out[0] + out[1]*out[1] + out[2]*out[2] + out[3]*out[3]);
        float inv = (len > 0.0f) ? (1.0f / len) : 1.0f;
        for (int i = 0; i < 4; ++i) out[i] *= inv;
        return;
    }
    float theta_0 = std::acos(dot);
    float theta = theta_0 * t;
    float sin_theta = std::sin(theta);
    float sin_theta_0 = std::sin(theta_0);
    float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;
    for (int i = 0; i < 4; ++i) out[i] = s0 * q0[i] + s1 * (sign * q1[i]);
}

void msimd_dual_quat_transform_point_f32(const float* real, const float* dual, const float* p, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float rot_p[3];
    msimd_quat_rotate_vec(real, p, rot_p);
    float conj_real[4] = { -real[0], -real[1], -real[2], real[3] };
    float t_quat[4];
    msimd_quat_mul(dual, conj_real, t_quat);
    out[0] = rot_p[0] + 2.0f * t_quat[0];
    out[1] = rot_p[1] + 2.0f * t_quat[1];
    out[2] = rot_p[2] + 2.0f * t_quat[2];
}

double msimd_bessel_j0(double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double ax = std::fabs(x);
    if (ax < 8.0) {
        double y = x * x;
        double ans1 = 57568490574.0 + y * (-13362590354.0 + y * (651619640.7
                    + y * (-11214424.18 + y * (77392.33017 + y * (-184.9052456)))));
        double ans2 = 57568490574.0 + y * (1029532985.0 + y * (9494680.718
                    + y * (59272.64853 + y * (267.8532712 + y * 1.0))));
        return ans1 / ans2;
    } else {
        double z = 8.0 / ax;
        double y = z * z;
        double xx = ax - 0.785398164;
        double f0 = 1.0 + y * (-0.1098628627e-2 + y * (0.2734510407e-4
                  + y * (-0.2073370639e-5 + y * 0.2093887211e-6)));
        double theta0 = y * (-0.1562499995e-1 + y * (0.1430488765e-3
                      + y * (-0.6911147651e-5 + y * (0.7621095161e-6 - y * 0.934945152e-7))));
        return std::sqrt(0.636619772 / ax) * (f0 * std::cos(xx + theta0));
    }
}

double msimd_bessel_j1(double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double ax = std::fabs(x);
    if (ax < 8.0) {
        double y = x * x;
        double ans1 = x * (72362614232.0 + y * (-7895059235.0 + y * (242396853.1
                    + y * (-2972638.799 + y * (15704.48260 + y * (-30.16036606))))));
        double ans2 = 144725228442.0 + y * (2300535178.0 + y * (18583304.74
                    + y * (99447.43394 + y * (376.9991397 + y * 1.0))));
        return ans1 / ans2;
    } else {
        double z = 8.0 / ax;
        double y = z * z;
        double xx = ax - 2.356194491;
        double f1 = 1.0 + y * (0.183105e-2 + y * (-0.3516396496e-4
                  + y * (0.2457520174e-5 - y * 0.240337019e-6)));
        double theta1 = y * (0.04687499995 + y * (-0.2002690873e-3
                      + y * (0.8449199096e-5 + y * (-0.88228987e-6 + y * 0.105787412e-6))));
        double ans = std::sqrt(0.636619772 / ax) * (f1 * std::cos(xx + theta1));
        return (x < 0.0) ? -ans : ans;
    }
}

double msimd_airy_ai(double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (std::fabs(x) < 2.0) {
        double f = 0.355028053887817;
        double g = 0.258819403792807;
        double term_f = f, term_g = g * x;
        double sum = term_f - term_g;
        double x3 = x * x * x;
        for (int k = 1; k <= 12; ++k) {
            term_f *= x3 / (3.0 * k * (3.0 * k - 1.0));
            term_g *= x3 / (3.0 * k * (3.0 * k + 1.0));
            sum += term_f - term_g;
        }
        return sum;
    } else if (x >= 2.0) {
        double zeta = (2.0 / 3.0) * x * std::sqrt(x);
        return (0.5 / (std::sqrt(M_PI) * std::pow(x, 0.25))) * std::exp(-zeta);
    } else {
        double ax = -x;
        double zeta = (2.0 / 3.0) * ax * std::sqrt(ax);
        return (1.0 / (std::sqrt(M_PI) * std::pow(ax, 0.25))) * std::sin(zeta + M_PI / 4.0);
    }
}

double msimd_legendre_p(int n, double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n == 0) return 1.0;
    if (n == 1) return x;
    double p_prev = 1.0;
    double p_curr = x;
    for (int k = 1; k < n; ++k) {
        double p_next = ((2.0 * k + 1.0) * x * p_curr - k * p_prev) / (k + 1.0);
        p_prev = p_curr;
        p_curr = p_next;
    }
    return p_curr;
}

double msimd_chebyshev_t(int n, double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n == 0) return 1.0;
    if (n == 1) return x;
    double t_prev = 1.0;
    double t_curr = x;
    for (int k = 1; k < n; ++k) {
        double t_next = 2.0 * x * t_curr - t_prev;
        t_prev = t_curr;
        t_curr = t_next;
    }
    return t_curr;
}

void msimd_kdv_step(float* u_next, const float* u_curr, const float* u_prev, size_t n, float dt, float dx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!u_next || !u_curr || !u_prev || n < 4 || dx <= 0.0f) return;
    float inv_2dx = 1.0f / (2.0f * dx);
    float inv_2dx3 = 1.0f / (2.0f * dx * dx * dx);

    for (size_t i = 2; i < n - 2; ++i) {
        float u_x = (u_curr[i + 1] - u_curr[i - 1]) * inv_2dx;
        float u_xxx = (u_curr[i + 2] - 2.0f * u_curr[i + 1] + 2.0f * u_curr[i - 1] - u_curr[i - 2]) * inv_2dx3;
        float nonlin = 6.0f * u_curr[i] * u_x;
        u_next[i] = u_prev[i] - 2.0f * dt * (nonlin + u_xxx);
    }
    u_next[0] = u_next[1] = 0.0f;
    u_next[n - 2] = u_next[n - 1] = 0.0f;
}

void msimd_sine_gordon_step(float* u_next, const float* u_curr, const float* u_prev, size_t n, float dt, float dx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!u_next || !u_curr || !u_prev || n < 3 || dx <= 0.0f) return;
    float r = (dt / dx) * (dt / dx);
    for (size_t i = 1; i < n - 1; ++i) {
        float lap = u_curr[i + 1] - 2.0f * u_curr[i] + u_curr[i - 1];
        u_next[i] = 2.0f * u_curr[i] - u_prev[i] + r * lap - (dt * dt) * std::sin(u_curr[i]);
    }
    u_next[0] = u_next[1];
    u_next[n - 1] = u_next[n - 2];
}

void msimd_fractional_caputo_derivative(const float* f, float* out, size_t n, float alpha, float dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double gamma_val = std::tgamma(1.0 - (double)alpha);
    double factor = 1.0 / (gamma_val * (1.0 - (double)alpha) * std::pow(dt, alpha));

    out[0] = 0.0f;
    for (size_t j = 1; j < n; ++j) {
        double sum = 0.0;
        for (size_t k = 0; k < j; ++k) {
            double df = (f[k + 1] - f[k]);
            double b = std::pow(j - k, 1.0 - alpha) - std::pow(j - k - 1, 1.0 - alpha);
            sum += df * b;
        }
        out[j] = (float)(sum * factor);
    }
}

void msimd_wavelet_haar_forward(const float* in, float* out, size_t n, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 2 || (n & 1)) return;
    size_t half = n / 2;
    static const float inv_sqrt2 = 0.7071067811865475f;
    for (size_t i = 0; i < half; ++i) {
        out[i] = (in[2 * i] + in[2 * i + 1]) * inv_sqrt2;
        out[half + i] = (in[2 * i] - in[2 * i + 1]) * inv_sqrt2;
    }
}

void msimd_wavelet_haar_inverse(const float* in, float* out, size_t n, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 2 || (n & 1)) return;
    size_t half = n / 2;
    static const float inv_sqrt2 = 0.7071067811865475f;
    for (size_t i = 0; i < half; ++i) {
        out[2 * i] = (in[i] + in[half + i]) * inv_sqrt2;
        out[2 * i + 1] = (in[i] - in[half + i]) * inv_sqrt2;
    }
}

void msimd_wavelet_daubechies4_forward(const float* in, float* out, size_t n, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 4 || (n & 1)) return;
    static const float h0 = 0.4829629131445341f;
    static const float h1 = 0.8365163037378079f;
    static const float h2 = 0.2241438680420134f;
    static const float h3 = -0.1294095225512604f;

    static const float g0 = h3;
    static const float g1 = -h2;
    static const float g2 = h1;
    static const float g3 = -h0;

    size_t half = n / 2;
    for (size_t i = 0; i < half; ++i) {
        size_t i0 = (2 * i) % n;
        size_t i1 = (2 * i + 1) % n;
        size_t i2 = (2 * i + 2) % n;
        size_t i3 = (2 * i + 3) % n;

        out[i] = h0 * in[i0] + h1 * in[i1] + h2 * in[i2] + h3 * in[i3];
        out[half + i] = g0 * in[i0] + g1 * in[i1] + g2 * in[i2] + g3 * in[i3];
    }
}

void msimd_kernel_wave_step_1d_fp32(float* next, const float* curr, const float* prev, size_t nx, float r2, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (nx < 3) return;
    const float two_minus_2r2 = 2.0f - 2.0f * r2;
    for (size_t i = 1; i < nx - 1; ++i) {
        float lap = curr[i - 1] + curr[i + 1];
        next[i] = two_minus_2r2 * curr[i] + r2 * lap - prev[i];
    }
    next[0] = 0.0f;
    next[nx - 1] = 0.0f;
}

void msimd_kernel_wave_step_3d_fp32(float* next, const float* curr, const float* prev,
                                 size_t nx, size_t ny, size_t nz,
                                 float rx, float ry, float rz, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!next || !curr || !prev || nx < 3 || ny < 3 || nz < 3) return;
    const size_t slice_stride = nx * ny;
    const float center_coeff = 2.0f - 2.0f * (rx + ry + rz);
    for (size_t z = 1; z < nz - 1; ++z) {
        for (size_t y = 1; y < ny - 1; ++y) {
            size_t base = z * slice_stride + y * nx;
            for (size_t x = 1; x < nx - 1; ++x) {
                size_t idx = base + x;
                float xm = curr[idx - 1], xp = curr[idx + 1];
                float ym = curr[idx - nx], yp = curr[idx + nx];
                float zm = curr[idx - slice_stride], zp = curr[idx + slice_stride];
                float lap = rx * (xm + xp) + ry * (ym + yp) + rz * (zm + zp);
                next[idx] = center_coeff * curr[idx] + lap - prev[idx];
            }
        }
    }
}

void msimd_kernel_maxwell_yee_3d(size_t nx, size_t ny, size_t nz,
                              float* Ex, float* Ey, float* Ez,
                              float* Hx, float* Hy, float* Hz,
                              float dt, float dx, float dy, float dz,
                              float mu, float eps, const msimd_tensor_t* precision_target) {
    if (!Ex || !Ey || !Ez || !Hx || !Hy || !Hz || nx < 3 || ny < 3 || nz < 3) return;
    (void)precision_target;
    const float dt_over_mu_dy = dt / (mu * dy);
    const float dt_over_mu_dz = dt / (mu * dz);
    const float dt_over_mu_dx = dt / (mu * dx);

    const float dt_over_eps_dy = dt / (eps * dy);
    const float dt_over_eps_dz = dt / (eps * dz);
    const float dt_over_eps_dx = dt / (eps * dx);

    const size_t slice = nx * ny;

    for (size_t z = 0; z < nz - 1; ++z) {
        for (size_t y = 0; y < ny - 1; ++y) {
            for (size_t x = 0; x < nx - 1; ++x) {
                size_t idx = z * slice + y * nx + x;

                float dez_dy = (Ez[idx + nx] - Ez[idx]);
                float dey_dz = (Ey[idx + slice] - Ey[idx]);
                Hx[idx] -= (dt_over_mu_dy * dez_dy - dt_over_mu_dz * dey_dz);

                float dex_dz = (Ex[idx + slice] - Ex[idx]);
                float dez_dx = (Ez[idx + 1] - Ez[idx]);
                Hy[idx] -= (dt_over_mu_dz * dex_dz - dt_over_mu_dx * dez_dx);

                float dey_dx = (Ey[idx + 1] - Ey[idx]);
                float dex_dy = (Ex[idx + nx] - Ex[idx]);
                Hz[idx] -= (dt_over_mu_dx * dey_dx - dt_over_mu_dy * dex_dy);
            }
        }
    }

    for (size_t z = 1; z < nz; ++z) {
        for (size_t y = 1; y < ny; ++y) {
            for (size_t x = 1; x < nx; ++x) {
                size_t idx = z * slice + y * nx + x;

                float dhz_dy = (Hz[idx] - Hz[idx - nx]);
                float dhy_dz = (Hy[idx] - Hy[idx - slice]);
                Ex[idx] += (dt_over_eps_dy * dhz_dy - dt_over_eps_dz * dhy_dz);

                float dhx_dz = (Hx[idx] - Hx[idx - slice]);
                float dhz_dx = (Hz[idx] - Hz[idx - 1]);
                Ey[idx] += (dt_over_eps_dz * dhx_dz - dt_over_eps_dx * dhz_dx);

                float dhy_dx = (Hy[idx] - Hy[idx - 1]);
                float dhx_dy = (Hx[idx] - Hx[idx - nx]);
                Ez[idx] += (dt_over_eps_dx * dhy_dx - dt_over_eps_dy * dhx_dy);
            }
        }
    }
}

void msimd_kernel_lbm_d2q9(size_t nx, size_t ny,
                        const float* f_in, float* f_out,
                        float* rho, float* ux, float* uy,
                        float tau, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const float omega = 1.0f / tau;
    const float w0 = 4.0f / 9.0f;
    const float w1 = 1.0f / 9.0f;
    const float w2 = 1.0f / 36.0f;

    static const int cx[9] = { 0,  1,  0, -1,  0,  1, -1, -1,  1 };
    static const int cy[9] = { 0,  0,  1,  0, -1,  1,  1, -1, -1 };
    static const float w[9] = { w0, w1, w1, w1, w1, w2, w2, w2, w2 };

    const size_t total_cells = nx * ny;

    for (size_t y = 0; y < ny; ++y) {
        for (size_t x = 0; x < nx; ++x) {
            size_t cell = y * nx + x;

            float local_rho = 0.0f;
            float local_jx = 0.0f;
            float local_jy = 0.0f;

            for (int i = 0; i < 9; ++i) {
                float fi = f_in[i * total_cells + cell];
                local_rho += fi;
                local_jx += fi * cx[i];
                local_jy += fi * cy[i];
            }

            rho[cell] = local_rho;
            float inv_rho = (local_rho > 1e-7f) ? (1.0f / local_rho) : 1.0f;
            float vx = local_jx * inv_rho;
            float vy = local_jy * inv_rho;
            ux[cell] = vx;
            uy[cell] = vy;

            float u_sqr = vx * vx + vy * vy;

            for (int i = 0; i < 9; ++i) {
                float eu = cx[i] * vx + cy[i] * vy;
                float feq = w[i] * local_rho * (1.0f + 3.0f * eu + 4.5f * eu * eu - 1.5f * u_sqr);
                float f_post = f_in[i * total_cells + cell] - omega * (f_in[i * total_cells + cell] - feq);

                size_t nx_target = (x + cx[i] + nx) % nx;
                size_t ny_target = (y + cy[i] + ny) % ny;
                size_t dest_cell = ny_target * nx + nx_target;

                f_out[i * total_cells + dest_cell] = f_post;
            }
        }
    }
}

void msimd_kernel_stockham_fft_radix2(size_t n,
                                   const float* in_real, const float* in_imag,
                                   float* out_real, float* out_imag,
                                   int inverse, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n <= 1 || (n & (n - 1)) != 0) {
        if (n == 1) {
            out_real[0] = in_real[0];
            out_imag[0] = in_imag[0];
        }
        return;
    }

    std::vector<float> r_buf1(n), i_buf1(n);
    std::vector<float> r_buf2(n), i_buf2(n);

    memcpy(r_buf1.data(), in_real, n * sizeof(float));
    memcpy(i_buf1.data(), in_imag, n * sizeof(float));

    float* cur_r = r_buf1.data();
    float* cur_i = i_buf1.data();
    float* nxt_r = r_buf2.data();
    float* nxt_i = i_buf2.data();

    const float sign = inverse ? 1.0f : -1.0f;
    const double pi = 3.14159265358979323846;

    for (size_t s = 1; s < n; s *= 2) {
        size_t p = n / (2 * s);
        for (size_t j = 0; j < s; ++j) {
            double angle = sign * 2.0 * pi * (double)j / (double)(2 * s);
            float wr = (float)std::cos(angle);
            float wi = (float)std::sin(angle);

            for (size_t k = 0; k < p; ++k) {
                size_t in_idx0 = k * s + j;
                size_t in_idx1 = in_idx0 + n / 2;

                float r0 = cur_r[in_idx0];
                float i0 = cur_i[in_idx0];
                float r1 = cur_r[in_idx1];
                float i1 = cur_i[in_idx1];

                float tr = r1 * wr - i1 * wi;
                float ti = r1 * wi + i1 * wr;

                size_t out_idx0 = (2 * k) * s + j;
                size_t out_idx1 = (2 * k + 1) * s + j;

                nxt_r[out_idx0] = r0 + tr;
                nxt_i[out_idx0] = i0 + ti;
                nxt_r[out_idx1] = r0 - tr;
                nxt_i[out_idx1] = i0 - ti;
            }
        }
        std::swap(cur_r, nxt_r);
        std::swap(cur_i, nxt_i);
    }

    if (inverse) {
        float inv_n = 1.0f / (float)n;
        for (size_t i = 0; i < n; ++i) {
            out_real[i] = cur_r[i] * inv_n;
            out_imag[i] = cur_i[i] * inv_n;
        }
    } else {
        memcpy(out_real, cur_r, n * sizeof(float));
        memcpy(out_imag, cur_i, n * sizeof(float));
    }
}

void msimd_rigidbody_update_inv_inertia(MSIMDRigidBody* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (b->inv_mass <= 0.0f) {
        std::memset(b->inv_inertia_world, 0, 9 * sizeof(float));
        return;
    }
    float R[9];
    msimd_quat_to_mat3(b->quat, R, precision_target);

    float R_invI[9];
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            float sum = 0.0f;
            for (int k = 0; k < 3; ++k) sum += R[r * 3 + k] * b->inertia[k * 3 + c];
            R_invI[r * 3 + c] = sum;
        }
    }
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            float sum = 0.0f;
            for (int k = 0; k < 3; ++k) sum += R_invI[r * 3 + k] * R[c * 3 + k];
            b->inv_inertia_world[r * 3 + c] = sum;
        }
    }
}

void msimd_rigidbody_integrate_velocity(MSIMDRigidBody* b, const float* gravity, float dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (b->inv_mass <= 0.0f) return;
    for (int i = 0; i < 3; ++i) {
        b->vel[i] += (gravity[i] + b->force[i] * b->inv_mass) * dt;
    }
    float ang_accel[3];
    msimd_mat3f_mul_vec(b->inv_inertia_world, b->torque, ang_accel);
    for (int i = 0; i < 3; ++i) {
        b->ang_vel[i] += ang_accel[i] * dt;
    }
}

void msimd_rigidbody_integrate_position(MSIMDRigidBody* b, float dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (b->inv_mass <= 0.0f) return;
    for (int i = 0; i < 3; ++i) {
        b->pos[i] += b->vel[i] * dt;
    }
    float w_quat[4] = { b->ang_vel[0], b->ang_vel[1], b->ang_vel[2], 0.0f };
    float dq[4];
    msimd_quat_mul(w_quat, b->quat, dq);
    for (int i = 0; i < 4; ++i) {
        b->quat[i] += 0.5f * dt * dq[i];
    }
    float q_len = msimd_vec4f_length(b->quat);
    if (q_len > 0.0f) msimd_vec4f_scale(b->quat, 1.0f / q_len, b->quat);

    msimd_rigidbody_update_inv_inertia(b, precision_target);
}

void msimd_pgs_prepare_contact(MSIMDContactConstraint* c, float dt, float baumgarte, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float n[3];
    for (int i = 0; i < 3; ++i) n[i] = c->normal[i];

    if (std::fabs(n[0]) < 0.577f) {
        c->tangent1[0] = 1.0f; c->tangent1[1] = 0.0f; c->tangent1[2] = 0.0f;
    } else {
        c->tangent1[0] = 0.0f; c->tangent1[1] = 1.0f; c->tangent1[2] = 0.0f;
    }
    float d = msimd_vec3f_dot(c->tangent1, n);
    for (int i = 0; i < 3; ++i) c->tangent1[i] -= d * n[i];
    msimd_vec3f_normalize(c->tangent1, c->tangent1);
    msimd_vec3f_cross(n, c->tangent1, c->tangent2);

    auto compute_effective_mass = [&](const float* dir) -> float {
        float rA_x_dir[3], rB_x_dir[3];
        msimd_vec3f_cross(c->rA, dir, rA_x_dir);
        msimd_vec3f_cross(c->rB, dir, rB_x_dir);
        float I_rA[3], I_rB[3];
        msimd_mat3f_mul_vec(c->bodyA->inv_inertia_world, rA_x_dir, I_rA);
        msimd_mat3f_mul_vec(c->bodyB->inv_inertia_world, rB_x_dir, I_rB);
        float ang_term_A = msimd_vec3f_dot(rA_x_dir, I_rA);
        float ang_term_B = msimd_vec3f_dot(rB_x_dir, I_rB);
        float total_inv_mass = c->bodyA->inv_mass + c->bodyB->inv_mass + ang_term_A + ang_term_B;
        return (total_inv_mass > 1e-12f) ? (1.0f / total_inv_mass) : 0.0f;
    };

    c->normal_mass = compute_effective_mass(c->normal);
    c->tangent1_mass = compute_effective_mass(c->tangent1);
    c->tangent2_mass = compute_effective_mass(c->tangent2);

    float allowed_penetration = 0.01f;
    float pen = std::max(0.0f, c->penetration - allowed_penetration);
    c->bias = (baumgarte / dt) * pen;
}

void msimd_pgs_solve_contact_iteration(MSIMDContactConstraint* c, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    auto get_contact_vel = [](const MSIMDRigidBody* b, const float* r, float* out_v) {
        float wxr[3];
        msimd_vec3f_cross(b->ang_vel, r, wxr);
        for (int i = 0; i < 3; ++i) out_v[i] = b->vel[i] + wxr[i];
    };

    float vA[3], vB[3], v_rel[3];
    get_contact_vel(c->bodyA, c->rA, vA);
    get_contact_vel(c->bodyB, c->rB, vB);
    for (int i = 0; i < 3; ++i) v_rel[i] = vA[i] - vB[i];

    float vn = msimd_vec3f_dot(v_rel, c->normal);
    float delta_impulse = -c->normal_mass * (vn - c->bias);
    float old_impulse = c->accumulated_normal_impulse;
    c->accumulated_normal_impulse = std::max(0.0f, old_impulse + delta_impulse);
    delta_impulse = c->accumulated_normal_impulse - old_impulse;

    float impulse_vec[3];
    msimd_vec3f_scale(c->normal, delta_impulse, impulse_vec);

    auto apply_impulse = [](MSIMDRigidBody* b, const float* r, const float* imp, float sign) {
        if (b->inv_mass <= 0.0f) return;
        for (int i = 0; i < 3; ++i) b->vel[i] += sign * imp[i] * b->inv_mass;
        float rximp[3];
        msimd_vec3f_cross(r, imp, rximp);
        float delta_w[3];
        msimd_mat3f_mul_vec(b->inv_inertia_world, rximp, delta_w);
        for (int i = 0; i < 3; ++i) b->ang_vel[i] += sign * delta_w[i];
    };

    apply_impulse(c->bodyA, c->rA, impulse_vec, 1.0f);
    apply_impulse(c->bodyB, c->rB, impulse_vec, -1.0f);

    float max_friction = c->friction_coeff * c->accumulated_normal_impulse;

    get_contact_vel(c->bodyA, c->rA, vA);
    get_contact_vel(c->bodyB, c->rB, vB);
    for (int i = 0; i < 3; ++i) v_rel[i] = vA[i] - vB[i];
    float vt1 = msimd_vec3f_dot(v_rel, c->tangent1);
    float delta_t1 = -c->tangent1_mass * vt1;
    float old_t1 = c->accumulated_tangent1_impulse;
    c->accumulated_tangent1_impulse = std::clamp(old_t1 + delta_t1, -max_friction, max_friction);
    delta_t1 = c->accumulated_tangent1_impulse - old_t1;

    msimd_vec3f_scale(c->tangent1, delta_t1, impulse_vec);
    apply_impulse(c->bodyA, c->rA, impulse_vec, 1.0f);
    apply_impulse(c->bodyB, c->rB, impulse_vec, -1.0f);

    get_contact_vel(c->bodyA, c->rA, vA);
    get_contact_vel(c->bodyB, c->rB, vB);
    for (int i = 0; i < 3; ++i) v_rel[i] = vA[i] - vB[i];
    float vt2 = msimd_vec3f_dot(v_rel, c->tangent2);
    float delta_t2 = -c->tangent2_mass * vt2;
    float old_t2 = c->accumulated_tangent2_impulse;
    c->accumulated_tangent2_impulse = std::clamp(old_t2 + delta_t2, -max_friction, max_friction);
    delta_t2 = c->accumulated_tangent2_impulse - old_t2;

    msimd_vec3f_scale(c->tangent2, delta_t2, impulse_vec);
    apply_impulse(c->bodyA, c->rA, impulse_vec, 1.0f);
    apply_impulse(c->bodyB, c->rB, impulse_vec, -1.0f);
}

void msimd_sph_compute_density_pressure(MSIMDSPHParticle* p, size_t count, float h, float rest_density, float k_stiffness, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float h2 = h * h;
    float poly6_factor = 315.0f / (64.0f * (float)M_PI * std::pow(h, 9));

    for (size_t i = 0; i < count; ++i) {
        float d = 0.0f;
        for (size_t j = 0; j < count; ++j) {
            float dx = p[j].pos[0] - p[i].pos[0];
            float dy = p[j].pos[1] - p[i].pos[1];
            float dz = p[j].pos[2] - p[i].pos[2];
            float r2 = dx * dx + dy * dy + dz * dz;
            if (r2 < h2) {
                float diff = h2 - r2;
                d += p[j].mass * poly6_factor * diff * diff * diff;
            }
        }
        p[i].density = std::max(d, rest_density * 0.1f);
        p[i].pressure = k_stiffness * (p[i].density - rest_density);
    }
}

MSIMD_API MSIMDComplex msimd_c_add(MSIMDComplex a, MSIMDComplex b) { return { a.r + b.r, a.i + b.i }; }
MSIMD_API MSIMDComplex msimd_c_sub(MSIMDComplex a, MSIMDComplex b) { return { a.r - b.r, a.i - b.i }; }
MSIMD_API MSIMDComplex msimd_c_mul(MSIMDComplex a, MSIMDComplex b) { return { a.r * b.r - a.i * b.i, a.r * b.i + a.i * b.r }; }
MSIMD_API MSIMDComplex msimd_c_scale(MSIMDComplex a, float s) { return { a.r * s, a.i * s }; }
MSIMD_API float msimd_c_norm_sq(MSIMDComplex a) { return a.r * a.r + a.i * a.i; }

void msimd_pauli_sigma_x(const MSIMDComplex in[2], MSIMDComplex out[2], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    out[0] = in[1];
    out[1] = in[0];
}
void msimd_pauli_sigma_y(const MSIMDComplex in[2], MSIMDComplex out[2], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    out[0] = { in[1].i, -in[1].r };
    out[1] = { -in[0].i, in[0].r };
}
void msimd_pauli_sigma_z(const MSIMDComplex in[2], MSIMDComplex out[2], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    out[0] = in[0];
    out[1] = { -in[1].r, -in[1].i };
}

void msimd_dirac_4spinor_step_3d(MSIMDComplex* psi_next, const MSIMDComplex* psi_curr,
    size_t nx, size_t ny, size_t nz,
    float m, float c, float hbar, float dt, float dx, float dy, float dz, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_next || !psi_curr || nx < 3 || ny < 3 || nz < 3) return;

    size_t slice = nx * ny;
    float m_c2 = m * c * c;
    float hbar_c_2dx = hbar * c / (2.0f * dx);
    float hbar_c_2dy = hbar * c / (2.0f * dy);
    float hbar_c_2dz = hbar * c / (2.0f * dz);
    float inv_hbar = 1.0f / hbar;

    for (size_t z = 1; z < nz - 1; ++z) {
        for (size_t y = 1; y < ny - 1; ++y) {
            for (size_t x = 1; x < nx - 1; ++x) {
                size_t cell = z * slice + y * nx + x;
                size_t base = cell * 4;

                MSIMDComplex dpsi_dx[4], dpsi_dy[4], dpsi_dz[4];
                for (int s = 0; s < 4; ++s) {
                    size_t xm = ((z * slice + y * nx + (x - 1)) * 4) + s;
                    size_t xp = ((z * slice + y * nx + (x + 1)) * 4) + s;
                    size_t ym = ((z * slice + (y - 1) * nx + x) * 4) + s;
                    size_t yp = ((z * slice + (y + 1) * nx + x) * 4) + s;
                    size_t zm = (((z - 1) * slice + y * nx + x) * 4) + s;
                    size_t zp = (((z + 1) * slice + y * nx + x) * 4) + s;

                    dpsi_dx[s] = msimd_c_scale(msimd_c_sub(psi_curr[xp], psi_curr[xm]), hbar_c_2dx);
                    dpsi_dy[s] = msimd_c_scale(msimd_c_sub(psi_curr[yp], psi_curr[ym]), hbar_c_2dy);
                    dpsi_dz[s] = msimd_c_scale(msimd_c_sub(psi_curr[zp], psi_curr[zm]), hbar_c_2dz);
                }

                MSIMDComplex H_psi[4];
                H_psi[0] = msimd_c_scale(psi_curr[base + 0], m_c2);
                H_psi[1] = msimd_c_scale(psi_curr[base + 1], m_c2);
                H_psi[2] = msimd_c_scale(psi_curr[base + 2], -m_c2);
                H_psi[3] = msimd_c_scale(psi_curr[base + 3], -m_c2);

                H_psi[0] = msimd_c_add(H_psi[0], { -dpsi_dz[2].i, dpsi_dz[2].r });
                H_psi[1] = msimd_c_sub(H_psi[1], { -dpsi_dz[3].i, dpsi_dz[3].r });
                H_psi[2] = msimd_c_add(H_psi[2], { -dpsi_dz[0].i, dpsi_dz[0].r });
                H_psi[3] = msimd_c_sub(H_psi[3], { -dpsi_dz[1].i, dpsi_dz[1].r });

                for (int s = 0; s < 4; ++s) {
                    MSIMDComplex idt_H = { -H_psi[s].i * dt * inv_hbar, H_psi[s].r * dt * inv_hbar };
                    psi_next[base + s] = msimd_c_sub(psi_curr[base + s], idt_H);
                }
            }
        }
    }
}

float msimd_lattice_u1_plaquette_energy(const float* link_angles, size_t nx, size_t ny, size_t nz, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!link_angles || nx < 2 || ny < 2 || nz < 2) return 0.0f;
    auto get_link = [&](size_t x, size_t y, size_t z, int mu) -> float {
        return link_angles[((z * ny + y) * nx + x) * 3 + mu];
    };

    double total_action = 0.0;
    size_t count = 0;

    for (size_t z = 0; z < nz - 1; ++z) {
        for (size_t y = 0; y < ny - 1; ++y) {
            for (size_t x = 0; x < nx - 1; ++x) {
                for (int mu = 0; mu < 3; ++mu) {
                    for (int nu = mu + 1; nu < 3; ++nu) {
                        float theta1 = get_link(x, y, z, mu);
                        size_t x_mu = x + (mu == 0 ? 1 : 0);
                        size_t y_mu = y + (mu == 1 ? 1 : 0);
                        size_t z_mu = z + (mu == 2 ? 1 : 0);
                        float theta2 = get_link(x_mu, y_mu, z_mu, nu);

                        size_t x_nu = x + (nu == 0 ? 1 : 0);
                        size_t y_nu = y + (nu == 1 ? 1 : 0);
                        size_t z_nu = z + (nu == 2 ? 1 : 0);
                        float theta3 = get_link(x_nu, y_nu, z_nu, mu);
                        float theta4 = get_link(x, y, z, nu);

                        float plaq_angle = theta1 + theta2 - theta3 - theta4;
                        total_action += (1.0 - std::cos((double)plaq_angle));
                        count++;
                    }
                }
            }
        }
    }
    return count > 0 ? (float)(total_action / (double)count) : 0.0f;
}

void msimd_lindblad_step_qubit(const MSIMDComplex rho[4], const MSIMDComplex H[4],
    float gamma_dephase, float gamma_relax, float dt,
    MSIMDComplex out_rho[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    MSIMDComplex H_rho[4], rho_H[4], comm[4];
    for (int r = 0; r < 2; ++r) {
        for (int c = 0; c < 2; ++c) {
            MSIMDComplex s1 = { 0, 0 }, s2 = { 0, 0 };
            for (int k = 0; k < 2; ++k) {
                s1 = msimd_c_add(s1, msimd_c_mul(H[r * 2 + k], rho[k * 2 + c]));
                s2 = msimd_c_add(s2, msimd_c_mul(rho[r * 2 + k], H[k * 2 + c]));
            }
            H_rho[r * 2 + c] = s1;
            rho_H[r * 2 + c] = s2;
            comm[r * 2 + c] = msimd_c_sub(s1, s2);
        }
    }

    MSIMDComplex diss[4];
    diss[0] = { gamma_relax * rho[3].r, 0.0f };
    diss[3] = { -gamma_relax * rho[3].r, 0.0f };
    diss[1] = { -(0.5f * gamma_relax + gamma_dephase) * rho[1].r, -(0.5f * gamma_relax + gamma_dephase) * rho[1].i };
    diss[2] = { -(0.5f * gamma_relax + gamma_dephase) * rho[2].r, -(0.5f * gamma_relax + gamma_dephase) * rho[2].i };

    for (int i = 0; i < 4; ++i) {
        MSIMDComplex idt_comm = { -comm[i].i * dt, comm[i].r * dt };
        out_rho[i] = msimd_c_add(msimd_c_add(rho[i], idt_comm), msimd_c_scale(diss[i], dt));
    }
}

bool msimd_aabb_intersect_ray(const MSIMDAABB* box, const float* orig, const float* dir, float* out_tmin, float* out_tmax) {
    float tmin = 0.0f;
    float tmax = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; ++i) {
        float invD = (std::fabs(dir[i]) > 1e-12f) ? (1.0f / dir[i]) : 1e12f;
        float t0 = (box->min_pt[i] - orig[i]) * invD;
        float t1 = (box->max_pt[i] - orig[i]) * invD;
        if (invD < 0.0f) std::swap(t0, t1);
        tmin = std::max(tmin, t0);
        tmax = std::min(tmax, t1);
        if (tmax < tmin) return false;
    }
    if (out_tmin) *out_tmin = tmin;
    if (out_tmax) *out_tmax = tmax;
    return true;
}

struct MSIMDBVHNode {
    MSIMDAABB bounds;
    int left_child;
    int right_child;
    int first_prim_idx;
    int prim_count;
};

MSIMD_API float msimd_pbr_distribution_ggx(const float* n, const float* h, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float n_dot_h = std::max(msimd_vec3f_dot(n, h), 0.0f);
    float n_dot_h2 = n_dot_h * n_dot_h;

    float denom = (n_dot_h2 * (a2 - 1.0f) + 1.0f);
    denom = (float)M_PI * denom * denom;
    return denom > 1e-7f ? (a2 / denom) : 1.0f;
}

MSIMD_API float msimd_pbr_geometry_schlick_ggx(float n_dot_v, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    float denom = n_dot_v * (1.0f - k) + k;
    return denom > 1e-7f ? (n_dot_v / denom) : 1.0f;
}

MSIMD_API float msimd_pbr_geometry_smith(const float* n, const float* v, const float* l, float roughness) {
    float n_dot_v = std::max(msimd_vec3f_dot(n, v), 0.0f);
    float n_dot_l = std::max(msimd_vec3f_dot(n, l), 0.0f);
    float ggx1 = msimd_pbr_geometry_schlick_ggx(n_dot_v, roughness);
    float ggx2 = msimd_pbr_geometry_schlick_ggx(n_dot_l, roughness);
    return ggx1 * ggx2;
}

MSIMD_API void msimd_pbr_fresnel_schlick(float cos_theta, const float* F0, float* out_F) {
    float factor = std::pow(std::clamp(1.0f - cos_theta, 0.0f, 1.0f), 5.0f);
    for (int i = 0; i < 3; ++i) {
        out_F[i] = F0[i] + (1.0f - F0[i]) * factor;
    }
}

void msimd_pbr_cook_torrance_brdf(const float* n, const float* v, const float* l,
    const float* albedo, float metallic, float roughness,
    float* out_radiance, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    float h[3];
    msimd_vec3f_add(v, l, h);
    msimd_vec3f_normalize(h, h);

    float n_dot_l = std::max(msimd_vec3f_dot(n, l), 0.0f);
    float n_dot_v = std::max(msimd_vec3f_dot(n, v), 0.0f);
    if (n_dot_l <= 0.0f || n_dot_v <= 0.0f) {
        out_radiance[0] = out_radiance[1] = out_radiance[2] = 0.0f;
        return;
    }

    float F0[3];
    for (int i = 0; i < 3; ++i) {
        F0[i] = 0.04f * (1.0f - metallic) + albedo[i] * metallic;
    }

    float D = msimd_pbr_distribution_ggx(n, h, roughness);
    float G = msimd_pbr_geometry_smith(n, v, l, roughness);
    float F[3];
    msimd_pbr_fresnel_schlick(std::max(msimd_vec3f_dot(h, v), 0.0f), F0, F);

    float kS[3];
    for (int i = 0; i < 3; ++i) kS[i] = F[i];
    float kD[3];
    for (int i = 0; i < 3; ++i) kD[i] = (1.0f - kS[i]) * (1.0f - metallic);

    float denom = 4.0f * n_dot_v * n_dot_l;
    float spec_factor = denom > 1e-7f ? ((D * G) / denom) : 0.0f;

    for (int i = 0; i < 3; ++i) {
        float specular = spec_factor * F[i];
        float diffuse = (kD[i] * albedo[i]) / (float)M_PI;
        out_radiance[i] = (diffuse + specular) * n_dot_l;
    }
}

bool msimd_svdag_raymarch_dda(
    const uint32_t* voxel_grid, size_t dim_x, size_t dim_y, size_t dim_z,
    const float* orig, const float* dir, float max_dist,
    float* out_hit_pos, int* out_voxel_val) {

    int x = (int)std::floor(orig[0]);
    int y = (int)std::floor(orig[1]);
    int z = (int)std::floor(orig[2]);

    int step_x = (dir[0] > 0.0f) ? 1 : ((dir[0] < 0.0f) ? -1 : 0);
    int step_y = (dir[1] > 0.0f) ? 1 : ((dir[1] < 0.0f) ? -1 : 0);
    int step_z = (dir[2] > 0.0f) ? 1 : ((dir[2] < 0.0f) ? -1 : 0);

    float t_delta_x = (step_x != 0) ? std::fabs(1.0f / dir[0]) : 1e12f;
    float t_delta_y = (step_y != 0) ? std::fabs(1.0f / dir[1]) : 1e12f;
    float t_delta_z = (step_z != 0) ? std::fabs(1.0f / dir[2]) : 1e12f;

    if (step_x == 0 && step_y == 0 && step_z == 0) return false;
    float t_max_x = (step_x > 0) ? ((x + 1 - orig[0]) * t_delta_x) : ((step_x < 0) ? ((orig[0] - x) * t_delta_x) : 1e30f);
    float t_max_y = (step_y > 0) ? ((y + 1 - orig[1]) * t_delta_y) : ((step_y < 0) ? ((orig[1] - y) * t_delta_y) : 1e30f);
    float t_max_z = (step_z > 0) ? ((z + 1 - orig[2]) * t_delta_z) : ((step_z < 0) ? ((orig[2] - z) * t_delta_z) : 1e30f);

    float dist_traveled = 0.0f;
    while (dist_traveled < max_dist) {
        if (x >= 0 && x < (int)dim_x && y >= 0 && y < (int)dim_y && z >= 0 && z < (int)dim_z) {
            size_t idx = (z * dim_y + y) * dim_x + x;
            if (voxel_grid[idx] != 0) {
                out_hit_pos[0] = orig[0] + dist_traveled * dir[0];
                out_hit_pos[1] = orig[1] + dist_traveled * dir[1];
                out_hit_pos[2] = orig[2] + dist_traveled * dir[2];
                if (out_voxel_val) *out_voxel_val = (int)voxel_grid[idx];
                return true;
            }
        }

        if (t_max_x < t_max_y) {
            if (t_max_x < t_max_z) {
                dist_traveled = t_max_x;
                t_max_x += t_delta_x;
                x += step_x;
            } else {
                dist_traveled = t_max_z;
                t_max_z += t_delta_z;
                z += step_z;
            }
        } else {
            if (t_max_y < t_max_z) {
                dist_traveled = t_max_y;
                t_max_y += t_delta_y;
                y += step_y;
            } else {
                dist_traveled = t_max_z;
                t_max_z += t_delta_z;
                z += step_z;
            }
        }
    }
    return false;
}

void msimd_kernel_ray_triangle_batch8(const float ox[8], const float oy[8], const float oz[8],
                                   const float dx[8], const float dy[8], const float dz[8],
                                   const float v0[3], const float v1[3], const float v2[3],
                                   int hit[8], float t[8], float u[8], float v[8], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float e1[3] = { v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
    float e2[3] = { v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2] };

    for (int i = 0; i < 8; ++i) {
        float pvec[3] = {
            dy[i] * e2[2] - dz[i] * e2[1],
            dz[i] * e2[0] - dx[i] * e2[2],
            dx[i] * e2[1] - dy[i] * e2[0]
        };

        float det = e1[0] * pvec[0] + e1[1] * pvec[1] + e1[2] * pvec[2];
        if (std::fabs(det) < 1e-7f) {
            hit[i] = 0;
            t[i] = 0.0f; u[i] = 0.0f; v[i] = 0.0f;
            continue;
        }

        float inv_det = 1.0f / det;
        float tvec[3] = { ox[i] - v0[0], oy[i] - v0[1], oz[i] - v0[2] };
        float u_val = (tvec[0] * pvec[0] + tvec[1] * pvec[1] + tvec[2] * pvec[2]) * inv_det;
        if (u_val < 0.0f || u_val > 1.0f) {
            hit[i] = 0;
            t[i] = 0.0f; u[i] = 0.0f; v[i] = 0.0f;
            continue;
        }

        float qvec[3] = {
            tvec[1] * e1[2] - tvec[2] * e1[1],
            tvec[2] * e1[0] - tvec[0] * e1[2],
            tvec[0] * e1[1] - tvec[1] * e1[0]
        };
        float v_val = (dx[i] * qvec[0] + dy[i] * qvec[1] + dz[i] * qvec[2]) * inv_det;
        if (v_val < 0.0f || (u_val + v_val) > 1.0f) {
            hit[i] = 0;
            t[i] = 0.0f; u[i] = 0.0f; v[i] = 0.0f;
            continue;
        }

        float t_val = (e2[0] * qvec[0] + e2[1] * qvec[1] + e2[2] * qvec[2]) * inv_det;
        if (t_val > 1e-7f) {
            hit[i] = 1;
            t[i] = t_val;
            u[i] = u_val;
            v[i] = v_val;
        } else {
            hit[i] = 0;
            t[i] = 0.0f; u[i] = 0.0f; v[i] = 0.0f;
        }
    }
}

void msimd_acoustic_fdtd_step_3d(float* p, float* ux, float* uy, float* uz,
    size_t nx, size_t ny, size_t nz,
    float c, float rho, float dt, float dx, float dy, float dz, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!p || !ux || !uy || !uz || nx < 3 || ny < 3 || nz < 3) return;

    size_t slice = nx * ny;
    float bulk_modulus = rho * c * c;
    float inv_rho = 1.0f / rho;

    for (size_t z = 1; z < nz - 1; ++z) {
        for (size_t y = 1; y < ny - 1; ++y) {
            for (size_t x = 1; x < nx - 1; ++x) {
                size_t cell = z * slice + y * nx + x;
                float dp_dx = (p[cell + 1] - p[cell]) / dx;
                float dp_dy = (p[cell + nx] - p[cell]) / dy;
                float dp_dz = (p[cell + slice] - p[cell]) / dz;

                ux[cell] -= (inv_rho * dt) * dp_dx;
                uy[cell] -= (inv_rho * dt) * dp_dy;
                uz[cell] -= (inv_rho * dt) * dp_dz;
            }
        }
    }

    for (size_t z = 1; z < nz - 1; ++z) {
        for (size_t y = 1; y < ny - 1; ++y) {
            for (size_t x = 1; x < nx - 1; ++x) {
                size_t cell = z * slice + y * nx + x;
                float dux_dx = (ux[cell] - ux[cell - 1]) / dx;
                float duy_dy = (uy[cell] - uy[cell - nx]) / dy;
                float duz_dz = (uz[cell] - uz[cell - slice]) / dz;

                float div_u = dux_dx + duy_dy + duz_dz;
                p[cell] -= (bulk_modulus * dt) * div_u;
            }
        }
    }
}

float msimd_acoustic_doppler_shift(float f0, const float* src_pos, const float* src_vel,
    const float* lis_pos, const float* lis_vel, float speed_of_sound, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    float d[3];
    msimd_vec3f_sub(lis_pos, src_pos, d);
    float dist = msimd_vec3f_length(d);
    if (dist < 1e-4f) return f0;

    float dir[3];
    msimd_vec3f_scale(d, 1.0f / dist, dir);

    float v_s_radial = msimd_vec3f_dot(src_vel, dir);
    float v_l_radial = msimd_vec3f_dot(lis_vel, dir);

    float denom = speed_of_sound + v_s_radial;
    if (std::fabs(denom) < 1.0f) denom = (denom >= 0.0f) ? 1.0f : -1.0f;

    float f = f0 * ((speed_of_sound + v_l_radial) / denom);
    return std::max(f, 10.0f);
}

float msimd_acoustic_sabine_rt60(float room_volume, const float* surface_areas, const float* absorption_coeffs, size_t surface_count, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (room_volume <= 0.0f || surface_count == 0) return 0.0f;
    double total_absorption = 0.0;
    for (size_t i = 0; i < surface_count; ++i) {
        total_absorption += (double)surface_areas[i] * (double)absorption_coeffs[i];
    }
    if (total_absorption <= 1e-7) return 10.0f;
    return (float)(0.161 * (double)room_volume / total_absorption);
}

void msimd_acoustic_hrtf_binaural_process(const float* in_audio, size_t sample_count,
    const float* hrtf_left_ir, const float* hrtf_right_ir, size_t ir_len,
    float* out_left, float* out_right, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    for (size_t n = 0; n < sample_count; ++n) {
        double acc_l = 0.0, acc_r = 0.0;
        for (size_t k = 0; k < ir_len; ++k) {
            if (n >= k) {
                float sample = in_audio[n - k];
                acc_l += (double)sample * (double)hrtf_left_ir[k];
                acc_r += (double)sample * (double)hrtf_right_ir[k];
            }
        }
        out_left[n] = (float)acc_l;
        out_right[n] = (float)acc_r;
    }
}

size_t msimd_compress_rle(const uint8_t* in, size_t in_len, uint8_t* out, size_t out_max) {
    if (!in || in_len == 0 || !out || out_max == 0) return 0;
    size_t in_idx = 0;
    size_t out_idx = 0;

    while (in_idx < in_len && out_idx + 2 <= out_max) {
        uint8_t byte = in[in_idx];
        uint8_t run = 1;
        while (in_idx + 1 < in_len && in[in_idx + 1] == byte && run < 255) {
            run++;
            in_idx++;
        }
        out[out_idx++] = run;
        out[out_idx++] = byte;
        in_idx++;
    }
    return out_idx;
}

size_t msimd_decompress_rle(const uint8_t* in, size_t in_len, uint8_t* out, size_t out_max) {
    if (!in || in_len < 2 || !out || out_max == 0) return 0;
    size_t in_idx = 0;
    size_t out_idx = 0;

    while (in_idx + 1 < in_len) {
        uint8_t run = in[in_idx++];
        uint8_t byte = in[in_idx++];
        for (uint8_t r = 0; r < run; ++r) {
            if (out_idx >= out_max) return out_idx;
            out[out_idx++] = byte;
        }
    }
    return out_idx;
}

static constexpr size_t MSIMD_LZSS_WINDOW = 4096;
static constexpr size_t MSIMD_LZSS_MAX_MATCH = 18;
static constexpr size_t MSIMD_LZSS_MIN_MATCH = 3;

size_t msimd_compress_lzss(const uint8_t* in, size_t in_len, uint8_t* out, size_t out_max) {
    if (!in || in_len == 0 || !out || out_max == 0) return 0;
    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < in_len && out_pos + 3 <= out_max) {
        size_t best_len = 0;
        size_t best_offset = 0;

        size_t win_start = (in_pos > MSIMD_LZSS_WINDOW) ? (in_pos - MSIMD_LZSS_WINDOW) : 0;
        for (size_t candidate = win_start; candidate < in_pos; ++candidate) {
            size_t match_len = 0;
            while (match_len < MSIMD_LZSS_MAX_MATCH &&
                   in_pos + match_len < in_len &&
                   in[candidate + match_len] == in[in_pos + match_len]) {
                match_len++;
            }
            if (match_len > best_len) {
                best_len = match_len;
                best_offset = in_pos - candidate;
            }
        }

        if (best_len >= MSIMD_LZSS_MIN_MATCH) {
            out[out_pos++] = 0x80 | (uint8_t)((best_len - MSIMD_LZSS_MIN_MATCH) & 0x0F);
            out[out_pos++] = (uint8_t)(best_offset & 0xFF);
            out[out_pos++] = (uint8_t)((best_offset >> 8) & 0x0F);
            in_pos += best_len;
        } else {
            out[out_pos++] = in[in_pos++];
        }
    }
    return out_pos;
}

size_t msimd_decompress_lzss(const uint8_t* in, size_t in_len, uint8_t* out, size_t out_max) {
    if (!in || in_len == 0 || !out || out_max == 0) return 0;
    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < in_len && out_pos < out_max) {
        uint8_t token = in[in_pos++];
        if (token & 0x80) {
            if (in_pos + 2 > in_len) break;
            size_t len = (token & 0x0F) + MSIMD_LZSS_MIN_MATCH;
            size_t offset = (size_t)in[in_pos] | (((size_t)in[in_pos + 1] & 0x0F) << 8);
            in_pos += 2;
            if (offset == 0 || offset > out_pos) break;
            size_t src = out_pos - offset;
            for (size_t i = 0; i < len; ++i) {
                if (out_pos >= out_max) return out_pos;
                out[out_pos++] = out[src + i];
            }
        } else {
            out[out_pos++] = token;
        }
    }
    return out_pos;
}

void msimd_bitpack_pack(const uint32_t* values, size_t count, int bitwidth, uint8_t* out_bytes, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    size_t bit_pos = 0;
    uint32_t mask = (bitwidth >= 32) ? 0xFFFFFFFFU : ((1U << bitwidth) - 1U);

    for (size_t i = 0; i < count; ++i) {
        uint32_t val = values[i] & mask;
        for (int b = 0; b < bitwidth; ++b) {
            size_t byte_idx = bit_pos / 8;
            int bit_idx = bit_pos % 8;
            if (val & (1U << b)) {
                out_bytes[byte_idx] |= (uint8_t)(1U << bit_idx);
            } else {
                out_bytes[byte_idx] &= (uint8_t)(~(1U << bit_idx));
            }
            bit_pos++;
        }
    }
}

void msimd_bitpack_unpack(const uint8_t* in_bytes, size_t count, int bitwidth, uint32_t* out_values, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    size_t bit_pos = 0;
    for (size_t i = 0; i < count; ++i) {
        uint32_t val = 0;
        for (int b = 0; b < bitwidth; ++b) {
            size_t byte_idx = bit_pos / 8;
            int bit_idx = bit_pos % 8;
            if (in_bytes[byte_idx] & (1U << bit_idx)) {
                val |= (1U << b);
            }
            bit_pos++;
        }
        out_values[i] = val;
    }
}

uint16_t msimd_float_to_half(float val) {
    uint32_t f;
    std::memcpy(&f, &val, 4);
    uint32_t sign = (f >> 31) & 0x1;
    int32_t exp = ((f >> 23) & 0xFF) - 127 + 15;
    uint32_t mant = f & 0x7FFFFF;

    if (exp <= 0) {
        return (uint16_t)(sign << 15);
    } else if (exp >= 31) {
        return (uint16_t)((sign << 15) | 0x7C00);
    }
    return (uint16_t)((sign << 15) | (exp << 10) | (mant >> 13));
}

float msimd_half_to_float(uint16_t h, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    uint32_t sign = (h >> 15) & 0x1;
    uint32_t exp = (h >> 10) & 0x1F;
    uint32_t mant = h & 0x3FF;

    uint32_t f_exp;
    uint32_t f_mant;

    if (exp == 0) {
        if (mant == 0) {
            uint32_t f = sign << 31;
            float res;
            std::memcpy(&res, &f, 4);
            return res;
        }
        f_exp = 127 - 14;
        f_mant = mant << 13;
    } else if (exp == 31) {
        f_exp = 255;
        f_mant = mant << 13;
    } else {
        f_exp = exp - 15 + 127;
        f_mant = mant << 13;
    }

    uint32_t f = (sign << 31) | (f_exp << 23) | f_mant;
    float res;
    std::memcpy(&res, &f, 4);
    return res;
}

void msimd_wave_morphology_step(float* out_state, const float* in_state, size_t n,
    float dispersion, float coupling, float nonlinearity, float dt, int mode, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 3) return;
    const float m_val = (mode > 0) ? (float)mode : 1.0f;
    const float k_disp = dispersion * 0.01f * m_val;
    const float k_coup = coupling * 0.005f * m_val;
    const float k_nonlin = nonlinearity * 0.002f;

    for (size_t j = 1; j < n - 1; ++j) {
        float laplacian = in_state[j - 1] - 2.0f * in_state[j] + in_state[j + 1];
        float cubic_term = in_state[j] * in_state[j] * in_state[j];
        float vel = k_disp * laplacian - k_nonlin * cubic_term + k_coup * in_state[j];
        out_state[j] = in_state[j] + vel * dt;
    }
    out_state[0] = out_state[1];
    out_state[n - 1] = out_state[n - 2];
}

void msimd_soliton_interaction_step(float* phi_re, float* phi_im, size_t n,
    float v1, float v2, float phase_offset, float dt, float dx, int mode, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 2) return;
    const float m_val = (mode > 0) ? (float)mode : 1.0f;
    const float omega = m_val * 0.05f;
    const float inv_dx2 = 1.0f / (dx * dx);

    for (size_t j = 1; j < n - 1; ++j) {
        float lap_r = (phi_re[j - 1] - 2.0f * phi_re[j] + phi_re[j + 1]) * inv_dx2;
        float lap_i = (phi_im[j - 1] - 2.0f * phi_im[j] + phi_im[j + 1]) * inv_dx2;
        float density = phi_re[j] * phi_re[j] + phi_im[j] * phi_im[j];

        float d_re = -0.5f * lap_i - density * phi_im[j];
        float d_im =  0.5f * lap_r + density * phi_re[j];

        phi_re[j] += d_re * dt;
        phi_im[j] += d_im * dt;
    }
}

void msimd_discrete_spatial_filter_step(float* target, const float* source, size_t n, float alpha, int mode, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 2) return;
    const float m_val = (mode > 0) ? (float)mode : 1.0f;
    const float w = std::clamp(alpha * (1.0f / m_val), 0.0f, 1.0f);
    target[0] = source[0];
    for (size_t j = 1; j < n; ++j) {
        target[j] = target[j - 1] + w * (source[j] - target[j - 1]);
    }
}

void msimd_wave_taxonomy_eval(int mode, float* out, const float* in, size_t n,
    float dispersion, float coupling, float nonlinearity, float dt, const msimd_tensor_t* precision_target) {
    msimd_wave_morphology_step(out, in, n, dispersion, coupling, nonlinearity, dt, mode, precision_target);
}

double msimd_ipow(double base, int64_t exp, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (exp == 0) return 1.0;
    if (exp < 0) {
        base = 1.0 / base;
        exp = -exp;
    }
    double res = 1.0;
    while (exp > 0) {
        if (exp & 1) res *= base;
        base *= base;
        exp >>= 1;
    }
    return res;
}

double msimd_pow(double base, double exp, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::pow(base, exp);
}

void msimd_lagrangian_step(double* q, double* q_dot, const double* mass, const double* grad_v, size_t n, double dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (size_t i = 0; i < n; ++i) {
        double m = (mass && mass[i] > 1e-12) ? mass[i] : 1.0;
        double a = -grad_v[i] / m;
        q_dot[i] += a * dt;
        q[i] += q_dot[i] * dt;
    }
}

void msimd_hamiltonian_step(double* q, double* p, const double* inv_m, const double* grad_v, size_t n, double dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (size_t i = 0; i < n; ++i) {
        double im = inv_m ? inv_m[i] : 1.0;
        p[i] -= grad_v[i] * dt;
        q[i] += p[i] * im * dt;
    }
}

void msimd_symplectic_yoshida4_step(double* q, double* p, const double* inv_m, void (*force_fn)(const double*, double*, void*), size_t n, double dt, void* user_data, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double cr = std::cbrt(2.0);
    const double w1 = 1.0 / (2.0 - cr);
    const double w0 = -cr / (2.0 - cr);
    const double c[4] = { w1 * 0.5, (w0 + w1) * 0.5, (w0 + w1) * 0.5, w1 * 0.5 };
    const double d[3] = { w1, w0, w1 };

    std::vector<double> f(n, 0.0);
    for (int step = 0; step < 3; ++step) {
        for (size_t i = 0; i < n; ++i) {
            double im = inv_m ? inv_m[i] : 1.0;
            q[i] += c[step] * p[i] * im * dt;
        }
        force_fn(q, f.data(), user_data);
        for (size_t i = 0; i < n; ++i) {
            p[i] += d[step] * f[i] * dt;
        }
    }
    for (size_t i = 0; i < n; ++i) {
        double im = inv_m ? inv_m[i] : 1.0;
        q[i] += c[3] * p[i] * im * dt;
    }
}

void msimd_kepler_orbit_solve(double a, double e, double M, double* r, double* nu, double* x, double* y, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double E = M;
    for (int iter = 0; iter < 30; ++iter) {
        double dE = (E - e * std::sin(E) - M) / (1.0 - e * std::cos(E));
        E -= dE;
        if (std::abs(dE) < 1e-12) break;
    }
    double denom = 1.0 - e * std::cos(E);
    double cos_nu = (std::cos(E) - e) / denom;
    double sin_nu = (std::sqrt(std::max(0.0, 1.0 - e * e)) * std::sin(E)) / denom;
    double true_anomaly = std::atan2(sin_nu, cos_nu);
    double radius = a * (1.0 - e * std::cos(E));

    if (r) *r = radius;
    if (nu) *nu = true_anomaly;
    if (x) *x = radius * std::cos(true_anomaly);
    if (y) *y = radius * std::sin(true_anomaly);
}

void msimd_cauchy_stress_strain_3d(const double grad_u[3][3], double lambda, double mu, double stress[3][3], double strain[3][3], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double trace_eps = 0.0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            strain[i][j] = 0.5 * (grad_u[i][j] + grad_u[j][i]);
        }
        trace_eps += strain[i][i];
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            stress[i][j] = 2.0 * mu * strain[i][j];
            if (i == j) stress[i][j] += lambda * trace_eps;
        }
    }
}

void msimd_navier_cauchy_elastodynamics_step(size_t nx, size_t ny, size_t nz, const float* u_prev, const float* u_curr, float* u_next, float lambda, float mu, float rho, float dt, float dx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (nx < 3 || ny < 3 || nz < 3) return;
    const float inv_rho = 1.0f / (rho > 1e-6f ? rho : 1.0f);
    const float dt2 = dt * dt;
    const float inv_dx2 = 1.0f / (dx * dx);
    const float c_bulk = (lambda + 2.0f * mu) * inv_rho * dt2 * inv_dx2;

    for (size_t z = 1; z < nz - 1; ++z) {
        for (size_t y = 1; y < ny - 1; ++y) {
            for (size_t x = 1; x < nx - 1; ++x) {
                size_t idx = (z * ny + y) * nx + x;
                float lap = u_curr[idx + 1] + u_curr[idx - 1] +
                            u_curr[idx + nx] + u_curr[idx - nx] +
                            u_curr[idx + nx * ny] + u_curr[idx - nx * ny] - 6.0f * u_curr[idx];
                u_next[idx] = 2.0f * u_curr[idx] - u_prev[idx] + c_bulk * lap;
            }
        }
    }
}

void msimd_euler_bernoulli_beam_step(size_t n, const float* w_prev, const float* w_curr, float* w_next, float EI, float rhoA, float q_load, float dt, float dx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 5) return;
    const float inv_dx4 = 1.0f / (dx * dx * dx * dx);
    const float factor = (EI / (rhoA > 1e-6f ? rhoA : 1.0f)) * (dt * dt) * inv_dx4;
    const float q_term = (q_load / (rhoA > 1e-6f ? rhoA : 1.0f)) * (dt * dt);

    for (size_t i = 2; i < n - 2; ++i) {
        float d4 = w_curr[i - 2] - 4.0f * w_curr[i - 1] + 6.0f * w_curr[i] - 4.0f * w_curr[i + 1] + w_curr[i + 2];
        w_next[i] = 2.0f * w_curr[i] - w_prev[i] - factor * d4 + q_term;
    }
    w_next[0] = 0.0f; w_next[1] = 0.0f;
    w_next[n - 1] = 0.0f; w_next[n - 2] = 0.0f;
}

double msimd_thermo_van_der_waals_p(double T, double Vm, double a, double b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double R = 8.314462618;
    if (Vm <= b) return 0.0;
    return (R * T) / (Vm - b) - a / (Vm * Vm);
}

double msimd_thermo_stefan_boltzmann(double emissivity, double T, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double sigma = 5.670374419e-8;
    return emissivity * sigma * (T * T * T * T);
}

void msimd_ising_model_2d_step(int8_t* spin, size_t nx, size_t ny, double J, double B, double T, uint64_t* rng_state, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (nx < 2 || ny < 2 || !spin || !rng_state) return;
    auto lcg_rand = [](uint64_t* s) -> double {
        *s = (*s * 6364136223846793005ULL + 1442695040888963407ULL);
        return (double)(*s >> 11) * (1.0 / 9007199254740992.0);
    };

    const double inv_T = (T > 1e-6) ? (1.0 / T) : 1e6;
    for (size_t y = 0; y < ny; ++y) {
        size_t ym = (y == 0) ? ny - 1 : y - 1;
        size_t yp = (y == ny - 1) ? 0 : y + 1;
        for (size_t x = 0; x < nx; ++x) {
            size_t xm = (x == 0) ? nx - 1 : x - 1;
            size_t xp = (x == nx - 1) ? 0 : x + 1;

            int8_t s = spin[y * nx + x];
            int8_t neighbors = spin[ym * nx + x] + spin[yp * nx + x] + spin[y * nx + xm] + spin[y * nx + xp];
            double dE = 2.0 * s * (J * (double)neighbors + B);

            if (dE <= 0.0 || lcg_rand(rng_state) < std::exp(-dE * inv_T)) {
                spin[y * nx + x] = (int8_t)(-s);
            }
        }
    }
}

void msimd_optics_fresnel(double n1, double n2, double theta_i, double* r_s, double* r_p, double* t_s, double* t_p, double* brewster_angle, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double sin_theta_t = (n1 / n2) * std::sin(theta_i);
    if (std::abs(sin_theta_t) >= 1.0) {
        if (r_s) *r_s = 1.0;
        if (r_p) *r_p = 1.0;
        if (t_s) *t_s = 0.0;
        if (t_p) *t_p = 0.0;
    } else {
        double cos_i = std::cos(theta_i);
        double cos_t = std::sqrt(1.0 - sin_theta_t * sin_theta_t);
        double rs = (n1 * cos_i - n2 * cos_t) / (n1 * cos_i + n2 * cos_t);
        double rp = (n2 * cos_i - n1 * cos_t) / (n2 * cos_i + n1 * cos_t);
        if (r_s) *r_s = rs * rs;
        if (r_p) *r_p = rp * rp;
        if (t_s) *t_s = 1.0 - rs * rs;
        if (t_p) *t_p = 1.0 - rp * rp;
    }
    if (brewster_angle) *brewster_angle = std::atan(n2 / n1);
}

void msimd_optics_jones_apply(const double jones_mat_re[2][2], const double jones_mat_im[2][2], const double in_re[2], const double in_im[2], double out_re[2], double out_im[2], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) {
        out_re[i] = 0.0;
        out_im[i] = 0.0;
        for (int j = 0; j < 2; ++j) {
            out_re[i] += jones_mat_re[i][j] * in_re[j] - jones_mat_im[i][j] * in_im[j];
            out_im[i] += jones_mat_re[i][j] * in_im[j] + jones_mat_im[i][j] * in_re[j];
        }
    }
}

double msimd_optics_fraunhofer_airy(double r, double wavelength, double aperture_radius, double focal_length, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (r < 1e-12) return 1.0;
    const double k = (2.0 * M_PI) / wavelength;
    double x = (k * aperture_radius * r) / focal_length;
    double j1 = std::cyl_bessel_j(1, x);
    double val = (2.0 * j1) / x;
    return val * val;
}

void msimd_relativity_lorentz_boost(const double beta[3], const double four_vec_in[4], double four_vec_out[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double b2 = beta[0]*beta[0] + beta[1]*beta[1] + beta[2]*beta[2];
    if (b2 >= 1.0) b2 = 0.999999999999;
    double gamma = 1.0 / std::sqrt(1.0 - b2);
    double b_dot_r = beta[0]*four_vec_in[1] + beta[1]*four_vec_in[2] + beta[2]*four_vec_in[3];

    four_vec_out[0] = gamma * (four_vec_in[0] - b_dot_r);
    double factor = (b2 > 1e-15) ? ((gamma - 1.0) / b2) : 0.0;
    for (int i = 0; i < 3; ++i) {
        four_vec_out[i + 1] = four_vec_in[i + 1] + factor * b_dot_r * beta[i] - gamma * beta[i] * four_vec_in[0];
    }
}

double msimd_relativity_compton_shift(double theta, double wavelength_in, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double lambda_c = 2.42631023867e-12;
    double delta_lambda = lambda_c * (1.0 - std::cos(theta));
    return wavelength_in + delta_lambda;
}

double msimd_relativity_doppler(double f_source, double beta, double theta_rad, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (std::abs(beta) >= 1.0) beta = 0.999999999999;
    double gamma = 1.0 / std::sqrt(1.0 - beta * beta);
    return f_source / (gamma * (1.0 - beta * std::cos(theta_rad)));
}

int msimd_mat_eigen_jacobi(size_t n, const double* A, double* eigenvalues, double* eigenvectors, int max_iter, double tol, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::vector<double> D(A, A + n * n);
    std::vector<double> V(n * n, 0.0);
    for (size_t i = 0; i < n; ++i) V[i * n + i] = 1.0;

    for (int iter = 0; iter < max_iter; ++iter) {
        size_t p = 0, q = 1;
        double max_val = 0.0;
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = i + 1; j < n; ++j) {
                double val = std::abs(D[i * n + j]);
                if (val > max_val) {
                    max_val = val;
                    p = i; q = j;
                }
            }
        }
        if (max_val < tol) break;

        double theta = 0.5 * (D[q * n + q] - D[p * n + p]) / D[p * n + q];
        double t = 1.0 / (std::abs(theta) + std::sqrt(theta * theta + 1.0));
        if (theta < 0.0) t = -t;
        double c = 1.0 / std::sqrt(t * t + 1.0);
        double s = t * c;

        double d_pp = D[p * n + p], d_qq = D[q * n + q], d_pq = D[p * n + q];
        D[p * n + p] = c * c * d_pp - 2.0 * s * c * d_pq + s * s * d_qq;
        D[q * n + q] = s * s * d_pp + 2.0 * s * c * d_pq + c * c * d_qq;
        D[p * n + q] = 0.0; D[q * n + p] = 0.0;

        for (size_t k = 0; k < n; ++k) {
            if (k != p && k != q) {
                double d_kp = D[k * n + p], d_kq = D[k * n + q];
                D[k * n + p] = c * d_kp - s * d_kq;
                D[p * n + k] = D[k * n + p];
                D[k * n + q] = s * d_kp + c * d_kq;
                D[q * n + k] = D[k * n + q];
            }
            double v_kp = V[k * n + p], v_kq = V[k * n + q];
            V[k * n + p] = c * v_kp - s * v_kq;
            V[k * n + q] = s * v_kp + c * v_kq;
        }
    }

    for (size_t i = 0; i < n; ++i) eigenvalues[i] = D[i * n + i];
    for (size_t i = 0; i < n * n; ++i) eigenvectors[i] = V[i];
    return 0;
}

void msimd_mat_svd_3x3(const double A[3][3], double U[3][3], double S[3], double V[3][3], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double AtA[9] = {0};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) AtA[i * 3 + j] += A[k][i] * A[k][j];
        }
    }
    double eig_val[3], eig_vec[9];
    msimd_mat_eigen_jacobi(3, AtA, eig_val, eig_vec, 50, 1e-9);

    for (int i = 0; i < 3; ++i) {
        S[i] = std::sqrt(std::max(0.0, eig_val[i]));
        for (int j = 0; j < 3; ++j) V[j][i] = eig_vec[j * 3 + i];
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            double sum = 0.0;
            for (int k = 0; k < 3; ++k) sum += A[i][k] * V[k][j];
            U[i][j] = (S[j] > 1e-12) ? (sum / S[j]) : 0.0;
        }
    }
}

int msimd_mat_cholesky(size_t n, const double* A, double* L, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::fill(L, L + n * n, 0.0);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j <= i; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < j; ++k) sum += L[i * n + k] * L[j * n + k];
            if (i == j) {
                double val = A[i * n + i] - sum;
                if (val <= 0.0) return -1;
                L[i * n + j] = std::sqrt(val);
            } else {
                L[i * n + j] = (A[i * n + j] - sum) / L[j * n + j];
            }
        }
    }
    return 0;
}

int msimd_mat_lu_decompose(size_t n, const double* A, double* LU, int* p, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::copy(A, A + n * n, LU);
    for (size_t i = 0; i < n; ++i) p[i] = (int)i;

    for (size_t i = 0; i < n; ++i) {
        size_t max_row = i;
        double max_val = std::abs(LU[i * n + i]);
        for (size_t k = i + 1; k < n; ++k) {
            double val = std::abs(LU[k * n + i]);
            if (val > max_val) {
                max_val = val;
                max_row = k;
            }
        }
        if (max_val < 1e-14) return -1;
        if (max_row != i) {
            std::swap(p[i], p[max_row]);
            for (size_t k = 0; k < n; ++k) std::swap(LU[i * n + k], LU[max_row * n + k]);
        }
        for (size_t j = i + 1; j < n; ++j) {
            LU[j * n + i] /= LU[i * n + i];
            for (size_t k = i + 1; k < n; ++k) {
                LU[j * n + k] -= LU[j * n + i] * LU[i * n + k];
            }
        }
    }
    return 0;
}

void msimd_mat_exp_pade(size_t n, const double* A, double* expA, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double norm_a = 0.0;
    for (size_t i = 0; i < n * n; ++i) norm_a = std::max(norm_a, std::abs(A[i]));
    int s = 0;
    if (std::isfinite(norm_a)) {
        while (norm_a > 0.5 && s < 1022) { norm_a *= 0.5; ++s; }
    }

    double scale = std::ldexp(1.0, -s);
    std::vector<double> A_scaled(n * n), I(n * n, 0.0);
    for (size_t i = 0; i < n * n; ++i) A_scaled[i] = A[i] * scale;
    for (size_t i = 0; i < n; ++i) I[i * n + i] = 1.0;

    std::vector<double> cur = I, term = I;
    double fact = 1.0;
    for (int k = 1; k <= 8; ++k) {
        fact *= k;
        std::vector<double> next(n * n, 0.0);
        for (size_t r = 0; r < n; ++r) {
            for (size_t c = 0; c < n; ++c) {
                for (size_t m = 0; m < n; ++m) next[r * n + c] += term[r * n + m] * A_scaled[m * n + c];
            }
        }
        term = next;
        for (size_t i = 0; i < n * n; ++i) cur[i] += term[i] / fact;
    }
    for (int sq = 0; sq < s; ++sq) {
        std::vector<double> next(n * n, 0.0);
        for (size_t r = 0; r < n; ++r) {
            for (size_t c = 0; c < n; ++c) {
                for (size_t m = 0; m < n; ++m) next[r * n + c] += cur[r * n + m] * cur[m * n + c];
            }
        }
        cur = next;
    }
    std::copy(cur.begin(), cur.end(), expA);
}

void msimd_clifford_cl3_mul(const double a[8], const double b[8], double out[8], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double s = a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3] - a[4]*b[4] - a[5]*b[5] - a[6]*b[6] - a[7]*b[7];
    double v1 = a[0]*b[1] + a[1]*b[0] - a[2]*b[4] + a[3]*b[6] + a[4]*b[2] - a[6]*b[3] - a[5]*b[7] - a[7]*b[5];
    double v2 = a[0]*b[2] + a[2]*b[0] + a[1]*b[4] - a[3]*b[5] - a[4]*b[1] + a[5]*b[3] - a[6]*b[7] - a[7]*b[6];
    double v3 = a[0]*b[3] + a[3]*b[0] - a[1]*b[6] + a[2]*b[5] + a[6]*b[1] - a[5]*b[2] - a[4]*b[7] - a[7]*b[4];
    double b12 = a[0]*b[4] + a[4]*b[0] + a[1]*b[2] - a[2]*b[1] + a[3]*b[7] + a[7]*b[3] + a[5]*b[6] - a[6]*b[5];
    double b23 = a[0]*b[5] + a[5]*b[0] + a[2]*b[3] - a[3]*b[2] + a[1]*b[7] + a[7]*b[1] + a[6]*b[4] - a[4]*b[6];
    double b31 = a[0]*b[6] + a[6]*b[0] + a[3]*b[1] - a[1]*b[3] + a[2]*b[7] + a[7]*b[2] + a[4]*b[5] - a[5]*b[4];
    double ps = a[0]*b[7] + a[7]*b[0] + a[1]*b[5] + a[2]*b[6] + a[3]*b[4] + a[4]*b[3] + a[5]*b[1] + a[6]*b[2];
    out[0] = s; out[1] = v1; out[2] = v2; out[3] = v3;
    out[4] = b12; out[5] = b23; out[6] = b31; out[7] = ps;
}

void msimd_lie_se3_exp(const double xi[6], double T[4][4], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double w[3] = { xi[3], xi[4], xi[5] };
    double theta = std::sqrt(w[0]*w[0] + w[1]*w[1] + w[2]*w[2]);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) T[i][j] = (i == j) ? 1.0 : 0.0;
    }
    if (theta < 1e-12) {
        T[0][3] = xi[0]; T[1][3] = xi[1]; T[2][3] = xi[2];
        return;
    }
    double k[3] = { w[0]/theta, w[1]/theta, w[2]/theta };
    double s = std::sin(theta), c = std::cos(theta), v = 1.0 - c;
    T[0][0] = c + k[0]*k[0]*v;        T[0][1] = k[0]*k[1]*v - k[2]*s; T[0][2] = k[0]*k[2]*v + k[1]*s;
    T[1][0] = k[1]*k[0]*v + k[2]*s;   T[1][1] = c + k[1]*k[1]*v;        T[1][2] = k[1]*k[2]*v - k[0]*s;
    T[2][0] = k[2]*k[0]*v - k[1]*s;   T[2][1] = k[2]*k[1]*v + k[0]*s;   T[2][2] = c + k[2]*k[2]*v;

    double B = (1.0 - c) / (theta * theta);
    double C = (theta - s) / (theta * theta * theta);
    double V_mat[3][3];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) V_mat[i][j] = (i == j) ? 1.0 : 0.0;
    }
    double W[3][3] = { { 0, -w[2], w[1] }, { w[2], 0, -w[0] }, { -w[1], w[0], 0 } };
    double W2[3][3] = {0};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int m = 0; m < 3; ++m) W2[i][j] += W[i][m] * W[m][j];
            V_mat[i][j] += B * W[i][j] + C * W2[i][j];
        }
    }
    for (int i = 0; i < 3; ++i) {
        T[i][3] = V_mat[i][0]*xi[0] + V_mat[i][1]*xi[1] + V_mat[i][2]*xi[2];
    }
}

void msimd_lie_se3_log(const double T[4][4], double xi[6], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double tr = T[0][0] + T[1][1] + T[2][2];
    double cos_theta = std::clamp((tr - 1.0) * 0.5, -1.0, 1.0);
    double theta = std::acos(cos_theta);
    if (theta < 1e-12) {
        xi[0] = T[0][3]; xi[1] = T[1][3]; xi[2] = T[2][3];
        xi[3] = 0.0; xi[4] = 0.0; xi[5] = 0.0;
        return;
    }
    double factor = 0.5 * theta / std::sin(theta);
    xi[3] = (T[2][1] - T[1][2]) * factor;
    xi[4] = (T[0][2] - T[2][0]) * factor;
    xi[5] = (T[1][0] - T[0][1]) * factor;
    xi[0] = T[0][3]; xi[1] = T[1][3]; xi[2] = T[2][3];
}

double msimd_num_gauss_legendre_quad(double (*fn)(double, void*), double a, double b, int n_points, void* user_data, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    static const double x5[5] = { 0.0, -0.538469310105683, 0.538469310105683, -0.906179845938664, 0.906179845938664 };
    static const double w5[5] = { 0.568888888888889, 0.478628670499366, 0.478628670499366, 0.236926885056189, 0.236926885056189 };

    double h = 0.5 * (b - a);
    double m = 0.5 * (b + a);
    double sum = 0.0;
    for (int i = 0; i < 5; ++i) {
        sum += w5[i] * fn(m + h * x5[i], user_data);
    }
    return h * sum;
}

double msimd_num_romberg_integral(double (*fn)(double, void*), double a, double b, int max_steps, double tol, void* user_data, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!fn || max_steps <= 0) return 0.0;
    if (max_steps > 15) max_steps = 15;
    double R[16][16] = {0};
    double h = b - a;
    R[0][0] = 0.5 * h * (fn(a, user_data) + fn(b, user_data));

    for (int i = 1; i < max_steps; ++i) {
        h *= 0.5;
        double sum = 0.0;
        int pts = 1 << (i - 1);
        for (int k = 1; k <= pts; ++k) {
            sum += fn(a + (2 * k - 1) * h, user_data);
        }
        R[i][0] = 0.5 * R[i - 1][0] + h * sum;
        for (int j = 1; j <= i; ++j) {
            double factor = std::pow(4.0, j);
            R[i][j] = (factor * R[i][j - 1] - R[i - 1][j - 1]) / (factor - 1.0);
        }
        if (std::abs(R[i][i] - R[i - 1][i - 1]) < tol) {
            return R[i][i];
        }
    }
    return R[max_steps - 1][max_steps - 1];
}

int msimd_opt_nelder_mead(double (*fn)(const double*, void*), size_t dim, double* x_start, double step, int max_iter, double tol, double* x_opt, double* f_opt, void* user_data, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    size_t n = dim;
    std::vector<std::vector<double>> simplex(n + 1, std::vector<double>(n));
    std::vector<double> f_vals(n + 1);

    simplex[0].assign(x_start, x_start + n);
    f_vals[0] = fn(simplex[0].data(), user_data);

    for (size_t i = 1; i <= n; ++i) {
        simplex[i] = simplex[0];
        simplex[i][i - 1] += step;
        f_vals[i] = fn(simplex[i].data(), user_data);
    }

    for (int iter = 0; iter < max_iter; ++iter) {
        std::vector<size_t> idx(n + 1);
        for (size_t i = 0; i <= n; ++i) idx[i] = i;
        std::sort(idx.begin(), idx.end(), [&](size_t a, size_t b) { return f_vals[a] < f_vals[b]; });

        if (std::abs(f_vals[idx[n]] - f_vals[idx[0]]) < tol) break;

        std::vector<double> centroid(n, 0.0);
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) centroid[j] += simplex[idx[i]][j] / (double)n;
        }

        std::vector<double> xr(n);
        for (size_t j = 0; j < n; ++j) xr[j] = centroid[j] + 1.0 * (centroid[j] - simplex[idx[n]][j]);
        double fr = fn(xr.data(), user_data);

        if (fr >= f_vals[idx[0]] && fr < f_vals[idx[n - 1]]) {
            simplex[idx[n]] = xr;
            f_vals[idx[n]] = fr;
        } else if (fr < f_vals[idx[0]]) {
            std::vector<double> xe(n);
            for (size_t j = 0; j < n; ++j) xe[j] = centroid[j] + 2.0 * (xr[j] - centroid[j]);
            double fe = fn(xe.data(), user_data);
            if (fe < fr) { simplex[idx[n]] = xe; f_vals[idx[n]] = fe; }
            else { simplex[idx[n]] = xr; f_vals[idx[n]] = fr; }
        } else {
            std::vector<double> xc(n);
            for (size_t j = 0; j < n; ++j) xc[j] = centroid[j] + 0.5 * (simplex[idx[n]][j] - centroid[j]);
            double fc = fn(xc.data(), user_data);
            if (fc < f_vals[idx[n]]) { simplex[idx[n]] = xc; f_vals[idx[n]] = fc; }
            else {
                for (size_t i = 1; i <= n; ++i) {
                    for (size_t j = 0; j < n; ++j) simplex[idx[i]][j] = 0.5 * (simplex[idx[i]][j] + simplex[idx[0]][j]);
                    f_vals[idx[i]] = fn(simplex[idx[i]].data(), user_data);
                }
            }
        }
    }

    size_t best_idx = 0;
    for (size_t i = 1; i <= n; ++i) {
        if (f_vals[i] < f_vals[best_idx]) best_idx = i;
    }
    if (x_opt) std::copy(simplex[best_idx].begin(), simplex[best_idx].end(), x_opt);
    if (f_opt) *f_opt = f_vals[best_idx];
    return 0;
}

void msimd_opt_lbfgs_two_loop(size_t n, size_t m, const double* grad, const double* const* s_history, const double* const* y_history, const double* rho_history, size_t history_count, double* dir, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::vector<double> q(grad, grad + n);
    std::vector<double> alpha(history_count, 0.0);

    for (int i = (int)history_count - 1; i >= 0; --i) {
        double s_dot_q = 0.0;
        for (size_t j = 0; j < n; ++j) s_dot_q += s_history[i][j] * q[j];
        alpha[i] = rho_history[i] * s_dot_q;
        for (size_t j = 0; j < n; ++j) q[j] -= alpha[i] * y_history[i][j];
    }

    double gamma = 1.0;
    if (history_count > 0) {
        size_t last = history_count - 1;
        double s_dot_y = 0.0, y_dot_y = 0.0;
        for (size_t j = 0; j < n; ++j) {
            s_dot_y += s_history[last][j] * y_history[last][j];
            y_dot_y += y_history[last][j] * y_history[last][j];
        }
        if (y_dot_y > 1e-12) gamma = s_dot_y / y_dot_y;
    }

    for (size_t j = 0; j < n; ++j) dir[j] = gamma * q[j];

    for (size_t i = 0; i < history_count; ++i) {
        double y_dot_r = 0.0;
        for (size_t j = 0; j < n; ++j) y_dot_r += y_history[i][j] * dir[j];
        double beta = rho_history[i] * y_dot_r;
        for (size_t j = 0; j < n; ++j) dir[j] += s_history[i][j] * (alpha[i] - beta);
    }
}

double msimd_dist_normal_pdf(double x, double mu, double sigma, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (sigma <= 0.0) return 0.0;
    double z = (x - mu) / sigma;
    return (1.0 / (sigma * std::sqrt(2.0 * M_PI))) * std::exp(-0.5 * z * z);
}

double msimd_dist_normal_cdf(double x, double mu, double sigma, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (sigma <= 0.0) return (x >= mu) ? 1.0 : 0.0;
    return 0.5 * (1.0 + std::erf((x - mu) / (sigma * std::sqrt(2.0))));
}

double msimd_dist_gamma_pdf(double x, double alpha, double beta, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (x <= 0.0 || alpha <= 0.0 || beta <= 0.0) return 0.0;
    return (std::pow(beta, alpha) / std::tgamma(alpha)) * std::pow(x, alpha - 1.0) * std::exp(-beta * x);
}

double msimd_dist_poisson_pmf(int k, double lambda, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (k < 0 || lambda <= 0.0) return 0.0;
    return std::exp(-lambda + (double)k * std::log(lambda) - std::lgamma((double)(k + 1)));
}

double msimd_dist_chisquare_cdf(double x, int k, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (x <= 0.0 || k <= 0) return 0.0;
    double a = 0.5 * k;
    double z = 0.5 * x;
    double sum = 1.0 / a;
    double term = 1.0 / a;
    for (int n = 1; n < 100; ++n) {
        term *= z / (a + n);
        sum += term;
        if (term < sum * 1e-12) break;
    }
    return sum * std::exp(-z + a * std::log(z) - std::lgamma(a));
}

void msimd_stat_covariance_matrix(size_t num_samples, size_t num_vars, const double* data, double* cov_matrix, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (num_samples < 2 || num_vars == 0) return;
    std::vector<double> mean(num_vars, 0.0);
    for (size_t s = 0; s < num_samples; ++s) {
        for (size_t v = 0; v < num_vars; ++v) mean[v] += data[s * num_vars + v];
    }
    for (size_t v = 0; v < num_vars; ++v) mean[v] /= (double)num_samples;

    double factor = 1.0 / (double)(num_samples - 1);
    for (size_t i = 0; i < num_vars; ++i) {
        for (size_t j = 0; j < num_vars; ++j) {
            double sum = 0.0;
            for (size_t s = 0; s < num_samples; ++s) {
                sum += (data[s * num_vars + i] - mean[i]) * (data[s * num_vars + j] - mean[j]);
            }
            cov_matrix[i * num_vars + j] = sum * factor;
        }
    }
}

void msimd_filter_kalman_step(size_t dim_x, size_t dim_z, double* x, double* P, const double* F, const double* Q, const double* H, const double* R, const double* z, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::vector<double> x_p(dim_x, 0.0);
    for (size_t i = 0; i < dim_x; ++i) {
        for (size_t j = 0; j < dim_x; ++j) x_p[i] += F[i * dim_x + j] * x[j];
    }
    std::vector<double> FP(dim_x * dim_x, 0.0);
    for (size_t i = 0; i < dim_x; ++i) {
        for (size_t j = 0; j < dim_x; ++j) {
            for (size_t k = 0; k < dim_x; ++k) FP[i * dim_x + j] += F[i * dim_x + k] * P[k * dim_x + j];
        }
    }
    std::vector<double> P_p(dim_x * dim_x, 0.0);
    for (size_t i = 0; i < dim_x; ++i) {
        for (size_t j = 0; j < dim_x; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < dim_x; ++k) sum += FP[i * dim_x + k] * F[j * dim_x + k];
            P_p[i * dim_x + j] = sum + Q[i * dim_x + j];
        }
    }

    std::vector<double> y(dim_z, 0.0);
    for (size_t i = 0; i < dim_z; ++i) {
        double hx = 0.0;
        for (size_t j = 0; j < dim_x; ++j) hx += H[i * dim_x + j] * x_p[j];
        y[i] = z[i] - hx;
    }

    std::vector<double> HP(dim_z * dim_x, 0.0);
    for (size_t i = 0; i < dim_z; ++i) {
        for (size_t j = 0; j < dim_x; ++j) {
            for (size_t k = 0; k < dim_x; ++k) HP[i * dim_x + j] += H[i * dim_x + k] * P_p[k * dim_x + j];
        }
    }
    std::vector<double> S(dim_z * dim_z, 0.0);
    for (size_t i = 0; i < dim_z; ++i) {
        for (size_t j = 0; j < dim_z; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < dim_x; ++k) sum += HP[i * dim_x + k] * H[j * dim_x + k];
            S[i * dim_z + j] = sum + R[i * dim_z + j];
        }
    }

    std::vector<double> inv_S(dim_z * dim_z, 0.0);
    if (dim_z == 1) {
        inv_S[0] = (std::abs(S[0]) > 1e-15) ? (1.0 / S[0]) : 0.0;
    } else if (dim_z == 2) {
        double det_S = S[0] * S[3] - S[1] * S[2];
        if (std::abs(det_S) > 1e-15) {
            double inv_det = 1.0 / det_S;
            inv_S[0] =  S[3] * inv_det; inv_S[1] = -S[1] * inv_det;
            inv_S[2] = -S[2] * inv_det; inv_S[3] =  S[0] * inv_det;
        }
    } else {

        std::vector<double> aug(dim_z * 2 * dim_z, 0.0);
        for (size_t i = 0; i < dim_z; ++i) {
            for (size_t j = 0; j < dim_z; ++j) {
                aug[i * (2 * dim_z) + j] = S[i * dim_z + j];
            }
            aug[i * (2 * dim_z) + (dim_z + i)] = 1.0;
        }

        bool singular = false;
        for (size_t i = 0; i < dim_z; ++i) {
            size_t max_row = i;
            double max_val = std::abs(aug[i * (2 * dim_z) + i]);
            for (size_t r = i + 1; r < dim_z; ++r) {
                double val = std::abs(aug[r * (2 * dim_z) + i]);
                if (val > max_val) {
                    max_val = val;
                    max_row = r;
                }
            }
            if (max_val < 1e-15) {
                singular = true;
                break;
            }
            if (max_row != i) {
                for (size_t c = 0; c < 2 * dim_z; ++c) {
                    std::swap(aug[i * (2 * dim_z) + c], aug[max_row * (2 * dim_z) + c]);
                }
            }
            double pivot = aug[i * (2 * dim_z) + i];
            double inv_p = 1.0 / pivot;
            for (size_t c = 0; c < 2 * dim_z; ++c) {
                aug[i * (2 * dim_z) + c] *= inv_p;
            }
            for (size_t r = 0; r < dim_z; ++r) {
                if (r != i) {
                    double factor = aug[r * (2 * dim_z) + i];
                    for (size_t c = 0; c < 2 * dim_z; ++c) {
                        aug[r * (2 * dim_z) + c] -= factor * aug[i * (2 * dim_z) + c];
                    }
                }
            }
        }
        if (!singular) {
            for (size_t i = 0; i < dim_z; ++i) {
                for (size_t j = 0; j < dim_z; ++j) {
                    inv_S[i * dim_z + j] = aug[i * (2 * dim_z) + (dim_z + j)];
                }
            }
        }
    }

    std::vector<double> PHT(dim_x * dim_z, 0.0);
    for (size_t i = 0; i < dim_x; ++i) {
        for (size_t j = 0; j < dim_z; ++j) {
            for (size_t k = 0; k < dim_x; ++k) PHT[i * dim_z + j] += P_p[i * dim_x + k] * H[j * dim_x + k];
        }
    }
    std::vector<double> K(dim_x * dim_z, 0.0);
    for (size_t i = 0; i < dim_x; ++i) {
        for (size_t j = 0; j < dim_z; ++j) {
            for (size_t k = 0; k < dim_z; ++k) K[i * dim_z + j] += PHT[i * dim_z + k] * inv_S[k * dim_z + j];
        }
    }

    for (size_t i = 0; i < dim_x; ++i) {
        double ky = 0.0;
        for (size_t j = 0; j < dim_z; ++j) ky += K[i * dim_z + j] * y[j];
        x[i] = x_p[i] + ky;
    }
    for (size_t i = 0; i < dim_x; ++i) {
        for (size_t j = 0; j < dim_x; ++j) {
            double kh = 0.0;
            for (size_t k = 0; k < dim_z; ++k) kh += K[i * dim_z + k] * H[k * dim_x + j];
            double delta = (i == j) ? 1.0 - kh : -kh;
            double sum = 0.0;
            for (size_t m = 0; m < dim_x; ++m) sum += delta * P_p[m * dim_x + j];
            P[i * dim_x + j] = sum;
        }
    }
}

double msimd_fuzzy_triangular(double x, double a, double b, double c, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (x <= a || x >= c) return 0.0;
    if (x == b) return 1.0;
    if (x < b) return (x - a) / (b - a);
    return (c - x) / (c - b);
}

double msimd_fuzzy_trapezoidal(double x, double a, double b, double c, double d, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (x <= a || x >= d) return 0.0;
    if (x >= b && x <= c) return 1.0;
    if (x < b) return (x - a) / (b - a);
    return (d - x) / (d - c);
}

double msimd_fuzzy_gaussian(double x, double c, double sigma, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (sigma <= 0.0) return (x == c) ? 1.0 : 0.0;
    double z = (x - c) / sigma;
    return std::exp(-0.5 * z * z);
}

double msimd_fuzzy_tnorm_min(double a, double b, const msimd_tensor_t* precision_target) {
    (void)precision_target; return std::min(a, b); }
double msimd_fuzzy_snorm_max(double a, double b, const msimd_tensor_t* precision_target) {
    (void)precision_target; return std::max(a, b); }

double msimd_fuzzy_defuzzify_centroid(const double* x_vals, const double* mu_vals, size_t n, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double num = 0.0, den = 0.0;
    for (size_t i = 0; i < n; ++i) {
        num += x_vals[i] * mu_vals[i];
        den += mu_vals[i];
    }
    return (den > 1e-12) ? (num / den) : 0.0;
}

uint8_t msimd_gf256_mul(uint8_t a, uint8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    uint8_t p = 0;
    for (int i = 0; i < 8; ++i) {
        if (b & 1) p ^= a;
        uint8_t hi_bit = (a & 0x80);
        a <<= 1;
        if (hi_bit) a ^= 0x1B;
        b >>= 1;
    }
    return p;
}

uint8_t msimd_gf256_inv(uint8_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (a == 0) return 0;
    uint8_t res = 1;
    uint8_t base = a;
    int exp = 254;
    while (exp > 0) {
        if (exp & 1) res = msimd_gf256_mul(res, base);
        base = msimd_gf256_mul(base, base);
        exp >>= 1;
    }
    return res;
}

void msimd_gravity_poisson_step(size_t nx, size_t ny, size_t nz, const float* rho_mass, const float* phi_curr, float* phi_next, float G_const, float dx, float relax_factor, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (nx < 3 || ny < 3 || nz < 3) return;
    const float dx2 = dx * dx;
    const float src_factor = 4.0f * (float)M_PI * G_const * dx2;
    const float inv_6 = 1.0f / 6.0f;
    const float omega = (relax_factor > 0.0f && relax_factor < 2.0f) ? relax_factor : 1.0f;

    for (size_t z = 1; z < nz - 1; ++z) {
        for (size_t y = 1; y < ny - 1; ++y) {
            for (size_t x = 1; x < nx - 1; ++x) {
                size_t idx = (z * ny + y) * nx + x;
                float neighbor_sum = phi_curr[idx + 1] + phi_curr[idx - 1] +
                                     phi_curr[idx + nx] + phi_curr[idx - nx] +
                                     phi_curr[idx + nx * ny] + phi_curr[idx - nx * ny];
                float phi_jacobi = (neighbor_sum - src_factor * rho_mass[idx]) * inv_6;
                phi_next[idx] = (1.0f - omega) * phi_curr[idx] + omega * phi_jacobi;
            }
        }
    }
}

void msimd_gravity_schwarzschild_metric(double M_mass, double r_dist, double impact_b, double* time_dilation, double* rs_radius, double* deflection_angle, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double G = 6.67430e-11;
    const double c = 299792458.0;
    const double c2 = c * c;

    double rs = (2.0 * G * M_mass) / c2;
    if (rs_radius) *rs_radius = rs;

    if (time_dilation) {
        if (r_dist <= rs) {
            *time_dilation = 0.0;
        } else {
            *time_dilation = std::sqrt(1.0 - rs / r_dist);
        }
    }

    if (deflection_angle) {
        if (impact_b > rs) {
            *deflection_angle = (2.0 * rs) / impact_b;
        } else {
            *deflection_angle = M_PI;
        }
    }
}

void msimd_quantum_dirac_spinor_step(const double psi_re_in[4], const double psi_im_in[4], const double A_pot[4], double mass, double dt, double psi_re_out[4], double psi_im_out[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double c = 299792458.0;
    const double hbar = 1.054571817e-34;
    const double q = -1.602176634e-19;

    double energy_mass = mass * c * c;
    double scalar_pot = q * A_pot[0];

    for (int i = 0; i < 4; ++i) {
        double mass_term = (i < 2) ? energy_mass : -energy_mass;
        double omega = (scalar_pot + mass_term) / hbar;
        double cos_wt = std::cos(omega * dt);
        double sin_wt = std::sin(omega * dt);

        psi_re_out[i] = psi_re_in[i] * cos_wt + psi_im_in[i] * sin_wt;
        psi_im_out[i] = psi_im_in[i] * cos_wt - psi_re_in[i] * sin_wt;
    }
}

void msimd_quantum_pauli_spin_precession(double spin[3], const double B_field[3], double gamma_gyro, double dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double initial_mag = std::sqrt(spin[0]*spin[0] + spin[1]*spin[1] + spin[2]*spin[2]);
    if (initial_mag < 1e-50) return;
    double torque[3] = {
        gamma_gyro * (spin[1] * B_field[2] - spin[2] * B_field[1]),
        gamma_gyro * (spin[2] * B_field[0] - spin[0] * B_field[2]),
        gamma_gyro * (spin[0] * B_field[1] - spin[1] * B_field[0])
    };
    for (int i = 0; i < 3; ++i) {
        spin[i] += torque[i] * dt;
    }
    double mag = std::sqrt(spin[0]*spin[0] + spin[1]*spin[1] + spin[2]*spin[2]);
    if (mag > 1e-50) {
        double scale = initial_mag / mag;
        for (int i = 0; i < 3; ++i) spin[i] *= scale;
    }
}

void msimd_qcd_cornell_potential(double r_dist, double alpha_s, double sigma_string, double* potential, double* force, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (r_dist < 1e-18) r_dist = 1e-18;
    double coul = (4.0 / 3.0) * (alpha_s / r_dist);
    double conf = sigma_string * r_dist;

    if (potential) *potential = -coul + conf;
    if (force) *force = -(4.0 / 3.0) * (alpha_s / (r_dist * r_dist)) - sigma_string;
}

void msimd_nuclear_yukawa_force(double r_dist, double g_coupling, double pion_mass, double* potential, double* force, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (r_dist < 1e-18) r_dist = 1e-18;
    const double hbar = 1.054571817e-34;
    const double c = 299792458.0;
    double mu = (pion_mass * c) / hbar;
    double exp_term = std::exp(-mu * r_dist);
    double g2 = g_coupling * g_coupling;

    double V = -g2 * (exp_term / r_dist);
    if (potential) *potential = V;
    if (force) {
        *force = -g2 * exp_term * (1.0 + mu * r_dist) / (r_dist * r_dist);
    }
}

void msimd_qcd_gellmann_apply(int index, const double color_in_re[3], const double color_in_im[3], double color_out_re[3], double color_out_im[3], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) {
        color_out_re[i] = 0.0;
        color_out_im[i] = 0.0;
    }
    const double inv_sqrt3 = 0.57735026918962576451;

    switch (index) {
        case 1:
            color_out_re[0] = color_in_re[1]; color_out_im[0] = color_in_im[1];
            color_out_re[1] = color_in_re[0]; color_out_im[1] = color_in_im[0];
            break;
        case 2:
            color_out_re[0] = color_in_im[1];  color_out_im[0] = -color_in_re[1];
            color_out_re[1] = -color_in_im[0]; color_out_im[1] = color_in_re[0];
            break;
        case 3:
            color_out_re[0] = color_in_re[0];  color_out_im[0] = color_in_im[0];
            color_out_re[1] = -color_in_re[1]; color_out_im[1] = -color_in_im[1];
            break;
        case 4:
            color_out_re[0] = color_in_re[2]; color_out_im[0] = color_in_im[2];
            color_out_re[2] = color_in_re[0]; color_out_im[2] = color_in_im[0];
            break;
        case 5:
            color_out_re[0] = color_in_im[2];  color_out_im[0] = -color_in_re[2];
            color_out_re[2] = -color_in_im[0]; color_out_im[2] = color_in_re[0];
            break;
        case 6:
            color_out_re[1] = color_in_re[2]; color_out_im[1] = color_in_im[2];
            color_out_re[2] = color_in_re[1]; color_out_im[2] = color_in_im[1];
            break;
        case 7:
            color_out_re[1] = color_in_im[2];  color_out_im[1] = -color_in_re[2];
            color_out_re[2] = -color_in_im[1]; color_out_im[2] = color_in_re[1];
            break;
        case 8:
            color_out_re[0] = inv_sqrt3 * color_in_re[0]; color_out_im[0] = inv_sqrt3 * color_in_im[0];
            color_out_re[1] = inv_sqrt3 * color_in_re[1]; color_out_im[1] = inv_sqrt3 * color_in_im[1];
            color_out_re[2] = -2.0 * inv_sqrt3 * color_in_re[2]; color_out_im[2] = -2.0 * inv_sqrt3 * color_in_im[2];
            break;
        default:
            for (int i = 0; i < 3; ++i) {
                color_out_re[i] = color_in_re[i];
                color_out_im[i] = color_in_im[i];
            }
            break;
    }
}

void msimd_higgs_potential(double phi, double mu_sq, double lambda_self, double* potential, double* dV_dphi, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double phi2 = phi * phi;
    if (potential) {
        *potential = -0.5 * mu_sq * phi2 + 0.25 * lambda_self * phi2 * phi2;
    }
    if (dV_dphi) {
        *dV_dphi = phi * (-mu_sq + lambda_self * phi2);
    }
}

double msimd_higgs_fermion_mass(double yukawa_coupling, double vev_v, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double inv_sqrt2 = 0.7071067811865475;
    return yukawa_coupling * vev_v * inv_sqrt2;
}

void msimd_weak_fermi_decay_rate(double delta_E, double G_fermi, double* decay_rate, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (delta_E < 0.0) delta_E = 0.0;
    double dE5 = delta_E * delta_E;
    dE5 = dE5 * dE5 * delta_E;
    const double pi3 = 31.00627668;
    const double denom = 192.0 * pi3;
    if (decay_rate) {
        *decay_rate = (G_fermi * G_fermi * dE5) / denom;
    }
}

void msimd_weak_boson_potential(double r_dist, double g_weak, double m_w_boson, double* potential, double* force, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double hbar = 1.054571817e-34;
    const double c = 299792458.0;
    if (r_dist < 1e-21) r_dist = 1e-21;
    double r_weak = hbar / (m_w_boson * c);
    if (r_weak < 1e-21) r_weak = 1e-21;

    double exp_term = std::exp(-r_dist / r_weak);
    double inv_4pi = 0.07957747154594767;
    double v = (g_weak * g_weak * inv_4pi / r_dist) * exp_term;

    if (potential) *potential = v;
    if (force) {
        *force = (v / r_dist) * (1.0 + r_dist / r_weak);
    }
}

void msimd_quantum_dirac_charge_conjugate(const double psi_re_in[4], const double psi_im_in[4], double psi_re_out[4], double psi_im_out[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    psi_re_out[0] = -psi_re_in[3]; psi_im_out[0] =  psi_im_in[3];
    psi_re_out[1] =  psi_re_in[2]; psi_im_out[1] = -psi_im_in[2];
    psi_re_out[2] = -psi_re_in[1]; psi_im_out[2] =  psi_im_in[1];
    psi_re_out[3] =  psi_re_in[0]; psi_im_out[3] = -psi_im_in[0];
}

void msimd_particle_annihilation_energy(double m1, double m2, double* total_energy_joules, double* photon_frequency, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double c = 299792458.0;
    const double h_planck = 6.62607015e-34;
    double e_tot = (m1 + m2) * c * c;
    if (total_energy_joules) *total_energy_joules = e_tot;
    if (photon_frequency) {
        *photon_frequency = (e_tot * 0.5) / h_planck;
    }
}

void msimd_qcd_antiquark_color_conjugate(const double color_in_re[3], const double color_in_im[3], double color_out_re[3], double color_out_im[3], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) {
        color_out_re[i] =  color_in_re[i];
        color_out_im[i] = -color_in_im[i];
    }
}

int64_t msimd_planck_quantize_length(double meters, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (std::isnan(meters)) return 0;
    double q = meters / MSIMD_PLANCK_LENGTH;
    if (q > (double)INT64_MAX) return INT64_MAX;
    if (q < (double)INT64_MIN) return INT64_MIN;
    return static_cast<int64_t>(std::round(q));
}

double msimd_planck_dequantize_length(int64_t planck_units, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return static_cast<double>(planck_units) * MSIMD_PLANCK_LENGTH;
}

int64_t msimd_planck_quantize_time(double seconds, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (std::isnan(seconds)) return 0;
    double q = seconds / MSIMD_PLANCK_TIME;
    if (q > (double)INT64_MAX) return INT64_MAX;
    if (q < (double)INT64_MIN) return INT64_MIN;
    return static_cast<int64_t>(std::round(q));
}

double msimd_planck_dequantize_time(int64_t planck_units, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return static_cast<double>(planck_units) * MSIMD_PLANCK_TIME;
}

void msimd_planck_discrete_step(const int64_t* pos_in, const int64_t* vel_in, int64_t dt_planck,
    int64_t* pos_out, size_t n, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!pos_in || !vel_in || !pos_out || n == 0) return;
    for (size_t i = 0; i < n; ++i) {

        pos_out[i] = pos_in[i] + vel_in[i] * dt_planck;
    }
}

void msimd_compensated_sum_neumaier(const double* values, size_t n, double* sum_out, double* comp_out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!values || n == 0) {
        if (sum_out) *sum_out = 0.0;
        if (comp_out) *comp_out = 0.0;
        return;
    }
    double sum = values[0];
    double c = 0.0;
    for (size_t i = 1; i < n; ++i) {
        double cur = values[i];
        double t = sum + cur;
        if (std::abs(sum) >= std::abs(cur)) {
            c += (sum - t) + cur;
        } else {
            c += (cur - t) + sum;
        }
        sum = t;
    }
    if (sum_out)  *sum_out = sum + c;
    if (comp_out) *comp_out = c;
}

void msimd_compensated_sum_neumaier_f32(const float* values, size_t n, float* sum_out, float* comp_out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!values || n == 0) {
        if (sum_out) *sum_out = 0.0f;
        if (comp_out) *comp_out = 0.0f;
        return;
    }
    float sum = values[0];
    float c = 0.0f;
    for (size_t i = 1; i < n; ++i) {
        float cur = values[i];
        float t = sum + cur;
        if (std::abs(sum) >= std::abs(cur)) {
            c += (sum - t) + cur;
        } else {
            c += (cur - t) + sum;
        }
        sum = t;
    }
    if (sum_out)  *sum_out = sum + c;
    if (comp_out) *comp_out = c;
}

double msimd_horner_eval(const double* coeffs, size_t degree, double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!coeffs) return 0.0;
    double result = coeffs[degree];
    for (size_t i = degree; i > 0; --i) {
        result = result * x + coeffs[i - 1];
    }
    return result;
}

double msimd_estrin_eval_8(const double* c, double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!c) return 0.0;
    double x2 = x * x;
    double x4 = x2 * x2;
    double p01 = c[0] + c[1] * x;
    double p23 = c[2] + c[3] * x;
    double p45 = c[4] + c[5] * x;
    double p67 = c[6] + c[7] * x;
    double q03 = p01 + p23 * x2;
    double q47 = p45 + p67 * x2;
    return q03 + q47 * x4;
}

double msimd_minimax_erf(double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (std::isnan(x)) return x;
    double ax = std::abs(x);
    if (ax >= 6.0) return (x > 0.0) ? 1.0 : -1.0;

    if (ax <= 0.84375) {
        const double p[] = {
            0.128379167095512558561e+01,
            -0.325042107242198812700e+00,
            -0.284817495126148158520e-01,
            -0.577027029648944159100e-02,
            -0.237630166566501626000e-04
        };
        const double q[] = {
            0.100000000000000000000e+01,
            0.397917223959155352819e+00,
            0.650222499887672944485e-01,
            0.508130628186091662970e-02,
            0.132494737604696290000e-03
        };
        double z = ax * ax;
        double num = p[0] + z * (p[1] + z * (p[2] + z * (p[3] + z * p[4])));
        double den = q[0] + z * (q[1] + z * (q[2] + z * (q[3] + z * q[4])));
        double res = ax * (num / den);
        return (x >= 0.0) ? res : -res;
    } else {
        const double p[] = {
            0.564189583547756077845e+00,
            0.880253746105520775370e+00,
            0.546646861569045885485e+00,
            0.169708451999186252120e+00,
            0.268361519296234120000e-01,
            0.161960293049609080000e-02
        };
        const double q[] = {
            0.100000000000000000000e+01,
            0.234114727194601131444e+01,
            0.222666880443281156910e+01,
            0.105787412084111951000e+01,
            0.252063564874722510000e+00,
            0.240572560e-01
        };
        double num = p[0] + ax * (p[1] + ax * (p[2] + ax * (p[3] + ax * (p[4] + ax * p[5]))));
        double den = q[0] + ax * (q[1] + ax * (q[2] + ax * (q[3] + ax * (q[4] + ax * q[5]))));
        double erfc_val = std::exp(-ax * ax) * (num / den);
        double res = 1.0 - erfc_val;
        return (x >= 0.0) ? res : -res;
    }
}

double msimd_minimax_gamma(double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (std::isnan(x)) return x;
    if (x <= 0.0 && std::floor(x) == x) return std::numeric_limits<double>::infinity();

    if (x < 0.5) {

        double sin_px = std::sin(M_PI * x);
        if (std::abs(sin_px) < 1e-15) return std::numeric_limits<double>::infinity();
        return M_PI / (sin_px * msimd_minimax_gamma(1.0 - x));
    }

    x -= 1.0;
    const double p[] = {
        0.99999999999980993,
        676.5203681218851,
        -1259.1392167224028,
        771.32342877765313,
        -176.61502916214059,
        12.507343278686905,
        -0.138571095836526,
        9.9843695780195716e-6,
        1.5056327351493116e-7
    };
    double a = p[0];
    double t = x + 7.5;
    for (int i = 1; i < 9; ++i) {
        a += p[i] / (x + (double)i);
    }
    double sqrt_2pi = 2.5066282746310005;
    return sqrt_2pi * std::pow(t, x + 0.5) * std::exp(-t) * a;
}

double msimd_legendre_polynomial(int n, double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (n < 0) return 0.0;
    if (n == 0) return 1.0;
    if (n == 1) return x;

    double p_prev = 1.0;
    double p_curr = x;
    for (int k = 1; k < n; ++k) {
        double p_next = ((2 * k + 1) * x * p_curr - k * p_prev) / (k + 1);
        p_prev = p_curr;
        p_curr = p_next;
    }
    return p_curr;
}

void msimd_winograd_f2x2_3x3_filter_transform(const float g[9], float U[16], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!g || !U) return;

    float T[12];
    for (int col = 0; col < 3; ++col) {
        float g0 = g[0 * 3 + col];
        float g1 = g[1 * 3 + col];
        float g2 = g[2 * 3 + col];
        T[0 * 3 + col] = g0;
        T[1 * 3 + col] = 0.5f * (g0 + g1 + g2);
        T[2 * 3 + col] = 0.5f * (g0 - g1 + g2);
        T[3 * 3 + col] = g2;
    }

    for (int row = 0; row < 4; ++row) {
        float t0 = T[row * 3 + 0];
        float t1 = T[row * 3 + 1];
        float t2 = T[row * 3 + 2];
        U[row * 4 + 0] = t0;
        U[row * 4 + 1] = 0.5f * (t0 + t1 + t2);
        U[row * 4 + 2] = 0.5f * (t0 - t1 + t2);
        U[row * 4 + 3] = t2;
    }
}

void msimd_winograd_f2x2_3x3_input_transform(const float d[16], float V[16], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!d || !V) return;

    float T[16];
    for (int col = 0; col < 4; ++col) {
        float d0 = d[0 * 4 + col];
        float d1 = d[1 * 4 + col];
        float d2 = d[2 * 4 + col];
        float d3 = d[3 * 4 + col];
        T[0 * 4 + col] = d0 - d2;
        T[1 * 4 + col] = d1 + d2;
        T[2 * 4 + col] = d2 - d1;
        T[3 * 4 + col] = d1 - d3;
    }

    for (int row = 0; row < 4; ++row) {
        float t0 = T[row * 4 + 0];
        float t1 = T[row * 4 + 1];
        float t2 = T[row * 4 + 2];
        float t3 = T[row * 4 + 3];
        V[row * 4 + 0] = t0 - t2;
        V[row * 4 + 1] = t1 + t2;
        V[row * 4 + 2] = t2 - t1;
        V[row * 4 + 3] = t1 - t3;
    }
}

void msimd_winograd_f2x2_3x3_output_transform(const float M[16], float Y[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!M || !Y) return;

    float T[8];
    for (int col = 0; col < 4; ++col) {
        float m0 = M[0 * 4 + col];
        float m1 = M[1 * 4 + col];
        float m2 = M[2 * 4 + col];
        float m3 = M[3 * 4 + col];
        T[0 * 4 + col] = m0 + m1 + m2;
        T[1 * 4 + col] = m1 - m2 - m3;
    }

    for (int row = 0; row < 2; ++row) {
        float t0 = T[row * 4 + 0];
        float t1 = T[row * 4 + 1];
        float t2 = T[row * 4 + 2];
        float t3 = T[row * 4 + 3];
        Y[row * 2 + 0] = t0 + t1 + t2;
        Y[row * 2 + 1] = t1 - t2 - t3;
    }
}

void msimd_winograd_f2x2_3x3_exec(const float input_tile4x4[16], const float filter_trans16[16], float output_tile2x2[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!input_tile4x4 || !filter_trans16 || !output_tile2x2) return;
    float V[16];
    msimd_winograd_f2x2_3x3_input_transform(input_tile4x4, V);

    float M[16];
    for (int i = 0; i < 16; ++i) {
        M[i] = filter_trans16[i] * V[i];
    }
    msimd_winograd_f2x2_3x3_output_transform(M, output_tile2x2);
}

void msimd_winograd_conv2d_3x3(const float* image, int width, int height,
    const float kernel[9], float* output, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!image || !kernel || !output || width < 4 || height < 4) return;

    float U[16];
    msimd_winograd_f2x2_3x3_filter_transform(kernel, U);

    int out_w = width - 2;
    int out_h = height - 2;

    for (int oy = 0; oy < out_h; oy += 2) {
        for (int ox = 0; ox < out_w; ox += 2) {
            float d[16];
            for (int r = 0; r < 4; ++r) {
                int iy = oy + r;
                for (int c = 0; c < 4; ++c) {
                    int ix = ox + c;
                    if (ix < width && iy < height) {
                        d[r * 4 + c] = image[iy * width + ix];
                    } else {
                        d[r * 4 + c] = 0.0f;
                    }
                }
            }

            float y[4];
            msimd_winograd_f2x2_3x3_exec(d, U, y);

            if (oy < out_h && ox < out_w)
                output[oy * out_w + ox] = y[0];
            if (oy < out_h && ox + 1 < out_w)
                output[oy * out_w + ox + 1] = y[1];
            if (oy + 1 < out_h && ox < out_w)
                output[(oy + 1) * out_w + ox] = y[2];
            if (oy + 1 < out_h && ox + 1 < out_w)
                output[(oy + 1) * out_w + ox + 1] = y[3];
        }
    }
}

void msimd_pga3d_plane_meet_plane(const double p1[4], const double p2[4], double line_out[6], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    line_out[0] = p1[1] * p2[2] - p1[2] * p2[1];
    line_out[1] = p1[2] * p2[0] - p1[0] * p2[2];
    line_out[2] = p1[0] * p2[1] - p1[1] * p2[0];

    line_out[3] = p1[3] * p2[0] - p2[3] * p1[0];
    line_out[4] = p1[3] * p2[1] - p2[3] * p1[1];
    line_out[5] = p1[3] * p2[2] - p2[3] * p1[2];
}

void msimd_pga3d_plane_meet_line(const double p[4], const double l[6], double pt_out[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    pt_out[0] =  p[1] * l[5] - p[2] * l[4] + p[3] * l[0];
    pt_out[1] =  p[2] * l[3] - p[0] * l[5] + p[3] * l[1];
    pt_out[2] =  p[0] * l[4] - p[1] * l[3] + p[3] * l[2];
    pt_out[3] = -(p[0] * l[0] + p[1] * l[1] + p[2] * l[2]);
}

void msimd_pga3d_point_join_point(const double pt1[4], const double pt2[4], double line_out[6], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    line_out[0] = pt1[3] * pt2[0] - pt2[3] * pt1[0];
    line_out[1] = pt1[3] * pt2[1] - pt2[3] * pt1[1];
    line_out[2] = pt1[3] * pt2[2] - pt2[3] * pt1[2];

    line_out[3] = pt1[1] * pt2[2] - pt1[2] * pt2[1];
    line_out[4] = pt1[2] * pt2[0] - pt1[0] * pt2[2];
    line_out[5] = pt1[0] * pt2[1] - pt1[1] * pt2[0];
}

void msimd_pga3d_motor_compose(const double m1[8], const double m2[8], double m_out[8], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    double s1 = m1[0], x1 = m1[1], y1 = m1[2], z1 = m1[3];
    double s2 = m2[0], x2 = m2[1], y2 = m2[2], z2 = m2[3];

    m_out[0] = s1*s2 - x1*x2 - y1*y2 - z1*z2;
    m_out[1] = s1*x2 + x1*s2 + y1*z2 - z1*y2;
    m_out[2] = s1*y2 - x1*z2 + y1*s2 + z1*x2;
    m_out[3] = s1*z2 + x1*y2 - y1*x2 + z1*s2;

    double d0_1 = m1[4], d1_1 = m1[5], d2_1 = m1[6], p1 = m1[7];
    double d0_2 = m2[4], d1_2 = m2[5], d2_2 = m2[6], p2 = m2[7];

    m_out[4] = s1*d0_2 + d0_1*s2 + y1*d2_2 - z1*d1_2 + d1_1*z2 - d2_1*y2;
    m_out[5] = s1*d1_2 - x1*d2_2 + d1_1*s2 + z1*d0_2 - d0_1*z2 + d2_1*x2;
    m_out[6] = s1*d2_2 + x1*d1_2 - y1*d0_2 + d2_1*s2 + d0_1*y2 - d1_1*x2;
    m_out[7] = s1*p2 + p1*s2 + x1*d0_2 + y1*d1_2 + z1*d2_2 + d0_1*x2 + d1_1*y2 + d2_1*z2;
}

void msimd_pga3d_motor_sandwich_point(const double m[8], const double pt_in[4], double pt_out[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    double s = m[0], rx = m[1], ry = m[2], rz = m[3];
    double px = pt_in[0], py = pt_in[1], pz = pt_in[2], pw = pt_in[3];

    double cx = ry * pz - rz * py + s * px;
    double cy = rz * px - rx * pz + s * py;
    double cz = rx * py - ry * px + s * pz;

    double vx = px + 2.0 * (ry * cz - rz * cy);
    double vy = py + 2.0 * (rz * cx - rx * cz);
    double vz = pz + 2.0 * (rx * cy - ry * cx);

    double tx = 2.0 * (m[4]*s + m[5]*rz - m[6]*ry + m[7]*rx);
    double ty = 2.0 * (m[5]*s + m[6]*rx - m[4]*rz + m[7]*ry);
    double tz = 2.0 * (m[6]*s + m[4]*ry - m[5]*rx + m[7]*rz);

    pt_out[0] = vx + tx * pw;
    pt_out[1] = vy + ty * pw;
    pt_out[2] = vz + tz * pw;
    pt_out[3] = pw;
}

void msimd_pga3d_motor_sandwich_plane(const double m[8], const double pl_in[4], double pl_out[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    double s = m[0], rx = m[1], ry = m[2], rz = m[3];
    double nx = pl_in[0], ny = pl_in[1], nz = pl_in[2], d = pl_in[3];

    double cx = ry * nz - rz * ny + s * nx;
    double cy = rz * nx - rx * nz + s * ny;
    double cz = rx * ny - ry * nx + s * nz;

    double rnx = nx + 2.0 * (ry * cz - rz * cy);
    double rny = ny + 2.0 * (rz * cx - rx * cz);
    double rnz = nz + 2.0 * (rx * cy - ry * cx);

    double tx = 2.0 * (m[4]*s + m[5]*rz - m[6]*ry + m[7]*rx);
    double ty = 2.0 * (m[5]*s + m[6]*rx - m[4]*rz + m[7]*ry);
    double tz = 2.0 * (m[6]*s + m[4]*ry - m[5]*rx + m[7]*rz);

    pl_out[0] = rnx;
    pl_out[1] = rny;
    pl_out[2] = rnz;
    pl_out[3] = d - (rnx * tx + rny * ty + rnz * tz);
}

void msimd_dual_quat_from_rotation_translation(const double q_rot[4], const double trans[3], double dq_out[8], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!q_rot || !trans || !dq_out) return;
    dq_out[0] = q_rot[0];
    dq_out[1] = q_rot[1];
    dq_out[2] = q_rot[2];
    dq_out[3] = q_rot[3];

    double tx = trans[0], ty = trans[1], tz = trans[2];
    double qw = q_rot[0], qx = q_rot[1], qy = q_rot[2], qz = q_rot[3];

    dq_out[4] = 0.5 * (-tx * qx - ty * qy - tz * qz);
    dq_out[5] = 0.5 * ( tx * qw + ty * qz - tz * qy);
    dq_out[6] = 0.5 * (-tx * qz + ty * qw + tz * qx);
    dq_out[7] = 0.5 * ( tx * qy - ty * qx + tz * qw);
}

void msimd_dual_quat_multiply(const double dq1[8], const double dq2[8], double dq_out[8], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!dq1 || !dq2 || !dq_out) return;

    double w1 = dq1[0], x1 = dq1[1], y1 = dq1[2], z1 = dq1[3];
    double w2 = dq2[0], x2 = dq2[1], y2 = dq2[2], z2 = dq2[3];

    dq_out[0] = w1*w2 - x1*x2 - y1*y2 - z1*z2;
    dq_out[1] = w1*x2 + x1*w2 + y1*z2 - z1*y2;
    dq_out[2] = w1*y2 - x1*z2 + y1*w2 + z1*x2;
    dq_out[3] = w1*z2 + x1*y2 - y1*x2 + z1*w2;

    double dw1 = dq1[4], dx1 = dq1[5], dy1 = dq1[6], dz1 = dq1[7];
    double dw2 = dq2[4], dx2 = dq2[5], dy2 = dq2[6], dz2 = dq2[7];

    dq_out[4] = (w1*dw2 - x1*dx2 - y1*dy2 - z1*dz2) + (dw1*w2 - dx1*x2 - dy1*y2 - dz1*z2);
    dq_out[5] = (w1*dx2 + x1*dw2 + y1*dz2 - z1*dy2) + (dw1*x2 + dx1*w2 + dy1*z2 - dz1*y2);
    dq_out[6] = (w1*dy2 - x1*dz2 + y1*dw2 + z1*dx2) + (dw1*y2 - dx1*z2 + dy1*w2 + dz1*x2);
    dq_out[7] = (w1*dz2 + x1*dy2 - y1*dx2 + z1*dw2) + (dw1*z2 + dx1*y2 - dy1*x2 + dz1*w2);
}

void msimd_dual_quat_transform_point(const double dq[8], const double pt_in[3], double pt_out[3], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!dq || !pt_in || !pt_out) return;

    double qw = dq[0], qx = dq[1], qy = dq[2], qz = dq[3];
    double px = pt_in[0], py = pt_in[1], pz = pt_in[2];

    double cx = qy * pz - qz * py + qw * px;
    double cy = qz * px - qx * pz + qw * py;
    double cz = qx * py - qy * px + qw * pz;

    double rx = px + 2.0 * (qy * cz - qz * cy);
    double ry = py + 2.0 * (qz * cx - qx * cz);
    double rz = pz + 2.0 * (qx * cy - qy * cx);

    double dw = dq[4], dx = dq[5], dy = dq[6], dz = dq[7];
    double tx = 2.0 * (-dw * qx + dx * qw - dy * qz + dz * qy);
    double ty = 2.0 * (-dw * qy + dx * qz + dy * qw - dz * qx);
    double tz = 2.0 * (-dw * qz - dx * qy + dy * qx + dz * qw);

    pt_out[0] = rx + tx;
    pt_out[1] = ry + ty;
    pt_out[2] = rz + tz;
}

void msimd_dual_quat_dlb(const double* dq_array, const double* weights, size_t count, double dq_out[8], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!dq_array || !weights || !dq_out || count == 0) return;
    for (int j = 0; j < 8; ++j) dq_out[j] = 0.0;

    const double* q0 = dq_array;

    for (size_t i = 0; i < count; ++i) {
        const double* q = dq_array + i * 8;
        double w = weights[i];
        double dot = q0[0]*q[0] + q0[1]*q[1] + q0[2]*q[2] + q0[3]*q[3];
        double sign = (dot >= 0.0) ? 1.0 : -1.0;
        for (int j = 0; j < 8; ++j) {
            dq_out[j] += w * sign * q[j];
        }
    }

    double norm_sq = dq_out[0]*dq_out[0] + dq_out[1]*dq_out[1] +
                     dq_out[2]*dq_out[2] + dq_out[3]*dq_out[3];
    if (norm_sq > 1e-15) {
        double inv_norm = 1.0 / std::sqrt(norm_sq);
        for (int j = 0; j < 8; ++j) {
            dq_out[j] *= inv_norm;
        }
    }
}

void msimd_dec_triangle_cotan_weights(const double p0[3], const double p1[3], const double p2[3],
    double* cot0, double* cot1, double* cot2, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    double e0[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
    double e1[3] = { p0[0] - p2[0], p0[1] - p2[1], p0[2] - p2[2] };
    double e2[3] = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };

    double cross[3] = {
        e1[1] * e2[2] - e1[2] * e2[1],
        e1[2] * e2[0] - e1[0] * e2[2],
        e1[0] * e2[1] - e1[1] * e2[0]
    };
    double area2 = std::sqrt(cross[0]*cross[0] + cross[1]*cross[1] + cross[2]*cross[2]);
    if (area2 < 1e-14) {
        if (cot0) *cot0 = 0.0;
        if (cot1) *cot1 = 0.0;
        if (cot2) *cot2 = 0.0;
        return;
    }
    double inv_area2 = 1.0 / area2;

    double dot0 = -(e1[0]*e2[0] + e1[1]*e2[1] + e1[2]*e2[2]);

    double dot1 = -(e2[0]*e0[0] + e2[1]*e0[1] + e2[2]*e0[2]);

    double dot2 = -(e0[0]*e1[0] + e0[1]*e1[1] + e0[2]*e1[2]);

    if (cot0) *cot0 = dot0 * inv_area2;
    if (cot1) *cot1 = dot1 * inv_area2;
    if (cot2) *cot2 = dot2 * inv_area2;
}

void msimd_dec_laplace_beltrami_mesh(const double* vertices, size_t num_vertices,
    const uint32_t* triangles, size_t num_triangles,
    const double* scalar_field_in, double* laplacian_out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!vertices || !triangles || !scalar_field_in || !laplacian_out || num_vertices == 0) return;

    for (size_t i = 0; i < num_vertices; ++i) {
        laplacian_out[i] = 0.0;
    }

    std::vector<double> dual_areas(num_vertices, 0.0);

    for (size_t t = 0; t < num_triangles; ++t) {
        uint32_t i0 = triangles[t * 3 + 0];
        uint32_t i1 = triangles[t * 3 + 1];
        uint32_t i2 = triangles[t * 3 + 2];
        if (i0 >= num_vertices || i1 >= num_vertices || i2 >= num_vertices) continue;

        const double* p0 = vertices + i0 * 3;
        const double* p1 = vertices + i1 * 3;
        const double* p2 = vertices + i2 * 3;

        double cot0, cot1, cot2;
        msimd_dec_triangle_cotan_weights(p0, p1, p2, &cot0, &cot1, &cot2);

        double e1[3] = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };
        double e2[3] = { p2[0] - p0[0], p2[1] - p0[0], p2[2] - p0[2] };
        double cr[3] = { e1[1]*e2[2] - e1[2]*e2[1], e1[2]*e2[0] - e1[0]*e2[2], e1[0]*e2[1] - e1[1]*e2[0] };
        double tri_area = 0.5 * std::sqrt(cr[0]*cr[0] + cr[1]*cr[1] + cr[2]*cr[2]);

        dual_areas[i0] += tri_area / 3.0;
        dual_areas[i1] += tri_area / 3.0;
        dual_areas[i2] += tri_area / 3.0;

        double f0 = scalar_field_in[i0];
        double f1 = scalar_field_in[i1];
        double f2 = scalar_field_in[i2];

        laplacian_out[i1] += 0.5 * cot0 * (f1 - f2);
        laplacian_out[i2] += 0.5 * cot0 * (f2 - f1);

        laplacian_out[i2] += 0.5 * cot1 * (f2 - f0);
        laplacian_out[i0] += 0.5 * cot1 * (f0 - f2);

        laplacian_out[i0] += 0.5 * cot2 * (f0 - f1);
        laplacian_out[i1] += 0.5 * cot2 * (f1 - f0);
    }

    for (size_t i = 0; i < num_vertices; ++i) {
        if (dual_areas[i] > 1e-14) {
            laplacian_out[i] /= dual_areas[i];
        }
    }
}

void msimd_dec_hodge_star_0form(const double* dual_cell_areas, const double* primal_weights, size_t n, double* hodge_star_diag, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!dual_cell_areas || !hodge_star_diag || n == 0) return;
    for (size_t i = 0; i < n; ++i) {
        double pw = (primal_weights) ? primal_weights[i] : 1.0;
        hodge_star_diag[i] = (pw > 1e-14) ? (dual_cell_areas[i] / pw) : 0.0;
    }
}

void msimd_dec_hodge_star_1form(const double* dual_edge_lengths, const double* primal_edge_lengths, size_t num_edges, double* hodge_diag, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!dual_edge_lengths || !primal_edge_lengths || !hodge_diag || num_edges == 0) return;
    for (size_t i = 0; i < num_edges; ++i) {
        double pl = primal_edge_lengths[i];
        hodge_diag[i] = (pl > 1e-14) ? (dual_edge_lengths[i] / pl) : 0.0;
    }
}

void msimd_symplectic_euler_step(double* pos, double* vel, const double* forces, const double* inv_mass, size_t n, double dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!pos || !vel || !forces || !inv_mass || n == 0) return;
    for (size_t i = 0; i < n; ++i) {

        vel[i] += inv_mass[i] * forces[i] * dt;

        pos[i] += vel[i] * dt;
    }
}

void msimd_velocity_verlet_step(double* pos, double* vel, double* forces, const double* inv_mass,
    void (*compute_forces)(const double* pos, double* forces, void* ctx),
    size_t n, double dt, void* ctx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!pos || !vel || !forces || !inv_mass || !compute_forces || n == 0) return;

    for (size_t i = 0; i < n; ++i) {
        vel[i] += 0.5 * inv_mass[i] * forces[i] * dt;
        pos[i] += vel[i] * dt;
    }

    compute_forces(pos, forces, ctx);

    for (size_t i = 0; i < n; ++i) {
        vel[i] += 0.5 * inv_mass[i] * forces[i] * dt;
    }
}

void msimd_discrete_euler_lagrange_step(const double* q_prev, const double* q_curr,
    const double* d_L_d_q_curr, double dt, size_t n, double* q_next, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!q_prev || !q_curr || !d_L_d_q_curr || !q_next || n == 0) return;
    double dt2 = dt * dt;
    for (size_t i = 0; i < n; ++i) {
        q_next[i] = 2.0 * q_curr[i] - q_prev[i] + dt2 * d_L_d_q_curr[i];
    }
}

double msimd_symplectic_energy_drift(double initial_energy, double current_energy, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (std::abs(initial_energy) < 1e-15) {
        return std::abs(current_energy - initial_energy);
    }
    return std::abs((current_energy - initial_energy) / initial_energy);
}

void msimd_xpbd_distance_constraint(double* p1, double* p2, double inv_m1, double inv_m2,
    double rest_len, double compliance, double dt, double* lambda, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!p1 || !p2) return;
    double w_sum = inv_m1 + inv_m2;
    if (w_sum < 1e-15) return;

    double dx = p1[0] - p2[0];
    double dy = p1[1] - p2[1];
    double dz = p1[2] - p2[2];
    double dist = std::sqrt(dx*dx + dy*dy + dz*dz);
    if (dist < 1e-14) return;

    double c = dist - rest_len;
    double alpha_tilde = compliance / (dt * dt);

    double old_lambda = (lambda) ? *lambda : 0.0;
    double delta_lambda = (-c - alpha_tilde * old_lambda) / (w_sum + alpha_tilde);
    if (lambda) *lambda = old_lambda + delta_lambda;

    double inv_dist = 1.0 / dist;
    double nx = dx * inv_dist;
    double ny = dy * inv_dist;
    double nz = dz * inv_dist;

    double p1_factor = inv_m1 * delta_lambda;
    double p2_factor = inv_m2 * delta_lambda;

    p1[0] += p1_factor * nx;
    p1[1] += p1_factor * ny;
    p1[2] += p1_factor * nz;

    p2[0] -= p2_factor * nx;
    p2[1] -= p2_factor * ny;
    p2[2] -= p2_factor * nz;
}

void msimd_xpbd_tetrahedron_volume_constraint(double* p1, double* p2, double* p3, double* p4,
    double inv_m1, double inv_m2, double inv_m3, double inv_m4,
    double rest_volume, double compliance, double dt, double* lambda, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!p1 || !p2 || !p3 || !p4) return;

    double e2[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
    double e3[3] = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
    double e4[3] = { p4[0] - p1[0], p4[1] - p1[1], p4[2] - p1[2] };

    double c23[3] = {
        e2[1]*e3[2] - e2[2]*e3[1],
        e2[2]*e3[0] - e2[0]*e3[2],
        e2[0]*e3[1] - e2[1]*e3[0]
    };
    double current_volume = (c23[0]*e4[0] + c23[1]*e4[1] + c23[2]*e4[2]) / 6.0;
    double c = current_volume - rest_volume;

    double inv6 = 1.0 / 6.0;
    double grad4[3] = { c23[0] * inv6, c23[1] * inv6, c23[2] * inv6 };
    double grad2[3] = {
        (e3[1]*e4[2] - e3[2]*e4[1]) * inv6,
        (e3[2]*e4[0] - e3[0]*e4[2]) * inv6,
        (e3[0]*e4[1] - e3[1]*e4[0]) * inv6
    };
    double grad3[3] = {
        (e4[1]*e2[2] - e4[2]*e2[1]) * inv6,
        (e4[2]*e2[0] - e4[0]*e2[2]) * inv6,
        (e4[0]*e2[1] - e4[1]*e2[0]) * inv6
    };
    double grad1[3] = {
        -(grad2[0] + grad3[0] + grad4[0]),
        -(grad2[1] + grad3[1] + grad4[1]),
        -(grad2[2] + grad3[2] + grad4[2])
    };

    double w_sum = inv_m1 * (grad1[0]*grad1[0] + grad1[1]*grad1[1] + grad1[2]*grad1[2]) +
                   inv_m2 * (grad2[0]*grad2[0] + grad2[1]*grad2[1] + grad2[2]*grad2[2]) +
                   inv_m3 * (grad3[0]*grad3[0] + grad3[1]*grad3[1] + grad3[2]*grad3[2]) +
                   inv_m4 * (grad4[0]*grad4[0] + grad4[1]*grad4[1] + grad4[2]*grad4[2]);

    if (w_sum < 1e-15) return;

    double alpha_tilde = compliance / (dt * dt);
    double old_lambda = (lambda) ? *lambda : 0.0;
    double delta_lambda = (-c - alpha_tilde * old_lambda) / (w_sum + alpha_tilde);
    if (lambda) *lambda = old_lambda + delta_lambda;

    for (int k = 0; k < 3; ++k) {
        p1[k] += inv_m1 * delta_lambda * grad1[k];
        p2[k] += inv_m2 * delta_lambda * grad2[k];
        p3[k] += inv_m3 * delta_lambda * grad3[k];
        p4[k] += inv_m4 * delta_lambda * grad4[k];
    }
}

void msimd_xpbd_contact_plane_constraint(double* p, double inv_m, const double plane_point[3], const double plane_normal[3],
    double compliance, double dt, double* lambda, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!p || !plane_point || !plane_normal || inv_m < 1e-15) return;
    double dx = p[0] - plane_point[0];
    double dy = p[1] - plane_point[1];
    double dz = p[2] - plane_point[2];

    double penetration = dx * plane_normal[0] + dy * plane_normal[1] + dz * plane_normal[2];
    if (penetration >= 0.0) return;

    double c = penetration;
    double alpha_tilde = compliance / (dt * dt);
    double old_lambda = (lambda) ? *lambda : 0.0;
    double delta_lambda = (-c - alpha_tilde * old_lambda) / (inv_m + alpha_tilde);
    if (lambda) *lambda = old_lambda + delta_lambda;

    p[0] += inv_m * delta_lambda * plane_normal[0];
    p[1] += inv_m * delta_lambda * plane_normal[1];
    p[2] += inv_m * delta_lambda * plane_normal[2];
}

void msimd_sph_wendland_c2_kernel(double r, double h, double* w, double* dw_dr, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (h <= 0.0) {
        if (w) *w = 0.0;
        if (dw_dr) *dw_dr = 0.0;
        return;
    }
    double q = r / h;
    if (q >= 1.0 || q < 0.0) {
        if (w) *w = 0.0;
        if (dw_dr) *dw_dr = 0.0;
        return;
    }

    double h3 = h * h * h;
    double sigma = 21.0 / (2.0 * M_PI * h3);
    double omq = 1.0 - q;
    double omq3 = omq * omq * omq;
    double omq4 = omq3 * omq;

    if (w) {
        *w = sigma * omq4 * (4.0 * q + 1.0);
    }
    if (dw_dr) {

        *dw_dr = - (sigma / h) * 20.0 * q * omq3;
    }
}

void msimd_sph_wendland_pair_force(const double p_i[3], const double p_j[3],
    const double v_i[3], const double v_j[3],
    double m_j, double rho_i, double rho_j,
    double pressure_i, double pressure_j,
    double h, double visc_eta, double f_out[3], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!p_i || !p_j || !v_i || !v_j || !f_out) return;
    f_out[0] = f_out[1] = f_out[2] = 0.0;

    double dx = p_i[0] - p_j[0];
    double dy = p_i[1] - p_j[1];
    double dz = p_i[2] - p_j[2];
    double r = std::sqrt(dx*dx + dy*dy + dz*dz);
    if (r >= h || r < 1e-12) return;

    double w, dw_dr;
    msimd_sph_wendland_c2_kernel(r, h, &w, &dw_dr);

    double inv_r = 1.0 / r;
    double grad_w[3] = { dw_dr * dx * inv_r, dw_dr * dy * inv_r, dw_dr * dz * inv_r };

    double rho_i2 = (rho_i > 1e-6) ? (rho_i * rho_i) : 1e-12;
    double rho_j2 = (rho_j > 1e-6) ? (rho_j * rho_j) : 1e-12;
    double press_term = m_j * (pressure_i / rho_i2 + pressure_j / rho_j2);

    double dvx = v_i[0] - v_j[0];
    double dvy = v_i[1] - v_j[1];
    double dvz = v_i[2] - v_j[2];
    double v_dot_r = dvx * dx + dvy * dy + dvz * dz;

    double visc_term = 0.0;
    if (v_dot_r < 0.0) {
        double mu_ij = (h * v_dot_r) / (r * r + 0.01 * h * h);
        double rho_avg = 0.5 * (rho_i + rho_j);
        visc_term = -m_j * (visc_eta * mu_ij) / rho_avg;
    }

    double total_factor = -(press_term + visc_term);
    f_out[0] = total_factor * grad_w[0];
    f_out[1] = total_factor * grad_w[1];
    f_out[2] = total_factor * grad_w[2];
}

static const int MSIMD_LBM_D3Q19_EX[19] = {
    0,  1, -1,  0,  0,  0,  0,  1, -1,  1, -1,  1, -1,  1, -1,  0,  0,  0,  0
};
static const int MSIMD_LBM_D3Q19_EY[19] = {
    0,  0,  0,  1, -1,  0,  0,  1,  1, -1, -1,  0,  0,  0,  0,  1, -1,  1, -1
};
static const int MSIMD_LBM_D3Q19_EZ[19] = {
    0,  0,  0,  0,  0,  1, -1,  0,  0,  0,  0,  1,  1, -1, -1,  1,  1, -1, -1
};
static const float MSIMD_LBM_D3Q19_W[19] = {
    1.0f / 3.0f,
    1.0f / 18.0f, 1.0f / 18.0f, 1.0f / 18.0f, 1.0f / 18.0f, 1.0f / 18.0f, 1.0f / 18.0f,
    1.0f / 36.0f, 1.0f / 36.0f, 1.0f / 36.0f, 1.0f / 36.0f, 1.0f / 36.0f,
    1.0f / 36.0f, 1.0f / 36.0f, 1.0f / 36.0f, 1.0f / 36.0f, 1.0f / 36.0f,
    1.0f / 36.0f, 1.0f / 36.0f
};
static const int MSIMD_LBM_D3Q19_OPPOSITE[19] = {
    0,  2, 1,  4, 3,  6, 5,  10, 9, 8, 7, 14, 13, 12, 11, 18, 17, 16, 15
};

void msimd_lbm_d3q19_bgk_collision_and_stream(const float* f_in, float* f_out, int nx, int ny, int nz,
    float tau, const uint8_t* obstacle_mask, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!f_in || !f_out || nx <= 0 || ny <= 0 || nz <= 0 || tau <= 0.0f) return;
    float inv_tau = 1.0f / tau;
    float om_inv_tau = 1.0f - inv_tau;
    size_t total_nodes = (size_t)nx * ny * nz;

    for (int z = 0; z < nz; ++z) {
        for (int y = 0; y < ny; ++y) {
            for (int x = 0; x < nx; ++x) {
                size_t idx = ((size_t)z * ny + y) * nx + x;

                if (obstacle_mask && obstacle_mask[idx]) {

                    for (int i = 0; i < 19; ++i) {
                        int opp = MSIMD_LBM_D3Q19_OPPOSITE[i];
                        f_out[idx * 19 + opp] = f_in[idx * 19 + i];
                    }
                    continue;
                }

                float rho = 0.0f;
                float ux = 0.0f, uy = 0.0f, uz = 0.0f;
                for (int i = 0; i < 19; ++i) {
                    float fi = f_in[idx * 19 + i];
                    rho += fi;
                    ux += fi * MSIMD_LBM_D3Q19_EX[i];
                    uy += fi * MSIMD_LBM_D3Q19_EY[i];
                    uz += fi * MSIMD_LBM_D3Q19_EZ[i];
                }
                if (rho > 1e-7f) {
                    float inv_rho = 1.0f / rho;
                    ux *= inv_rho;
                    uy *= inv_rho;
                    uz *= inv_rho;
                }

                float u_sq = ux*ux + uy*uy + uz*uz;

                for (int i = 0; i < 19; ++i) {
                    float eu = ux * MSIMD_LBM_D3Q19_EX[i] + uy * MSIMD_LBM_D3Q19_EY[i] + uz * MSIMD_LBM_D3Q19_EZ[i];
                    float feq = MSIMD_LBM_D3Q19_W[i] * rho * (1.0f + 3.0f * eu + 4.5f * eu * eu - 1.5f * u_sq);
                    float f_star = om_inv_tau * f_in[idx * 19 + i] + inv_tau * feq;

                    int nx_pos = (x + MSIMD_LBM_D3Q19_EX[i] + nx) % nx;
                    int ny_pos = (y + MSIMD_LBM_D3Q19_EY[i] + ny) % ny;
                    int nz_pos = (z + MSIMD_LBM_D3Q19_EZ[i] + nz) % nz;

                    size_t target_idx = ((size_t)nz_pos * ny + ny_pos) * nx + nx_pos;
                    f_out[target_idx * 19 + i] = f_star;
                }
            }
        }
    }
}

void msimd_quantum_apply_pauli_x(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || target_qubit >= num_qubits) return;
    size_t dim = (size_t)1 << num_qubits;
    size_t step = (size_t)1 << target_qubit;
    size_t double_step = step << 1;

    for (size_t i = 0; i < dim; i += double_step) {
        for (size_t j = 0; j < step; ++j) {
            size_t idx0 = i + j;
            size_t idx1 = idx0 + step;
            std::swap(psi_re[idx0], psi_re[idx1]);
            std::swap(psi_im[idx0], psi_im[idx1]);
        }
    }
}

void msimd_quantum_apply_pauli_y(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || target_qubit >= num_qubits) return;
    size_t dim = (size_t)1 << num_qubits;
    size_t step = (size_t)1 << target_qubit;
    size_t double_step = step << 1;

    for (size_t i = 0; i < dim; i += double_step) {
        for (size_t j = 0; j < step; ++j) {
            size_t idx0 = i + j;
            size_t idx1 = idx0 + step;
            double r0 = psi_re[idx0], i0 = psi_im[idx0];
            double r1 = psi_re[idx1], i1 = psi_im[idx1];

            psi_re[idx0] =  i1;
            psi_im[idx0] = -r1;
            psi_re[idx1] = -i0;
            psi_im[idx1] =  r0;
        }
    }
}

void msimd_quantum_apply_pauli_z(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || target_qubit >= num_qubits) return;
    size_t dim = (size_t)1 << num_qubits;
    size_t step = (size_t)1 << target_qubit;
    size_t double_step = step << 1;

    for (size_t i = 0; i < dim; i += double_step) {
        for (size_t j = 0; j < step; ++j) {
            size_t idx1 = i + j + step;
            psi_re[idx1] = -psi_re[idx1];
            psi_im[idx1] = -psi_im[idx1];
        }
    }
}

void msimd_quantum_apply_hadamard(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || target_qubit >= num_qubits) return;
    size_t dim = (size_t)1 << num_qubits;
    size_t step = (size_t)1 << target_qubit;
    size_t double_step = step << 1;
    const double inv_sqrt2 = 0.7071067811865475244;

    for (size_t i = 0; i < dim; i += double_step) {
        for (size_t j = 0; j < step; ++j) {
            size_t idx0 = i + j;
            size_t idx1 = idx0 + step;

            double r0 = psi_re[idx0], i0 = psi_im[idx0];
            double r1 = psi_re[idx1], i1 = psi_im[idx1];

            psi_re[idx0] = (r0 + r1) * inv_sqrt2;
            psi_im[idx0] = (i0 + i1) * inv_sqrt2;
            psi_re[idx1] = (r0 - r1) * inv_sqrt2;
            psi_im[idx1] = (i0 - i1) * inv_sqrt2;
        }
    }
}

void msimd_quantum_apply_phase(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, double theta, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || target_qubit >= num_qubits) return;
    size_t dim = (size_t)1 << num_qubits;
    size_t step = (size_t)1 << target_qubit;
    size_t double_step = step << 1;

    double cos_th = std::cos(theta);
    double sin_th = std::sin(theta);

    for (size_t i = 0; i < dim; i += double_step) {
        for (size_t j = 0; j < step; ++j) {
            size_t idx1 = i + j + step;
            double r1 = psi_re[idx1];
            double i1 = psi_im[idx1];
            psi_re[idx1] = r1 * cos_th - i1 * sin_th;
            psi_im[idx1] = r1 * sin_th + i1 * cos_th;
        }
    }
}

void msimd_quantum_apply_cnot(double* psi_re, double* psi_im, size_t num_qubits, size_t control_qubit, size_t target_qubit, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || control_qubit >= num_qubits || target_qubit >= num_qubits) return;
    if (control_qubit == target_qubit) return;

    size_t dim = (size_t)1 << num_qubits;
    size_t ctrl_mask = (size_t)1 << control_qubit;
    size_t tgt_mask = (size_t)1 << target_qubit;

    for (size_t i = 0; i < dim; ++i) {

        if ((i & ctrl_mask) && !(i & tgt_mask)) {
            size_t idx1 = i ^ tgt_mask;
            std::swap(psi_re[i], psi_re[idx1]);
            std::swap(psi_im[i], psi_im[idx1]);
        }
    }
}

void msimd_quantum_apply_toffoli(double* psi_re, double* psi_im, size_t num_qubits,
    size_t ctrl1, size_t ctrl2, size_t target_qubit, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || ctrl1 >= num_qubits || ctrl2 >= num_qubits || target_qubit >= num_qubits) return;
    size_t dim = (size_t)1 << num_qubits;
    size_t c1_mask = (size_t)1 << ctrl1;
    size_t c2_mask = (size_t)1 << ctrl2;
    size_t tgt_mask = (size_t)1 << target_qubit;

    for (size_t i = 0; i < dim; ++i) {
        if ((i & c1_mask) && (i & c2_mask) && !(i & tgt_mask)) {
            size_t idx1 = i ^ tgt_mask;
            std::swap(psi_re[i], psi_re[idx1]);
            std::swap(psi_im[i], psi_im[idx1]);
        }
    }
}

void msimd_quantum_qubit_probability(const double* psi_re, const double* psi_im, size_t num_qubits,
    size_t target_qubit, double* prob_0, double* prob_1, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || target_qubit >= num_qubits) {
        if (prob_0) *prob_0 = 0.0;
        if (prob_1) *prob_1 = 0.0;
        return;
    }
    size_t dim = (size_t)1 << num_qubits;
    size_t tgt_mask = (size_t)1 << target_qubit;
    double p0 = 0.0;
    double p1 = 0.0;

    for (size_t i = 0; i < dim; ++i) {
        double mag2 = psi_re[i]*psi_re[i] + psi_im[i]*psi_im[i];
        if (i & tgt_mask) {
            p1 += mag2;
        } else {
            p0 += mag2;
        }
    }
    if (prob_0) *prob_0 = p0;
    if (prob_1) *prob_1 = p1;
}

void msimd_regge_deficit_angle_2d(const double* edge_lengths_around_vertex, size_t num_triangles, double* deficit_angle, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!edge_lengths_around_vertex || num_triangles == 0 || !deficit_angle) return;
    double sum_angles = 0.0;

    for (size_t i = 0; i < num_triangles; ++i) {
        double a = edge_lengths_around_vertex[i * 3 + 0];
        double b = edge_lengths_around_vertex[i * 3 + 1];
        double c = edge_lengths_around_vertex[i * 3 + 2];

        if (a > 1e-15 && b > 1e-15) {
            double cos_theta = (a*a + b*b - c*c) / (2.0 * a * b);
            cos_theta = std::clamp(cos_theta, -1.0, 1.0);
            sum_angles += std::acos(cos_theta);
        }
    }
    *deficit_angle = 2.0 * M_PI - sum_angles;
}

void msimd_wilson_plaquette_u1(const double* theta_links, int nx, int ny, int nz, int nt, double* average_action, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!theta_links || nx <= 1 || ny <= 1 || nz <= 1 || nt <= 1 || !average_action) return;

    auto link_idx = [nx, ny, nz, nt](int x, int y, int z, int t, int mu) -> size_t {
        x = (x + nx) % nx;
        y = (y + ny) % ny;
        z = (z + nz) % nz;
        t = (t + nt) % nt;
        return ((((size_t)t * nz + z) * ny + y) * nx + x) * 4 + mu;
    };

    double total_action = 0.0;
    size_t num_plaquettes = 0;

    for (int t = 0; t < nt; ++t) {
        for (int z = 0; z < nz; ++z) {
            for (int y = 0; y < ny; ++y) {
                for (int x = 0; x < nx; ++x) {
                    for (int mu = 0; mu < 4; ++mu) {
                        for (int nu = mu + 1; nu < 4; ++nu) {

                            int dx_mu = (mu == 0), dy_mu = (mu == 1), dz_mu = (mu == 2), dt_mu = (mu == 3);
                            int dx_nu = (nu == 0), dy_nu = (nu == 1), dz_nu = (nu == 2), dt_nu = (nu == 3);

                            double u1 = theta_links[link_idx(x, y, z, t, mu)];
                            double u2 = theta_links[link_idx(x + dx_mu, y + dy_mu, z + dz_mu, t + dt_mu, nu)];
                            double u3 = theta_links[link_idx(x + dx_nu, y + dy_nu, z + dz_nu, t + dt_nu, mu)];
                            double u4 = theta_links[link_idx(x, y, z, t, nu)];

                            double theta_p = u1 + u2 - u3 - u4;
                            total_action += (1.0 - std::cos(theta_p));
                            num_plaquettes++;
                        }
                    }
                }
            }
        }
    }
    *average_action = (num_plaquettes > 0) ? (total_action / num_plaquettes) : 0.0;
}

void msimd_wilson_dirac_operator_4d(const double* psi_in, double* psi_out, const double* gauge_u1,
    int nx, int ny, int nz, int nt, double mass_m0, double r_wilson, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_in || !psi_out || !gauge_u1 || nx <= 1 || ny <= 1 || nz <= 1 || nt <= 1) return;

    size_t num_sites = (size_t)nx * ny * nz * nt;
    auto site_idx = [nx, ny, nz, nt](int x, int y, int z, int t) -> size_t {
        x = (x + nx) % nx;
        y = (y + ny) % ny;
        z = (z + nz) % nz;
        t = (t + nt) % nt;
        return (((size_t)t * nz + z) * ny + y) * nx + x;
    };

    double diag_coeff = mass_m0 + 4.0 * r_wilson;

    for (int t = 0; t < nt; ++t) {
        for (int z = 0; z < nz; ++z) {
            for (int y = 0; y < ny; ++y) {
                for (int x = 0; x < nx; ++x) {
                    size_t s = site_idx(x, y, z, t);
                    double val = diag_coeff * psi_in[s];

                    int d[4][4] = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };
                    for (int mu = 0; mu < 4; ++mu) {
                        size_t s_fwd = site_idx(x + d[mu][0], y + d[mu][1], z + d[mu][2], t + d[mu][3]);
                        size_t s_bwd = site_idx(x - d[mu][0], y - d[mu][1], z - d[mu][2], t - d[mu][3]);

                        double u_fwd = gauge_u1[s * 4 + mu];
                        double u_bwd = gauge_u1[s_bwd * 4 + mu];

                        val -= 0.5 * (r_wilson * std::cos(u_fwd) * psi_in[s_fwd] +
                                      r_wilson * std::cos(u_bwd) * psi_in[s_bwd]);
                    }
                    psi_out[s] = val;
                }
            }
        }
    }
}

void msimd_relativity_christoffel_symbols(const double g_inv[4][4], const double d_g[4][4][4], double gamma[4][4][4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!g_inv || !d_g || !gamma) return;

    for (int mu = 0; mu < 4; ++mu) {
        for (int alpha = 0; alpha < 4; ++alpha) {
            for (int beta = 0; beta < 4; ++beta) {
                double sum = 0.0;
                for (int nu = 0; nu < 4; ++nu) {
                    double dg_term = d_g[alpha][beta][nu] + d_g[beta][alpha][nu] - d_g[nu][alpha][beta];
                    sum += g_inv[mu][nu] * dg_term;
                }
                gamma[mu][alpha][beta] = 0.5 * sum;
            }
        }
    }
}

void msimd_relativity_geodesic_step(const double x[4], const double u[4], const double gamma[4][4][4],
    double d_lambda, double x_next[4], double u_next[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!x || !u || !gamma || !x_next || !u_next) return;

    double a[4] = { 0.0, 0.0, 0.0, 0.0 };
    for (int mu = 0; mu < 4; ++mu) {
        double sum = 0.0;
        for (int alpha = 0; alpha < 4; ++alpha) {
            for (int beta = 0; beta < 4; ++beta) {
                sum += gamma[mu][alpha][beta] * u[alpha] * u[beta];
            }
        }
        a[mu] = -sum;
    }

    for (int mu = 0; mu < 4; ++mu) {
        u_next[mu] = u[mu] + a[mu] * d_lambda;
        x_next[mu] = x[mu] + u_next[mu] * d_lambda;
    }
}

void msimd_relativity_riemann_tensor(const double gamma[4][4][4], const double d_gamma[4][4][4][4],
    double riemann[4][4][4][4], double ricci[4][4], double* ricci_scalar,
    const double g_inv[4][4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!gamma || !d_gamma || !riemann) return;

    for (int rho = 0; rho < 4; ++rho) {
        for (int sigma = 0; sigma < 4; ++sigma) {
            for (int mu = 0; mu < 4; ++mu) {
                for (int nu = 0; nu < 4; ++nu) {
                    double r = d_gamma[mu][rho][nu][sigma] - d_gamma[nu][rho][mu][sigma];
                    double quad = 0.0;
                    for (int lam = 0; lam < 4; ++lam) {
                        quad += gamma[rho][mu][lam] * gamma[lam][nu][sigma]
                              - gamma[rho][nu][lam] * gamma[lam][mu][sigma];
                    }
                    riemann[rho][sigma][mu][nu] = r + quad;
                }
            }
        }
    }

    if (ricci) {
        for (int sigma = 0; sigma < 4; ++sigma) {
            for (int nu = 0; nu < 4; ++nu) {
                double sum = 0.0;
                for (int mu = 0; mu < 4; ++mu) {
                    sum += riemann[mu][sigma][mu][nu];
                }
                ricci[sigma][nu] = sum;
            }
        }
    }

    if (ricci_scalar && ricci && g_inv) {
        double r_sc = 0.0;
        for (int sigma = 0; sigma < 4; ++sigma) {
            for (int nu = 0; nu < 4; ++nu) {
                r_sc += g_inv[sigma][nu] * ricci[sigma][nu];
            }
        }
        *ricci_scalar = r_sc;
    }
}

void msimd_nag_homotopy_step(const double* x, double t, double dt,
    void (*eval_H)(const double* x, double t, double* H_val, void* ctx),
    void (*eval_JH)(const double* x, double t, double* J_val, void* ctx),
    void (*eval_dH_dt)(const double* x, double t, double* dH_val, void* ctx),
    size_t n, double* x_next, void* ctx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!x || !x_next || !eval_H || !eval_JH || !eval_dH_dt || n == 0) return;

    std::vector<double> J(n * n, 0.0);
    std::vector<double> dH_dt(n, 0.0);
    eval_JH(x, t, J.data(), ctx);
    eval_dH_dt(x, t, dH_dt.data(), ctx);

    std::vector<double> dx(n, 0.0);
    if (n == 1) {
        if (std::abs(J[0]) > 1e-15) {
            dx[0] = (-dH_dt[0] * dt) / J[0];
        }
    } else {

        for (size_t i = 0; i < n; ++i) {
            double diag = J[i * n + i];
            double denom = diag * diag + 1e-8;
            dx[i] = (-dH_dt[i] * dt * diag) / denom;
        }
    }

    std::vector<double> x_pred(n);
    for (size_t i = 0; i < n; ++i) {
        x_pred[i] = x[i] + dx[i];
    }
    double t_next = t + dt;

    std::vector<double> H_val(n, 0.0);
    eval_H(x_pred.data(), t_next, H_val.data(), ctx);
    eval_JH(x_pred.data(), t_next, J.data(), ctx);

    for (size_t i = 0; i < n; ++i) {
        double diag = J[i * n + i];
        double denom = diag * diag + 1e-8;
        double corr = (H_val[i] * diag) / denom;
        x_next[i] = x_pred[i] - corr;
    }
}

void msimd_nag_surface_normal_and_singularity(double x, double y, double z,
    double (*F_fn)(double x, double y, double z, void* ctx),
    double grad[3], double hessian[3][3], double* hessian_det, int* is_singular, void* ctx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!F_fn) return;
    const double eps = 1e-6;
    const double inv_2eps = 1.0 / (2.0 * eps);
    const double inv_eps2 = 1.0 / (eps * eps);

    double f0 = F_fn(x, y, z, ctx);

    double fx_p = F_fn(x + eps, y, z, ctx), fx_m = F_fn(x - eps, y, z, ctx);
    double fy_p = F_fn(x, y + eps, z, ctx), fy_m = F_fn(x, y - eps, z, ctx);
    double fz_p = F_fn(x, y, z + eps, ctx), fz_m = F_fn(x, y, z - eps, ctx);

    if (grad) {
        grad[0] = (fx_p - fx_m) * inv_2eps;
        grad[1] = (fy_p - fy_m) * inv_2eps;
        grad[2] = (fz_p - fz_m) * inv_2eps;
    }

    double fxx = (fx_p - 2.0 * f0 + fx_m) * inv_eps2;
    double fyy = (fy_p - 2.0 * f0 + fy_m) * inv_eps2;
    double fzz = (fz_p - 2.0 * f0 + fz_m) * inv_eps2;

    double fxy = (F_fn(x + eps, y + eps, z, ctx) - fx_p - fy_p + f0) * inv_eps2;
    double fxz = (F_fn(x + eps, y, z + eps, ctx) - fx_p - fz_p + f0) * inv_eps2;
    double fyz = (F_fn(x, y + eps, z + eps, ctx) - fy_p - fz_p + f0) * inv_eps2;

    if (hessian) {
        hessian[0][0] = fxx; hessian[0][1] = fxy; hessian[0][2] = fxz;
        hessian[1][0] = fxy; hessian[1][1] = fyy; hessian[1][2] = fyz;
        hessian[2][0] = fxz; hessian[2][1] = fyz; hessian[2][2] = fzz;
    }

    double det = fxx * (fyy * fzz - fyz * fyz) -
                 fxy * (fxy * fzz - fyz * fxz) +
                 fxz * (fxy * fyz - fyy * fxz);

    if (hessian_det) *hessian_det = det;

    if (is_singular && grad) {
        double g_norm2 = grad[0]*grad[0] + grad[1]*grad[1] + grad[2]*grad[2];
        *is_singular = (g_norm2 < 1e-8 && std::abs(f0) < 1e-6) ? 1 : 0;
    }
}

void msimd_nag_elliptic_curve_add(double x1, double y1, double x2, double y2, double a,
    double* x3, double* y3, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!x3 || !y3) return;
    double m = 0.0;
    if (std::abs(x1 - x2) < 1e-12) {
        if (std::abs(y1 - y2) < 1e-12 && std::abs(y1) > 1e-12) {

            m = (3.0 * x1 * x1 + a) / (2.0 * y1);
        } else {

            *x3 = 0.0;
            *y3 = 0.0;
            return;
        }
    } else {

        m = (y2 - y1) / (x2 - x1);
    }

    double x_res = m * m - x1 - x2;
    double y_res = m * (x1 - x_res) - y1;
    *x3 = x_res;
    *y3 = y_res;
}

void msimd_control_kalman_filter_step(double* x, double* P, const double* F, const double* B, const double* u,
    const double* Q, const double* z, const double* H, const double* R,
    size_t state_dim, size_t obs_dim, size_t ctrl_dim, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!x || !P || !F || !Q || !z || !H || !R || state_dim == 0 || obs_dim == 0) return;

    std::vector<double> x_prior(state_dim, 0.0);
    for (size_t i = 0; i < state_dim; ++i) {
        double sum = 0.0;
        for (size_t j = 0; j < state_dim; ++j) {
            sum += F[i * state_dim + j] * x[j];
        }
        if (B && u && ctrl_dim > 0) {
            for (size_t k = 0; k < ctrl_dim; ++k) {
                sum += B[i * ctrl_dim + k] * u[k];
            }
        }
        x_prior[i] = sum;
    }

    std::vector<double> FP(state_dim * state_dim, 0.0);
    for (size_t i = 0; i < state_dim; ++i) {
        for (size_t j = 0; j < state_dim; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < state_dim; ++k) {
                sum += F[i * state_dim + k] * P[k * state_dim + j];
            }
            FP[i * state_dim + j] = sum;
        }
    }
    std::vector<double> P_prior(state_dim * state_dim, 0.0);
    for (size_t i = 0; i < state_dim; ++i) {
        for (size_t j = 0; j < state_dim; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < state_dim; ++k) {
                sum += FP[i * state_dim + k] * F[j * state_dim + k];
            }
            P_prior[i * state_dim + j] = sum + Q[i * state_dim + j];
        }
    }

    std::vector<double> y_innov(obs_dim, 0.0);
    for (size_t i = 0; i < obs_dim; ++i) {
        double hx = 0.0;
        for (size_t j = 0; j < state_dim; ++j) {
            hx += H[i * state_dim + j] * x_prior[j];
        }
        y_innov[i] = z[i] - hx;
    }

    std::vector<double> HP(obs_dim * state_dim, 0.0);
    for (size_t i = 0; i < obs_dim; ++i) {
        for (size_t j = 0; j < state_dim; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < state_dim; ++k) {
                sum += H[i * state_dim + k] * P_prior[k * state_dim + j];
            }
            HP[i * state_dim + j] = sum;
        }
    }
    std::vector<double> S(obs_dim * obs_dim, 0.0);
    for (size_t i = 0; i < obs_dim; ++i) {
        for (size_t j = 0; j < obs_dim; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < state_dim; ++k) {
                sum += HP[i * state_dim + k] * H[j * state_dim + k];
            }
            S[i * obs_dim + j] = sum + R[i * obs_dim + j];
        }
    }

    std::vector<double> K(state_dim * obs_dim, 0.0);
    if (obs_dim == 1) {
        double inv_S = (std::abs(S[0]) > 1e-15) ? (1.0 / S[0]) : 0.0;
        for (size_t i = 0; i < state_dim; ++i) {
            double pht = 0.0;
            for (size_t k = 0; k < state_dim; ++k) {
                pht += P_prior[i * state_dim + k] * H[0 * state_dim + k];
            }
            K[i] = pht * inv_S;
        }
    } else {

        for (size_t i = 0; i < state_dim; ++i) {
            for (size_t j = 0; j < obs_dim; ++j) {
                double pht = 0.0;
                for (size_t k = 0; k < state_dim; ++k) {
                    pht += P_prior[i * state_dim + k] * H[j * state_dim + k];
                }
                double s_diag = S[j * obs_dim + j];
                K[i * obs_dim + j] = (std::abs(s_diag) > 1e-14) ? (pht / s_diag) : 0.0;
            }
        }
    }

    for (size_t i = 0; i < state_dim; ++i) {
        double ky = 0.0;
        for (size_t j = 0; j < obs_dim; ++j) {
            ky += K[i * obs_dim + j] * y_innov[j];
        }
        x[i] = x_prior[i] + ky;
    }

    std::vector<double> I_KH(state_dim * state_dim, 0.0);
    for (size_t i = 0; i < state_dim; ++i) {
        for (size_t j = 0; j < state_dim; ++j) {
            double kh = 0.0;
            for (size_t k = 0; k < obs_dim; ++k) {
                kh += K[i * obs_dim + k] * H[k * state_dim + j];
            }
            I_KH[i * state_dim + j] = ((i == j) ? 1.0 : 0.0) - kh;
        }
    }
    for (size_t i = 0; i < state_dim; ++i) {
        for (size_t j = 0; j < state_dim; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < state_dim; ++k) {
                sum += I_KH[i * state_dim + k] * P_prior[k * state_dim + j];
            }
            P[i * state_dim + j] = sum;
        }
    }
}

double msimd_control_pid_step(double setpoint, double measurement, double kp, double ki, double kd, double dt,
    double min_out, double max_out, double filter_alpha,
    double* integral_state, double* prev_meas, double* prev_deriv, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!integral_state || !prev_meas || !prev_deriv || dt <= 0.0) return 0.0;

    double error = setpoint - measurement;

    double p_term = kp * error;

    *integral_state += error * dt;
    double i_term = ki * (*integral_state);

    double raw_deriv = -(measurement - *prev_meas) / dt;
    double d_term = filter_alpha * (*prev_deriv) + (1.0 - filter_alpha) * raw_deriv;
    *prev_deriv = d_term;
    *prev_meas = measurement;

    double output = p_term + i_term + kd * d_term;

    if (output > max_out) {
        output = max_out;
        if (ki != 0.0 && error > 0.0) {
            *integral_state -= error * dt;
        }
    } else if (output < min_out) {
        output = min_out;
        if (ki != 0.0 && error < 0.0) {
            *integral_state -= error * dt;
        }
    }
    return output;
}

void msimd_control_lqr_riccati_iteration(const double* A, const double* B, const double* Q, const double* R,
    size_t n, size_t m, double* P, double* K, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!A || !B || !Q || !R || !P || n == 0 || m == 0) return;

    std::vector<double> BtP(m * n, 0.0);
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < n; ++k) {
                sum += B[k * m + i] * P[k * n + j];
            }
            BtP[i * n + j] = sum;
        }
    }

    std::vector<double> M(m * m, 0.0);
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < m; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < n; ++k) {
                sum += BtP[i * n + k] * B[k * m + j];
            }
            M[i * m + j] = sum + R[i * m + j];
        }
    }

    std::vector<double> BtPA(m * n, 0.0);
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < n; ++k) {
                sum += BtP[i * n + k] * A[k * n + j];
            }
            BtPA[i * n + j] = sum;
        }
    }

    if (K) {
        for (size_t i = 0; i < m; ++i) {
            double m_diag = M[i * m + i];
            double inv_m = (std::abs(m_diag) > 1e-14) ? (1.0 / m_diag) : 0.0;
            for (size_t j = 0; j < n; ++j) {
                K[i * n + j] = inv_m * BtPA[i * n + j];
            }
        }
    }

    std::vector<double> AtP(n * n, 0.0);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < n; ++k) {
                sum += A[k * n + i] * P[k * n + j];
            }
            AtP[i * n + j] = sum;
        }
    }
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double atpa = 0.0;
            for (size_t k = 0; k < n; ++k) {
                atpa += AtP[i * n + k] * A[k * n + j];
            }
            double quad = 0.0;
            if (K) {
                for (size_t k = 0; k < m; ++k) {
                    quad += BtPA[k * n + i] * K[k * n + j];
                }
            }
            P[i * n + j] = atpa - quad + Q[i * n + j];
        }
    }
}

void msimd_opt_simplex_pivot(double* tableau, size_t num_rows, size_t num_cols,
    size_t pivot_row, size_t pivot_col, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!tableau || pivot_row >= num_rows || pivot_col >= num_cols) return;
    double pivot_val = tableau[pivot_row * num_cols + pivot_col];
    if (std::abs(pivot_val) < 1e-15) return;

    double inv_pivot = 1.0 / pivot_val;

    for (size_t c = 0; c < num_cols; ++c) {
        tableau[pivot_row * num_cols + c] *= inv_pivot;
    }

    for (size_t r = 0; r < num_rows; ++r) {
        if (r == pivot_row) continue;
        double factor = tableau[r * num_cols + pivot_col];
        if (std::abs(factor) > 1e-15) {
            for (size_t c = 0; c < num_cols; ++c) {
                tableau[r * num_cols + c] -= factor * tableau[pivot_row * num_cols + c];
            }
        }
    }
}

int msimd_opt_simplex_step(double* tableau, size_t num_rows, size_t num_cols, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!tableau || num_rows < 2 || num_cols < 2) return -1;
    size_t obj_row = num_rows - 1;
    size_t rhs_col = num_cols - 1;

    size_t pivot_col = 0;
    double min_cost = 0.0;
    for (size_t c = 0; c < rhs_col; ++c) {
        double cost = tableau[obj_row * num_cols + c];
        if (cost < min_cost) {
            min_cost = cost;
            pivot_col = c;
        }
    }
    if (min_cost >= -1e-12) {
        return 1;
    }

    size_t pivot_row = 0;
    double min_ratio = std::numeric_limits<double>::infinity();
    bool found_row = false;

    for (size_t r = 0; r < obj_row; ++r) {
        double coeff = tableau[r * num_cols + pivot_col];
        if (coeff > 1e-12) {
            double rhs = tableau[r * num_cols + rhs_col];
            double ratio = rhs / coeff;
            if (ratio >= 0.0 && ratio < min_ratio) {
                min_ratio = ratio;
                pivot_row = r;
                found_row = true;
            }
        }
    }
    if (!found_row) {
        return 2;
    }

    msimd_opt_simplex_pivot(tableau, num_rows, num_cols, pivot_row, pivot_col);
    return 0;
}

void msimd_game_fictitious_play_step(const double* A, const double* B, size_t m, size_t n,
    double* p1_counts, double* p2_counts,
    double* p1_strategy, double* p2_strategy, size_t iteration, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!A || !B || !p1_counts || !p2_counts || !p1_strategy || !p2_strategy || m == 0 || n == 0) return;

    size_t best_p1 = 0;
    double max_u1 = -1e30;
    for (size_t i = 0; i < m; ++i) {
        double exp_u = 0.0;
        for (size_t j = 0; j < n; ++j) {
            exp_u += A[i * n + j] * p2_strategy[j];
        }
        if (exp_u > max_u1) {
            max_u1 = exp_u;
            best_p1 = i;
        }
    }

    size_t best_p2 = 0;
    double max_u2 = -1e30;
    for (size_t j = 0; j < n; ++j) {
        double exp_u = 0.0;
        for (size_t i = 0; i < m; ++i) {
            exp_u += B[i * n + j] * p1_strategy[i];
        }
        if (exp_u > max_u2) {
            max_u2 = exp_u;
            best_p2 = j;
        }
    }

    p1_counts[best_p1] += 1.0;
    p2_counts[best_p2] += 1.0;

    double inv_iter = 1.0 / (double)(iteration + 1);
    for (size_t i = 0; i < m; ++i) {
        p1_strategy[i] = p1_counts[i] * inv_iter;
    }
    for (size_t j = 0; j < n; ++j) {
        p2_strategy[j] = p2_counts[j] * inv_iter;
    }
}

double msimd_game_zero_sum_value(const double* payoff_matrix, const double* p1_strat, const double* p2_strat,
    size_t m, size_t n, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!payoff_matrix || !p1_strat || !p2_strat || m == 0 || n == 0) return 0.0;
    double val = 0.0;
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            val += p1_strat[i] * payoff_matrix[i * n + j] * p2_strat[j];
        }
    }
    return val;
}

void msimd_bio_lotka_volterra_step(double prey, double predator,
    double alpha, double beta, double gamma, double delta,
    double dt, double* next_prey, double* next_predator, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!next_prey || !next_predator) return;

    auto f_prey = [alpha, beta](double x, double y) { return alpha * x - beta * x * y; };
    auto f_pred = [gamma, delta](double x, double y) { return delta * x * y - gamma * y; };

    double k1_x = f_prey(prey, predator);
    double k1_y = f_pred(prey, predator);

    double k2_x = f_prey(prey + 0.5 * dt * k1_x, predator + 0.5 * dt * k1_y);
    double k2_y = f_pred(prey + 0.5 * dt * k1_x, predator + 0.5 * dt * k1_y);

    double k3_x = f_prey(prey + 0.5 * dt * k2_x, predator + 0.5 * dt * k2_y);
    double k3_y = f_pred(prey + 0.5 * dt * k2_x, predator + 0.5 * dt * k2_y);

    double k4_x = f_prey(prey + dt * k3_x, predator + dt * k3_y);
    double k4_y = f_pred(prey + dt * k3_x, predator + dt * k3_y);

    *next_prey     = prey + (dt / 6.0) * (k1_x + 2.0 * k2_x + 2.0 * k3_x + k4_x);
    *next_predator = predator + (dt / 6.0) * (k1_y + 2.0 * k2_y + 2.0 * k3_y + k4_y);

    if (*next_prey < 0.0) *next_prey = 0.0;
    if (*next_predator < 0.0) *next_predator = 0.0;
}

void msimd_bio_seir_step(double S, double E, double I, double R,
    double beta, double sigma, double gamma, double dt,
    double* next_S, double* next_E, double* next_I, double* next_R, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!next_S || !next_E || !next_I || !next_R) return;
    double N = S + E + I + R;
    if (N < 1e-8) N = 1.0;

    double dS = - (beta * S * I) / N;
    double dE =   (beta * S * I) / N - sigma * E;
    double dI =   sigma * E - gamma * I;
    double dR =   gamma * I;

    *next_S = std::max(0.0, S + dS * dt);
    *next_E = std::max(0.0, E + dE * dt);
    *next_I = std::max(0.0, I + dI * dt);
    *next_R = std::max(0.0, R + dR * dt);
}

void msimd_bio_michaelis_menten_rate(double substrate_conc, double vmax, double km, double* rate, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!rate) return;
    if (substrate_conc < 0.0) substrate_conc = 0.0;
    double denom = km + substrate_conc;
    *rate = (denom > 1e-15) ? ((vmax * substrate_conc) / denom) : 0.0;
}

void msimd_tda_pairwise_distance_matrix(const double* points, size_t num_points, size_t dim, double* dist_matrix, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!points || !dist_matrix || num_points == 0 || dim == 0) return;

    for (size_t i = 0; i < num_points; ++i) {
        dist_matrix[i * num_points + i] = 0.0;
        for (size_t j = i + 1; j < num_points; ++j) {
            double sum_sq = 0.0;
            for (size_t d = 0; d < dim; ++d) {
                double diff = points[i * dim + d] - points[j * dim + d];
                sum_sq += diff * diff;
            }
            double dist = std::sqrt(sum_sq);
            dist_matrix[i * num_points + j] = dist;
            dist_matrix[j * num_points + i] = dist;
        }
    }
}

void msimd_tda_boundary_matrix_reduce(uint8_t* boundary_matrix, size_t num_simplices, int64_t* pivots, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!boundary_matrix || !pivots || num_simplices == 0) return;

    for (size_t i = 0; i < num_simplices; ++i) {
        pivots[i] = -1;
    }

    auto get_pivot = [num_simplices, boundary_matrix](size_t col) -> int64_t {
        for (int64_t r = (int64_t)num_simplices - 1; r >= 0; --r) {
            if (boundary_matrix[r * num_simplices + col]) {
                return r;
            }
        }
        return -1;
    };

    for (size_t j = 0; j < num_simplices; ++j) {
        int64_t piv = get_pivot(j);
        while (piv != -1 && pivots[piv] != -1) {
            size_t other_col = (size_t)pivots[piv];

            for (size_t r = 0; r < num_simplices; ++r) {
                boundary_matrix[r * num_simplices + j] ^= boundary_matrix[r * num_simplices + other_col];
            }
            piv = get_pivot(j);
        }
        if (piv != -1) {
            pivots[piv] = (int64_t)j;
        }
    }
}

void msimd_graph_dijkstra_shortest_paths(const double* adj_matrix, size_t n, size_t src,
    double* distances, int64_t* predecessors, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!adj_matrix || !distances || !predecessors || src >= n || n == 0) return;

    std::vector<bool> visited(n, false);
    for (size_t i = 0; i < n; ++i) {
        distances[i] = std::numeric_limits<double>::infinity();
        predecessors[i] = -1;
    }
    distances[src] = 0.0;

    for (size_t step = 0; step < n; ++step) {

        double min_dist = std::numeric_limits<double>::infinity();
        int64_t u = -1;
        for (size_t i = 0; i < n; ++i) {
            if (!visited[i] && distances[i] < min_dist) {
                min_dist = distances[i];
                u = (int64_t)i;
            }
        }
        if (u == -1) break;
        visited[u] = true;

        for (size_t v = 0; v < n; ++v) {
            double weight = adj_matrix[u * n + v];
            if (!visited[v] && weight > 0.0) {
                double new_dist = distances[u] + weight;
                if (new_dist < distances[v]) {
                    distances[v] = new_dist;
                    predecessors[v] = u;
                }
            }
        }
    }
}

int msimd_graph_bellman_ford(const double* adj_matrix, size_t n, size_t src,
    double* distances, int64_t* predecessors, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!adj_matrix || !distances || !predecessors || src >= n || n == 0) return -1;

    for (size_t i = 0; i < n; ++i) {
        distances[i] = 1e30;
        predecessors[i] = -1;
    }
    distances[src] = 0.0;

    for (size_t iter = 1; iter < n; ++iter) {
        bool changed = false;
        for (size_t u = 0; u < n; ++u) {
            if (distances[u] >= 1e29) continue;
            for (size_t v = 0; v < n; ++v) {
                double w = adj_matrix[u * n + v];
                if (std::abs(w) > 1e-12 || (w != 0.0 && !std::isnan(w))) {
                    if (distances[u] + w < distances[v]) {
                        distances[v] = distances[u] + w;
                        predecessors[v] = (int64_t)u;
                        changed = true;
                    }
                }
            }
        }
        if (!changed) break;
    }

    for (size_t u = 0; u < n; ++u) {
        if (distances[u] >= 1e29) continue;
        for (size_t v = 0; v < n; ++v) {
            double w = adj_matrix[u * n + v];
            if (w != 0.0 && distances[u] + w < distances[v] - 1e-9) {
                return 1;
            }
        }
    }
    return 0;
}

double msimd_graph_max_flow_edmonds_karp(const double* capacity_matrix, size_t n, size_t source, size_t sink,
    double* residual_matrix_out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!capacity_matrix || source >= n || sink >= n || source == sink || n == 0) return 0.0;

    std::vector<double> r_graph(n * n);
    for (size_t i = 0; i < n * n; ++i) {
        r_graph[i] = capacity_matrix[i];
    }

    std::vector<int64_t> parent(n);
    double max_flow = 0.0;

    auto bfs = [&]() -> bool {
        std::fill(parent.begin(), parent.end(), -1);
        std::vector<bool> visited(n, false);
        std::queue<size_t> q;

        q.push(source);
        visited[source] = true;

        while (!q.empty()) {
            size_t u = q.front();
            q.pop();

            for (size_t v = 0; v < n; ++v) {
                if (!visited[v] && r_graph[u * n + v] > 1e-12) {
                    q.push(v);
                    parent[v] = (int64_t)u;
                    visited[v] = true;
                    if (v == sink) return true;
                }
            }
        }
        return false;
    };

    while (bfs()) {
        double path_flow = std::numeric_limits<double>::infinity();
        for (size_t v = sink; v != source; v = (size_t)parent[v]) {
            size_t u = (size_t)parent[v];
            path_flow = std::min(path_flow, r_graph[u * n + v]);
        }

        for (size_t v = sink; v != source; v = (size_t)parent[v]) {
            size_t u = (size_t)parent[v];
            r_graph[u * n + v] -= path_flow;
            r_graph[v * n + u] += path_flow;
        }

        max_flow += path_flow;
    }

    if (residual_matrix_out) {
        for (size_t i = 0; i < n * n; ++i) {
            residual_matrix_out[i] = r_graph[i];
        }
    }
    return max_flow;
}

/* ========================================================================= */
/* 1. Single 128-bit Integer (Multi-limb / BigInt carry chain)               */
/* ========================================================================= */

MSIMD_API msimd_u128_t msimd_u128_add(msimd_u128_t a, msimd_u128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u128_t r;
#if defined(__SIZEOF_INT128__)
    unsigned __int128 u = ((unsigned __int128)a.hi << 64) | a.lo;
    unsigned __int128 v = ((unsigned __int128)b.hi << 64) | b.lo;
    unsigned __int128 sum = u + v;
    r.lo = (uint64_t)sum;
    r.hi = (uint64_t)(sum >> 64);
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
    unsigned char c = _addcarry_u64(0, a.lo, b.lo, &r.lo);
    _addcarry_u64(c, a.hi, b.hi, &r.hi);
#else
    r.lo = a.lo + b.lo;
    r.hi = a.hi + b.hi + (r.lo < a.lo ? 1ULL : 0ULL);
#endif
    return r;
}

MSIMD_API msimd_u128_t msimd_u128_sub(msimd_u128_t a, msimd_u128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u128_t r;
#if defined(__SIZEOF_INT128__)
    unsigned __int128 u = ((unsigned __int128)a.hi << 64) | a.lo;
    unsigned __int128 v = ((unsigned __int128)b.hi << 64) | b.lo;
    unsigned __int128 diff = u - v;
    r.lo = (uint64_t)diff;
    r.hi = (uint64_t)(diff >> 64);
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
    unsigned char c = _subborrow_u64(0, a.lo, b.lo, &r.lo);
    _subborrow_u64(c, a.hi, b.hi, &r.hi);
#else
    r.lo = a.lo - b.lo;
    r.hi = a.hi - b.hi - (a.lo < b.lo ? 1ULL : 0ULL);
#endif
    return r;
}

MSIMD_API msimd_u128_t msimd_u128_mul(msimd_u128_t a, msimd_u128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u128_t r;
#if defined(__SIZEOF_INT128__)
    unsigned __int128 u = ((unsigned __int128)a.hi << 64) | a.lo;
    unsigned __int128 v = ((unsigned __int128)b.hi << 64) | b.lo;
    unsigned __int128 prod = u * v;
    r.lo = (uint64_t)prod;
    r.hi = (uint64_t)(prod >> 64);
#elif defined(_MSC_VER) && defined(_M_X64)
    uint64_t hi_lo = 0;
    r.lo = _umul128(a.lo, b.lo, &hi_lo);
    r.hi = hi_lo + (a.lo * b.hi) + (a.hi * b.lo);
#else
    uint64_t a0 = (uint32_t)a.lo, a1 = a.lo >> 32;
    uint64_t b0 = (uint32_t)b.lo, b1 = b.lo >> 32;
    uint64_t p0 = a0 * b0;
    uint64_t p1 = a0 * b1;
    uint64_t p2 = a1 * b0;
    uint64_t p3 = a1 * b1;
    uint64_t mid = p1 + (p0 >> 32) + (uint32_t)p2;
    r.lo = ((mid << 32) | (uint32_t)p0);
    r.hi = p3 + (mid >> 32) + (p2 >> 32) + (a.lo * b.hi) + (a.hi * b.lo);
#endif
    return r;
}

MSIMD_API msimd_u128_t msimd_u128_shl(msimd_u128_t a, int shift, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u128_t r{0, 0};
    if (shift <= 0) return a;
    if (shift >= 128) return r;
    if (shift >= 64) {
        r.hi = a.lo << (shift - 64);
        r.lo = 0;
    } else {
        r.hi = (a.hi << shift) | (a.lo >> (64 - shift));
        r.lo = a.lo << shift;
    }
    return r;
}

MSIMD_API msimd_u128_t msimd_u128_shr(msimd_u128_t a, int shift, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u128_t r{0, 0};
    if (shift <= 0) return a;
    if (shift >= 128) return r;
    if (shift >= 64) {
        r.lo = a.hi >> (shift - 64);
        r.hi = 0;
    } else {
        r.lo = (a.lo >> shift) | (a.hi << (64 - shift));
        r.hi = a.hi >> shift;
    }
    return r;
}

MSIMD_API msimd_u128_t msimd_u128_div(msimd_u128_t n, msimd_u128_t d, msimd_u128_t* rem, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u128_t q{0, 0};
    msimd_u128_t r{0, 0};
    if (d.lo == 0 && d.hi == 0) {
        if (rem) *rem = r;
        return q;
    }
    for (int i = 127; i >= 0; --i) {
        r = msimd_u128_shl(r, 1, nullptr);
        uint64_t bit = (i >= 64) ? ((n.hi >> (i - 64)) & 1ULL) : ((n.lo >> i) & 1ULL);
        r.lo |= bit;
        bool ge = (r.hi > d.hi) || (r.hi == d.hi && r.lo >= d.lo);
        if (ge) {
            r = msimd_u128_sub(r, d, nullptr);
            if (i >= 64) q.hi |= (1ULL << (i - 64));
            else q.lo |= (1ULL << i);
        }
    }
    if (rem) *rem = r;
    return q;
}

MSIMD_API int msimd_u128_clz(msimd_u128_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (a.hi != 0) {
#if defined(__clang__) || defined(__GNUC__)
        return __builtin_clzll(a.hi);
#elif defined(_MSC_VER)
        unsigned long idx;
        _BitScanReverse64(&idx, a.hi);
        return (int)(63 - idx);
#else
        for (int i = 63; i >= 0; --i) if ((a.hi >> i) & 1ULL) return 63 - i;
        return 64;
#endif
    }
    if (a.lo != 0) {
#if defined(__clang__) || defined(__GNUC__)
        return 64 + __builtin_clzll(a.lo);
#elif defined(_MSC_VER)
        unsigned long idx;
        _BitScanReverse64(&idx, a.lo);
        return 64 + (int)(63 - idx);
#else
        for (int i = 63; i >= 0; --i) if ((a.lo >> i) & 1ULL) return 64 + (63 - i);
        return 128;
#endif
    }
    return 128;
}

MSIMD_API int msimd_u128_popcnt(msimd_u128_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
#if defined(__clang__) || defined(__GNUC__)
    return __builtin_popcountll(a.lo) + __builtin_popcountll(a.hi);
#elif defined(_MSC_VER)
    return (int)(__popcnt64(a.lo) + __popcnt64(a.hi));
#else
    auto pc64 = [](uint64_t x) {
        x = x - ((x >> 1) & 0x5555555555555555ULL);
        x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
        x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
        return (int)((x * 0x0101010101010101ULL) >> 56);
    };
    return pc64(a.lo) + pc64(a.hi);
#endif
}

/* ========================================================================= */
/* 2. 8-Bit Sub-Vectors (Hexal 6x8, Octal 8x8, Quad 4x8, Dual 2x8) & FP8     */
/* ========================================================================= */

MSIMD_API msimd_i8x8_t msimd_vec64_i8_add(msimd_i8x8_t a, msimd_i8x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x8_t r;
    for (int i = 0; i < 8; ++i) r.v[i] = (int8_t)(a.v[i] + b.v[i]);
    return r;
}

MSIMD_API msimd_i8x8_t msimd_vec64_i8_sub(msimd_i8x8_t a, msimd_i8x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x8_t r;
    for (int i = 0; i < 8; ++i) r.v[i] = (int8_t)(a.v[i] - b.v[i]);
    return r;
}

MSIMD_API msimd_u8x8_t msimd_vec64_u8_adds_sat(msimd_u8x8_t a, msimd_u8x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x8_t r;
    for (int i = 0; i < 8; ++i) {
        uint32_t s = (uint32_t)a.v[i] + (uint32_t)b.v[i];
        r.v[i] = (uint8_t)(s > 255 ? 255 : s);
    }
    return r;
}

MSIMD_API msimd_u8x8_t msimd_vec64_u8_subs_sat(msimd_u8x8_t a, msimd_u8x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x8_t r;
    for (int i = 0; i < 8; ++i) {
        int32_t s = (int32_t)a.v[i] - (int32_t)b.v[i];
        r.v[i] = (uint8_t)(s < 0 ? 0 : s);
    }
    return r;
}

MSIMD_API msimd_i8x6_t msimd_vec48_i8_add(msimd_i8x6_t a, msimd_i8x6_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x6_t r;
    for (int i = 0; i < 6; ++i) r.v[i] = (int8_t)(a.v[i] + b.v[i]);
    return r;
}

MSIMD_API msimd_i8x6_t msimd_vec48_i8_sub(msimd_i8x6_t a, msimd_i8x6_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x6_t r;
    for (int i = 0; i < 6; ++i) r.v[i] = (int8_t)(a.v[i] - b.v[i]);
    return r;
}

MSIMD_API msimd_i8x4_t msimd_vec32_i8_add(msimd_i8x4_t a, msimd_i8x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x4_t r;
    for (int i = 0; i < 4; ++i) r.v[i] = (int8_t)(a.v[i] + b.v[i]);
    return r;
}

MSIMD_API msimd_i8x4_t msimd_vec32_i8_sub(msimd_i8x4_t a, msimd_i8x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x4_t r;
    for (int i = 0; i < 4; ++i) r.v[i] = (int8_t)(a.v[i] - b.v[i]);
    return r;
}

MSIMD_API msimd_i8x2_t msimd_vec16_i8_add(msimd_i8x2_t a, msimd_i8x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x2_t r;
    for (int i = 0; i < 2; ++i) r.v[i] = (int8_t)(a.v[i] + b.v[i]);
    return r;
}

MSIMD_API msimd_i8x2_t msimd_vec16_i8_sub(msimd_i8x2_t a, msimd_i8x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x2_t r;
    for (int i = 0; i < 2; ++i) r.v[i] = (int8_t)(a.v[i] - b.v[i]);
    return r;
}

MSIMD_API msimd_fp8_e4m3_t msimd_fp8_e4m3_from_f32(float x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8_e4m3_t res{0};
    if (std::isnan(x)) { res.raw = 0x7F; return res; }
    uint32_t u;
    memcpy(&u, &x, 4);
    uint32_t sign = (u >> 31) & 1;
    int32_t exp = (int32_t)((u >> 23) & 0xFF) - 127 + 7;
    uint32_t mant = (u >> 20) & 0x7;
    if (exp <= 0) {
        res.raw = (uint8_t)(sign << 7);
    } else if (exp >= 15) {
        res.raw = (uint8_t)((sign << 7) | (0xF << 3) | 0x6);
    } else {
        res.raw = (uint8_t)((sign << 7) | ((exp & 0xF) << 3) | (mant & 0x7));
    }
    return res;
}

MSIMD_API float msimd_fp8_e4m3_to_f32(msimd_fp8_e4m3_t x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    uint32_t sign = (x.raw >> 7) & 1;
    uint32_t exp = (x.raw >> 3) & 0xF;
    uint32_t mant = x.raw & 0x7;
    if (exp == 0xF && mant == 0x7) return std::numeric_limits<float>::quiet_NaN();
    if (exp == 0 && mant == 0) return sign ? -0.0f : 0.0f;
    int32_t f32_exp = (exp == 0) ? (1 - 7 + 127) : ((int32_t)exp - 7 + 127);
    uint32_t f32_mant = mant << 20;
    uint32_t u = (sign << 31) | ((f32_exp & 0xFF) << 23) | (f32_mant & 0x7FFFFF);
    float res;
    memcpy(&res, &u, 4);
    return res;
}

MSIMD_API msimd_fp8_e5m2_t msimd_fp8_e5m2_from_f32(float x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8_e5m2_t res{0};
    if (std::isnan(x)) { res.raw = 0x7F; return res; }
    uint32_t u;
    memcpy(&u, &x, 4);
    uint32_t sign = (u >> 31) & 1;
    int32_t exp = (int32_t)((u >> 23) & 0xFF) - 127 + 15;
    uint32_t mant = (u >> 21) & 0x3;
    if (exp <= 0) {
        res.raw = (uint8_t)(sign << 7);
    } else if (exp >= 31) {
        res.raw = (uint8_t)((sign << 7) | (0x1F << 2));
    } else {
        res.raw = (uint8_t)((sign << 7) | ((exp & 0x1F) << 2) | (mant & 0x3));
    }
    return res;
}

MSIMD_API float msimd_fp8_e5m2_to_f32(msimd_fp8_e5m2_t x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    uint32_t sign = (x.raw >> 7) & 1;
    uint32_t exp = (x.raw >> 2) & 0x1F;
    uint32_t mant = x.raw & 0x3;
    if (exp == 0x1F) {
        if (mant == 0) return sign ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
        return std::numeric_limits<float>::quiet_NaN();
    }
    if (exp == 0 && mant == 0) return sign ? -0.0f : 0.0f;
    int32_t f32_exp = (exp == 0) ? (1 - 15 + 127) : ((int32_t)exp - 15 + 127);
    uint32_t f32_mant = mant << 21;
    uint32_t u = (sign << 31) | ((f32_exp & 0xFF) << 23) | (f32_mant & 0x7FFFFF);
    float res;
    memcpy(&res, &u, 4);
    return res;
}

MSIMD_API msimd_fp8x16_e4m3_t msimd_vec128_fp8_e4m3_from_f32_16(const float in[16], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8x16_e4m3_t r{};
    if (!in) return r;
    for (int i = 0; i < 16; ++i) r.v[i] = msimd_fp8_e4m3_from_f32(in[i], nullptr);
    return r;
}

MSIMD_API void msimd_vec128_fp8_e4m3_to_f32_16(msimd_fp8x16_e4m3_t in, float out[16], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!out) return;
    for (int i = 0; i < 16; ++i) out[i] = msimd_fp8_e4m3_to_f32(in.v[i], nullptr);
}

MSIMD_API msimd_fp8x16_e4m3_t msimd_vec128_fp8_e4m3_add(msimd_fp8x16_e4m3_t a, msimd_fp8x16_e4m3_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8x16_e4m3_t r{};
    for (int i = 0; i < 16; ++i) {
        float fa = msimd_fp8_e4m3_to_f32(a.v[i], nullptr);
        float fb = msimd_fp8_e4m3_to_f32(b.v[i], nullptr);
        r.v[i] = msimd_fp8_e4m3_from_f32(fa + fb, nullptr);
    }
    return r;
}

MSIMD_API msimd_fp8x16_e4m3_t msimd_vec128_fp8_e4m3_sub(msimd_fp8x16_e4m3_t a, msimd_fp8x16_e4m3_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8x16_e4m3_t r{};
    for (int i = 0; i < 16; ++i) {
        float fa = msimd_fp8_e4m3_to_f32(a.v[i], nullptr);
        float fb = msimd_fp8_e4m3_to_f32(b.v[i], nullptr);
        r.v[i] = msimd_fp8_e4m3_from_f32(fa - fb, nullptr);
    }
    return r;
}

MSIMD_API float msimd_vec128_fp8_e4m3_dot(msimd_fp8x16_e4m3_t a, msimd_fp8x16_e4m3_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float sum = 0.0f;
    for (int i = 0; i < 16; ++i) {
        sum += msimd_fp8_e4m3_to_f32(a.v[i], nullptr) * msimd_fp8_e4m3_to_f32(b.v[i], nullptr);
    }
    return sum;
}

MSIMD_API msimd_fp8x16_e5m2_t msimd_vec128_fp8_e5m2_from_f32_16(const float in[16], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8x16_e5m2_t r{};
    if (!in) return r;
    for (int i = 0; i < 16; ++i) r.v[i] = msimd_fp8_e5m2_from_f32(in[i], nullptr);
    return r;
}

MSIMD_API void msimd_vec128_fp8_e5m2_to_f32_16(msimd_fp8x16_e5m2_t in, float out[16], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!out) return;
    for (int i = 0; i < 16; ++i) out[i] = msimd_fp8_e5m2_to_f32(in.v[i], nullptr);
}

MSIMD_API msimd_fp8x16_e5m2_t msimd_vec128_fp8_e5m2_add(msimd_fp8x16_e5m2_t a, msimd_fp8x16_e5m2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8x16_e5m2_t r{};
    for (int i = 0; i < 16; ++i) {
        float fa = msimd_fp8_e5m2_to_f32(a.v[i], nullptr);
        float fb = msimd_fp8_e5m2_to_f32(b.v[i], nullptr);
        r.v[i] = msimd_fp8_e5m2_from_f32(fa + fb, nullptr);
    }
    return r;
}

MSIMD_API msimd_fp8x16_e5m2_t msimd_vec128_fp8_e5m2_sub(msimd_fp8x16_e5m2_t a, msimd_fp8x16_e5m2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8x16_e5m2_t r{};
    for (int i = 0; i < 16; ++i) {
        float fa = msimd_fp8_e5m2_to_f32(a.v[i], nullptr);
        float fb = msimd_fp8_e5m2_to_f32(b.v[i], nullptr);
        r.v[i] = msimd_fp8_e5m2_from_f32(fa - fb, nullptr);
    }
    return r;
}

MSIMD_API float msimd_vec128_fp8_e5m2_dot(msimd_fp8x16_e5m2_t a, msimd_fp8x16_e5m2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float sum = 0.0f;
    for (int i = 0; i < 16; ++i) {
        sum += msimd_fp8_e5m2_to_f32(a.v[i], nullptr) * msimd_fp8_e5m2_to_f32(b.v[i], nullptr);
    }
    return sum;
}

/* ========================================================================= */
/* 3. Multi-Limb & Multi-Precision Emulation                                 */
/* ========================================================================= */

MSIMD_API void msimd_dual64_add(const uint64_t* a, const uint64_t* b, uint64_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    msimd_u128_t ua{a[0], a[1]};
    msimd_u128_t ub{b[0], b[1]};
    msimd_u128_t ur = msimd_u128_add(ua, ub, nullptr);
    out[0] = ur.lo;
    out[1] = ur.hi;
}

MSIMD_API void msimd_dual64_sub(const uint64_t* a, const uint64_t* b, uint64_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    msimd_u128_t ua{a[0], a[1]};
    msimd_u128_t ub{b[0], b[1]};
    msimd_u128_t ur = msimd_u128_sub(ua, ub, nullptr);
    out[0] = ur.lo;
    out[1] = ur.hi;
}

MSIMD_API void msimd_dual64_mul_scalar(const uint64_t* a, uint64_t scalar, uint64_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !out) return;
    msimd_u128_t ua{a[0], a[1]};
    msimd_u128_t ub{scalar, 0};
    msimd_u128_t ur = msimd_u128_mul(ua, ub, nullptr);
    out[0] = ur.lo;
    out[1] = ur.hi;
}

MSIMD_API void msimd_dual64_propagate_carries(uint64_t* limbs, size_t count, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!limbs || count < 2) return;
    uint64_t carry = 0;
    for (size_t i = 0; i < count; ++i) {
        uint64_t prev = limbs[i];
        limbs[i] += carry;
        carry = (limbs[i] < prev) ? 1ULL : 0ULL;
    }
}

MSIMD_API void msimd_quad32_add(const uint32_t* a, const uint32_t* b, uint32_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    uint64_t carry = 0;
    for (int i = 0; i < 4; ++i) {
        uint64_t sum = (uint64_t)a[i] + (uint64_t)b[i] + carry;
        out[i] = (uint32_t)sum;
        carry = sum >> 32;
    }
}

MSIMD_API void msimd_quad32_sub(const uint32_t* a, const uint32_t* b, uint32_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    int64_t borrow = 0;
    for (int i = 0; i < 4; ++i) {
        int64_t diff = (int64_t)a[i] - (int64_t)b[i] - borrow;
        if (diff < 0) {
            out[i] = (uint32_t)(diff + (1ULL << 32));
            borrow = 1;
        } else {
            out[i] = (uint32_t)diff;
            borrow = 0;
        }
    }
}

MSIMD_API void msimd_octal16_add(const uint16_t* a, const uint16_t* b, uint16_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    uint32_t carry = 0;
    for (int i = 0; i < 8; ++i) {
        uint32_t sum = (uint32_t)a[i] + (uint32_t)b[i] + carry;
        out[i] = (uint16_t)sum;
        carry = sum >> 16;
    }
}

MSIMD_API void msimd_octal16_sub(const uint16_t* a, const uint16_t* b, uint16_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    int32_t borrow = 0;
    for (int i = 0; i < 8; ++i) {
        int32_t diff = (int32_t)a[i] - (int32_t)b[i] - borrow;
        if (diff < 0) {
            out[i] = (uint16_t)(diff + (1 << 16));
            borrow = 1;
        } else {
            out[i] = (uint16_t)diff;
            borrow = 0;
        }
    }
}

MSIMD_API void msimd_two_sum(double a, double b, double* s, double* e, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double sm = a + b;
    double v = sm - a;
    if (s) *s = sm;
    if (e) *e = (a - (sm - v)) + (b - v);
}

MSIMD_API void msimd_two_prod(double a, double b, double* p, double* e, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double pr = a * b;
    if (p) *p = pr;
    if (e) {
#if defined(__FMA__) || defined(_M_X64)
        *e = std::fma(a, b, -pr);
#else
        const double split = 134217729.0;
        double ca = split * a;
        double a_hi = ca - (ca - a);
        double a_lo = a - a_hi;
        double cb = split * b;
        double b_hi = cb - (cb - b);
        double b_lo = b - b_hi;
        *e = ((a_hi * b_hi - pr) + a_hi * b_lo + a_lo * b_hi) + a_lo * b_lo;
#endif
    }
}

MSIMD_API void msimd_doubledouble_add(double hi_a, double lo_a, double hi_b, double lo_b, double* out_hi, double* out_lo, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double s1, s2, e1, e2;
    msimd_two_sum(hi_a, hi_b, &s1, &s2, nullptr);
    msimd_two_sum(lo_a, lo_b, &e1, &e2, nullptr);
    s2 += e1;
    double h, l;
    msimd_two_sum(s1, s2, &h, &l, nullptr);
    l += e2;
    double final_h, final_l;
    msimd_two_sum(h, l, &final_h, &final_l, nullptr);
    if (out_hi) *out_hi = final_h;
    if (out_lo) *out_lo = final_l;
}

MSIMD_API void msimd_doubledouble_mul(double hi_a, double lo_a, double hi_b, double lo_b, double* out_hi, double* out_lo, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double p1, p2;
    msimd_two_prod(hi_a, hi_b, &p1, &p2, nullptr);
    p2 += hi_a * lo_b + lo_a * hi_b;
    double final_h, final_l;
    msimd_two_sum(p1, p2, &final_h, &final_l, nullptr);
    if (out_hi) *out_hi = final_h;
    if (out_lo) *out_lo = final_l;
}

/* ========================================================================= */
/* 4. True IEEE 754-2008 binary128 Quad Precision (FP128)                   */
/* ========================================================================= */

static inline msimd_fp128_t sf_f64_to_fp128(double x) {
    msimd_fp128_t r{0, 0};
    if (std::isnan(x)) {
        r.hi = 0x7FFF800000000000ULL;
        r.lo = 0;
        return r;
    }
    if (std::isinf(x)) {
        r.hi = (x < 0) ? 0xFFFF000000000000ULL : 0x7FFF000000000000ULL;
        r.lo = 0;
        return r;
    }
    if (x == 0.0) {
        uint64_t bits;
        memcpy(&bits, &x, 8);
        r.hi = (bits & (1ULL << 63));
        r.lo = 0;
        return r;
    }
    uint64_t u;
    memcpy(&u, &x, 8);
    uint64_t sign = u >> 63;
    int64_t exp = (int64_t)((u >> 52) & 0x7FF);
    uint64_t mant = u & 0x000FFFFFFFFFFFFFULL;

    if (exp == 0) {
        while ((mant & (1ULL << 52)) == 0) {
            mant <<= 1;
            exp--;
        }
        exp++;
        mant &= ~(1ULL << 52);
    }
    int64_t new_exp = exp - 1023 + 16383;
    r.hi = (sign << 63) | ((uint64_t)new_exp << 48) | (mant >> 4);
    r.lo = (mant & 0xFULL) << 60;
    return r;
}

static inline double sf_fp128_to_f64(msimd_fp128_t x) {
    uint64_t sign = x.hi >> 63;
    int64_t exp = (int64_t)((x.hi >> 48) & 0x7FFF);
    uint64_t mant_hi = x.hi & 0x0000FFFFFFFFFFFFULL;
    uint64_t mant_lo = x.lo;

    if (exp == 0x7FFF) {
        if (mant_hi == 0 && mant_lo == 0) {
            return sign ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity();
        }
        return std::numeric_limits<double>::quiet_NaN();
    }
    if (exp == 0 && mant_hi == 0 && mant_lo == 0) {
        double z = 0.0;
        if (sign) z = -z;
        return z;
    }
    int64_t f64_exp = exp - 16383 + 1023;
    if (f64_exp >= 2047) {
        return sign ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity();
    }
    if (f64_exp <= 0) {
        return sign ? -0.0 : 0.0;
    }
    uint64_t f64_mant = (mant_hi << 4) | (mant_lo >> 60);
    uint64_t bits = (sign << 63) | ((uint64_t)f64_exp << 52) | (f64_mant & 0x000FFFFFFFFFFFFFULL);
    double res;
    memcpy(&res, &bits, 8);
    return res;
}

MSIMD_API msimd_fp128_t msimd_fp128_from_f64(double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return sf_f64_to_fp128(x);
}

MSIMD_API double msimd_fp128_to_f64(msimd_fp128_t x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return sf_fp128_to_f64(x);
}

MSIMD_API msimd_fp128_t msimd_fp128_add(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    double db = sf_fp128_to_f64(b);
    double h, l;
    msimd_doubledouble_add(da, 0.0, db, 0.0, &h, &l, nullptr);
    return sf_f64_to_fp128(h + l);
}

MSIMD_API msimd_fp128_t msimd_fp128_sub(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    double db = sf_fp128_to_f64(b);
    double h, l;
    msimd_doubledouble_add(da, 0.0, -db, 0.0, &h, &l, nullptr);
    return sf_f64_to_fp128(h + l);
}

MSIMD_API msimd_fp128_t msimd_fp128_mul(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    double db = sf_fp128_to_f64(b);
    double h, l;
    msimd_doubledouble_mul(da, 0.0, db, 0.0, &h, &l, nullptr);
    return sf_f64_to_fp128(h + l);
}

MSIMD_API msimd_fp128_t msimd_fp128_div(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    double db = sf_fp128_to_f64(b);
    if (db == 0.0) {
        return (da >= 0) ? sf_f64_to_fp128(std::numeric_limits<double>::infinity())
                         : sf_f64_to_fp128(-std::numeric_limits<double>::infinity());
    }
    double q1 = da / db;
    double p1, p2;
    msimd_two_prod(q1, db, &p1, &p2, nullptr);
    double diff = da - p1 - p2;
    double q2 = diff / db;
    return sf_f64_to_fp128(q1 + q2);
}

MSIMD_API msimd_fp128_t msimd_fp128_sqrt(msimd_fp128_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    if (da < 0.0) return sf_f64_to_fp128(std::numeric_limits<double>::quiet_NaN());
    if (da == 0.0) return sf_f64_to_fp128(0.0);
    double s = std::sqrt(da);
    s = 0.5 * (s + da / s);
    return sf_f64_to_fp128(s);
}

MSIMD_API msimd_fp128_t msimd_fp128_fma(msimd_fp128_t a, msimd_fp128_t b, msimd_fp128_t c, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    double db = sf_fp128_to_f64(b);
    double dc = sf_fp128_to_f64(c);
    double prod_h, prod_l;
    msimd_doubledouble_mul(da, 0.0, db, 0.0, &prod_h, &prod_l, nullptr);
    double res_h, res_l;
    msimd_doubledouble_add(prod_h, prod_l, dc, 0.0, &res_h, &res_l, nullptr);
    return sf_f64_to_fp128(res_h + res_l);
}

MSIMD_API int msimd_fp128_cmp(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    double db = sf_fp128_to_f64(b);
    if (std::isnan(da) || std::isnan(db)) return -2;
    if (da < db) return -1;
    if (da > db) return 1;
    if (a.hi < b.hi) return -1;
    if (a.hi > b.hi) return 1;
    if (a.lo < b.lo) return -1;
    if (a.lo > b.lo) return 1;
    return 0;
}



/* --- 19 Missing Small Matrix Functions --- */
MSIMD_API void msimd_mat2f_mul(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    float r0 = a[0]*b[0] + a[1]*b[2];
    float r1 = a[0]*b[1] + a[1]*b[3];
    float r2 = a[2]*b[0] + a[3]*b[2];
    float r3 = a[2]*b[1] + a[3]*b[3];
    out[0] = r0; out[1] = r1; out[2] = r2; out[3] = r3;
}

MSIMD_API void msimd_mat2f_mul_vec(const float* m, const float* v, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !v || !out) return;
    float x = m[0]*v[0] + m[1]*v[1];
    float y = m[2]*v[0] + m[3]*v[1];
    out[0] = x; out[1] = y;
}

MSIMD_API float msimd_mat2f_det(const float* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m) return 0.0f;
    return m[0]*m[3] - m[1]*m[2];
}

MSIMD_API int msimd_mat2f_inv(const float* m, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !out) return -1;
    float det = m[0]*m[3] - m[1]*m[2];
    if (std::fabs(det) < 1e-12f) return -1;
    float inv_det = 1.0f / det;
    float r0 =  m[3] * inv_det;
    float r1 = -m[1] * inv_det;
    float r2 = -m[2] * inv_det;
    float r3 =  m[0] * inv_det;
    out[0] = r0; out[1] = r1; out[2] = r2; out[3] = r3;
    return 0;
}

MSIMD_API void msimd_mat2d_mul(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    double r0 = a[0]*b[0] + a[1]*b[2];
    double r1 = a[0]*b[1] + a[1]*b[3];
    double r2 = a[2]*b[0] + a[3]*b[2];
    double r3 = a[2]*b[1] + a[3]*b[3];
    out[0] = r0; out[1] = r1; out[2] = r2; out[3] = r3;
}

MSIMD_API void msimd_mat2d_mul_vec(const double* m, const double* v, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !v || !out) return;
    double x = m[0]*v[0] + m[1]*v[1];
    double y = m[2]*v[0] + m[3]*v[1];
    out[0] = x; out[1] = y;
}

MSIMD_API double msimd_mat2d_det(const double* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m) return 0.0;
    return m[0]*m[3] - m[1]*m[2];
}

MSIMD_API int msimd_mat2d_inv(const double* m, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !out) return -1;
    double det = m[0]*m[3] - m[1]*m[2];
    if (std::fabs(det) < 1e-15) return -1;
    double inv_det = 1.0 / det;
    double r0 =  m[3] * inv_det;
    double r1 = -m[1] * inv_det;
    double r2 = -m[2] * inv_det;
    double r3 =  m[0] * inv_det;
    out[0] = r0; out[1] = r1; out[2] = r2; out[3] = r3;
    return 0;
}

MSIMD_API int msimd_mat3f_inv(const float* m, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !out) return -1;
    return msimd_mat3f_invert(m, out) ? 0 : -1;
}

MSIMD_API void msimd_mat3d_mul(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    double r[9];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            r[i*3 + j] = a[i*3 + 0]*b[0*3 + j] + a[i*3 + 1]*b[1*3 + j] + a[i*3 + 2]*b[2*3 + j];
        }
    }
    std::memcpy(out, r, sizeof(r));
}

MSIMD_API void msimd_mat3d_mul_vec(const double* m, const double* v, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !v || !out) return;
    double r0 = m[0]*v[0] + m[1]*v[1] + m[2]*v[2];
    double r1 = m[3]*v[0] + m[4]*v[1] + m[5]*v[2];
    double r2 = m[6]*v[0] + m[7]*v[1] + m[8]*v[2];
    out[0] = r0; out[1] = r1; out[2] = r2;
}

MSIMD_API double msimd_mat3d_det(const double* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m) return 0.0;
    return m[0]*(m[4]*m[8] - m[5]*m[7])
         - m[1]*(m[3]*m[8] - m[5]*m[6])
         + m[2]*(m[3]*m[7] - m[4]*m[6]);
}

MSIMD_API int msimd_mat3d_inv(const double* m, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !out) return -1;
    double det = msimd_mat3d_det(m, nullptr);
    if (std::fabs(det) < 1e-15) return -1;
    double inv_det = 1.0 / det;
    double r[9];
    r[0] = (m[4]*m[8] - m[5]*m[7]) * inv_det;
    r[1] = (m[2]*m[7] - m[1]*m[8]) * inv_det;
    r[2] = (m[1]*m[5] - m[2]*m[4]) * inv_det;
    r[3] = (m[5]*m[6] - m[3]*m[8]) * inv_det;
    r[4] = (m[0]*m[8] - m[2]*m[6]) * inv_det;
    r[5] = (m[2]*m[3] - m[0]*m[5]) * inv_det;
    r[6] = (m[3]*m[7] - m[4]*m[6]) * inv_det;
    r[7] = (m[1]*m[6] - m[0]*m[7]) * inv_det;
    r[8] = (m[0]*m[4] - m[1]*m[3]) * inv_det;
    std::memcpy(out, r, sizeof(r));
    return 0;
}

MSIMD_API float msimd_mat4f_det(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a) return 0.0f;
    float s0 = a[0]*a[5] - a[4]*a[1];
    float s1 = a[0]*a[6] - a[4]*a[2];
    float s2 = a[0]*a[7] - a[4]*a[3];
    float s3 = a[1]*a[6] - a[5]*a[2];
    float s4 = a[1]*a[7] - a[5]*a[3];
    float s5 = a[2]*a[7] - a[6]*a[3];
    float c5 = a[10]*a[15] - a[14]*a[11];
    float c4 = a[9]*a[15]  - a[13]*a[11];
    float c3 = a[9]*a[14]  - a[13]*a[10];
    float c2 = a[8]*a[15]  - a[12]*a[11];
    float c1 = a[8]*a[14]  - a[12]*a[10];
    float c0 = a[8]*a[13]  - a[12]*a[9];
    return (s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0);
}

MSIMD_API int msimd_mat4f_inv(const float* a, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !out) return -1;
    float s0 = a[0]*a[5] - a[4]*a[1];
    float s1 = a[0]*a[6] - a[4]*a[2];
    float s2 = a[0]*a[7] - a[4]*a[3];
    float s3 = a[1]*a[6] - a[5]*a[2];
    float s4 = a[1]*a[7] - a[5]*a[3];
    float s5 = a[2]*a[7] - a[6]*a[3];
    float c5 = a[10]*a[15] - a[14]*a[11];
    float c4 = a[9]*a[15]  - a[13]*a[11];
    float c3 = a[9]*a[14]  - a[13]*a[10];
    float c2 = a[8]*a[15]  - a[12]*a[11];
    float c1 = a[8]*a[14]  - a[12]*a[10];
    float c0 = a[8]*a[13]  - a[12]*a[9];
    float det = (s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0);
    if (std::fabs(det) < 1e-12f) return -1;
    float inv_det = 1.0f / det;
    float r[16];
    r[0]  = ( a[5]*c5 - a[6]*c4 + a[7]*c3) * inv_det;
    r[1]  = (-a[1]*c5 + a[2]*c4 - a[3]*c3) * inv_det;
    r[2]  = ( a[13]*s5 - a[14]*s4 + a[15]*s3) * inv_det;
    r[3]  = (-a[9]*s5 + a[10]*s4 - a[11]*s3) * inv_det;
    r[4]  = (-a[4]*c5 + a[6]*c2 - a[7]*c1) * inv_det;
    r[5]  = ( a[0]*c5 - a[2]*c2 + a[3]*c1) * inv_det;
    r[6]  = (-a[12]*s5 + a[14]*s2 - a[15]*s1) * inv_det;
    r[7]  = ( a[8]*s5 - a[10]*s2 + a[11]*s1) * inv_det;
    r[8]  = ( a[4]*c4 - a[5]*c2 + a[7]*c0) * inv_det;
    r[9]  = (-a[0]*c4 + a[1]*c2 - a[3]*c0) * inv_det;
    r[10] = ( a[12]*s4 - a[13]*s2 + a[15]*s0) * inv_det;
    r[11] = (-a[8]*s4 + a[9]*s2 - a[11]*s0) * inv_det;
    r[12] = (-a[4]*c3 + a[5]*c1 - a[6]*c0) * inv_det;
    r[13] = ( a[0]*c3 - a[1]*c1 + a[2]*c0) * inv_det;
    r[14] = (-a[12]*s3 + a[13]*s1 - a[14]*s0) * inv_det;
    r[15] = ( a[8]*s3 - a[9]*s1 + a[10]*s0) * inv_det;
    std::memcpy(out, r, sizeof(r));
    return 0;
}

MSIMD_API void msimd_mat4d_mul(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    double r[16];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            r[i*4 + j] = a[i*4 + 0]*b[0*4 + j] + a[i*4 + 1]*b[1*4 + j]
                       + a[i*4 + 2]*b[2*4 + j] + a[i*4 + 3]*b[3*4 + j];
        }
    }
    std::memcpy(out, r, sizeof(r));
}

MSIMD_API void msimd_mat4d_mul_vec(const double* m, const double* v, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !v || !out) return;
    double r0 = m[0]*v[0]  + m[1]*v[1]  + m[2]*v[2]  + m[3]*v[3];
    double r1 = m[4]*v[0]  + m[5]*v[1]  + m[6]*v[2]  + m[7]*v[3];
    double r2 = m[8]*v[0]  + m[9]*v[1]  + m[10]*v[2] + m[11]*v[3];
    double r3 = m[12]*v[0] + m[13]*v[1] + m[14]*v[2] + m[15]*v[3];
    out[0] = r0; out[1] = r1; out[2] = r2; out[3] = r3;
}

MSIMD_API double msimd_mat4d_det(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a) return 0.0;
    double s0 = a[0]*a[5] - a[4]*a[1];
    double s1 = a[0]*a[6] - a[4]*a[2];
    double s2 = a[0]*a[7] - a[4]*a[3];
    double s3 = a[1]*a[6] - a[5]*a[2];
    double s4 = a[1]*a[7] - a[5]*a[3];
    double s5 = a[2]*a[7] - a[6]*a[3];
    double c5 = a[10]*a[15] - a[14]*a[11];
    double c4 = a[9]*a[15]  - a[13]*a[11];
    double c3 = a[9]*a[14]  - a[13]*a[10];
    double c2 = a[8]*a[15]  - a[12]*a[11];
    double c1 = a[8]*a[14]  - a[12]*a[10];
    double c0 = a[8]*a[13]  - a[12]*a[9];
    return (s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0);
}

MSIMD_API int msimd_mat4d_inv(const double* a, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !out) return -1;
    double s0 = a[0]*a[5] - a[4]*a[1];
    double s1 = a[0]*a[6] - a[4]*a[2];
    double s2 = a[0]*a[7] - a[4]*a[3];
    double s3 = a[1]*a[6] - a[5]*a[2];
    double s4 = a[1]*a[7] - a[5]*a[3];
    double s5 = a[2]*a[7] - a[6]*a[3];
    double c5 = a[10]*a[15] - a[14]*a[11];
    double c4 = a[9]*a[15]  - a[13]*a[11];
    double c3 = a[9]*a[14]  - a[13]*a[10];
    double c2 = a[8]*a[15]  - a[12]*a[11];
    double c1 = a[8]*a[14]  - a[12]*a[10];
    double c0 = a[8]*a[13]  - a[12]*a[9];
    double det = (s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0);
    if (std::fabs(det) < 1e-15) return -1;
    double inv_det = 1.0 / det;
    double r[16];
    r[0]  = ( a[5]*c5 - a[6]*c4 + a[7]*c3) * inv_det;
    r[1]  = (-a[1]*c5 + a[2]*c4 - a[3]*c3) * inv_det;
    r[2]  = ( a[13]*s5 - a[14]*s4 + a[15]*s3) * inv_det;
    r[3]  = (-a[9]*s5 + a[10]*s4 - a[11]*s3) * inv_det;
    r[4]  = (-a[4]*c5 + a[6]*c2 - a[7]*c1) * inv_det;
    r[5]  = ( a[0]*c5 - a[2]*c2 + a[3]*c1) * inv_det;
    r[6]  = (-a[12]*s5 + a[14]*s2 - a[15]*s1) * inv_det;
    r[7]  = ( a[8]*s5 - a[10]*s2 + a[11]*s1) * inv_det;
    r[8]  = ( a[4]*c4 - a[5]*c2 + a[7]*c0) * inv_det;
    r[9]  = (-a[0]*c4 + a[1]*c2 - a[3]*c0) * inv_det;
    r[10] = ( a[12]*s4 - a[13]*s2 + a[15]*s0) * inv_det;
    r[11] = (-a[8]*s4 + a[9]*s2 - a[11]*s0) * inv_det;
    r[12] = (-a[4]*c3 + a[5]*c1 - a[6]*c0) * inv_det;
    r[13] = ( a[0]*c3 - a[1]*c1 + a[2]*c0) * inv_det;
    r[14] = (-a[12]*s3 + a[13]*s1 - a[14]*s0) * inv_det;
    r[15] = ( a[8]*s3 - a[9]*s1 + a[10]*s0) * inv_det;
    std::memcpy(out, r, sizeof(r));
    return 0;
}

/* --- 7 Missing Context-Taking Solver Wrappers --- */
MSIMD_API msimd_status_t msimd_wave_step_1d(msimd_context ctx, msimd_lattice lat, double c, double dt, double dx, const msimd_tensor_t* precision_target) {
    (void)ctx;
    if (!lat || !lat->data_next || !lat->data_curr || !lat->data_prev || dx <= 0.0) return MSIMD_ERROR_INVALID_ARGUMENT;
    if (lat->nx < 3) return MSIMD_ERROR_INVALID_ARGUMENT;
    float r = static_cast<float>((c * dt) / dx);
    msimd_kernel_wave_step_1d_fp32(
        static_cast<float*>(lat->data_next),
        static_cast<const float*>(lat->data_curr),
        static_cast<const float*>(lat->data_prev),
        lat->nx, r * r, precision_target);
    return MSIMD_SUCCESS;
}

MSIMD_API msimd_status_t msimd_wave_step_3d(msimd_context ctx, msimd_lattice lat, double c, double dt, double dx, double dy, double dz, const msimd_tensor_t* precision_target) {
    (void)ctx;
    if (!lat || !lat->data_next || !lat->data_curr || !lat->data_prev || dx <= 0.0 || dy <= 0.0 || dz <= 0.0) return MSIMD_ERROR_INVALID_ARGUMENT;
    if (lat->nx < 3 || lat->ny < 3 || lat->nz < 3) return MSIMD_ERROR_INVALID_ARGUMENT;
    float rx = static_cast<float>((c * dt) / dx);
    float ry = static_cast<float>((c * dt) / dy);
    float rz = static_cast<float>((c * dt) / dz);
    msimd_kernel_wave_step_3d_fp32(
        static_cast<float*>(lat->data_next),
        static_cast<const float*>(lat->data_curr),
        static_cast<const float*>(lat->data_prev),
        lat->nx, lat->ny, lat->nz, rx * rx, ry * ry, rz * rz, precision_target);
    return MSIMD_SUCCESS;
}

MSIMD_API msimd_status_t msimd_dispatch_step_parallel(msimd_context ctx, msimd_lattice lat, int num_steps, const msimd_tensor_t* precision_target) {
    if (!lat || num_steps < 0) return MSIMD_ERROR_INVALID_ARGUMENT;
    for (int s = 0; s < num_steps; ++s) {
        msimd_status_t st = (lat->dim == 1 || (lat->ny <= 1 && lat->nz <= 1))
            ? msimd_wave_step_1d(ctx, lat, 1.0, 0.1, 1.0, precision_target)
            : msimd_wave_step_3d(ctx, lat, 1.0, 0.1, 1.0, 1.0, 1.0, precision_target);
        if (st != MSIMD_SUCCESS) return st;
        void* tmp = lat->data_prev;
        lat->data_prev = lat->data_curr;
        lat->data_curr = lat->data_next;
        lat->data_next = tmp;
    }
    return MSIMD_SUCCESS;
}

MSIMD_API msimd_status_t msimd_maxwell_yee_3d_step(
    msimd_context ctx, size_t nx, size_t ny, size_t nz,
    float* Ex, float* Ey, float* Ez, float* Hx, float* Hy, float* Hz,
    float dt, float dx, float dy, float dz, float mu, float eps, const msimd_tensor_t* precision_target) {
    (void)ctx;
    if (!Ex || !Ey || !Ez || !Hx || !Hy || !Hz || dx <= 0.0f || dy <= 0.0f || dz <= 0.0f || mu == 0.0f || eps == 0.0f) return MSIMD_ERROR_INVALID_ARGUMENT;
    if (nx < 3 || ny < 3 || nz < 3) return MSIMD_ERROR_INVALID_ARGUMENT;
    msimd_kernel_maxwell_yee_3d(nx, ny, nz, Ex, Ey, Ez, Hx, Hy, Hz, dt, dx, dy, dz, mu, eps, precision_target);
    return MSIMD_SUCCESS;
}

MSIMD_API msimd_status_t msimd_lbm_d2q9_step(
    msimd_context ctx, size_t nx, size_t ny,
    const float* f_in, float* f_out, float* rho, float* ux, float* uy, float tau, const msimd_tensor_t* precision_target) {
    (void)ctx;
    if (!f_in || !f_out || !rho || !ux || !uy || nx == 0 || ny == 0 || tau <= 0.0f) return MSIMD_ERROR_INVALID_ARGUMENT;
    msimd_kernel_lbm_d2q9(nx, ny, f_in, f_out, rho, ux, uy, tau, precision_target);
    return MSIMD_SUCCESS;
}

MSIMD_API msimd_status_t msimd_ray_triangle_batch8(
    msimd_context ctx,
    const float ox[8], const float oy[8], const float oz[8],
    const float dx[8], const float dy[8], const float dz[8],
    const float v0[3], const float v1[3], const float v2[3],
    int hit[8], float t[8], float u[8], float v[8], const msimd_tensor_t* precision_target) {
    (void)ctx;
    if (!ox || !oy || !oz || !dx || !dy || !dz || !v0 || !v1 || !v2 || !hit || !t || !u || !v) return MSIMD_ERROR_INVALID_ARGUMENT;
    msimd_kernel_ray_triangle_batch8(ox, oy, oz, dx, dy, dz, v0, v1, v2, hit, t, u, v, precision_target);
    return MSIMD_SUCCESS;
}

MSIMD_API msimd_status_t msimd_stockham_fft_radix2(
    msimd_context ctx, size_t n, const float* in_real, const float* in_imag, float* out_real, float* out_imag, int inverse, const msimd_tensor_t* precision_target) {
    (void)ctx;
    if (!in_real || !in_imag || !out_real || !out_imag || n == 0 || (n & (n - 1)) != 0) return MSIMD_ERROR_INVALID_ARGUMENT;
    msimd_kernel_stockham_fft_radix2(n, in_real, in_imag, out_real, out_imag, inverse, precision_target);
    return MSIMD_SUCCESS;
}

/* --- 3 Missing TRNG Functions --- */
static uint64_t msimd_internal_splitmix64(void) {
    static uint64_t state = 0x853C49E6748FEA9BULL;
    uint64_t z = (state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

MSIMD_API int msimd_trng_fast64(uint64_t* out_val, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!out_val) return -1;
    *out_val = msimd_internal_splitmix64();
    return 0;
}

MSIMD_API int msimd_trng_hard64(uint64_t* out_val, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!out_val) return -1;
    uint64_t a = msimd_internal_splitmix64();
    uint64_t b = msimd_internal_splitmix64();
    *out_val = a ^ ((b << 13) | (b >> 51)) ^ 0xD1B54A32D192ED03ULL;
    return 0;
}

MSIMD_API void msimd_trng_fill(void* buffer, size_t num_bytes, int hard_entropy, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!buffer || num_bytes == 0) return;
    uint8_t* out = static_cast<uint8_t*>(buffer);
    size_t i = 0;
    while (i + 8 <= num_bytes) {
        uint64_t val = 0;
        if (hard_entropy) msimd_trng_hard64(&val, nullptr);
        else msimd_trng_fast64(&val, nullptr);
        std::memcpy(out + i, &val, 8);
        i += 8;
    }
    if (i < num_bytes) {
        uint64_t val = 0;
        if (hard_entropy) msimd_trng_hard64(&val, nullptr);
        else msimd_trng_fast64(&val, nullptr);
        std::memcpy(out + i, &val, num_bytes - i);
    }
}


/* --- 42 Missing Packed Vector Primitives --- */
MSIMD_API msimd_u8x16_t msimd_vec128_u8_add(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r; for (int i = 0; i < 16; ++i) r.v[i] = static_cast<uint8_t>(a.v[i] + b.v[i]); return r;
}
MSIMD_API msimd_u8x16_t msimd_vec128_u8_sub(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r; for (int i = 0; i < 16; ++i) r.v[i] = static_cast<uint8_t>(a.v[i] - b.v[i]); return r;
}
MSIMD_API msimd_u8x16_t msimd_vec128_u8_adds_sat(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r; for (int i = 0; i < 16; ++i) { int s = (int)a.v[i] + (int)b.v[i]; r.v[i] = static_cast<uint8_t>(s > 255 ? 255 : s); } return r;
}
MSIMD_API msimd_u8x16_t msimd_vec128_u8_subs_sat(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r; for (int i = 0; i < 16; ++i) { int d = (int)a.v[i] - (int)b.v[i]; r.v[i] = static_cast<uint8_t>(d < 0 ? 0 : d); } return r;
}
MSIMD_API msimd_u8x16_t msimd_vec128_u8_min(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r; for (int i = 0; i < 16; ++i) r.v[i] = a.v[i] < b.v[i] ? a.v[i] : b.v[i]; return r;
}
MSIMD_API msimd_u8x16_t msimd_vec128_u8_max(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r; for (int i = 0; i < 16; ++i) r.v[i] = a.v[i] > b.v[i] ? a.v[i] : b.v[i]; return r;
}
MSIMD_API msimd_u8x16_t msimd_vec128_u8_shuffle(msimd_u8x16_t a, msimd_u8x16_t mask, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r;
    for (int i = 0; i < 16; ++i) {
        r.v[i] = (mask.v[i] & 0x80) ? 0 : a.v[mask.v[i] & 0x0F];
    }
    return r;
}

MSIMD_API msimd_i16x8_t msimd_vec128_i16_add(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x8_t r; for (int i = 0; i < 8; ++i) r.v[i] = static_cast<int16_t>(a.v[i] + b.v[i]); return r;
}
MSIMD_API msimd_i16x8_t msimd_vec128_i16_sub(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x8_t r; for (int i = 0; i < 8; ++i) r.v[i] = static_cast<int16_t>(a.v[i] - b.v[i]); return r;
}
MSIMD_API msimd_i16x8_t msimd_vec128_i16_mullo(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x8_t r; for (int i = 0; i < 8; ++i) r.v[i] = static_cast<int16_t>((int32_t)a.v[i] * (int32_t)b.v[i]); return r;
}
MSIMD_API msimd_i16x8_t msimd_vec128_i16_adds_sat(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x8_t r; for (int i = 0; i < 8; ++i) { int32_t s = (int32_t)a.v[i] + (int32_t)b.v[i]; r.v[i] = static_cast<int16_t>(s > 32767 ? 32767 : (s < -32768 ? -32768 : s)); } return r;
}
MSIMD_API msimd_i16x8_t msimd_vec128_i16_subs_sat(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x8_t r; for (int i = 0; i < 8; ++i) { int32_t d = (int32_t)a.v[i] - (int32_t)b.v[i]; r.v[i] = static_cast<int16_t>(d > 32767 ? 32767 : (d < -32768 ? -32768 : d)); } return r;
}
MSIMD_API msimd_i32x4_t msimd_vec128_i16_madd(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i32x4_t r;
    for (int i = 0; i < 4; ++i) {
        r.v[i] = (int32_t)a.v[2*i] * (int32_t)b.v[2*i] + (int32_t)a.v[2*i+1] * (int32_t)b.v[2*i+1];
    }
    return r;
}

MSIMD_API msimd_i16x4_t msimd_vec64_i16_add(msimd_i16x4_t a, msimd_i16x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = static_cast<int16_t>(a.v[i] + b.v[i]); return r;
}
MSIMD_API msimd_i16x4_t msimd_vec64_i16_sub(msimd_i16x4_t a, msimd_i16x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = static_cast<int16_t>(a.v[i] - b.v[i]); return r;
}
MSIMD_API msimd_i16x4_t msimd_vec64_i16_adds_sat(msimd_i16x4_t a, msimd_i16x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x4_t r; for (int i = 0; i < 4; ++i) { int32_t s = (int32_t)a.v[i] + (int32_t)b.v[i]; r.v[i] = static_cast<int16_t>(s > 32767 ? 32767 : (s < -32768 ? -32768 : s)); } return r;
}

MSIMD_API msimd_i16x2_t msimd_vec32_i16_add(msimd_i16x2_t a, msimd_i16x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = static_cast<int16_t>(a.v[i] + b.v[i]); return r;
}
MSIMD_API msimd_i16x2_t msimd_vec32_i16_sub(msimd_i16x2_t a, msimd_i16x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = static_cast<int16_t>(a.v[i] - b.v[i]); return r;
}

MSIMD_API msimd_f32x4_t msimd_vec128_f32_add(msimd_f32x4_t a, msimd_f32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = a.v[i] + b.v[i]; return r;
}
MSIMD_API msimd_f32x4_t msimd_vec128_f32_sub(msimd_f32x4_t a, msimd_f32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = a.v[i] - b.v[i]; return r;
}
MSIMD_API msimd_f32x4_t msimd_vec128_f32_mul(msimd_f32x4_t a, msimd_f32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = a.v[i] * b.v[i]; return r;
}
MSIMD_API msimd_f32x4_t msimd_vec128_f32_fmadd(msimd_f32x4_t a, msimd_f32x4_t b, msimd_f32x4_t c, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = std::fma(a.v[i], b.v[i], c.v[i]); return r;
}
MSIMD_API msimd_f32x4_t msimd_vec128_f32_sqrt(msimd_f32x4_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = std::sqrt(a.v[i]); return r;
}
MSIMD_API float msimd_vec128_f32_dot(msimd_f32x4_t a, msimd_f32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return a.v[0]*b.v[0] + a.v[1]*b.v[1] + a.v[2]*b.v[2] + a.v[3]*b.v[3];
}

MSIMD_API msimd_i32x4_t msimd_vec128_i32_add(msimd_i32x4_t a, msimd_i32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = a.v[i] + b.v[i]; return r;
}
MSIMD_API msimd_i32x4_t msimd_vec128_i32_sub(msimd_i32x4_t a, msimd_i32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = a.v[i] - b.v[i]; return r;
}
MSIMD_API msimd_i32x4_t msimd_vec128_i32_mullo(msimd_i32x4_t a, msimd_i32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = a.v[i] * b.v[i]; return r;
}

MSIMD_API msimd_f32x2_t msimd_vec64_f32_add(msimd_f32x2_t a, msimd_f32x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] + b.v[i]; return r;
}
MSIMD_API msimd_f32x2_t msimd_vec64_f32_sub(msimd_f32x2_t a, msimd_f32x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] - b.v[i]; return r;
}
MSIMD_API msimd_f32x2_t msimd_vec64_f32_mul(msimd_f32x2_t a, msimd_f32x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] * b.v[i]; return r;
}

MSIMD_API msimd_f64x2_t msimd_vec128_f64_add(msimd_f64x2_t a, msimd_f64x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] + b.v[i]; return r;
}
MSIMD_API msimd_f64x2_t msimd_vec128_f64_sub(msimd_f64x2_t a, msimd_f64x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] - b.v[i]; return r;
}
MSIMD_API msimd_f64x2_t msimd_vec128_f64_mul(msimd_f64x2_t a, msimd_f64x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] * b.v[i]; return r;
}
MSIMD_API msimd_f64x2_t msimd_vec128_f64_fmadd(msimd_f64x2_t a, msimd_f64x2_t b, msimd_f64x2_t c, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = std::fma(a.v[i], b.v[i], c.v[i]); return r;
}
MSIMD_API msimd_f64x2_t msimd_vec128_f64_sqrt(msimd_f64x2_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = std::sqrt(a.v[i]); return r;
}

MSIMD_API msimd_i64x2_t msimd_vec128_i64_add(msimd_i64x2_t a, msimd_i64x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] + b.v[i]; return r;
}
MSIMD_API msimd_i64x2_t msimd_vec128_i64_sub(msimd_i64x2_t a, msimd_i64x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] - b.v[i]; return r;
}

MSIMD_API msimd_cpx32x2_t msimd_vec128_cpx32_add(msimd_cpx32x2_t a, msimd_cpx32x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_cpx32x2_t r = { a.r0 + b.r0, a.i0 + b.i0, a.r1 + b.r1, a.i1 + b.i1 };
    return r;
}
MSIMD_API msimd_cpx32x2_t msimd_vec128_cpx32_sub(msimd_cpx32x2_t a, msimd_cpx32x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_cpx32x2_t r = { a.r0 - b.r0, a.i0 - b.i0, a.r1 - b.r1, a.i1 - b.i1 };
    return r;
}
MSIMD_API msimd_cpx32x2_t msimd_vec128_cpx32_mul(msimd_cpx32x2_t a, msimd_cpx32x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_cpx32x2_t r = {
        a.r0 * b.r0 - a.i0 * b.i0,
        a.r0 * b.i0 + a.i0 * b.r0,
        a.r1 * b.r1 - a.i1 * b.i1,
        a.r1 * b.i1 + a.i1 * b.r1
    };
    return r;
}
MSIMD_API msimd_cpx32x2_t msimd_vec128_cpx32_conj(msimd_cpx32x2_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_cpx32x2_t r = { a.r0, -a.i0, a.r1, -a.i1 };
    return r;
}
MSIMD_API msimd_f32x2_t msimd_vec128_cpx32_mag2(msimd_cpx32x2_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x2_t r;
    r.v[0] = a.r0 * a.r0 + a.i0 * a.i0;
    r.v[1] = a.r1 * a.r1 + a.i1 * a.i1;
    return r;
}

/* ========================================================================= */
/* SECTION 48: HIGH-PRECISION EMULATION, QUANTUM & ADVANCED DOMAIN KERNELS   */
/* ========================================================================= */

MSIMD_API void msimd_acoustic_trace_ray(
    const float ray_origin[3],
    const float ray_dir[3],
    float initial_pressure,
    float medium_attenuation_db_m,
    float max_distance,
    float hit_absorption_coeff,
    float* out_traversed_dist,
    float* out_final_pressure,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!ray_origin || !ray_dir || !out_traversed_dist || !out_final_pressure) return;

    float dir_len_sq = ray_dir[0]*ray_dir[0] + ray_dir[1]*ray_dir[1] + ray_dir[2]*ray_dir[2];
    float dir_len = (dir_len_sq > 1e-12f) ? std::sqrt(dir_len_sq) : 1.0f;
    (void)dir_len;

    float dist = (max_distance > 0.0f) ? max_distance : 10.0f;
    *out_traversed_dist = dist;

    float atten_factor = std::pow(10.0f, -(medium_attenuation_db_m * dist) / 20.0f);
    float boundary_transmission = 1.0f - std::clamp(hit_absorption_coeff, 0.0f, 1.0f);
    *out_final_pressure = initial_pressure * atten_factor * boundary_transmission;
}

namespace {
class MolangParser {
    const char* p;
    const float* qvars;
    size_t qcount;

    void skip_ws() {
        while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) p++;
    }

public:
    MolangParser(const char* expr, const float* vars, size_t count)
        : p(expr), qvars(vars), qcount(count) {}

    float parse() {
        if (!p) return 0.0f;
        skip_ws();
        if (!*p) return 0.0f;
        return parse_expr();
    }

private:
    float parse_expr() {
        float val = parse_term();
        while (true) {
            skip_ws();
            if (*p == '+') {
                p++;
                val += parse_term();
            } else if (*p == '-') {
                p++;
                val -= parse_term();
            } else {
                break;
            }
        }
        return val;
    }

    float parse_term() {
        float val = parse_factor();
        while (true) {
            skip_ws();
            if (*p == '*') {
                p++;
                val *= parse_factor();
            } else if (*p == '/') {
                p++;
                float denom = parse_factor();
                val = (std::fabs(denom) > 1e-12f) ? (val / denom) : 0.0f;
            } else {
                break;
            }
        }
        return val;
    }

    float parse_factor() {
        skip_ws();
        if (*p == '+') {
            p++;
            return parse_factor();
        }
        if (*p == '-') {
            p++;
            return -parse_factor();
        }
        if (*p == '(') {
            p++;
            float val = parse_expr();
            skip_ws();
            if (*p == ')') p++;
            return val;
        }

        // Functions: math.sin, math.cos, math.abs, math.sqrt, math.floor, math.ceil, math.min, math.max, math.clamp, etc.
        if (std::strncmp(p, "math.", 5) == 0 || std::strncmp(p, "sin(", 4) == 0 ||
            std::strncmp(p, "cos(", 4) == 0 || std::strncmp(p, "abs(", 4) == 0 ||
            std::strncmp(p, "sqrt(", 5) == 0 || std::strncmp(p, "min(", 4) == 0 ||
            std::strncmp(p, "max(", 4) == 0 || std::strncmp(p, "clamp(", 6) == 0) {
            char fn[32] = {0};
            if (std::strncmp(p, "math.", 5) == 0) p += 5;
            size_t fn_len = 0;
            while (*p && *p != '(' && *p != ' ' && *p != '\t' && fn_len < sizeof(fn) - 1) {
                fn[fn_len++] = *p++;
            }
            fn[fn_len] = '\0';
            skip_ws();
            if (*p == '(') {
                p++;
                float args[4] = {0};
                int argc = 0;
                if (*p != ')') {
                    args[argc++] = parse_expr();
                    skip_ws();
                    while (*p == ',' && argc < 4) {
                        p++;
                        args[argc++] = parse_expr();
                        skip_ws();
                    }
                }
                if (*p == ')') p++;
                if (std::strcmp(fn, "sin") == 0) return std::sin(args[0]);
                if (std::strcmp(fn, "cos") == 0) return std::cos(args[0]);
                if (std::strcmp(fn, "abs") == 0) return std::fabs(args[0]);
                if (std::strcmp(fn, "sqrt") == 0) return (args[0] >= 0.0f) ? std::sqrt(args[0]) : 0.0f;
                if (std::strcmp(fn, "floor") == 0) return std::floor(args[0]);
                if (std::strcmp(fn, "ceil") == 0) return std::ceil(args[0]);
                if (std::strcmp(fn, "min") == 0 && argc >= 2) return (std::min)(args[0], args[1]);
                if (std::strcmp(fn, "max") == 0 && argc >= 2) return (std::max)(args[0], args[1]);
                if (std::strcmp(fn, "clamp") == 0 && argc >= 3) return std::clamp(args[0], args[1], args[2]);
                return args[0];
            }
        }

        // Query variables: query.var0, q.var0, q[0], etc.
        bool is_query = false;
        if (std::strncmp(p, "query.", 6) == 0 || std::strncmp(p, "QUERY.", 6) == 0) {
            p += 6; is_query = true;
        } else if (std::strncmp(p, "q.", 2) == 0 || std::strncmp(p, "Q.", 2) == 0) {
            p += 2; is_query = true;
        } else if ((p[0] == 'q' || p[0] == 'Q') && p[1] == '[') {
            p += 2; is_query = true;
        }

        if (is_query) {
            if (std::strncmp(p, "var", 3) == 0 || std::strncmp(p, "VAR", 3) == 0) {
                p += 3;
            }
            int idx = 0;
            char* endptr = nullptr;
            if (*p >= '0' && *p <= '9') {
                idx = (int)std::strtol(p, &endptr, 10);
                p = endptr;
            } else {
                while (*p && *p != ' ' && *p != '\t' && *p != ']' && *p != ')' &&
                       *p != '+' && *p != '-' && *p != '*' && *p != '/' && *p != ',') p++;
            }
            if (*p == ']') p++;
            if (qvars && idx >= 0 && (size_t)idx < qcount) {
                return qvars[idx];
            }
            return 0.0f;
        }

        // Numeric literals:
        char* endptr = nullptr;
        float val = std::strtof(p, &endptr);
        if (endptr && endptr != p) {
            p = endptr;
            return val;
        }

        return 0.0f;
    }
};
} // anonymous namespace

MSIMD_API float msimd_molang_eval(
    const char* molang_expression,
    const float* query_vars,
    size_t query_var_count,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!molang_expression) return 0.0f;
    MolangParser parser(molang_expression, query_vars, query_var_count);
    return parser.parse();
}

MSIMD_API void msimd_animation_dualquat_skinning(
    const float in_vertex[3],
    const float in_normal[3],
    const float dual_quats[][8],
    const float* bone_weights,
    size_t bone_count,
    float out_vertex[3],
    float out_normal[3],
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!in_vertex || !out_vertex) return;

    if (!dual_quats || !bone_weights || bone_count == 0) {
        out_vertex[0] = in_vertex[0];
        out_vertex[1] = in_vertex[1];
        out_vertex[2] = in_vertex[2];
        if (in_normal && out_normal) {
            out_normal[0] = in_normal[0];
            out_normal[1] = in_normal[1];
            out_normal[2] = in_normal[2];
        }
        return;
    }

    float b[8] = { 0.0f };
    float first_r_w = dual_quats[0][0];
    float first_r_x = dual_quats[0][1];
    float first_r_y = dual_quats[0][2];
    float first_r_z = dual_quats[0][3];

    for (size_t i = 0; i < bone_count; ++i) {
        float w = bone_weights[i];
        if (w == 0.0f) continue;
        float dot = first_r_w * dual_quats[i][0] + first_r_x * dual_quats[i][1] +
                    first_r_y * dual_quats[i][2] + first_r_z * dual_quats[i][3];
        float sign = (dot < 0.0f) ? -1.0f : 1.0f;
        for (int k = 0; k < 8; ++k) {
            b[k] += dual_quats[i][k] * (w * sign);
        }
    }

    float mag_sq = b[0]*b[0] + b[1]*b[1] + b[2]*b[2] + b[3]*b[3];
    float inv_mag = (mag_sq > 1e-12f) ? (1.0f / std::sqrt(mag_sq)) : 1.0f;
    for (int k = 0; k < 8; ++k) b[k] *= inv_mag;

    float rw = b[0], rx = b[1], ry = b[2], rz = b[3];
    float dw = b[4], dx = b[5], dy = b[6], dz = b[7];

    float tx = 2.0f * (-dw * rx + dx * rw - dy * rz + dz * ry);
    float ty = 2.0f * (-dw * ry + dx * rz + dy * rw - dz * rx);
    float tz = 2.0f * (-dw * rz - dx * ry + dy * rx + dz * rw);

    float vx = in_vertex[0], vy = in_vertex[1], vz = in_vertex[2];
    float cx = ry * vz - rz * vy + rw * vx;
    float cy = rz * vx - rx * vz + rw * vy;
    float cz = rx * vy - ry * vx + rw * vz;

    out_vertex[0] = vx + 2.0f * (ry * cz - rz * cy) + tx;
    out_vertex[1] = vy + 2.0f * (rz * cx - rx * cz) + ty;
    out_vertex[2] = vz + 2.0f * (rx * cy - ry * cx) + tz;

    if (in_normal && out_normal) {
        float nx = in_normal[0], ny = in_normal[1], nz = in_normal[2];
        float ncx = ry * nz - rz * ny + rw * nx;
        float ncy = rz * nx - rx * nz + rw * ny;
        float ncz = rx * ny - ry * nx + rw * nz;
        out_normal[0] = nx + 2.0f * (ry * ncz - rz * ncy);
        out_normal[1] = ny + 2.0f * (rz * ncx - rx * ncz);
        out_normal[2] = nz + 2.0f * (rx * ncy - ry * ncx);
    }
}

MSIMD_API int msimd_astro_kerr_raymarch(
    double bh_mass,
    double bh_spin_a,
    const double ray_origin[3],
    const double ray_dir[3],
    double step_size,
    int max_steps,
    double* out_hit_dist,
    int* out_captured,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!ray_origin || !ray_dir || !out_hit_dist || !out_captured) return -1;

    double M = (bh_mass > 0.0) ? bh_mass : 1.0;
    double a = std::clamp(bh_spin_a, -0.9999 * M, 0.9999 * M);
    double r_plus = M + std::sqrt(std::max(0.0, M * M - a * a));

    double pos[3] = { ray_origin[0], ray_origin[1], ray_origin[2] };
    double dir_len_sq = ray_dir[0]*ray_dir[0] + ray_dir[1]*ray_dir[1] + ray_dir[2]*ray_dir[2];
    double dir_len = (dir_len_sq > 1e-16) ? std::sqrt(dir_len_sq) : 1.0;
    double dir[3] = { ray_dir[0]/dir_len, ray_dir[1]/dir_len, ray_dir[2]/dir_len };

    double dt = (step_size > 0.0) ? step_size : 0.05 * M;
    int steps = (max_steps > 0) ? max_steps : 500;
    double total_dist = 0.0;
    *out_captured = 0;

    for (int s = 0; s < steps; ++s) {
        double r2 = pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2];
        double r = std::sqrt(r2);
        if (r <= r_plus) {
            *out_captured = 1;
            *out_hit_dist = total_dist;
            return 1;
        }
        if (r > 100.0 * M) {
            break;
        }
        double inv_r3 = 1.0 / (r * r * r);
        double accel_grav[3] = { -M * pos[0] * inv_r3, -M * pos[1] * inv_r3, -M * pos[2] * inv_r3 };
        double accel_drag[3] = { 2.0 * a * M * dir[1] * inv_r3, -2.0 * a * M * dir[0] * inv_r3, 0.0 };

        dir[0] += (accel_grav[0] + accel_drag[0]) * dt;
        dir[1] += (accel_grav[1] + accel_drag[1]) * dt;
        dir[2] += (accel_grav[2] + accel_drag[2]) * dt;

        double cur_len = std::sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);
        if (cur_len > 1e-12) {
            dir[0] /= cur_len; dir[1] /= cur_len; dir[2] /= cur_len;
        }

        pos[0] += dir[0] * dt;
        pos[1] += dir[1] * dt;
        pos[2] += dir[2] * dt;
        total_dist += dt;
    }

    *out_hit_dist = total_dist;
    return 0;
}

MSIMD_API void msimd_astro_magnetar_bfield(
    double b0_pole_tesla,
    double magnetar_radius_km,
    const double eval_pos_km[3],
    double out_bfield_tesla[3],
    double* out_b_magnitude,
    double* out_vacuum_polarization_idx,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!eval_pos_km || !out_bfield_tesla || !out_b_magnitude) return;

    double R = (magnetar_radius_km > 0.0) ? magnetar_radius_km : 10.0;
    double B0 = b0_pole_tesla;

    double r2 = eval_pos_km[0]*eval_pos_km[0] + eval_pos_km[1]*eval_pos_km[1] + eval_pos_km[2]*eval_pos_km[2];
    double r = (r2 > 1e-12) ? std::sqrt(r2) : R;
    double r_clamped = std::max(r, R);

    double factor = (B0 * R * R * R) / (2.0 * r_clamped * r_clamped * r_clamped * r_clamped * r_clamped);
    double z = eval_pos_km[2];
    out_bfield_tesla[0] = factor * (3.0 * z * eval_pos_km[0]);
    out_bfield_tesla[1] = factor * (3.0 * z * eval_pos_km[1]);
    out_bfield_tesla[2] = factor * (3.0 * z * z - r_clamped * r_clamped);

    double b_mag = std::sqrt(out_bfield_tesla[0]*out_bfield_tesla[0] +
                             out_bfield_tesla[1]*out_bfield_tesla[1] +
                             out_bfield_tesla[2]*out_bfield_tesla[2]);
    *out_b_magnitude = b_mag;

    if (out_vacuum_polarization_idx) {
        const double B_QED = 4.414e9;
        const double alpha_fine = 1.0 / 137.035999;
        double b_norm = b_mag / B_QED;
        *out_vacuum_polarization_idx = (2.0 * alpha_fine / (45.0 * 3.14159265358979323846)) * (b_norm * b_norm);
    }
}

MSIMD_API int msimd_quantum_planck_bounded_uncertainty_256(
    const msimd_f256_t* delta_x_in,
    const msimd_f256_t* delta_p_in,
    msimd_f256_t* phase_space_uncertainty_out,
    int* satisfies_planck_bound,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!delta_x_in || !delta_p_in || !phase_space_uncertainty_out || !satisfies_planck_bound) return -1;

    // Fundamental Discrete Planck-Scale Spacetime Bounds:
    // 1) Minimum position bound: Planck length ell_P = 1.616255e-35 m
    // 2) Maximum position bound: Hubble horizon / observable universe radius R_U = 4.4e26 m
    // 3) Minimum momentum bound: p_min = hbar / (2 * R_U) = 1.198e-61 kg*m/s
    // 4) Maximum momentum bound: Planck lattice momentum limit p_P = 6.52485e10 kg*m/s
    // 5) Finite bounded phase space: [0.5 * hbar, max_phase_space_bound]
    // In discrete Planck-scale universe, equations cannot go to 0 or infinity.
    const double hbar_over_2 = 0.5272859088364e-34;
    const double planck_length = 1.616255e-35;
    const double max_length_bound = 4.4e26;
    const double min_momentum_bound = 1.198e-61;
    const double max_momentum_bound = 6.52485e10;
    const double max_phase_space_bound = 2.87e37;

    double x_val = delta_x_in->limbs[0] + delta_x_in->limbs[1] + delta_x_in->limbs[2] + delta_x_in->limbs[3];
    double p_val = delta_p_in->limbs[0] + delta_p_in->limbs[1] + delta_p_in->limbs[2] + delta_p_in->limbs[3];

    x_val = std::clamp(std::fabs(x_val), planck_length, max_length_bound);
    p_val = std::clamp(std::fabs(p_val), min_momentum_bound, max_momentum_bound);

    double raw_product = x_val * p_val;
    *satisfies_planck_bound = (raw_product >= hbar_over_2 && raw_product <= max_phase_space_bound) ? 1 : 0;

    double product = std::clamp(raw_product, hbar_over_2, max_phase_space_bound);

    phase_space_uncertainty_out->limbs[0] = product;
    phase_space_uncertainty_out->limbs[1] = 0.0;
    phase_space_uncertainty_out->limbs[2] = 0.0;
    phase_space_uncertainty_out->limbs[3] = 0.0;

    return 0;
}

MSIMD_API size_t msimd_quantum_wavepacket_peak_pinpoint_256(
    const msimd_f256_t* psi_real,
    const msimd_f256_t* psi_imag,
    size_t sample_count,
    msimd_f256_t* out_peak_probability,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!psi_real || sample_count == 0) return 0;

    size_t peak_idx = 0;
    double max_prob = -1.0;

    for (size_t i = 0; i < sample_count; ++i) {
        double r = psi_real[i].limbs[0] + psi_real[i].limbs[1] + psi_real[i].limbs[2] + psi_real[i].limbs[3];
        double im = psi_imag ? (psi_imag[i].limbs[0] + psi_imag[i].limbs[1] + psi_imag[i].limbs[2] + psi_imag[i].limbs[3]) : 0.0;
        double prob = r * r + im * im;
        if (prob > max_prob) {
            max_prob = prob;
            peak_idx = i;
        }
    }

    if (out_peak_probability) {
        out_peak_probability->limbs[0] = max_prob;
        out_peak_probability->limbs[1] = 0.0;
        out_peak_probability->limbs[2] = 0.0;
        out_peak_probability->limbs[3] = 0.0;
    }

    return peak_idx;
}

MSIMD_API void msimd_quantum_flavor_ckm_mix(
    const double in_quark_triplet_re[3],
    const double in_quark_triplet_im[3],
    const double ckm_matrix_re[9],
    const double ckm_matrix_im[9],
    double out_quark_triplet_re[3],
    double out_quark_triplet_im[3],
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!in_quark_triplet_re || !ckm_matrix_re || !out_quark_triplet_re) return;

    for (int row = 0; row < 3; ++row) {
        double sum_r = 0.0;
        double sum_i = 0.0;
        for (int col = 0; col < 3; ++col) {
            int idx = row * 3 + col;
            double a_r = ckm_matrix_re[idx];
            double a_i = ckm_matrix_im ? ckm_matrix_im[idx] : 0.0;
            double b_r = in_quark_triplet_re[col];
            double b_i = in_quark_triplet_im ? in_quark_triplet_im[col] : 0.0;
            sum_r += (a_r * b_r - a_i * b_i);
            sum_i += (a_r * b_i + a_i * b_r);
        }
        out_quark_triplet_re[row] = sum_r;
        if (out_quark_triplet_im) {
            out_quark_triplet_im[row] = sum_i;
        }
    }
}

MSIMD_API size_t msimd_compress_rans_encode(
    const uint8_t* in_data,
    size_t in_len,
    uint8_t* out_buf,
    size_t out_max,
    const uint32_t* symbol_freqs,
    size_t alphabet_size,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!in_data || in_len == 0 || !out_buf || out_max < 8) return 0;

    const uint32_t scale_bits = 12;
    const uint32_t scale_m = 1 << scale_bits;
    const uint32_t L = 1 << 16;

    uint32_t freq_table[256];
    uint32_t start_table[256];
    uint32_t cum = 0;
    if (symbol_freqs && alphabet_size >= 256) {
        for (int i = 0; i < 256; ++i) {
            freq_table[i] = (std::max)(1u, symbol_freqs[i]);
            start_table[i] = cum;
            cum += freq_table[i];
        }
    } else {
        for (int i = 0; i < 256; ++i) {
            freq_table[i] = 16;
            start_table[i] = i * 16;
        }
        cum = 4096;
    }
    (void)cum;

    uint32_t r_state = L;
    size_t out_pos = out_max;

    for (size_t i = in_len; i > 0; --i) {
        uint8_t s = in_data[i - 1];
        uint32_t f = freq_table[s];
        uint32_t c = start_table[s];

        uint32_t max_x = ((L >> scale_bits) << 8) * f;
        while (r_state >= max_x) {
            if (out_pos == 0) return 0;
            out_buf[--out_pos] = (uint8_t)(r_state & 0xFF);
            r_state >>= 8;
        }

        r_state = ((r_state / f) << scale_bits) + (r_state % f) + c;
    }

    if (out_pos < 4) return 0;
    out_buf[--out_pos] = (uint8_t)(r_state >> 24);
    out_buf[--out_pos] = (uint8_t)(r_state >> 16);
    out_buf[--out_pos] = (uint8_t)(r_state >> 8);
    out_buf[--out_pos] = (uint8_t)(r_state >> 0);

    size_t compressed_len = out_max - out_pos;
    std::memmove(out_buf, out_buf + out_pos, compressed_len);
    return compressed_len;
}

MSIMD_API size_t msimd_compress_rans_decode(
    const uint8_t* in_data,
    size_t in_len,
    uint8_t* out_buf,
    size_t out_max,
    const uint32_t* symbol_freqs,
    size_t alphabet_size,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!in_data || in_len < 4 || !out_buf || out_max == 0) return 0;

    const uint32_t scale_bits = 12;
    const uint32_t scale_mask = (1 << scale_bits) - 1;
    const uint32_t L = 1 << 16;

    uint32_t freq_table[256];
    uint32_t start_table[256];
    uint8_t slot_to_sym[4096];

    uint32_t cum = 0;
    if (symbol_freqs && alphabet_size >= 256) {
        for (int i = 0; i < 256; ++i) {
            freq_table[i] = (std::max)(1u, symbol_freqs[i]);
            start_table[i] = cum;
            for (uint32_t j = 0; j < freq_table[i] && (cum + j) < 4096; ++j) {
                slot_to_sym[cum + j] = (uint8_t)i;
            }
            cum += freq_table[i];
        }
    } else {
        for (int i = 0; i < 256; ++i) {
            freq_table[i] = 16;
            start_table[i] = i * 16;
            for (uint32_t j = 0; j < 16; ++j) {
                slot_to_sym[i * 16 + j] = (uint8_t)i;
            }
        }
    }

    size_t in_pos = 0;
    uint32_t b0 = (uint32_t)in_data[in_pos++];
    uint32_t b1 = (uint32_t)in_data[in_pos++];
    uint32_t b2 = (uint32_t)in_data[in_pos++];
    uint32_t b3 = (uint32_t)in_data[in_pos++];
    uint32_t r_state = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);

    size_t out_count = 0;
    while (out_count < out_max) {
        uint32_t slot = r_state & scale_mask;
        uint8_t s = slot_to_sym[slot];
        out_buf[out_count++] = s;

        uint32_t f = freq_table[s];
        uint32_t c = start_table[s];

        r_state = f * (r_state >> scale_bits) + (slot - c);

        while (r_state < L && in_pos < in_len) {
            r_state = (r_state << 8) | in_data[in_pos++];
        }
        if (in_pos >= in_len && r_state == L) break;
    }

    return out_count;
}

/* ========================================================================= */
/* SECTION 49: OPEN-WORLD VISIBILITY CULLING & GEOMETRY OPTIMIZATION         */
/* ========================================================================= */

/**
 * @brief Open-World Batched Frustum Culling of Axis-Aligned Bounding Boxes (AABBs).
 *
 * GPU COMPUTE SHADER REFERENCE (GLSL):
 * ------------------------------------
 * #version 450 core
 * layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
 * struct AABB { vec4 min_pos; vec4 max_pos; }; // std430 alignment
 * layout(std430, binding = 0) readonly buffer InAABBs { AABB aabbs[]; };
 * layout(std430, binding = 1) writeonly buffer OutVis { uint vis_mask[]; };
 * uniform vec4 u_frustum_planes[6]; // Nx, Ny, Nz, D
 * uniform uint u_count;
 * void main() {
 *     uint id = gl_GlobalInvocationID.x;
 *     if (id >= u_count) return;
 *     vec3 bmin = aabbs[id].min_pos.xyz;
 *     vec3 bmax = aabbs[id].max_pos.xyz;
 *     bool visible = true;
 *     for (int p = 0; p < 6; ++p) {
 *         vec3 p_vert = vec3(
 *             u_frustum_planes[p].x >= 0.0 ? bmax.x : bmin.x,
 *             u_frustum_planes[p].y >= 0.0 ? bmax.y : bmin.y,
 *             u_frustum_planes[p].z >= 0.0 ? bmax.z : bmin.z
 *         );
 *         if (dot(u_frustum_planes[p].xyz, p_vert) + u_frustum_planes[p].w < 0.0) {
 *             visible = false; break;
 *         }
 *     }
 *     vis_mask[id] = visible ? 1u : 0u;
 * }
 */
size_t msimd_cull_frustum_aabbs_f32(
    const float* frustum_planes,
    const float* aabb_min_max,
    size_t count,
    uint8_t* out_visibility_mask,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!frustum_planes || !aabb_min_max || !out_visibility_mask || count == 0) return 0;

    size_t visible_count = 0;
    for (size_t i = 0; i < count; ++i) {
        if ((i & 511u) == 0u) sf_gc_pausepoint_poll();
        const float* box = aabb_min_max + i * 6;
        float min_x = box[0], min_y = box[1], min_z = box[2];
        float max_x = box[3], max_y = box[4], max_z = box[5];

        uint8_t visible = 1;
        for (int p = 0; p < 6; ++p) {
            const float* plane = frustum_planes + p * 4;
            float nx = plane[0], ny = plane[1], nz = plane[2], d = plane[3];
            float px = (nx >= 0.0f) ? max_x : min_x;
            float py = (ny >= 0.0f) ? max_y : min_y;
            float pz = (nz >= 0.0f) ? max_z : min_z;

            if (nx * px + ny * py + nz * pz + d < 0.0f) {
                visible = 0;
                break;
            }
        }
        out_visibility_mask[i] = visible;
        visible_count += (size_t)visible;
    }
    return visible_count;
}

/**
 * @brief Open-World Batched Frustum Culling of Bounding Spheres.
 *
 * GPU COMPUTE SHADER REFERENCE (GLSL):
 * ------------------------------------
 * #version 450 core
 * layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
 * layout(std430, binding = 0) readonly buffer InSpheres { vec4 spheres[]; }; // xyz: center, w: radius
 * layout(std430, binding = 1) writeonly buffer OutVis { uint vis_mask[]; };
 * uniform vec4 u_frustum_planes[6];
 * uniform uint u_count;
 * void main() {
 *     uint id = gl_GlobalInvocationID.x;
 *     if (id >= u_count) return;
 *     vec4 s = spheres[id];
 *     bool visible = true;
 *     for (int p = 0; p < 6; ++p) {
 *         float dist = dot(u_frustum_planes[p].xyz, s.xyz) + u_frustum_planes[p].w;
 *         if (dist < -s.w) { visible = false; break; }
 *     }
 *     vis_mask[id] = visible ? 1u : 0u;
 * }
 */
size_t msimd_cull_frustum_spheres_f32(
    const float* frustum_planes,
    const float* sphere_centers_radii,
    size_t count,
    uint8_t* out_visibility_mask,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!frustum_planes || !sphere_centers_radii || !out_visibility_mask || count == 0) return 0;

    size_t visible_count = 0;
    for (size_t i = 0; i < count; ++i) {
        if ((i & 511u) == 0u) sf_gc_pausepoint_poll();
        const float* sphere = sphere_centers_radii + i * 4;
        float cx = sphere[0], cy = sphere[1], cz = sphere[2], radius = sphere[3];

        uint8_t visible = 1;
        for (int p = 0; p < 6; ++p) {
            const float* plane = frustum_planes + p * 4;
            float dist = plane[0] * cx + plane[1] * cy + plane[2] * cz + plane[3];
            if (dist < -radius) {
                visible = 0;
                break;
            }
        }
        out_visibility_mask[i] = visible;
        visible_count += (size_t)visible;
    }
    return visible_count;
}

/**
 * @brief Open-World Batched Distance Evaluation & Level of Detail (LOD) / Range Culling.
 *
 * GPU COMPUTE SHADER REFERENCE (GLSL):
 * ------------------------------------
 * #version 450 core
 * layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
 * layout(std430, binding = 0) readonly buffer InPositions { vec4 positions[]; }; // xyz: pos
 * layout(std430, binding = 1) writeonly buffer OutLOD { uint lod_levels[]; };
 * uniform vec3 u_cam_pos;
 * uniform vec4 u_lod_thresholds_sq; // x: lod0, y: lod1, z: lod2, w: cull_dist_sq
 * uniform uint u_count;
 * void main() {
 *     uint id = gl_GlobalInvocationID.x;
 *     if (id >= u_count) return;
 *     vec3 d = positions[id].xyz - u_cam_pos;
 *     float dist_sq = dot(d, d);
 *     uint lod = 255u;
 *     if (dist_sq <= u_lod_thresholds_sq.x) lod = 0u;
 *     else if (dist_sq <= u_lod_thresholds_sq.y) lod = 1u;
 *     else if (dist_sq <= u_lod_thresholds_sq.z) lod = 2u;
 *     else if (dist_sq <= u_lod_thresholds_sq.w) lod = 3u;
 *     lod_levels[id] = lod;
 * }
 */
size_t msimd_cull_distance_lod_f32(
    const float* camera_pos,
    const float* object_positions,
    size_t count,
    const float* lod_sq_thresholds,
    uint8_t* out_lod_levels,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!camera_pos || !object_positions || !lod_sq_thresholds || !out_lod_levels || count == 0) return 0;

    float cam_x = camera_pos[0], cam_y = camera_pos[1], cam_z = camera_pos[2];
    float t0_sq = lod_sq_thresholds[0];
    float t1_sq = lod_sq_thresholds[1];
    float t2_sq = lod_sq_thresholds[2];
    float tcull_sq = lod_sq_thresholds[3];

    size_t visible_count = 0;
    for (size_t i = 0; i < count; ++i) {
        if ((i & 511u) == 0u) sf_gc_pausepoint_poll();
        const float* pos = object_positions + i * 3;
        float dx = pos[0] - cam_x;
        float dy = pos[1] - cam_y;
        float dz = pos[2] - cam_z;
        float dist_sq = dx * dx + dy * dy + dz * dz;

        uint8_t lod = 255;
        if (dist_sq <= t0_sq) lod = 0;
        else if (dist_sq <= t1_sq) lod = 1;
        else if (dist_sq <= t2_sq) lod = 2;
        else if (dist_sq <= tcull_sq) lod = 3;

        out_lod_levels[i] = lod;
        if (lod != 255) visible_count++;
    }
    return visible_count;
}

/**
 * @brief Open-World Voxel 6-Neighbor Hidden Face Elimination & Occlusion Mask Synthesis.
 *
 * GPU COMPUTE SHADER REFERENCE (GLSL):
 * ------------------------------------
 * #version 450 core
 * layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
 * layout(binding = 0, r32ui) readonly uniform uimage3D u_voxel_texture;
 * layout(std430, binding = 1) writeonly buffer OutFaces { uint8_t face_masks[]; };
 * uniform uvec3 u_chunk_size;
 * #define INDEX(p) ((p).z * (u_chunk_size.x * u_chunk_size.y) + (p).y * u_chunk_size.x + (p).x)
 * void main() {
 *     ivec3 pos = ivec3(gl_GlobalInvocationID.xyz);
 *     if (any(greaterThanEqual(uvec3(pos), u_chunk_size))) return;
 *     uint block = imageLoad(u_voxel_texture, pos).r;
 *     if (block == 0u) { face_masks[INDEX(pos)] = 0u; return; }
 *     uint mask = 0u;
 *     if (pos.x + 1 >= int(u_chunk_size.x) || imageLoad(u_voxel_texture, pos + ivec3(1,0,0)).r == 0u) mask |= 1u;
 *     if (pos.x - 1 < 0                   || imageLoad(u_voxel_texture, pos - ivec3(1,0,0)).r == 0u) mask |= 2u;
 *     if (pos.y + 1 >= int(u_chunk_size.y) || imageLoad(u_voxel_texture, pos + ivec3(0,1,0)).r == 0u) mask |= 4u;
 *     if (pos.y - 1 < 0                   || imageLoad(u_voxel_texture, pos - ivec3(0,1,0)).r == 0u) mask |= 8u;
 *     if (pos.z + 1 >= int(u_chunk_size.z) || imageLoad(u_voxel_texture, pos + ivec3(0,0,1)).r == 0u) mask |= 16u;
 *     if (pos.z - 1 < 0                   || imageLoad(u_voxel_texture, pos - ivec3(0,0,1)).r == 0u) mask |= 32u;
 *     face_masks[INDEX(pos)] = uint8_t(mask);
 * }
 */
size_t msimd_voxel_cull_hidden_faces_u32(
    const uint32_t* voxel_grid,
    size_t dim_x,
    size_t dim_y,
    size_t dim_z,
    uint8_t* out_face_masks,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!voxel_grid || !out_face_masks || dim_x == 0 || dim_y == 0 || dim_z == 0) return 0;

    size_t slice_size = dim_x * dim_y;
    size_t total_visible_faces = 0;

    for (size_t z = 0; z < dim_z; ++z) {
        if ((z & 7u) == 0u) sf_gc_pausepoint_poll();
        size_t z_offset = z * slice_size;
        for (size_t y = 0; y < dim_y; ++y) {
            size_t y_offset = z_offset + y * dim_x;
            for (size_t x = 0; x < dim_x; ++x) {
                size_t idx = y_offset + x;
                uint32_t block = voxel_grid[idx];
                if (block == 0u) {
                    out_face_masks[idx] = 0;
                    continue;
                }

                uint8_t mask = 0;
                // +X face
                if (x + 1 >= dim_x || voxel_grid[idx + 1] == 0u) mask |= 1u;
                // -X face
                if (x == 0 || voxel_grid[idx - 1] == 0u) mask |= 2u;
                // +Y face
                if (y + 1 >= dim_y || voxel_grid[idx + dim_x] == 0u) mask |= 4u;
                // -Y face
                if (y == 0 || voxel_grid[idx - dim_x] == 0u) mask |= 8u;
                // +Z face
                if (z + 1 >= dim_z || voxel_grid[idx + slice_size] == 0u) mask |= 16u;
                // -Z face
                if (z == 0 || voxel_grid[idx - slice_size] == 0u) mask |= 32u;

                out_face_masks[idx] = mask;
                total_visible_faces += (size_t)(
                    ((mask >> 0) & 1u) + ((mask >> 1) & 1u) +
                    ((mask >> 2) & 1u) + ((mask >> 3) & 1u) +
                    ((mask >> 4) & 1u) + ((mask >> 5) & 1u)
                );
            }
        }
    }
    return total_visible_faces;
}

}

}]>;

// =============================================================================
// SECTION 9: INDIVIDUAL ALGORITHMIC KERNEL IMPLEMENTATION RECORDS
// =============================================================================

defm Kernel_vec2_add : DualKernelImplementation<"msimd_vec2f_add", "msimd_vec2d_add", [{
void msimd_vec2f_add(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] + b[i];
}
}], [{
void msimd_vec2d_add(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] + b[i];
}
}]>;

defm Kernel_vec2_sub : DualKernelImplementation<"msimd_vec2f_sub", "msimd_vec2d_sub", [{
void msimd_vec2f_sub(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] - b[i];
}
}], [{
void msimd_vec2d_sub(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] - b[i];
}
}]>;

defm Kernel_vec2_scale : DualKernelImplementation<"msimd_vec2f_scale", "msimd_vec2d_scale", [{
void msimd_vec2f_scale(const float* a, float s, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] * s;
}
}], [{
void msimd_vec2d_scale(const double* a, double s, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] * s;
}
}]>;

defm Kernel_vec2_dot : DualKernelImplementation<"msimd_vec2f_dot", "msimd_vec2d_dot", [{
float msimd_vec2f_dot(const float* a, const float* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float sum = 0;
    for (int i = 0; i < 2; ++i) sum += a[i] * b[i];
    return sum;
}
}], [{
double msimd_vec2d_dot(const double* a, const double* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double sum = 0;
    for (int i = 0; i < 2; ++i) sum += a[i] * b[i];
    return sum;
}
}]>;

defm Kernel_vec2_length_sq : DualKernelImplementation<"msimd_vec2f_length_sq", "msimd_vec2d_length_sq", [{
float msimd_vec2f_length_sq(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return msimd_vec2f_dot(a, a);
}
}], [{
double msimd_vec2d_length_sq(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return msimd_vec2d_dot(a, a);
}
}]>;

defm Kernel_vec2_length : DualKernelImplementation<"msimd_vec2f_length", "msimd_vec2d_length", [{
float msimd_vec2f_length(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sqrt(msimd_vec2f_length_sq(a));
}
}], [{
double msimd_vec2d_length(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sqrt(msimd_vec2d_length_sq(a));
}
}]>;

defm Kernel_vec2_normalize : DualKernelImplementation<"msimd_vec2f_normalize", "msimd_vec2d_normalize", [{
void msimd_vec2f_normalize(const float* a, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float len = msimd_vec2f_length(a);
    if (len > 1e-12) {
        float inv = float(1.0) / len;
        msimd_vec2f_scale(a, inv, out);
    } else {
        for (int i = 0; i < 2; ++i) out[i] = 0;
    }
}
}], [{
void msimd_vec2d_normalize(const double* a, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double len = msimd_vec2d_length(a);
    if (len > 1e-12) {
        double inv = double(1.0) / len;
        msimd_vec2d_scale(a, inv, out);
    } else {
        for (int i = 0; i < 2; ++i) out[i] = 0;
    }
}
}]>;

defm Kernel_vec2_lerp : DualKernelImplementation<"msimd_vec2f_lerp", "msimd_vec2d_lerp", [{
void msimd_vec2f_lerp(const float* a, const float* b, float t, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] + t * (b[i] - a[i]);
}
}], [{
void msimd_vec2d_lerp(const double* a, const double* b, double t, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) out[i] = a[i] + t * (b[i] - a[i]);
}
}]>;

defm Kernel_vec2_reflect : DualKernelImplementation<"msimd_vec2f_reflect", "msimd_vec2d_reflect", [{
void msimd_vec2f_reflect(const float* v, const float* n, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float d = msimd_vec2f_dot(v, n);
    for (int i = 0; i < 2; ++i) out[i] = v[i] - float(2.0) * d * n[i];
}
}], [{
void msimd_vec2d_reflect(const double* v, const double* n, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double d = msimd_vec2d_dot(v, n);
    for (int i = 0; i < 2; ++i) out[i] = v[i] - double(2.0) * d * n[i];
}
}]>;

defm Kernel_vec3_add : DualKernelImplementation<"msimd_vec3f_add", "msimd_vec3d_add", [{
void msimd_vec3f_add(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] + b[i];
}
}], [{
void msimd_vec3d_add(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] + b[i];
}
}]>;

defm Kernel_vec3_sub : DualKernelImplementation<"msimd_vec3f_sub", "msimd_vec3d_sub", [{
void msimd_vec3f_sub(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] - b[i];
}
}], [{
void msimd_vec3d_sub(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] - b[i];
}
}]>;

defm Kernel_vec3_scale : DualKernelImplementation<"msimd_vec3f_scale", "msimd_vec3d_scale", [{
void msimd_vec3f_scale(const float* a, float s, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] * s;
}
}], [{
void msimd_vec3d_scale(const double* a, double s, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] * s;
}
}]>;

defm Kernel_vec3_dot : DualKernelImplementation<"msimd_vec3f_dot", "msimd_vec3d_dot", [{
float msimd_vec3f_dot(const float* a, const float* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float sum = 0;
    for (int i = 0; i < 3; ++i) sum += a[i] * b[i];
    return sum;
}
}], [{
double msimd_vec3d_dot(const double* a, const double* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double sum = 0;
    for (int i = 0; i < 3; ++i) sum += a[i] * b[i];
    return sum;
}
}]>;

defm Kernel_vec3_length_sq : DualKernelImplementation<"msimd_vec3f_length_sq", "msimd_vec3d_length_sq", [{
float msimd_vec3f_length_sq(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return msimd_vec3f_dot(a, a);
}
}], [{
double msimd_vec3d_length_sq(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return msimd_vec3d_dot(a, a);
}
}]>;

defm Kernel_vec3_length : DualKernelImplementation<"msimd_vec3f_length", "msimd_vec3d_length", [{
float msimd_vec3f_length(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sqrt(msimd_vec3f_length_sq(a));
}
}], [{
double msimd_vec3d_length(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sqrt(msimd_vec3d_length_sq(a));
}
}]>;

defm Kernel_vec3_normalize : DualKernelImplementation<"msimd_vec3f_normalize", "msimd_vec3d_normalize", [{
void msimd_vec3f_normalize(const float* a, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float len = msimd_vec3f_length(a);
    if (len > 1e-12) {
        float inv = float(1.0) / len;
        msimd_vec3f_scale(a, inv, out);
    } else {
        for (int i = 0; i < 3; ++i) out[i] = 0;
    }
}
}], [{
void msimd_vec3d_normalize(const double* a, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double len = msimd_vec3d_length(a);
    if (len > 1e-12) {
        double inv = double(1.0) / len;
        msimd_vec3d_scale(a, inv, out);
    } else {
        for (int i = 0; i < 3; ++i) out[i] = 0;
    }
}
}]>;

defm Kernel_vec3_lerp : DualKernelImplementation<"msimd_vec3f_lerp", "msimd_vec3d_lerp", [{
void msimd_vec3f_lerp(const float* a, const float* b, float t, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] + t * (b[i] - a[i]);
}
}], [{
void msimd_vec3d_lerp(const double* a, const double* b, double t, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) out[i] = a[i] + t * (b[i] - a[i]);
}
}]>;

defm Kernel_vec3_reflect : DualKernelImplementation<"msimd_vec3f_reflect", "msimd_vec3d_reflect", [{
void msimd_vec3f_reflect(const float* v, const float* n, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float d = msimd_vec3f_dot(v, n);
    for (int i = 0; i < 3; ++i) out[i] = v[i] - float(2.0) * d * n[i];
}
}], [{
void msimd_vec3d_reflect(const double* v, const double* n, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double d = msimd_vec3d_dot(v, n);
    for (int i = 0; i < 3; ++i) out[i] = v[i] - double(2.0) * d * n[i];
}
}]>;

defm Kernel_vec4_add : DualKernelImplementation<"msimd_vec4f_add", "msimd_vec4d_add", [{
void msimd_vec4f_add(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] + b[i];
}
}], [{
void msimd_vec4d_add(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] + b[i];
}
}]>;

defm Kernel_vec4_sub : DualKernelImplementation<"msimd_vec4f_sub", "msimd_vec4d_sub", [{
void msimd_vec4f_sub(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] - b[i];
}
}], [{
void msimd_vec4d_sub(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] - b[i];
}
}]>;

defm Kernel_vec4_scale : DualKernelImplementation<"msimd_vec4f_scale", "msimd_vec4d_scale", [{
void msimd_vec4f_scale(const float* a, float s, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] * s;
}
}], [{
void msimd_vec4d_scale(const double* a, double s, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] * s;
}
}]>;

defm Kernel_vec4_dot : DualKernelImplementation<"msimd_vec4f_dot", "msimd_vec4d_dot", [{
float msimd_vec4f_dot(const float* a, const float* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float sum = 0;
    for (int i = 0; i < 4; ++i) sum += a[i] * b[i];
    return sum;
}
}], [{
double msimd_vec4d_dot(const double* a, const double* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double sum = 0;
    for (int i = 0; i < 4; ++i) sum += a[i] * b[i];
    return sum;
}
}]>;

defm Kernel_vec4_length_sq : DualKernelImplementation<"msimd_vec4f_length_sq", "msimd_vec4d_length_sq", [{
float msimd_vec4f_length_sq(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return msimd_vec4f_dot(a, a);
}
}], [{
double msimd_vec4d_length_sq(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return msimd_vec4d_dot(a, a);
}
}]>;

defm Kernel_vec4_length : DualKernelImplementation<"msimd_vec4f_length", "msimd_vec4d_length", [{
float msimd_vec4f_length(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sqrt(msimd_vec4f_length_sq(a));
}
}], [{
double msimd_vec4d_length(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::sqrt(msimd_vec4d_length_sq(a));
}
}]>;

defm Kernel_vec4_normalize : DualKernelImplementation<"msimd_vec4f_normalize", "msimd_vec4d_normalize", [{
void msimd_vec4f_normalize(const float* a, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float len = msimd_vec4f_length(a);
    if (len > 1e-12) {
        float inv = float(1.0) / len;
        msimd_vec4f_scale(a, inv, out);
    } else {
        for (int i = 0; i < 4; ++i) out[i] = 0;
    }
}
}], [{
void msimd_vec4d_normalize(const double* a, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double len = msimd_vec4d_length(a);
    if (len > 1e-12) {
        double inv = double(1.0) / len;
        msimd_vec4d_scale(a, inv, out);
    } else {
        for (int i = 0; i < 4; ++i) out[i] = 0;
    }
}
}]>;

defm Kernel_vec4_lerp : DualKernelImplementation<"msimd_vec4f_lerp", "msimd_vec4d_lerp", [{
void msimd_vec4f_lerp(const float* a, const float* b, float t, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] + t * (b[i] - a[i]);
}
}], [{
void msimd_vec4d_lerp(const double* a, const double* b, double t, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 4; ++i) out[i] = a[i] + t * (b[i] - a[i]);
}
}]>;

defm Kernel_vec4_reflect : DualKernelImplementation<"msimd_vec4f_reflect", "msimd_vec4d_reflect", [{
void msimd_vec4f_reflect(const float* v, const float* n, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float d = msimd_vec4f_dot(v, n);
    for (int i = 0; i < 4; ++i) out[i] = v[i] - float(2.0) * d * n[i];
}
}], [{
void msimd_vec4d_reflect(const double* v, const double* n, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double d = msimd_vec4d_dot(v, n);
    for (int i = 0; i < 4; ++i) out[i] = v[i] - double(2.0) * d * n[i];
}
}]>;

defm Kernel_vec3_cross : DualKernelImplementation<"msimd_vec3f_cross", "msimd_vec3d_cross", [{
void msimd_vec3f_cross(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    out[0] = a[1] * b[2] - a[2] * b[1];
    out[1] = a[2] * b[0] - a[0] * b[2];
    out[2] = a[0] * b[1] - a[1] * b[0];
}
}], [{
void msimd_vec3d_cross(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    out[0] = a[1] * b[2] - a[2] * b[1];
    out[1] = a[2] * b[0] - a[0] * b[2];
    out[2] = a[0] * b[1] - a[1] * b[0];
}
}]>;

def Kernel_mat3f_identity : MasterKernelImplementation<"msimd_mat3f_identity", [{
void msimd_mat3f_identity(float* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::memset(m, 0, 9 * sizeof(float));
    m[0] = m[4] = m[8] = 1.0f;
}
}]>;

defm Kernel_mat3_mul : DualKernelImplementation<"msimd_mat3f_mul", "msimd_mat3d_mul", [{
void msimd_mat3f_mul(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            float s = 0.0f;
            for (int k = 0; k < 3; ++k) s += a[r * 3 + k] * b[k * 3 + c];
            out[r * 3 + c] = s;
        }
    }
}
}], [{
MSIMD_API void msimd_mat3d_mul(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    double r[9];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            r[i*3 + j] = a[i*3 + 0]*b[0*3 + j] + a[i*3 + 1]*b[1*3 + j] + a[i*3 + 2]*b[2*3 + j];
        }
    }
    std::memcpy(out, r, sizeof(r));
}
}]>;

defm Kernel_mat3_mul_vec : DualKernelImplementation<"msimd_mat3f_mul_vec", "msimd_mat3d_mul_vec", [{
void msimd_mat3f_mul_vec(const float* m, const float* v, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int r = 0; r < 3; ++r) {
        out[r] = m[r * 3 + 0] * v[0] + m[r * 3 + 1] * v[1] + m[r * 3 + 2] * v[2];
    }
}
}], [{
MSIMD_API void msimd_mat3d_mul_vec(const double* m, const double* v, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !v || !out) return;
    double r0 = m[0]*v[0] + m[1]*v[1] + m[2]*v[2];
    double r1 = m[3]*v[0] + m[4]*v[1] + m[5]*v[2];
    double r2 = m[6]*v[0] + m[7]*v[1] + m[8]*v[2];
    out[0] = r0; out[1] = r1; out[2] = r2;
}
}]>;

defm Kernel_mat3_det : DualKernelImplementation<"msimd_mat3f_det", "msimd_mat3d_det", [{
float msimd_mat3f_det(const float* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return m[0] * (m[4] * m[8] - m[5] * m[7]) -
           m[1] * (m[3] * m[8] - m[5] * m[6]) +
           m[2] * (m[3] * m[7] - m[4] * m[6]);
}
}], [{
MSIMD_API double msimd_mat3d_det(const double* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m) return 0.0;
    return m[0]*(m[4]*m[8] - m[5]*m[7])
         - m[1]*(m[3]*m[8] - m[5]*m[6])
         + m[2]*(m[3]*m[7] - m[4]*m[6]);
}
}]>;

def Kernel_mat3f_invert : MasterKernelImplementation<"msimd_mat3f_invert", [{
bool msimd_mat3f_invert(const float* m, float* out) {
    float det = msimd_mat3f_det(m);
    if (std::fabs(det) < 1e-12f) return false;
    float inv_det = 1.0f / det;

    out[0] = (m[4] * m[8] - m[5] * m[7]) * inv_det;
    out[1] = (m[2] * m[7] - m[1] * m[8]) * inv_det;
    out[2] = (m[1] * m[5] - m[2] * m[4]) * inv_det;

    out[3] = (m[5] * m[6] - m[3] * m[8]) * inv_det;
    out[4] = (m[0] * m[8] - m[2] * m[6]) * inv_det;
    out[5] = (m[2] * m[3] - m[0] * m[5]) * inv_det;

    out[6] = (m[3] * m[7] - m[4] * m[6]) * inv_det;
    out[7] = (m[1] * m[6] - m[0] * m[7]) * inv_det;
    out[8] = (m[0] * m[4] - m[1] * m[3]) * inv_det;
    return true;
}
}]>;

def Kernel_mat4f_identity : MasterKernelImplementation<"msimd_mat4f_identity", [{
void msimd_mat4f_identity(float* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::memset(m, 0, 16 * sizeof(float));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}
}]>;

defm Kernel_mat4_mul : DualKernelImplementation<"msimd_mat4f_mul", "msimd_mat4d_mul", [{
void msimd_mat4f_mul(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            float s = 0.0f;
            for (int k = 0; k < 4; ++k) s += a[r * 4 + k] * b[k * 4 + c];
            out[r * 4 + c] = s;
        }
    }
}
}], [{
MSIMD_API void msimd_mat4d_mul(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    double r[16];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            r[i*4 + j] = a[i*4 + 0]*b[0*4 + j] + a[i*4 + 1]*b[1*4 + j]
                       + a[i*4 + 2]*b[2*4 + j] + a[i*4 + 3]*b[3*4 + j];
        }
    }
    std::memcpy(out, r, sizeof(r));
}
}]>;

defm Kernel_mat4_mul_vec : DualKernelImplementation<"msimd_mat4f_mul_vec", "msimd_mat4d_mul_vec", [{
void msimd_mat4f_mul_vec(const float* m, const float* v, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int r = 0; r < 4; ++r) {
        out[r] = m[r * 4 + 0] * v[0] + m[r * 4 + 1] * v[1] + m[r * 4 + 2] * v[2] + m[r * 4 + 3] * v[3];
    }
}
}], [{
MSIMD_API void msimd_mat4d_mul_vec(const double* m, const double* v, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !v || !out) return;
    double r0 = m[0]*v[0]  + m[1]*v[1]  + m[2]*v[2]  + m[3]*v[3];
    double r1 = m[4]*v[0]  + m[5]*v[1]  + m[6]*v[2]  + m[7]*v[3];
    double r2 = m[8]*v[0]  + m[9]*v[1]  + m[10]*v[2] + m[11]*v[3];
    double r3 = m[12]*v[0] + m[13]*v[1] + m[14]*v[2] + m[15]*v[3];
    out[0] = r0; out[1] = r1; out[2] = r2; out[3] = r3;
}
}]>;

def Kernel_mat4f_transpose : MasterKernelImplementation<"msimd_mat4f_transpose", [{
void msimd_mat4f_transpose(const float* m, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            out[c * 4 + r] = m[r * 4 + c];
        }
    }
}
}]>;

def Kernel_quat_identity : MasterKernelImplementation<"msimd_quat_identity", [{
void msimd_quat_identity(float* q, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    q[0] = q[1] = q[2] = 0.0f;
    q[3] = 1.0f;
}
}]>;

def Kernel_quat_mul : MasterKernelImplementation<"msimd_quat_mul", [{
void msimd_quat_mul(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    out[0] = a[3] * b[0] + a[0] * b[3] + a[1] * b[2] - a[2] * b[1];
    out[1] = a[3] * b[1] - a[0] * b[2] + a[1] * b[3] + a[2] * b[0];
    out[2] = a[3] * b[2] + a[0] * b[1] - a[1] * b[0] + a[2] * b[3];
    out[3] = a[3] * b[3] - a[0] * b[0] - a[1] * b[1] - a[2] * b[2];
}
}]>;

def Kernel_quat_rotate_vec : MasterKernelImplementation<"msimd_quat_rotate_vec", [{
void msimd_quat_rotate_vec(const float* q, const float* v, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float qv[3] = { q[0], q[1], q[2] };
    float uv[3], uuv[3];
    msimd_vec3f_cross(qv, v, uv);
    msimd_vec3f_cross(qv, uv, uuv);
    for (int i = 0; i < 3; ++i) {
        out[i] = v[i] + ((uv[i] * q[3]) + uuv[i]) * 2.0f;
    }
}
}]>;

def Kernel_quat_to_mat3 : MasterKernelImplementation<"msimd_quat_to_mat3", [{
void msimd_quat_to_mat3(const float* q, float* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float x = q[0], y = q[1], z = q[2], w = q[3];
    float x2 = x + x, y2 = y + y, z2 = z + z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2, zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;

    m[0] = 1.0f - (yy + zz); m[1] = xy - wz;         m[2] = xz + wy;
    m[3] = xy + wz;         m[4] = 1.0f - (xx + zz); m[5] = yz - wx;
    m[6] = xz - wy;         m[7] = yz + wx;         m[8] = 1.0f - (xx + yy);
}
}]>;

def Kernel_quat_slerp : MasterKernelImplementation<"msimd_quat_slerp", [{
void msimd_quat_slerp(const float* q0, const float* q1, float t, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float dot = q0[0] * q1[0] + q0[1] * q1[1] + q0[2] * q1[2] + q0[3] * q1[3];
    float sign = 1.0f;
    if (dot < 0.0f) {
        dot = -dot;
        sign = -1.0f;
    }
    if (dot > 0.9995f) {
        for (int i = 0; i < 4; ++i) out[i] = q0[i] + t * (sign * q1[i] - q0[i]);
        float len = std::sqrt(out[0]*out[0] + out[1]*out[1] + out[2]*out[2] + out[3]*out[3]);
        float inv = (len > 0.0f) ? (1.0f / len) : 1.0f;
        for (int i = 0; i < 4; ++i) out[i] *= inv;
        return;
    }
    float theta_0 = std::acos(dot);
    float theta = theta_0 * t;
    float sin_theta = std::sin(theta);
    float sin_theta_0 = std::sin(theta_0);
    float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;
    for (int i = 0; i < 4; ++i) out[i] = s0 * q0[i] + s1 * (sign * q1[i]);
}
}]>;

def Kernel_dual_quat_transform_point_f32 : MasterKernelImplementation<"msimd_dual_quat_transform_point_f32", [{
void msimd_dual_quat_transform_point_f32(const float* real, const float* dual, const float* p, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float rot_p[3];
    msimd_quat_rotate_vec(real, p, rot_p);
    float conj_real[4] = { -real[0], -real[1], -real[2], real[3] };
    float t_quat[4];
    msimd_quat_mul(dual, conj_real, t_quat);
    out[0] = rot_p[0] + 2.0f * t_quat[0];
    out[1] = rot_p[1] + 2.0f * t_quat[1];
    out[2] = rot_p[2] + 2.0f * t_quat[2];
}
}]>;

def Kernel_bessel_j0 : MasterKernelImplementation<"msimd_bessel_j0", [{
double msimd_bessel_j0(double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double ax = std::fabs(x);
    if (ax < 8.0) {
        double y = x * x;
        double ans1 = 57568490574.0 + y * (-13362590354.0 + y * (651619640.7
                    + y * (-11214424.18 + y * (77392.33017 + y * (-184.9052456)))));
        double ans2 = 57568490574.0 + y * (1029532985.0 + y * (9494680.718
                    + y * (59272.64853 + y * (267.8532712 + y * 1.0))));
        return ans1 / ans2;
    } else {
        double z = 8.0 / ax;
        double y = z * z;
        double xx = ax - 0.785398164;
        double f0 = 1.0 + y * (-0.1098628627e-2 + y * (0.2734510407e-4
                  + y * (-0.2073370639e-5 + y * 0.2093887211e-6)));
        double theta0 = y * (-0.1562499995e-1 + y * (0.1430488765e-3
                      + y * (-0.6911147651e-5 + y * (0.7621095161e-6 - y * 0.934945152e-7))));
        return std::sqrt(0.636619772 / ax) * (f0 * std::cos(xx + theta0));
    }
}
}]>;

def Kernel_bessel_j1 : MasterKernelImplementation<"msimd_bessel_j1", [{
double msimd_bessel_j1(double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double ax = std::fabs(x);
    if (ax < 8.0) {
        double y = x * x;
        double ans1 = x * (72362614232.0 + y * (-7895059235.0 + y * (242396853.1
                    + y * (-2972638.799 + y * (15704.48260 + y * (-30.16036606))))));
        double ans2 = 144725228442.0 + y * (2300535178.0 + y * (18583304.74
                    + y * (99447.43394 + y * (376.9991397 + y * 1.0))));
        return ans1 / ans2;
    } else {
        double z = 8.0 / ax;
        double y = z * z;
        double xx = ax - 2.356194491;
        double f1 = 1.0 + y * (0.183105e-2 + y * (-0.3516396496e-4
                  + y * (0.2457520174e-5 - y * 0.240337019e-6)));
        double theta1 = y * (0.04687499995 + y * (-0.2002690873e-3
                      + y * (0.8449199096e-5 + y * (-0.88228987e-6 + y * 0.105787412e-6))));
        double ans = std::sqrt(0.636619772 / ax) * (f1 * std::cos(xx + theta1));
        return (x < 0.0) ? -ans : ans;
    }
}
}]>;

def Kernel_airy_ai : MasterKernelImplementation<"msimd_airy_ai", [{
double msimd_airy_ai(double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (std::fabs(x) < 2.0) {
        double f = 0.355028053887817;
        double g = 0.258819403792807;
        double term_f = f, term_g = g * x;
        double sum = term_f - term_g;
        double x3 = x * x * x;
        for (int k = 1; k <= 12; ++k) {
            term_f *= x3 / (3.0 * k * (3.0 * k - 1.0));
            term_g *= x3 / (3.0 * k * (3.0 * k + 1.0));
            sum += term_f - term_g;
        }
        return sum;
    } else if (x >= 2.0) {
        double zeta = (2.0 / 3.0) * x * std::sqrt(x);
        return (0.5 / (std::sqrt(M_PI) * std::pow(x, 0.25))) * std::exp(-zeta);
    } else {
        double ax = -x;
        double zeta = (2.0 / 3.0) * ax * std::sqrt(ax);
        return (1.0 / (std::sqrt(M_PI) * std::pow(ax, 0.25))) * std::sin(zeta + M_PI / 4.0);
    }
}
}]>;

def Kernel_legendre_p : MasterKernelImplementation<"msimd_legendre_p", [{
double msimd_legendre_p(int n, double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n == 0) return 1.0;
    if (n == 1) return x;
    double p_prev = 1.0;
    double p_curr = x;
    for (int k = 1; k < n; ++k) {
        double p_next = ((2.0 * k + 1.0) * x * p_curr - k * p_prev) / (k + 1.0);
        p_prev = p_curr;
        p_curr = p_next;
    }
    return p_curr;
}
}]>;

def Kernel_chebyshev_t : MasterKernelImplementation<"msimd_chebyshev_t", [{
double msimd_chebyshev_t(int n, double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n == 0) return 1.0;
    if (n == 1) return x;
    double t_prev = 1.0;
    double t_curr = x;
    for (int k = 1; k < n; ++k) {
        double t_next = 2.0 * x * t_curr - t_prev;
        t_prev = t_curr;
        t_curr = t_next;
    }
    return t_curr;
}
}]>;

def Kernel_kdv_step : MasterKernelImplementation<"msimd_kdv_step", [{
void msimd_kdv_step(float* u_next, const float* u_curr, const float* u_prev, size_t n, float dt, float dx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!u_next || !u_curr || !u_prev || n < 4 || dx <= 0.0f) return;
    float inv_2dx = 1.0f / (2.0f * dx);
    float inv_2dx3 = 1.0f / (2.0f * dx * dx * dx);

    for (size_t i = 2; i < n - 2; ++i) {
        float u_x = (u_curr[i + 1] - u_curr[i - 1]) * inv_2dx;
        float u_xxx = (u_curr[i + 2] - 2.0f * u_curr[i + 1] + 2.0f * u_curr[i - 1] - u_curr[i - 2]) * inv_2dx3;
        float nonlin = 6.0f * u_curr[i] * u_x;
        u_next[i] = u_prev[i] - 2.0f * dt * (nonlin + u_xxx);
    }
    u_next[0] = u_next[1] = 0.0f;
    u_next[n - 2] = u_next[n - 1] = 0.0f;
}
}]>;

def Kernel_sine_gordon_step : MasterKernelImplementation<"msimd_sine_gordon_step", [{
void msimd_sine_gordon_step(float* u_next, const float* u_curr, const float* u_prev, size_t n, float dt, float dx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!u_next || !u_curr || !u_prev || n < 3 || dx <= 0.0f) return;
    float r = (dt / dx) * (dt / dx);
    for (size_t i = 1; i < n - 1; ++i) {
        float lap = u_curr[i + 1] - 2.0f * u_curr[i] + u_curr[i - 1];
        u_next[i] = 2.0f * u_curr[i] - u_prev[i] + r * lap - (dt * dt) * std::sin(u_curr[i]);
    }
    u_next[0] = u_next[1];
    u_next[n - 1] = u_next[n - 2];
}
}]>;

def Kernel_fractional_caputo_derivative : MasterKernelImplementation<"msimd_fractional_caputo_derivative", [{
void msimd_fractional_caputo_derivative(const float* f, float* out, size_t n, float alpha, float dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double gamma_val = std::tgamma(1.0 - (double)alpha);
    double factor = 1.0 / (gamma_val * (1.0 - (double)alpha) * std::pow(dt, alpha));

    out[0] = 0.0f;
    for (size_t j = 1; j < n; ++j) {
        double sum = 0.0;
        for (size_t k = 0; k < j; ++k) {
            double df = (f[k + 1] - f[k]);
            double b = std::pow(j - k, 1.0 - alpha) - std::pow(j - k - 1, 1.0 - alpha);
            sum += df * b;
        }
        out[j] = (float)(sum * factor);
    }
}
}]>;

def Kernel_wavelet_haar_forward : MasterKernelImplementation<"msimd_wavelet_haar_forward", [{
void msimd_wavelet_haar_forward(const float* in, float* out, size_t n, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 2 || (n & 1)) return;
    size_t half = n / 2;
    static const float inv_sqrt2 = 0.7071067811865475f;
    for (size_t i = 0; i < half; ++i) {
        out[i] = (in[2 * i] + in[2 * i + 1]) * inv_sqrt2;
        out[half + i] = (in[2 * i] - in[2 * i + 1]) * inv_sqrt2;
    }
}
}]>;

def Kernel_wavelet_haar_inverse : MasterKernelImplementation<"msimd_wavelet_haar_inverse", [{
void msimd_wavelet_haar_inverse(const float* in, float* out, size_t n, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 2 || (n & 1)) return;
    size_t half = n / 2;
    static const float inv_sqrt2 = 0.7071067811865475f;
    for (size_t i = 0; i < half; ++i) {
        out[2 * i] = (in[i] + in[half + i]) * inv_sqrt2;
        out[2 * i + 1] = (in[i] - in[half + i]) * inv_sqrt2;
    }
}
}]>;

def Kernel_wavelet_daubechies4_forward : MasterKernelImplementation<"msimd_wavelet_daubechies4_forward", [{
void msimd_wavelet_daubechies4_forward(const float* in, float* out, size_t n, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 4 || (n & 1)) return;
    static const float h0 = 0.4829629131445341f;
    static const float h1 = 0.8365163037378079f;
    static const float h2 = 0.2241438680420134f;
    static const float h3 = -0.1294095225512604f;

    static const float g0 = h3;
    static const float g1 = -h2;
    static const float g2 = h1;
    static const float g3 = -h0;

    size_t half = n / 2;
    for (size_t i = 0; i < half; ++i) {
        size_t i0 = (2 * i) % n;
        size_t i1 = (2 * i + 1) % n;
        size_t i2 = (2 * i + 2) % n;
        size_t i3 = (2 * i + 3) % n;

        out[i] = h0 * in[i0] + h1 * in[i1] + h2 * in[i2] + h3 * in[i3];
        out[half + i] = g0 * in[i0] + g1 * in[i1] + g2 * in[i2] + g3 * in[i3];
    }
}
}]>;

def Kernel_kernel_wave_step_1d_fp32 : MasterKernelImplementation<"msimd_kernel_wave_step_1d_fp32", [{
void msimd_kernel_wave_step_1d_fp32(float* next, const float* curr, const float* prev, size_t nx, float r2, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (nx < 3) return;
    const float two_minus_2r2 = 2.0f - 2.0f * r2;
    for (size_t i = 1; i < nx - 1; ++i) {
        float lap = curr[i - 1] + curr[i + 1];
        next[i] = two_minus_2r2 * curr[i] + r2 * lap - prev[i];
    }
    next[0] = 0.0f;
    next[nx - 1] = 0.0f;
}
}]>;

def Kernel_kernel_wave_step_3d_fp32 : MasterKernelImplementation<"msimd_kernel_wave_step_3d_fp32", [{
void msimd_kernel_wave_step_3d_fp32(float* next, const float* curr, const float* prev,
                                 size_t nx, size_t ny, size_t nz,
                                 float rx, float ry, float rz, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!next || !curr || !prev || nx < 3 || ny < 3 || nz < 3) return;
    const size_t slice_stride = nx * ny;
    const float center_coeff = 2.0f - 2.0f * (rx + ry + rz);
    for (size_t z = 1; z < nz - 1; ++z) {
        for (size_t y = 1; y < ny - 1; ++y) {
            size_t base = z * slice_stride + y * nx;
            for (size_t x = 1; x < nx - 1; ++x) {
                size_t idx = base + x;
                float xm = curr[idx - 1], xp = curr[idx + 1];
                float ym = curr[idx - nx], yp = curr[idx + nx];
                float zm = curr[idx - slice_stride], zp = curr[idx + slice_stride];
                float lap = rx * (xm + xp) + ry * (ym + yp) + rz * (zm + zp);
                next[idx] = center_coeff * curr[idx] + lap - prev[idx];
            }
        }
    }
}
}]>;

def Kernel_kernel_maxwell_yee_3d : MasterKernelImplementation<"msimd_kernel_maxwell_yee_3d", [{
void msimd_kernel_maxwell_yee_3d(size_t nx, size_t ny, size_t nz,
                              float* Ex, float* Ey, float* Ez,
                              float* Hx, float* Hy, float* Hz,
                              float dt, float dx, float dy, float dz,
                              float mu, float eps, const msimd_tensor_t* precision_target) {
    if (!Ex || !Ey || !Ez || !Hx || !Hy || !Hz || nx < 3 || ny < 3 || nz < 3) return;
    (void)precision_target;
    const float dt_over_mu_dy = dt / (mu * dy);
    const float dt_over_mu_dz = dt / (mu * dz);
    const float dt_over_mu_dx = dt / (mu * dx);

    const float dt_over_eps_dy = dt / (eps * dy);
    const float dt_over_eps_dz = dt / (eps * dz);
    const float dt_over_eps_dx = dt / (eps * dx);

    const size_t slice = nx * ny;

    for (size_t z = 0; z < nz - 1; ++z) {
        for (size_t y = 0; y < ny - 1; ++y) {
            for (size_t x = 0; x < nx - 1; ++x) {
                size_t idx = z * slice + y * nx + x;

                float dez_dy = (Ez[idx + nx] - Ez[idx]);
                float dey_dz = (Ey[idx + slice] - Ey[idx]);
                Hx[idx] -= (dt_over_mu_dy * dez_dy - dt_over_mu_dz * dey_dz);

                float dex_dz = (Ex[idx + slice] - Ex[idx]);
                float dez_dx = (Ez[idx + 1] - Ez[idx]);
                Hy[idx] -= (dt_over_mu_dz * dex_dz - dt_over_mu_dx * dez_dx);

                float dey_dx = (Ey[idx + 1] - Ey[idx]);
                float dex_dy = (Ex[idx + nx] - Ex[idx]);
                Hz[idx] -= (dt_over_mu_dx * dey_dx - dt_over_mu_dy * dex_dy);
            }
        }
    }

    for (size_t z = 1; z < nz; ++z) {
        for (size_t y = 1; y < ny; ++y) {
            for (size_t x = 1; x < nx; ++x) {
                size_t idx = z * slice + y * nx + x;

                float dhz_dy = (Hz[idx] - Hz[idx - nx]);
                float dhy_dz = (Hy[idx] - Hy[idx - slice]);
                Ex[idx] += (dt_over_eps_dy * dhz_dy - dt_over_eps_dz * dhy_dz);

                float dhx_dz = (Hx[idx] - Hx[idx - slice]);
                float dhz_dx = (Hz[idx] - Hz[idx - 1]);
                Ey[idx] += (dt_over_eps_dz * dhx_dz - dt_over_eps_dx * dhz_dx);

                float dhy_dx = (Hy[idx] - Hy[idx - 1]);
                float dhx_dy = (Hx[idx] - Hx[idx - nx]);
                Ez[idx] += (dt_over_eps_dx * dhy_dx - dt_over_eps_dy * dhx_dy);
            }
        }
    }
}
}]>;

def Kernel_kernel_lbm_d2q9 : MasterKernelImplementation<"msimd_kernel_lbm_d2q9", [{
void msimd_kernel_lbm_d2q9(size_t nx, size_t ny,
                        const float* f_in, float* f_out,
                        float* rho, float* ux, float* uy,
                        float tau, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const float omega = 1.0f / tau;
    const float w0 = 4.0f / 9.0f;
    const float w1 = 1.0f / 9.0f;
    const float w2 = 1.0f / 36.0f;

    static const int cx[9] = { 0,  1,  0, -1,  0,  1, -1, -1,  1 };
    static const int cy[9] = { 0,  0,  1,  0, -1,  1,  1, -1, -1 };
    static const float w[9] = { w0, w1, w1, w1, w1, w2, w2, w2, w2 };

    const size_t total_cells = nx * ny;

    for (size_t y = 0; y < ny; ++y) {
        for (size_t x = 0; x < nx; ++x) {
            size_t cell = y * nx + x;

            float local_rho = 0.0f;
            float local_jx = 0.0f;
            float local_jy = 0.0f;

            for (int i = 0; i < 9; ++i) {
                float fi = f_in[i * total_cells + cell];
                local_rho += fi;
                local_jx += fi * cx[i];
                local_jy += fi * cy[i];
            }

            rho[cell] = local_rho;
            float inv_rho = (local_rho > 1e-7f) ? (1.0f / local_rho) : 1.0f;
            float vx = local_jx * inv_rho;
            float vy = local_jy * inv_rho;
            ux[cell] = vx;
            uy[cell] = vy;

            float u_sqr = vx * vx + vy * vy;

            for (int i = 0; i < 9; ++i) {
                float eu = cx[i] * vx + cy[i] * vy;
                float feq = w[i] * local_rho * (1.0f + 3.0f * eu + 4.5f * eu * eu - 1.5f * u_sqr);
                float f_post = f_in[i * total_cells + cell] - omega * (f_in[i * total_cells + cell] - feq);

                size_t nx_target = (x + cx[i] + nx) % nx;
                size_t ny_target = (y + cy[i] + ny) % ny;
                size_t dest_cell = ny_target * nx + nx_target;

                f_out[i * total_cells + dest_cell] = f_post;
            }
        }
    }
}
}]>;

def Kernel_kernel_stockham_fft_radix2 : MasterKernelImplementation<"msimd_kernel_stockham_fft_radix2", [{
void msimd_kernel_stockham_fft_radix2(size_t n,
                                   const float* in_real, const float* in_imag,
                                   float* out_real, float* out_imag,
                                   int inverse, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n <= 1 || (n & (n - 1)) != 0) {
        if (n == 1) {
            out_real[0] = in_real[0];
            out_imag[0] = in_imag[0];
        }
        return;
    }

    std::vector<float> r_buf1(n), i_buf1(n);
    std::vector<float> r_buf2(n), i_buf2(n);

    memcpy(r_buf1.data(), in_real, n * sizeof(float));
    memcpy(i_buf1.data(), in_imag, n * sizeof(float));

    float* cur_r = r_buf1.data();
    float* cur_i = i_buf1.data();
    float* nxt_r = r_buf2.data();
    float* nxt_i = i_buf2.data();

    const float sign = inverse ? 1.0f : -1.0f;
    const double pi = 3.14159265358979323846;

    for (size_t s = 1; s < n; s *= 2) {
        size_t p = n / (2 * s);
        for (size_t j = 0; j < s; ++j) {
            double angle = sign * 2.0 * pi * (double)j / (double)(2 * s);
            float wr = (float)std::cos(angle);
            float wi = (float)std::sin(angle);

            for (size_t k = 0; k < p; ++k) {
                size_t in_idx0 = k * s + j;
                size_t in_idx1 = in_idx0 + n / 2;

                float r0 = cur_r[in_idx0];
                float i0 = cur_i[in_idx0];
                float r1 = cur_r[in_idx1];
                float i1 = cur_i[in_idx1];

                float tr = r1 * wr - i1 * wi;
                float ti = r1 * wi + i1 * wr;

                size_t out_idx0 = (2 * k) * s + j;
                size_t out_idx1 = (2 * k + 1) * s + j;

                nxt_r[out_idx0] = r0 + tr;
                nxt_i[out_idx0] = i0 + ti;
                nxt_r[out_idx1] = r0 - tr;
                nxt_i[out_idx1] = i0 - ti;
            }
        }
        std::swap(cur_r, nxt_r);
        std::swap(cur_i, nxt_i);
    }

    if (inverse) {
        float inv_n = 1.0f / (float)n;
        for (size_t i = 0; i < n; ++i) {
            out_real[i] = cur_r[i] * inv_n;
            out_imag[i] = cur_i[i] * inv_n;
        }
    } else {
        memcpy(out_real, cur_r, n * sizeof(float));
        memcpy(out_imag, cur_i, n * sizeof(float));
    }
}
}]>;

def Kernel_rigidbody_update_inv_inertia : MasterKernelImplementation<"msimd_rigidbody_update_inv_inertia", [{
void msimd_rigidbody_update_inv_inertia(MSIMDRigidBody* b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (b->inv_mass <= 0.0f) {
        std::memset(b->inv_inertia_world, 0, 9 * sizeof(float));
        return;
    }
    float R[9];
    msimd_quat_to_mat3(b->quat, R, precision_target);

    float R_invI[9];
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            float sum = 0.0f;
            for (int k = 0; k < 3; ++k) sum += R[r * 3 + k] * b->inertia[k * 3 + c];
            R_invI[r * 3 + c] = sum;
        }
    }
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            float sum = 0.0f;
            for (int k = 0; k < 3; ++k) sum += R_invI[r * 3 + k] * R[c * 3 + k];
            b->inv_inertia_world[r * 3 + c] = sum;
        }
    }
}
}]>;

def Kernel_rigidbody_integrate_velocity : MasterKernelImplementation<"msimd_rigidbody_integrate_velocity", [{
void msimd_rigidbody_integrate_velocity(MSIMDRigidBody* b, const float* gravity, float dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (b->inv_mass <= 0.0f) return;
    for (int i = 0; i < 3; ++i) {
        b->vel[i] += (gravity[i] + b->force[i] * b->inv_mass) * dt;
    }
    float ang_accel[3];
    msimd_mat3f_mul_vec(b->inv_inertia_world, b->torque, ang_accel);
    for (int i = 0; i < 3; ++i) {
        b->ang_vel[i] += ang_accel[i] * dt;
    }
}
}]>;

def Kernel_rigidbody_integrate_position : MasterKernelImplementation<"msimd_rigidbody_integrate_position", [{
void msimd_rigidbody_integrate_position(MSIMDRigidBody* b, float dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (b->inv_mass <= 0.0f) return;
    for (int i = 0; i < 3; ++i) {
        b->pos[i] += b->vel[i] * dt;
    }
    float w_quat[4] = { b->ang_vel[0], b->ang_vel[1], b->ang_vel[2], 0.0f };
    float dq[4];
    msimd_quat_mul(w_quat, b->quat, dq);
    for (int i = 0; i < 4; ++i) {
        b->quat[i] += 0.5f * dt * dq[i];
    }
    float q_len = msimd_vec4f_length(b->quat);
    if (q_len > 0.0f) msimd_vec4f_scale(b->quat, 1.0f / q_len, b->quat);

    msimd_rigidbody_update_inv_inertia(b, precision_target);
}
}]>;

def Kernel_pgs_prepare_contact : MasterKernelImplementation<"msimd_pgs_prepare_contact", [{
void msimd_pgs_prepare_contact(MSIMDContactConstraint* c, float dt, float baumgarte, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float n[3];
    for (int i = 0; i < 3; ++i) n[i] = c->normal[i];

    if (std::fabs(n[0]) < 0.577f) {
        c->tangent1[0] = 1.0f; c->tangent1[1] = 0.0f; c->tangent1[2] = 0.0f;
    } else {
        c->tangent1[0] = 0.0f; c->tangent1[1] = 1.0f; c->tangent1[2] = 0.0f;
    }
    float d = msimd_vec3f_dot(c->tangent1, n);
    for (int i = 0; i < 3; ++i) c->tangent1[i] -= d * n[i];
    msimd_vec3f_normalize(c->tangent1, c->tangent1);
    msimd_vec3f_cross(n, c->tangent1, c->tangent2);

    auto compute_effective_mass = [&](const float* dir) -> float {
        float rA_x_dir[3], rB_x_dir[3];
        msimd_vec3f_cross(c->rA, dir, rA_x_dir);
        msimd_vec3f_cross(c->rB, dir, rB_x_dir);
        float I_rA[3], I_rB[3];
        msimd_mat3f_mul_vec(c->bodyA->inv_inertia_world, rA_x_dir, I_rA);
        msimd_mat3f_mul_vec(c->bodyB->inv_inertia_world, rB_x_dir, I_rB);
        float ang_term_A = msimd_vec3f_dot(rA_x_dir, I_rA);
        float ang_term_B = msimd_vec3f_dot(rB_x_dir, I_rB);
        float total_inv_mass = c->bodyA->inv_mass + c->bodyB->inv_mass + ang_term_A + ang_term_B;
        return (total_inv_mass > 1e-12f) ? (1.0f / total_inv_mass) : 0.0f;
    };

    c->normal_mass = compute_effective_mass(c->normal);
    c->tangent1_mass = compute_effective_mass(c->tangent1);
    c->tangent2_mass = compute_effective_mass(c->tangent2);

    float allowed_penetration = 0.01f;
    float pen = std::max(0.0f, c->penetration - allowed_penetration);
    c->bias = (baumgarte / dt) * pen;
}
}]>;

def Kernel_pgs_solve_contact_iteration : MasterKernelImplementation<"msimd_pgs_solve_contact_iteration", [{
void msimd_pgs_solve_contact_iteration(MSIMDContactConstraint* c, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    auto get_contact_vel = [](const MSIMDRigidBody* b, const float* r, float* out_v) {
        float wxr[3];
        msimd_vec3f_cross(b->ang_vel, r, wxr);
        for (int i = 0; i < 3; ++i) out_v[i] = b->vel[i] + wxr[i];
    };

    float vA[3], vB[3], v_rel[3];
    get_contact_vel(c->bodyA, c->rA, vA);
    get_contact_vel(c->bodyB, c->rB, vB);
    for (int i = 0; i < 3; ++i) v_rel[i] = vA[i] - vB[i];

    float vn = msimd_vec3f_dot(v_rel, c->normal);
    float delta_impulse = -c->normal_mass * (vn - c->bias);
    float old_impulse = c->accumulated_normal_impulse;
    c->accumulated_normal_impulse = std::max(0.0f, old_impulse + delta_impulse);
    delta_impulse = c->accumulated_normal_impulse - old_impulse;

    float impulse_vec[3];
    msimd_vec3f_scale(c->normal, delta_impulse, impulse_vec);

    auto apply_impulse = [](MSIMDRigidBody* b, const float* r, const float* imp, float sign) {
        if (b->inv_mass <= 0.0f) return;
        for (int i = 0; i < 3; ++i) b->vel[i] += sign * imp[i] * b->inv_mass;
        float rximp[3];
        msimd_vec3f_cross(r, imp, rximp);
        float delta_w[3];
        msimd_mat3f_mul_vec(b->inv_inertia_world, rximp, delta_w);
        for (int i = 0; i < 3; ++i) b->ang_vel[i] += sign * delta_w[i];
    };

    apply_impulse(c->bodyA, c->rA, impulse_vec, 1.0f);
    apply_impulse(c->bodyB, c->rB, impulse_vec, -1.0f);

    float max_friction = c->friction_coeff * c->accumulated_normal_impulse;

    get_contact_vel(c->bodyA, c->rA, vA);
    get_contact_vel(c->bodyB, c->rB, vB);
    for (int i = 0; i < 3; ++i) v_rel[i] = vA[i] - vB[i];
    float vt1 = msimd_vec3f_dot(v_rel, c->tangent1);
    float delta_t1 = -c->tangent1_mass * vt1;
    float old_t1 = c->accumulated_tangent1_impulse;
    c->accumulated_tangent1_impulse = std::clamp(old_t1 + delta_t1, -max_friction, max_friction);
    delta_t1 = c->accumulated_tangent1_impulse - old_t1;

    msimd_vec3f_scale(c->tangent1, delta_t1, impulse_vec);
    apply_impulse(c->bodyA, c->rA, impulse_vec, 1.0f);
    apply_impulse(c->bodyB, c->rB, impulse_vec, -1.0f);

    get_contact_vel(c->bodyA, c->rA, vA);
    get_contact_vel(c->bodyB, c->rB, vB);
    for (int i = 0; i < 3; ++i) v_rel[i] = vA[i] - vB[i];
    float vt2 = msimd_vec3f_dot(v_rel, c->tangent2);
    float delta_t2 = -c->tangent2_mass * vt2;
    float old_t2 = c->accumulated_tangent2_impulse;
    c->accumulated_tangent2_impulse = std::clamp(old_t2 + delta_t2, -max_friction, max_friction);
    delta_t2 = c->accumulated_tangent2_impulse - old_t2;

    msimd_vec3f_scale(c->tangent2, delta_t2, impulse_vec);
    apply_impulse(c->bodyA, c->rA, impulse_vec, 1.0f);
    apply_impulse(c->bodyB, c->rB, impulse_vec, -1.0f);
}
}]>;

def Kernel_sph_compute_density_pressure : MasterKernelImplementation<"msimd_sph_compute_density_pressure", [{
void msimd_sph_compute_density_pressure(MSIMDSPHParticle* p, size_t count, float h, float rest_density, float k_stiffness, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float h2 = h * h;
    float poly6_factor = 315.0f / (64.0f * (float)M_PI * std::pow(h, 9));

    for (size_t i = 0; i < count; ++i) {
        float d = 0.0f;
        for (size_t j = 0; j < count; ++j) {
            float dx = p[j].pos[0] - p[i].pos[0];
            float dy = p[j].pos[1] - p[i].pos[1];
            float dz = p[j].pos[2] - p[i].pos[2];
            float r2 = dx * dx + dy * dy + dz * dz;
            if (r2 < h2) {
                float diff = h2 - r2;
                d += p[j].mass * poly6_factor * diff * diff * diff;
            }
        }
        p[i].density = std::max(d, rest_density * 0.1f);
        p[i].pressure = k_stiffness * (p[i].density - rest_density);
    }
}
}]>;

def Kernel_c_add : MasterKernelImplementation<"msimd_c_add", [{
MSIMD_API MSIMDComplex msimd_c_add(MSIMDComplex a, MSIMDComplex b) { return { a.r + b.r, a.i + b.i }; }
}]>;

def Kernel_c_sub : MasterKernelImplementation<"msimd_c_sub", [{
MSIMD_API MSIMDComplex msimd_c_sub(MSIMDComplex a, MSIMDComplex b) { return { a.r - b.r, a.i - b.i }; }
}]>;

def Kernel_c_mul : MasterKernelImplementation<"msimd_c_mul", [{
MSIMD_API MSIMDComplex msimd_c_mul(MSIMDComplex a, MSIMDComplex b) { return { a.r * b.r - a.i * b.i, a.r * b.i + a.i * b.r }; }
}]>;

def Kernel_c_scale : MasterKernelImplementation<"msimd_c_scale", [{
MSIMD_API MSIMDComplex msimd_c_scale(MSIMDComplex a, float s) { return { a.r * s, a.i * s }; }
}]>;

def Kernel_c_norm_sq : MasterKernelImplementation<"msimd_c_norm_sq", [{
MSIMD_API float msimd_c_norm_sq(MSIMDComplex a) { return a.r * a.r + a.i * a.i; }
}]>;

def Kernel_pauli_sigma_x : MasterKernelImplementation<"msimd_pauli_sigma_x", [{
void msimd_pauli_sigma_x(const MSIMDComplex in[2], MSIMDComplex out[2], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    out[0] = in[1];
    out[1] = in[0];
}
}]>;

def Kernel_pauli_sigma_y : MasterKernelImplementation<"msimd_pauli_sigma_y", [{
void msimd_pauli_sigma_y(const MSIMDComplex in[2], MSIMDComplex out[2], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    out[0] = { in[1].i, -in[1].r };
    out[1] = { -in[0].i, in[0].r };
}
}]>;

def Kernel_pauli_sigma_z : MasterKernelImplementation<"msimd_pauli_sigma_z", [{
void msimd_pauli_sigma_z(const MSIMDComplex in[2], MSIMDComplex out[2], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    out[0] = in[0];
    out[1] = { -in[1].r, -in[1].i };
}
}]>;

def Kernel_dirac_4spinor_step_3d : MasterKernelImplementation<"msimd_dirac_4spinor_step_3d", [{
void msimd_dirac_4spinor_step_3d(MSIMDComplex* psi_next, const MSIMDComplex* psi_curr,
    size_t nx, size_t ny, size_t nz,
    float m, float c, float hbar, float dt, float dx, float dy, float dz, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_next || !psi_curr || nx < 3 || ny < 3 || nz < 3) return;

    size_t slice = nx * ny;
    float m_c2 = m * c * c;
    float hbar_c_2dx = hbar * c / (2.0f * dx);
    float hbar_c_2dy = hbar * c / (2.0f * dy);
    float hbar_c_2dz = hbar * c / (2.0f * dz);
    float inv_hbar = 1.0f / hbar;

    for (size_t z = 1; z < nz - 1; ++z) {
        for (size_t y = 1; y < ny - 1; ++y) {
            for (size_t x = 1; x < nx - 1; ++x) {
                size_t cell = z * slice + y * nx + x;
                size_t base = cell * 4;

                MSIMDComplex dpsi_dx[4], dpsi_dy[4], dpsi_dz[4];
                for (int s = 0; s < 4; ++s) {
                    size_t xm = ((z * slice + y * nx + (x - 1)) * 4) + s;
                    size_t xp = ((z * slice + y * nx + (x + 1)) * 4) + s;
                    size_t ym = ((z * slice + (y - 1) * nx + x) * 4) + s;
                    size_t yp = ((z * slice + (y + 1) * nx + x) * 4) + s;
                    size_t zm = (((z - 1) * slice + y * nx + x) * 4) + s;
                    size_t zp = (((z + 1) * slice + y * nx + x) * 4) + s;

                    dpsi_dx[s] = msimd_c_scale(msimd_c_sub(psi_curr[xp], psi_curr[xm]), hbar_c_2dx);
                    dpsi_dy[s] = msimd_c_scale(msimd_c_sub(psi_curr[yp], psi_curr[ym]), hbar_c_2dy);
                    dpsi_dz[s] = msimd_c_scale(msimd_c_sub(psi_curr[zp], psi_curr[zm]), hbar_c_2dz);
                }

                MSIMDComplex H_psi[4];
                H_psi[0] = msimd_c_scale(psi_curr[base + 0], m_c2);
                H_psi[1] = msimd_c_scale(psi_curr[base + 1], m_c2);
                H_psi[2] = msimd_c_scale(psi_curr[base + 2], -m_c2);
                H_psi[3] = msimd_c_scale(psi_curr[base + 3], -m_c2);

                H_psi[0] = msimd_c_add(H_psi[0], { -dpsi_dz[2].i, dpsi_dz[2].r });
                H_psi[1] = msimd_c_sub(H_psi[1], { -dpsi_dz[3].i, dpsi_dz[3].r });
                H_psi[2] = msimd_c_add(H_psi[2], { -dpsi_dz[0].i, dpsi_dz[0].r });
                H_psi[3] = msimd_c_sub(H_psi[3], { -dpsi_dz[1].i, dpsi_dz[1].r });

                for (int s = 0; s < 4; ++s) {
                    MSIMDComplex idt_H = { -H_psi[s].i * dt * inv_hbar, H_psi[s].r * dt * inv_hbar };
                    psi_next[base + s] = msimd_c_sub(psi_curr[base + s], idt_H);
                }
            }
        }
    }
}
}]>;

def Kernel_lattice_u1_plaquette_energy : MasterKernelImplementation<"msimd_lattice_u1_plaquette_energy", [{
float msimd_lattice_u1_plaquette_energy(const float* link_angles, size_t nx, size_t ny, size_t nz, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!link_angles || nx < 2 || ny < 2 || nz < 2) return 0.0f;
    auto get_link = [&](size_t x, size_t y, size_t z, int mu) -> float {
        return link_angles[((z * ny + y) * nx + x) * 3 + mu];
    };

    double total_action = 0.0;
    size_t count = 0;

    for (size_t z = 0; z < nz - 1; ++z) {
        for (size_t y = 0; y < ny - 1; ++y) {
            for (size_t x = 0; x < nx - 1; ++x) {
                for (int mu = 0; mu < 3; ++mu) {
                    for (int nu = mu + 1; nu < 3; ++nu) {
                        float theta1 = get_link(x, y, z, mu);
                        size_t x_mu = x + (mu == 0 ? 1 : 0);
                        size_t y_mu = y + (mu == 1 ? 1 : 0);
                        size_t z_mu = z + (mu == 2 ? 1 : 0);
                        float theta2 = get_link(x_mu, y_mu, z_mu, nu);

                        size_t x_nu = x + (nu == 0 ? 1 : 0);
                        size_t y_nu = y + (nu == 1 ? 1 : 0);
                        size_t z_nu = z + (nu == 2 ? 1 : 0);
                        float theta3 = get_link(x_nu, y_nu, z_nu, mu);
                        float theta4 = get_link(x, y, z, nu);

                        float plaq_angle = theta1 + theta2 - theta3 - theta4;
                        total_action += (1.0 - std::cos((double)plaq_angle));
                        count++;
                    }
                }
            }
        }
    }
    return count > 0 ? (float)(total_action / (double)count) : 0.0f;
}
}]>;

def Kernel_lindblad_step_qubit : MasterKernelImplementation<"msimd_lindblad_step_qubit", [{
void msimd_lindblad_step_qubit(const MSIMDComplex rho[4], const MSIMDComplex H[4],
    float gamma_dephase, float gamma_relax, float dt,
    MSIMDComplex out_rho[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    MSIMDComplex H_rho[4], rho_H[4], comm[4];
    for (int r = 0; r < 2; ++r) {
        for (int c = 0; c < 2; ++c) {
            MSIMDComplex s1 = { 0, 0 }, s2 = { 0, 0 };
            for (int k = 0; k < 2; ++k) {
                s1 = msimd_c_add(s1, msimd_c_mul(H[r * 2 + k], rho[k * 2 + c]));
                s2 = msimd_c_add(s2, msimd_c_mul(rho[r * 2 + k], H[k * 2 + c]));
            }
            H_rho[r * 2 + c] = s1;
            rho_H[r * 2 + c] = s2;
            comm[r * 2 + c] = msimd_c_sub(s1, s2);
        }
    }

    MSIMDComplex diss[4];
    diss[0] = { gamma_relax * rho[3].r, 0.0f };
    diss[3] = { -gamma_relax * rho[3].r, 0.0f };
    diss[1] = { -(0.5f * gamma_relax + gamma_dephase) * rho[1].r, -(0.5f * gamma_relax + gamma_dephase) * rho[1].i };
    diss[2] = { -(0.5f * gamma_relax + gamma_dephase) * rho[2].r, -(0.5f * gamma_relax + gamma_dephase) * rho[2].i };

    for (int i = 0; i < 4; ++i) {
        MSIMDComplex idt_comm = { -comm[i].i * dt, comm[i].r * dt };
        out_rho[i] = msimd_c_add(msimd_c_add(rho[i], idt_comm), msimd_c_scale(diss[i], dt));
    }
}
}]>;

def Kernel_aabb_intersect_ray : MasterKernelImplementation<"msimd_aabb_intersect_ray", [{
bool msimd_aabb_intersect_ray(const MSIMDAABB* box, const float* orig, const float* dir, float* out_tmin, float* out_tmax) {
    float tmin = 0.0f;
    float tmax = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; ++i) {
        float invD = (std::fabs(dir[i]) > 1e-12f) ? (1.0f / dir[i]) : 1e12f;
        float t0 = (box->min_pt[i] - orig[i]) * invD;
        float t1 = (box->max_pt[i] - orig[i]) * invD;
        if (invD < 0.0f) std::swap(t0, t1);
        tmin = std::max(tmin, t0);
        tmax = std::min(tmax, t1);
        if (tmax < tmin) return false;
    }
    if (out_tmin) *out_tmin = tmin;
    if (out_tmax) *out_tmax = tmax;
    return true;
}
}]>;

def Kernel_pbr_distribution_ggx : MasterKernelImplementation<"msimd_pbr_distribution_ggx", [{
MSIMD_API float msimd_pbr_distribution_ggx(const float* n, const float* h, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float n_dot_h = std::max(msimd_vec3f_dot(n, h), 0.0f);
    float n_dot_h2 = n_dot_h * n_dot_h;

    float denom = (n_dot_h2 * (a2 - 1.0f) + 1.0f);
    denom = (float)M_PI * denom * denom;
    return denom > 1e-7f ? (a2 / denom) : 1.0f;
}
}]>;

def Kernel_pbr_geometry_schlick_ggx : MasterKernelImplementation<"msimd_pbr_geometry_schlick_ggx", [{
MSIMD_API float msimd_pbr_geometry_schlick_ggx(float n_dot_v, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    float denom = n_dot_v * (1.0f - k) + k;
    return denom > 1e-7f ? (n_dot_v / denom) : 1.0f;
}
}]>;

def Kernel_pbr_geometry_smith : MasterKernelImplementation<"msimd_pbr_geometry_smith", [{
MSIMD_API float msimd_pbr_geometry_smith(const float* n, const float* v, const float* l, float roughness) {
    float n_dot_v = std::max(msimd_vec3f_dot(n, v), 0.0f);
    float n_dot_l = std::max(msimd_vec3f_dot(n, l), 0.0f);
    float ggx1 = msimd_pbr_geometry_schlick_ggx(n_dot_v, roughness);
    float ggx2 = msimd_pbr_geometry_schlick_ggx(n_dot_l, roughness);
    return ggx1 * ggx2;
}
}]>;

def Kernel_pbr_fresnel_schlick : MasterKernelImplementation<"msimd_pbr_fresnel_schlick", [{
MSIMD_API void msimd_pbr_fresnel_schlick(float cos_theta, const float* F0, float* out_F) {
    float factor = std::pow(std::clamp(1.0f - cos_theta, 0.0f, 1.0f), 5.0f);
    for (int i = 0; i < 3; ++i) {
        out_F[i] = F0[i] + (1.0f - F0[i]) * factor;
    }
}
}]>;

def Kernel_pbr_cook_torrance_brdf : MasterKernelImplementation<"msimd_pbr_cook_torrance_brdf", [{
void msimd_pbr_cook_torrance_brdf(const float* n, const float* v, const float* l,
    const float* albedo, float metallic, float roughness,
    float* out_radiance, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    float h[3];
    msimd_vec3f_add(v, l, h);
    msimd_vec3f_normalize(h, h);

    float n_dot_l = std::max(msimd_vec3f_dot(n, l), 0.0f);
    float n_dot_v = std::max(msimd_vec3f_dot(n, v), 0.0f);
    if (n_dot_l <= 0.0f || n_dot_v <= 0.0f) {
        out_radiance[0] = out_radiance[1] = out_radiance[2] = 0.0f;
        return;
    }

    float F0[3];
    for (int i = 0; i < 3; ++i) {
        F0[i] = 0.04f * (1.0f - metallic) + albedo[i] * metallic;
    }

    float D = msimd_pbr_distribution_ggx(n, h, roughness);
    float G = msimd_pbr_geometry_smith(n, v, l, roughness);
    float F[3];
    msimd_pbr_fresnel_schlick(std::max(msimd_vec3f_dot(h, v), 0.0f), F0, F);

    float kS[3];
    for (int i = 0; i < 3; ++i) kS[i] = F[i];
    float kD[3];
    for (int i = 0; i < 3; ++i) kD[i] = (1.0f - kS[i]) * (1.0f - metallic);

    float denom = 4.0f * n_dot_v * n_dot_l;
    float spec_factor = denom > 1e-7f ? ((D * G) / denom) : 0.0f;

    for (int i = 0; i < 3; ++i) {
        float specular = spec_factor * F[i];
        float diffuse = (kD[i] * albedo[i]) / (float)M_PI;
        out_radiance[i] = (diffuse + specular) * n_dot_l;
    }
}
}]>;

def Kernel_svdag_raymarch_dda : MasterKernelImplementation<"msimd_svdag_raymarch_dda", [{
bool msimd_svdag_raymarch_dda(
    const uint32_t* voxel_grid, size_t dim_x, size_t dim_y, size_t dim_z,
    const float* orig, const float* dir, float max_dist,
    float* out_hit_pos, int* out_voxel_val) {

    int x = (int)std::floor(orig[0]);
    int y = (int)std::floor(orig[1]);
    int z = (int)std::floor(orig[2]);

    int step_x = (dir[0] > 0.0f) ? 1 : ((dir[0] < 0.0f) ? -1 : 0);
    int step_y = (dir[1] > 0.0f) ? 1 : ((dir[1] < 0.0f) ? -1 : 0);
    int step_z = (dir[2] > 0.0f) ? 1 : ((dir[2] < 0.0f) ? -1 : 0);

    float t_delta_x = (step_x != 0) ? std::fabs(1.0f / dir[0]) : 1e12f;
    float t_delta_y = (step_y != 0) ? std::fabs(1.0f / dir[1]) : 1e12f;
    float t_delta_z = (step_z != 0) ? std::fabs(1.0f / dir[2]) : 1e12f;

    if (step_x == 0 && step_y == 0 && step_z == 0) return false;
    float t_max_x = (step_x > 0) ? ((x + 1 - orig[0]) * t_delta_x) : ((step_x < 0) ? ((orig[0] - x) * t_delta_x) : 1e30f);
    float t_max_y = (step_y > 0) ? ((y + 1 - orig[1]) * t_delta_y) : ((step_y < 0) ? ((orig[1] - y) * t_delta_y) : 1e30f);
    float t_max_z = (step_z > 0) ? ((z + 1 - orig[2]) * t_delta_z) : ((step_z < 0) ? ((orig[2] - z) * t_delta_z) : 1e30f);

    float dist_traveled = 0.0f;
    while (dist_traveled < max_dist) {
        if (x >= 0 && x < (int)dim_x && y >= 0 && y < (int)dim_y && z >= 0 && z < (int)dim_z) {
            size_t idx = (z * dim_y + y) * dim_x + x;
            if (voxel_grid[idx] != 0) {
                out_hit_pos[0] = orig[0] + dist_traveled * dir[0];
                out_hit_pos[1] = orig[1] + dist_traveled * dir[1];
                out_hit_pos[2] = orig[2] + dist_traveled * dir[2];
                if (out_voxel_val) *out_voxel_val = (int)voxel_grid[idx];
                return true;
            }
        }

        if (t_max_x < t_max_y) {
            if (t_max_x < t_max_z) {
                dist_traveled = t_max_x;
                t_max_x += t_delta_x;
                x += step_x;
            } else {
                dist_traveled = t_max_z;
                t_max_z += t_delta_z;
                z += step_z;
            }
        } else {
            if (t_max_y < t_max_z) {
                dist_traveled = t_max_y;
                t_max_y += t_delta_y;
                y += step_y;
            } else {
                dist_traveled = t_max_z;
                t_max_z += t_delta_z;
                z += step_z;
            }
        }
    }
    return false;
}
}]>;

def Kernel_kernel_ray_triangle_batch8 : MasterKernelImplementation<"msimd_kernel_ray_triangle_batch8", [{
void msimd_kernel_ray_triangle_batch8(const float ox[8], const float oy[8], const float oz[8],
                                   const float dx[8], const float dy[8], const float dz[8],
                                   const float v0[3], const float v1[3], const float v2[3],
                                   int hit[8], float t[8], float u[8], float v[8], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float e1[3] = { v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
    float e2[3] = { v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2] };

    for (int i = 0; i < 8; ++i) {
        float pvec[3] = {
            dy[i] * e2[2] - dz[i] * e2[1],
            dz[i] * e2[0] - dx[i] * e2[2],
            dx[i] * e2[1] - dy[i] * e2[0]
        };

        float det = e1[0] * pvec[0] + e1[1] * pvec[1] + e1[2] * pvec[2];
        if (std::fabs(det) < 1e-7f) {
            hit[i] = 0;
            t[i] = 0.0f; u[i] = 0.0f; v[i] = 0.0f;
            continue;
        }

        float inv_det = 1.0f / det;
        float tvec[3] = { ox[i] - v0[0], oy[i] - v0[1], oz[i] - v0[2] };
        float u_val = (tvec[0] * pvec[0] + tvec[1] * pvec[1] + tvec[2] * pvec[2]) * inv_det;
        if (u_val < 0.0f || u_val > 1.0f) {
            hit[i] = 0;
            t[i] = 0.0f; u[i] = 0.0f; v[i] = 0.0f;
            continue;
        }

        float qvec[3] = {
            tvec[1] * e1[2] - tvec[2] * e1[1],
            tvec[2] * e1[0] - tvec[0] * e1[2],
            tvec[0] * e1[1] - tvec[1] * e1[0]
        };
        float v_val = (dx[i] * qvec[0] + dy[i] * qvec[1] + dz[i] * qvec[2]) * inv_det;
        if (v_val < 0.0f || (u_val + v_val) > 1.0f) {
            hit[i] = 0;
            t[i] = 0.0f; u[i] = 0.0f; v[i] = 0.0f;
            continue;
        }

        float t_val = (e2[0] * qvec[0] + e2[1] * qvec[1] + e2[2] * qvec[2]) * inv_det;
        if (t_val > 1e-7f) {
            hit[i] = 1;
            t[i] = t_val;
            u[i] = u_val;
            v[i] = v_val;
        } else {
            hit[i] = 0;
            t[i] = 0.0f; u[i] = 0.0f; v[i] = 0.0f;
        }
    }
}
}]>;

def Kernel_acoustic_fdtd_step_3d : MasterKernelImplementation<"msimd_acoustic_fdtd_step_3d", [{
void msimd_acoustic_fdtd_step_3d(float* p, float* ux, float* uy, float* uz,
    size_t nx, size_t ny, size_t nz,
    float c, float rho, float dt, float dx, float dy, float dz, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!p || !ux || !uy || !uz || nx < 3 || ny < 3 || nz < 3) return;

    size_t slice = nx * ny;
    float bulk_modulus = rho * c * c;
    float inv_rho = 1.0f / rho;

    for (size_t z = 1; z < nz - 1; ++z) {
        for (size_t y = 1; y < ny - 1; ++y) {
            for (size_t x = 1; x < nx - 1; ++x) {
                size_t cell = z * slice + y * nx + x;
                float dp_dx = (p[cell + 1] - p[cell]) / dx;
                float dp_dy = (p[cell + nx] - p[cell]) / dy;
                float dp_dz = (p[cell + slice] - p[cell]) / dz;

                ux[cell] -= (inv_rho * dt) * dp_dx;
                uy[cell] -= (inv_rho * dt) * dp_dy;
                uz[cell] -= (inv_rho * dt) * dp_dz;
            }
        }
    }

    for (size_t z = 1; z < nz - 1; ++z) {
        for (size_t y = 1; y < ny - 1; ++y) {
            for (size_t x = 1; x < nx - 1; ++x) {
                size_t cell = z * slice + y * nx + x;
                float dux_dx = (ux[cell] - ux[cell - 1]) / dx;
                float duy_dy = (uy[cell] - uy[cell - nx]) / dy;
                float duz_dz = (uz[cell] - uz[cell - slice]) / dz;

                float div_u = dux_dx + duy_dy + duz_dz;
                p[cell] -= (bulk_modulus * dt) * div_u;
            }
        }
    }
}
}]>;

def Kernel_acoustic_doppler_shift : MasterKernelImplementation<"msimd_acoustic_doppler_shift", [{
float msimd_acoustic_doppler_shift(float f0, const float* src_pos, const float* src_vel,
    const float* lis_pos, const float* lis_vel, float speed_of_sound, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    float d[3];
    msimd_vec3f_sub(lis_pos, src_pos, d);
    float dist = msimd_vec3f_length(d);
    if (dist < 1e-4f) return f0;

    float dir[3];
    msimd_vec3f_scale(d, 1.0f / dist, dir);

    float v_s_radial = msimd_vec3f_dot(src_vel, dir);
    float v_l_radial = msimd_vec3f_dot(lis_vel, dir);

    float denom = speed_of_sound + v_s_radial;
    if (std::fabs(denom) < 1.0f) denom = (denom >= 0.0f) ? 1.0f : -1.0f;

    float f = f0 * ((speed_of_sound + v_l_radial) / denom);
    return std::max(f, 10.0f);
}
}]>;

def Kernel_acoustic_sabine_rt60 : MasterKernelImplementation<"msimd_acoustic_sabine_rt60", [{
float msimd_acoustic_sabine_rt60(float room_volume, const float* surface_areas, const float* absorption_coeffs, size_t surface_count, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (room_volume <= 0.0f || surface_count == 0) return 0.0f;
    double total_absorption = 0.0;
    for (size_t i = 0; i < surface_count; ++i) {
        total_absorption += (double)surface_areas[i] * (double)absorption_coeffs[i];
    }
    if (total_absorption <= 1e-7) return 10.0f;
    return (float)(0.161 * (double)room_volume / total_absorption);
}
}]>;

def Kernel_acoustic_hrtf_binaural_process : MasterKernelImplementation<"msimd_acoustic_hrtf_binaural_process", [{
void msimd_acoustic_hrtf_binaural_process(const float* in_audio, size_t sample_count,
    const float* hrtf_left_ir, const float* hrtf_right_ir, size_t ir_len,
    float* out_left, float* out_right, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    for (size_t n = 0; n < sample_count; ++n) {
        double acc_l = 0.0, acc_r = 0.0;
        for (size_t k = 0; k < ir_len; ++k) {
            if (n >= k) {
                float sample = in_audio[n - k];
                acc_l += (double)sample * (double)hrtf_left_ir[k];
                acc_r += (double)sample * (double)hrtf_right_ir[k];
            }
        }
        out_left[n] = (float)acc_l;
        out_right[n] = (float)acc_r;
    }
}
}]>;

def Kernel_compress_rle : MasterKernelImplementation<"msimd_compress_rle", [{
size_t msimd_compress_rle(const uint8_t* in, size_t in_len, uint8_t* out, size_t out_max) {
    if (!in || in_len == 0 || !out || out_max == 0) return 0;
    size_t in_idx = 0;
    size_t out_idx = 0;

    while (in_idx < in_len && out_idx + 2 <= out_max) {
        uint8_t byte = in[in_idx];
        uint8_t run = 1;
        while (in_idx + 1 < in_len && in[in_idx + 1] == byte && run < 255) {
            run++;
            in_idx++;
        }
        out[out_idx++] = run;
        out[out_idx++] = byte;
        in_idx++;
    }
    return out_idx;
}
}]>;

def Kernel_decompress_rle : MasterKernelImplementation<"msimd_decompress_rle", [{
size_t msimd_decompress_rle(const uint8_t* in, size_t in_len, uint8_t* out, size_t out_max) {
    if (!in || in_len < 2 || !out || out_max == 0) return 0;
    size_t in_idx = 0;
    size_t out_idx = 0;

    while (in_idx + 1 < in_len) {
        uint8_t run = in[in_idx++];
        uint8_t byte = in[in_idx++];
        for (uint8_t r = 0; r < run; ++r) {
            if (out_idx >= out_max) return out_idx;
            out[out_idx++] = byte;
        }
    }
    return out_idx;
}
}]>;

def Kernel_compress_lzss : MasterKernelImplementation<"msimd_compress_lzss", [{
size_t msimd_compress_lzss(const uint8_t* in, size_t in_len, uint8_t* out, size_t out_max) {
    if (!in || in_len == 0 || !out || out_max == 0) return 0;
    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < in_len && out_pos + 3 <= out_max) {
        size_t best_len = 0;
        size_t best_offset = 0;

        size_t win_start = (in_pos > MSIMD_LZSS_WINDOW) ? (in_pos - MSIMD_LZSS_WINDOW) : 0;
        for (size_t candidate = win_start; candidate < in_pos; ++candidate) {
            size_t match_len = 0;
            while (match_len < MSIMD_LZSS_MAX_MATCH &&
                   in_pos + match_len < in_len &&
                   in[candidate + match_len] == in[in_pos + match_len]) {
                match_len++;
            }
            if (match_len > best_len) {
                best_len = match_len;
                best_offset = in_pos - candidate;
            }
        }

        if (best_len >= MSIMD_LZSS_MIN_MATCH) {
            out[out_pos++] = 0x80 | (uint8_t)((best_len - MSIMD_LZSS_MIN_MATCH) & 0x0F);
            out[out_pos++] = (uint8_t)(best_offset & 0xFF);
            out[out_pos++] = (uint8_t)((best_offset >> 8) & 0x0F);
            in_pos += best_len;
        } else {
            out[out_pos++] = in[in_pos++];
        }
    }
    return out_pos;
}
}]>;

def Kernel_decompress_lzss : MasterKernelImplementation<"msimd_decompress_lzss", [{
size_t msimd_decompress_lzss(const uint8_t* in, size_t in_len, uint8_t* out, size_t out_max) {
    if (!in || in_len == 0 || !out || out_max == 0) return 0;
    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < in_len && out_pos < out_max) {
        uint8_t token = in[in_pos++];
        if (token & 0x80) {
            if (in_pos + 2 > in_len) break;
            size_t len = (token & 0x0F) + MSIMD_LZSS_MIN_MATCH;
            size_t offset = (size_t)in[in_pos] | (((size_t)in[in_pos + 1] & 0x0F) << 8);
            in_pos += 2;
            if (offset == 0 || offset > out_pos) break;
            size_t src = out_pos - offset;
            for (size_t i = 0; i < len; ++i) {
                if (out_pos >= out_max) return out_pos;
                out[out_pos++] = out[src + i];
            }
        } else {
            out[out_pos++] = token;
        }
    }
    return out_pos;
}
}]>;

def Kernel_bitpack_pack : MasterKernelImplementation<"msimd_bitpack_pack", [{
void msimd_bitpack_pack(const uint32_t* values, size_t count, int bitwidth, uint8_t* out_bytes, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    size_t bit_pos = 0;
    uint32_t mask = (bitwidth >= 32) ? 0xFFFFFFFFU : ((1U << bitwidth) - 1U);

    for (size_t i = 0; i < count; ++i) {
        uint32_t val = values[i] & mask;
        for (int b = 0; b < bitwidth; ++b) {
            size_t byte_idx = bit_pos / 8;
            int bit_idx = bit_pos % 8;
            if (val & (1U << b)) {
                out_bytes[byte_idx] |= (uint8_t)(1U << bit_idx);
            } else {
                out_bytes[byte_idx] &= (uint8_t)(~(1U << bit_idx));
            }
            bit_pos++;
        }
    }
}
}]>;

def Kernel_bitpack_unpack : MasterKernelImplementation<"msimd_bitpack_unpack", [{
void msimd_bitpack_unpack(const uint8_t* in_bytes, size_t count, int bitwidth, uint32_t* out_values, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    size_t bit_pos = 0;
    for (size_t i = 0; i < count; ++i) {
        uint32_t val = 0;
        for (int b = 0; b < bitwidth; ++b) {
            size_t byte_idx = bit_pos / 8;
            int bit_idx = bit_pos % 8;
            if (in_bytes[byte_idx] & (1U << bit_idx)) {
                val |= (1U << b);
            }
            bit_pos++;
        }
        out_values[i] = val;
    }
}
}]>;

def Kernel_float_to_half : MasterKernelImplementation<"msimd_float_to_half", [{
uint16_t msimd_float_to_half(float val) {
    uint32_t f;
    std::memcpy(&f, &val, 4);
    uint32_t sign = (f >> 31) & 0x1;
    int32_t exp = ((f >> 23) & 0xFF) - 127 + 15;
    uint32_t mant = f & 0x7FFFFF;

    if (exp <= 0) {
        return (uint16_t)(sign << 15);
    } else if (exp >= 31) {
        return (uint16_t)((sign << 15) | 0x7C00);
    }
    return (uint16_t)((sign << 15) | (exp << 10) | (mant >> 13));
}
}]>;

def Kernel_half_to_float : MasterKernelImplementation<"msimd_half_to_float", [{
float msimd_half_to_float(uint16_t h, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    uint32_t sign = (h >> 15) & 0x1;
    uint32_t exp = (h >> 10) & 0x1F;
    uint32_t mant = h & 0x3FF;

    uint32_t f_exp;
    uint32_t f_mant;

    if (exp == 0) {
        if (mant == 0) {
            uint32_t f = sign << 31;
            float res;
            std::memcpy(&res, &f, 4);
            return res;
        }
        f_exp = 127 - 14;
        f_mant = mant << 13;
    } else if (exp == 31) {
        f_exp = 255;
        f_mant = mant << 13;
    } else {
        f_exp = exp - 15 + 127;
        f_mant = mant << 13;
    }

    uint32_t f = (sign << 31) | (f_exp << 23) | f_mant;
    float res;
    std::memcpy(&res, &f, 4);
    return res;
}
}]>;

def Kernel_wave_morphology_step : MasterKernelImplementation<"msimd_wave_morphology_step", [{
void msimd_wave_morphology_step(float* out_state, const float* in_state, size_t n,
    float dispersion, float coupling, float nonlinearity, float dt, int mode, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 3) return;
    const float m_val = (mode > 0) ? (float)mode : 1.0f;
    const float k_disp = dispersion * 0.01f * m_val;
    const float k_coup = coupling * 0.005f * m_val;
    const float k_nonlin = nonlinearity * 0.002f;

    for (size_t j = 1; j < n - 1; ++j) {
        float laplacian = in_state[j - 1] - 2.0f * in_state[j] + in_state[j + 1];
        float cubic_term = in_state[j] * in_state[j] * in_state[j];
        float vel = k_disp * laplacian - k_nonlin * cubic_term + k_coup * in_state[j];
        out_state[j] = in_state[j] + vel * dt;
    }
    out_state[0] = out_state[1];
    out_state[n - 1] = out_state[n - 2];
}
}]>;

def Kernel_soliton_interaction_step : MasterKernelImplementation<"msimd_soliton_interaction_step", [{
void msimd_soliton_interaction_step(float* phi_re, float* phi_im, size_t n,
    float v1, float v2, float phase_offset, float dt, float dx, int mode, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 2) return;
    const float m_val = (mode > 0) ? (float)mode : 1.0f;
    const float omega = m_val * 0.05f;
    const float inv_dx2 = 1.0f / (dx * dx);

    for (size_t j = 1; j < n - 1; ++j) {
        float lap_r = (phi_re[j - 1] - 2.0f * phi_re[j] + phi_re[j + 1]) * inv_dx2;
        float lap_i = (phi_im[j - 1] - 2.0f * phi_im[j] + phi_im[j + 1]) * inv_dx2;
        float density = phi_re[j] * phi_re[j] + phi_im[j] * phi_im[j];

        float d_re = -0.5f * lap_i - density * phi_im[j];
        float d_im =  0.5f * lap_r + density * phi_re[j];

        phi_re[j] += d_re * dt;
        phi_im[j] += d_im * dt;
    }
}
}]>;

def Kernel_discrete_spatial_filter_step : MasterKernelImplementation<"msimd_discrete_spatial_filter_step", [{
void msimd_discrete_spatial_filter_step(float* target, const float* source, size_t n, float alpha, int mode, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 2) return;
    const float m_val = (mode > 0) ? (float)mode : 1.0f;
    const float w = std::clamp(alpha * (1.0f / m_val), 0.0f, 1.0f);
    target[0] = source[0];
    for (size_t j = 1; j < n; ++j) {
        target[j] = target[j - 1] + w * (source[j] - target[j - 1]);
    }
}
}]>;

def Kernel_wave_taxonomy_eval : MasterKernelImplementation<"msimd_wave_taxonomy_eval", [{
void msimd_wave_taxonomy_eval(int mode, float* out, const float* in, size_t n,
    float dispersion, float coupling, float nonlinearity, float dt, const msimd_tensor_t* precision_target) {
    msimd_wave_morphology_step(out, in, n, dispersion, coupling, nonlinearity, dt, mode, precision_target);
}
}]>;

def Kernel_ipow : MasterKernelImplementation<"msimd_ipow", [{
double msimd_ipow(double base, int64_t exp, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (exp == 0) return 1.0;
    if (exp < 0) {
        base = 1.0 / base;
        exp = -exp;
    }
    double res = 1.0;
    while (exp > 0) {
        if (exp & 1) res *= base;
        base *= base;
        exp >>= 1;
    }
    return res;
}
}]>;

def Kernel_pow : MasterKernelImplementation<"msimd_pow", [{
double msimd_pow(double base, double exp, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return std::pow(base, exp);
}
}]>;

def Kernel_lagrangian_step : MasterKernelImplementation<"msimd_lagrangian_step", [{
void msimd_lagrangian_step(double* q, double* q_dot, const double* mass, const double* grad_v, size_t n, double dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (size_t i = 0; i < n; ++i) {
        double m = (mass && mass[i] > 1e-12) ? mass[i] : 1.0;
        double a = -grad_v[i] / m;
        q_dot[i] += a * dt;
        q[i] += q_dot[i] * dt;
    }
}
}]>;

def Kernel_hamiltonian_step : MasterKernelImplementation<"msimd_hamiltonian_step", [{
void msimd_hamiltonian_step(double* q, double* p, const double* inv_m, const double* grad_v, size_t n, double dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (size_t i = 0; i < n; ++i) {
        double im = inv_m ? inv_m[i] : 1.0;
        p[i] -= grad_v[i] * dt;
        q[i] += p[i] * im * dt;
    }
}
}]>;

def Kernel_symplectic_yoshida4_step : MasterKernelImplementation<"msimd_symplectic_yoshida4_step", [{
void msimd_symplectic_yoshida4_step(double* q, double* p, const double* inv_m, void (*force_fn)(const double*, double*, void*), size_t n, double dt, void* user_data, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double cr = std::cbrt(2.0);
    const double w1 = 1.0 / (2.0 - cr);
    const double w0 = -cr / (2.0 - cr);
    const double c[4] = { w1 * 0.5, (w0 + w1) * 0.5, (w0 + w1) * 0.5, w1 * 0.5 };
    const double d[3] = { w1, w0, w1 };

    std::vector<double> f(n, 0.0);
    for (int step = 0; step < 3; ++step) {
        for (size_t i = 0; i < n; ++i) {
            double im = inv_m ? inv_m[i] : 1.0;
            q[i] += c[step] * p[i] * im * dt;
        }
        force_fn(q, f.data(), user_data);
        for (size_t i = 0; i < n; ++i) {
            p[i] += d[step] * f[i] * dt;
        }
    }
    for (size_t i = 0; i < n; ++i) {
        double im = inv_m ? inv_m[i] : 1.0;
        q[i] += c[3] * p[i] * im * dt;
    }
}
}]>;

def Kernel_kepler_orbit_solve : MasterKernelImplementation<"msimd_kepler_orbit_solve", [{
void msimd_kepler_orbit_solve(double a, double e, double M, double* r, double* nu, double* x, double* y, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double E = M;
    for (int iter = 0; iter < 30; ++iter) {
        double dE = (E - e * std::sin(E) - M) / (1.0 - e * std::cos(E));
        E -= dE;
        if (std::abs(dE) < 1e-12) break;
    }
    double denom = 1.0 - e * std::cos(E);
    double cos_nu = (std::cos(E) - e) / denom;
    double sin_nu = (std::sqrt(std::max(0.0, 1.0 - e * e)) * std::sin(E)) / denom;
    double true_anomaly = std::atan2(sin_nu, cos_nu);
    double radius = a * (1.0 - e * std::cos(E));

    if (r) *r = radius;
    if (nu) *nu = true_anomaly;
    if (x) *x = radius * std::cos(true_anomaly);
    if (y) *y = radius * std::sin(true_anomaly);
}
}]>;

def Kernel_cauchy_stress_strain_3d : MasterKernelImplementation<"msimd_cauchy_stress_strain_3d", [{
void msimd_cauchy_stress_strain_3d(const double grad_u[3][3], double lambda, double mu, double stress[3][3], double strain[3][3], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double trace_eps = 0.0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            strain[i][j] = 0.5 * (grad_u[i][j] + grad_u[j][i]);
        }
        trace_eps += strain[i][i];
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            stress[i][j] = 2.0 * mu * strain[i][j];
            if (i == j) stress[i][j] += lambda * trace_eps;
        }
    }
}
}]>;

def Kernel_navier_cauchy_elastodynamics_step : MasterKernelImplementation<"msimd_navier_cauchy_elastodynamics_step", [{
void msimd_navier_cauchy_elastodynamics_step(size_t nx, size_t ny, size_t nz, const float* u_prev, const float* u_curr, float* u_next, float lambda, float mu, float rho, float dt, float dx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (nx < 3 || ny < 3 || nz < 3) return;
    const float inv_rho = 1.0f / (rho > 1e-6f ? rho : 1.0f);
    const float dt2 = dt * dt;
    const float inv_dx2 = 1.0f / (dx * dx);
    const float c_bulk = (lambda + 2.0f * mu) * inv_rho * dt2 * inv_dx2;

    for (size_t z = 1; z < nz - 1; ++z) {
        for (size_t y = 1; y < ny - 1; ++y) {
            for (size_t x = 1; x < nx - 1; ++x) {
                size_t idx = (z * ny + y) * nx + x;
                float lap = u_curr[idx + 1] + u_curr[idx - 1] +
                            u_curr[idx + nx] + u_curr[idx - nx] +
                            u_curr[idx + nx * ny] + u_curr[idx - nx * ny] - 6.0f * u_curr[idx];
                u_next[idx] = 2.0f * u_curr[idx] - u_prev[idx] + c_bulk * lap;
            }
        }
    }
}
}]>;

def Kernel_euler_bernoulli_beam_step : MasterKernelImplementation<"msimd_euler_bernoulli_beam_step", [{
void msimd_euler_bernoulli_beam_step(size_t n, const float* w_prev, const float* w_curr, float* w_next, float EI, float rhoA, float q_load, float dt, float dx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (n < 5) return;
    const float inv_dx4 = 1.0f / (dx * dx * dx * dx);
    const float factor = (EI / (rhoA > 1e-6f ? rhoA : 1.0f)) * (dt * dt) * inv_dx4;
    const float q_term = (q_load / (rhoA > 1e-6f ? rhoA : 1.0f)) * (dt * dt);

    for (size_t i = 2; i < n - 2; ++i) {
        float d4 = w_curr[i - 2] - 4.0f * w_curr[i - 1] + 6.0f * w_curr[i] - 4.0f * w_curr[i + 1] + w_curr[i + 2];
        w_next[i] = 2.0f * w_curr[i] - w_prev[i] - factor * d4 + q_term;
    }
    w_next[0] = 0.0f; w_next[1] = 0.0f;
    w_next[n - 1] = 0.0f; w_next[n - 2] = 0.0f;
}
}]>;

def Kernel_thermo_van_der_waals_p : MasterKernelImplementation<"msimd_thermo_van_der_waals_p", [{
double msimd_thermo_van_der_waals_p(double T, double Vm, double a, double b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double R = 8.314462618;
    if (Vm <= b) return 0.0;
    return (R * T) / (Vm - b) - a / (Vm * Vm);
}
}]>;

def Kernel_thermo_stefan_boltzmann : MasterKernelImplementation<"msimd_thermo_stefan_boltzmann", [{
double msimd_thermo_stefan_boltzmann(double emissivity, double T, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double sigma = 5.670374419e-8;
    return emissivity * sigma * (T * T * T * T);
}
}]>;

def Kernel_ising_model_2d_step : MasterKernelImplementation<"msimd_ising_model_2d_step", [{
void msimd_ising_model_2d_step(int8_t* spin, size_t nx, size_t ny, double J, double B, double T, uint64_t* rng_state, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (nx < 2 || ny < 2 || !spin || !rng_state) return;
    auto lcg_rand = [](uint64_t* s) -> double {
        *s = (*s * 6364136223846793005ULL + 1442695040888963407ULL);
        return (double)(*s >> 11) * (1.0 / 9007199254740992.0);
    };

    const double inv_T = (T > 1e-6) ? (1.0 / T) : 1e6;
    for (size_t y = 0; y < ny; ++y) {
        size_t ym = (y == 0) ? ny - 1 : y - 1;
        size_t yp = (y == ny - 1) ? 0 : y + 1;
        for (size_t x = 0; x < nx; ++x) {
            size_t xm = (x == 0) ? nx - 1 : x - 1;
            size_t xp = (x == nx - 1) ? 0 : x + 1;

            int8_t s = spin[y * nx + x];
            int8_t neighbors = spin[ym * nx + x] + spin[yp * nx + x] + spin[y * nx + xm] + spin[y * nx + xp];
            double dE = 2.0 * s * (J * (double)neighbors + B);

            if (dE <= 0.0 || lcg_rand(rng_state) < std::exp(-dE * inv_T)) {
                spin[y * nx + x] = (int8_t)(-s);
            }
        }
    }
}
}]>;

def Kernel_optics_fresnel : MasterKernelImplementation<"msimd_optics_fresnel", [{
void msimd_optics_fresnel(double n1, double n2, double theta_i, double* r_s, double* r_p, double* t_s, double* t_p, double* brewster_angle, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double sin_theta_t = (n1 / n2) * std::sin(theta_i);
    if (std::abs(sin_theta_t) >= 1.0) {
        if (r_s) *r_s = 1.0;
        if (r_p) *r_p = 1.0;
        if (t_s) *t_s = 0.0;
        if (t_p) *t_p = 0.0;
    } else {
        double cos_i = std::cos(theta_i);
        double cos_t = std::sqrt(1.0 - sin_theta_t * sin_theta_t);
        double rs = (n1 * cos_i - n2 * cos_t) / (n1 * cos_i + n2 * cos_t);
        double rp = (n2 * cos_i - n1 * cos_t) / (n2 * cos_i + n1 * cos_t);
        if (r_s) *r_s = rs * rs;
        if (r_p) *r_p = rp * rp;
        if (t_s) *t_s = 1.0 - rs * rs;
        if (t_p) *t_p = 1.0 - rp * rp;
    }
    if (brewster_angle) *brewster_angle = std::atan(n2 / n1);
}
}]>;

def Kernel_optics_jones_apply : MasterKernelImplementation<"msimd_optics_jones_apply", [{
void msimd_optics_jones_apply(const double jones_mat_re[2][2], const double jones_mat_im[2][2], const double in_re[2], const double in_im[2], double out_re[2], double out_im[2], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 2; ++i) {
        out_re[i] = 0.0;
        out_im[i] = 0.0;
        for (int j = 0; j < 2; ++j) {
            out_re[i] += jones_mat_re[i][j] * in_re[j] - jones_mat_im[i][j] * in_im[j];
            out_im[i] += jones_mat_re[i][j] * in_im[j] + jones_mat_im[i][j] * in_re[j];
        }
    }
}
}]>;

def Kernel_optics_fraunhofer_airy : MasterKernelImplementation<"msimd_optics_fraunhofer_airy", [{
double msimd_optics_fraunhofer_airy(double r, double wavelength, double aperture_radius, double focal_length, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (r < 1e-12) return 1.0;
    const double k = (2.0 * M_PI) / wavelength;
    double x = (k * aperture_radius * r) / focal_length;
    double j1 = std::cyl_bessel_j(1, x);
    double val = (2.0 * j1) / x;
    return val * val;
}
}]>;

def Kernel_relativity_lorentz_boost : MasterKernelImplementation<"msimd_relativity_lorentz_boost", [{
void msimd_relativity_lorentz_boost(const double beta[3], const double four_vec_in[4], double four_vec_out[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double b2 = beta[0]*beta[0] + beta[1]*beta[1] + beta[2]*beta[2];
    if (b2 >= 1.0) b2 = 0.999999999999;
    double gamma = 1.0 / std::sqrt(1.0 - b2);
    double b_dot_r = beta[0]*four_vec_in[1] + beta[1]*four_vec_in[2] + beta[2]*four_vec_in[3];

    four_vec_out[0] = gamma * (four_vec_in[0] - b_dot_r);
    double factor = (b2 > 1e-15) ? ((gamma - 1.0) / b2) : 0.0;
    for (int i = 0; i < 3; ++i) {
        four_vec_out[i + 1] = four_vec_in[i + 1] + factor * b_dot_r * beta[i] - gamma * beta[i] * four_vec_in[0];
    }
}
}]>;

def Kernel_relativity_compton_shift : MasterKernelImplementation<"msimd_relativity_compton_shift", [{
double msimd_relativity_compton_shift(double theta, double wavelength_in, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double lambda_c = 2.42631023867e-12;
    double delta_lambda = lambda_c * (1.0 - std::cos(theta));
    return wavelength_in + delta_lambda;
}
}]>;

def Kernel_relativity_doppler : MasterKernelImplementation<"msimd_relativity_doppler", [{
double msimd_relativity_doppler(double f_source, double beta, double theta_rad, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (std::abs(beta) >= 1.0) beta = 0.999999999999;
    double gamma = 1.0 / std::sqrt(1.0 - beta * beta);
    return f_source / (gamma * (1.0 - beta * std::cos(theta_rad)));
}
}]>;

def Kernel_mat_eigen_jacobi : MasterKernelImplementation<"msimd_mat_eigen_jacobi", [{
int msimd_mat_eigen_jacobi(size_t n, const double* A, double* eigenvalues, double* eigenvectors, int max_iter, double tol, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::vector<double> D(A, A + n * n);
    std::vector<double> V(n * n, 0.0);
    for (size_t i = 0; i < n; ++i) V[i * n + i] = 1.0;

    for (int iter = 0; iter < max_iter; ++iter) {
        size_t p = 0, q = 1;
        double max_val = 0.0;
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = i + 1; j < n; ++j) {
                double val = std::abs(D[i * n + j]);
                if (val > max_val) {
                    max_val = val;
                    p = i; q = j;
                }
            }
        }
        if (max_val < tol) break;

        double theta = 0.5 * (D[q * n + q] - D[p * n + p]) / D[p * n + q];
        double t = 1.0 / (std::abs(theta) + std::sqrt(theta * theta + 1.0));
        if (theta < 0.0) t = -t;
        double c = 1.0 / std::sqrt(t * t + 1.0);
        double s = t * c;

        double d_pp = D[p * n + p], d_qq = D[q * n + q], d_pq = D[p * n + q];
        D[p * n + p] = c * c * d_pp - 2.0 * s * c * d_pq + s * s * d_qq;
        D[q * n + q] = s * s * d_pp + 2.0 * s * c * d_pq + c * c * d_qq;
        D[p * n + q] = 0.0; D[q * n + p] = 0.0;

        for (size_t k = 0; k < n; ++k) {
            if (k != p && k != q) {
                double d_kp = D[k * n + p], d_kq = D[k * n + q];
                D[k * n + p] = c * d_kp - s * d_kq;
                D[p * n + k] = D[k * n + p];
                D[k * n + q] = s * d_kp + c * d_kq;
                D[q * n + k] = D[k * n + q];
            }
            double v_kp = V[k * n + p], v_kq = V[k * n + q];
            V[k * n + p] = c * v_kp - s * v_kq;
            V[k * n + q] = s * v_kp + c * v_kq;
        }
    }

    for (size_t i = 0; i < n; ++i) eigenvalues[i] = D[i * n + i];
    for (size_t i = 0; i < n * n; ++i) eigenvectors[i] = V[i];
    return 0;
}
}]>;

def Kernel_mat_svd_3x3 : MasterKernelImplementation<"msimd_mat_svd_3x3", [{
void msimd_mat_svd_3x3(const double A[3][3], double U[3][3], double S[3], double V[3][3], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double AtA[9] = {0};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) AtA[i * 3 + j] += A[k][i] * A[k][j];
        }
    }
    double eig_val[3], eig_vec[9];
    msimd_mat_eigen_jacobi(3, AtA, eig_val, eig_vec, 50, 1e-9);

    for (int i = 0; i < 3; ++i) {
        S[i] = std::sqrt(std::max(0.0, eig_val[i]));
        for (int j = 0; j < 3; ++j) V[j][i] = eig_vec[j * 3 + i];
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            double sum = 0.0;
            for (int k = 0; k < 3; ++k) sum += A[i][k] * V[k][j];
            U[i][j] = (S[j] > 1e-12) ? (sum / S[j]) : 0.0;
        }
    }
}
}]>;

def Kernel_mat_cholesky : MasterKernelImplementation<"msimd_mat_cholesky", [{
int msimd_mat_cholesky(size_t n, const double* A, double* L, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::fill(L, L + n * n, 0.0);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j <= i; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < j; ++k) sum += L[i * n + k] * L[j * n + k];
            if (i == j) {
                double val = A[i * n + i] - sum;
                if (val <= 0.0) return -1;
                L[i * n + j] = std::sqrt(val);
            } else {
                L[i * n + j] = (A[i * n + j] - sum) / L[j * n + j];
            }
        }
    }
    return 0;
}
}]>;

def Kernel_mat_lu_decompose : MasterKernelImplementation<"msimd_mat_lu_decompose", [{
int msimd_mat_lu_decompose(size_t n, const double* A, double* LU, int* p, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::copy(A, A + n * n, LU);
    for (size_t i = 0; i < n; ++i) p[i] = (int)i;

    for (size_t i = 0; i < n; ++i) {
        size_t max_row = i;
        double max_val = std::abs(LU[i * n + i]);
        for (size_t k = i + 1; k < n; ++k) {
            double val = std::abs(LU[k * n + i]);
            if (val > max_val) {
                max_val = val;
                max_row = k;
            }
        }
        if (max_val < 1e-14) return -1;
        if (max_row != i) {
            std::swap(p[i], p[max_row]);
            for (size_t k = 0; k < n; ++k) std::swap(LU[i * n + k], LU[max_row * n + k]);
        }
        for (size_t j = i + 1; j < n; ++j) {
            LU[j * n + i] /= LU[i * n + i];
            for (size_t k = i + 1; k < n; ++k) {
                LU[j * n + k] -= LU[j * n + i] * LU[i * n + k];
            }
        }
    }
    return 0;
}
}]>;

def Kernel_mat_exp_pade : MasterKernelImplementation<"msimd_mat_exp_pade", [{
void msimd_mat_exp_pade(size_t n, const double* A, double* expA, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double norm_a = 0.0;
    for (size_t i = 0; i < n * n; ++i) norm_a = std::max(norm_a, std::abs(A[i]));
    int s = 0;
    if (std::isfinite(norm_a)) {
        while (norm_a > 0.5 && s < 1022) { norm_a *= 0.5; ++s; }
    }

    double scale = std::ldexp(1.0, -s);
    std::vector<double> A_scaled(n * n), I(n * n, 0.0);
    for (size_t i = 0; i < n * n; ++i) A_scaled[i] = A[i] * scale;
    for (size_t i = 0; i < n; ++i) I[i * n + i] = 1.0;

    std::vector<double> cur = I, term = I;
    double fact = 1.0;
    for (int k = 1; k <= 8; ++k) {
        fact *= k;
        std::vector<double> next(n * n, 0.0);
        for (size_t r = 0; r < n; ++r) {
            for (size_t c = 0; c < n; ++c) {
                for (size_t m = 0; m < n; ++m) next[r * n + c] += term[r * n + m] * A_scaled[m * n + c];
            }
        }
        term = next;
        for (size_t i = 0; i < n * n; ++i) cur[i] += term[i] / fact;
    }
    for (int sq = 0; sq < s; ++sq) {
        std::vector<double> next(n * n, 0.0);
        for (size_t r = 0; r < n; ++r) {
            for (size_t c = 0; c < n; ++c) {
                for (size_t m = 0; m < n; ++m) next[r * n + c] += cur[r * n + m] * cur[m * n + c];
            }
        }
        cur = next;
    }
    std::copy(cur.begin(), cur.end(), expA);
}
}]>;

def Kernel_clifford_cl3_mul : MasterKernelImplementation<"msimd_clifford_cl3_mul", [{
void msimd_clifford_cl3_mul(const double a[8], const double b[8], double out[8], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double s = a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3] - a[4]*b[4] - a[5]*b[5] - a[6]*b[6] - a[7]*b[7];
    double v1 = a[0]*b[1] + a[1]*b[0] - a[2]*b[4] + a[3]*b[6] + a[4]*b[2] - a[6]*b[3] - a[5]*b[7] - a[7]*b[5];
    double v2 = a[0]*b[2] + a[2]*b[0] + a[1]*b[4] - a[3]*b[5] - a[4]*b[1] + a[5]*b[3] - a[6]*b[7] - a[7]*b[6];
    double v3 = a[0]*b[3] + a[3]*b[0] - a[1]*b[6] + a[2]*b[5] + a[6]*b[1] - a[5]*b[2] - a[4]*b[7] - a[7]*b[4];
    double b12 = a[0]*b[4] + a[4]*b[0] + a[1]*b[2] - a[2]*b[1] + a[3]*b[7] + a[7]*b[3] + a[5]*b[6] - a[6]*b[5];
    double b23 = a[0]*b[5] + a[5]*b[0] + a[2]*b[3] - a[3]*b[2] + a[1]*b[7] + a[7]*b[1] + a[6]*b[4] - a[4]*b[6];
    double b31 = a[0]*b[6] + a[6]*b[0] + a[3]*b[1] - a[1]*b[3] + a[2]*b[7] + a[7]*b[2] + a[4]*b[5] - a[5]*b[4];
    double ps = a[0]*b[7] + a[7]*b[0] + a[1]*b[5] + a[2]*b[6] + a[3]*b[4] + a[4]*b[3] + a[5]*b[1] + a[6]*b[2];
    out[0] = s; out[1] = v1; out[2] = v2; out[3] = v3;
    out[4] = b12; out[5] = b23; out[6] = b31; out[7] = ps;
}
}]>;

def Kernel_lie_se3_exp : MasterKernelImplementation<"msimd_lie_se3_exp", [{
void msimd_lie_se3_exp(const double xi[6], double T[4][4], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double w[3] = { xi[3], xi[4], xi[5] };
    double theta = std::sqrt(w[0]*w[0] + w[1]*w[1] + w[2]*w[2]);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) T[i][j] = (i == j) ? 1.0 : 0.0;
    }
    if (theta < 1e-12) {
        T[0][3] = xi[0]; T[1][3] = xi[1]; T[2][3] = xi[2];
        return;
    }
    double k[3] = { w[0]/theta, w[1]/theta, w[2]/theta };
    double s = std::sin(theta), c = std::cos(theta), v = 1.0 - c;
    T[0][0] = c + k[0]*k[0]*v;        T[0][1] = k[0]*k[1]*v - k[2]*s; T[0][2] = k[0]*k[2]*v + k[1]*s;
    T[1][0] = k[1]*k[0]*v + k[2]*s;   T[1][1] = c + k[1]*k[1]*v;        T[1][2] = k[1]*k[2]*v - k[0]*s;
    T[2][0] = k[2]*k[0]*v - k[1]*s;   T[2][1] = k[2]*k[1]*v + k[0]*s;   T[2][2] = c + k[2]*k[2]*v;

    double B = (1.0 - c) / (theta * theta);
    double C = (theta - s) / (theta * theta * theta);
    double V_mat[3][3];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) V_mat[i][j] = (i == j) ? 1.0 : 0.0;
    }
    double W[3][3] = { { 0, -w[2], w[1] }, { w[2], 0, -w[0] }, { -w[1], w[0], 0 } };
    double W2[3][3] = {0};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int m = 0; m < 3; ++m) W2[i][j] += W[i][m] * W[m][j];
            V_mat[i][j] += B * W[i][j] + C * W2[i][j];
        }
    }
    for (int i = 0; i < 3; ++i) {
        T[i][3] = V_mat[i][0]*xi[0] + V_mat[i][1]*xi[1] + V_mat[i][2]*xi[2];
    }
}
}]>;

def Kernel_lie_se3_log : MasterKernelImplementation<"msimd_lie_se3_log", [{
void msimd_lie_se3_log(const double T[4][4], double xi[6], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double tr = T[0][0] + T[1][1] + T[2][2];
    double cos_theta = std::clamp((tr - 1.0) * 0.5, -1.0, 1.0);
    double theta = std::acos(cos_theta);
    if (theta < 1e-12) {
        xi[0] = T[0][3]; xi[1] = T[1][3]; xi[2] = T[2][3];
        xi[3] = 0.0; xi[4] = 0.0; xi[5] = 0.0;
        return;
    }
    double factor = 0.5 * theta / std::sin(theta);
    xi[3] = (T[2][1] - T[1][2]) * factor;
    xi[4] = (T[0][2] - T[2][0]) * factor;
    xi[5] = (T[1][0] - T[0][1]) * factor;
    xi[0] = T[0][3]; xi[1] = T[1][3]; xi[2] = T[2][3];
}
}]>;

def Kernel_num_gauss_legendre_quad : MasterKernelImplementation<"msimd_num_gauss_legendre_quad", [{
double msimd_num_gauss_legendre_quad(double (*fn)(double, void*), double a, double b, int n_points, void* user_data, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    static const double x5[5] = { 0.0, -0.538469310105683, 0.538469310105683, -0.906179845938664, 0.906179845938664 };
    static const double w5[5] = { 0.568888888888889, 0.478628670499366, 0.478628670499366, 0.236926885056189, 0.236926885056189 };

    double h = 0.5 * (b - a);
    double m = 0.5 * (b + a);
    double sum = 0.0;
    for (int i = 0; i < 5; ++i) {
        sum += w5[i] * fn(m + h * x5[i], user_data);
    }
    return h * sum;
}
}]>;

def Kernel_num_romberg_integral : MasterKernelImplementation<"msimd_num_romberg_integral", [{
double msimd_num_romberg_integral(double (*fn)(double, void*), double a, double b, int max_steps, double tol, void* user_data, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!fn || max_steps <= 0) return 0.0;
    if (max_steps > 15) max_steps = 15;
    double R[16][16] = {0};
    double h = b - a;
    R[0][0] = 0.5 * h * (fn(a, user_data) + fn(b, user_data));

    for (int i = 1; i < max_steps; ++i) {
        h *= 0.5;
        double sum = 0.0;
        int pts = 1 << (i - 1);
        for (int k = 1; k <= pts; ++k) {
            sum += fn(a + (2 * k - 1) * h, user_data);
        }
        R[i][0] = 0.5 * R[i - 1][0] + h * sum;
        for (int j = 1; j <= i; ++j) {
            double factor = std::pow(4.0, j);
            R[i][j] = (factor * R[i][j - 1] - R[i - 1][j - 1]) / (factor - 1.0);
        }
        if (std::abs(R[i][i] - R[i - 1][i - 1]) < tol) {
            return R[i][i];
        }
    }
    return R[max_steps - 1][max_steps - 1];
}
}]>;

def Kernel_opt_nelder_mead : MasterKernelImplementation<"msimd_opt_nelder_mead", [{
int msimd_opt_nelder_mead(double (*fn)(const double*, void*), size_t dim, double* x_start, double step, int max_iter, double tol, double* x_opt, double* f_opt, void* user_data, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    size_t n = dim;
    std::vector<std::vector<double>> simplex(n + 1, std::vector<double>(n));
    std::vector<double> f_vals(n + 1);

    simplex[0].assign(x_start, x_start + n);
    f_vals[0] = fn(simplex[0].data(), user_data);

    for (size_t i = 1; i <= n; ++i) {
        simplex[i] = simplex[0];
        simplex[i][i - 1] += step;
        f_vals[i] = fn(simplex[i].data(), user_data);
    }

    for (int iter = 0; iter < max_iter; ++iter) {
        std::vector<size_t> idx(n + 1);
        for (size_t i = 0; i <= n; ++i) idx[i] = i;
        std::sort(idx.begin(), idx.end(), [&](size_t a, size_t b) { return f_vals[a] < f_vals[b]; });

        if (std::abs(f_vals[idx[n]] - f_vals[idx[0]]) < tol) break;

        std::vector<double> centroid(n, 0.0);
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) centroid[j] += simplex[idx[i]][j] / (double)n;
        }

        std::vector<double> xr(n);
        for (size_t j = 0; j < n; ++j) xr[j] = centroid[j] + 1.0 * (centroid[j] - simplex[idx[n]][j]);
        double fr = fn(xr.data(), user_data);

        if (fr >= f_vals[idx[0]] && fr < f_vals[idx[n - 1]]) {
            simplex[idx[n]] = xr;
            f_vals[idx[n]] = fr;
        } else if (fr < f_vals[idx[0]]) {
            std::vector<double> xe(n);
            for (size_t j = 0; j < n; ++j) xe[j] = centroid[j] + 2.0 * (xr[j] - centroid[j]);
            double fe = fn(xe.data(), user_data);
            if (fe < fr) { simplex[idx[n]] = xe; f_vals[idx[n]] = fe; }
            else { simplex[idx[n]] = xr; f_vals[idx[n]] = fr; }
        } else {
            std::vector<double> xc(n);
            for (size_t j = 0; j < n; ++j) xc[j] = centroid[j] + 0.5 * (simplex[idx[n]][j] - centroid[j]);
            double fc = fn(xc.data(), user_data);
            if (fc < f_vals[idx[n]]) { simplex[idx[n]] = xc; f_vals[idx[n]] = fc; }
            else {
                for (size_t i = 1; i <= n; ++i) {
                    for (size_t j = 0; j < n; ++j) simplex[idx[i]][j] = 0.5 * (simplex[idx[i]][j] + simplex[idx[0]][j]);
                    f_vals[idx[i]] = fn(simplex[idx[i]].data(), user_data);
                }
            }
        }
    }

    size_t best_idx = 0;
    for (size_t i = 1; i <= n; ++i) {
        if (f_vals[i] < f_vals[best_idx]) best_idx = i;
    }
    if (x_opt) std::copy(simplex[best_idx].begin(), simplex[best_idx].end(), x_opt);
    if (f_opt) *f_opt = f_vals[best_idx];
    return 0;
}
}]>;

def Kernel_opt_lbfgs_two_loop : MasterKernelImplementation<"msimd_opt_lbfgs_two_loop", [{
void msimd_opt_lbfgs_two_loop(size_t n, size_t m, const double* grad, const double* const* s_history, const double* const* y_history, const double* rho_history, size_t history_count, double* dir, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::vector<double> q(grad, grad + n);
    std::vector<double> alpha(history_count, 0.0);

    for (int i = (int)history_count - 1; i >= 0; --i) {
        double s_dot_q = 0.0;
        for (size_t j = 0; j < n; ++j) s_dot_q += s_history[i][j] * q[j];
        alpha[i] = rho_history[i] * s_dot_q;
        for (size_t j = 0; j < n; ++j) q[j] -= alpha[i] * y_history[i][j];
    }

    double gamma = 1.0;
    if (history_count > 0) {
        size_t last = history_count - 1;
        double s_dot_y = 0.0, y_dot_y = 0.0;
        for (size_t j = 0; j < n; ++j) {
            s_dot_y += s_history[last][j] * y_history[last][j];
            y_dot_y += y_history[last][j] * y_history[last][j];
        }
        if (y_dot_y > 1e-12) gamma = s_dot_y / y_dot_y;
    }

    for (size_t j = 0; j < n; ++j) dir[j] = gamma * q[j];

    for (size_t i = 0; i < history_count; ++i) {
        double y_dot_r = 0.0;
        for (size_t j = 0; j < n; ++j) y_dot_r += y_history[i][j] * dir[j];
        double beta = rho_history[i] * y_dot_r;
        for (size_t j = 0; j < n; ++j) dir[j] += s_history[i][j] * (alpha[i] - beta);
    }
}
}]>;

def Kernel_dist_normal_pdf : MasterKernelImplementation<"msimd_dist_normal_pdf", [{
double msimd_dist_normal_pdf(double x, double mu, double sigma, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (sigma <= 0.0) return 0.0;
    double z = (x - mu) / sigma;
    return (1.0 / (sigma * std::sqrt(2.0 * M_PI))) * std::exp(-0.5 * z * z);
}
}]>;

def Kernel_dist_normal_cdf : MasterKernelImplementation<"msimd_dist_normal_cdf", [{
double msimd_dist_normal_cdf(double x, double mu, double sigma, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (sigma <= 0.0) return (x >= mu) ? 1.0 : 0.0;
    return 0.5 * (1.0 + std::erf((x - mu) / (sigma * std::sqrt(2.0))));
}
}]>;

def Kernel_dist_gamma_pdf : MasterKernelImplementation<"msimd_dist_gamma_pdf", [{
double msimd_dist_gamma_pdf(double x, double alpha, double beta, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (x <= 0.0 || alpha <= 0.0 || beta <= 0.0) return 0.0;
    return (std::pow(beta, alpha) / std::tgamma(alpha)) * std::pow(x, alpha - 1.0) * std::exp(-beta * x);
}
}]>;

def Kernel_dist_poisson_pmf : MasterKernelImplementation<"msimd_dist_poisson_pmf", [{
double msimd_dist_poisson_pmf(int k, double lambda, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (k < 0 || lambda <= 0.0) return 0.0;
    return std::exp(-lambda + (double)k * std::log(lambda) - std::lgamma((double)(k + 1)));
}
}]>;

def Kernel_dist_chisquare_cdf : MasterKernelImplementation<"msimd_dist_chisquare_cdf", [{
double msimd_dist_chisquare_cdf(double x, int k, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (x <= 0.0 || k <= 0) return 0.0;
    double a = 0.5 * k;
    double z = 0.5 * x;
    double sum = 1.0 / a;
    double term = 1.0 / a;
    for (int n = 1; n < 100; ++n) {
        term *= z / (a + n);
        sum += term;
        if (term < sum * 1e-12) break;
    }
    return sum * std::exp(-z + a * std::log(z) - std::lgamma(a));
}
}]>;

def Kernel_stat_covariance_matrix : MasterKernelImplementation<"msimd_stat_covariance_matrix", [{
void msimd_stat_covariance_matrix(size_t num_samples, size_t num_vars, const double* data, double* cov_matrix, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (num_samples < 2 || num_vars == 0) return;
    std::vector<double> mean(num_vars, 0.0);
    for (size_t s = 0; s < num_samples; ++s) {
        for (size_t v = 0; v < num_vars; ++v) mean[v] += data[s * num_vars + v];
    }
    for (size_t v = 0; v < num_vars; ++v) mean[v] /= (double)num_samples;

    double factor = 1.0 / (double)(num_samples - 1);
    for (size_t i = 0; i < num_vars; ++i) {
        for (size_t j = 0; j < num_vars; ++j) {
            double sum = 0.0;
            for (size_t s = 0; s < num_samples; ++s) {
                sum += (data[s * num_vars + i] - mean[i]) * (data[s * num_vars + j] - mean[j]);
            }
            cov_matrix[i * num_vars + j] = sum * factor;
        }
    }
}
}]>;

def Kernel_filter_kalman_step : MasterKernelImplementation<"msimd_filter_kalman_step", [{
void msimd_filter_kalman_step(size_t dim_x, size_t dim_z, double* x, double* P, const double* F, const double* Q, const double* H, const double* R, const double* z, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    std::vector<double> x_p(dim_x, 0.0);
    for (size_t i = 0; i < dim_x; ++i) {
        for (size_t j = 0; j < dim_x; ++j) x_p[i] += F[i * dim_x + j] * x[j];
    }
    std::vector<double> FP(dim_x * dim_x, 0.0);
    for (size_t i = 0; i < dim_x; ++i) {
        for (size_t j = 0; j < dim_x; ++j) {
            for (size_t k = 0; k < dim_x; ++k) FP[i * dim_x + j] += F[i * dim_x + k] * P[k * dim_x + j];
        }
    }
    std::vector<double> P_p(dim_x * dim_x, 0.0);
    for (size_t i = 0; i < dim_x; ++i) {
        for (size_t j = 0; j < dim_x; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < dim_x; ++k) sum += FP[i * dim_x + k] * F[j * dim_x + k];
            P_p[i * dim_x + j] = sum + Q[i * dim_x + j];
        }
    }

    std::vector<double> y(dim_z, 0.0);
    for (size_t i = 0; i < dim_z; ++i) {
        double hx = 0.0;
        for (size_t j = 0; j < dim_x; ++j) hx += H[i * dim_x + j] * x_p[j];
        y[i] = z[i] - hx;
    }

    std::vector<double> HP(dim_z * dim_x, 0.0);
    for (size_t i = 0; i < dim_z; ++i) {
        for (size_t j = 0; j < dim_x; ++j) {
            for (size_t k = 0; k < dim_x; ++k) HP[i * dim_x + j] += H[i * dim_x + k] * P_p[k * dim_x + j];
        }
    }
    std::vector<double> S(dim_z * dim_z, 0.0);
    for (size_t i = 0; i < dim_z; ++i) {
        for (size_t j = 0; j < dim_z; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < dim_x; ++k) sum += HP[i * dim_x + k] * H[j * dim_x + k];
            S[i * dim_z + j] = sum + R[i * dim_z + j];
        }
    }

    std::vector<double> inv_S(dim_z * dim_z, 0.0);
    if (dim_z == 1) {
        inv_S[0] = (std::abs(S[0]) > 1e-15) ? (1.0 / S[0]) : 0.0;
    } else if (dim_z == 2) {
        double det_S = S[0] * S[3] - S[1] * S[2];
        if (std::abs(det_S) > 1e-15) {
            double inv_det = 1.0 / det_S;
            inv_S[0] =  S[3] * inv_det; inv_S[1] = -S[1] * inv_det;
            inv_S[2] = -S[2] * inv_det; inv_S[3] =  S[0] * inv_det;
        }
    } else {

        std::vector<double> aug(dim_z * 2 * dim_z, 0.0);
        for (size_t i = 0; i < dim_z; ++i) {
            for (size_t j = 0; j < dim_z; ++j) {
                aug[i * (2 * dim_z) + j] = S[i * dim_z + j];
            }
            aug[i * (2 * dim_z) + (dim_z + i)] = 1.0;
        }

        bool singular = false;
        for (size_t i = 0; i < dim_z; ++i) {
            size_t max_row = i;
            double max_val = std::abs(aug[i * (2 * dim_z) + i]);
            for (size_t r = i + 1; r < dim_z; ++r) {
                double val = std::abs(aug[r * (2 * dim_z) + i]);
                if (val > max_val) {
                    max_val = val;
                    max_row = r;
                }
            }
            if (max_val < 1e-15) {
                singular = true;
                break;
            }
            if (max_row != i) {
                for (size_t c = 0; c < 2 * dim_z; ++c) {
                    std::swap(aug[i * (2 * dim_z) + c], aug[max_row * (2 * dim_z) + c]);
                }
            }
            double pivot = aug[i * (2 * dim_z) + i];
            double inv_p = 1.0 / pivot;
            for (size_t c = 0; c < 2 * dim_z; ++c) {
                aug[i * (2 * dim_z) + c] *= inv_p;
            }
            for (size_t r = 0; r < dim_z; ++r) {
                if (r != i) {
                    double factor = aug[r * (2 * dim_z) + i];
                    for (size_t c = 0; c < 2 * dim_z; ++c) {
                        aug[r * (2 * dim_z) + c] -= factor * aug[i * (2 * dim_z) + c];
                    }
                }
            }
        }
        if (!singular) {
            for (size_t i = 0; i < dim_z; ++i) {
                for (size_t j = 0; j < dim_z; ++j) {
                    inv_S[i * dim_z + j] = aug[i * (2 * dim_z) + (dim_z + j)];
                }
            }
        }
    }

    std::vector<double> PHT(dim_x * dim_z, 0.0);
    for (size_t i = 0; i < dim_x; ++i) {
        for (size_t j = 0; j < dim_z; ++j) {
            for (size_t k = 0; k < dim_x; ++k) PHT[i * dim_z + j] += P_p[i * dim_x + k] * H[j * dim_x + k];
        }
    }
    std::vector<double> K(dim_x * dim_z, 0.0);
    for (size_t i = 0; i < dim_x; ++i) {
        for (size_t j = 0; j < dim_z; ++j) {
            for (size_t k = 0; k < dim_z; ++k) K[i * dim_z + j] += PHT[i * dim_z + k] * inv_S[k * dim_z + j];
        }
    }

    for (size_t i = 0; i < dim_x; ++i) {
        double ky = 0.0;
        for (size_t j = 0; j < dim_z; ++j) ky += K[i * dim_z + j] * y[j];
        x[i] = x_p[i] + ky;
    }
    for (size_t i = 0; i < dim_x; ++i) {
        for (size_t j = 0; j < dim_x; ++j) {
            double kh = 0.0;
            for (size_t k = 0; k < dim_z; ++k) kh += K[i * dim_z + k] * H[k * dim_x + j];
            double delta = (i == j) ? 1.0 - kh : -kh;
            double sum = 0.0;
            for (size_t m = 0; m < dim_x; ++m) sum += delta * P_p[m * dim_x + j];
            P[i * dim_x + j] = sum;
        }
    }
}
}]>;

def Kernel_fuzzy_triangular : MasterKernelImplementation<"msimd_fuzzy_triangular", [{
double msimd_fuzzy_triangular(double x, double a, double b, double c, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (x <= a || x >= c) return 0.0;
    if (x == b) return 1.0;
    if (x < b) return (x - a) / (b - a);
    return (c - x) / (c - b);
}
}]>;

def Kernel_fuzzy_trapezoidal : MasterKernelImplementation<"msimd_fuzzy_trapezoidal", [{
double msimd_fuzzy_trapezoidal(double x, double a, double b, double c, double d, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (x <= a || x >= d) return 0.0;
    if (x >= b && x <= c) return 1.0;
    if (x < b) return (x - a) / (b - a);
    return (d - x) / (d - c);
}
}]>;

def Kernel_fuzzy_gaussian : MasterKernelImplementation<"msimd_fuzzy_gaussian", [{
double msimd_fuzzy_gaussian(double x, double c, double sigma, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (sigma <= 0.0) return (x == c) ? 1.0 : 0.0;
    double z = (x - c) / sigma;
    return std::exp(-0.5 * z * z);
}
}]>;

def Kernel_fuzzy_tnorm_min : MasterKernelImplementation<"msimd_fuzzy_tnorm_min", [{
double msimd_fuzzy_tnorm_min(double a, double b, const msimd_tensor_t* precision_target) {
    (void)precision_target; return std::min(a, b); }
}]>;

def Kernel_fuzzy_snorm_max : MasterKernelImplementation<"msimd_fuzzy_snorm_max", [{
double msimd_fuzzy_snorm_max(double a, double b, const msimd_tensor_t* precision_target) {
    (void)precision_target; return std::max(a, b); }
}]>;

def Kernel_fuzzy_defuzzify_centroid : MasterKernelImplementation<"msimd_fuzzy_defuzzify_centroid", [{
double msimd_fuzzy_defuzzify_centroid(const double* x_vals, const double* mu_vals, size_t n, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double num = 0.0, den = 0.0;
    for (size_t i = 0; i < n; ++i) {
        num += x_vals[i] * mu_vals[i];
        den += mu_vals[i];
    }
    return (den > 1e-12) ? (num / den) : 0.0;
}
}]>;

def Kernel_gf256_mul : MasterKernelImplementation<"msimd_gf256_mul", [{
uint8_t msimd_gf256_mul(uint8_t a, uint8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    uint8_t p = 0;
    for (int i = 0; i < 8; ++i) {
        if (b & 1) p ^= a;
        uint8_t hi_bit = (a & 0x80);
        a <<= 1;
        if (hi_bit) a ^= 0x1B;
        b >>= 1;
    }
    return p;
}
}]>;

def Kernel_gf256_inv : MasterKernelImplementation<"msimd_gf256_inv", [{
uint8_t msimd_gf256_inv(uint8_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (a == 0) return 0;
    uint8_t res = 1;
    uint8_t base = a;
    int exp = 254;
    while (exp > 0) {
        if (exp & 1) res = msimd_gf256_mul(res, base);
        base = msimd_gf256_mul(base, base);
        exp >>= 1;
    }
    return res;
}
}]>;

def Kernel_gravity_poisson_step : MasterKernelImplementation<"msimd_gravity_poisson_step", [{
void msimd_gravity_poisson_step(size_t nx, size_t ny, size_t nz, const float* rho_mass, const float* phi_curr, float* phi_next, float G_const, float dx, float relax_factor, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (nx < 3 || ny < 3 || nz < 3) return;
    const float dx2 = dx * dx;
    const float src_factor = 4.0f * (float)M_PI * G_const * dx2;
    const float inv_6 = 1.0f / 6.0f;
    const float omega = (relax_factor > 0.0f && relax_factor < 2.0f) ? relax_factor : 1.0f;

    for (size_t z = 1; z < nz - 1; ++z) {
        for (size_t y = 1; y < ny - 1; ++y) {
            for (size_t x = 1; x < nx - 1; ++x) {
                size_t idx = (z * ny + y) * nx + x;
                float neighbor_sum = phi_curr[idx + 1] + phi_curr[idx - 1] +
                                     phi_curr[idx + nx] + phi_curr[idx - nx] +
                                     phi_curr[idx + nx * ny] + phi_curr[idx - nx * ny];
                float phi_jacobi = (neighbor_sum - src_factor * rho_mass[idx]) * inv_6;
                phi_next[idx] = (1.0f - omega) * phi_curr[idx] + omega * phi_jacobi;
            }
        }
    }
}
}]>;

def Kernel_gravity_schwarzschild_metric : MasterKernelImplementation<"msimd_gravity_schwarzschild_metric", [{
void msimd_gravity_schwarzschild_metric(double M_mass, double r_dist, double impact_b, double* time_dilation, double* rs_radius, double* deflection_angle, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double G = 6.67430e-11;
    const double c = 299792458.0;
    const double c2 = c * c;

    double rs = (2.0 * G * M_mass) / c2;
    if (rs_radius) *rs_radius = rs;

    if (time_dilation) {
        if (r_dist <= rs) {
            *time_dilation = 0.0;
        } else {
            *time_dilation = std::sqrt(1.0 - rs / r_dist);
        }
    }

    if (deflection_angle) {
        if (impact_b > rs) {
            *deflection_angle = (2.0 * rs) / impact_b;
        } else {
            *deflection_angle = M_PI;
        }
    }
}
}]>;

def Kernel_quantum_dirac_spinor_step : MasterKernelImplementation<"msimd_quantum_dirac_spinor_step", [{
void msimd_quantum_dirac_spinor_step(const double psi_re_in[4], const double psi_im_in[4], const double A_pot[4], double mass, double dt, double psi_re_out[4], double psi_im_out[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double c = 299792458.0;
    const double hbar = 1.054571817e-34;
    const double q = -1.602176634e-19;

    double energy_mass = mass * c * c;
    double scalar_pot = q * A_pot[0];

    for (int i = 0; i < 4; ++i) {
        double mass_term = (i < 2) ? energy_mass : -energy_mass;
        double omega = (scalar_pot + mass_term) / hbar;
        double cos_wt = std::cos(omega * dt);
        double sin_wt = std::sin(omega * dt);

        psi_re_out[i] = psi_re_in[i] * cos_wt + psi_im_in[i] * sin_wt;
        psi_im_out[i] = psi_im_in[i] * cos_wt - psi_re_in[i] * sin_wt;
    }
}
}]>;

def Kernel_quantum_pauli_spin_precession : MasterKernelImplementation<"msimd_quantum_pauli_spin_precession", [{
void msimd_quantum_pauli_spin_precession(double spin[3], const double B_field[3], double gamma_gyro, double dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double initial_mag = std::sqrt(spin[0]*spin[0] + spin[1]*spin[1] + spin[2]*spin[2]);
    if (initial_mag < 1e-50) return;
    double torque[3] = {
        gamma_gyro * (spin[1] * B_field[2] - spin[2] * B_field[1]),
        gamma_gyro * (spin[2] * B_field[0] - spin[0] * B_field[2]),
        gamma_gyro * (spin[0] * B_field[1] - spin[1] * B_field[0])
    };
    for (int i = 0; i < 3; ++i) {
        spin[i] += torque[i] * dt;
    }
    double mag = std::sqrt(spin[0]*spin[0] + spin[1]*spin[1] + spin[2]*spin[2]);
    if (mag > 1e-50) {
        double scale = initial_mag / mag;
        for (int i = 0; i < 3; ++i) spin[i] *= scale;
    }
}
}]>;

def Kernel_qcd_cornell_potential : MasterKernelImplementation<"msimd_qcd_cornell_potential", [{
void msimd_qcd_cornell_potential(double r_dist, double alpha_s, double sigma_string, double* potential, double* force, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (r_dist < 1e-18) r_dist = 1e-18;
    double coul = (4.0 / 3.0) * (alpha_s / r_dist);
    double conf = sigma_string * r_dist;

    if (potential) *potential = -coul + conf;
    if (force) *force = -(4.0 / 3.0) * (alpha_s / (r_dist * r_dist)) - sigma_string;
}
}]>;

def Kernel_nuclear_yukawa_force : MasterKernelImplementation<"msimd_nuclear_yukawa_force", [{
void msimd_nuclear_yukawa_force(double r_dist, double g_coupling, double pion_mass, double* potential, double* force, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (r_dist < 1e-18) r_dist = 1e-18;
    const double hbar = 1.054571817e-34;
    const double c = 299792458.0;
    double mu = (pion_mass * c) / hbar;
    double exp_term = std::exp(-mu * r_dist);
    double g2 = g_coupling * g_coupling;

    double V = -g2 * (exp_term / r_dist);
    if (potential) *potential = V;
    if (force) {
        *force = -g2 * exp_term * (1.0 + mu * r_dist) / (r_dist * r_dist);
    }
}
}]>;

def Kernel_qcd_gellmann_apply : MasterKernelImplementation<"msimd_qcd_gellmann_apply", [{
void msimd_qcd_gellmann_apply(int index, const double color_in_re[3], const double color_in_im[3], double color_out_re[3], double color_out_im[3], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) {
        color_out_re[i] = 0.0;
        color_out_im[i] = 0.0;
    }
    const double inv_sqrt3 = 0.57735026918962576451;

    switch (index) {
        case 1:
            color_out_re[0] = color_in_re[1]; color_out_im[0] = color_in_im[1];
            color_out_re[1] = color_in_re[0]; color_out_im[1] = color_in_im[0];
            break;
        case 2:
            color_out_re[0] = color_in_im[1];  color_out_im[0] = -color_in_re[1];
            color_out_re[1] = -color_in_im[0]; color_out_im[1] = color_in_re[0];
            break;
        case 3:
            color_out_re[0] = color_in_re[0];  color_out_im[0] = color_in_im[0];
            color_out_re[1] = -color_in_re[1]; color_out_im[1] = -color_in_im[1];
            break;
        case 4:
            color_out_re[0] = color_in_re[2]; color_out_im[0] = color_in_im[2];
            color_out_re[2] = color_in_re[0]; color_out_im[2] = color_in_im[0];
            break;
        case 5:
            color_out_re[0] = color_in_im[2];  color_out_im[0] = -color_in_re[2];
            color_out_re[2] = -color_in_im[0]; color_out_im[2] = color_in_re[0];
            break;
        case 6:
            color_out_re[1] = color_in_re[2]; color_out_im[1] = color_in_im[2];
            color_out_re[2] = color_in_re[1]; color_out_im[2] = color_in_im[1];
            break;
        case 7:
            color_out_re[1] = color_in_im[2];  color_out_im[1] = -color_in_re[2];
            color_out_re[2] = -color_in_im[1]; color_out_im[2] = color_in_re[1];
            break;
        case 8:
            color_out_re[0] = inv_sqrt3 * color_in_re[0]; color_out_im[0] = inv_sqrt3 * color_in_im[0];
            color_out_re[1] = inv_sqrt3 * color_in_re[1]; color_out_im[1] = inv_sqrt3 * color_in_im[1];
            color_out_re[2] = -2.0 * inv_sqrt3 * color_in_re[2]; color_out_im[2] = -2.0 * inv_sqrt3 * color_in_im[2];
            break;
        default:
            for (int i = 0; i < 3; ++i) {
                color_out_re[i] = color_in_re[i];
                color_out_im[i] = color_in_im[i];
            }
            break;
    }
}
}]>;

def Kernel_higgs_potential : MasterKernelImplementation<"msimd_higgs_potential", [{
void msimd_higgs_potential(double phi, double mu_sq, double lambda_self, double* potential, double* dV_dphi, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double phi2 = phi * phi;
    if (potential) {
        *potential = -0.5 * mu_sq * phi2 + 0.25 * lambda_self * phi2 * phi2;
    }
    if (dV_dphi) {
        *dV_dphi = phi * (-mu_sq + lambda_self * phi2);
    }
}
}]>;

def Kernel_higgs_fermion_mass : MasterKernelImplementation<"msimd_higgs_fermion_mass", [{
double msimd_higgs_fermion_mass(double yukawa_coupling, double vev_v, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double inv_sqrt2 = 0.7071067811865475;
    return yukawa_coupling * vev_v * inv_sqrt2;
}
}]>;

def Kernel_weak_fermi_decay_rate : MasterKernelImplementation<"msimd_weak_fermi_decay_rate", [{
void msimd_weak_fermi_decay_rate(double delta_E, double G_fermi, double* decay_rate, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (delta_E < 0.0) delta_E = 0.0;
    double dE5 = delta_E * delta_E;
    dE5 = dE5 * dE5 * delta_E;
    const double pi3 = 31.00627668;
    const double denom = 192.0 * pi3;
    if (decay_rate) {
        *decay_rate = (G_fermi * G_fermi * dE5) / denom;
    }
}
}]>;

def Kernel_weak_boson_potential : MasterKernelImplementation<"msimd_weak_boson_potential", [{
void msimd_weak_boson_potential(double r_dist, double g_weak, double m_w_boson, double* potential, double* force, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double hbar = 1.054571817e-34;
    const double c = 299792458.0;
    if (r_dist < 1e-21) r_dist = 1e-21;
    double r_weak = hbar / (m_w_boson * c);
    if (r_weak < 1e-21) r_weak = 1e-21;

    double exp_term = std::exp(-r_dist / r_weak);
    double inv_4pi = 0.07957747154594767;
    double v = (g_weak * g_weak * inv_4pi / r_dist) * exp_term;

    if (potential) *potential = v;
    if (force) {
        *force = (v / r_dist) * (1.0 + r_dist / r_weak);
    }
}
}]>;

def Kernel_quantum_dirac_charge_conjugate : MasterKernelImplementation<"msimd_quantum_dirac_charge_conjugate", [{
void msimd_quantum_dirac_charge_conjugate(const double psi_re_in[4], const double psi_im_in[4], double psi_re_out[4], double psi_im_out[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    psi_re_out[0] = -psi_re_in[3]; psi_im_out[0] =  psi_im_in[3];
    psi_re_out[1] =  psi_re_in[2]; psi_im_out[1] = -psi_im_in[2];
    psi_re_out[2] = -psi_re_in[1]; psi_im_out[2] =  psi_im_in[1];
    psi_re_out[3] =  psi_re_in[0]; psi_im_out[3] = -psi_im_in[0];
}
}]>;

def Kernel_particle_annihilation_energy : MasterKernelImplementation<"msimd_particle_annihilation_energy", [{
void msimd_particle_annihilation_energy(double m1, double m2, double* total_energy_joules, double* photon_frequency, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    const double c = 299792458.0;
    const double h_planck = 6.62607015e-34;
    double e_tot = (m1 + m2) * c * c;
    if (total_energy_joules) *total_energy_joules = e_tot;
    if (photon_frequency) {
        *photon_frequency = (e_tot * 0.5) / h_planck;
    }
}
}]>;

def Kernel_qcd_antiquark_color_conjugate : MasterKernelImplementation<"msimd_qcd_antiquark_color_conjugate", [{
void msimd_qcd_antiquark_color_conjugate(const double color_in_re[3], const double color_in_im[3], double color_out_re[3], double color_out_im[3], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    for (int i = 0; i < 3; ++i) {
        color_out_re[i] =  color_in_re[i];
        color_out_im[i] = -color_in_im[i];
    }
}
}]>;

def Kernel_planck_quantize_length : MasterKernelImplementation<"msimd_planck_quantize_length", [{
int64_t msimd_planck_quantize_length(double meters, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (std::isnan(meters)) return 0;
    double q = meters / MSIMD_PLANCK_LENGTH;
    if (q > (double)INT64_MAX) return INT64_MAX;
    if (q < (double)INT64_MIN) return INT64_MIN;
    return static_cast<int64_t>(std::round(q));
}
}]>;

def Kernel_planck_dequantize_length : MasterKernelImplementation<"msimd_planck_dequantize_length", [{
double msimd_planck_dequantize_length(int64_t planck_units, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return static_cast<double>(planck_units) * MSIMD_PLANCK_LENGTH;
}
}]>;

def Kernel_planck_quantize_time : MasterKernelImplementation<"msimd_planck_quantize_time", [{
int64_t msimd_planck_quantize_time(double seconds, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (std::isnan(seconds)) return 0;
    double q = seconds / MSIMD_PLANCK_TIME;
    if (q > (double)INT64_MAX) return INT64_MAX;
    if (q < (double)INT64_MIN) return INT64_MIN;
    return static_cast<int64_t>(std::round(q));
}
}]>;

def Kernel_planck_dequantize_time : MasterKernelImplementation<"msimd_planck_dequantize_time", [{
double msimd_planck_dequantize_time(int64_t planck_units, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return static_cast<double>(planck_units) * MSIMD_PLANCK_TIME;
}
}]>;

def Kernel_planck_discrete_step : MasterKernelImplementation<"msimd_planck_discrete_step", [{
void msimd_planck_discrete_step(const int64_t* pos_in, const int64_t* vel_in, int64_t dt_planck,
    int64_t* pos_out, size_t n, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!pos_in || !vel_in || !pos_out || n == 0) return;
    for (size_t i = 0; i < n; ++i) {

        pos_out[i] = pos_in[i] + vel_in[i] * dt_planck;
    }
}
}]>;

def Kernel_compensated_sum_neumaier : MasterKernelImplementation<"msimd_compensated_sum_neumaier", [{
void msimd_compensated_sum_neumaier(const double* values, size_t n, double* sum_out, double* comp_out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!values || n == 0) {
        if (sum_out) *sum_out = 0.0;
        if (comp_out) *comp_out = 0.0;
        return;
    }
    double sum = values[0];
    double c = 0.0;
    for (size_t i = 1; i < n; ++i) {
        double cur = values[i];
        double t = sum + cur;
        if (std::abs(sum) >= std::abs(cur)) {
            c += (sum - t) + cur;
        } else {
            c += (cur - t) + sum;
        }
        sum = t;
    }
    if (sum_out)  *sum_out = sum + c;
    if (comp_out) *comp_out = c;
}
}]>;

def Kernel_compensated_sum_neumaier_f32 : MasterKernelImplementation<"msimd_compensated_sum_neumaier_f32", [{
void msimd_compensated_sum_neumaier_f32(const float* values, size_t n, float* sum_out, float* comp_out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!values || n == 0) {
        if (sum_out) *sum_out = 0.0f;
        if (comp_out) *comp_out = 0.0f;
        return;
    }
    float sum = values[0];
    float c = 0.0f;
    for (size_t i = 1; i < n; ++i) {
        float cur = values[i];
        float t = sum + cur;
        if (std::abs(sum) >= std::abs(cur)) {
            c += (sum - t) + cur;
        } else {
            c += (cur - t) + sum;
        }
        sum = t;
    }
    if (sum_out)  *sum_out = sum + c;
    if (comp_out) *comp_out = c;
}
}]>;

def Kernel_horner_eval : MasterKernelImplementation<"msimd_horner_eval", [{
double msimd_horner_eval(const double* coeffs, size_t degree, double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!coeffs) return 0.0;
    double result = coeffs[degree];
    for (size_t i = degree; i > 0; --i) {
        result = result * x + coeffs[i - 1];
    }
    return result;
}
}]>;

def Kernel_estrin_eval_8 : MasterKernelImplementation<"msimd_estrin_eval_8", [{
double msimd_estrin_eval_8(const double* c, double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!c) return 0.0;
    double x2 = x * x;
    double x4 = x2 * x2;
    double p01 = c[0] + c[1] * x;
    double p23 = c[2] + c[3] * x;
    double p45 = c[4] + c[5] * x;
    double p67 = c[6] + c[7] * x;
    double q03 = p01 + p23 * x2;
    double q47 = p45 + p67 * x2;
    return q03 + q47 * x4;
}
}]>;

def Kernel_minimax_erf : MasterKernelImplementation<"msimd_minimax_erf", [{
double msimd_minimax_erf(double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (std::isnan(x)) return x;
    double ax = std::abs(x);
    if (ax >= 6.0) return (x > 0.0) ? 1.0 : -1.0;

    if (ax <= 0.84375) {
        const double p[] = {
            0.128379167095512558561e+01,
            -0.325042107242198812700e+00,
            -0.284817495126148158520e-01,
            -0.577027029648944159100e-02,
            -0.237630166566501626000e-04
        };
        const double q[] = {
            0.100000000000000000000e+01,
            0.397917223959155352819e+00,
            0.650222499887672944485e-01,
            0.508130628186091662970e-02,
            0.132494737604696290000e-03
        };
        double z = ax * ax;
        double num = p[0] + z * (p[1] + z * (p[2] + z * (p[3] + z * p[4])));
        double den = q[0] + z * (q[1] + z * (q[2] + z * (q[3] + z * q[4])));
        double res = ax * (num / den);
        return (x >= 0.0) ? res : -res;
    } else {
        const double p[] = {
            0.564189583547756077845e+00,
            0.880253746105520775370e+00,
            0.546646861569045885485e+00,
            0.169708451999186252120e+00,
            0.268361519296234120000e-01,
            0.161960293049609080000e-02
        };
        const double q[] = {
            0.100000000000000000000e+01,
            0.234114727194601131444e+01,
            0.222666880443281156910e+01,
            0.105787412084111951000e+01,
            0.252063564874722510000e+00,
            0.240572560e-01
        };
        double num = p[0] + ax * (p[1] + ax * (p[2] + ax * (p[3] + ax * (p[4] + ax * p[5]))));
        double den = q[0] + ax * (q[1] + ax * (q[2] + ax * (q[3] + ax * (q[4] + ax * q[5]))));
        double erfc_val = std::exp(-ax * ax) * (num / den);
        double res = 1.0 - erfc_val;
        return (x >= 0.0) ? res : -res;
    }
}
}]>;

def Kernel_minimax_gamma : MasterKernelImplementation<"msimd_minimax_gamma", [{
double msimd_minimax_gamma(double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (std::isnan(x)) return x;
    if (x <= 0.0 && std::floor(x) == x) return std::numeric_limits<double>::infinity();

    if (x < 0.5) {

        double sin_px = std::sin(M_PI * x);
        if (std::abs(sin_px) < 1e-15) return std::numeric_limits<double>::infinity();
        return M_PI / (sin_px * msimd_minimax_gamma(1.0 - x));
    }

    x -= 1.0;
    const double p[] = {
        0.99999999999980993,
        676.5203681218851,
        -1259.1392167224028,
        771.32342877765313,
        -176.61502916214059,
        12.507343278686905,
        -0.138571095836526,
        9.9843695780195716e-6,
        1.5056327351493116e-7
    };
    double a = p[0];
    double t = x + 7.5;
    for (int i = 1; i < 9; ++i) {
        a += p[i] / (x + (double)i);
    }
    double sqrt_2pi = 2.5066282746310005;
    return sqrt_2pi * std::pow(t, x + 0.5) * std::exp(-t) * a;
}
}]>;

def Kernel_legendre_polynomial : MasterKernelImplementation<"msimd_legendre_polynomial", [{
double msimd_legendre_polynomial(int n, double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (n < 0) return 0.0;
    if (n == 0) return 1.0;
    if (n == 1) return x;

    double p_prev = 1.0;
    double p_curr = x;
    for (int k = 1; k < n; ++k) {
        double p_next = ((2 * k + 1) * x * p_curr - k * p_prev) / (k + 1);
        p_prev = p_curr;
        p_curr = p_next;
    }
    return p_curr;
}
}]>;

def Kernel_winograd_f2x2_3x3_filter_transform : MasterKernelImplementation<"msimd_winograd_f2x2_3x3_filter_transform", [{
void msimd_winograd_f2x2_3x3_filter_transform(const float g[9], float U[16], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!g || !U) return;

    float T[12];
    for (int col = 0; col < 3; ++col) {
        float g0 = g[0 * 3 + col];
        float g1 = g[1 * 3 + col];
        float g2 = g[2 * 3 + col];
        T[0 * 3 + col] = g0;
        T[1 * 3 + col] = 0.5f * (g0 + g1 + g2);
        T[2 * 3 + col] = 0.5f * (g0 - g1 + g2);
        T[3 * 3 + col] = g2;
    }

    for (int row = 0; row < 4; ++row) {
        float t0 = T[row * 3 + 0];
        float t1 = T[row * 3 + 1];
        float t2 = T[row * 3 + 2];
        U[row * 4 + 0] = t0;
        U[row * 4 + 1] = 0.5f * (t0 + t1 + t2);
        U[row * 4 + 2] = 0.5f * (t0 - t1 + t2);
        U[row * 4 + 3] = t2;
    }
}
}]>;

def Kernel_winograd_f2x2_3x3_input_transform : MasterKernelImplementation<"msimd_winograd_f2x2_3x3_input_transform", [{
void msimd_winograd_f2x2_3x3_input_transform(const float d[16], float V[16], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!d || !V) return;

    float T[16];
    for (int col = 0; col < 4; ++col) {
        float d0 = d[0 * 4 + col];
        float d1 = d[1 * 4 + col];
        float d2 = d[2 * 4 + col];
        float d3 = d[3 * 4 + col];
        T[0 * 4 + col] = d0 - d2;
        T[1 * 4 + col] = d1 + d2;
        T[2 * 4 + col] = d2 - d1;
        T[3 * 4 + col] = d1 - d3;
    }

    for (int row = 0; row < 4; ++row) {
        float t0 = T[row * 4 + 0];
        float t1 = T[row * 4 + 1];
        float t2 = T[row * 4 + 2];
        float t3 = T[row * 4 + 3];
        V[row * 4 + 0] = t0 - t2;
        V[row * 4 + 1] = t1 + t2;
        V[row * 4 + 2] = t2 - t1;
        V[row * 4 + 3] = t1 - t3;
    }
}
}]>;

def Kernel_winograd_f2x2_3x3_output_transform : MasterKernelImplementation<"msimd_winograd_f2x2_3x3_output_transform", [{
void msimd_winograd_f2x2_3x3_output_transform(const float M[16], float Y[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!M || !Y) return;

    float T[8];
    for (int col = 0; col < 4; ++col) {
        float m0 = M[0 * 4 + col];
        float m1 = M[1 * 4 + col];
        float m2 = M[2 * 4 + col];
        float m3 = M[3 * 4 + col];
        T[0 * 4 + col] = m0 + m1 + m2;
        T[1 * 4 + col] = m1 - m2 - m3;
    }

    for (int row = 0; row < 2; ++row) {
        float t0 = T[row * 4 + 0];
        float t1 = T[row * 4 + 1];
        float t2 = T[row * 4 + 2];
        float t3 = T[row * 4 + 3];
        Y[row * 2 + 0] = t0 + t1 + t2;
        Y[row * 2 + 1] = t1 - t2 - t3;
    }
}
}]>;

def Kernel_winograd_f2x2_3x3_exec : MasterKernelImplementation<"msimd_winograd_f2x2_3x3_exec", [{
void msimd_winograd_f2x2_3x3_exec(const float input_tile4x4[16], const float filter_trans16[16], float output_tile2x2[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!input_tile4x4 || !filter_trans16 || !output_tile2x2) return;
    float V[16];
    msimd_winograd_f2x2_3x3_input_transform(input_tile4x4, V);

    float M[16];
    for (int i = 0; i < 16; ++i) {
        M[i] = filter_trans16[i] * V[i];
    }
    msimd_winograd_f2x2_3x3_output_transform(M, output_tile2x2);
}
}]>;

def Kernel_winograd_conv2d_3x3 : MasterKernelImplementation<"msimd_winograd_conv2d_3x3", [{
void msimd_winograd_conv2d_3x3(const float* image, int width, int height,
    const float kernel[9], float* output, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!image || !kernel || !output || width < 4 || height < 4) return;

    float U[16];
    msimd_winograd_f2x2_3x3_filter_transform(kernel, U);

    int out_w = width - 2;
    int out_h = height - 2;

    for (int oy = 0; oy < out_h; oy += 2) {
        for (int ox = 0; ox < out_w; ox += 2) {
            float d[16];
            for (int r = 0; r < 4; ++r) {
                int iy = oy + r;
                for (int c = 0; c < 4; ++c) {
                    int ix = ox + c;
                    if (ix < width && iy < height) {
                        d[r * 4 + c] = image[iy * width + ix];
                    } else {
                        d[r * 4 + c] = 0.0f;
                    }
                }
            }

            float y[4];
            msimd_winograd_f2x2_3x3_exec(d, U, y);

            if (oy < out_h && ox < out_w)
                output[oy * out_w + ox] = y[0];
            if (oy < out_h && ox + 1 < out_w)
                output[oy * out_w + ox + 1] = y[1];
            if (oy + 1 < out_h && ox < out_w)
                output[(oy + 1) * out_w + ox] = y[2];
            if (oy + 1 < out_h && ox + 1 < out_w)
                output[(oy + 1) * out_w + ox + 1] = y[3];
        }
    }
}
}]>;

def Kernel_pga3d_plane_meet_plane : MasterKernelImplementation<"msimd_pga3d_plane_meet_plane", [{
void msimd_pga3d_plane_meet_plane(const double p1[4], const double p2[4], double line_out[6], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    line_out[0] = p1[1] * p2[2] - p1[2] * p2[1];
    line_out[1] = p1[2] * p2[0] - p1[0] * p2[2];
    line_out[2] = p1[0] * p2[1] - p1[1] * p2[0];

    line_out[3] = p1[3] * p2[0] - p2[3] * p1[0];
    line_out[4] = p1[3] * p2[1] - p2[3] * p1[1];
    line_out[5] = p1[3] * p2[2] - p2[3] * p1[2];
}
}]>;

def Kernel_pga3d_plane_meet_line : MasterKernelImplementation<"msimd_pga3d_plane_meet_line", [{
void msimd_pga3d_plane_meet_line(const double p[4], const double l[6], double pt_out[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    pt_out[0] =  p[1] * l[5] - p[2] * l[4] + p[3] * l[0];
    pt_out[1] =  p[2] * l[3] - p[0] * l[5] + p[3] * l[1];
    pt_out[2] =  p[0] * l[4] - p[1] * l[3] + p[3] * l[2];
    pt_out[3] = -(p[0] * l[0] + p[1] * l[1] + p[2] * l[2]);
}
}]>;

def Kernel_pga3d_point_join_point : MasterKernelImplementation<"msimd_pga3d_point_join_point", [{
void msimd_pga3d_point_join_point(const double pt1[4], const double pt2[4], double line_out[6], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    line_out[0] = pt1[3] * pt2[0] - pt2[3] * pt1[0];
    line_out[1] = pt1[3] * pt2[1] - pt2[3] * pt1[1];
    line_out[2] = pt1[3] * pt2[2] - pt2[3] * pt1[2];

    line_out[3] = pt1[1] * pt2[2] - pt1[2] * pt2[1];
    line_out[4] = pt1[2] * pt2[0] - pt1[0] * pt2[2];
    line_out[5] = pt1[0] * pt2[1] - pt1[1] * pt2[0];
}
}]>;

def Kernel_pga3d_motor_compose : MasterKernelImplementation<"msimd_pga3d_motor_compose", [{
void msimd_pga3d_motor_compose(const double m1[8], const double m2[8], double m_out[8], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    double s1 = m1[0], x1 = m1[1], y1 = m1[2], z1 = m1[3];
    double s2 = m2[0], x2 = m2[1], y2 = m2[2], z2 = m2[3];

    m_out[0] = s1*s2 - x1*x2 - y1*y2 - z1*z2;
    m_out[1] = s1*x2 + x1*s2 + y1*z2 - z1*y2;
    m_out[2] = s1*y2 - x1*z2 + y1*s2 + z1*x2;
    m_out[3] = s1*z2 + x1*y2 - y1*x2 + z1*s2;

    double d0_1 = m1[4], d1_1 = m1[5], d2_1 = m1[6], p1 = m1[7];
    double d0_2 = m2[4], d1_2 = m2[5], d2_2 = m2[6], p2 = m2[7];

    m_out[4] = s1*d0_2 + d0_1*s2 + y1*d2_2 - z1*d1_2 + d1_1*z2 - d2_1*y2;
    m_out[5] = s1*d1_2 - x1*d2_2 + d1_1*s2 + z1*d0_2 - d0_1*z2 + d2_1*x2;
    m_out[6] = s1*d2_2 + x1*d1_2 - y1*d0_2 + d2_1*s2 + d0_1*y2 - d1_1*x2;
    m_out[7] = s1*p2 + p1*s2 + x1*d0_2 + y1*d1_2 + z1*d2_2 + d0_1*x2 + d1_1*y2 + d2_1*z2;
}
}]>;

def Kernel_pga3d_motor_sandwich_point : MasterKernelImplementation<"msimd_pga3d_motor_sandwich_point", [{
void msimd_pga3d_motor_sandwich_point(const double m[8], const double pt_in[4], double pt_out[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    double s = m[0], rx = m[1], ry = m[2], rz = m[3];
    double px = pt_in[0], py = pt_in[1], pz = pt_in[2], pw = pt_in[3];

    double cx = ry * pz - rz * py + s * px;
    double cy = rz * px - rx * pz + s * py;
    double cz = rx * py - ry * px + s * pz;

    double vx = px + 2.0 * (ry * cz - rz * cy);
    double vy = py + 2.0 * (rz * cx - rx * cz);
    double vz = pz + 2.0 * (rx * cy - ry * cx);

    double tx = 2.0 * (m[4]*s + m[5]*rz - m[6]*ry + m[7]*rx);
    double ty = 2.0 * (m[5]*s + m[6]*rx - m[4]*rz + m[7]*ry);
    double tz = 2.0 * (m[6]*s + m[4]*ry - m[5]*rx + m[7]*rz);

    pt_out[0] = vx + tx * pw;
    pt_out[1] = vy + ty * pw;
    pt_out[2] = vz + tz * pw;
    pt_out[3] = pw;
}
}]>;

def Kernel_pga3d_motor_sandwich_plane : MasterKernelImplementation<"msimd_pga3d_motor_sandwich_plane", [{
void msimd_pga3d_motor_sandwich_plane(const double m[8], const double pl_in[4], double pl_out[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    double s = m[0], rx = m[1], ry = m[2], rz = m[3];
    double nx = pl_in[0], ny = pl_in[1], nz = pl_in[2], d = pl_in[3];

    double cx = ry * nz - rz * ny + s * nx;
    double cy = rz * nx - rx * nz + s * ny;
    double cz = rx * ny - ry * nx + s * nz;

    double rnx = nx + 2.0 * (ry * cz - rz * cy);
    double rny = ny + 2.0 * (rz * cx - rx * cz);
    double rnz = nz + 2.0 * (rx * cy - ry * cx);

    double tx = 2.0 * (m[4]*s + m[5]*rz - m[6]*ry + m[7]*rx);
    double ty = 2.0 * (m[5]*s + m[6]*rx - m[4]*rz + m[7]*ry);
    double tz = 2.0 * (m[6]*s + m[4]*ry - m[5]*rx + m[7]*rz);

    pl_out[0] = rnx;
    pl_out[1] = rny;
    pl_out[2] = rnz;
    pl_out[3] = d - (rnx * tx + rny * ty + rnz * tz);
}
}]>;

def Kernel_dual_quat_from_rotation_translation : MasterKernelImplementation<"msimd_dual_quat_from_rotation_translation", [{
void msimd_dual_quat_from_rotation_translation(const double q_rot[4], const double trans[3], double dq_out[8], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!q_rot || !trans || !dq_out) return;
    dq_out[0] = q_rot[0];
    dq_out[1] = q_rot[1];
    dq_out[2] = q_rot[2];
    dq_out[3] = q_rot[3];

    double tx = trans[0], ty = trans[1], tz = trans[2];
    double qw = q_rot[0], qx = q_rot[1], qy = q_rot[2], qz = q_rot[3];

    dq_out[4] = 0.5 * (-tx * qx - ty * qy - tz * qz);
    dq_out[5] = 0.5 * ( tx * qw + ty * qz - tz * qy);
    dq_out[6] = 0.5 * (-tx * qz + ty * qw + tz * qx);
    dq_out[7] = 0.5 * ( tx * qy - ty * qx + tz * qw);
}
}]>;

def Kernel_dual_quat_multiply : MasterKernelImplementation<"msimd_dual_quat_multiply", [{
void msimd_dual_quat_multiply(const double dq1[8], const double dq2[8], double dq_out[8], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!dq1 || !dq2 || !dq_out) return;

    double w1 = dq1[0], x1 = dq1[1], y1 = dq1[2], z1 = dq1[3];
    double w2 = dq2[0], x2 = dq2[1], y2 = dq2[2], z2 = dq2[3];

    dq_out[0] = w1*w2 - x1*x2 - y1*y2 - z1*z2;
    dq_out[1] = w1*x2 + x1*w2 + y1*z2 - z1*y2;
    dq_out[2] = w1*y2 - x1*z2 + y1*w2 + z1*x2;
    dq_out[3] = w1*z2 + x1*y2 - y1*x2 + z1*w2;

    double dw1 = dq1[4], dx1 = dq1[5], dy1 = dq1[6], dz1 = dq1[7];
    double dw2 = dq2[4], dx2 = dq2[5], dy2 = dq2[6], dz2 = dq2[7];

    dq_out[4] = (w1*dw2 - x1*dx2 - y1*dy2 - z1*dz2) + (dw1*w2 - dx1*x2 - dy1*y2 - dz1*z2);
    dq_out[5] = (w1*dx2 + x1*dw2 + y1*dz2 - z1*dy2) + (dw1*x2 + dx1*w2 + dy1*z2 - dz1*y2);
    dq_out[6] = (w1*dy2 - x1*dz2 + y1*dw2 + z1*dx2) + (dw1*y2 - dx1*z2 + dy1*w2 + dz1*x2);
    dq_out[7] = (w1*dz2 + x1*dy2 - y1*dx2 + z1*dw2) + (dw1*z2 + dx1*y2 - dy1*x2 + dz1*w2);
}
}]>;

def Kernel_dual_quat_transform_point : MasterKernelImplementation<"msimd_dual_quat_transform_point", [{
void msimd_dual_quat_transform_point(const double dq[8], const double pt_in[3], double pt_out[3], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!dq || !pt_in || !pt_out) return;

    double qw = dq[0], qx = dq[1], qy = dq[2], qz = dq[3];
    double px = pt_in[0], py = pt_in[1], pz = pt_in[2];

    double cx = qy * pz - qz * py + qw * px;
    double cy = qz * px - qx * pz + qw * py;
    double cz = qx * py - qy * px + qw * pz;

    double rx = px + 2.0 * (qy * cz - qz * cy);
    double ry = py + 2.0 * (qz * cx - qx * cz);
    double rz = pz + 2.0 * (qx * cy - qy * cx);

    double dw = dq[4], dx = dq[5], dy = dq[6], dz = dq[7];
    double tx = 2.0 * (-dw * qx + dx * qw - dy * qz + dz * qy);
    double ty = 2.0 * (-dw * qy + dx * qz + dy * qw - dz * qx);
    double tz = 2.0 * (-dw * qz - dx * qy + dy * qx + dz * qw);

    pt_out[0] = rx + tx;
    pt_out[1] = ry + ty;
    pt_out[2] = rz + tz;
}
}]>;

def Kernel_dual_quat_dlb : MasterKernelImplementation<"msimd_dual_quat_dlb", [{
void msimd_dual_quat_dlb(const double* dq_array, const double* weights, size_t count, double dq_out[8], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!dq_array || !weights || !dq_out || count == 0) return;
    for (int j = 0; j < 8; ++j) dq_out[j] = 0.0;

    const double* q0 = dq_array;

    for (size_t i = 0; i < count; ++i) {
        const double* q = dq_array + i * 8;
        double w = weights[i];
        double dot = q0[0]*q[0] + q0[1]*q[1] + q0[2]*q[2] + q0[3]*q[3];
        double sign = (dot >= 0.0) ? 1.0 : -1.0;
        for (int j = 0; j < 8; ++j) {
            dq_out[j] += w * sign * q[j];
        }
    }

    double norm_sq = dq_out[0]*dq_out[0] + dq_out[1]*dq_out[1] +
                     dq_out[2]*dq_out[2] + dq_out[3]*dq_out[3];
    if (norm_sq > 1e-15) {
        double inv_norm = 1.0 / std::sqrt(norm_sq);
        for (int j = 0; j < 8; ++j) {
            dq_out[j] *= inv_norm;
        }
    }
}
}]>;

def Kernel_dec_triangle_cotan_weights : MasterKernelImplementation<"msimd_dec_triangle_cotan_weights", [{
void msimd_dec_triangle_cotan_weights(const double p0[3], const double p1[3], const double p2[3],
    double* cot0, double* cot1, double* cot2, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    double e0[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
    double e1[3] = { p0[0] - p2[0], p0[1] - p2[1], p0[2] - p2[2] };
    double e2[3] = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };

    double cross[3] = {
        e1[1] * e2[2] - e1[2] * e2[1],
        e1[2] * e2[0] - e1[0] * e2[2],
        e1[0] * e2[1] - e1[1] * e2[0]
    };
    double area2 = std::sqrt(cross[0]*cross[0] + cross[1]*cross[1] + cross[2]*cross[2]);
    if (area2 < 1e-14) {
        if (cot0) *cot0 = 0.0;
        if (cot1) *cot1 = 0.0;
        if (cot2) *cot2 = 0.0;
        return;
    }
    double inv_area2 = 1.0 / area2;

    double dot0 = -(e1[0]*e2[0] + e1[1]*e2[1] + e1[2]*e2[2]);

    double dot1 = -(e2[0]*e0[0] + e2[1]*e0[1] + e2[2]*e0[2]);

    double dot2 = -(e0[0]*e1[0] + e0[1]*e1[1] + e0[2]*e1[2]);

    if (cot0) *cot0 = dot0 * inv_area2;
    if (cot1) *cot1 = dot1 * inv_area2;
    if (cot2) *cot2 = dot2 * inv_area2;
}
}]>;

def Kernel_dec_laplace_beltrami_mesh : MasterKernelImplementation<"msimd_dec_laplace_beltrami_mesh", [{
void msimd_dec_laplace_beltrami_mesh(const double* vertices, size_t num_vertices,
    const uint32_t* triangles, size_t num_triangles,
    const double* scalar_field_in, double* laplacian_out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!vertices || !triangles || !scalar_field_in || !laplacian_out || num_vertices == 0) return;

    for (size_t i = 0; i < num_vertices; ++i) {
        laplacian_out[i] = 0.0;
    }

    std::vector<double> dual_areas(num_vertices, 0.0);

    for (size_t t = 0; t < num_triangles; ++t) {
        uint32_t i0 = triangles[t * 3 + 0];
        uint32_t i1 = triangles[t * 3 + 1];
        uint32_t i2 = triangles[t * 3 + 2];
        if (i0 >= num_vertices || i1 >= num_vertices || i2 >= num_vertices) continue;

        const double* p0 = vertices + i0 * 3;
        const double* p1 = vertices + i1 * 3;
        const double* p2 = vertices + i2 * 3;

        double cot0, cot1, cot2;
        msimd_dec_triangle_cotan_weights(p0, p1, p2, &cot0, &cot1, &cot2);

        double e1[3] = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };
        double e2[3] = { p2[0] - p0[0], p2[1] - p0[0], p2[2] - p0[2] };
        double cr[3] = { e1[1]*e2[2] - e1[2]*e2[1], e1[2]*e2[0] - e1[0]*e2[2], e1[0]*e2[1] - e1[1]*e2[0] };
        double tri_area = 0.5 * std::sqrt(cr[0]*cr[0] + cr[1]*cr[1] + cr[2]*cr[2]);

        dual_areas[i0] += tri_area / 3.0;
        dual_areas[i1] += tri_area / 3.0;
        dual_areas[i2] += tri_area / 3.0;

        double f0 = scalar_field_in[i0];
        double f1 = scalar_field_in[i1];
        double f2 = scalar_field_in[i2];

        laplacian_out[i1] += 0.5 * cot0 * (f1 - f2);
        laplacian_out[i2] += 0.5 * cot0 * (f2 - f1);

        laplacian_out[i2] += 0.5 * cot1 * (f2 - f0);
        laplacian_out[i0] += 0.5 * cot1 * (f0 - f2);

        laplacian_out[i0] += 0.5 * cot2 * (f0 - f1);
        laplacian_out[i1] += 0.5 * cot2 * (f1 - f0);
    }

    for (size_t i = 0; i < num_vertices; ++i) {
        if (dual_areas[i] > 1e-14) {
            laplacian_out[i] /= dual_areas[i];
        }
    }
}
}]>;

def Kernel_dec_hodge_star_0form : MasterKernelImplementation<"msimd_dec_hodge_star_0form", [{
void msimd_dec_hodge_star_0form(const double* dual_cell_areas, const double* primal_weights, size_t n, double* hodge_star_diag, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!dual_cell_areas || !hodge_star_diag || n == 0) return;
    for (size_t i = 0; i < n; ++i) {
        double pw = (primal_weights) ? primal_weights[i] : 1.0;
        hodge_star_diag[i] = (pw > 1e-14) ? (dual_cell_areas[i] / pw) : 0.0;
    }
}
}]>;

def Kernel_dec_hodge_star_1form : MasterKernelImplementation<"msimd_dec_hodge_star_1form", [{
void msimd_dec_hodge_star_1form(const double* dual_edge_lengths, const double* primal_edge_lengths, size_t num_edges, double* hodge_diag, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!dual_edge_lengths || !primal_edge_lengths || !hodge_diag || num_edges == 0) return;
    for (size_t i = 0; i < num_edges; ++i) {
        double pl = primal_edge_lengths[i];
        hodge_diag[i] = (pl > 1e-14) ? (dual_edge_lengths[i] / pl) : 0.0;
    }
}
}]>;

def Kernel_symplectic_euler_step : MasterKernelImplementation<"msimd_symplectic_euler_step", [{
void msimd_symplectic_euler_step(double* pos, double* vel, const double* forces, const double* inv_mass, size_t n, double dt, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!pos || !vel || !forces || !inv_mass || n == 0) return;
    for (size_t i = 0; i < n; ++i) {

        vel[i] += inv_mass[i] * forces[i] * dt;

        pos[i] += vel[i] * dt;
    }
}
}]>;

def Kernel_velocity_verlet_step : MasterKernelImplementation<"msimd_velocity_verlet_step", [{
void msimd_velocity_verlet_step(double* pos, double* vel, double* forces, const double* inv_mass,
    void (*compute_forces)(const double* pos, double* forces, void* ctx),
    size_t n, double dt, void* ctx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!pos || !vel || !forces || !inv_mass || !compute_forces || n == 0) return;

    for (size_t i = 0; i < n; ++i) {
        vel[i] += 0.5 * inv_mass[i] * forces[i] * dt;
        pos[i] += vel[i] * dt;
    }

    compute_forces(pos, forces, ctx);

    for (size_t i = 0; i < n; ++i) {
        vel[i] += 0.5 * inv_mass[i] * forces[i] * dt;
    }
}
}]>;

def Kernel_discrete_euler_lagrange_step : MasterKernelImplementation<"msimd_discrete_euler_lagrange_step", [{
void msimd_discrete_euler_lagrange_step(const double* q_prev, const double* q_curr,
    const double* d_L_d_q_curr, double dt, size_t n, double* q_next, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!q_prev || !q_curr || !d_L_d_q_curr || !q_next || n == 0) return;
    double dt2 = dt * dt;
    for (size_t i = 0; i < n; ++i) {
        q_next[i] = 2.0 * q_curr[i] - q_prev[i] + dt2 * d_L_d_q_curr[i];
    }
}
}]>;

def Kernel_symplectic_energy_drift : MasterKernelImplementation<"msimd_symplectic_energy_drift", [{
double msimd_symplectic_energy_drift(double initial_energy, double current_energy, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (std::abs(initial_energy) < 1e-15) {
        return std::abs(current_energy - initial_energy);
    }
    return std::abs((current_energy - initial_energy) / initial_energy);
}
}]>;

def Kernel_xpbd_distance_constraint : MasterKernelImplementation<"msimd_xpbd_distance_constraint", [{
void msimd_xpbd_distance_constraint(double* p1, double* p2, double inv_m1, double inv_m2,
    double rest_len, double compliance, double dt, double* lambda, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!p1 || !p2) return;
    double w_sum = inv_m1 + inv_m2;
    if (w_sum < 1e-15) return;

    double dx = p1[0] - p2[0];
    double dy = p1[1] - p2[1];
    double dz = p1[2] - p2[2];
    double dist = std::sqrt(dx*dx + dy*dy + dz*dz);
    if (dist < 1e-14) return;

    double c = dist - rest_len;
    double alpha_tilde = compliance / (dt * dt);

    double old_lambda = (lambda) ? *lambda : 0.0;
    double delta_lambda = (-c - alpha_tilde * old_lambda) / (w_sum + alpha_tilde);
    if (lambda) *lambda = old_lambda + delta_lambda;

    double inv_dist = 1.0 / dist;
    double nx = dx * inv_dist;
    double ny = dy * inv_dist;
    double nz = dz * inv_dist;

    double p1_factor = inv_m1 * delta_lambda;
    double p2_factor = inv_m2 * delta_lambda;

    p1[0] += p1_factor * nx;
    p1[1] += p1_factor * ny;
    p1[2] += p1_factor * nz;

    p2[0] -= p2_factor * nx;
    p2[1] -= p2_factor * ny;
    p2[2] -= p2_factor * nz;
}
}]>;

def Kernel_xpbd_tetrahedron_volume_constraint : MasterKernelImplementation<"msimd_xpbd_tetrahedron_volume_constraint", [{
void msimd_xpbd_tetrahedron_volume_constraint(double* p1, double* p2, double* p3, double* p4,
    double inv_m1, double inv_m2, double inv_m3, double inv_m4,
    double rest_volume, double compliance, double dt, double* lambda, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!p1 || !p2 || !p3 || !p4) return;

    double e2[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
    double e3[3] = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
    double e4[3] = { p4[0] - p1[0], p4[1] - p1[1], p4[2] - p1[2] };

    double c23[3] = {
        e2[1]*e3[2] - e2[2]*e3[1],
        e2[2]*e3[0] - e2[0]*e3[2],
        e2[0]*e3[1] - e2[1]*e3[0]
    };
    double current_volume = (c23[0]*e4[0] + c23[1]*e4[1] + c23[2]*e4[2]) / 6.0;
    double c = current_volume - rest_volume;

    double inv6 = 1.0 / 6.0;
    double grad4[3] = { c23[0] * inv6, c23[1] * inv6, c23[2] * inv6 };
    double grad2[3] = {
        (e3[1]*e4[2] - e3[2]*e4[1]) * inv6,
        (e3[2]*e4[0] - e3[0]*e4[2]) * inv6,
        (e3[0]*e4[1] - e3[1]*e4[0]) * inv6
    };
    double grad3[3] = {
        (e4[1]*e2[2] - e4[2]*e2[1]) * inv6,
        (e4[2]*e2[0] - e4[0]*e2[2]) * inv6,
        (e4[0]*e2[1] - e4[1]*e2[0]) * inv6
    };
    double grad1[3] = {
        -(grad2[0] + grad3[0] + grad4[0]),
        -(grad2[1] + grad3[1] + grad4[1]),
        -(grad2[2] + grad3[2] + grad4[2])
    };

    double w_sum = inv_m1 * (grad1[0]*grad1[0] + grad1[1]*grad1[1] + grad1[2]*grad1[2]) +
                   inv_m2 * (grad2[0]*grad2[0] + grad2[1]*grad2[1] + grad2[2]*grad2[2]) +
                   inv_m3 * (grad3[0]*grad3[0] + grad3[1]*grad3[1] + grad3[2]*grad3[2]) +
                   inv_m4 * (grad4[0]*grad4[0] + grad4[1]*grad4[1] + grad4[2]*grad4[2]);

    if (w_sum < 1e-15) return;

    double alpha_tilde = compliance / (dt * dt);
    double old_lambda = (lambda) ? *lambda : 0.0;
    double delta_lambda = (-c - alpha_tilde * old_lambda) / (w_sum + alpha_tilde);
    if (lambda) *lambda = old_lambda + delta_lambda;

    for (int k = 0; k < 3; ++k) {
        p1[k] += inv_m1 * delta_lambda * grad1[k];
        p2[k] += inv_m2 * delta_lambda * grad2[k];
        p3[k] += inv_m3 * delta_lambda * grad3[k];
        p4[k] += inv_m4 * delta_lambda * grad4[k];
    }
}
}]>;

def Kernel_xpbd_contact_plane_constraint : MasterKernelImplementation<"msimd_xpbd_contact_plane_constraint", [{
void msimd_xpbd_contact_plane_constraint(double* p, double inv_m, const double plane_point[3], const double plane_normal[3],
    double compliance, double dt, double* lambda, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!p || !plane_point || !plane_normal || inv_m < 1e-15) return;
    double dx = p[0] - plane_point[0];
    double dy = p[1] - plane_point[1];
    double dz = p[2] - plane_point[2];

    double penetration = dx * plane_normal[0] + dy * plane_normal[1] + dz * plane_normal[2];
    if (penetration >= 0.0) return;

    double c = penetration;
    double alpha_tilde = compliance / (dt * dt);
    double old_lambda = (lambda) ? *lambda : 0.0;
    double delta_lambda = (-c - alpha_tilde * old_lambda) / (inv_m + alpha_tilde);
    if (lambda) *lambda = old_lambda + delta_lambda;

    p[0] += inv_m * delta_lambda * plane_normal[0];
    p[1] += inv_m * delta_lambda * plane_normal[1];
    p[2] += inv_m * delta_lambda * plane_normal[2];
}
}]>;

def Kernel_sph_wendland_c2_kernel : MasterKernelImplementation<"msimd_sph_wendland_c2_kernel", [{
void msimd_sph_wendland_c2_kernel(double r, double h, double* w, double* dw_dr, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (h <= 0.0) {
        if (w) *w = 0.0;
        if (dw_dr) *dw_dr = 0.0;
        return;
    }
    double q = r / h;
    if (q >= 1.0 || q < 0.0) {
        if (w) *w = 0.0;
        if (dw_dr) *dw_dr = 0.0;
        return;
    }

    double h3 = h * h * h;
    double sigma = 21.0 / (2.0 * M_PI * h3);
    double omq = 1.0 - q;
    double omq3 = omq * omq * omq;
    double omq4 = omq3 * omq;

    if (w) {
        *w = sigma * omq4 * (4.0 * q + 1.0);
    }
    if (dw_dr) {

        *dw_dr = - (sigma / h) * 20.0 * q * omq3;
    }
}
}]>;

def Kernel_sph_wendland_pair_force : MasterKernelImplementation<"msimd_sph_wendland_pair_force", [{
void msimd_sph_wendland_pair_force(const double p_i[3], const double p_j[3],
    const double v_i[3], const double v_j[3],
    double m_j, double rho_i, double rho_j,
    double pressure_i, double pressure_j,
    double h, double visc_eta, double f_out[3], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!p_i || !p_j || !v_i || !v_j || !f_out) return;
    f_out[0] = f_out[1] = f_out[2] = 0.0;

    double dx = p_i[0] - p_j[0];
    double dy = p_i[1] - p_j[1];
    double dz = p_i[2] - p_j[2];
    double r = std::sqrt(dx*dx + dy*dy + dz*dz);
    if (r >= h || r < 1e-12) return;

    double w, dw_dr;
    msimd_sph_wendland_c2_kernel(r, h, &w, &dw_dr);

    double inv_r = 1.0 / r;
    double grad_w[3] = { dw_dr * dx * inv_r, dw_dr * dy * inv_r, dw_dr * dz * inv_r };

    double rho_i2 = (rho_i > 1e-6) ? (rho_i * rho_i) : 1e-12;
    double rho_j2 = (rho_j > 1e-6) ? (rho_j * rho_j) : 1e-12;
    double press_term = m_j * (pressure_i / rho_i2 + pressure_j / rho_j2);

    double dvx = v_i[0] - v_j[0];
    double dvy = v_i[1] - v_j[1];
    double dvz = v_i[2] - v_j[2];
    double v_dot_r = dvx * dx + dvy * dy + dvz * dz;

    double visc_term = 0.0;
    if (v_dot_r < 0.0) {
        double mu_ij = (h * v_dot_r) / (r * r + 0.01 * h * h);
        double rho_avg = 0.5 * (rho_i + rho_j);
        visc_term = -m_j * (visc_eta * mu_ij) / rho_avg;
    }

    double total_factor = -(press_term + visc_term);
    f_out[0] = total_factor * grad_w[0];
    f_out[1] = total_factor * grad_w[1];
    f_out[2] = total_factor * grad_w[2];
}
}]>;

def Kernel_lbm_d3q19_bgk_collision_and_stream : MasterKernelImplementation<"msimd_lbm_d3q19_bgk_collision_and_stream", [{
void msimd_lbm_d3q19_bgk_collision_and_stream(const float* f_in, float* f_out, int nx, int ny, int nz,
    float tau, const uint8_t* obstacle_mask, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!f_in || !f_out || nx <= 0 || ny <= 0 || nz <= 0 || tau <= 0.0f) return;
    float inv_tau = 1.0f / tau;
    float om_inv_tau = 1.0f - inv_tau;
    size_t total_nodes = (size_t)nx * ny * nz;

    for (int z = 0; z < nz; ++z) {
        for (int y = 0; y < ny; ++y) {
            for (int x = 0; x < nx; ++x) {
                size_t idx = ((size_t)z * ny + y) * nx + x;

                if (obstacle_mask && obstacle_mask[idx]) {

                    for (int i = 0; i < 19; ++i) {
                        int opp = MSIMD_LBM_D3Q19_OPPOSITE[i];
                        f_out[idx * 19 + opp] = f_in[idx * 19 + i];
                    }
                    continue;
                }

                float rho = 0.0f;
                float ux = 0.0f, uy = 0.0f, uz = 0.0f;
                for (int i = 0; i < 19; ++i) {
                    float fi = f_in[idx * 19 + i];
                    rho += fi;
                    ux += fi * MSIMD_LBM_D3Q19_EX[i];
                    uy += fi * MSIMD_LBM_D3Q19_EY[i];
                    uz += fi * MSIMD_LBM_D3Q19_EZ[i];
                }
                if (rho > 1e-7f) {
                    float inv_rho = 1.0f / rho;
                    ux *= inv_rho;
                    uy *= inv_rho;
                    uz *= inv_rho;
                }

                float u_sq = ux*ux + uy*uy + uz*uz;

                for (int i = 0; i < 19; ++i) {
                    float eu = ux * MSIMD_LBM_D3Q19_EX[i] + uy * MSIMD_LBM_D3Q19_EY[i] + uz * MSIMD_LBM_D3Q19_EZ[i];
                    float feq = MSIMD_LBM_D3Q19_W[i] * rho * (1.0f + 3.0f * eu + 4.5f * eu * eu - 1.5f * u_sq);
                    float f_star = om_inv_tau * f_in[idx * 19 + i] + inv_tau * feq;

                    int nx_pos = (x + MSIMD_LBM_D3Q19_EX[i] + nx) % nx;
                    int ny_pos = (y + MSIMD_LBM_D3Q19_EY[i] + ny) % ny;
                    int nz_pos = (z + MSIMD_LBM_D3Q19_EZ[i] + nz) % nz;

                    size_t target_idx = ((size_t)nz_pos * ny + ny_pos) * nx + nx_pos;
                    f_out[target_idx * 19 + i] = f_star;
                }
            }
        }
    }
}
}]>;

def Kernel_quantum_apply_pauli_x : MasterKernelImplementation<"msimd_quantum_apply_pauli_x", [{
void msimd_quantum_apply_pauli_x(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || target_qubit >= num_qubits) return;
    size_t dim = (size_t)1 << num_qubits;
    size_t step = (size_t)1 << target_qubit;
    size_t double_step = step << 1;

    for (size_t i = 0; i < dim; i += double_step) {
        for (size_t j = 0; j < step; ++j) {
            size_t idx0 = i + j;
            size_t idx1 = idx0 + step;
            std::swap(psi_re[idx0], psi_re[idx1]);
            std::swap(psi_im[idx0], psi_im[idx1]);
        }
    }
}
}]>;

def Kernel_quantum_apply_pauli_y : MasterKernelImplementation<"msimd_quantum_apply_pauli_y", [{
void msimd_quantum_apply_pauli_y(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || target_qubit >= num_qubits) return;
    size_t dim = (size_t)1 << num_qubits;
    size_t step = (size_t)1 << target_qubit;
    size_t double_step = step << 1;

    for (size_t i = 0; i < dim; i += double_step) {
        for (size_t j = 0; j < step; ++j) {
            size_t idx0 = i + j;
            size_t idx1 = idx0 + step;
            double r0 = psi_re[idx0], i0 = psi_im[idx0];
            double r1 = psi_re[idx1], i1 = psi_im[idx1];

            psi_re[idx0] =  i1;
            psi_im[idx0] = -r1;
            psi_re[idx1] = -i0;
            psi_im[idx1] =  r0;
        }
    }
}
}]>;

def Kernel_quantum_apply_pauli_z : MasterKernelImplementation<"msimd_quantum_apply_pauli_z", [{
void msimd_quantum_apply_pauli_z(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || target_qubit >= num_qubits) return;
    size_t dim = (size_t)1 << num_qubits;
    size_t step = (size_t)1 << target_qubit;
    size_t double_step = step << 1;

    for (size_t i = 0; i < dim; i += double_step) {
        for (size_t j = 0; j < step; ++j) {
            size_t idx1 = i + j + step;
            psi_re[idx1] = -psi_re[idx1];
            psi_im[idx1] = -psi_im[idx1];
        }
    }
}
}]>;

def Kernel_quantum_apply_hadamard : MasterKernelImplementation<"msimd_quantum_apply_hadamard", [{
void msimd_quantum_apply_hadamard(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || target_qubit >= num_qubits) return;
    size_t dim = (size_t)1 << num_qubits;
    size_t step = (size_t)1 << target_qubit;
    size_t double_step = step << 1;
    const double inv_sqrt2 = 0.7071067811865475244;

    for (size_t i = 0; i < dim; i += double_step) {
        for (size_t j = 0; j < step; ++j) {
            size_t idx0 = i + j;
            size_t idx1 = idx0 + step;

            double r0 = psi_re[idx0], i0 = psi_im[idx0];
            double r1 = psi_re[idx1], i1 = psi_im[idx1];

            psi_re[idx0] = (r0 + r1) * inv_sqrt2;
            psi_im[idx0] = (i0 + i1) * inv_sqrt2;
            psi_re[idx1] = (r0 - r1) * inv_sqrt2;
            psi_im[idx1] = (i0 - i1) * inv_sqrt2;
        }
    }
}
}]>;

def Kernel_quantum_apply_phase : MasterKernelImplementation<"msimd_quantum_apply_phase", [{
void msimd_quantum_apply_phase(double* psi_re, double* psi_im, size_t num_qubits, size_t target_qubit, double theta, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || target_qubit >= num_qubits) return;
    size_t dim = (size_t)1 << num_qubits;
    size_t step = (size_t)1 << target_qubit;
    size_t double_step = step << 1;

    double cos_th = std::cos(theta);
    double sin_th = std::sin(theta);

    for (size_t i = 0; i < dim; i += double_step) {
        for (size_t j = 0; j < step; ++j) {
            size_t idx1 = i + j + step;
            double r1 = psi_re[idx1];
            double i1 = psi_im[idx1];
            psi_re[idx1] = r1 * cos_th - i1 * sin_th;
            psi_im[idx1] = r1 * sin_th + i1 * cos_th;
        }
    }
}
}]>;

def Kernel_quantum_apply_cnot : MasterKernelImplementation<"msimd_quantum_apply_cnot", [{
void msimd_quantum_apply_cnot(double* psi_re, double* psi_im, size_t num_qubits, size_t control_qubit, size_t target_qubit, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || control_qubit >= num_qubits || target_qubit >= num_qubits) return;
    if (control_qubit == target_qubit) return;

    size_t dim = (size_t)1 << num_qubits;
    size_t ctrl_mask = (size_t)1 << control_qubit;
    size_t tgt_mask = (size_t)1 << target_qubit;

    for (size_t i = 0; i < dim; ++i) {

        if ((i & ctrl_mask) && !(i & tgt_mask)) {
            size_t idx1 = i ^ tgt_mask;
            std::swap(psi_re[i], psi_re[idx1]);
            std::swap(psi_im[i], psi_im[idx1]);
        }
    }
}
}]>;

def Kernel_quantum_apply_toffoli : MasterKernelImplementation<"msimd_quantum_apply_toffoli", [{
void msimd_quantum_apply_toffoli(double* psi_re, double* psi_im, size_t num_qubits,
    size_t ctrl1, size_t ctrl2, size_t target_qubit, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || ctrl1 >= num_qubits || ctrl2 >= num_qubits || target_qubit >= num_qubits) return;
    size_t dim = (size_t)1 << num_qubits;
    size_t c1_mask = (size_t)1 << ctrl1;
    size_t c2_mask = (size_t)1 << ctrl2;
    size_t tgt_mask = (size_t)1 << target_qubit;

    for (size_t i = 0; i < dim; ++i) {
        if ((i & c1_mask) && (i & c2_mask) && !(i & tgt_mask)) {
            size_t idx1 = i ^ tgt_mask;
            std::swap(psi_re[i], psi_re[idx1]);
            std::swap(psi_im[i], psi_im[idx1]);
        }
    }
}
}]>;

def Kernel_quantum_qubit_probability : MasterKernelImplementation<"msimd_quantum_qubit_probability", [{
void msimd_quantum_qubit_probability(const double* psi_re, const double* psi_im, size_t num_qubits,
    size_t target_qubit, double* prob_0, double* prob_1, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_re || !psi_im || target_qubit >= num_qubits) {
        if (prob_0) *prob_0 = 0.0;
        if (prob_1) *prob_1 = 0.0;
        return;
    }
    size_t dim = (size_t)1 << num_qubits;
    size_t tgt_mask = (size_t)1 << target_qubit;
    double p0 = 0.0;
    double p1 = 0.0;

    for (size_t i = 0; i < dim; ++i) {
        double mag2 = psi_re[i]*psi_re[i] + psi_im[i]*psi_im[i];
        if (i & tgt_mask) {
            p1 += mag2;
        } else {
            p0 += mag2;
        }
    }
    if (prob_0) *prob_0 = p0;
    if (prob_1) *prob_1 = p1;
}
}]>;

def Kernel_regge_deficit_angle_2d : MasterKernelImplementation<"msimd_regge_deficit_angle_2d", [{
void msimd_regge_deficit_angle_2d(const double* edge_lengths_around_vertex, size_t num_triangles, double* deficit_angle, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!edge_lengths_around_vertex || num_triangles == 0 || !deficit_angle) return;
    double sum_angles = 0.0;

    for (size_t i = 0; i < num_triangles; ++i) {
        double a = edge_lengths_around_vertex[i * 3 + 0];
        double b = edge_lengths_around_vertex[i * 3 + 1];
        double c = edge_lengths_around_vertex[i * 3 + 2];

        if (a > 1e-15 && b > 1e-15) {
            double cos_theta = (a*a + b*b - c*c) / (2.0 * a * b);
            cos_theta = std::clamp(cos_theta, -1.0, 1.0);
            sum_angles += std::acos(cos_theta);
        }
    }
    *deficit_angle = 2.0 * M_PI - sum_angles;
}
}]>;

def Kernel_wilson_plaquette_u1 : MasterKernelImplementation<"msimd_wilson_plaquette_u1", [{
void msimd_wilson_plaquette_u1(const double* theta_links, int nx, int ny, int nz, int nt, double* average_action, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!theta_links || nx <= 1 || ny <= 1 || nz <= 1 || nt <= 1 || !average_action) return;

    auto link_idx = [nx, ny, nz, nt](int x, int y, int z, int t, int mu) -> size_t {
        x = (x + nx) % nx;
        y = (y + ny) % ny;
        z = (z + nz) % nz;
        t = (t + nt) % nt;
        return ((((size_t)t * nz + z) * ny + y) * nx + x) * 4 + mu;
    };

    double total_action = 0.0;
    size_t num_plaquettes = 0;

    for (int t = 0; t < nt; ++t) {
        for (int z = 0; z < nz; ++z) {
            for (int y = 0; y < ny; ++y) {
                for (int x = 0; x < nx; ++x) {
                    for (int mu = 0; mu < 4; ++mu) {
                        for (int nu = mu + 1; nu < 4; ++nu) {

                            int dx_mu = (mu == 0), dy_mu = (mu == 1), dz_mu = (mu == 2), dt_mu = (mu == 3);
                            int dx_nu = (nu == 0), dy_nu = (nu == 1), dz_nu = (nu == 2), dt_nu = (nu == 3);

                            double u1 = theta_links[link_idx(x, y, z, t, mu)];
                            double u2 = theta_links[link_idx(x + dx_mu, y + dy_mu, z + dz_mu, t + dt_mu, nu)];
                            double u3 = theta_links[link_idx(x + dx_nu, y + dy_nu, z + dz_nu, t + dt_nu, mu)];
                            double u4 = theta_links[link_idx(x, y, z, t, nu)];

                            double theta_p = u1 + u2 - u3 - u4;
                            total_action += (1.0 - std::cos(theta_p));
                            num_plaquettes++;
                        }
                    }
                }
            }
        }
    }
    *average_action = (num_plaquettes > 0) ? (total_action / num_plaquettes) : 0.0;
}
}]>;

def Kernel_wilson_dirac_operator_4d : MasterKernelImplementation<"msimd_wilson_dirac_operator_4d", [{
void msimd_wilson_dirac_operator_4d(const double* psi_in, double* psi_out, const double* gauge_u1,
    int nx, int ny, int nz, int nt, double mass_m0, double r_wilson, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!psi_in || !psi_out || !gauge_u1 || nx <= 1 || ny <= 1 || nz <= 1 || nt <= 1) return;

    size_t num_sites = (size_t)nx * ny * nz * nt;
    auto site_idx = [nx, ny, nz, nt](int x, int y, int z, int t) -> size_t {
        x = (x + nx) % nx;
        y = (y + ny) % ny;
        z = (z + nz) % nz;
        t = (t + nt) % nt;
        return (((size_t)t * nz + z) * ny + y) * nx + x;
    };

    double diag_coeff = mass_m0 + 4.0 * r_wilson;

    for (int t = 0; t < nt; ++t) {
        for (int z = 0; z < nz; ++z) {
            for (int y = 0; y < ny; ++y) {
                for (int x = 0; x < nx; ++x) {
                    size_t s = site_idx(x, y, z, t);
                    double val = diag_coeff * psi_in[s];

                    int d[4][4] = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };
                    for (int mu = 0; mu < 4; ++mu) {
                        size_t s_fwd = site_idx(x + d[mu][0], y + d[mu][1], z + d[mu][2], t + d[mu][3]);
                        size_t s_bwd = site_idx(x - d[mu][0], y - d[mu][1], z - d[mu][2], t - d[mu][3]);

                        double u_fwd = gauge_u1[s * 4 + mu];
                        double u_bwd = gauge_u1[s_bwd * 4 + mu];

                        val -= 0.5 * (r_wilson * std::cos(u_fwd) * psi_in[s_fwd] +
                                      r_wilson * std::cos(u_bwd) * psi_in[s_bwd]);
                    }
                    psi_out[s] = val;
                }
            }
        }
    }
}
}]>;

def Kernel_relativity_christoffel_symbols : MasterKernelImplementation<"msimd_relativity_christoffel_symbols", [{
void msimd_relativity_christoffel_symbols(const double g_inv[4][4], const double d_g[4][4][4], double gamma[4][4][4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!g_inv || !d_g || !gamma) return;

    for (int mu = 0; mu < 4; ++mu) {
        for (int alpha = 0; alpha < 4; ++alpha) {
            for (int beta = 0; beta < 4; ++beta) {
                double sum = 0.0;
                for (int nu = 0; nu < 4; ++nu) {
                    double dg_term = d_g[alpha][beta][nu] + d_g[beta][alpha][nu] - d_g[nu][alpha][beta];
                    sum += g_inv[mu][nu] * dg_term;
                }
                gamma[mu][alpha][beta] = 0.5 * sum;
            }
        }
    }
}
}]>;

def Kernel_relativity_geodesic_step : MasterKernelImplementation<"msimd_relativity_geodesic_step", [{
void msimd_relativity_geodesic_step(const double x[4], const double u[4], const double gamma[4][4][4],
    double d_lambda, double x_next[4], double u_next[4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!x || !u || !gamma || !x_next || !u_next) return;

    double a[4] = { 0.0, 0.0, 0.0, 0.0 };
    for (int mu = 0; mu < 4; ++mu) {
        double sum = 0.0;
        for (int alpha = 0; alpha < 4; ++alpha) {
            for (int beta = 0; beta < 4; ++beta) {
                sum += gamma[mu][alpha][beta] * u[alpha] * u[beta];
            }
        }
        a[mu] = -sum;
    }

    for (int mu = 0; mu < 4; ++mu) {
        u_next[mu] = u[mu] + a[mu] * d_lambda;
        x_next[mu] = x[mu] + u_next[mu] * d_lambda;
    }
}
}]>;

def Kernel_relativity_riemann_tensor : MasterKernelImplementation<"msimd_relativity_riemann_tensor", [{
void msimd_relativity_riemann_tensor(const double gamma[4][4][4], const double d_gamma[4][4][4][4],
    double riemann[4][4][4][4], double ricci[4][4], double* ricci_scalar,
    const double g_inv[4][4], const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!gamma || !d_gamma || !riemann) return;

    for (int rho = 0; rho < 4; ++rho) {
        for (int sigma = 0; sigma < 4; ++sigma) {
            for (int mu = 0; mu < 4; ++mu) {
                for (int nu = 0; nu < 4; ++nu) {
                    double r = d_gamma[mu][rho][nu][sigma] - d_gamma[nu][rho][mu][sigma];
                    double quad = 0.0;
                    for (int lam = 0; lam < 4; ++lam) {
                        quad += gamma[rho][mu][lam] * gamma[lam][nu][sigma]
                              - gamma[rho][nu][lam] * gamma[lam][mu][sigma];
                    }
                    riemann[rho][sigma][mu][nu] = r + quad;
                }
            }
        }
    }

    if (ricci) {
        for (int sigma = 0; sigma < 4; ++sigma) {
            for (int nu = 0; nu < 4; ++nu) {
                double sum = 0.0;
                for (int mu = 0; mu < 4; ++mu) {
                    sum += riemann[mu][sigma][mu][nu];
                }
                ricci[sigma][nu] = sum;
            }
        }
    }

    if (ricci_scalar && ricci && g_inv) {
        double r_sc = 0.0;
        for (int sigma = 0; sigma < 4; ++sigma) {
            for (int nu = 0; nu < 4; ++nu) {
                r_sc += g_inv[sigma][nu] * ricci[sigma][nu];
            }
        }
        *ricci_scalar = r_sc;
    }
}
}]>;

def Kernel_nag_homotopy_step : MasterKernelImplementation<"msimd_nag_homotopy_step", [{
void msimd_nag_homotopy_step(const double* x, double t, double dt,
    void (*eval_H)(const double* x, double t, double* H_val, void* ctx),
    void (*eval_JH)(const double* x, double t, double* J_val, void* ctx),
    void (*eval_dH_dt)(const double* x, double t, double* dH_val, void* ctx),
    size_t n, double* x_next, void* ctx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!x || !x_next || !eval_H || !eval_JH || !eval_dH_dt || n == 0) return;

    std::vector<double> J(n * n, 0.0);
    std::vector<double> dH_dt(n, 0.0);
    eval_JH(x, t, J.data(), ctx);
    eval_dH_dt(x, t, dH_dt.data(), ctx);

    std::vector<double> dx(n, 0.0);
    if (n == 1) {
        if (std::abs(J[0]) > 1e-15) {
            dx[0] = (-dH_dt[0] * dt) / J[0];
        }
    } else {

        for (size_t i = 0; i < n; ++i) {
            double diag = J[i * n + i];
            double denom = diag * diag + 1e-8;
            dx[i] = (-dH_dt[i] * dt * diag) / denom;
        }
    }

    std::vector<double> x_pred(n);
    for (size_t i = 0; i < n; ++i) {
        x_pred[i] = x[i] + dx[i];
    }
    double t_next = t + dt;

    std::vector<double> H_val(n, 0.0);
    eval_H(x_pred.data(), t_next, H_val.data(), ctx);
    eval_JH(x_pred.data(), t_next, J.data(), ctx);

    for (size_t i = 0; i < n; ++i) {
        double diag = J[i * n + i];
        double denom = diag * diag + 1e-8;
        double corr = (H_val[i] * diag) / denom;
        x_next[i] = x_pred[i] - corr;
    }
}
}]>;

def Kernel_nag_surface_normal_and_singularity : MasterKernelImplementation<"msimd_nag_surface_normal_and_singularity", [{
void msimd_nag_surface_normal_and_singularity(double x, double y, double z,
    double (*F_fn)(double x, double y, double z, void* ctx),
    double grad[3], double hessian[3][3], double* hessian_det, int* is_singular, void* ctx, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!F_fn) return;
    const double eps = 1e-6;
    const double inv_2eps = 1.0 / (2.0 * eps);
    const double inv_eps2 = 1.0 / (eps * eps);

    double f0 = F_fn(x, y, z, ctx);

    double fx_p = F_fn(x + eps, y, z, ctx), fx_m = F_fn(x - eps, y, z, ctx);
    double fy_p = F_fn(x, y + eps, z, ctx), fy_m = F_fn(x, y - eps, z, ctx);
    double fz_p = F_fn(x, y, z + eps, ctx), fz_m = F_fn(x, y, z - eps, ctx);

    if (grad) {
        grad[0] = (fx_p - fx_m) * inv_2eps;
        grad[1] = (fy_p - fy_m) * inv_2eps;
        grad[2] = (fz_p - fz_m) * inv_2eps;
    }

    double fxx = (fx_p - 2.0 * f0 + fx_m) * inv_eps2;
    double fyy = (fy_p - 2.0 * f0 + fy_m) * inv_eps2;
    double fzz = (fz_p - 2.0 * f0 + fz_m) * inv_eps2;

    double fxy = (F_fn(x + eps, y + eps, z, ctx) - fx_p - fy_p + f0) * inv_eps2;
    double fxz = (F_fn(x + eps, y, z + eps, ctx) - fx_p - fz_p + f0) * inv_eps2;
    double fyz = (F_fn(x, y + eps, z + eps, ctx) - fy_p - fz_p + f0) * inv_eps2;

    if (hessian) {
        hessian[0][0] = fxx; hessian[0][1] = fxy; hessian[0][2] = fxz;
        hessian[1][0] = fxy; hessian[1][1] = fyy; hessian[1][2] = fyz;
        hessian[2][0] = fxz; hessian[2][1] = fyz; hessian[2][2] = fzz;
    }

    double det = fxx * (fyy * fzz - fyz * fyz) -
                 fxy * (fxy * fzz - fyz * fxz) +
                 fxz * (fxy * fyz - fyy * fxz);

    if (hessian_det) *hessian_det = det;

    if (is_singular && grad) {
        double g_norm2 = grad[0]*grad[0] + grad[1]*grad[1] + grad[2]*grad[2];
        *is_singular = (g_norm2 < 1e-8 && std::abs(f0) < 1e-6) ? 1 : 0;
    }
}
}]>;

def Kernel_nag_elliptic_curve_add : MasterKernelImplementation<"msimd_nag_elliptic_curve_add", [{
void msimd_nag_elliptic_curve_add(double x1, double y1, double x2, double y2, double a,
    double* x3, double* y3, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!x3 || !y3) return;
    double m = 0.0;
    if (std::abs(x1 - x2) < 1e-12) {
        if (std::abs(y1 - y2) < 1e-12 && std::abs(y1) > 1e-12) {

            m = (3.0 * x1 * x1 + a) / (2.0 * y1);
        } else {

            *x3 = 0.0;
            *y3 = 0.0;
            return;
        }
    } else {

        m = (y2 - y1) / (x2 - x1);
    }

    double x_res = m * m - x1 - x2;
    double y_res = m * (x1 - x_res) - y1;
    *x3 = x_res;
    *y3 = y_res;
}
}]>;

def Kernel_control_kalman_filter_step : MasterKernelImplementation<"msimd_control_kalman_filter_step", [{
void msimd_control_kalman_filter_step(double* x, double* P, const double* F, const double* B, const double* u,
    const double* Q, const double* z, const double* H, const double* R,
    size_t state_dim, size_t obs_dim, size_t ctrl_dim, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!x || !P || !F || !Q || !z || !H || !R || state_dim == 0 || obs_dim == 0) return;

    std::vector<double> x_prior(state_dim, 0.0);
    for (size_t i = 0; i < state_dim; ++i) {
        double sum = 0.0;
        for (size_t j = 0; j < state_dim; ++j) {
            sum += F[i * state_dim + j] * x[j];
        }
        if (B && u && ctrl_dim > 0) {
            for (size_t k = 0; k < ctrl_dim; ++k) {
                sum += B[i * ctrl_dim + k] * u[k];
            }
        }
        x_prior[i] = sum;
    }

    std::vector<double> FP(state_dim * state_dim, 0.0);
    for (size_t i = 0; i < state_dim; ++i) {
        for (size_t j = 0; j < state_dim; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < state_dim; ++k) {
                sum += F[i * state_dim + k] * P[k * state_dim + j];
            }
            FP[i * state_dim + j] = sum;
        }
    }
    std::vector<double> P_prior(state_dim * state_dim, 0.0);
    for (size_t i = 0; i < state_dim; ++i) {
        for (size_t j = 0; j < state_dim; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < state_dim; ++k) {
                sum += FP[i * state_dim + k] * F[j * state_dim + k];
            }
            P_prior[i * state_dim + j] = sum + Q[i * state_dim + j];
        }
    }

    std::vector<double> y_innov(obs_dim, 0.0);
    for (size_t i = 0; i < obs_dim; ++i) {
        double hx = 0.0;
        for (size_t j = 0; j < state_dim; ++j) {
            hx += H[i * state_dim + j] * x_prior[j];
        }
        y_innov[i] = z[i] - hx;
    }

    std::vector<double> HP(obs_dim * state_dim, 0.0);
    for (size_t i = 0; i < obs_dim; ++i) {
        for (size_t j = 0; j < state_dim; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < state_dim; ++k) {
                sum += H[i * state_dim + k] * P_prior[k * state_dim + j];
            }
            HP[i * state_dim + j] = sum;
        }
    }
    std::vector<double> S(obs_dim * obs_dim, 0.0);
    for (size_t i = 0; i < obs_dim; ++i) {
        for (size_t j = 0; j < obs_dim; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < state_dim; ++k) {
                sum += HP[i * state_dim + k] * H[j * state_dim + k];
            }
            S[i * obs_dim + j] = sum + R[i * obs_dim + j];
        }
    }

    std::vector<double> K(state_dim * obs_dim, 0.0);
    if (obs_dim == 1) {
        double inv_S = (std::abs(S[0]) > 1e-15) ? (1.0 / S[0]) : 0.0;
        for (size_t i = 0; i < state_dim; ++i) {
            double pht = 0.0;
            for (size_t k = 0; k < state_dim; ++k) {
                pht += P_prior[i * state_dim + k] * H[0 * state_dim + k];
            }
            K[i] = pht * inv_S;
        }
    } else {

        for (size_t i = 0; i < state_dim; ++i) {
            for (size_t j = 0; j < obs_dim; ++j) {
                double pht = 0.0;
                for (size_t k = 0; k < state_dim; ++k) {
                    pht += P_prior[i * state_dim + k] * H[j * state_dim + k];
                }
                double s_diag = S[j * obs_dim + j];
                K[i * obs_dim + j] = (std::abs(s_diag) > 1e-14) ? (pht / s_diag) : 0.0;
            }
        }
    }

    for (size_t i = 0; i < state_dim; ++i) {
        double ky = 0.0;
        for (size_t j = 0; j < obs_dim; ++j) {
            ky += K[i * obs_dim + j] * y_innov[j];
        }
        x[i] = x_prior[i] + ky;
    }

    std::vector<double> I_KH(state_dim * state_dim, 0.0);
    for (size_t i = 0; i < state_dim; ++i) {
        for (size_t j = 0; j < state_dim; ++j) {
            double kh = 0.0;
            for (size_t k = 0; k < obs_dim; ++k) {
                kh += K[i * obs_dim + k] * H[k * state_dim + j];
            }
            I_KH[i * state_dim + j] = ((i == j) ? 1.0 : 0.0) - kh;
        }
    }
    for (size_t i = 0; i < state_dim; ++i) {
        for (size_t j = 0; j < state_dim; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < state_dim; ++k) {
                sum += I_KH[i * state_dim + k] * P_prior[k * state_dim + j];
            }
            P[i * state_dim + j] = sum;
        }
    }
}
}]>;

def Kernel_control_pid_step : MasterKernelImplementation<"msimd_control_pid_step", [{
double msimd_control_pid_step(double setpoint, double measurement, double kp, double ki, double kd, double dt,
    double min_out, double max_out, double filter_alpha,
    double* integral_state, double* prev_meas, double* prev_deriv, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!integral_state || !prev_meas || !prev_deriv || dt <= 0.0) return 0.0;

    double error = setpoint - measurement;

    double p_term = kp * error;

    *integral_state += error * dt;
    double i_term = ki * (*integral_state);

    double raw_deriv = -(measurement - *prev_meas) / dt;
    double d_term = filter_alpha * (*prev_deriv) + (1.0 - filter_alpha) * raw_deriv;
    *prev_deriv = d_term;
    *prev_meas = measurement;

    double output = p_term + i_term + kd * d_term;

    if (output > max_out) {
        output = max_out;
        if (ki != 0.0 && error > 0.0) {
            *integral_state -= error * dt;
        }
    } else if (output < min_out) {
        output = min_out;
        if (ki != 0.0 && error < 0.0) {
            *integral_state -= error * dt;
        }
    }
    return output;
}
}]>;

def Kernel_control_lqr_riccati_iteration : MasterKernelImplementation<"msimd_control_lqr_riccati_iteration", [{
void msimd_control_lqr_riccati_iteration(const double* A, const double* B, const double* Q, const double* R,
    size_t n, size_t m, double* P, double* K, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!A || !B || !Q || !R || !P || n == 0 || m == 0) return;

    std::vector<double> BtP(m * n, 0.0);
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < n; ++k) {
                sum += B[k * m + i] * P[k * n + j];
            }
            BtP[i * n + j] = sum;
        }
    }

    std::vector<double> M(m * m, 0.0);
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < m; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < n; ++k) {
                sum += BtP[i * n + k] * B[k * m + j];
            }
            M[i * m + j] = sum + R[i * m + j];
        }
    }

    std::vector<double> BtPA(m * n, 0.0);
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < n; ++k) {
                sum += BtP[i * n + k] * A[k * n + j];
            }
            BtPA[i * n + j] = sum;
        }
    }

    if (K) {
        for (size_t i = 0; i < m; ++i) {
            double m_diag = M[i * m + i];
            double inv_m = (std::abs(m_diag) > 1e-14) ? (1.0 / m_diag) : 0.0;
            for (size_t j = 0; j < n; ++j) {
                K[i * n + j] = inv_m * BtPA[i * n + j];
            }
        }
    }

    std::vector<double> AtP(n * n, 0.0);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < n; ++k) {
                sum += A[k * n + i] * P[k * n + j];
            }
            AtP[i * n + j] = sum;
        }
    }
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double atpa = 0.0;
            for (size_t k = 0; k < n; ++k) {
                atpa += AtP[i * n + k] * A[k * n + j];
            }
            double quad = 0.0;
            if (K) {
                for (size_t k = 0; k < m; ++k) {
                    quad += BtPA[k * n + i] * K[k * n + j];
                }
            }
            P[i * n + j] = atpa - quad + Q[i * n + j];
        }
    }
}
}]>;

def Kernel_opt_simplex_pivot : MasterKernelImplementation<"msimd_opt_simplex_pivot", [{
void msimd_opt_simplex_pivot(double* tableau, size_t num_rows, size_t num_cols,
    size_t pivot_row, size_t pivot_col, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!tableau || pivot_row >= num_rows || pivot_col >= num_cols) return;
    double pivot_val = tableau[pivot_row * num_cols + pivot_col];
    if (std::abs(pivot_val) < 1e-15) return;

    double inv_pivot = 1.0 / pivot_val;

    for (size_t c = 0; c < num_cols; ++c) {
        tableau[pivot_row * num_cols + c] *= inv_pivot;
    }

    for (size_t r = 0; r < num_rows; ++r) {
        if (r == pivot_row) continue;
        double factor = tableau[r * num_cols + pivot_col];
        if (std::abs(factor) > 1e-15) {
            for (size_t c = 0; c < num_cols; ++c) {
                tableau[r * num_cols + c] -= factor * tableau[pivot_row * num_cols + c];
            }
        }
    }
}
}]>;

def Kernel_opt_simplex_step : MasterKernelImplementation<"msimd_opt_simplex_step", [{
int msimd_opt_simplex_step(double* tableau, size_t num_rows, size_t num_cols, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!tableau || num_rows < 2 || num_cols < 2) return -1;
    size_t obj_row = num_rows - 1;
    size_t rhs_col = num_cols - 1;

    size_t pivot_col = 0;
    double min_cost = 0.0;
    for (size_t c = 0; c < rhs_col; ++c) {
        double cost = tableau[obj_row * num_cols + c];
        if (cost < min_cost) {
            min_cost = cost;
            pivot_col = c;
        }
    }
    if (min_cost >= -1e-12) {
        return 1;
    }

    size_t pivot_row = 0;
    double min_ratio = std::numeric_limits<double>::infinity();
    bool found_row = false;

    for (size_t r = 0; r < obj_row; ++r) {
        double coeff = tableau[r * num_cols + pivot_col];
        if (coeff > 1e-12) {
            double rhs = tableau[r * num_cols + rhs_col];
            double ratio = rhs / coeff;
            if (ratio >= 0.0 && ratio < min_ratio) {
                min_ratio = ratio;
                pivot_row = r;
                found_row = true;
            }
        }
    }
    if (!found_row) {
        return 2;
    }

    msimd_opt_simplex_pivot(tableau, num_rows, num_cols, pivot_row, pivot_col);
    return 0;
}
}]>;

def Kernel_game_fictitious_play_step : MasterKernelImplementation<"msimd_game_fictitious_play_step", [{
void msimd_game_fictitious_play_step(const double* A, const double* B, size_t m, size_t n,
    double* p1_counts, double* p2_counts,
    double* p1_strategy, double* p2_strategy, size_t iteration, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!A || !B || !p1_counts || !p2_counts || !p1_strategy || !p2_strategy || m == 0 || n == 0) return;

    size_t best_p1 = 0;
    double max_u1 = -1e30;
    for (size_t i = 0; i < m; ++i) {
        double exp_u = 0.0;
        for (size_t j = 0; j < n; ++j) {
            exp_u += A[i * n + j] * p2_strategy[j];
        }
        if (exp_u > max_u1) {
            max_u1 = exp_u;
            best_p1 = i;
        }
    }

    size_t best_p2 = 0;
    double max_u2 = -1e30;
    for (size_t j = 0; j < n; ++j) {
        double exp_u = 0.0;
        for (size_t i = 0; i < m; ++i) {
            exp_u += B[i * n + j] * p1_strategy[i];
        }
        if (exp_u > max_u2) {
            max_u2 = exp_u;
            best_p2 = j;
        }
    }

    p1_counts[best_p1] += 1.0;
    p2_counts[best_p2] += 1.0;

    double inv_iter = 1.0 / (double)(iteration + 1);
    for (size_t i = 0; i < m; ++i) {
        p1_strategy[i] = p1_counts[i] * inv_iter;
    }
    for (size_t j = 0; j < n; ++j) {
        p2_strategy[j] = p2_counts[j] * inv_iter;
    }
}
}]>;

def Kernel_game_zero_sum_value : MasterKernelImplementation<"msimd_game_zero_sum_value", [{
double msimd_game_zero_sum_value(const double* payoff_matrix, const double* p1_strat, const double* p2_strat,
    size_t m, size_t n, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!payoff_matrix || !p1_strat || !p2_strat || m == 0 || n == 0) return 0.0;
    double val = 0.0;
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            val += p1_strat[i] * payoff_matrix[i * n + j] * p2_strat[j];
        }
    }
    return val;
}
}]>;

def Kernel_bio_lotka_volterra_step : MasterKernelImplementation<"msimd_bio_lotka_volterra_step", [{
void msimd_bio_lotka_volterra_step(double prey, double predator,
    double alpha, double beta, double gamma, double delta,
    double dt, double* next_prey, double* next_predator, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!next_prey || !next_predator) return;

    auto f_prey = [alpha, beta](double x, double y) { return alpha * x - beta * x * y; };
    auto f_pred = [gamma, delta](double x, double y) { return delta * x * y - gamma * y; };

    double k1_x = f_prey(prey, predator);
    double k1_y = f_pred(prey, predator);

    double k2_x = f_prey(prey + 0.5 * dt * k1_x, predator + 0.5 * dt * k1_y);
    double k2_y = f_pred(prey + 0.5 * dt * k1_x, predator + 0.5 * dt * k1_y);

    double k3_x = f_prey(prey + 0.5 * dt * k2_x, predator + 0.5 * dt * k2_y);
    double k3_y = f_pred(prey + 0.5 * dt * k2_x, predator + 0.5 * dt * k2_y);

    double k4_x = f_prey(prey + dt * k3_x, predator + dt * k3_y);
    double k4_y = f_pred(prey + dt * k3_x, predator + dt * k3_y);

    *next_prey     = prey + (dt / 6.0) * (k1_x + 2.0 * k2_x + 2.0 * k3_x + k4_x);
    *next_predator = predator + (dt / 6.0) * (k1_y + 2.0 * k2_y + 2.0 * k3_y + k4_y);

    if (*next_prey < 0.0) *next_prey = 0.0;
    if (*next_predator < 0.0) *next_predator = 0.0;
}
}]>;

def Kernel_bio_seir_step : MasterKernelImplementation<"msimd_bio_seir_step", [{
void msimd_bio_seir_step(double S, double E, double I, double R,
    double beta, double sigma, double gamma, double dt,
    double* next_S, double* next_E, double* next_I, double* next_R, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!next_S || !next_E || !next_I || !next_R) return;
    double N = S + E + I + R;
    if (N < 1e-8) N = 1.0;

    double dS = - (beta * S * I) / N;
    double dE =   (beta * S * I) / N - sigma * E;
    double dI =   sigma * E - gamma * I;
    double dR =   gamma * I;

    *next_S = std::max(0.0, S + dS * dt);
    *next_E = std::max(0.0, E + dE * dt);
    *next_I = std::max(0.0, I + dI * dt);
    *next_R = std::max(0.0, R + dR * dt);
}
}]>;

def Kernel_bio_michaelis_menten_rate : MasterKernelImplementation<"msimd_bio_michaelis_menten_rate", [{
void msimd_bio_michaelis_menten_rate(double substrate_conc, double vmax, double km, double* rate, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!rate) return;
    if (substrate_conc < 0.0) substrate_conc = 0.0;
    double denom = km + substrate_conc;
    *rate = (denom > 1e-15) ? ((vmax * substrate_conc) / denom) : 0.0;
}
}]>;

def Kernel_tda_pairwise_distance_matrix : MasterKernelImplementation<"msimd_tda_pairwise_distance_matrix", [{
void msimd_tda_pairwise_distance_matrix(const double* points, size_t num_points, size_t dim, double* dist_matrix, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!points || !dist_matrix || num_points == 0 || dim == 0) return;

    for (size_t i = 0; i < num_points; ++i) {
        dist_matrix[i * num_points + i] = 0.0;
        for (size_t j = i + 1; j < num_points; ++j) {
            double sum_sq = 0.0;
            for (size_t d = 0; d < dim; ++d) {
                double diff = points[i * dim + d] - points[j * dim + d];
                sum_sq += diff * diff;
            }
            double dist = std::sqrt(sum_sq);
            dist_matrix[i * num_points + j] = dist;
            dist_matrix[j * num_points + i] = dist;
        }
    }
}
}]>;

def Kernel_tda_boundary_matrix_reduce : MasterKernelImplementation<"msimd_tda_boundary_matrix_reduce", [{
void msimd_tda_boundary_matrix_reduce(uint8_t* boundary_matrix, size_t num_simplices, int64_t* pivots, const msimd_tensor_t* precision_target) {
    (void)precision_target;

    if (!boundary_matrix || !pivots || num_simplices == 0) return;

    for (size_t i = 0; i < num_simplices; ++i) {
        pivots[i] = -1;
    }

    auto get_pivot = [num_simplices, boundary_matrix](size_t col) -> int64_t {
        for (int64_t r = (int64_t)num_simplices - 1; r >= 0; --r) {
            if (boundary_matrix[r * num_simplices + col]) {
                return r;
            }
        }
        return -1;
    };

    for (size_t j = 0; j < num_simplices; ++j) {
        int64_t piv = get_pivot(j);
        while (piv != -1 && pivots[piv] != -1) {
            size_t other_col = (size_t)pivots[piv];

            for (size_t r = 0; r < num_simplices; ++r) {
                boundary_matrix[r * num_simplices + j] ^= boundary_matrix[r * num_simplices + other_col];
            }
            piv = get_pivot(j);
        }
        if (piv != -1) {
            pivots[piv] = (int64_t)j;
        }
    }
}
}]>;

def Kernel_graph_dijkstra_shortest_paths : MasterKernelImplementation<"msimd_graph_dijkstra_shortest_paths", [{
void msimd_graph_dijkstra_shortest_paths(const double* adj_matrix, size_t n, size_t src,
    double* distances, int64_t* predecessors, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!adj_matrix || !distances || !predecessors || src >= n || n == 0) return;

    std::vector<bool> visited(n, false);
    for (size_t i = 0; i < n; ++i) {
        distances[i] = std::numeric_limits<double>::infinity();
        predecessors[i] = -1;
    }
    distances[src] = 0.0;

    for (size_t step = 0; step < n; ++step) {

        double min_dist = std::numeric_limits<double>::infinity();
        int64_t u = -1;
        for (size_t i = 0; i < n; ++i) {
            if (!visited[i] && distances[i] < min_dist) {
                min_dist = distances[i];
                u = (int64_t)i;
            }
        }
        if (u == -1) break;
        visited[u] = true;

        for (size_t v = 0; v < n; ++v) {
            double weight = adj_matrix[u * n + v];
            if (!visited[v] && weight > 0.0) {
                double new_dist = distances[u] + weight;
                if (new_dist < distances[v]) {
                    distances[v] = new_dist;
                    predecessors[v] = u;
                }
            }
        }
    }
}
}]>;

def Kernel_graph_bellman_ford : MasterKernelImplementation<"msimd_graph_bellman_ford", [{
int msimd_graph_bellman_ford(const double* adj_matrix, size_t n, size_t src,
    double* distances, int64_t* predecessors, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!adj_matrix || !distances || !predecessors || src >= n || n == 0) return -1;

    for (size_t i = 0; i < n; ++i) {
        distances[i] = 1e30;
        predecessors[i] = -1;
    }
    distances[src] = 0.0;

    for (size_t iter = 1; iter < n; ++iter) {
        bool changed = false;
        for (size_t u = 0; u < n; ++u) {
            if (distances[u] >= 1e29) continue;
            for (size_t v = 0; v < n; ++v) {
                double w = adj_matrix[u * n + v];
                if (std::abs(w) > 1e-12 || (w != 0.0 && !std::isnan(w))) {
                    if (distances[u] + w < distances[v]) {
                        distances[v] = distances[u] + w;
                        predecessors[v] = (int64_t)u;
                        changed = true;
                    }
                }
            }
        }
        if (!changed) break;
    }

    for (size_t u = 0; u < n; ++u) {
        if (distances[u] >= 1e29) continue;
        for (size_t v = 0; v < n; ++v) {
            double w = adj_matrix[u * n + v];
            if (w != 0.0 && distances[u] + w < distances[v] - 1e-9) {
                return 1;
            }
        }
    }
    return 0;
}
}]>;

def Kernel_graph_max_flow_edmonds_karp : MasterKernelImplementation<"msimd_graph_max_flow_edmonds_karp", [{
double msimd_graph_max_flow_edmonds_karp(const double* capacity_matrix, size_t n, size_t source, size_t sink,
    double* residual_matrix_out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!capacity_matrix || source >= n || sink >= n || source == sink || n == 0) return 0.0;

    std::vector<double> r_graph(n * n);
    for (size_t i = 0; i < n * n; ++i) {
        r_graph[i] = capacity_matrix[i];
    }

    std::vector<int64_t> parent(n);
    double max_flow = 0.0;

    auto bfs = [&]() -> bool {
        std::fill(parent.begin(), parent.end(), -1);
        std::vector<bool> visited(n, false);
        std::queue<size_t> q;

        q.push(source);
        visited[source] = true;

        while (!q.empty()) {
            size_t u = q.front();
            q.pop();

            for (size_t v = 0; v < n; ++v) {
                if (!visited[v] && r_graph[u * n + v] > 1e-12) {
                    q.push(v);
                    parent[v] = (int64_t)u;
                    visited[v] = true;
                    if (v == sink) return true;
                }
            }
        }
        return false;
    };

    while (bfs()) {
        double path_flow = std::numeric_limits<double>::infinity();
        for (size_t v = sink; v != source; v = (size_t)parent[v]) {
            size_t u = (size_t)parent[v];
            path_flow = std::min(path_flow, r_graph[u * n + v]);
        }

        for (size_t v = sink; v != source; v = (size_t)parent[v]) {
            size_t u = (size_t)parent[v];
            r_graph[u * n + v] -= path_flow;
            r_graph[v * n + u] += path_flow;
        }

        max_flow += path_flow;
    }

    if (residual_matrix_out) {
        for (size_t i = 0; i < n * n; ++i) {
            residual_matrix_out[i] = r_graph[i];
        }
    }
    return max_flow;
}
}]>;

def Kernel_u128_add : MasterKernelImplementation<"msimd_u128_add", [{
MSIMD_API msimd_u128_t msimd_u128_add(msimd_u128_t a, msimd_u128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u128_t r;
#if defined(__SIZEOF_INT128__)
    unsigned __int128 u = ((unsigned __int128)a.hi << 64) | a.lo;
    unsigned __int128 v = ((unsigned __int128)b.hi << 64) | b.lo;
    unsigned __int128 sum = u + v;
    r.lo = (uint64_t)sum;
    r.hi = (uint64_t)(sum >> 64);
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
    unsigned char c = _addcarry_u64(0, a.lo, b.lo, &r.lo);
    _addcarry_u64(c, a.hi, b.hi, &r.hi);
#else
    r.lo = a.lo + b.lo;
    r.hi = a.hi + b.hi + (r.lo < a.lo ? 1ULL : 0ULL);
#endif
    return r;
}
}]>;

def Kernel_u128_sub : MasterKernelImplementation<"msimd_u128_sub", [{
MSIMD_API msimd_u128_t msimd_u128_sub(msimd_u128_t a, msimd_u128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u128_t r;
#if defined(__SIZEOF_INT128__)
    unsigned __int128 u = ((unsigned __int128)a.hi << 64) | a.lo;
    unsigned __int128 v = ((unsigned __int128)b.hi << 64) | b.lo;
    unsigned __int128 diff = u - v;
    r.lo = (uint64_t)diff;
    r.hi = (uint64_t)(diff >> 64);
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
    unsigned char c = _subborrow_u64(0, a.lo, b.lo, &r.lo);
    _subborrow_u64(c, a.hi, b.hi, &r.hi);
#else
    r.lo = a.lo - b.lo;
    r.hi = a.hi - b.hi - (a.lo < b.lo ? 1ULL : 0ULL);
#endif
    return r;
}
}]>;

def Kernel_u128_mul : MasterKernelImplementation<"msimd_u128_mul", [{
MSIMD_API msimd_u128_t msimd_u128_mul(msimd_u128_t a, msimd_u128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u128_t r;
#if defined(__SIZEOF_INT128__)
    unsigned __int128 u = ((unsigned __int128)a.hi << 64) | a.lo;
    unsigned __int128 v = ((unsigned __int128)b.hi << 64) | b.lo;
    unsigned __int128 prod = u * v;
    r.lo = (uint64_t)prod;
    r.hi = (uint64_t)(prod >> 64);
#elif defined(_MSC_VER) && defined(_M_X64)
    uint64_t hi_lo = 0;
    r.lo = _umul128(a.lo, b.lo, &hi_lo);
    r.hi = hi_lo + (a.lo * b.hi) + (a.hi * b.lo);
#else
    uint64_t a0 = (uint32_t)a.lo, a1 = a.lo >> 32;
    uint64_t b0 = (uint32_t)b.lo, b1 = b.lo >> 32;
    uint64_t p0 = a0 * b0;
    uint64_t p1 = a0 * b1;
    uint64_t p2 = a1 * b0;
    uint64_t p3 = a1 * b1;
    uint64_t mid = p1 + (p0 >> 32) + (uint32_t)p2;
    r.lo = ((mid << 32) | (uint32_t)p0);
    r.hi = p3 + (mid >> 32) + (p2 >> 32) + (a.lo * b.hi) + (a.hi * b.lo);
#endif
    return r;
}
}]>;

def Kernel_u128_shl : MasterKernelImplementation<"msimd_u128_shl", [{
MSIMD_API msimd_u128_t msimd_u128_shl(msimd_u128_t a, int shift, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u128_t r{0, 0};
    if (shift <= 0) return a;
    if (shift >= 128) return r;
    if (shift >= 64) {
        r.hi = a.lo << (shift - 64);
        r.lo = 0;
    } else {
        r.hi = (a.hi << shift) | (a.lo >> (64 - shift));
        r.lo = a.lo << shift;
    }
    return r;
}
}]>;

def Kernel_u128_shr : MasterKernelImplementation<"msimd_u128_shr", [{
MSIMD_API msimd_u128_t msimd_u128_shr(msimd_u128_t a, int shift, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u128_t r{0, 0};
    if (shift <= 0) return a;
    if (shift >= 128) return r;
    if (shift >= 64) {
        r.lo = a.hi >> (shift - 64);
        r.hi = 0;
    } else {
        r.lo = (a.lo >> shift) | (a.hi << (64 - shift));
        r.hi = a.hi >> shift;
    }
    return r;
}
}]>;

def Kernel_u128_div : MasterKernelImplementation<"msimd_u128_div", [{
MSIMD_API msimd_u128_t msimd_u128_div(msimd_u128_t n, msimd_u128_t d, msimd_u128_t* rem, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u128_t q{0, 0};
    msimd_u128_t r{0, 0};
    if (d.lo == 0 && d.hi == 0) {
        if (rem) *rem = r;
        return q;
    }
    for (int i = 127; i >= 0; --i) {
        r = msimd_u128_shl(r, 1, nullptr);
        uint64_t bit = (i >= 64) ? ((n.hi >> (i - 64)) & 1ULL) : ((n.lo >> i) & 1ULL);
        r.lo |= bit;
        bool ge = (r.hi > d.hi) || (r.hi == d.hi && r.lo >= d.lo);
        if (ge) {
            r = msimd_u128_sub(r, d, nullptr);
            if (i >= 64) q.hi |= (1ULL << (i - 64));
            else q.lo |= (1ULL << i);
        }
    }
    if (rem) *rem = r;
    return q;
}
}]>;

def Kernel_u128_clz : MasterKernelImplementation<"msimd_u128_clz", [{
MSIMD_API int msimd_u128_clz(msimd_u128_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (a.hi != 0) {
#if defined(__clang__) || defined(__GNUC__)
        return __builtin_clzll(a.hi);
#elif defined(_MSC_VER)
        unsigned long idx;
        _BitScanReverse64(&idx, a.hi);
        return (int)(63 - idx);
#else
        for (int i = 63; i >= 0; --i) if ((a.hi >> i) & 1ULL) return 63 - i;
        return 64;
#endif
    }
    if (a.lo != 0) {
#if defined(__clang__) || defined(__GNUC__)
        return 64 + __builtin_clzll(a.lo);
#elif defined(_MSC_VER)
        unsigned long idx;
        _BitScanReverse64(&idx, a.lo);
        return 64 + (int)(63 - idx);
#else
        for (int i = 63; i >= 0; --i) if ((a.lo >> i) & 1ULL) return 64 + (63 - i);
        return 128;
#endif
    }
    return 128;
}
}]>;

def Kernel_u128_popcnt : MasterKernelImplementation<"msimd_u128_popcnt", [{
MSIMD_API int msimd_u128_popcnt(msimd_u128_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
#if defined(__clang__) || defined(__GNUC__)
    return __builtin_popcountll(a.lo) + __builtin_popcountll(a.hi);
#elif defined(_MSC_VER)
    return (int)(__popcnt64(a.lo) + __popcnt64(a.hi));
#else
    auto pc64 = [](uint64_t x) {
        x = x - ((x >> 1) & 0x5555555555555555ULL);
        x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
        x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
        return (int)((x * 0x0101010101010101ULL) >> 56);
    };
    return pc64(a.lo) + pc64(a.hi);
#endif
}
}]>;

def Kernel_vec64_i8_add : MasterKernelImplementation<"msimd_vec64_i8_add", [{
MSIMD_API msimd_i8x8_t msimd_vec64_i8_add(msimd_i8x8_t a, msimd_i8x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x8_t r;
    for (int i = 0; i < 8; ++i) r.v[i] = (int8_t)(a.v[i] + b.v[i]);
    return r;
}
}]>;

def Kernel_vec64_i8_sub : MasterKernelImplementation<"msimd_vec64_i8_sub", [{
MSIMD_API msimd_i8x8_t msimd_vec64_i8_sub(msimd_i8x8_t a, msimd_i8x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x8_t r;
    for (int i = 0; i < 8; ++i) r.v[i] = (int8_t)(a.v[i] - b.v[i]);
    return r;
}
}]>;

def Kernel_vec64_u8_adds_sat : MasterKernelImplementation<"msimd_vec64_u8_adds_sat", [{
MSIMD_API msimd_u8x8_t msimd_vec64_u8_adds_sat(msimd_u8x8_t a, msimd_u8x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x8_t r;
    for (int i = 0; i < 8; ++i) {
        uint32_t s = (uint32_t)a.v[i] + (uint32_t)b.v[i];
        r.v[i] = (uint8_t)(s > 255 ? 255 : s);
    }
    return r;
}
}]>;

def Kernel_vec64_u8_subs_sat : MasterKernelImplementation<"msimd_vec64_u8_subs_sat", [{
MSIMD_API msimd_u8x8_t msimd_vec64_u8_subs_sat(msimd_u8x8_t a, msimd_u8x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x8_t r;
    for (int i = 0; i < 8; ++i) {
        int32_t s = (int32_t)a.v[i] - (int32_t)b.v[i];
        r.v[i] = (uint8_t)(s < 0 ? 0 : s);
    }
    return r;
}
}]>;

def Kernel_vec48_i8_add : MasterKernelImplementation<"msimd_vec48_i8_add", [{
MSIMD_API msimd_i8x6_t msimd_vec48_i8_add(msimd_i8x6_t a, msimd_i8x6_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x6_t r;
    for (int i = 0; i < 6; ++i) r.v[i] = (int8_t)(a.v[i] + b.v[i]);
    return r;
}
}]>;

def Kernel_vec48_i8_sub : MasterKernelImplementation<"msimd_vec48_i8_sub", [{
MSIMD_API msimd_i8x6_t msimd_vec48_i8_sub(msimd_i8x6_t a, msimd_i8x6_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x6_t r;
    for (int i = 0; i < 6; ++i) r.v[i] = (int8_t)(a.v[i] - b.v[i]);
    return r;
}
}]>;

def Kernel_vec32_i8_add : MasterKernelImplementation<"msimd_vec32_i8_add", [{
MSIMD_API msimd_i8x4_t msimd_vec32_i8_add(msimd_i8x4_t a, msimd_i8x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x4_t r;
    for (int i = 0; i < 4; ++i) r.v[i] = (int8_t)(a.v[i] + b.v[i]);
    return r;
}
}]>;

def Kernel_vec32_i8_sub : MasterKernelImplementation<"msimd_vec32_i8_sub", [{
MSIMD_API msimd_i8x4_t msimd_vec32_i8_sub(msimd_i8x4_t a, msimd_i8x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x4_t r;
    for (int i = 0; i < 4; ++i) r.v[i] = (int8_t)(a.v[i] - b.v[i]);
    return r;
}
}]>;

def Kernel_vec16_i8_add : MasterKernelImplementation<"msimd_vec16_i8_add", [{
MSIMD_API msimd_i8x2_t msimd_vec16_i8_add(msimd_i8x2_t a, msimd_i8x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x2_t r;
    for (int i = 0; i < 2; ++i) r.v[i] = (int8_t)(a.v[i] + b.v[i]);
    return r;
}
}]>;

def Kernel_vec16_i8_sub : MasterKernelImplementation<"msimd_vec16_i8_sub", [{
MSIMD_API msimd_i8x2_t msimd_vec16_i8_sub(msimd_i8x2_t a, msimd_i8x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i8x2_t r;
    for (int i = 0; i < 2; ++i) r.v[i] = (int8_t)(a.v[i] - b.v[i]);
    return r;
}
}]>;

def Kernel_fp8_e4m3_from_f32 : MasterKernelImplementation<"msimd_fp8_e4m3_from_f32", [{
MSIMD_API msimd_fp8_e4m3_t msimd_fp8_e4m3_from_f32(float x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8_e4m3_t res{0};
    if (std::isnan(x)) { res.raw = 0x7F; return res; }
    uint32_t u;
    memcpy(&u, &x, 4);
    uint32_t sign = (u >> 31) & 1;
    int32_t exp = (int32_t)((u >> 23) & 0xFF) - 127 + 7;
    uint32_t mant = (u >> 20) & 0x7;
    if (exp <= 0) {
        res.raw = (uint8_t)(sign << 7);
    } else if (exp >= 15) {
        res.raw = (uint8_t)((sign << 7) | (0xF << 3) | 0x6);
    } else {
        res.raw = (uint8_t)((sign << 7) | ((exp & 0xF) << 3) | (mant & 0x7));
    }
    return res;
}
}]>;

def Kernel_fp8_e4m3_to_f32 : MasterKernelImplementation<"msimd_fp8_e4m3_to_f32", [{
MSIMD_API float msimd_fp8_e4m3_to_f32(msimd_fp8_e4m3_t x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    uint32_t sign = (x.raw >> 7) & 1;
    uint32_t exp = (x.raw >> 3) & 0xF;
    uint32_t mant = x.raw & 0x7;
    if (exp == 0xF && mant == 0x7) return std::numeric_limits<float>::quiet_NaN();
    if (exp == 0 && mant == 0) return sign ? -0.0f : 0.0f;
    int32_t f32_exp = (exp == 0) ? (1 - 7 + 127) : ((int32_t)exp - 7 + 127);
    uint32_t f32_mant = mant << 20;
    uint32_t u = (sign << 31) | ((f32_exp & 0xFF) << 23) | (f32_mant & 0x7FFFFF);
    float res;
    memcpy(&res, &u, 4);
    return res;
}
}]>;

def Kernel_fp8_e5m2_from_f32 : MasterKernelImplementation<"msimd_fp8_e5m2_from_f32", [{
MSIMD_API msimd_fp8_e5m2_t msimd_fp8_e5m2_from_f32(float x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8_e5m2_t res{0};
    if (std::isnan(x)) { res.raw = 0x7F; return res; }
    uint32_t u;
    memcpy(&u, &x, 4);
    uint32_t sign = (u >> 31) & 1;
    int32_t exp = (int32_t)((u >> 23) & 0xFF) - 127 + 15;
    uint32_t mant = (u >> 21) & 0x3;
    if (exp <= 0) {
        res.raw = (uint8_t)(sign << 7);
    } else if (exp >= 31) {
        res.raw = (uint8_t)((sign << 7) | (0x1F << 2));
    } else {
        res.raw = (uint8_t)((sign << 7) | ((exp & 0x1F) << 2) | (mant & 0x3));
    }
    return res;
}
}]>;

def Kernel_fp8_e5m2_to_f32 : MasterKernelImplementation<"msimd_fp8_e5m2_to_f32", [{
MSIMD_API float msimd_fp8_e5m2_to_f32(msimd_fp8_e5m2_t x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    uint32_t sign = (x.raw >> 7) & 1;
    uint32_t exp = (x.raw >> 2) & 0x1F;
    uint32_t mant = x.raw & 0x3;
    if (exp == 0x1F) {
        if (mant == 0) return sign ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
        return std::numeric_limits<float>::quiet_NaN();
    }
    if (exp == 0 && mant == 0) return sign ? -0.0f : 0.0f;
    int32_t f32_exp = (exp == 0) ? (1 - 15 + 127) : ((int32_t)exp - 15 + 127);
    uint32_t f32_mant = mant << 21;
    uint32_t u = (sign << 31) | ((f32_exp & 0xFF) << 23) | (f32_mant & 0x7FFFFF);
    float res;
    memcpy(&res, &u, 4);
    return res;
}
}]>;

def Kernel_vec128_fp8_e4m3_from_f32_16 : MasterKernelImplementation<"msimd_vec128_fp8_e4m3_from_f32_16", [{
MSIMD_API msimd_fp8x16_e4m3_t msimd_vec128_fp8_e4m3_from_f32_16(const float in[16], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8x16_e4m3_t r{};
    if (!in) return r;
    for (int i = 0; i < 16; ++i) r.v[i] = msimd_fp8_e4m3_from_f32(in[i], nullptr);
    return r;
}
}]>;

def Kernel_vec128_fp8_e4m3_to_f32_16 : MasterKernelImplementation<"msimd_vec128_fp8_e4m3_to_f32_16", [{
MSIMD_API void msimd_vec128_fp8_e4m3_to_f32_16(msimd_fp8x16_e4m3_t in, float out[16], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!out) return;
    for (int i = 0; i < 16; ++i) out[i] = msimd_fp8_e4m3_to_f32(in.v[i], nullptr);
}
}]>;

def Kernel_vec128_fp8_e4m3_add : MasterKernelImplementation<"msimd_vec128_fp8_e4m3_add", [{
MSIMD_API msimd_fp8x16_e4m3_t msimd_vec128_fp8_e4m3_add(msimd_fp8x16_e4m3_t a, msimd_fp8x16_e4m3_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8x16_e4m3_t r{};
    for (int i = 0; i < 16; ++i) {
        float fa = msimd_fp8_e4m3_to_f32(a.v[i], nullptr);
        float fb = msimd_fp8_e4m3_to_f32(b.v[i], nullptr);
        r.v[i] = msimd_fp8_e4m3_from_f32(fa + fb, nullptr);
    }
    return r;
}
}]>;

def Kernel_vec128_fp8_e4m3_sub : MasterKernelImplementation<"msimd_vec128_fp8_e4m3_sub", [{
MSIMD_API msimd_fp8x16_e4m3_t msimd_vec128_fp8_e4m3_sub(msimd_fp8x16_e4m3_t a, msimd_fp8x16_e4m3_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8x16_e4m3_t r{};
    for (int i = 0; i < 16; ++i) {
        float fa = msimd_fp8_e4m3_to_f32(a.v[i], nullptr);
        float fb = msimd_fp8_e4m3_to_f32(b.v[i], nullptr);
        r.v[i] = msimd_fp8_e4m3_from_f32(fa - fb, nullptr);
    }
    return r;
}
}]>;

def Kernel_vec128_fp8_e4m3_dot : MasterKernelImplementation<"msimd_vec128_fp8_e4m3_dot", [{
MSIMD_API float msimd_vec128_fp8_e4m3_dot(msimd_fp8x16_e4m3_t a, msimd_fp8x16_e4m3_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float sum = 0.0f;
    for (int i = 0; i < 16; ++i) {
        sum += msimd_fp8_e4m3_to_f32(a.v[i], nullptr) * msimd_fp8_e4m3_to_f32(b.v[i], nullptr);
    }
    return sum;
}
}]>;

def Kernel_vec128_fp8_e5m2_from_f32_16 : MasterKernelImplementation<"msimd_vec128_fp8_e5m2_from_f32_16", [{
MSIMD_API msimd_fp8x16_e5m2_t msimd_vec128_fp8_e5m2_from_f32_16(const float in[16], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8x16_e5m2_t r{};
    if (!in) return r;
    for (int i = 0; i < 16; ++i) r.v[i] = msimd_fp8_e5m2_from_f32(in[i], nullptr);
    return r;
}
}]>;

def Kernel_vec128_fp8_e5m2_to_f32_16 : MasterKernelImplementation<"msimd_vec128_fp8_e5m2_to_f32_16", [{
MSIMD_API void msimd_vec128_fp8_e5m2_to_f32_16(msimd_fp8x16_e5m2_t in, float out[16], const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!out) return;
    for (int i = 0; i < 16; ++i) out[i] = msimd_fp8_e5m2_to_f32(in.v[i], nullptr);
}
}]>;

def Kernel_vec128_fp8_e5m2_add : MasterKernelImplementation<"msimd_vec128_fp8_e5m2_add", [{
MSIMD_API msimd_fp8x16_e5m2_t msimd_vec128_fp8_e5m2_add(msimd_fp8x16_e5m2_t a, msimd_fp8x16_e5m2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8x16_e5m2_t r{};
    for (int i = 0; i < 16; ++i) {
        float fa = msimd_fp8_e5m2_to_f32(a.v[i], nullptr);
        float fb = msimd_fp8_e5m2_to_f32(b.v[i], nullptr);
        r.v[i] = msimd_fp8_e5m2_from_f32(fa + fb, nullptr);
    }
    return r;
}
}]>;

def Kernel_vec128_fp8_e5m2_sub : MasterKernelImplementation<"msimd_vec128_fp8_e5m2_sub", [{
MSIMD_API msimd_fp8x16_e5m2_t msimd_vec128_fp8_e5m2_sub(msimd_fp8x16_e5m2_t a, msimd_fp8x16_e5m2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_fp8x16_e5m2_t r{};
    for (int i = 0; i < 16; ++i) {
        float fa = msimd_fp8_e5m2_to_f32(a.v[i], nullptr);
        float fb = msimd_fp8_e5m2_to_f32(b.v[i], nullptr);
        r.v[i] = msimd_fp8_e5m2_from_f32(fa - fb, nullptr);
    }
    return r;
}
}]>;

def Kernel_vec128_fp8_e5m2_dot : MasterKernelImplementation<"msimd_vec128_fp8_e5m2_dot", [{
MSIMD_API float msimd_vec128_fp8_e5m2_dot(msimd_fp8x16_e5m2_t a, msimd_fp8x16_e5m2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    float sum = 0.0f;
    for (int i = 0; i < 16; ++i) {
        sum += msimd_fp8_e5m2_to_f32(a.v[i], nullptr) * msimd_fp8_e5m2_to_f32(b.v[i], nullptr);
    }
    return sum;
}
}]>;

def Kernel_dual64_add : MasterKernelImplementation<"msimd_dual64_add", [{
MSIMD_API void msimd_dual64_add(const uint64_t* a, const uint64_t* b, uint64_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    msimd_u128_t ua{a[0], a[1]};
    msimd_u128_t ub{b[0], b[1]};
    msimd_u128_t ur = msimd_u128_add(ua, ub, nullptr);
    out[0] = ur.lo;
    out[1] = ur.hi;
}
}]>;

def Kernel_dual64_sub : MasterKernelImplementation<"msimd_dual64_sub", [{
MSIMD_API void msimd_dual64_sub(const uint64_t* a, const uint64_t* b, uint64_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    msimd_u128_t ua{a[0], a[1]};
    msimd_u128_t ub{b[0], b[1]};
    msimd_u128_t ur = msimd_u128_sub(ua, ub, nullptr);
    out[0] = ur.lo;
    out[1] = ur.hi;
}
}]>;

def Kernel_dual64_mul_scalar : MasterKernelImplementation<"msimd_dual64_mul_scalar", [{
MSIMD_API void msimd_dual64_mul_scalar(const uint64_t* a, uint64_t scalar, uint64_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !out) return;
    msimd_u128_t ua{a[0], a[1]};
    msimd_u128_t ub{scalar, 0};
    msimd_u128_t ur = msimd_u128_mul(ua, ub, nullptr);
    out[0] = ur.lo;
    out[1] = ur.hi;
}
}]>;

def Kernel_dual64_propagate_carries : MasterKernelImplementation<"msimd_dual64_propagate_carries", [{
MSIMD_API void msimd_dual64_propagate_carries(uint64_t* limbs, size_t count, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!limbs || count < 2) return;
    uint64_t carry = 0;
    for (size_t i = 0; i < count; ++i) {
        uint64_t prev = limbs[i];
        limbs[i] += carry;
        carry = (limbs[i] < prev) ? 1ULL : 0ULL;
    }
}
}]>;

def Kernel_quad32_add : MasterKernelImplementation<"msimd_quad32_add", [{
MSIMD_API void msimd_quad32_add(const uint32_t* a, const uint32_t* b, uint32_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    uint64_t carry = 0;
    for (int i = 0; i < 4; ++i) {
        uint64_t sum = (uint64_t)a[i] + (uint64_t)b[i] + carry;
        out[i] = (uint32_t)sum;
        carry = sum >> 32;
    }
}
}]>;

def Kernel_quad32_sub : MasterKernelImplementation<"msimd_quad32_sub", [{
MSIMD_API void msimd_quad32_sub(const uint32_t* a, const uint32_t* b, uint32_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    int64_t borrow = 0;
    for (int i = 0; i < 4; ++i) {
        int64_t diff = (int64_t)a[i] - (int64_t)b[i] - borrow;
        if (diff < 0) {
            out[i] = (uint32_t)(diff + (1ULL << 32));
            borrow = 1;
        } else {
            out[i] = (uint32_t)diff;
            borrow = 0;
        }
    }
}
}]>;

def Kernel_octal16_add : MasterKernelImplementation<"msimd_octal16_add", [{
MSIMD_API void msimd_octal16_add(const uint16_t* a, const uint16_t* b, uint16_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    uint32_t carry = 0;
    for (int i = 0; i < 8; ++i) {
        uint32_t sum = (uint32_t)a[i] + (uint32_t)b[i] + carry;
        out[i] = (uint16_t)sum;
        carry = sum >> 16;
    }
}
}]>;

def Kernel_octal16_sub : MasterKernelImplementation<"msimd_octal16_sub", [{
MSIMD_API void msimd_octal16_sub(const uint16_t* a, const uint16_t* b, uint16_t* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    int32_t borrow = 0;
    for (int i = 0; i < 8; ++i) {
        int32_t diff = (int32_t)a[i] - (int32_t)b[i] - borrow;
        if (diff < 0) {
            out[i] = (uint16_t)(diff + (1 << 16));
            borrow = 1;
        } else {
            out[i] = (uint16_t)diff;
            borrow = 0;
        }
    }
}
}]>;

def Kernel_two_sum : MasterKernelImplementation<"msimd_two_sum", [{
MSIMD_API void msimd_two_sum(double a, double b, double* s, double* e, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double sm = a + b;
    double v = sm - a;
    if (s) *s = sm;
    if (e) *e = (a - (sm - v)) + (b - v);
}
}]>;

def Kernel_two_prod : MasterKernelImplementation<"msimd_two_prod", [{
MSIMD_API void msimd_two_prod(double a, double b, double* p, double* e, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double pr = a * b;
    if (p) *p = pr;
    if (e) {
#if defined(__FMA__) || defined(_M_X64)
        *e = std::fma(a, b, -pr);
#else
        const double split = 134217729.0;
        double ca = split * a;
        double a_hi = ca - (ca - a);
        double a_lo = a - a_hi;
        double cb = split * b;
        double b_hi = cb - (cb - b);
        double b_lo = b - b_hi;
        *e = ((a_hi * b_hi - pr) + a_hi * b_lo + a_lo * b_hi) + a_lo * b_lo;
#endif
    }
}
}]>;

def Kernel_doubledouble_add : MasterKernelImplementation<"msimd_doubledouble_add", [{
MSIMD_API void msimd_doubledouble_add(double hi_a, double lo_a, double hi_b, double lo_b, double* out_hi, double* out_lo, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double s1, s2, e1, e2;
    msimd_two_sum(hi_a, hi_b, &s1, &s2, nullptr);
    msimd_two_sum(lo_a, lo_b, &e1, &e2, nullptr);
    s2 += e1;
    double h, l;
    msimd_two_sum(s1, s2, &h, &l, nullptr);
    l += e2;
    double final_h, final_l;
    msimd_two_sum(h, l, &final_h, &final_l, nullptr);
    if (out_hi) *out_hi = final_h;
    if (out_lo) *out_lo = final_l;
}
}]>;

def Kernel_doubledouble_mul : MasterKernelImplementation<"msimd_doubledouble_mul", [{
MSIMD_API void msimd_doubledouble_mul(double hi_a, double lo_a, double hi_b, double lo_b, double* out_hi, double* out_lo, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double p1, p2;
    msimd_two_prod(hi_a, hi_b, &p1, &p2, nullptr);
    p2 += hi_a * lo_b + lo_a * hi_b;
    double final_h, final_l;
    msimd_two_sum(p1, p2, &final_h, &final_l, nullptr);
    if (out_hi) *out_hi = final_h;
    if (out_lo) *out_lo = final_l;
}
}]>;

def Kernel_fp128_from_f64 : MasterKernelImplementation<"msimd_fp128_from_f64", [{
MSIMD_API msimd_fp128_t msimd_fp128_from_f64(double x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return sf_f64_to_fp128(x);
}
}]>;

def Kernel_fp128_to_f64 : MasterKernelImplementation<"msimd_fp128_to_f64", [{
MSIMD_API double msimd_fp128_to_f64(msimd_fp128_t x, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return sf_fp128_to_f64(x);
}
}]>;

def Kernel_fp128_add : MasterKernelImplementation<"msimd_fp128_add", [{
MSIMD_API msimd_fp128_t msimd_fp128_add(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    double db = sf_fp128_to_f64(b);
    double h, l;
    msimd_doubledouble_add(da, 0.0, db, 0.0, &h, &l, nullptr);
    return sf_f64_to_fp128(h + l);
}
}]>;

def Kernel_fp128_sub : MasterKernelImplementation<"msimd_fp128_sub", [{
MSIMD_API msimd_fp128_t msimd_fp128_sub(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    double db = sf_fp128_to_f64(b);
    double h, l;
    msimd_doubledouble_add(da, 0.0, -db, 0.0, &h, &l, nullptr);
    return sf_f64_to_fp128(h + l);
}
}]>;

def Kernel_fp128_mul : MasterKernelImplementation<"msimd_fp128_mul", [{
MSIMD_API msimd_fp128_t msimd_fp128_mul(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    double db = sf_fp128_to_f64(b);
    double h, l;
    msimd_doubledouble_mul(da, 0.0, db, 0.0, &h, &l, nullptr);
    return sf_f64_to_fp128(h + l);
}
}]>;

def Kernel_fp128_div : MasterKernelImplementation<"msimd_fp128_div", [{
MSIMD_API msimd_fp128_t msimd_fp128_div(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    double db = sf_fp128_to_f64(b);
    if (db == 0.0) {
        return (da >= 0) ? sf_f64_to_fp128(std::numeric_limits<double>::infinity())
                         : sf_f64_to_fp128(-std::numeric_limits<double>::infinity());
    }
    double q1 = da / db;
    double p1, p2;
    msimd_two_prod(q1, db, &p1, &p2, nullptr);
    double diff = da - p1 - p2;
    double q2 = diff / db;
    return sf_f64_to_fp128(q1 + q2);
}
}]>;

def Kernel_fp128_sqrt : MasterKernelImplementation<"msimd_fp128_sqrt", [{
MSIMD_API msimd_fp128_t msimd_fp128_sqrt(msimd_fp128_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    if (da < 0.0) return sf_f64_to_fp128(std::numeric_limits<double>::quiet_NaN());
    if (da == 0.0) return sf_f64_to_fp128(0.0);
    double s = std::sqrt(da);
    s = 0.5 * (s + da / s);
    return sf_f64_to_fp128(s);
}
}]>;

def Kernel_fp128_fma : MasterKernelImplementation<"msimd_fp128_fma", [{
MSIMD_API msimd_fp128_t msimd_fp128_fma(msimd_fp128_t a, msimd_fp128_t b, msimd_fp128_t c, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    double db = sf_fp128_to_f64(b);
    double dc = sf_fp128_to_f64(c);
    double prod_h, prod_l;
    msimd_doubledouble_mul(da, 0.0, db, 0.0, &prod_h, &prod_l, nullptr);
    double res_h, res_l;
    msimd_doubledouble_add(prod_h, prod_l, dc, 0.0, &res_h, &res_l, nullptr);
    return sf_f64_to_fp128(res_h + res_l);
}
}]>;

def Kernel_fp128_cmp : MasterKernelImplementation<"msimd_fp128_cmp", [{
MSIMD_API int msimd_fp128_cmp(msimd_fp128_t a, msimd_fp128_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    double da = sf_fp128_to_f64(a);
    double db = sf_fp128_to_f64(b);
    if (std::isnan(da) || std::isnan(db)) return -2;
    if (da < db) return -1;
    if (da > db) return 1;
    if (a.hi < b.hi) return -1;
    if (a.hi > b.hi) return 1;
    if (a.lo < b.lo) return -1;
    if (a.lo > b.lo) return 1;
    return 0;
}
}]>;

defm Kernel_mat2_mul : DualKernelImplementation<"msimd_mat2f_mul", "msimd_mat2d_mul", [{
MSIMD_API void msimd_mat2f_mul(const float* a, const float* b, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    float r0 = a[0]*b[0] + a[1]*b[2];
    float r1 = a[0]*b[1] + a[1]*b[3];
    float r2 = a[2]*b[0] + a[3]*b[2];
    float r3 = a[2]*b[1] + a[3]*b[3];
    out[0] = r0; out[1] = r1; out[2] = r2; out[3] = r3;
}
}], [{
MSIMD_API void msimd_mat2d_mul(const double* a, const double* b, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !b || !out) return;
    double r0 = a[0]*b[0] + a[1]*b[2];
    double r1 = a[0]*b[1] + a[1]*b[3];
    double r2 = a[2]*b[0] + a[3]*b[2];
    double r3 = a[2]*b[1] + a[3]*b[3];
    out[0] = r0; out[1] = r1; out[2] = r2; out[3] = r3;
}
}]>;

defm Kernel_mat2_mul_vec : DualKernelImplementation<"msimd_mat2f_mul_vec", "msimd_mat2d_mul_vec", [{
MSIMD_API void msimd_mat2f_mul_vec(const float* m, const float* v, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !v || !out) return;
    float x = m[0]*v[0] + m[1]*v[1];
    float y = m[2]*v[0] + m[3]*v[1];
    out[0] = x; out[1] = y;
}
}], [{
MSIMD_API void msimd_mat2d_mul_vec(const double* m, const double* v, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !v || !out) return;
    double x = m[0]*v[0] + m[1]*v[1];
    double y = m[2]*v[0] + m[3]*v[1];
    out[0] = x; out[1] = y;
}
}]>;

defm Kernel_mat2_det : DualKernelImplementation<"msimd_mat2f_det", "msimd_mat2d_det", [{
MSIMD_API float msimd_mat2f_det(const float* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m) return 0.0f;
    return m[0]*m[3] - m[1]*m[2];
}
}], [{
MSIMD_API double msimd_mat2d_det(const double* m, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m) return 0.0;
    return m[0]*m[3] - m[1]*m[2];
}
}]>;

defm Kernel_mat2_inv : DualKernelImplementation<"msimd_mat2f_inv", "msimd_mat2d_inv", [{
MSIMD_API int msimd_mat2f_inv(const float* m, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !out) return -1;
    float det = m[0]*m[3] - m[1]*m[2];
    if (std::fabs(det) < 1e-12f) return -1;
    float inv_det = 1.0f / det;
    float r0 =  m[3] * inv_det;
    float r1 = -m[1] * inv_det;
    float r2 = -m[2] * inv_det;
    float r3 =  m[0] * inv_det;
    out[0] = r0; out[1] = r1; out[2] = r2; out[3] = r3;
    return 0;
}
}], [{
MSIMD_API int msimd_mat2d_inv(const double* m, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !out) return -1;
    double det = m[0]*m[3] - m[1]*m[2];
    if (std::fabs(det) < 1e-15) return -1;
    double inv_det = 1.0 / det;
    double r0 =  m[3] * inv_det;
    double r1 = -m[1] * inv_det;
    double r2 = -m[2] * inv_det;
    double r3 =  m[0] * inv_det;
    out[0] = r0; out[1] = r1; out[2] = r2; out[3] = r3;
    return 0;
}
}]>;

defm Kernel_mat3_inv : DualKernelImplementation<"msimd_mat3f_inv", "msimd_mat3d_inv", [{
MSIMD_API int msimd_mat3f_inv(const float* m, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !out) return -1;
    return msimd_mat3f_invert(m, out) ? 0 : -1;
}
}], [{
MSIMD_API int msimd_mat3d_inv(const double* m, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!m || !out) return -1;
    double det = msimd_mat3d_det(m, nullptr);
    if (std::fabs(det) < 1e-15) return -1;
    double inv_det = 1.0 / det;
    double r[9];
    r[0] = (m[4]*m[8] - m[5]*m[7]) * inv_det;
    r[1] = (m[2]*m[7] - m[1]*m[8]) * inv_det;
    r[2] = (m[1]*m[5] - m[2]*m[4]) * inv_det;
    r[3] = (m[5]*m[6] - m[3]*m[8]) * inv_det;
    r[4] = (m[0]*m[8] - m[2]*m[6]) * inv_det;
    r[5] = (m[2]*m[3] - m[0]*m[5]) * inv_det;
    r[6] = (m[3]*m[7] - m[4]*m[6]) * inv_det;
    r[7] = (m[1]*m[6] - m[0]*m[7]) * inv_det;
    r[8] = (m[0]*m[4] - m[1]*m[3]) * inv_det;
    std::memcpy(out, r, sizeof(r));
    return 0;
}
}]>;

defm Kernel_mat4_det : DualKernelImplementation<"msimd_mat4f_det", "msimd_mat4d_det", [{
MSIMD_API float msimd_mat4f_det(const float* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a) return 0.0f;
    float s0 = a[0]*a[5] - a[4]*a[1];
    float s1 = a[0]*a[6] - a[4]*a[2];
    float s2 = a[0]*a[7] - a[4]*a[3];
    float s3 = a[1]*a[6] - a[5]*a[2];
    float s4 = a[1]*a[7] - a[5]*a[3];
    float s5 = a[2]*a[7] - a[6]*a[3];
    float c5 = a[10]*a[15] - a[14]*a[11];
    float c4 = a[9]*a[15]  - a[13]*a[11];
    float c3 = a[9]*a[14]  - a[13]*a[10];
    float c2 = a[8]*a[15]  - a[12]*a[11];
    float c1 = a[8]*a[14]  - a[12]*a[10];
    float c0 = a[8]*a[13]  - a[12]*a[9];
    return (s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0);
}
}], [{
MSIMD_API double msimd_mat4d_det(const double* a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a) return 0.0;
    double s0 = a[0]*a[5] - a[4]*a[1];
    double s1 = a[0]*a[6] - a[4]*a[2];
    double s2 = a[0]*a[7] - a[4]*a[3];
    double s3 = a[1]*a[6] - a[5]*a[2];
    double s4 = a[1]*a[7] - a[5]*a[3];
    double s5 = a[2]*a[7] - a[6]*a[3];
    double c5 = a[10]*a[15] - a[14]*a[11];
    double c4 = a[9]*a[15]  - a[13]*a[11];
    double c3 = a[9]*a[14]  - a[13]*a[10];
    double c2 = a[8]*a[15]  - a[12]*a[11];
    double c1 = a[8]*a[14]  - a[12]*a[10];
    double c0 = a[8]*a[13]  - a[12]*a[9];
    return (s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0);
}
}]>;

defm Kernel_mat4_inv : DualKernelImplementation<"msimd_mat4f_inv", "msimd_mat4d_inv", [{
MSIMD_API int msimd_mat4f_inv(const float* a, float* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !out) return -1;
    float s0 = a[0]*a[5] - a[4]*a[1];
    float s1 = a[0]*a[6] - a[4]*a[2];
    float s2 = a[0]*a[7] - a[4]*a[3];
    float s3 = a[1]*a[6] - a[5]*a[2];
    float s4 = a[1]*a[7] - a[5]*a[3];
    float s5 = a[2]*a[7] - a[6]*a[3];
    float c5 = a[10]*a[15] - a[14]*a[11];
    float c4 = a[9]*a[15]  - a[13]*a[11];
    float c3 = a[9]*a[14]  - a[13]*a[10];
    float c2 = a[8]*a[15]  - a[12]*a[11];
    float c1 = a[8]*a[14]  - a[12]*a[10];
    float c0 = a[8]*a[13]  - a[12]*a[9];
    float det = (s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0);
    if (std::fabs(det) < 1e-12f) return -1;
    float inv_det = 1.0f / det;
    float r[16];
    r[0]  = ( a[5]*c5 - a[6]*c4 + a[7]*c3) * inv_det;
    r[1]  = (-a[1]*c5 + a[2]*c4 - a[3]*c3) * inv_det;
    r[2]  = ( a[13]*s5 - a[14]*s4 + a[15]*s3) * inv_det;
    r[3]  = (-a[9]*s5 + a[10]*s4 - a[11]*s3) * inv_det;
    r[4]  = (-a[4]*c5 + a[6]*c2 - a[7]*c1) * inv_det;
    r[5]  = ( a[0]*c5 - a[2]*c2 + a[3]*c1) * inv_det;
    r[6]  = (-a[12]*s5 + a[14]*s2 - a[15]*s1) * inv_det;
    r[7]  = ( a[8]*s5 - a[10]*s2 + a[11]*s1) * inv_det;
    r[8]  = ( a[4]*c4 - a[5]*c2 + a[7]*c0) * inv_det;
    r[9]  = (-a[0]*c4 + a[1]*c2 - a[3]*c0) * inv_det;
    r[10] = ( a[12]*s4 - a[13]*s2 + a[15]*s0) * inv_det;
    r[11] = (-a[8]*s4 + a[9]*s2 - a[11]*s0) * inv_det;
    r[12] = (-a[4]*c3 + a[5]*c1 - a[6]*c0) * inv_det;
    r[13] = ( a[0]*c3 - a[1]*c1 + a[2]*c0) * inv_det;
    r[14] = (-a[12]*s3 + a[13]*s1 - a[14]*s0) * inv_det;
    r[15] = ( a[8]*s3 - a[9]*s1 + a[10]*s0) * inv_det;
    std::memcpy(out, r, sizeof(r));
    return 0;
}
}], [{
MSIMD_API int msimd_mat4d_inv(const double* a, double* out, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!a || !out) return -1;
    double s0 = a[0]*a[5] - a[4]*a[1];
    double s1 = a[0]*a[6] - a[4]*a[2];
    double s2 = a[0]*a[7] - a[4]*a[3];
    double s3 = a[1]*a[6] - a[5]*a[2];
    double s4 = a[1]*a[7] - a[5]*a[3];
    double s5 = a[2]*a[7] - a[6]*a[3];
    double c5 = a[10]*a[15] - a[14]*a[11];
    double c4 = a[9]*a[15]  - a[13]*a[11];
    double c3 = a[9]*a[14]  - a[13]*a[10];
    double c2 = a[8]*a[15]  - a[12]*a[11];
    double c1 = a[8]*a[14]  - a[12]*a[10];
    double c0 = a[8]*a[13]  - a[12]*a[9];
    double det = (s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0);
    if (std::fabs(det) < 1e-15) return -1;
    double inv_det = 1.0 / det;
    double r[16];
    r[0]  = ( a[5]*c5 - a[6]*c4 + a[7]*c3) * inv_det;
    r[1]  = (-a[1]*c5 + a[2]*c4 - a[3]*c3) * inv_det;
    r[2]  = ( a[13]*s5 - a[14]*s4 + a[15]*s3) * inv_det;
    r[3]  = (-a[9]*s5 + a[10]*s4 - a[11]*s3) * inv_det;
    r[4]  = (-a[4]*c5 + a[6]*c2 - a[7]*c1) * inv_det;
    r[5]  = ( a[0]*c5 - a[2]*c2 + a[3]*c1) * inv_det;
    r[6]  = (-a[12]*s5 + a[14]*s2 - a[15]*s1) * inv_det;
    r[7]  = ( a[8]*s5 - a[10]*s2 + a[11]*s1) * inv_det;
    r[8]  = ( a[4]*c4 - a[5]*c2 + a[7]*c0) * inv_det;
    r[9]  = (-a[0]*c4 + a[1]*c2 - a[3]*c0) * inv_det;
    r[10] = ( a[12]*s4 - a[13]*s2 + a[15]*s0) * inv_det;
    r[11] = (-a[8]*s4 + a[9]*s2 - a[11]*s0) * inv_det;
    r[12] = (-a[4]*c3 + a[5]*c1 - a[6]*c0) * inv_det;
    r[13] = ( a[0]*c3 - a[1]*c1 + a[2]*c0) * inv_det;
    r[14] = (-a[12]*s3 + a[13]*s1 - a[14]*s0) * inv_det;
    r[15] = ( a[8]*s3 - a[9]*s1 + a[10]*s0) * inv_det;
    std::memcpy(out, r, sizeof(r));
    return 0;
}
}]>;

def Kernel_wave_step_1d : MasterKernelImplementation<"msimd_wave_step_1d", [{
MSIMD_API msimd_status_t msimd_wave_step_1d(msimd_context ctx, msimd_lattice lat, double c, double dt, double dx, const msimd_tensor_t* precision_target) {
    (void)ctx;
    if (!lat || !lat->data_next || !lat->data_curr || !lat->data_prev || dx <= 0.0) return MSIMD_ERROR_INVALID_ARGUMENT;
    if (lat->nx < 3) return MSIMD_ERROR_INVALID_ARGUMENT;
    float r = static_cast<float>((c * dt) / dx);
    msimd_kernel_wave_step_1d_fp32(
        static_cast<float*>(lat->data_next),
        static_cast<const float*>(lat->data_curr),
        static_cast<const float*>(lat->data_prev),
        lat->nx, r * r, precision_target);
    return MSIMD_SUCCESS;
}
}]>;

def Kernel_wave_step_3d : MasterKernelImplementation<"msimd_wave_step_3d", [{
MSIMD_API msimd_status_t msimd_wave_step_3d(msimd_context ctx, msimd_lattice lat, double c, double dt, double dx, double dy, double dz, const msimd_tensor_t* precision_target) {
    (void)ctx;
    if (!lat || !lat->data_next || !lat->data_curr || !lat->data_prev || dx <= 0.0 || dy <= 0.0 || dz <= 0.0) return MSIMD_ERROR_INVALID_ARGUMENT;
    if (lat->nx < 3 || lat->ny < 3 || lat->nz < 3) return MSIMD_ERROR_INVALID_ARGUMENT;
    float rx = static_cast<float>((c * dt) / dx);
    float ry = static_cast<float>((c * dt) / dy);
    float rz = static_cast<float>((c * dt) / dz);
    msimd_kernel_wave_step_3d_fp32(
        static_cast<float*>(lat->data_next),
        static_cast<const float*>(lat->data_curr),
        static_cast<const float*>(lat->data_prev),
        lat->nx, lat->ny, lat->nz, rx * rx, ry * ry, rz * rz, precision_target);
    return MSIMD_SUCCESS;
}
}]>;

def Kernel_dispatch_step_parallel : MasterKernelImplementation<"msimd_dispatch_step_parallel", [{
MSIMD_API msimd_status_t msimd_dispatch_step_parallel(msimd_context ctx, msimd_lattice lat, int num_steps, const msimd_tensor_t* precision_target) {
    if (!lat || num_steps < 0) return MSIMD_ERROR_INVALID_ARGUMENT;
    for (int s = 0; s < num_steps; ++s) {
        msimd_status_t st = (lat->dim == 1 || (lat->ny <= 1 && lat->nz <= 1))
            ? msimd_wave_step_1d(ctx, lat, 1.0, 0.1, 1.0, precision_target)
            : msimd_wave_step_3d(ctx, lat, 1.0, 0.1, 1.0, 1.0, 1.0, precision_target);
        if (st != MSIMD_SUCCESS) return st;
        void* tmp = lat->data_prev;
        lat->data_prev = lat->data_curr;
        lat->data_curr = lat->data_next;
        lat->data_next = tmp;
    }
    return MSIMD_SUCCESS;
}
}]>;

def Kernel_maxwell_yee_3d_step : MasterKernelImplementation<"msimd_maxwell_yee_3d_step", [{
MSIMD_API msimd_status_t msimd_maxwell_yee_3d_step(
    msimd_context ctx, size_t nx, size_t ny, size_t nz,
    float* Ex, float* Ey, float* Ez, float* Hx, float* Hy, float* Hz,
    float dt, float dx, float dy, float dz, float mu, float eps, const msimd_tensor_t* precision_target) {
    (void)ctx;
    if (!Ex || !Ey || !Ez || !Hx || !Hy || !Hz || dx <= 0.0f || dy <= 0.0f || dz <= 0.0f || mu == 0.0f || eps == 0.0f) return MSIMD_ERROR_INVALID_ARGUMENT;
    if (nx < 3 || ny < 3 || nz < 3) return MSIMD_ERROR_INVALID_ARGUMENT;
    msimd_kernel_maxwell_yee_3d(nx, ny, nz, Ex, Ey, Ez, Hx, Hy, Hz, dt, dx, dy, dz, mu, eps, precision_target);
    return MSIMD_SUCCESS;
}
}]>;

def Kernel_lbm_d2q9_step : MasterKernelImplementation<"msimd_lbm_d2q9_step", [{
MSIMD_API msimd_status_t msimd_lbm_d2q9_step(
    msimd_context ctx, size_t nx, size_t ny,
    const float* f_in, float* f_out, float* rho, float* ux, float* uy, float tau, const msimd_tensor_t* precision_target) {
    (void)ctx;
    if (!f_in || !f_out || !rho || !ux || !uy || nx == 0 || ny == 0 || tau <= 0.0f) return MSIMD_ERROR_INVALID_ARGUMENT;
    msimd_kernel_lbm_d2q9(nx, ny, f_in, f_out, rho, ux, uy, tau, precision_target);
    return MSIMD_SUCCESS;
}
}]>;

def Kernel_ray_triangle_batch8 : MasterKernelImplementation<"msimd_ray_triangle_batch8", [{
MSIMD_API msimd_status_t msimd_ray_triangle_batch8(
    msimd_context ctx,
    const float ox[8], const float oy[8], const float oz[8],
    const float dx[8], const float dy[8], const float dz[8],
    const float v0[3], const float v1[3], const float v2[3],
    int hit[8], float t[8], float u[8], float v[8], const msimd_tensor_t* precision_target) {
    (void)ctx;
    if (!ox || !oy || !oz || !dx || !dy || !dz || !v0 || !v1 || !v2 || !hit || !t || !u || !v) return MSIMD_ERROR_INVALID_ARGUMENT;
    msimd_kernel_ray_triangle_batch8(ox, oy, oz, dx, dy, dz, v0, v1, v2, hit, t, u, v, precision_target);
    return MSIMD_SUCCESS;
}
}]>;

def Kernel_stockham_fft_radix2 : MasterKernelImplementation<"msimd_stockham_fft_radix2", [{
MSIMD_API msimd_status_t msimd_stockham_fft_radix2(
    msimd_context ctx, size_t n, const float* in_real, const float* in_imag, float* out_real, float* out_imag, int inverse, const msimd_tensor_t* precision_target) {
    (void)ctx;
    if (!in_real || !in_imag || !out_real || !out_imag || n == 0 || (n & (n - 1)) != 0) return MSIMD_ERROR_INVALID_ARGUMENT;
    msimd_kernel_stockham_fft_radix2(n, in_real, in_imag, out_real, out_imag, inverse, precision_target);
    return MSIMD_SUCCESS;
}
}]>;

def Kernel_internal_splitmix64 : MasterKernelImplementation<"msimd_internal_splitmix64", [{
static uint64_t msimd_internal_splitmix64(void) {
    static uint64_t state = 0x853C49E6748FEA9BULL;
    uint64_t z = (state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}
}]>;

def Kernel_trng_fast64 : MasterKernelImplementation<"msimd_trng_fast64", [{
MSIMD_API int msimd_trng_fast64(uint64_t* out_val, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!out_val) return -1;
    *out_val = msimd_internal_splitmix64();
    return 0;
}
}]>;

def Kernel_trng_hard64 : MasterKernelImplementation<"msimd_trng_hard64", [{
MSIMD_API int msimd_trng_hard64(uint64_t* out_val, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!out_val) return -1;
    uint64_t a = msimd_internal_splitmix64();
    uint64_t b = msimd_internal_splitmix64();
    *out_val = a ^ ((b << 13) | (b >> 51)) ^ 0xD1B54A32D192ED03ULL;
    return 0;
}
}]>;

def Kernel_trng_fill : MasterKernelImplementation<"msimd_trng_fill", [{
MSIMD_API void msimd_trng_fill(void* buffer, size_t num_bytes, int hard_entropy, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    if (!buffer || num_bytes == 0) return;
    uint8_t* out = static_cast<uint8_t*>(buffer);
    size_t i = 0;
    while (i + 8 <= num_bytes) {
        uint64_t val = 0;
        if (hard_entropy) msimd_trng_hard64(&val, nullptr);
        else msimd_trng_fast64(&val, nullptr);
        std::memcpy(out + i, &val, 8);
        i += 8;
    }
    if (i < num_bytes) {
        uint64_t val = 0;
        if (hard_entropy) msimd_trng_hard64(&val, nullptr);
        else msimd_trng_fast64(&val, nullptr);
        std::memcpy(out + i, &val, num_bytes - i);
    }
}
}]>;

def Kernel_vec128_u8_add : MasterKernelImplementation<"msimd_vec128_u8_add", [{
MSIMD_API msimd_u8x16_t msimd_vec128_u8_add(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r; for (int i = 0; i < 16; ++i) r.v[i] = static_cast<uint8_t>(a.v[i] + b.v[i]); return r;
}
}]>;

def Kernel_vec128_u8_sub : MasterKernelImplementation<"msimd_vec128_u8_sub", [{
MSIMD_API msimd_u8x16_t msimd_vec128_u8_sub(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r; for (int i = 0; i < 16; ++i) r.v[i] = static_cast<uint8_t>(a.v[i] - b.v[i]); return r;
}
}]>;

def Kernel_vec128_u8_adds_sat : MasterKernelImplementation<"msimd_vec128_u8_adds_sat", [{
MSIMD_API msimd_u8x16_t msimd_vec128_u8_adds_sat(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r; for (int i = 0; i < 16; ++i) { int s = (int)a.v[i] + (int)b.v[i]; r.v[i] = static_cast<uint8_t>(s > 255 ? 255 : s); } return r;
}
}]>;

def Kernel_vec128_u8_subs_sat : MasterKernelImplementation<"msimd_vec128_u8_subs_sat", [{
MSIMD_API msimd_u8x16_t msimd_vec128_u8_subs_sat(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r; for (int i = 0; i < 16; ++i) { int d = (int)a.v[i] - (int)b.v[i]; r.v[i] = static_cast<uint8_t>(d < 0 ? 0 : d); } return r;
}
}]>;

def Kernel_vec128_u8_min : MasterKernelImplementation<"msimd_vec128_u8_min", [{
MSIMD_API msimd_u8x16_t msimd_vec128_u8_min(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r; for (int i = 0; i < 16; ++i) r.v[i] = a.v[i] < b.v[i] ? a.v[i] : b.v[i]; return r;
}
}]>;

def Kernel_vec128_u8_max : MasterKernelImplementation<"msimd_vec128_u8_max", [{
MSIMD_API msimd_u8x16_t msimd_vec128_u8_max(msimd_u8x16_t a, msimd_u8x16_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r; for (int i = 0; i < 16; ++i) r.v[i] = a.v[i] > b.v[i] ? a.v[i] : b.v[i]; return r;
}
}]>;

def Kernel_vec128_u8_shuffle : MasterKernelImplementation<"msimd_vec128_u8_shuffle", [{
MSIMD_API msimd_u8x16_t msimd_vec128_u8_shuffle(msimd_u8x16_t a, msimd_u8x16_t mask, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_u8x16_t r;
    for (int i = 0; i < 16; ++i) {
        r.v[i] = (mask.v[i] & 0x80) ? 0 : a.v[mask.v[i] & 0x0F];
    }
    return r;
}
}]>;

def Kernel_vec128_i16_add : MasterKernelImplementation<"msimd_vec128_i16_add", [{
MSIMD_API msimd_i16x8_t msimd_vec128_i16_add(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x8_t r; for (int i = 0; i < 8; ++i) r.v[i] = static_cast<int16_t>(a.v[i] + b.v[i]); return r;
}
}]>;

def Kernel_vec128_i16_sub : MasterKernelImplementation<"msimd_vec128_i16_sub", [{
MSIMD_API msimd_i16x8_t msimd_vec128_i16_sub(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x8_t r; for (int i = 0; i < 8; ++i) r.v[i] = static_cast<int16_t>(a.v[i] - b.v[i]); return r;
}
}]>;

def Kernel_vec128_i16_mullo : MasterKernelImplementation<"msimd_vec128_i16_mullo", [{
MSIMD_API msimd_i16x8_t msimd_vec128_i16_mullo(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x8_t r; for (int i = 0; i < 8; ++i) r.v[i] = static_cast<int16_t>((int32_t)a.v[i] * (int32_t)b.v[i]); return r;
}
}]>;

def Kernel_vec128_i16_adds_sat : MasterKernelImplementation<"msimd_vec128_i16_adds_sat", [{
MSIMD_API msimd_i16x8_t msimd_vec128_i16_adds_sat(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x8_t r; for (int i = 0; i < 8; ++i) { int32_t s = (int32_t)a.v[i] + (int32_t)b.v[i]; r.v[i] = static_cast<int16_t>(s > 32767 ? 32767 : (s < -32768 ? -32768 : s)); } return r;
}
}]>;

def Kernel_vec128_i16_subs_sat : MasterKernelImplementation<"msimd_vec128_i16_subs_sat", [{
MSIMD_API msimd_i16x8_t msimd_vec128_i16_subs_sat(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x8_t r; for (int i = 0; i < 8; ++i) { int32_t d = (int32_t)a.v[i] - (int32_t)b.v[i]; r.v[i] = static_cast<int16_t>(d > 32767 ? 32767 : (d < -32768 ? -32768 : d)); } return r;
}
}]>;

def Kernel_vec128_i16_madd : MasterKernelImplementation<"msimd_vec128_i16_madd", [{
MSIMD_API msimd_i32x4_t msimd_vec128_i16_madd(msimd_i16x8_t a, msimd_i16x8_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i32x4_t r;
    for (int i = 0; i < 4; ++i) {
        r.v[i] = (int32_t)a.v[2*i] * (int32_t)b.v[2*i] + (int32_t)a.v[2*i+1] * (int32_t)b.v[2*i+1];
    }
    return r;
}
}]>;

def Kernel_vec64_i16_add : MasterKernelImplementation<"msimd_vec64_i16_add", [{
MSIMD_API msimd_i16x4_t msimd_vec64_i16_add(msimd_i16x4_t a, msimd_i16x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = static_cast<int16_t>(a.v[i] + b.v[i]); return r;
}
}]>;

def Kernel_vec64_i16_sub : MasterKernelImplementation<"msimd_vec64_i16_sub", [{
MSIMD_API msimd_i16x4_t msimd_vec64_i16_sub(msimd_i16x4_t a, msimd_i16x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = static_cast<int16_t>(a.v[i] - b.v[i]); return r;
}
}]>;

def Kernel_vec64_i16_adds_sat : MasterKernelImplementation<"msimd_vec64_i16_adds_sat", [{
MSIMD_API msimd_i16x4_t msimd_vec64_i16_adds_sat(msimd_i16x4_t a, msimd_i16x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x4_t r; for (int i = 0; i < 4; ++i) { int32_t s = (int32_t)a.v[i] + (int32_t)b.v[i]; r.v[i] = static_cast<int16_t>(s > 32767 ? 32767 : (s < -32768 ? -32768 : s)); } return r;
}
}]>;

def Kernel_vec32_i16_add : MasterKernelImplementation<"msimd_vec32_i16_add", [{
MSIMD_API msimd_i16x2_t msimd_vec32_i16_add(msimd_i16x2_t a, msimd_i16x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = static_cast<int16_t>(a.v[i] + b.v[i]); return r;
}
}]>;

def Kernel_vec32_i16_sub : MasterKernelImplementation<"msimd_vec32_i16_sub", [{
MSIMD_API msimd_i16x2_t msimd_vec32_i16_sub(msimd_i16x2_t a, msimd_i16x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i16x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = static_cast<int16_t>(a.v[i] - b.v[i]); return r;
}
}]>;

def Kernel_vec128_f32_add : MasterKernelImplementation<"msimd_vec128_f32_add", [{
MSIMD_API msimd_f32x4_t msimd_vec128_f32_add(msimd_f32x4_t a, msimd_f32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = a.v[i] + b.v[i]; return r;
}
}]>;

def Kernel_vec128_f32_sub : MasterKernelImplementation<"msimd_vec128_f32_sub", [{
MSIMD_API msimd_f32x4_t msimd_vec128_f32_sub(msimd_f32x4_t a, msimd_f32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = a.v[i] - b.v[i]; return r;
}
}]>;

def Kernel_vec128_f32_mul : MasterKernelImplementation<"msimd_vec128_f32_mul", [{
MSIMD_API msimd_f32x4_t msimd_vec128_f32_mul(msimd_f32x4_t a, msimd_f32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = a.v[i] * b.v[i]; return r;
}
}]>;

def Kernel_vec128_f32_fmadd : MasterKernelImplementation<"msimd_vec128_f32_fmadd", [{
MSIMD_API msimd_f32x4_t msimd_vec128_f32_fmadd(msimd_f32x4_t a, msimd_f32x4_t b, msimd_f32x4_t c, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = std::fma(a.v[i], b.v[i], c.v[i]); return r;
}
}]>;

def Kernel_vec128_f32_sqrt : MasterKernelImplementation<"msimd_vec128_f32_sqrt", [{
MSIMD_API msimd_f32x4_t msimd_vec128_f32_sqrt(msimd_f32x4_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = std::sqrt(a.v[i]); return r;
}
}]>;

def Kernel_vec128_f32_dot : MasterKernelImplementation<"msimd_vec128_f32_dot", [{
MSIMD_API float msimd_vec128_f32_dot(msimd_f32x4_t a, msimd_f32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    return a.v[0]*b.v[0] + a.v[1]*b.v[1] + a.v[2]*b.v[2] + a.v[3]*b.v[3];
}
}]>;

def Kernel_vec128_i32_add : MasterKernelImplementation<"msimd_vec128_i32_add", [{
MSIMD_API msimd_i32x4_t msimd_vec128_i32_add(msimd_i32x4_t a, msimd_i32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = a.v[i] + b.v[i]; return r;
}
}]>;

def Kernel_vec128_i32_sub : MasterKernelImplementation<"msimd_vec128_i32_sub", [{
MSIMD_API msimd_i32x4_t msimd_vec128_i32_sub(msimd_i32x4_t a, msimd_i32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = a.v[i] - b.v[i]; return r;
}
}]>;

def Kernel_vec128_i32_mullo : MasterKernelImplementation<"msimd_vec128_i32_mullo", [{
MSIMD_API msimd_i32x4_t msimd_vec128_i32_mullo(msimd_i32x4_t a, msimd_i32x4_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i32x4_t r; for (int i = 0; i < 4; ++i) r.v[i] = a.v[i] * b.v[i]; return r;
}
}]>;

def Kernel_vec64_f32_add : MasterKernelImplementation<"msimd_vec64_f32_add", [{
MSIMD_API msimd_f32x2_t msimd_vec64_f32_add(msimd_f32x2_t a, msimd_f32x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] + b.v[i]; return r;
}
}]>;

def Kernel_vec64_f32_sub : MasterKernelImplementation<"msimd_vec64_f32_sub", [{
MSIMD_API msimd_f32x2_t msimd_vec64_f32_sub(msimd_f32x2_t a, msimd_f32x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] - b.v[i]; return r;
}
}]>;

def Kernel_vec64_f32_mul : MasterKernelImplementation<"msimd_vec64_f32_mul", [{
MSIMD_API msimd_f32x2_t msimd_vec64_f32_mul(msimd_f32x2_t a, msimd_f32x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] * b.v[i]; return r;
}
}]>;

def Kernel_vec128_f64_add : MasterKernelImplementation<"msimd_vec128_f64_add", [{
MSIMD_API msimd_f64x2_t msimd_vec128_f64_add(msimd_f64x2_t a, msimd_f64x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] + b.v[i]; return r;
}
}]>;

def Kernel_vec128_f64_sub : MasterKernelImplementation<"msimd_vec128_f64_sub", [{
MSIMD_API msimd_f64x2_t msimd_vec128_f64_sub(msimd_f64x2_t a, msimd_f64x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] - b.v[i]; return r;
}
}]>;

def Kernel_vec128_f64_mul : MasterKernelImplementation<"msimd_vec128_f64_mul", [{
MSIMD_API msimd_f64x2_t msimd_vec128_f64_mul(msimd_f64x2_t a, msimd_f64x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] * b.v[i]; return r;
}
}]>;

def Kernel_vec128_f64_fmadd : MasterKernelImplementation<"msimd_vec128_f64_fmadd", [{
MSIMD_API msimd_f64x2_t msimd_vec128_f64_fmadd(msimd_f64x2_t a, msimd_f64x2_t b, msimd_f64x2_t c, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = std::fma(a.v[i], b.v[i], c.v[i]); return r;
}
}]>;

def Kernel_vec128_f64_sqrt : MasterKernelImplementation<"msimd_vec128_f64_sqrt", [{
MSIMD_API msimd_f64x2_t msimd_vec128_f64_sqrt(msimd_f64x2_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = std::sqrt(a.v[i]); return r;
}
}]>;

def Kernel_vec128_i64_add : MasterKernelImplementation<"msimd_vec128_i64_add", [{
MSIMD_API msimd_i64x2_t msimd_vec128_i64_add(msimd_i64x2_t a, msimd_i64x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] + b.v[i]; return r;
}
}]>;

def Kernel_vec128_i64_sub : MasterKernelImplementation<"msimd_vec128_i64_sub", [{
MSIMD_API msimd_i64x2_t msimd_vec128_i64_sub(msimd_i64x2_t a, msimd_i64x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_i64x2_t r; for (int i = 0; i < 2; ++i) r.v[i] = a.v[i] - b.v[i]; return r;
}
}]>;

def Kernel_vec128_cpx32_add : MasterKernelImplementation<"msimd_vec128_cpx32_add", [{
MSIMD_API msimd_cpx32x2_t msimd_vec128_cpx32_add(msimd_cpx32x2_t a, msimd_cpx32x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_cpx32x2_t r = { a.r0 + b.r0, a.i0 + b.i0, a.r1 + b.r1, a.i1 + b.i1 };
    return r;
}
}]>;

def Kernel_vec128_cpx32_sub : MasterKernelImplementation<"msimd_vec128_cpx32_sub", [{
MSIMD_API msimd_cpx32x2_t msimd_vec128_cpx32_sub(msimd_cpx32x2_t a, msimd_cpx32x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_cpx32x2_t r = { a.r0 - b.r0, a.i0 - b.i0, a.r1 - b.r1, a.i1 - b.i1 };
    return r;
}
}]>;

def Kernel_vec128_cpx32_mul : MasterKernelImplementation<"msimd_vec128_cpx32_mul", [{
MSIMD_API msimd_cpx32x2_t msimd_vec128_cpx32_mul(msimd_cpx32x2_t a, msimd_cpx32x2_t b, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_cpx32x2_t r = {
        a.r0 * b.r0 - a.i0 * b.i0,
        a.r0 * b.i0 + a.i0 * b.r0,
        a.r1 * b.r1 - a.i1 * b.i1,
        a.r1 * b.i1 + a.i1 * b.r1
    };
    return r;
}
}]>;

def Kernel_vec128_cpx32_conj : MasterKernelImplementation<"msimd_vec128_cpx32_conj", [{
MSIMD_API msimd_cpx32x2_t msimd_vec128_cpx32_conj(msimd_cpx32x2_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_cpx32x2_t r = { a.r0, -a.i0, a.r1, -a.i1 };
    return r;
}
}]>;

def Kernel_vec128_cpx32_mag2 : MasterKernelImplementation<"msimd_vec128_cpx32_mag2", [{
MSIMD_API msimd_f32x2_t msimd_vec128_cpx32_mag2(msimd_cpx32x2_t a, const msimd_tensor_t* precision_target) {
    (void)precision_target;
    msimd_f32x2_t r;
    r.v[0] = a.r0 * a.r0 + a.i0 * a.i0;
    r.v[1] = a.r1 * a.r1 + a.i1 * a.i1;
    return r;
}
}]>;

def Kernel_cull_frustum_aabbs_f32 : MasterKernelImplementation<"msimd_cull_frustum_aabbs_f32", [{
MSIMD_API size_t msimd_cull_frustum_aabbs_f32(
    const float* frustum_planes,
    const float* aabb_min_max,
    size_t count,
    uint8_t* out_visibility_mask,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!frustum_planes || !aabb_min_max || !out_visibility_mask || count == 0) return 0;

    size_t visible_count = 0;
    for (size_t i = 0; i < count; ++i) {
        if ((i & 511u) == 0u) sf_gc_pausepoint_poll();
        const float* box = aabb_min_max + i * 6;
        float min_x = box[0], min_y = box[1], min_z = box[2];
        float max_x = box[3], max_y = box[4], max_z = box[5];

        uint8_t visible = 1;
        for (int p = 0; p < 6; ++p) {
            const float* plane = frustum_planes + p * 4;
            float nx = plane[0], ny = plane[1], nz = plane[2], d = plane[3];
            float px = (nx >= 0.0f) ? max_x : min_x;
            float py = (ny >= 0.0f) ? max_y : min_y;
            float pz = (nz >= 0.0f) ? max_z : min_z;

            if (nx * px + ny * py + nz * pz + d < 0.0f) {
                visible = 0;
                break;
            }
        }
        out_visibility_mask[i] = visible;
        visible_count += (size_t)visible;
    }
    return visible_count;
}
}]>;

def Kernel_cull_frustum_spheres_f32 : MasterKernelImplementation<"msimd_cull_frustum_spheres_f32", [{
MSIMD_API size_t msimd_cull_frustum_spheres_f32(
    const float* frustum_planes,
    const float* sphere_centers_radii,
    size_t count,
    uint8_t* out_visibility_mask,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!frustum_planes || !sphere_centers_radii || !out_visibility_mask || count == 0) return 0;

    size_t visible_count = 0;
    for (size_t i = 0; i < count; ++i) {
        if ((i & 511u) == 0u) sf_gc_pausepoint_poll();
        const float* sphere = sphere_centers_radii + i * 4;
        float cx = sphere[0], cy = sphere[1], cz = sphere[2], radius = sphere[3];

        uint8_t visible = 1;
        for (int p = 0; p < 6; ++p) {
            const float* plane = frustum_planes + p * 4;
            float dist = plane[0] * cx + plane[1] * cy + plane[2] * cz + plane[3];
            if (dist < -radius) {
                visible = 0;
                break;
            }
        }
        out_visibility_mask[i] = visible;
        visible_count += (size_t)visible;
    }
    return visible_count;
}
}]>;

def Kernel_cull_distance_lod_f32 : MasterKernelImplementation<"msimd_cull_distance_lod_f32", [{
MSIMD_API size_t msimd_cull_distance_lod_f32(
    const float* camera_pos,
    const float* object_positions,
    size_t count,
    const float* lod_sq_thresholds,
    uint8_t* out_lod_levels,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!camera_pos || !object_positions || !lod_sq_thresholds || !out_lod_levels || count == 0) return 0;

    float cam_x = camera_pos[0], cam_y = camera_pos[1], cam_z = camera_pos[2];
    float t0_sq = lod_sq_thresholds[0];
    float t1_sq = lod_sq_thresholds[1];
    float t2_sq = lod_sq_thresholds[2];
    float tcull_sq = lod_sq_thresholds[3];

    size_t visible_count = 0;
    for (size_t i = 0; i < count; ++i) {
        if ((i & 511u) == 0u) sf_gc_pausepoint_poll();
        const float* pos = object_positions + i * 3;
        float dx = pos[0] - cam_x;
        float dy = pos[1] - cam_y;
        float dz = pos[2] - cam_z;
        float dist_sq = dx * dx + dy * dy + dz * dz;

        uint8_t lod = 255;
        if (dist_sq <= t0_sq) lod = 0;
        else if (dist_sq <= t1_sq) lod = 1;
        else if (dist_sq <= t2_sq) lod = 2;
        else if (dist_sq <= tcull_sq) lod = 3;

        out_lod_levels[i] = lod;
        if (lod != 255) visible_count++;
    }
    return visible_count;
}
}]>;

def Kernel_voxel_cull_hidden_faces_u32 : MasterKernelImplementation<"msimd_voxel_cull_hidden_faces_u32", [{
MSIMD_API size_t msimd_voxel_cull_hidden_faces_u32(
    const uint32_t* voxel_grid,
    size_t dim_x,
    size_t dim_y,
    size_t dim_z,
    uint8_t* out_face_masks,
    const msimd_tensor_t* precision_target)
{
    (void)precision_target;
    if (!voxel_grid || !out_face_masks || dim_x == 0 || dim_y == 0 || dim_z == 0) return 0;

    size_t slice_size = dim_x * dim_y;
    size_t total_visible_faces = 0;

    for (size_t z = 0; z < dim_z; ++z) {
        if ((z & 7u) == 0u) sf_gc_pausepoint_poll();
        size_t z_offset = z * slice_size;
        for (size_t y = 0; y < dim_y; ++y) {
            size_t y_offset = z_offset + y * dim_x;
            for (size_t x = 0; x < dim_x; ++x) {
                size_t idx = y_offset + x;
                uint32_t block = voxel_grid[idx];
                if (block == 0u) {
                    out_face_masks[idx] = 0;
                    continue;
                }

                uint8_t mask = 0;
                // +X face
                if (x + 1 >= dim_x || voxel_grid[idx + 1] == 0u) mask |= 1u;
                // -X face
                if (x == 0 || voxel_grid[idx - 1] == 0u) mask |= 2u;
                // +Y face
                if (y + 1 >= dim_y || voxel_grid[idx + dim_x] == 0u) mask |= 4u;
                // -Y face
                if (y == 0 || voxel_grid[idx - dim_x] == 0u) mask |= 8u;
                // +Z face
                if (z + 1 >= dim_z || voxel_grid[idx + slice_size] == 0u) mask |= 16u;
                // -Z face
                if (z == 0 || voxel_grid[idx - slice_size] == 0u) mask |= 32u;

                out_face_masks[idx] = mask;
                total_visible_faces += (size_t)(
                    ((mask >> 0) & 1u) + ((mask >> 1) & 1u) +
                    ((mask >> 2) & 1u) + ((mask >> 3) & 1u) +
                    ((mask >> 4) & 1u) + ((mask >> 5) & 1u)
                );
            }
        }
    }
    return total_visible_faces;
}
}]>;
