/**
 * SacculusForge SFVM & MathventageSIMD Telemetry Protocol Types
 */

export type HardwareTier = 'ALT' | 'ORTA_ALT' | 'ORTA_ORTA' | 'ORTA_UST' | 'SUR';
export type VMProfile = 'NORMAX' | 'SSIOLSC';
export type SIMDTarget = 'SCALAR' | 'SSE4_2' | 'AVX2' | 'AVX512' | 'OPENVINO_NPU' | 'WASM_SIMD128';

export interface MemoryTopographyState {
  eggCount: number;
  fledglingCount: number;
  pitstopCount: number;
  fallineEmptyBoxes: number;
  tankRegionsBytes: number;
  landfillEvacuatedBytes: number;
  puzzleArenaActiveTests: number;
  totalAllocatedBytes: number;
  rmrpReclaimedBytes: number;
}

export interface LatencySample {
  timestamp: number;
  cycles: number;
  nanoseconds: number;
  kernelName: string;
  isa: SIMDTarget;
}

export interface LatencyHistogramStats {
  minNs: number;
  maxNs: number;
  avgNs: number;
  p50Ns: number;
  p90Ns: number;
  p95Ns: number;
  p99Ns: number;
  p999Ns: number;
  stdDevNs: number;
  jitterNs: number;
  sampleCount: number;
  bins: { binStartNs: number; binEndNs: number; count: number }[];
}

export interface RooflineKernelPoint {
  id: string;
  name: string;
  arithmeticIntensity: number; // FLOPs per Byte
  performanceGflops: number;   // Attainable GFLOPS
  isa: SIMDTarget;
  boundType: 'memory_bound' | 'compute_bound';
  timestamp: number;
}

export interface HardwareCeilings {
  cpuModel: string;
  tier: HardwareTier;
  mhz: number;
  pCores: number;
  eCores: number;
  l1dBandwidthGBs: number;
  l2BandwidthGBs: number;
  l3BandwidthGBs: number;
  dramBandwidthGBs: number;
  scalarPeakGflops: number;
  ssePeakGflops: number;
  avx2PeakGflops: number;
  avx512PeakGflops: number;
  npuPeakGflops: number;
}

export interface BenchmarkMetrics {
  currentGflops: number;
  peakGflops: number;
  currentThroughputGBs: number;
  estimatedIPC: number;
  totalOperations: number;
  dispatchedAluCount: number;
  speedupVsScalar: number;
  speedupVsWasm: number;
  activeKernel: string;
  activeProfile: VMProfile;
  activeIsa: SIMDTarget;
}

export interface LiveTelemetryFrame {
  timestamp: number;
  frameIndex: number;
  cpuMhz: number;
  topography: MemoryTopographyState;
  metrics: BenchmarkMetrics;
  recentLatencies: LatencySample[];
  rooflinePoints: RooflineKernelPoint[];
  selfHealStats: {
    corruptionsHealed: number;
    brooksCyclesBroken: number;
    codeErrorsCaptured: number;
    bitcodeExceptionsShielded: number;
  };
  fourPointStatus: {
    safepointPolls: number;
    controlpointChecks: number;
    hookpointEnters: number;
    hookpointExits: number;
    pausepointPolls: number;
    pausepointArmed: boolean;
  };
}

export interface BenchmarkConfig {
  kernel: 'lattice_fdtd_3d' | 'dense_gemm_f32' | 'complex_vector_mul' | 'vector_fma' | 'cordic_sin_cos' | 'neumaier_sum';
  workloadSize: number; // e.g. 1024, 65536, 1048576
  targetIsa: SIMDTarget;
  profile: VMProfile;
  allowAluDispatch: boolean;
  rmrpMethod: 'PLUMP' | 'SEQUENTIAL' | 'OFF';
  runMode: 'CONTINUOUS' | 'BURST_1000' | 'SINGLE_STEP';
}
