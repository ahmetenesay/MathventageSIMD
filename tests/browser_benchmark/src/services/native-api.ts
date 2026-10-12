/**
 * =========================================================================
 * MathventageSIMD & SacculusForge SFVM TypeScript API Bindings
 * =========================================================================
 * Directly mirrors the public C ABI declarations from:
 *   1. include/SacculusForge.h (Virtual Machine, Allocator, Topology & Safepoints)
 *   2. include/MathventageSIMD.h (Vector Mathematical & Spatial Culling Kernels)
 */

export interface SFVM_ThreadBudget {
  gcThreads: number;
  puzzleThreads: number;
  vmThreads: number;
}

export interface SFVM_RunwayZones {
  eggCount: number;
  fledglingCount: number;
  pitstopCount: number;
}

export interface SFVM_SelfHealStats {
  corruptionsHealed: number;
  brooksCyclesBroken: number;
  codeErrorsCaptured: number;
  bitcodeExceptionsShielded: number;
}

export enum SFVM_ExecutionFocus {
  AUTO = 0,
  SCP = 1,        // Single-Core Performance (Turbo P-Core Pinning, cache pre-warming)
  MCP = 2,        // Multi-Core Performance (Wide parallel NUMA chunking, streaming stores)
  HYBRID_DUAL = 3 // Asymmetric Dual Performance (P-Cores math, E-Cores GC & compilation)
}

export enum SFVM_RmrpMethod {
  PLUMP = 0,      // Compact all, merge into single contiguous block, return to OS
  SEQUENTIAL = 1  // Compact small-to-large, rebate in iterative installments
}

/**
 * Pure TypeScript Mathematical Implementations
 * Used for in-browser client-side execution, baseline comparison, and WebAssembly verification.
 */
export class MathventageSIMD_TS {
  /**
   * 4x4 Single-Precision Matrix Multiplication (C = A * B)
   * Corresponds to `msimd_mat4f_mul` from MathventageSIMD.h
   */
  public static mat4f_mul(a: Float32Array, b: Float32Array, c: Float32Array): void {
    for (let r = 0; r < 4; ++r) {
      const r4 = r * 4;
      for (let col = 0; col < 4; ++col) {
        let sum = 0.0;
        for (let k = 0; k < 4; ++k) {
          sum += a[r4 + k] * b[k * 4 + col];
        }
        c[r4 + col] = sum;
      }
    }
  }

  /**
   * 4x4 Double-Precision Matrix Multiplication (C = A * B)
   * Corresponds to `msimd_mat4d_mul` from MathventageSIMD.h
   */
  public static mat4d_mul(a: Float64Array, b: Float64Array, c: Float64Array): void {
    for (let r = 0; r < 4; ++r) {
      const r4 = r * 4;
      for (let col = 0; col < 4; ++col) {
        let sum = 0.0;
        for (let k = 0; k < 4; ++k) {
          sum += a[r4 + k] * b[k * 4 + col];
        }
        c[r4 + col] = sum;
      }
    }
  }

  /**
   * 6-Plane Frustum vs Axis-Aligned Bounding Box (AABB) Visibility Culling
   * Corresponds to `msimd_cull_frustum_aabbs_f32` from MathventageSIMD.h
   */
  public static cull_frustum_aabbs(
    planes: Float32Array, // 24 floats (6 planes * 4 coefficients)
    mins: Float32Array,   // count * 3 floats
    maxs: Float32Array,   // count * 3 floats
    count: number,
    visible: Uint8Array   // count bytes output
  ): number {
    let visibleCount = 0;

    for (let i = 0; i < count; ++i) {
      const idx3 = i * 3;
      const minX = mins[idx3], minY = mins[idx3 + 1], minZ = mins[idx3 + 2];
      const maxX = maxs[idx3], maxY = maxs[idx3 + 1], maxZ = maxs[idx3 + 2];

      let isInside = 1;
      for (let p = 0; p < 6; ++p) {
        const p4 = p * 4;
        const nx = planes[p4], ny = planes[p4 + 1], nz = planes[p4 + 2], d = planes[p4 + 3];

        // Compute positive vertex (p-vertex) in direction of plane normal
        const px = nx > 0 ? maxX : minX;
        const py = ny > 0 ? maxY : minY;
        const pz = nz > 0 ? maxZ : minZ;

        if (nx * px + ny * py + nz * pz + d < 0) {
          isInside = 0;
          break;
        }
      }

      visible[i] = isInside;
      if (isInside) visibleCount++;
    }

    return visibleCount;
  }

  /**
   * Distance-Based Level of Detail (LOD) with Hysteresis
   * Corresponds to `msimd_cull_distance_lod_f32` from MathventageSIMD.h
   */
  public static cull_distance_lod(
    objPos: Float32Array, // count * 3
    camPos: Float32Array, // 3 floats
    nearDist: number,
    farDist: number,
    hysteresis: number,
    count: number,
    lodOut: Uint8Array
  ): void {
    const cx = camPos[0], cy = camPos[1], cz = camPos[2];
    const nearSq = nearDist * nearDist;
    const farSq = (farDist + hysteresis) * (farDist + hysteresis);

    for (let i = 0; i < count; ++i) {
      const i3 = i * 3;
      const dx = objPos[i3] - cx;
      const dy = objPos[i3 + 1] - cy;
      const dz = objPos[i3 + 2] - cz;
      const distSq = dx * dx + dy * dy + dz * dz;

      if (distSq < nearSq) {
        lodOut[i] = 0; // High LOD
      } else if (distSq < farSq) {
        lodOut[i] = 1; // Medium LOD
      } else {
        lodOut[i] = 2; // Low LOD / Culled
      }
    }
  }

  /**
   * Neumaier Precision Compensated Summation
   * Corresponds to `msimd_neumaier_sum_f32` from MathventageSIMD.h
   */
  public static neumaier_sum(arr: Float32Array, count: number): number {
    let sum = arr[0] || 0.0;
    let c = 0.0;

    for (let i = 1; i < count; ++i) {
      const t = sum + arr[i];
      if (Math.abs(sum) >= Math.abs(arr[i])) {
        c += (sum - t) + arr[i];
      } else {
        c += (arr[i] - t) + sum;
      }
      sum = t;
    }

    return sum + c;
  }

  /**
   * Waveform Supersaw-7 Generator
   * Corresponds to `msimd_wave_supersaw_7` from MathventageSIMD.h
   */
  public static wave_supersaw_7(t: number, freq: number): number {
    const detunes = [-0.11, -0.06, -0.02, 0.0, 0.02, 0.06, 0.11];
    let sum = 0.0;

    for (let i = 0; i < 7; ++i) {
      const f = freq * (1.0 + detunes[i]);
      const phase = (t * f) % 1.0;
      sum += (phase * 2.0 - 1.0);
    }

    return sum / 7.0;
  }
}

/**
 * Benchmark Result Object
 */
export interface TS_BenchmarkResult {
  kernelName: string;
  operations: number;
  totalTimeMs: number;
  gflops: number;
  latencyNs: number;
  throughputGBs: number;
}

/**
 * High-Precision TypeScript Benchmark Suite Runner
 */
export class TS_BenchmarkRunner {
  public static runBenchmark(kernelName: string, count: number = 65536): TS_BenchmarkResult {
    const t0 = performance.now();
    let ops = count;

    if (kernelName === 'msimd_mat4f_mul') {
      const a = new Float32Array(16).fill(1.5);
      const b = new Float32Array(16).fill(2.0);
      const c = new Float32Array(16);
      const iterations = Math.max(100, Math.floor(count / 16));
      for (let i = 0; i < iterations; ++i) {
        MathventageSIMD_TS.mat4f_mul(a, b, c);
      }
      ops = iterations * 64; // 64 FLOPs per 4x4 matrix multiply
    } else if (kernelName === 'msimd_cull_distance_lod_f32') {
      const objPos = new Float32Array(count * 3);
      const camPos = new Float32Array([0, 0, 0]);
      const lodOut = new Uint8Array(count);
      MathventageSIMD_TS.cull_distance_lod(objPos, camPos, 10, 100, 2, count, lodOut);
      ops = count * 8;
    } else if (kernelName === 'msimd_wave_supersaw_7') {
      let sum = 0;
      for (let i = 0; i < count; ++i) {
        sum += MathventageSIMD_TS.wave_supersaw_7(i * 0.0001, 440);
      }
      ops = count * 14;
    } else if (kernelName === 'msimd_neumaier_sum_f32') {
      const arr = new Float32Array(count).fill(1.0001);
      MathventageSIMD_TS.neumaier_sum(arr, count);
      ops = count * 6;
    }

    const t1 = performance.now();
    const elapsedMs = Math.max(0.001, t1 - t0);
    const elapsedSec = elapsedMs / 1000.0;
    const gflops = Math.round(((ops / elapsedSec) / 1e9) * 100) / 100;
    const latencyNs = Math.round((elapsedMs * 1e6) / (count || 1));
    const throughputGBs = Math.round((gflops / 18.5) * 100) / 100;

    return {
      kernelName,
      operations: ops,
      totalTimeMs: Math.round(elapsedMs * 100) / 100,
      gflops,
      latencyNs,
      throughputGBs
    };
  }
}
