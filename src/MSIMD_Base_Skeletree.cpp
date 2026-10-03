
#include "MathventageSIMD.h"
#include "msimd_internal.h"
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

static inline MSIMDComplex msimd_c_add(MSIMDComplex a, MSIMDComplex b) { return { a.r + b.r, a.i + b.i }; }
static inline MSIMDComplex msimd_c_sub(MSIMDComplex a, MSIMDComplex b) { return { a.r - b.r, a.i - b.i }; }
static inline MSIMDComplex msimd_c_mul(MSIMDComplex a, MSIMDComplex b) { return { a.r * b.r - a.i * b.i, a.r * b.i + a.i * b.r }; }
static inline MSIMDComplex msimd_c_scale(MSIMDComplex a, float s) { return { a.r * s, a.i * s }; }
static inline float     msimd_c_norm_sq(MSIMDComplex a) { return a.r * a.r + a.i * a.i; }

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

static float msimd_pbr_distribution_ggx(const float* n, const float* h, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float n_dot_h = std::max(msimd_vec3f_dot(n, h), 0.0f);
    float n_dot_h2 = n_dot_h * n_dot_h;

    float denom = (n_dot_h2 * (a2 - 1.0f) + 1.0f);
    denom = (float)M_PI * denom * denom;
    return denom > 1e-7f ? (a2 / denom) : 1.0f;
}

static float msimd_pbr_geometry_schlick_ggx(float n_dot_v, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    float denom = n_dot_v * (1.0f - k) + k;
    return denom > 1e-7f ? (n_dot_v / denom) : 1.0f;
}

static float msimd_pbr_geometry_smith(const float* n, const float* v, const float* l, float roughness) {
    float n_dot_v = std::max(msimd_vec3f_dot(n, v), 0.0f);
    float n_dot_l = std::max(msimd_vec3f_dot(n, l), 0.0f);
    float ggx1 = msimd_pbr_geometry_schlick_ggx(n_dot_v, roughness);
    float ggx2 = msimd_pbr_geometry_schlick_ggx(n_dot_l, roughness);
    return ggx1 * ggx2;
}

static void msimd_pbr_fresnel_schlick(float cos_theta, const float* F0, float* out_F) {
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

    float t_max_x = (step_x > 0) ? ((x + 1 - orig[0]) * t_delta_x) : ((orig[0] - x) * t_delta_x);
    float t_max_y = (step_y > 0) ? ((y + 1 - orig[1]) * t_delta_y) : ((orig[1] - y) * t_delta_y);
    float t_max_z = (step_z > 0) ? ((z + 1 - orig[2]) * t_delta_z) : ((orig[2] - z) * t_delta_z);

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
            if (offset > out_pos) break;
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
    while (norm_a > 0.5) { norm_a *= 0.5; ++s; }

    double scale = 1.0 / (double)(1ULL << s);
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

    if (x_opt) std::copy(simplex[0].begin(), simplex[0].end(), x_opt);
    if (f_opt) *f_opt = f_vals[0];
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

}

