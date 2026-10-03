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

include "Derived_Mathematics_Codex.td"
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
  string KernelSymbol = wave.ExportName # "_" # ladder.Name # "_u" # ladder.Unroll.UnrollFactor;
  string DisplayTitle = wave.DisplayName # " [" # ladder.Name # " x" # ladder.Unroll.UnrollFactor # "]";
  
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

  string KernelSymbol = wave.ExportName # "_" # isa.Name # "_u" # unroll.UnrollFactor;
  string DisplayTitle = wave.DisplayName # " [" # isa.Name # " x" # unroll.UnrollFactor # "]";
  
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
