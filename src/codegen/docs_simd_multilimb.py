# -*- coding: utf-8 -*-
"""
Module providing Doxygen replacements for 128-bit vectors, FP128, FP8,
multi-limb emulation, coprocessors, soft-float, and leaf kernels in MathventageSIMD.h.
"""

SIMD_MULTILIMB_DOCS = '''/* ========================================================================= */
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
 * Uses Knuth\\'s Algorithm D multi-precision division to ensure exact quotient and remainder computation.
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
 * Uses Knuth\\'s TwoSum algorithm (6 floating-point operations) to compute the exact sum s
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

/** @} */ /* End of MSIMD_Coprocessors */'''
