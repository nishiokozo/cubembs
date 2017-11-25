//#define _GNU_SOURCE // M_PI が定義される
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <signal.h>

//#ifdef _WIN32
//#pragma comment(linker, "/subsystem:windows")	//	コンソール表示されなくする
//#endif  // _WIN32

#define M_PI	3.14159265358979323846

#define	VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>


//#ifndef LINMATH_H
//#define LINMATH_H

#include <math.h>

#include "key.h"
#include "vk.h"
#include "vk2.h"
#include "win.h"
#include "vect.h"

// Converts degrees to radians.
#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)

// Converts radians to degrees.
#define radiansToDegrees(angleRadians) (angleRadians * 180.0 / M_PI)

typedef float vec3[3];
static inline void vec3_add(vec3 r, vec3 const a, vec3 const b) {
    int i;
    for (i = 0; i < 3; ++i) r[i] = a[i] + b[i];
}
static inline void vec3_sub(vec3 r, vec3 const a, vec3 const b) {
    int i;
    for (i = 0; i < 3; ++i) r[i] = a[i] - b[i];
}
static inline void vec3_scale(vec3 r, vec3 const v, float const s) {
    int i;
    for (i = 0; i < 3; ++i) r[i] = v[i] * s;
}
static inline float vec3_mul_inner(vec3 const a, vec3 const b) {
    float p = 0.f;
    int i;
    for (i = 0; i < 3; ++i) p += b[i] * a[i];
    return p;
}
static inline void vec3_mul_cross(vec3 r, vec3 const a, vec3 const b) {
    r[0] = a[1] * b[2] - a[2] * b[1];
    r[1] = a[2] * b[0] - a[0] * b[2];
    r[2] = a[0] * b[1] - a[1] * b[0];
}
static inline float vec3_len(vec3 const v) { return sqrtf(vec3_mul_inner(v, v)); }
static inline void vec3_norm(vec3 r, vec3 const v) {
    float k = 1.f / vec3_len(v);
    vec3_scale(r, v, k);
}
static inline void vec3_reflect(vec3 r, vec3 const v, vec3 const n) {
    float p = 2.f * vec3_mul_inner(v, n);
    int i;
    for (i = 0; i < 3; ++i) r[i] = v[i] - p * n[i];
}

typedef float vec4[4];
static inline void vec4_add(vec4 r, vec4 const a, vec4 const b) {
    int i;
    for (i = 0; i < 4; ++i) r[i] = a[i] + b[i];
}
static inline void vec4_sub(vec4 r, vec4 const a, vec4 const b) {
    int i;
    for (i = 0; i < 4; ++i) r[i] = a[i] - b[i];
}
static inline void vec4_scale(vec4 r, vec4 v, float s) {
    int i;
    for (i = 0; i < 4; ++i) r[i] = v[i] * s;
}
static inline float vec4_mul_inner(vec4 a, vec4 b) {
    float p = 0.f;
    int i;
    for (i = 0; i < 4; ++i) p += b[i] * a[i];
    return p;
}
static inline void vec4_mul_cross(vec4 r, vec4 a, vec4 b) {
    r[0] = a[1] * b[2] - a[2] * b[1];
    r[1] = a[2] * b[0] - a[0] * b[2];
    r[2] = a[0] * b[1] - a[1] * b[0];
    r[3] = 1.f;
}
static inline float vec4_len(vec4 v) { return sqrtf(vec4_mul_inner(v, v)); }
static inline void vec4_norm(vec4 r, vec4 v) {
    float k = 1.f / vec4_len(v);
    vec4_scale(r, v, k);
}
static inline void vec4_reflect(vec4 r, vec4 v, vec4 n) {
    float p = 2.f * vec4_mul_inner(v, n);
    int i;
    for (i = 0; i < 4; ++i) r[i] = v[i] - p * n[i];
}

typedef vec4 mat4x4[4];
static inline void mat4x4_identity(mat4x4 M) {
    int i, j;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j) M[i][j] = i == j ? 1.f : 0.f;
}
static inline void mat4x4_dup(mat4x4 M, mat4x4 N) {
    int i, j;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j) M[i][j] = N[i][j];
}
static inline void mat4x4_row(vec4 r, mat4x4 M, int i) {
    int k;
    for (k = 0; k < 4; ++k) r[k] = M[k][i];
}
static inline void mat4x4_col(vec4 r, mat4x4 M, int i) {
    int k;
    for (k = 0; k < 4; ++k) r[k] = M[i][k];
}
static inline void mat4x4_transpose(mat4x4 M, mat4x4 N) {
    int i, j;
    for (j = 0; j < 4; ++j)
        for (i = 0; i < 4; ++i) M[i][j] = N[j][i];
}
static inline void mat4x4_add(mat4x4 M, mat4x4 a, mat4x4 b) {
    int i;
    for (i = 0; i < 4; ++i) vec4_add(M[i], a[i], b[i]);
}
static inline void mat4x4_sub(mat4x4 M, mat4x4 a, mat4x4 b) {
    int i;
    for (i = 0; i < 4; ++i) vec4_sub(M[i], a[i], b[i]);
}
static inline void mat4x4_scale(mat4x4 M, mat4x4 a, float k) {
    int i;
    for (i = 0; i < 4; ++i) vec4_scale(M[i], a[i], k);
}
static inline void mat4x4_scale_aniso(mat4x4 M, mat4x4 a, float x, float y, float z) {
    int i;
    vec4_scale(M[0], a[0], x);
    vec4_scale(M[1], a[1], y);
    vec4_scale(M[2], a[2], z);
    for (i = 0; i < 4; ++i) {
        M[3][i] = a[3][i];
    }
}
static inline void mat4x4_mul(mat4x4 M, mat4x4 a, mat4x4 b) {
    int k, r, c;
    for (c = 0; c < 4; ++c)
        for (r = 0; r < 4; ++r) {
            M[c][r] = 0.f;
            for (k = 0; k < 4; ++k) M[c][r] += a[k][r] * b[c][k];
        }
}
static inline void mat4x4_mul_vec4(vec4 r, mat4x4 M, vec4 v) {
    int i, j;
    for (j = 0; j < 4; ++j) {
        r[j] = 0.f;
        for (i = 0; i < 4; ++i) r[j] += M[i][j] * v[i];
    }
}
static inline void mat4x4_translate(mat4x4 T, float x, float y, float z) {
    mat4x4_identity(T);
    T[3][0] = x;
    T[3][1] = y;
    T[3][2] = z;
}
static inline void mat4x4_translate_in_place(mat4x4 M, float x, float y, float z) {
    vec4 t = {x, y, z, 0};
    vec4 r;
    int i;
    for (i = 0; i < 4; ++i) {
        mat4x4_row(r, M, i);
        M[3][i] += vec4_mul_inner(r, t);
    }
}
static inline void mat4x4_from_vec3_mul_outer(mat4x4 M, vec3 a, vec3 b) {
    int i, j;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j) M[i][j] = i < 3 && j < 3 ? a[i] * b[j] : 0.f;
}
static inline void mat4x4_rotate(mat4x4 R, mat4x4 M, float x, float y, float z, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);
    vec3 u = {x, y, z};

    if (vec3_len(u) > 1e-4) {
        vec3_norm(u, u);
        mat4x4 T;
        mat4x4_from_vec3_mul_outer(T, u, u);

        mat4x4 S = {{0, u[2], -u[1], 0}, {-u[2], 0, u[0], 0}, {u[1], -u[0], 0, 0}, {0, 0, 0, 0}};
        mat4x4_scale(S, S, s);

        mat4x4 C;
        mat4x4_identity(C);
        mat4x4_sub(C, C, T);

        mat4x4_scale(C, C, c);

        mat4x4_add(T, T, C);
        mat4x4_add(T, T, S);

        T[3][3] = 1.;
        mat4x4_mul(R, M, T);
    } else {
        mat4x4_dup(R, M);
    }
}
static inline void mat4x4_rotate_X(mat4x4 Q, mat4x4 M, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);
    mat4x4 R = {{1.f, 0.f, 0.f, 0.f}, {0.f, c, s, 0.f}, {0.f, -s, c, 0.f}, {0.f, 0.f, 0.f, 1.f}};
    mat4x4_mul(Q, M, R);
}
static inline void mat4x4_rotate_Y(mat4x4 Q, mat4x4 M, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);
    mat4x4 R = {{c, 0.f, s, 0.f}, {0.f, 1.f, 0.f, 0.f}, {-s, 0.f, c, 0.f}, {0.f, 0.f, 0.f, 1.f}};
    mat4x4_mul(Q, M, R);
}
static inline void mat4x4_rotate_Z(mat4x4 Q, mat4x4 M, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);
    mat4x4 R = {{c, s, 0.f, 0.f}, {-s, c, 0.f, 0.f}, {0.f, 0.f, 1.f, 0.f}, {0.f, 0.f, 0.f, 1.f}};
    mat4x4_mul(Q, M, R);
}
static inline void mat4x4_invert(mat4x4 T, mat4x4 M) {
    float s[6];
    float c[6];
    s[0] = M[0][0] * M[1][1] - M[1][0] * M[0][1];
    s[1] = M[0][0] * M[1][2] - M[1][0] * M[0][2];
    s[2] = M[0][0] * M[1][3] - M[1][0] * M[0][3];
    s[3] = M[0][1] * M[1][2] - M[1][1] * M[0][2];
    s[4] = M[0][1] * M[1][3] - M[1][1] * M[0][3];
    s[5] = M[0][2] * M[1][3] - M[1][2] * M[0][3];

    c[0] = M[2][0] * M[3][1] - M[3][0] * M[2][1];
    c[1] = M[2][0] * M[3][2] - M[3][0] * M[2][2];
    c[2] = M[2][0] * M[3][3] - M[3][0] * M[2][3];
    c[3] = M[2][1] * M[3][2] - M[3][1] * M[2][2];
    c[4] = M[2][1] * M[3][3] - M[3][1] * M[2][3];
    c[5] = M[2][2] * M[3][3] - M[3][2] * M[2][3];

    /* Assumes it is invertible */
    float idet = 1.0f / (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);

    T[0][0] = (M[1][1] * c[5] - M[1][2] * c[4] + M[1][3] * c[3]) * idet;
    T[0][1] = (-M[0][1] * c[5] + M[0][2] * c[4] - M[0][3] * c[3]) * idet;
    T[0][2] = (M[3][1] * s[5] - M[3][2] * s[4] + M[3][3] * s[3]) * idet;
    T[0][3] = (-M[2][1] * s[5] + M[2][2] * s[4] - M[2][3] * s[3]) * idet;

    T[1][0] = (-M[1][0] * c[5] + M[1][2] * c[2] - M[1][3] * c[1]) * idet;
    T[1][1] = (M[0][0] * c[5] - M[0][2] * c[2] + M[0][3] * c[1]) * idet;
    T[1][2] = (-M[3][0] * s[5] + M[3][2] * s[2] - M[3][3] * s[1]) * idet;
    T[1][3] = (M[2][0] * s[5] - M[2][2] * s[2] + M[2][3] * s[1]) * idet;

    T[2][0] = (M[1][0] * c[4] - M[1][1] * c[2] + M[1][3] * c[0]) * idet;
    T[2][1] = (-M[0][0] * c[4] + M[0][1] * c[2] - M[0][3] * c[0]) * idet;
    T[2][2] = (M[3][0] * s[4] - M[3][1] * s[2] + M[3][3] * s[0]) * idet;
    T[2][3] = (-M[2][0] * s[4] + M[2][1] * s[2] - M[2][3] * s[0]) * idet;

    T[3][0] = (-M[1][0] * c[3] + M[1][1] * c[1] - M[1][2] * c[0]) * idet;
    T[3][1] = (M[0][0] * c[3] - M[0][1] * c[1] + M[0][2] * c[0]) * idet;
    T[3][2] = (-M[3][0] * s[3] + M[3][1] * s[1] - M[3][2] * s[0]) * idet;
    T[3][3] = (M[2][0] * s[3] - M[2][1] * s[1] + M[2][2] * s[0]) * idet;
}
static inline void mat4x4_orthonormalize(mat4x4 R, mat4x4 M) {
    mat4x4_dup(R, M);
    float s = 1.;
    vec3 h;

    vec3_norm(R[2], R[2]);

    s = vec3_mul_inner(R[1], R[2]);
    vec3_scale(h, R[2], s);
    vec3_sub(R[1], R[1], h);
    vec3_norm(R[2], R[2]);

    s = vec3_mul_inner(R[1], R[2]);
    vec3_scale(h, R[2], s);
    vec3_sub(R[1], R[1], h);
    vec3_norm(R[1], R[1]);

    s = vec3_mul_inner(R[0], R[1]);
    vec3_scale(h, R[1], s);
    vec3_sub(R[0], R[0], h);
    vec3_norm(R[0], R[0]);
}

static inline void mat4x4_frustum(mat4x4 M, float l, float r, float b, float t, float n, float f) {
    M[0][0] = 2.f * n / (r - l);
    M[0][1] = M[0][2] = M[0][3] = 0.f;

    M[1][1] = 2.f * n / (t - b);
    M[1][0] = M[1][2] = M[1][3] = 0.f;

    M[2][0] = (r + l) / (r - l);
    M[2][1] = (t + b) / (t - b);
    M[2][2] = -(f + n) / (f - n);
    M[2][3] = -1.f;

    M[3][2] = -2.f * (f * n) / (f - n);
    M[3][0] = M[3][1] = M[3][3] = 0.f;
}
static inline void mat4x4_ortho(mat4x4 M, float l, float r, float b, float t, float n, float f) {
    M[0][0] = 2.f / (r - l);
    M[0][1] = M[0][2] = M[0][3] = 0.f;

    M[1][1] = 2.f / (t - b);
    M[1][0] = M[1][2] = M[1][3] = 0.f;

    M[2][2] = -2.f / (f - n);
    M[2][0] = M[2][1] = M[2][3] = 0.f;

    M[3][0] = -(r + l) / (r - l);
    M[3][1] = -(t + b) / (t - b);
    M[3][2] = -(f + n) / (f - n);
    M[3][3] = 1.f;
}
static inline void mat4x4_perspective(mat4x4 m, float y_fov, float aspect, float n, float f) {
    /* NOTE: Degrees are an unhandy unit to work with.
     * linmath.h uses radians for everything! */
    float const a = (float)(1.f / tan(y_fov / 2.f));

    m[0][0] = a / aspect;
    m[0][1] = 0.f;
    m[0][2] = 0.f;
    m[0][3] = 0.f;

    m[1][0] = 0.f;
    m[1][1] = a;
    m[1][2] = 0.f;
    m[1][3] = 0.f;

    m[2][0] = 0.f;
    m[2][1] = 0.f;
    m[2][2] = -((f + n) / (f - n));
    m[2][3] = -1.f;

    m[3][0] = 0.f;
    m[3][1] = 0.f;
    m[3][2] = -((2.f * f * n) / (f - n));
    m[3][3] = 0.f;
}
static inline void mat4x4_look_at(mat4x4 m, vec3 eye, vec3 center, vec3 up) {
    /* Adapted from Android's OpenGL Matrix.java.                        */
    /* See the OpenGL GLUT documentation for gluLookAt for a description */
    /* of the algorithm. We implement it in a straightforward way:       */

    /* TODO: The negation of of can be spared by swapping the order of
     *       operands in the following cross products in the right way. */
    vec3 f;
    vec3_sub(f, center, eye);
    vec3_norm(f, f);

    vec3 s;
    vec3_mul_cross(s, f, up);
    vec3_norm(s, s);

    vec3 t;
    vec3_mul_cross(t, s, f);

    m[0][0] = s[0];
    m[0][1] = t[0];
    m[0][2] = -f[0];
    m[0][3] = 0.f;

    m[1][0] = s[1];
    m[1][1] = t[1];
    m[1][2] = -f[1];
    m[1][3] = 0.f;

    m[2][0] = s[2];
    m[2][1] = t[2];
    m[2][2] = -f[2];
    m[2][3] = 0.f;

    m[3][0] = 0.f;
    m[3][1] = 0.f;
    m[3][2] = 0.f;
    m[3][3] = 1.f;

    mat4x4_translate_in_place(m, -eye[0], -eye[1], -eye[2]);
}

typedef float quat[4];
static inline void quat_identity(quat q) {
    q[0] = q[1] = q[2] = 0.f;
    q[3] = 1.f;
}
static inline void quat_add(quat r, quat a, quat b) {
    int i;
    for (i = 0; i < 4; ++i) r[i] = a[i] + b[i];
}
static inline void quat_sub(quat r, quat a, quat b) {
    int i;
    for (i = 0; i < 4; ++i) r[i] = a[i] - b[i];
}
static inline void quat_mul(quat r, quat p, quat q) {
    vec3 w;
    vec3_mul_cross(r, p, q);
    vec3_scale(w, p, q[3]);
    vec3_add(r, r, w);
    vec3_scale(w, q, p[3]);
    vec3_add(r, r, w);
    r[3] = p[3] * q[3] - vec3_mul_inner(p, q);
}
static inline void quat_scale(quat r, quat v, float s) {
    int i;
    for (i = 0; i < 4; ++i) r[i] = v[i] * s;
}
static inline float quat_inner_product(quat a, quat b) {
    float p = 0.f;
    int i;
    for (i = 0; i < 4; ++i) p += b[i] * a[i];
    return p;
}
static inline void quat_conj(quat r, quat q) {
    int i;
    for (i = 0; i < 3; ++i) r[i] = -q[i];
    r[3] = q[3];
}
#define quat_norm vec4_norm
static inline void quat_mul_vec3(vec3 r, quat q, vec3 v) {
    quat v_ = {v[0], v[1], v[2], 0.f};

    quat_conj(r, q);
    quat_norm(r, r);
    quat_mul(r, v_, r);
    quat_mul(r, q, r);
}
static inline void mat4x4_from_quat(mat4x4 M, quat q) {
    float a = q[3];
    float b = q[0];
    float c = q[1];
    float d = q[2];
    float a2 = a * a;
    float b2 = b * b;
    float c2 = c * c;
    float d2 = d * d;

    M[0][0] = a2 + b2 - c2 - d2;
    M[0][1] = 2.f * (b * c + a * d);
    M[0][2] = 2.f * (b * d - a * c);
    M[0][3] = 0.f;

    M[1][0] = 2 * (b * c - a * d);
    M[1][1] = a2 - b2 + c2 - d2;
    M[1][2] = 2.f * (c * d + a * b);
    M[1][3] = 0.f;

    M[2][0] = 2.f * (b * d + a * c);
    M[2][1] = 2.f * (c * d - a * b);
    M[2][2] = a2 - b2 - c2 + d2;
    M[2][3] = 0.f;

    M[3][0] = M[3][1] = M[3][2] = 0.f;
    M[3][3] = 1.f;
}

static inline void mat4x4o_mul_quat(mat4x4 R, mat4x4 M, quat q) {
    /*  XXX: The way this is written only works for othogonal matrices. */
    /* TODO: Take care of non-orthogonal case. */
    quat_mul_vec3(R[0], q, M[0]);
    quat_mul_vec3(R[1], q, M[1]);
    quat_mul_vec3(R[2], q, M[2]);

    R[3][0] = R[3][1] = R[3][2] = 0.f;
    R[3][3] = 1.f;
}
static inline void quat_from_mat4x4(quat q, mat4x4 M) {
    float r = 0.f;
    int i;

    int perm[] = {0, 1, 2, 0, 1};
    int *p = perm;

    for (i = 0; i < 3; i++) {
        float m = M[i][i];
        if (m < r) continue;
        m = r;
        p = &perm[i];
    }

    r = sqrtf(1.f + M[p[0]][p[0]] - M[p[1]][p[1]] - M[p[2]][p[2]]);

    if (r < 1e-6) {
        q[0] = 1.f;
        q[1] = q[2] = q[3] = 0.f;
        return;
    }

    q[0] = r / 2.f;
    q[1] = (M[p[0]][p[1]] - M[p[1]][p[0]]) / (2.f * r);
    q[2] = (M[p[2]][p[0]] - M[p[0]][p[2]]) / (2.f * r);
    q[3] = (M[p[2]][p[1]] - M[p[1]][p[2]]) / (2.f * r);
}

//#endif




//bool in_callback = false;





// clang-format on

void dumpMatrix(const char *note, mat4x4 MVP) 
{
	int i;

	printf("%s: \n", note);
	for (i = 0; i < 4; i++) 
	{
		printf("%f, %f, %f, %f\n", MVP[i][0], MVP[i][1], MVP[i][2], MVP[i][3]);
	}
	printf("\n");
	fflush(stdout);
}

void dumpVec4(const char *note, vec4 vector) 
{
	printf("%s: \n", note);
	printf("%f, %f, %f, %f\n", vector[0], vector[1], vector[2], vector[3]);
	printf("\n");
	fflush(stdout);
}

	//--------------------------------------------------------------------------------------
	// Mesh and VertexFormat Data
	//--------------------------------------------------------------------------------------
	// clang-format off
	static const float g_vertex_buffer_data[] = 
	{
		-1.0f,-1.0f,-1.0f,  // -X side
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,

		-1.0f,-1.0f,-1.0f,  // -Z side
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,

		-1.0f,-1.0f,-1.0f,  // -Y side
		 1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,

		-1.0f, 1.0f,-1.0f,  // +Y side
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,

		 1.0f, 1.0f,-1.0f,  // +X side
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,

		-1.0f, 1.0f, 1.0f,  // +Z side
		-1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
	};

	static const float g_uv_buffer_data[] = 
	{
		0.0f, 1.0f,  // -X side
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		1.0f, 1.0f,  // -Z side
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		1.0f, 0.0f,  // -Y side
		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 0.0f,  // +Y side
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,

		1.0f, 0.0f,  // +X side
		0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,  // +Z side
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	};


struct vktexcube_vs_uniform 
{
	// Must start with MVP
	float mvp[4][4];
	float position[12 * 3][4];
	float attr[12 * 3][4];
};
mat4x4 apr_projection_matrix;
mat4x4 apr_view_matrix;
mat4x4 apr_model_matrix;

float 	apr_spin_angle;
float 	apr_spin_increment;
bool	apr_pause;

static const	int	unit_MAX = 5;
static vktexcube_vs_uniform*	dataVert			= new vktexcube_vs_uniform[unit_MAX];
static VkBuffer** 				sc_uniform_buffer	= new VkBuffer*[unit_MAX];
static VkDeviceMemory** 		sc_uniform_memory	= new VkDeviceMemory*[unit_MAX];
static VkDescriptorSet** 		sc_descriptor_set	= new VkDescriptorSet*[unit_MAX];
static vect44*					mvp					= new vect44[unit_MAX];
static vect44* 					g_model				= new vect44[unit_MAX];
static vect44 g_view;
static int unit_cnt=0;

static vktexcube_vs_uniform 	dataVert0 = 
{
	{//mvp
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
	},
	{//xyzw
		{ 0.00f ,  0.60f , -0.50f, 0.0f},  // -X side
		{ 0.00f ,  0.00f ,  1.50f, 0.0f},
		{ 0.00f ,  0.00f ,  1.50f, 0.0f},
		{ 0.00f ,  0.00f ,  1.50f, 1.0f},
		{ 0.50f ,  0.00f , -0.50f, 1.0f},
		{ 0.00f ,  0.60f , -0.50f, 1.0f},

		{ 0.00f ,  0.60f , -0.50f, 0.0f},  // -Z side
		{-0.50f ,  0.00f , -0.50f, 0.0f},
		{-0.00f ,  0.60f , -0.50f, 0.0f},
		{ 0.00f ,  0.60f , -0.50f, 1.0f},
		{ 0.50f ,  0.00f , -0.50f, 1.0f},
		{-0.50f ,  0.00f , -0.50f, 1.0f},

		{-0.00f , -0.10f , -0.00f, 0.0f},  // -Y side
		{ 0.00f , -0.10f , -0.00f, 0.0f},
		{ 0.00f , -0.10f ,  0.00f, 0.0f},
		{-0.00f , -0.10f , -0.00f, 0.0f},
		{ 0.00f , -0.10f ,  0.00f, 0.0f},
		{-0.00f , -0.10f ,  0.00f, 0.0f},

		{ 0.50f ,  0.00f , -0.50f, 0.0f},  // +Y side
		{ 0.00f ,  0.00f ,  1.50f, 0.0f},
		{-0.00f ,  0.00f ,  1.50f, 0.0f},
		{ 0.50f ,  0.00f , -0.50f, 1.0f},
		{-0.00f ,  0.00f ,  1.50f, 1.0f},
		{-0.50f ,  0.00f , -0.50f, 1.0f},

		{ 0.00f ,  0.00f ,  0.00f, 0.0f},  // +X side
		{ 0.00f ,  0.00f ,  0.00f, 0.0f},
		{ 0.00f , -0.00f ,  0.00f, 0.0f},
		{-0.00f , -0.00f ,  1.50f, 1.0f},
		{-0.00f ,  0.60f , -0.50f, 1.0f},
		{-0.50f , -0.00f , -0.50f, 1.0f},

		{-0.00f ,  0.00f ,  1.50f, 0.0f},  // +Z side
		{-0.00f , -0.00f ,  1.50f, 0.0f},
		{ 0.00f ,  0.00f ,  1.50f, 0.0f},
		{-0.00f , -0.00f ,  1.50f, 0.0f},
		{ 0.00f , -0.00f ,  1.50f, 0.0f},
		{ 0.00f ,  0.00f ,  1.50f, 0.0f},
	},
	{//normal
		{ 0.0f , 1.0f },   // -X side
		{ 1.0f , 1.0f }, 
		{ 1.0f , 0.0f }, 
		{ 0.75f, 0.63f, 0.19f }, 
		{ 0.75f, 0.63f, 0.19f }, 
		{ 0.75f, 0.63f, 0.19f }, 
		     
		{ 1.0f , 1.0f },   // -Z side
		{ 0.0f , 0.0f }, 
		{ 0.0f , 1.0f }, 
		{ 0.0f , 0.0f,-1.0f }, 
		{ 0.0f , 0.0f,-1.0f }, 
		{ 0.0f , 0.0f,-1.0f }, 
		     
		{ 1.0f , 0.0f },   // -Y side
		{ 1.0f , 1.0f }, 
		{ 0.0f , 1.0f }, 
		{ 1.0f , 0.0f }, 
		{ 0.0f , 1.0f }, 
		{ 0.0f , 0.0f }, 
		     
		{ 1.0f , 0.0f },   // +Y side
		{ 0.0f , 0.0f }, 
		{ 0.0f , 1.0f }, 
		{ 0.0f ,-1.0f, 0.0f }, 
		{ 0.0f ,-1.0f, 0.0f }, 
		{ 0.0f ,-1.0f, 0.0f }, 
		     
		{ 1.0f , 0.0f },   // +X side
		{ 0.0f , 0.0f }, 
		{ 0.0f , 1.0f }, 
		{-0.75f, 0.63f, 0.19f }, 
		{-0.75f, 0.63f, 0.19f }, 
		{-0.75f, 0.63f, 0.19f }, 
		     
		{ 0.0f , 0.0f },   // +Z side
		{ 0.0f , 1.0f }, 
		{ 1.0f , 0.0f }, 
		{ 0.0f , 1.0f }, 
		{ 1.0f , 1.0f }, 
		{ 1.0f , 0.0f }, 
	}
};

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
//-----------------------------------------------------------------------------
{
	//---------------------------------------------------------
	// 透視変換行列の作成
	//---------------------------------------------------------
	{
		apr_spin_angle = 1.0f;
		apr_spin_increment = 0.2f;
		apr_pause = false;

		vec3 eye = {0.0f, 3.0f, 5.0f};
		vec3 origin = {0, 0, 0};
		vec3 up = {0.0f, 1.0f, 0.0};

		mat4x4_perspective(apr_projection_matrix, (float)degreesToRadians(45.0f),1.0f, 0.1f, 100.0f);
		mat4x4_look_at(apr_view_matrix, eye, origin, up);
		mat4x4_identity(apr_model_matrix);

		apr_projection_matrix[1][1]*=-1;  //Flip projection matrix from GL to Vulkan orientation.
	}

	for ( int u = 0; u < unit_MAX; u++ )
	{
		memcpy( &dataVert[u], &dataVert0,  sizeof(vktexcube_vs_uniform) );
		unit_cnt++;
	}

	//---------------------------------------------------------
	// 透視変換行列の作成
	//---------------------------------------------------------
	{
		for ( int u = 0; u < unit_cnt; u++ )
		{
			g_model[u].identity();
			g_model[u].translate( (-(unit_cnt-1)/2)+u*1.0,0,0);
		}
		g_view.identity();
		g_view.translate(0,0,-5);
	}

	{
		for ( int u = 0; u < unit_cnt; u++ )
		{
//		 	g_model[u].rotX(RAD(0.1));
		 	g_model[u].rotY(RAD(80));
		}
		for ( int u = 0; u < unit_cnt; u++ )
		{
			mvp[u].identity();
			mvp[u].perspectiveGL( 45, 512.0/512.0,0.1,100		 );
			mvp[u].m[1][1] *= -1; // GL to Vulkan
			mvp[u] =  g_model[u] * g_view * mvp[u];
		}
	}


	{
		mat4x4 VP;
		mat4x4_mul(VP, apr_projection_matrix, apr_view_matrix);

		mat4x4 MVP;
		mat4x4_mul(MVP, VP, apr_model_matrix);


		for ( int u = 0; u < unit_MAX; u++ )
		{

//			memcpy(dataVert[u].mvp, MVP, sizeof(MVP));
//			memcpy(dataVert[u].mvp, mvp[u].m, sizeof(mvp[u].m));
			//	dumpMatrix("MVP", MVP);

/*
			for (unsigned int i = 0; i < 12 * 3; i++) 
			{
				dataVert[u].position[i][0] = g_vertex_buffer_data[i * 3];
				dataVert[u].position[i][1] = g_vertex_buffer_data[i * 3 + 1];
				dataVert[u].position[i][2] = g_vertex_buffer_data[i * 3 + 2];
				dataVert[u].position[i][3] = 1.0f;
				dataVert[u].attr[i][0] = g_uv_buffer_data[2 * i];
				dataVert[u].attr[i][1] = g_uv_buffer_data[2 * i + 1];
				dataVert[u].attr[i][2] = 0;
				dataVert[u].attr[i][3] = 0;
			}
			unit_cnt++;
*/
		}
	}
	//---
	WinInf* pWin = new WinInf( "msb", 128, 128 );
	VkInf* pVk = new VkInf( pWin->hInstance, pWin->hWin, pWin->win_width, pWin->win_height );

	if ( pVk ) 
	{
//			if ( pVk->flgSetModel== false )
			{
				vk2_create( pVk->vk, pWin->win_width, pWin->win_height, unit_MAX );
			}
	}
	//-----------------------------------------------------
	// メインループ
	//-----------------------------------------------------
	MSG msg;   // message
	msg.wParam = 0;

	key_init(argc,argv);

	int lim1 = 0;
	int lim2 = 0;

			for ( int u = 0 ; u < unit_cnt ; u++ )
			{
					vk2_loadModel( pVk->vk
						, (void*)&dataVert[u]
						, sizeof(struct vktexcube_vs_uniform)
						, sc_uniform_buffer[u]
						, sc_uniform_memory[u]
						, sc_descriptor_set[u]
					);
			}
					pVk->flgSetModel = true;

	while (true) 
	{
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (msg.message == WM_QUIT) // check for a flgQuit message
		{
			break;
		} else 
		{
			/* Translate and dispatch to event queue*/
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			RedrawWindow(pWin->hWin, NULL, NULL, RDW_INTERNALPAINT);
		}
		if (msg.message != WM_PAINT) continue;

		if ( key.hi._1 )
		{
		}
		
		if ( key.hi._4 )
		{
			if ( pVk ) 
			{
		//		if ( pVk->flgSetModel== false )
				{
//					vk2_create( pVk->vk, pWin->win_width, pWin->win_height, (void*)&dataVert, sizeof(struct vktexcube_vs_uniform), unit_MAX );
				}
			}
		}
		if ( key.hi._5 )
		{
			if ( pVk ) 
			{
		//		if ( pVk->flgSetModel== true )
				{
//					vk2_release( pVk->vk );
//					pVk->flgSetModel = false;
				}
			}
		}
		if ( key.hi._6 )
		{
			lim2 = 100;
		}
		if ( lim2 )
		{
			if ( pVk != 0 ) 
			{
//				if ( pVk->flgSetModel== true )
				{
					vk2_release( pVk->vk );
//					pVk->flgSetModel = false;
				}
//				if ( pVk->flgSetModel== false )
				{
					vk2_create( pVk->vk, pWin->win_width, pWin->win_height, unit_MAX );
//					for ( int u = 0 ; u < unit_cnt ; u++ )
int u = 1;
					{
						vk2_loadModel( pVk->vk
							, (void*)&dataVert[u]
							, sizeof(struct vktexcube_vs_uniform)
							, sc_uniform_buffer[u]
							, sc_uniform_memory[u]
							, sc_descriptor_set[u]
						);
					}
					pVk->flgSetModel = true;
				}
				lim2--;
				printf("%d ",lim2 );
			}
		}
		
		//-----------------------------------------------------
		// 描画
		//-----------------------------------------------------
		if ( pVk ) 
		{
			mat4x4 MVP, Model, VP;
			int matrixSize = sizeof(MVP);

			mat4x4_mul(VP, apr_projection_matrix, apr_view_matrix);

			// Rotate around the Y axis
			mat4x4_dup(Model, apr_model_matrix);
			mat4x4_rotate(apr_model_matrix, Model, 0.0f, 1.0f, 0.0f, (float)degreesToRadians(apr_spin_angle));
			mat4x4_mul(MVP, VP, apr_model_matrix);

			if ( pVk->flgSetModel )
			{
	for ( int u = 0; u < unit_cnt; u++ )
	{
	 	g_model[u].rotX(RAD(0.1));
	 	g_model[u].rotY(RAD(1));
	}
	for ( int u = 0; u < unit_cnt; u++ )
	{
		mvp[u].identity();
		mvp[u].perspectiveGL( 45, 512.0/512.0,0.1,100		 );
		mvp[u].m[1][1] *= -1; // GL to Vulkan
		mvp[u] =  g_model[u] * g_view * mvp[u];
	}


				int _vertexCount		= 12*3;
				int _instanceCount		= 1;
				int _firstVertex		= 0;
				int _firstInstance		= 0;

				vk2_updateBegin( pVk->vk );
				{
					vk2_cmd1( pVk->vk, pWin->win_width, pWin->win_height );
					for ( int u =0 ; u < unit_cnt ; u++ )
					{
						vk2_cmd2( pVk->vk 
							,_vertexCount
							,_instanceCount
							,_firstVertex
							,_firstInstance
							, sc_descriptor_set[u]
						);
						vk2_drawPolygon( pVk->vk, mvp[u].m, matrixSize, sc_uniform_memory[u]);
					}
					vk2_cmd3( pVk->vk ); 
				}

				vk2_updateEnd( pVk->vk );
			}
		}

		key_update();

	}

	//-----------------------------------------------------
	// 終了
	//-----------------------------------------------------
	if ( pVk ) 
	{
		if ( pVk->flgSetModel== true )
		{
//			vk3_release( pVk->vk );
			vk2_release( pVk->vk );
			pVk->flgSetModel = false;
		}
		delete pVk;pVk=0;
	}
	if ( pWin ) delete pWin;

	return (int)msg.wParam;
}
//#endif
