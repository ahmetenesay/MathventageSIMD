// =============================================================================
// MathventageSIMD — Assembler.td
// Master Synthesis & Cartesian Cross-Product Rulebook
// =============================================================================
// STRICT PHILOSOPHY:
// 1. Acts as the root TableGen entrypoint orchestrating the entire derivation.
// 2. Multiplies Mathematics-Rules.td (Pure Math) with Machines-Rules.td (Pure Hardware)
//    without manual hardware redeclarations or hardcoded SIMD lists.
// 3. Eliminates manual ISA duplication: All ISA properties, register topologies,
//    unrolling strategies, and memory heuristics are dynamically inherited from
//    the MachineISA and DispatchLadderLevel instances defined in Machines-Rules.td.
// 4. Fed directly to codegen.py and llvm-tblgen to synthesize
//    MathventageSIMD-Final.inc without brittle external shell scripts.
// =============================================================================

include "Mathematics-Rules.td"
include "Machines-Rules.td"

// =============================================================================
// SECTION 1: MASTER CARTESIAN CROSS-PRODUCT SYNTHESIS
// =============================================================================
// Synthesizes a concrete mathematical kernel variant by dynamically binding
// an abstract WaveformRule (from Mathematics-Rules.td) to a DispatchLadderLevel
// (from Machines-Rules.td).
// All hardware attributes (ISA, Unroll, MemoryStrategy, Alignment, MinElements)
// are derived directly from the hardware ladder instance!
// =============================================================================

class SynthesizedCartesianKernel<WaveformRule wave, DispatchLadderLevel ladder> {
  WaveformRule Wave = wave;
  DispatchLadderLevel Ladder = ladder;
  MachineISA ISA = ladder.ISA;
  UnrollStrategy Unroll = ladder.Unroll;
  HardwareMemoryStrategy MemoryStrategy = ladder.MemoryStrategy;
  int MinElements = ladder.MinElements;

  // Uniquely articulated semantic symbol name
  string KernelSymbol = wave.ExportName # "_" # ladder.Name # "_u" # !cast<string>(ladder.Unroll.UnrollFactor);
  string DisplayTitle = wave.DisplayName # " [" # ladder.Name # " x" # !cast<string>(ladder.Unroll.UnrollFactor) # "]";
  
  // Pipeline metrics derived from hardware truth
  int VectorLanes = ladder.Unroll.ElementsPerIteration;
  int TargetAlignment = ladder.Unroll.AlignmentBytes;
  int SafepointInterval = ladder.Unroll.SafepointInterval;
  
  // Clean C-ABI Function Prototype
  string ReturnType = "void";
  string ParameterTypes = wave.DataType.CType # "* out, size_t count, double t, double freq, double amp";
  string FunctionPrototype = ReturnType # " " # KernelSymbol # "(" # ParameterTypes # ")";
}

// =============================================================================
// SECTION 2: 2D MATRIX & GEMM CACHE-BLOCKING CROSS-PRODUCT
// =============================================================================
// Multiplies Linear Algebra operators with BLIS Cache Blocking Strategies and
// hardware 2D Matrix Tile Engines (AMX/SME) defined in Machines-Rules.td.
// =============================================================================

class SynthesizedMatrixKernel<WaveformRule math_op, BLIS_CacheBlockingStrategy cache_strat, MachineISA isa> {
  WaveformRule MathOp = math_op;
  BLIS_CacheBlockingStrategy CacheStrategy = cache_strat;
  MachineISA ISA = isa;

  string KernelSymbol = math_op.ExportName # "_" # cache_strat.Name # "_" # isa.Name;
  string DisplayTitle = math_op.DisplayName # " [GEMM " # cache_strat.Name # " / " # isa.Name # "]";
  
  // Cache and register tile geometry derived from hardware
  int TileMR = cache_strat.MR;
  int TileNR = cache_strat.NR;
  int BlockKC = cache_strat.KC;
  int BlockMC = cache_strat.MC;
  int BlockNC = cache_strat.NC;
  int PackAlignment = cache_strat.PackAlignmentBytes;
  int PrefetchAheadA = cache_strat.PrefetchLinesAhead_A;
  int PrefetchAheadB = cache_strat.PrefetchLinesAhead_B;
}

// =============================================================================
// SECTION 3: LEGACY & DIRECT VARIANT COMPATIBILITY
// =============================================================================
// Allows explicit instantiation where a user specifies custom unroll parameters.
// =============================================================================

class SynthesizedKernelVariant<WaveformRule wave, MachineISA isa, UnrollStrategy unroll, HardwareMemoryStrategy mem_strat> {
  WaveformRule Wave = wave;
  MachineISA ISA = isa;
  UnrollStrategy Unroll = unroll;
  HardwareMemoryStrategy MemoryStrategy = mem_strat;

  string KernelSymbol = wave.ExportName # "_" # isa.Name # "_u" # !cast<string>(unroll.UnrollFactor);
  string DisplayTitle = wave.DisplayName # " [" # isa.Name # " x" # !cast<string>(unroll.UnrollFactor) # "]";
  
  int VectorLanes = unroll.ElementsPerIteration;
  int TargetAlignment = unroll.AlignmentBytes;
  int SafepointInterval = unroll.SafepointInterval;
  
  string ReturnType = "void";
  string ParameterTypes = wave.DataType.CType # "* out, size_t count, double t, double freq, double amp";
  string FunctionPrototype = ReturnType # " " # KernelSymbol # "(" # ParameterTypes # ")";
}

// =============================================================================
// SECTION 4: SFVM 4-POINT EXECUTION COORDINATION & INTERLOCK ARCHITECTURE
// =============================================================================
// Declarative specification of the 4 execution checkpoints injected into JIT/AOT kernels:
//  1. Point_Safepoint: Thread pause & Stop-The-World GC synchronization point.
//  2. Point_Controlpoint: Heap pressure monitoring & dynamic allocation pacing point.
//  3. Point_Hookpoint: Host runtime (Python/Rust/C#) and telemetry event dispatch point.
//  4. Point_Interruptpoint: Tight SIMD loop preemption & cooperative time-slice yield point.
// =============================================================================

class SFVM_Point<string name, string ir_symbol, string trigger_boundary, string purpose> {
  string Name = name;
  string IRSymbol = ir_symbol;
  string TriggerBoundary = trigger_boundary; // "Prologue", "LoopHeader", "LoopBody", "Epilogue"
  string Purpose = purpose;
}

// 1. SAFEPOINT: Thread dondurma ve GC Stop-The-World senkronizasyonu
def Point_Safepoint : SFVM_Point<
    "Safepoint",
    "sf_gc_safepoint_poll",
    "Prologue_And_LoopBody",
    "Polls GC phase and yields execution if stop-the-world or drain is requested">;

// 2. CONTROLPOINT: Bellek baskisi ve allocation pacing denetimi
def Point_Controlpoint : SFVM_Point<
    "Controlpoint",
    "sf_gc_controlpoint_check",
    "LoopHeader",
    "Monitors heap pressure and paces allocation if threshold exceeds 85%">;

// 3. HOOKPOINT: Host diller (Python/Rust/C#) ve telemetri köprüsü
def Point_Hookpoint : SFVM_Point<
    "Hookpoint",
    "sf_vm_hookpoint_trigger",
    "Prologue_And_Epilogue",
    "Fires telemetry events and host callbacks on phase transitions and kernel boundaries">;

// 4. INTERRUPTPOINT (YIELDPOINT): Agir SIMD döngülerinde time-slicing
def Point_Interruptpoint : SFVM_Point<
    "Interruptpoint",
    "sf_vm_interruptpoint_poll",
    "LoopBody_Unrolled",
    "Prevents thread starvation in tight SIMD loops by allowing cooperative preemption every SafepointInterval">;

class SFVM_InterlockStrategy<string name,
                             list<SFVM_Point> prologue_points,
                             list<SFVM_Point> loop_header_points,
                             list<SFVM_Point> loop_body_points,
                             list<SFVM_Point> epilogue_points,
                             int default_interrupt_interval> {
  string Name = name;
  list<SFVM_Point> ProloguePoints = prologue_points;
  list<SFVM_Point> LoopHeaderPoints = loop_header_points;
  list<SFVM_Point> LoopBodyPoints = loop_body_points;
  list<SFVM_Point> EpiloguePoints = epilogue_points;
  int DefaultInterruptInterval = default_interrupt_interval;
}

def SFVM_StandardKernelInterlock : SFVM_InterlockStrategy<
    "SFVM_StandardKernelInterlock",
    [Point_Safepoint, Point_Hookpoint],
    [Point_Controlpoint],
    [Point_Interruptpoint, Point_Safepoint],
    [Point_Hookpoint],
    64>;

// =============================================================================
// SECTION 5: CONCRETE CARTESIAN BINDER INSTANTIATIONS
// =============================================================================

foreach wave = [Wave_Sine_f32, Wave_Sine_f64,
                Wave_Cosine_f32, Wave_Cosine_f64,
                Wave_SymmetricSquare_f32, Wave_SymmetricSquare_f64,
                Wave_SymmetricTriangle_f32, Wave_SymmetricTriangle_f64,
                Wave_SawtoothRampUp_f32, Wave_SawtoothRampUp_f64,
                Wave_SawtoothRampDown_f32, Wave_SawtoothRampDown_f64,
                Wave_Trapezoidal_f32, Wave_Trapezoidal_f64,
                Wave_Staircase_f32, Wave_Staircase_f64] in {
  foreach ladder = [Ladder_L0_AMX_AVX512_Streaming,
                    Ladder_L1_AVX512_FMA_Cached,
                    Ladder_L2_AVX512_VL_Prefetch,
                    Ladder_L3_Zen4_Zen5_DualFPU,
                    Ladder_L4_AVX_VNNI_FMA3,
                    Ladder_L5_AVX2_Streaming_NT,
                    Ladder_L6_AVX2_Cached_Baseline,
                    Ladder_L7_SSE42_128Bit,
                    Ladder_L8_SRAM_Partitioned,
                    Ladder_L9_Pure_Scalar_Fallback,
                    Ladder_L10_ARM_SVE2,
                    Ladder_L11_ARM_NEON,
                    Ladder_L12_RISCV_Vector,
                    Ladder_L13_LoongArch_LASX,
                    Ladder_L14_Embedded_DSP_PIE,
                    Ladder_L15_Hardware_Coprocessor,
                    Ladder_L16_SoftFloat_Q31,
                    Ladder_L17_AVX10_2_Converged_512] in {
    def Cartesian_#wave#_#ladder : SynthesizedCartesianKernel<wave, ladder>;
  }
}

foreach wave = [Wave_Sine_f32, Wave_Sine_f64, Wave_Cosine_f32, Wave_Cosine_f64] in {
  def Matrix_#wave#_AVX2     : SynthesizedMatrixKernel<wave, Tile_AVX2_Haswell_Skylake, ISA_x86_AVX2>;
  def Matrix_#wave#_AVX512   : SynthesizedMatrixKernel<wave, Tile_AVX512_SkylakeX_ICL, ISA_x86_AVX512F>;
  def Matrix_#wave#_Zen5     : SynthesizedMatrixKernel<wave, Tile_AVX512_Zen4_Zen5, ISA_x86_AVX512F>;
  def Matrix_#wave#_Zen6     : SynthesizedMatrixKernel<wave, Tile_AVX512_Zen6_3DVCache, ISA_x86_AVX512F>;
  def Matrix_#wave#_AVX10_2  : SynthesizedMatrixKernel<wave, Tile_AVX10_2_NovaLake_bLLC, ISA_x86_AVX10_2>;
  def Matrix_#wave#_AppleM4  : SynthesizedMatrixKernel<wave, Tile_Apple_M1_M4, ISA_ARM_NEON>;
  def Matrix_#wave#_SVE2     : SynthesizedMatrixKernel<wave, Tile_ARM_NeoverseV2_SVE2, ISA_ARM_SVE2>;
  def Matrix_#wave#_RVV      : SynthesizedMatrixKernel<wave, Tile_RISCV_Vector_V1, ISA_RISCV_Vector>;
}

// =============================================================================
// SECTION 6: MULTI-LIMB EMULATED & HIGH-PRECISION KERNEL SYNTHESIS
// =============================================================================
// Synthesizes an emulated multi-limb arithmetic kernel variant binding a
// high-precision or multi-limb WaveformRule with a PrecisionExecutionRoute.
// Handles both native hardware ALUs (IBM Power/zArch) and multi-limb emulation
// (x86-64, ARM64, RISC-V, Universal 256b).
// =============================================================================

class SynthesizedEmulatedKernel<WaveformRule wave, PrecisionExecutionRoute route> {
  WaveformRule Wave = wave;
  PrecisionExecutionRoute Route = route;
  MultiLimbSchema Schema = route.EmulationSchema;
  bit IsNative = route.IsNativeALU;
  bit IsEmulated = route.IsEmulated;

  string KernelSymbol = wave.ExportName # "_" # route.Name # "_" # Schema.Name;
  string DisplayTitle = wave.DisplayName # " [" # route.Name # " / " # Schema.Name # "]";

  int TotalBits = Schema.TotalBits;
  int LimbCount = Schema.LimbCount;
  int LimbWidth = Schema.LimbWidthBits;

  string ReturnType = "void";
  string ParameterTypes = wave.DataType.CType # "* out, size_t count, double t, double freq, double amp";
  string FunctionPrototype = ReturnType # " " # KernelSymbol # "(" # ParameterTypes # ")";
}

foreach wave = [Wave_Sine_f64, Wave_Cosine_f64] in {
  def EmuKernel_#wave#_NativePower  : SynthesizedEmulatedKernel<wave, Route_128b_Native_IBMPower>;
  def EmuKernel_#wave#_Emux86       : SynthesizedEmulatedKernel<wave, Route_128b_Emulated_x86_64_Float>;
  def EmuKernel_#wave#_EmuARM       : SynthesizedEmulatedKernel<wave, Route_128b_Emulated_AArch64_Float>;
  def EmuKernel_#wave#_EmuUniversal : SynthesizedEmulatedKernel<wave, Route_256b_Emulated_Universal_QuadDouble>;
}
