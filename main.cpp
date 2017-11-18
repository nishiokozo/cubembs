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


#ifndef LINMATH_H
#define LINMATH_H

#include <math.h>

#include "key.h"


const static int WIN_NAME_LEN = 80;
struct WindowInf
{

	HINSTANCE hInstance;
	POINT win_minsize;				// minimum window size
	HWND hWin;				  // hWnd - window handle
	char win_name[WIN_NAME_LEN];  // Name to put on the window/icon

	int	win_width;
	int	win_height;
} ;


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

#endif


mat4x4 apr_projection_matrix;
mat4x4 apr_view_matrix;
mat4x4 apr_model_matrix;

float 	apr_spin_angle;
float 	apr_spin_increment;
bool	apr_pause;

#define DEMO_TEXTURE_COUNT 1
#define APP_SHORT_NAME "cube"
#define FRAME_LAG 2// Allow a maximum of two outstanding presentation operations.

//bool in_callback = false;
#define ERR_EXIT(err_msg, err_class)											 \
	do {																		 \
		MessageBox(NULL, err_msg, err_class, MB_OK); \
		exit(1);																 \
	} while (0)


static PFN_vkGetDeviceProcAddr g_gdpa = NULL;
struct texture_object 
{
	VkSampler sampler;
	VkImage image;
	VkImageLayout imageLayout;
	VkMemoryAllocateInfo	mem_alloc;
	VkDeviceMemory 			devmem;
	VkImageView				imgview;
	int32_t 				tex_width;
	int32_t 				tex_height;
};

static const char *tex_files[] = {"lunarg.ppm"};

static int validation_error = 0;

struct vktexcube_vs_uniform 
{
	// Must start with MVP
	float mvp[4][4];
	float position[12 * 3][4];
	float attr[12 * 3][4];
};

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

VKAPI_ATTR VkBool32 VKAPI_CALL BreakCallback(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject,
											 size_t location, int32_t msgCode, const char *pLayerPrefix, const char *pMsg,
											 void *pUserData) 
											 {
//#ifndef WIN32
//	raise(SIGTRAP);
//#else
	DebugBreak();
//#endif

	return false;
}

typedef struct 
{
	VkImage image;
	VkCommandBuffer 	cmdbuf;
	VkCommandBuffer 	graphics_to_present_cmdbuf;
	VkImageView 		imgview;
	VkBuffer 			uniform_buffer;
	VkDeviceMemory 		uniform_memory;
	VkFramebuffer 		framebuffer;
	VkDescriptorSet 	descriptor_set;
} SwapchainImageResources;

struct VulkanInf 
{
	VkSurfaceKHR 	surface;
//	bool 			flgPrepared;
	bool 			flg_separate_present_queue;


	VkInstance 			inst;
	VkPhysicalDevice 	gpu;
	VkDevice 			device;
	VkQueue 			graphics_queue;
	VkQueue 			present_queue;
	uint32_t 			q_graphics_queue_family_index;
	uint32_t 			q_present_queue_family_index;
	VkSemaphore 		image_acquired_semaphores[FRAME_LAG];
	VkSemaphore 		draw_complete_semaphores[FRAME_LAG];
	VkSemaphore 		image_ownership_semaphores[FRAME_LAG];
	VkPhysicalDeviceProperties 		gpu_props;
	VkQueueFamilyProperties*			queue_props;
	VkPhysicalDeviceMemoryProperties 	memory_properties;

	uint32_t 	enabled_extension_count;
	uint32_t 	enabled_layer_count;
	const char*	extension_names[64];
	char*		enabled_layers[64];

	VkFormat 		format;
	VkColorSpaceKHR color_space;

	PFN_vkGetPhysicalDeviceSurfaceSupportKHR 		fpGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR	fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR		fpGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR	fpGetPhysicalDeviceSurfacePresentModesKHR;
	PFN_vkCreateSwapchainKHR						fpCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR						fpDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR						fpGetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR						fpAcquireNextImageKHR;
	PFN_vkQueuePresentKHR							fpQueuePresentKHR;

	uint32_t 					swapchainImageCount;
	VkSwapchainKHR				swapchain;
	SwapchainImageResources*	swapchain_image_resources;
	VkPresentModeKHR 			presentMode;
	VkFence 					fences[FRAME_LAG];
	int 						frame_index;

	VkCommandPool cmd_pool;
	VkCommandPool present_cmd_pool;

	struct 
	{
		VkFormat 				format;
		VkImage					image;
		VkMemoryAllocateInfo	mem_alloc;
		VkDeviceMemory 			devmem;
		VkImageView 			imgview;
	} depth_inf;

	struct texture_object textures[DEMO_TEXTURE_COUNT];
	struct texture_object staging_texture;

	VkCommandBuffer 		cmdbuf;  // Buffer for initialization commands
	VkPipelineLayout		pipeline_layout;
	VkDescriptorSetLayout 	desc_layout;
	VkPipelineCache 		pipelineCache;
	VkRenderPass 			render_pass;
	VkPipeline 				pipeline;

	VkDescriptorPool desc_pool;

	uint32_t current_buffer;
	uint32_t queue_family_count;
};

// On MS-Windows, make this a global, so it's available to WndProc()


//-----------------------------------------------------------------------------
static bool memory_type_from_properties(
//-----------------------------------------------------------------------------
	  VkPhysicalDeviceMemoryProperties* pMemory_properties
	, uint32_t typeBits
	, VkFlags requirements_mask
	, uint32_t *typeIndex
) 
{
	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) 
	{
		if ((typeBits & 1) == 1) 
		{
			// Type is available, does it match user properties?
			if ((pMemory_properties->memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) 
			{
				*typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}
	// No memory types matched, return failure
	return false;
}





//-----------------------------------------------------------------------------
bool loadTexture(
//-----------------------------------------------------------------------------
	const char *filename, uint8_t *rgba_data,
	VkSubresourceLayout *layout, int32_t *width, int32_t *height) 

{

	FILE *fPtr = fopen(filename, "rb");
	char header[256], *cPtr, *tmp;

	if (!fPtr)
		return false;

	cPtr = fgets(header, 256, fPtr); // P6
	if (cPtr == NULL || strncmp(header, "P6\n", 3)) 
	{
		fclose(fPtr);
		return false;
	}

	do 
	{
		cPtr = fgets(header, 256, fPtr);
		if (cPtr == NULL) 
		{
			fclose(fPtr);
			return false;
		}
	} while (!strncmp(header, "#", 1));

	sscanf(header, "%u %u", width, height);
	if (rgba_data == NULL) 
	{
		fclose(fPtr);
		return true;
	}
	tmp = fgets(header, 256, fPtr); // Format
	(void)tmp;
	if (cPtr == NULL || strncmp(header, "255\n", 3)) 
	{
		fclose(fPtr);
		return false;
	}

	for (int y = 0; y < *height; y++) 
	{
		uint8_t *rowPtr = rgba_data;
		for (int x = 0; x < *width; x++) 
		{
			size_t s = fread(rowPtr, 3, 1, fPtr);
			(void)s;
			rowPtr[3] = 255; /* Alpha of 1 */
			rowPtr += 4;
		}
		rgba_data += layout->rowPitch;
	}
	fclose(fPtr);
	return true;
}


//-----------------------------------------------------------------------------
static void demo_prepare_texture_image(
//-----------------------------------------------------------------------------
	VkDevice device,
	VkPhysicalDeviceMemoryProperties* pMemory_properties,
	const char *filename,
	struct texture_object *tex_obj,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkFlags required_props
) 
{
	//-----------------------------------------------------
	// 
	//-----------------------------------------------------
	int32_t tex_width;
	int32_t tex_height;
	{

		if (!loadTexture(filename, NULL, NULL, &tex_width, &tex_height)) 
		{
			ERR_EXIT("Failed to load textures", "Load Texture Failure");
		}

		tex_obj->tex_width = tex_width;
		tex_obj->tex_height = tex_height;
	}

	//-----------------------------------------------------
	// イメージの作成
	//-----------------------------------------------------
	{
		const VkFormat tex_format = VK_FORMAT_R8G8B8A8_UNORM;
		const VkImageCreateInfo ici = 
		{
			.sType 					= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext 					= NULL,
			.flags 					= 0,
			.imageType 				= VK_IMAGE_TYPE_2D,
			.format 				= tex_format,
			.extent 				= 
			{ 
				  tex_width
				, tex_height
				, 1
			},
			.mipLevels 				= 1,
			.arrayLayers 			= 1,
			.samples 				= VK_SAMPLE_COUNT_1_BIT,
			.tiling 				= tiling,
			.usage 					= usage,
			.sharingMode			= VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount	= 0,
			.pQueueFamilyIndices	= 0,
			.initialLayout 			= VK_IMAGE_LAYOUT_PREINITIALIZED,

		};
		VkResult  err;
		err = vkCreateImage(device, &ici, NULL, &tex_obj->image);
		assert(!err);
	}

	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
	{
		VkMemoryRequirements mr;
		{
			//---------------------------------------------------------
			// イメージメモリ要求の取得
			//---------------------------------------------------------
			vkGetImageMemoryRequirements(device, tex_obj->image, &mr);

			tex_obj->mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			tex_obj->mem_alloc.pNext = NULL;
			tex_obj->mem_alloc.allocationSize = mr.size;
			tex_obj->mem_alloc.memoryTypeIndex = 0;

			//-----------------------------------------------------
			// プロパティーの取得
			//-----------------------------------------------------
			bool  pass = false;
			{
				uint32_t 							typeBits			= mr.memoryTypeBits;
				VkFlags 							requirements_mask	= required_props;
				uint32_t*							typeIndex			= &tex_obj->mem_alloc.memoryTypeIndex;

				// Search memtypes to find first index with those properties
				for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) 
				{
					if ((typeBits & 1) == 1) 
					{
						// Type is available, does it match user properties?
						if ((pMemory_properties->memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) 
						{
							*typeIndex = i;
							pass = true;
							break;
						}
					}
					typeBits >>= 1;
				}
			}
			assert(pass);
		}
	}

	/* allocate memory */
	//---------------------------------------------------------
	// メモリの確保
	//---------------------------------------------------------
	{
		VkResult  err;
		err = vkAllocateMemory(device, &tex_obj->mem_alloc, NULL, &(tex_obj->devmem));
		assert(!err);
	}

	/* bind memory */
	//---------------------------------------------------------
	// イメージメモリのバインド
	//---------------------------------------------------------
	{
		VkResult  err;
		err = vkBindImageMemory(device, tex_obj->image, tex_obj->devmem, 0);
		assert(!err);
	}

	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
	if (required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) 
	{
		//---------------------------------------------------------
		// イメージサブリソースレイアウトの取得
		//---------------------------------------------------------
		VkSubresourceLayout layout;
		{
			const VkImageSubresource subres = 
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.arrayLayer = 0,
			};

			vkGetImageSubresourceLayout(device, tex_obj->image, &subres, &layout);

		}

		//---------------------------------------------------------
		// マップメモリ
		//---------------------------------------------------------
		{
			void *data;
			VkResult  err;
			err = vkMapMemory(device, tex_obj->devmem, 0, tex_obj->mem_alloc.allocationSize, 0, &data);
			assert(!err);

			//---------------------------------------------------------
			// テクスチャ転送
			//---------------------------------------------------------
			if ( !loadTexture( filename, (uint8_t*)data, &layout, &tex_width, &tex_height ) )
			{
				fprintf(stderr, "Error loading texture: %s\n", filename);
			}
		}

		//---------------------------------------------------------
		// マップ解除
		//---------------------------------------------------------
		{
			vkUnmapMemory(device, tex_obj->devmem);
		}
	}

	tex_obj->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

//-----------------------------------------------------------------------------
char *demo_read_spv(const char *filename, size_t *psize) 
//-----------------------------------------------------------------------------

{
	long int size;
	size_t  retval;
	void *shader_code;

	FILE *fp = fopen(filename, "rb");
	if (!fp) return NULL;

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);

	shader_code = malloc(size);
	retval = fread(shader_code, size, 1, fp);
	assert(retval == 1);

	*psize = size;

	fclose(fp);
	return (char*)shader_code;
}



//-----------------------------------------------------------------------------
void	vk_draw( VulkanInf& vk )
//-----------------------------------------------------------------------------
{
	// Ensure no more than FRAME_LAG renderings are outstanding
	vkWaitForFences(vk.device, 1, &vk.fences[vk.frame_index], VK_TRUE, UINT64_MAX);
	vkResetFences(vk.device, 1, &vk.fences[vk.frame_index]);

	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
	{
		VkResult  err;
		do 
		{
			// Get the index of the next available swapchain image:
			err = vk.fpAcquireNextImageKHR(vk.device, vk.swapchain, UINT64_MAX,
											  vk.image_acquired_semaphores[vk.frame_index],
											  VK_NULL_HANDLE, &vk.current_buffer);

			if (err == VK_ERROR_OUT_OF_DATE_KHR) 
			{
				// vk.swapchain is out of date (e.g. the window was resized) and
				// must be recreated:
				//demo_resize(&vk);
			} else if (err == VK_SUBOPTIMAL_KHR) 
			{
				// vk.swapchain is not as optimal as it could be, but the platform's
				// presentation engine will still present the image correctly.
				break;
			} else 
			{
				assert(!err);
			}
		} while (err != VK_SUCCESS);
	}

	//demo_update_data_buffer(&vk);
	{
		mat4x4 MVP, Model, VP;
		int matrixSize = sizeof(MVP);
		uint8_t *pData;

		mat4x4_mul(VP, apr_projection_matrix, apr_view_matrix);

		// Rotate around the Y axis
		mat4x4_dup(Model, apr_model_matrix);
		mat4x4_rotate(apr_model_matrix, Model, 0.0f, 1.0f, 0.0f, (float)degreesToRadians(apr_spin_angle));
		mat4x4_mul(MVP, VP, apr_model_matrix);

		//---------------------------------------------------------
		// マップメモリ
		//---------------------------------------------------------
		{
			VkResult  err;
			err = vkMapMemory( 
				vk.device
				, vk.swapchain_image_resources[vk.current_buffer].uniform_memory
				, 0
				, VK_WHOLE_SIZE
				, 0
				, (void **)&pData
			);
			assert(!err);
		}

		//---------------------------------------------------------
		// マトリクスのコピー
		//---------------------------------------------------------
		memcpy(pData, (const void *)&MVP[0][0], matrixSize);

		//---------------------------------------------------------
		// マップ解除
		//---------------------------------------------------------
		vkUnmapMemory(vk.device, vk.swapchain_image_resources[vk.current_buffer].uniform_memory);
	}


	// Wait for the image acquired semaphore to be signaled to ensure
	// that the image won't be rendered to until the presentation
	// engine has fully released ownership to the application, and it is
	// okay to render to the image.

	//---------------------------------------------------------
	// グラフィックキューを登録
	//---------------------------------------------------------
	{
		VkPipelineStageFlags psf;
		psf = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		{
			VkSubmitInfo si;
			si.sType 				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
			si.pNext 				= NULL;
			si.pWaitDstStageMask 	= &psf;
			si.waitSemaphoreCount 	= 1;
			si.pWaitSemaphores 		= &vk.image_acquired_semaphores[vk.frame_index];
			si.commandBufferCount 	= 1;
			si.pCommandBuffers 		= &vk.swapchain_image_resources[vk.current_buffer].cmdbuf;
			si.signalSemaphoreCount = 1;
			si.pSignalSemaphores 	= &vk.draw_complete_semaphores[vk.frame_index];
			{
				VkResult  err;
				err = vkQueueSubmit(vk.graphics_queue, 1, &si, vk.fences[vk.frame_index]);
				assert(!err);
			}
		}
	}

	//---------------------------------------------------------
	// 描画キューを登録
	//---------------------------------------------------------
	if (vk.flg_separate_present_queue) 
	{
		VkPipelineStageFlags psf;
		psf = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		{

			// If we are using separate queues, change image ownership to the
			// present queue before presenting, waiting for the draw complete
			// semaphore and signalling the ownership released semaphore when finished
			VkSubmitInfo si;
			si.sType 				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
			si.pNext 				= NULL;
			si.pWaitDstStageMask 	= &psf;
			si.waitSemaphoreCount 	= 1;
			si.pWaitSemaphores 		= &vk.draw_complete_semaphores[vk.frame_index];
			si.commandBufferCount 	= 1;
			si.pCommandBuffers 		= &vk.swapchain_image_resources[vk.current_buffer].graphics_to_present_cmdbuf;
			si.signalSemaphoreCount = 1;
			si.pSignalSemaphores 	= &vk.image_ownership_semaphores[vk.frame_index];

			{
				VkFence nullFence = VK_NULL_HANDLE;
				VkResult  err;
				err = vkQueueSubmit(vk.present_queue, 1, &si, nullFence);
				assert(!err);
			}
		
		}
	}

	// If we are using separate queues we have to wait for image ownership,
	// otherwise wait for draw complete
	{
		VkPresentInfoKHR present = 
		{
			.sType 				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext 				= NULL,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores 	= (vk.flg_separate_present_queue)
								   ? &vk.image_ownership_semaphores[vk.frame_index]
								   : &vk.draw_complete_semaphores[vk.frame_index],
			.swapchainCount 		= 1,
			.pSwapchains 		= &vk.swapchain,
			.pImageIndices 		= &vk.current_buffer,
		};


		//---------------------------------------------------------
		// 描画キック
		//---------------------------------------------------------
		{
			VkResult  err;
			err = vk.fpQueuePresentKHR(vk.present_queue, &present);
			vk.frame_index += 1;
			vk.frame_index %= FRAME_LAG;

			if (err == VK_ERROR_OUT_OF_DATE_KHR) 
			{
				// vk.swapchain is out of date (e.g. the window was resized) and
				// must be recreated:
				//demo_resize(&vk);
			} else if (err == VK_SUBOPTIMAL_KHR) 
			{
				// vk.swapchain is not as optimal as it could be, but the platform's
				// presentation engine will still present the image correctly.
			} else 
			{
				assert(!err);
			}
		}
	}
}


//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
//-----------------------------------------------------------------------------

{
	switch (uMsg) 
	{
	case WM_CLOSE:
		PostQuitMessage(validation_error);
		break;
	case WM_PAINT:
		break;
	case WM_GETMINMAXINFO:	 // set window's minimum size
		return 0;
	case WM_SIZE:
		break;
	default:
		break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}


#include <shellapi.h>


//-----------------------------------------------------------------------------
void	vk_init( VulkanInf& vk )
//-----------------------------------------------------------------------------
{
	memset(&vk, 0, sizeof(vk));
	vk.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	vk.enabled_extension_count = 0;
	vk.enabled_layer_count = 0;

	//---------------------------------------------------------
	// 拡張インスタンス情報の取得
	//---------------------------------------------------------
	{
		VkBool32 surfaceExtFound = 0;
		VkBool32 platformSurfaceExtFound = 0;
		
		memset(vk.extension_names, 0, sizeof(vk.extension_names));

		{
			uint32_t instance_extension_count = 0;
			//---------------------------------------------------------
			// 
			//---------------------------------------------------------
			{
				VkResult err;
				err = vkEnumerateInstanceExtensionProperties( NULL, &instance_extension_count, NULL);
				assert(!err);
			}

			if (instance_extension_count > 0) 
			{
				VkExtensionProperties* instance_extensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * instance_extension_count);

				//---------------------------------------------------------
				// 
				//---------------------------------------------------------
				{
					VkResult err;
					err = vkEnumerateInstanceExtensionProperties( NULL, &instance_extension_count, instance_extensions);
					assert(!err);
				}
				for (uint32_t i = 0; i < instance_extension_count; i++) 
				{
					if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) 
					{
						surfaceExtFound = 1;
						vk.extension_names[vk.enabled_extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
					}
					if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) 
					{
						platformSurfaceExtFound = 1;
						vk.extension_names[vk.enabled_extension_count++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
					}

					assert(vk.enabled_extension_count < 64);
				}

				free(instance_extensions);
			}
		}

		if (!surfaceExtFound) 
		{
			ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find "
					 "the " VK_KHR_SURFACE_EXTENSION_NAME
					 " extension.\n\nDo you have a compatible "
					 "Vulkan installable client driver (ICD) installed?\nPlease "
					 "look at the Getting Started guide for additional "
					 "information.\n",
					 "vkCreateInstance Failure");
		}
		if (!platformSurfaceExtFound) 
		{
			ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find "
					 "the " VK_KHR_WIN32_SURFACE_EXTENSION_NAME
					 " extension.\n\nDo you have a compatible "
					 "Vulkan installable client driver (ICD) installed?\nPlease "
					 "look at the Getting Started guide for additional "
					 "information.\n",
					 "vkCreateInstance Failure");
		}
	}

	//---------------------------------------------------------
	// インスタンスの作成
	//---------------------------------------------------------
	{
		const VkApplicationInfo app = 
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = NULL,
			.pApplicationName = APP_SHORT_NAME,
			.applicationVersion = 0,
			.pEngineName = APP_SHORT_NAME,
			.engineVersion = 0,
			.apiVersion = VK_API_VERSION_1_0,
		};
		{
			char **instance_validation_layers = NULL;
			VkInstanceCreateInfo inst_info = 
			{
				.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.pNext = NULL,
				.pApplicationInfo = &app,
				.enabledLayerCount = vk.enabled_layer_count,
				.ppEnabledLayerNames = (const char *const *)instance_validation_layers,
				.enabledExtensionCount = vk.enabled_extension_count,
				.ppEnabledExtensionNames = (const char *const *)vk.extension_names,
			};


			//---------------------------------------------------------
			// インスタンスの作成
			//---------------------------------------------------------
			{
				VkResult err;
				err = vkCreateInstance(&inst_info, NULL, &vk.inst);
				if (err == VK_ERROR_INCOMPATIBLE_DRIVER) 
				{
					ERR_EXIT("Cannot find a compatible Vulkan installable client driver "
							 "(ICD).\n\nPlease look at the Getting Started guide for "
							 "additional information.\n",
							 "vkCreateInstance Failure");
				} else if (err == VK_ERROR_EXTENSION_NOT_PRESENT) 
				{
					ERR_EXIT("Cannot find a specified extension library"
							 ".\nMake sure your layers path is set appropriately.\n",
							 "vkCreateInstance Failure");
				} else if (err) 
				{
					ERR_EXIT("vkCreateInstance failed.\n\nDo you have a compatible Vulkan "
							 "installable client driver (ICD) installed?\nPlease look at "
							 "the Getting Started guide for additional information.\n",
							 "vkCreateInstance Failure");
				}
			}
		}
	}

	//---------------------------------------------------------
	// GPU数を取得
	//---------------------------------------------------------
	uint32_t gpu_count;
	{
		VkResult err;
		err = vkEnumeratePhysicalDevices(vk.inst, &gpu_count, NULL);
		assert(!err && gpu_count > 0);
	}

	//---------------------------------------------------------
	// GPUデバイスを列挙
	//---------------------------------------------------------
	if (gpu_count > 0) 
	{
		VkPhysicalDevice*	physical_devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
		{
			VkResult err;
			err = vkEnumeratePhysicalDevices(vk.inst, &gpu_count, physical_devices);
			assert(!err);
		}
		/* For cube &vk we just grab the first physical device */
		vk.gpu = physical_devices[0];
		free(physical_devices);
	} else 
	{
		ERR_EXIT("vkEnumeratePhysicalDevices reported zero accessible devices.\n\n"
				 "Do you have a compatible Vulkan installable client driver (ICD) "
				 "installed?\nPlease look at the Getting Started guide for "
				 "additional information.\n",
				 "vkEnumeratePhysicalDevices Failure");
	}

	//---------------------------------------------------------
	// 列挙デバイス情報初期化
	//---------------------------------------------------------
	{
		vk.enabled_extension_count = 0;
		memset(vk.extension_names, 0, sizeof(vk.extension_names));
	}

	//---------------------------------------------------------
	// 列挙デバイス情報取得
	//---------------------------------------------------------
	{
		uint32_t device_extension_count = 0;
		{
			VkResult err;
			err = vkEnumerateDeviceExtensionProperties(vk.gpu, NULL, &device_extension_count, NULL);
			assert(!err);
		}

		{
			VkBool32 swapchainExtFound = 0;
			if (device_extension_count > 0) 
			{
				VkExtensionProperties* device_extensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * device_extension_count);
				//---------------------------------------------------------
				// 列挙デバイス情報取得
				//---------------------------------------------------------
				{
					VkResult err;
					err = vkEnumerateDeviceExtensionProperties( vk.gpu, NULL, &device_extension_count, device_extensions);
					assert(!err);
				}

				for (uint32_t i = 0; i < device_extension_count; i++) 
				{
					if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, device_extensions[i].extensionName)) 
					{
						swapchainExtFound = 1;
						vk.extension_names[vk.enabled_extension_count++] = (char*)VK_KHR_SWAPCHAIN_EXTENSION_NAME;
					}
					assert(vk.enabled_extension_count < 64);
				}

				free(device_extensions);
			}
			if (!swapchainExtFound) 
			{
				ERR_EXIT("vkEnumerateDeviceExtensionProperties failed to find "
						 "the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
						 " extension.\n\nDo you have a compatible "
						 "Vulkan installable client driver (ICD) installed?\nPlease "
						 "look at the Getting Started guide for additional "
						 "information.\n",
						 "vkCreateInstance Failure");
			}
		}
	}



	//---------------------------------------------------------
	// GPUデバイス情報取得
	//---------------------------------------------------------
	vkGetPhysicalDeviceProperties(vk.gpu, &vk.gpu_props);

	//---------------------------------------------------------
	// デバイスキューファミリー情報取得
	//---------------------------------------------------------
	{
		vkGetPhysicalDeviceQueueFamilyProperties(vk.gpu,&vk.queue_family_count, NULL);
		assert(vk.queue_family_count >= 1);
	}

	//---------------------------------------------------------
	// キューファミリー情報バッファ確保
	//---------------------------------------------------------
	vk.queue_props = (VkQueueFamilyProperties *)malloc( vk.queue_family_count * sizeof(VkQueueFamilyProperties));

	//---------------------------------------------------------
	// キューファミリー情報取得
	//---------------------------------------------------------
	vkGetPhysicalDeviceQueueFamilyProperties( vk.gpu, &vk.queue_family_count, vk.queue_props);

	//---------------------------------------------------------
	// デバイス機能情報の取得
	//---------------------------------------------------------
	{
		// Query fine-grained feature support for this device.
		//  If app has specific feature requirements it should check supported
		//  features based on this query
		VkPhysicalDeviceFeatures physDevFeatures;
		vkGetPhysicalDeviceFeatures(vk.gpu, &physDevFeatures);
	}

	//---------------------------------------------------------
	// 拡張関数取得
	//---------------------------------------------------------
	{
		vk.fpGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(vk.inst, "vk" "GetPhysicalDeviceSurfaceSupportKHR");
		if (vk.fpGetPhysicalDeviceSurfaceSupportKHR == NULL) 
		{
			ERR_EXIT("vkGetInstanceProcAddr failed to find vk" "GetPhysicalDeviceSurfaceSupportKHR", "vkGetInstanceProcAddr Failure");
		}

		vk.fpGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(vk.inst, "vk" "GetPhysicalDeviceSurfaceCapabilitiesKHR");
		if (vk.fpGetPhysicalDeviceSurfaceCapabilitiesKHR == NULL) 
		{
			ERR_EXIT("vkGetInstanceProcAddr failed to find vk" "GetPhysicalDeviceSurfaceCapabilitiesKHR", "vkGetInstanceProcAddr Failure");
		}

		vk.fpGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(vk.inst, "vk" "GetPhysicalDeviceSurfaceFormatsKHR");
		if (vk.fpGetPhysicalDeviceSurfaceFormatsKHR == NULL) 
		{
			ERR_EXIT("vkGetInstanceProcAddr failed to find vk" "GetPhysicalDeviceSurfaceFormatsKHR", "vkGetInstanceProcAddr Failure");
		}

		vk.fpGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(vk.inst, "vk" "GetPhysicalDeviceSurfacePresentModesKHR");
		if (vk.fpGetPhysicalDeviceSurfacePresentModesKHR == NULL) 
		{
			ERR_EXIT("vkGetInstanceProcAddr failed to find vk" "GetPhysicalDeviceSurfacePresentModesKHR", "vkGetInstanceProcAddr Failure");
		}

		vk.fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)vkGetInstanceProcAddr(vk.inst, "vk" "GetSwapchainImagesKHR");
		if (vk.fpGetSwapchainImagesKHR == NULL) 
		{
			ERR_EXIT("vkGetInstanceProcAddr failed to find vk" "GetSwapchainImagesKHR", "vkGetInstanceProcAddr Failure");
		}
	}

}

//-----------------------------------------------------------------------------
void	win_init( WindowInf& win, const char* name, int width, int height  )
//-----------------------------------------------------------------------------
{
	win.win_width = width;
	win.win_height = height;

	win.hInstance		= GetModuleHandle( NULL );

	{
		strncpy(win.win_name, "cube", WIN_NAME_LEN);

		WNDCLASSEX win_class;

		// Initialize the window class structure:
		win_class.cbSize = sizeof(WNDCLASSEX);
		win_class.style = CS_HREDRAW | CS_VREDRAW;
		win_class.lpfnWndProc = WndProc;
		win_class.cbClsExtra = 0;
		win_class.cbWndExtra = 0;
		win_class.hInstance = win.hInstance;
		win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		win_class.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
		win_class.lpszMenuName = NULL;
		win_class.lpszClassName = win.win_name;
		win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
		// Register window class:
		if (!RegisterClassEx(&win_class)) 
		{
			// It didn't work, so try to give a useful error:
			printf("Unexpected error trying to start the application!\n");
			fflush(stdout);
			exit(1);
		}
		// Create window with the registered class:
		RECT wr = {0, 0, win.win_width, win.win_height};
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

		win.hWin = CreateWindowEx(
			0,
			win.win_name,		   			// class name
			win.win_name,		   			// app name
			WS_OVERLAPPEDWINDOW | 		// window style
			WS_VISIBLE | WS_SYSMENU,
			100, 100,		   			// x/y coords
			wr.right - wr.left, 		// width
			wr.bottom - wr.top, 		// height
			NULL,			   			// handle to parent
			NULL,			   			// handle to menu
			win.hInstance,
			NULL					  	// no extra parameters
		);
	
		if (!win.hWin) 
		{
			// It didn't work, so try to give a useful error:
			printf("Cannot create a window in which to draw!\n");
			fflush(stdout);
			exit(1);
		}
		// Window client area size must be at least 1 pixel high, to prevent crash.
		win.win_minsize.x = GetSystemMetrics(SM_CXMINTRACK);
		win.win_minsize.y = GetSystemMetrics(SM_CYMINTRACK)+1;
	}
}

//-----------------------------------------------------------------------------
void vk_setup( VulkanInf& vk, HINSTANCE hInstance, HWND hWin, int _width, int _height )
//-----------------------------------------------------------------------------
{
	//---------------------------------------------------------
	// windowsハンドルから、vkサーフェスを取得する
	//---------------------------------------------------------
	{
		VkWin32SurfaceCreateInfoKHR createInfo;
	
		createInfo.sType 		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.pNext 		= NULL;
		createInfo.flags 		= 0;
		createInfo.hinstance 	= hInstance;
		createInfo.hwnd 		= hWin;

		VkResult  err;
		err = vkCreateWin32SurfaceKHR(vk.inst, &createInfo, NULL, &vk.surface);
		assert(!err);
	}

	//---------------------------------------------------------
	// キューを作成
	//---------------------------------------------------------
	{
		// Iterate over each queue to learn whether it supports presenting:
		VkBool32 *supportsPresent = (VkBool32 *)malloc(vk.queue_family_count * sizeof(VkBool32));
		for (uint32_t i = 0; i < vk.queue_family_count; i++) 
		{
			vk.fpGetPhysicalDeviceSurfaceSupportKHR(vk.gpu, i, vk.surface, &supportsPresent[i]);
		}

		// Search for a graphics and a present queue in the array of queue
		// families, try to find one that supports both
		uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
		uint32_t presentQueueFamilyIndex = UINT32_MAX;
		for (uint32_t i = 0; i < vk.queue_family_count; i++) 
		{
			if ((vk.queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) 
			{
				if (graphicsQueueFamilyIndex == UINT32_MAX) 
				{
					graphicsQueueFamilyIndex = i;
				}

				if (supportsPresent[i] == VK_TRUE) 
				{
					graphicsQueueFamilyIndex = i;
					presentQueueFamilyIndex = i;
					break;
				}
			}
		}

		if (presentQueueFamilyIndex == UINT32_MAX) 
		{
			// If didn't find a queue that supports both graphics and present, then
			// find a separate present queue.
			for (uint32_t i = 0; i < vk.queue_family_count; ++i) 
			{
				if (supportsPresent[i] == VK_TRUE) 
				{
					presentQueueFamilyIndex = i;
					break;
				}
			}
		}

		// Generate error if could not find both a graphics and a present queue
		if (graphicsQueueFamilyIndex == UINT32_MAX || presentQueueFamilyIndex == UINT32_MAX) 
		{
			ERR_EXIT("Could not find both graphics and present queues\n",
					 "Swapchain Initialization Failure");
		}

		vk.q_graphics_queue_family_index = graphicsQueueFamilyIndex;
		vk.q_present_queue_family_index = presentQueueFamilyIndex;
		vk.flg_separate_present_queue = (vk.q_graphics_queue_family_index != vk.q_present_queue_family_index);
		free(supportsPresent);
	}

	//demo_create_device(&vk);
	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
	{
		float queue_priorities[1] = {0.0};
		VkDeviceQueueCreateInfo queues[2];
		queues[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queues[0].pNext = NULL;
		queues[0].queueFamilyIndex = vk.q_graphics_queue_family_index;
		queues[0].queueCount = 1;
		queues[0].pQueuePriorities = queue_priorities;
		queues[0].flags = 0;

		VkDeviceCreateInfo device = 
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = NULL,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = queues,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = NULL,
			.enabledExtensionCount = vk.enabled_extension_count,
			.ppEnabledExtensionNames = (const char *const *)vk.extension_names,
			.pEnabledFeatures =
				NULL, // If specific features are required, pass them in here
		};
		if (vk.flg_separate_present_queue) 
		{
			queues[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queues[1].pNext = NULL;
			queues[1].queueFamilyIndex = vk.q_present_queue_family_index;
			queues[1].queueCount = 1;
			queues[1].pQueuePriorities = queue_priorities;
			queues[1].flags = 0;
			device.queueCreateInfoCount = 2;
		}

		VkResult  err;
		err = vkCreateDevice(vk.gpu, &device, NULL, &vk.device);
		assert(!err);
	}


	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
	{
		{
			if (!g_gdpa) g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(vk.inst, "vkGetDeviceProcAddr");
			vk.fpCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)g_gdpa(vk.device, "vk" "CreateSwapchainKHR");
			if (vk.fpCreateSwapchainKHR == NULL) 
			{
				ERR_EXIT("vkGetDeviceProcAddr failed to find vk" "CreateSwapchainKHR", "vkGetDeviceProcAddr Failure");
			}
		}
		{
			if (!g_gdpa) g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(vk.inst, "vkGetDeviceProcAddr");
			vk.fpDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)g_gdpa(vk.device, "vk" "DestroySwapchainKHR");
			if (vk.fpDestroySwapchainKHR == NULL) 
			{
				ERR_EXIT("vkGetDeviceProcAddr failed to find vk" "DestroySwapchainKHR", "vkGetDeviceProcAddr Failure");
			}
		}
		{
			if (!g_gdpa) g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(vk.inst, "vkGetDeviceProcAddr");
			vk.fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)g_gdpa(vk.device, "vk" "GetSwapchainImagesKHR");
			if (vk.fpGetSwapchainImagesKHR == NULL) 
			{
				ERR_EXIT("vkGetDeviceProcAddr failed to find vk" "GetSwapchainImagesKHR", "vkGetDeviceProcAddr Failure");
			}
		}
		{
			if (!g_gdpa) g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(vk.inst, "vkGetDeviceProcAddr");
			vk.fpAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)g_gdpa(vk.device, "vk" "AcquireNextImageKHR");
			if (vk.fpAcquireNextImageKHR == NULL) 
			{
				ERR_EXIT("vkGetDeviceProcAddr failed to find vk" "AcquireNextImageKHR", "vkGetDeviceProcAddr Failure");
			}
		}
		{
			if (!g_gdpa) g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(vk.inst, "vkGetDeviceProcAddr");
			vk.fpQueuePresentKHR = (PFN_vkQueuePresentKHR)g_gdpa(vk.device, "vk" "QueuePresentKHR");
			if (vk.fpQueuePresentKHR == NULL) 
			{
				ERR_EXIT("vkGetDeviceProcAddr failed to find vk" "QueuePresentKHR", "vkGetDeviceProcAddr Failure");
			}
		}
	}


	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
	vkGetDeviceQueue(vk.device, vk.q_graphics_queue_family_index, 0, &vk.graphics_queue);

	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
	if (!vk.flg_separate_present_queue) 
	{
		vk.present_queue = vk.graphics_queue;
	} else 
	{
		vkGetDeviceQueue(vk.device, vk.q_present_queue_family_index, 0, &vk.present_queue);
	}

	//---------------------------------------------------------
	// フォーマット情報を取得
	//---------------------------------------------------------
	{	
		//---------------------------------------------------------
		// フォーム数を取得
		//---------------------------------------------------------
		uint32_t formatCount;
		{
			VkResult  err;
			err = vk.fpGetPhysicalDeviceSurfaceFormatsKHR(vk.gpu, vk.surface, &formatCount, NULL);
			assert(!err);
		}

		//---------------------------------------------------------
		// フォーム数分バッファを確保
		//---------------------------------------------------------
		VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));

		//---------------------------------------------------------
		// サーフェースフォーマットを取得
		//---------------------------------------------------------
		{
			VkResult  err;
			err = vk.fpGetPhysicalDeviceSurfaceFormatsKHR(vk.gpu, vk.surface, &formatCount, surfFormats);
			assert(!err);
		}

		//---------------------------------------------------------
		// 
		//---------------------------------------------------------
		{
			// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
			// the surface has no preferred format.  Otherwise, at least one
			// supported format will be returned.
			if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) 
			{
				vk.format = VK_FORMAT_B8G8R8A8_UNORM;
			} else 
			{
				assert(formatCount >= 1);
				vk.format = surfFormats[0].format;
			}
		
			vk.color_space	= surfFormats[0].colorSpace;
//			vk.flgQuit			= false;
//			vk.curFrame		= 0;
		}
	}


	for (uint32_t i = 0; i < FRAME_LAG; i++) 
	{
		//---------------------------------------------------------
		// 
		//---------------------------------------------------------
		{
			// Create fences that we can use to throttle if we get too far
			// ahead of the image presents
			VkFenceCreateInfo fci = 
			{
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.pNext = NULL,
				.flags = VK_FENCE_CREATE_SIGNALED_BIT
			};
			VkResult  err;
			err = vkCreateFence(vk.device, &fci, NULL, &vk.fences[i]);
			assert(!err);
		}

		{
			// Create semaphores to synchronize acquiring presentable buffers before
			// rendering and waiting for drawing to be complete before presenting
			VkSemaphoreCreateInfo sci = 
			{
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				.pNext = NULL,
				.flags = 0,
			};
			//---------------------------------------------------------
			// セマフォの作成：イメージアキュワイド
			//---------------------------------------------------------
			{
				VkResult  err;
				err = vkCreateSemaphore(vk.device, &sci, NULL, &vk.image_acquired_semaphores[i]);
				assert(!err);
			}

			//---------------------------------------------------------
			// セマフォの作成：描画完了
			//---------------------------------------------------------
			{
				VkResult  err;
				err = vkCreateSemaphore(vk.device, &sci, NULL, &vk.draw_complete_semaphores[i]);
				assert(!err);
			}

			//---------------------------------------------------------
			// セマフォの作成：イメージオーナーシップ
			//---------------------------------------------------------
			if (vk.flg_separate_present_queue) 
			{
				VkResult  err;
				err = vkCreateSemaphore(vk.device, &sci, NULL, &vk.image_ownership_semaphores[i]);
				assert(!err);
			}
		}
	}



	//---------------------------------------------------------
	// コマンドプールの作成
	//---------------------------------------------------------
	{

		const VkCommandPoolCreateInfo cmd_pool_info = 
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = NULL,
			.queueFamilyIndex = vk.q_graphics_queue_family_index,
			.flags = 0,
		};

		VkResult  err;
		err = vkCreateCommandPool(vk.device, &cmd_pool_info, NULL, &vk.cmd_pool);
		assert(!err);
	}



	//---------------------------------------------------------
	// コマンドバッファの確保
	//---------------------------------------------------------
	{
		const VkCommandBufferAllocateInfo cmai = 
		{
			.sType 				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext 				= NULL,
			.commandPool 		= vk.cmd_pool,
			.level 				= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};
		VkResult  err;
		err = vkAllocateCommandBuffers(vk.device, &cmai, &vk.cmdbuf);
		assert(!err);
	}

	//---------------------------------------------------------
	// コマンドバッファの開始
	//---------------------------------------------------------
	{
		VkCommandBufferBeginInfo cb = 
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = NULL,
			.flags = 0,
			.pInheritanceInfo = NULL,
		};
		VkResult  err;
		err = vkBeginCommandBuffer(vk.cmdbuf, &cb);
		assert(!err);
	}


	//---------------------------------------------------------
	// 描画モード値
	//---------------------------------------------------------
	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	{
		// The FIFO present mode is guaranteed by the spec to be supported
		// and to have no tearing.  It's a great default present mode to use.

		//  There are times when you may wish to use another present mode.  The
		//  following code shows how to select them, and the comments provide some
		//  reasons you may wish to use them.
		//
		// It should be noted that Vulkan 1.0 doesn't provide a method for
		// synchronizing rendering with the presentation engine's display.  There
		// is a method provided for throttling rendering with the display, but
		// there are some presentation engines for which this method will not work.
		// If an application doesn't throttle its rendering, and if it renders much
		// faster than the refresh rate of the display, this can waste power on
		// mobile devices.  That is because power is being spent rendering images
		// that may never be seen.

		// VK_PRESENT_MODE_IMMEDIATE_KHR is for applications that don't care about
		// tearing, or have some way of synchronizing their rendering with the
		// display.
		// VK_PRESENT_MODE_MAILBOX_KHR may be useful for applications that
		// generally render a new presentable image every refresh cycle, but are
		// occasionally early.  In this case, the application wants the new image
		// to be displayed instead of the previously-queued-for-presentation image
		// that has not yet been displayed.
		// VK_PRESENT_MODE_FIFO_RELAXED_KHR is for applications that generally
		// render a new presentable image every refresh cycle, but are occasionally
		// late.  In this case (perhaps because of stuttering/latency concerns),
		// the application wants the late image to be immediately displayed, even
		// though that may mean some tearing.
		//---------------------------------------------------------
		// 描画モードの数の取得
		//---------------------------------------------------------
		uint32_t presentModeCount;
		{
			VkResult  err;
			err = vk.fpGetPhysicalDeviceSurfacePresentModesKHR( vk.gpu, vk.surface, &presentModeCount, NULL);
			assert(!err);
		}

		//---------------------------------------------------------
		// 描画モードバッファの確保
		//---------------------------------------------------------
		VkPresentModeKHR *presentModes;
		{
			presentModes = (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
			assert(presentModes);

			VkResult  err;
			err = vk.fpGetPhysicalDeviceSurfacePresentModesKHR( vk.gpu, vk.surface, &presentModeCount, presentModes);
			assert(!err);
		}

		//---------------------------------------------------------
		// 描画モードを検索
		//---------------------------------------------------------
		if (vk.presentMode !=  swapchainPresentMode) 
		{

			for (size_t i = 0; i < presentModeCount; ++i) 
			{
				if (presentModes[i] == vk.presentMode) 
				{
					swapchainPresentMode = vk.presentMode;
					break;
				}
			}
		}
		if (swapchainPresentMode != vk.presentMode) 
		{
			ERR_EXIT("Present mode specified is not supported\n", "Present mode unsupported");
		}

		//---------------------------------------------------------
		// 描画モードバッファの解放
		//---------------------------------------------------------
		if (NULL != presentModes) 
		{
			free(presentModes);
		}
	}


	//---------------------------------------------------------
	// サーフェス情報の取得
	//---------------------------------------------------------
	VkSurfaceCapabilitiesKHR surfCapabilities;
	{
		VkResult  err;
		err = vk.fpGetPhysicalDeviceSurfaceCapabilitiesKHR( vk.gpu, vk.surface, &surfCapabilities);
		assert(!err);
	}

	//---------------------------------------------------------
	// 最小イメージ数値
	//---------------------------------------------------------
	uint32_t mic = 3;
	{
		// Determine the number of VkImages to use in the swap chain.
		// Application desires to acquire 3 images at a time for triple
		// buffering
		if (mic < surfCapabilities.minImageCount) 
		{
			mic = surfCapabilities.minImageCount;
		}
		// If maxImageCount is 0, we can ask for as many images as we want;
		// otherwise we're limited to maxImageCount
		if ((surfCapabilities.maxImageCount > 0) && (mic > surfCapabilities.maxImageCount)) 
		{
			// Application must settle for fewer images than desired:
			mic = surfCapabilities.maxImageCount;
		}
	}

	//---------------------------------------------------------
	// サーフェストランスフォーム値
	//---------------------------------------------------------
//	VkSurfaceTransformFlagsKHR preTransform;
	VkSurfaceTransformFlagBitsKHR preTransform;
	{
		if ( surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ) 
		{
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		} else 
		{
			preTransform = surfCapabilities.currentTransform;
		}
	}

	//---------------------------------------------------------
	// コンポジットアルファフラグ値
	//---------------------------------------------------------
	VkCompositeAlphaFlagBitsKHR caf = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	{
		// Find a supported composite alpha mode - one of these is guaranteed to be set
		VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = 
		{
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++) 
		{
			if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) 
			{
				caf = compositeAlphaFlags[i];
				break;
			}
		}
	}

	//---------------------------------------------------------
	// スワップチェインの切り替え
	//---------------------------------------------------------
	{
		//---------------------------------------------------------
		// スクリーンサイズ値
		//---------------------------------------------------------
		VkExtent2D ext;
		{
			// width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
			if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) 
			{
				// If the surface size is undefined, the size is set to the size
				// of the images requested, which must fit within the minimum and
				// maximum values.
				ext.width = _width;
				ext.height =_height;

				if (ext.width < surfCapabilities.minImageExtent.width) 
				{
					ext.width = surfCapabilities.minImageExtent.width;
				} 
				else 
				if (ext.width > surfCapabilities.maxImageExtent.width) 
				{
					ext.width = surfCapabilities.maxImageExtent.width;
				}
				
				if (ext.height < surfCapabilities.minImageExtent.height) 
				{
					ext.height = surfCapabilities.minImageExtent.height;
				} 
				else 
				if (ext.height > surfCapabilities.maxImageExtent.height) 
				{
					ext.height = surfCapabilities.maxImageExtent.height;
				}
			} else 
			{
				// If the surface size is defined, the swap chain size must match
				ext = surfCapabilities.currentExtent;
	//			win.win_width = surfCapabilities.currentExtent.width;
	//			win.win_height = surfCapabilities.currentExtent.height;
			}
		}

		//---------------------------------------------------------
		// スワップチェインの保存
		//---------------------------------------------------------
		VkSwapchainKHR oldSwapchain = vk.swapchain;

		//---------------------------------------------------------
		// スワップチェインの作成
		//---------------------------------------------------------
		{
			VkSwapchainCreateInfoKHR sci = 
			{
				.sType 					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
				.pNext 					= NULL,
				.surface 				= vk.surface,
				.minImageCount 			= mic,
				.imageFormat 			= vk.format,
				.imageColorSpace 		= vk.color_space,
				.imageExtent 			=
					{
					   .width = ext.width
					 , .height = ext.height
					},
				.imageUsage 			= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				.preTransform 			= preTransform,
				.compositeAlpha 		= caf,
				.imageArrayLayers 		= 1,
				.imageSharingMode 		= VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount 	= 0,
				.pQueueFamilyIndices 	= NULL,
				.presentMode 			= swapchainPresentMode,
				.oldSwapchain 			= oldSwapchain,
				.clipped 				= true,
			};
			VkResult  err;
			err = vk.fpCreateSwapchainKHR(vk.device, &sci, NULL, &vk.swapchain);
			assert(!err);
		}
		//---------------------------------------------------------
		// 旧スワップチェインの廃棄
		//---------------------------------------------------------
		{
			// If we just re-created an existing swapchain, we should destroy the old
			// swapchain at this point.
			// Note: destroying the swapchain also cleans up all its associated
			// presentable images once the platform is done with them.
			if (oldSwapchain != VK_NULL_HANDLE) 
			{
				vk.fpDestroySwapchainKHR(vk.device, oldSwapchain, NULL);
			}
		}
	}

	//---------------------------------------------------------
	// スワップチェイン・イメージ数の取得
	//---------------------------------------------------------
	{
		VkResult  err;
		err = vk.fpGetSwapchainImagesKHR(vk.device, vk.swapchain, &vk.swapchainImageCount, NULL);
		assert(!err);
	}

	{
		//---------------------------------------------------------
		// スワップチェイン・イメージバッファの確保
		//---------------------------------------------------------
		VkImage *sci = (VkImage *)malloc(vk.swapchainImageCount * sizeof(VkImage));
		assert(sci);
		{
			VkResult  err;
			err = vk.fpGetSwapchainImagesKHR(vk.device, vk.swapchain, &vk.swapchainImageCount, sci);
			assert(!err);
		}

		//---------------------------------------------------------
		// イメージビューの作成
		//---------------------------------------------------------
		{
			vk.swapchain_image_resources = (SwapchainImageResources *)malloc(sizeof(SwapchainImageResources) * vk.swapchainImageCount);
			assert(vk.swapchain_image_resources);

			for (uint32_t i = 0; i < vk.swapchainImageCount; i++) 
			{
				VkImageViewCreateInfo ivci = 
				{
					.sType 			= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
					.pNext 			= NULL,
					.format 		= vk.format,
					.components 	=
					{
						.r = VK_COMPONENT_SWIZZLE_R,
						.g = VK_COMPONENT_SWIZZLE_G,
						.b = VK_COMPONENT_SWIZZLE_B,
						.a = VK_COMPONENT_SWIZZLE_A,
					},
					.subresourceRange = 
					{
						.aspectMask 	= VK_IMAGE_ASPECT_COLOR_BIT,
						.baseMipLevel 	= 0,
						.levelCount 	= 1,
						.baseArrayLayer = 0,
						.layerCount 	= 1
					},
					.viewType = VK_IMAGE_VIEW_TYPE_2D,
					.flags = 0,
					.image = sci[i],
				};

				vk.swapchain_image_resources[i].image = sci[i];

				{
					VkResult  err;
					err = vkCreateImageView(vk.device, &ivci, NULL, &vk.swapchain_image_resources[i].imgview);
					assert(!err);
				}
			}
		}

	}
	
	//---------------------------------------------------------
	// 物理デバイスメモリプロパティを取得
	//---------------------------------------------------------
	{
		vk.frame_index = 0;
		// Get Memory information and properties
		vkGetPhysicalDeviceMemoryProperties(vk.gpu, &vk.memory_properties);
	}
	
	//-----------------------------------------------------
	// フレームバッファの作成
	//-----------------------------------------------------
	{//demo_prepare_depth(&vk);
		const VkFormat depth_format = VK_FORMAT_D16_UNORM;
		const VkImageCreateInfo image = 
		{
			.sType 			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext 			= NULL,
			.imageType 		= VK_IMAGE_TYPE_2D,
			.format 		= depth_format,
			.extent 		= {_width, _height, 1},
			.mipLevels 		= 1,
			.arrayLayers 	= 1,
			.samples 		= VK_SAMPLE_COUNT_1_BIT,
			.tiling 		= VK_IMAGE_TILING_OPTIMAL,
			.usage 			= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			.flags 			= 0,
		};



		//-----------------------------------------------------
		// デプス作成
		//-----------------------------------------------------
		{
			VkMemoryRequirements mr;
			vk.depth_inf.format = depth_format;

			//-----------------------------------------------------
			// イメージの作成
			//-----------------------------------------------------
			{
				/* create image */
				VkResult  err;
				err = vkCreateImage(vk.device, &image, NULL, &vk.depth_inf.image);
				assert(!err);
			}

			//-----------------------------------------------------
			// イメージメモリ情報の取得
			//-----------------------------------------------------
			{
				vkGetImageMemoryRequirements(vk.device, vk.depth_inf.image, &mr);
			}

			vk.depth_inf.mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			vk.depth_inf.mem_alloc.pNext = NULL;
			vk.depth_inf.mem_alloc.allocationSize = mr.size;
			vk.depth_inf.mem_alloc.memoryTypeIndex = 0;

			//-----------------------------------------------------
			// 
			//-----------------------------------------------------
			{
				bool  pass = false;
				{
					VkPhysicalDeviceMemoryProperties* 	pMemory_properties 	= &vk.memory_properties;
					uint32_t 							typeBits			= mr.memoryTypeBits;
					VkFlags 							requirements_mask	= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
					uint32_t*							typeIndex			= &vk.depth_inf.mem_alloc.memoryTypeIndex;

					// Search memtypes to find first index with those properties
					for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) 
					{
						if ((typeBits & 1) == 1) 
						{
							// Type is available, does it match user properties?
							if ((pMemory_properties->memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) 
							{
								*typeIndex = i;
								pass = true;
								break;
							}
						}
						typeBits >>= 1;
					}
				}
				assert(pass);
			}

			//-----------------------------------------------------
			// メモリ確保：デプス 
			//-----------------------------------------------------
			{ 
				/* allocate memory */
				VkResult  err;
				err = vkAllocateMemory(vk.device, &vk.depth_inf.mem_alloc, NULL, &vk.depth_inf.devmem);
				assert(!err);
			}

			//-----------------------------------------------------
			// イメージメモリのバインド
			//-----------------------------------------------------
			{
				/* bind memory */
				VkResult  err;
				err = vkBindImageMemory(vk.device, vk.depth_inf.image, vk.depth_inf.devmem, 0);
				assert(!err);
			}
		}

		//-----------------------------------------------------
		// イメージビューの作成
		//-----------------------------------------------------
		{
			VkImageViewCreateInfo ivci = 
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.pNext = NULL,
				.image = VK_NULL_HANDLE,
				.format = depth_format,
				.subresourceRange = 
				{.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
									 .baseMipLevel = 0,
									 .levelCount = 1,
									 .baseArrayLayer = 0,
									 .layerCount = 1},
				.flags = 0,
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
			};
			/* create image imgview */
			ivci.image = vk.depth_inf.image;
			VkResult  err;
			err = vkCreateImageView(vk.device, &ivci, NULL, &vk.depth_inf.imgview);
			assert(!err);
		}
	}

	//-----------------------------------------------------
	// テクスチャイメージビューの作成
	//-----------------------------------------------------
	//demo_prepare_textures(&vk);
	{
		const VkFormat tex_format = VK_FORMAT_R8G8B8A8_UNORM;
		VkFormatProperties props;
		uint32_t i;

		vkGetPhysicalDeviceFormatProperties(vk.gpu, tex_format, &props);

		for (i = 0; i < DEMO_TEXTURE_COUNT; i++) 
		{

			if ((props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) ) 
			{
				/* Device can texture using linear textures */
				demo_prepare_texture_image(
					  vk.device
					, &vk.memory_properties
					, tex_files[i]
					, &vk.textures[i]
					, VK_IMAGE_TILING_LINEAR
					, VK_IMAGE_USAGE_SAMPLED_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);

				// Nothing in the pipeline needs to be complete to start, and don't allow fragment
				// shader to run until layout transition completes
				//-----------------------------------------------------
				//
				//-----------------------------------------------------
				{
					VkImage 				image				= vk.textures[i].image;
					VkImageAspectFlags 		aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
					VkImageLayout 			old_image_layout	= VK_IMAGE_LAYOUT_PREINITIALIZED;
					VkImageLayout 			new_image_layout	= vk.textures[i].imageLayout;
					VkAccessFlagBits 		srcAccessMask		= VK_ACCESS_HOST_WRITE_BIT;
					VkPipelineStageFlags 	src_stages			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
					VkPipelineStageFlags 	dest_stages			= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

					assert(vk.cmdbuf);

					VkImageMemoryBarrier imb = 
					{
						.sType 				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
						.pNext 				= NULL,
						.srcAccessMask 		= srcAccessMask,
						.dstAccessMask 		= 0,
						.oldLayout 			= old_image_layout,
						.newLayout 			= new_image_layout,
						.image 				= image,
						.subresourceRange 	= {aspectMask, 0, 1, 0, 1}
					};
					switch (new_image_layout) 
					{
					case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:				imb.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;										break;
					case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:			imb.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;								break;
					case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:	imb.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; 						break;
					case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:			imb.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;	break;
					case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: 				imb.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;										break;
					case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:					imb.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;											break;
					default:												imb.dstAccessMask = 0;																	break;
					}
					vkCmdPipelineBarrier(vk.cmdbuf, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &imb);
				}

				vk.staging_texture.image = 0;
			} 
			else 
			if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) 
			{
				/* Must use staging buffer to copy linear texture to optimized */

				memset(&vk.staging_texture, 0, sizeof(vk.staging_texture));
				demo_prepare_texture_image(
					  vk.device
					, &vk.memory_properties
					, tex_files[i]
					, &vk.staging_texture
					, VK_IMAGE_TILING_LINEAR
					, VK_IMAGE_USAGE_TRANSFER_SRC_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);

				demo_prepare_texture_image(
					  vk.device
					, &vk.memory_properties
					, tex_files[i]
					, &vk.textures[i]
					, VK_IMAGE_TILING_OPTIMAL
					,(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
					,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				);				

				//-----------------------------------------------------
				//
				//-----------------------------------------------------
				{
					VkImage 				image				= vk.staging_texture.image;
					VkImageAspectFlags 		aspectMask 			= VK_IMAGE_ASPECT_COLOR_BIT;
					VkImageLayout 			old_image_layout 	= VK_IMAGE_LAYOUT_PREINITIALIZED;
					VkImageLayout 			new_image_layout 	= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					VkAccessFlagBits 		srcAccessMask		= VK_ACCESS_HOST_WRITE_BIT;
					VkPipelineStageFlags 	src_stages			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
					VkPipelineStageFlags 	dest_stages			= VK_PIPELINE_STAGE_TRANSFER_BIT;
					assert(vk.cmdbuf);
					VkImageMemoryBarrier imb = 
					{
						.sType 				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
						.pNext 				= NULL,
						.srcAccessMask 		= srcAccessMask,
						.dstAccessMask 		= 0,
						.oldLayout 			= old_image_layout,
						.newLayout 			= new_image_layout,
						.image 				= image,
						.subresourceRange 	= {aspectMask, 0, 1, 0, 1}
					};
					switch (new_image_layout) 
					{
					case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:				imb.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;										break;
					case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:			imb.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;								break;
					case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:	imb.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; 						break;
					case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:			imb.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;	break;
					case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: 				imb.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;										break;
					case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:					imb.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;											break;
					default:												imb.dstAccessMask = 0;																	break;
					}
					vkCmdPipelineBarrier(vk.cmdbuf, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &imb);
				}

				//-----------------------------------------------------
				//
				//-----------------------------------------------------
				{
					VkImage 				image				= vk.textures[i].image;
					VkImageAspectFlags 		aspectMask 			= VK_IMAGE_ASPECT_COLOR_BIT;
					VkImageLayout 			old_image_layout 	= VK_IMAGE_LAYOUT_PREINITIALIZED;
					VkImageLayout 			new_image_layout 	= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					VkAccessFlagBits 		srcAccessMask		= VK_ACCESS_HOST_WRITE_BIT;
					VkPipelineStageFlags 	src_stages			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
					VkPipelineStageFlags 	dest_stages			= VK_PIPELINE_STAGE_TRANSFER_BIT;
					assert(vk.cmdbuf);
					VkImageMemoryBarrier imb = 
					{
						.sType 				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
						.pNext 				= NULL,
						.srcAccessMask 		= srcAccessMask,
						.dstAccessMask 		= 0,
						.oldLayout 			= old_image_layout,
						.newLayout 			= new_image_layout,
						.image 				= image,
						.subresourceRange 	= {aspectMask, 0, 1, 0, 1}
					};
					switch (new_image_layout) 
					{
					case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:				imb.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;										break;
					case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:			imb.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;								break;
					case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:	imb.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; 						break;
					case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:			imb.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;	break;
					case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: 				imb.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;										break;
					case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:					imb.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;											break;
					default:												imb.dstAccessMask = 0;																	break;
					}
					vkCmdPipelineBarrier(vk.cmdbuf, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &imb);
				}

				//-----------------------------------------------------
				//
				//-----------------------------------------------------
				{
					VkImageCopy ic = 
					{
						.srcSubresource 	= {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
						.srcOffset 			= {0, 0, 0},
						.dstSubresource 	= {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
						.dstOffset 			= {0, 0, 0},
						.extent 			= {vk.staging_texture.tex_width,vk.staging_texture.tex_height, 1},
					};
					vkCmdCopyImage(
						  vk.cmdbuf
						, vk.staging_texture.image
						, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
						, vk.textures[i].image
						, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
						, 1
						, &ic
					);
				}

				
				//-----------------------------------------------------
				//
				//-----------------------------------------------------
				{
					VkImage 				image				= vk.textures[i].image;
					VkImageAspectFlags 		aspectMask 			= VK_IMAGE_ASPECT_COLOR_BIT;
					VkImageLayout 			old_image_layout 	= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					VkImageLayout 			new_image_layout 	= vk.textures[i].imageLayout;
					VkAccessFlagBits 		srcAccessMask		= VK_ACCESS_TRANSFER_WRITE_BIT;
					VkPipelineStageFlags 	src_stages			= VK_PIPELINE_STAGE_TRANSFER_BIT;
					VkPipelineStageFlags 	dest_stages			= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
					assert(vk.cmdbuf);
					VkImageMemoryBarrier imb = 
					{
						.sType 				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
						.pNext 				= NULL,
						.srcAccessMask 		= srcAccessMask,
						.dstAccessMask 		= 0,
						.oldLayout 			= old_image_layout,
						.newLayout		 	= new_image_layout,
						.image 				= image,
						.subresourceRange 	= {aspectMask, 0, 1, 0, 1}
					};
					switch (new_image_layout) 
					{
					case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:				imb.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;										break;
					case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:			imb.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;								break;
					case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:	imb.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; 						break;
					case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:			imb.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;	break;
					case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: 				imb.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;										break;
					case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:					imb.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;											break;
					default:												imb.dstAccessMask = 0;																	break;
					}
					vkCmdPipelineBarrier(vk.cmdbuf, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &imb);
				}

			} 
			else 
			{
				/* Can't support VK_FORMAT_R8G8B8A8_UNORM !? */
				assert(!"No support for R8G8B8A8_UNORM as texture image format");
			}


			//-----------------------------------------------------
			// サンプラーの作成
			//-----------------------------------------------------
			{
				/* create sampler */
				const VkSamplerCreateInfo sci = 
				{
					.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
					.pNext = NULL,
					.magFilter = VK_FILTER_NEAREST,
					.minFilter = VK_FILTER_NEAREST,
					.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
					.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
					.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
					.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
					.mipLodBias = 0.0f,
					.anisotropyEnable = VK_FALSE,
					.maxAnisotropy = 1,
					.compareOp = VK_COMPARE_OP_NEVER,
					.minLod = 0.0f,
					.maxLod = 0.0f,
					.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
					.unnormalizedCoordinates = VK_FALSE,
				};
				VkResult  err;
				err = vkCreateSampler(vk.device, &sci, NULL, &vk.textures[i].sampler);
				assert(!err);
			}

			//-----------------------------------------------------
			// イメージビューの作成
			//-----------------------------------------------------
			{
				/* create image imgview */
				VkImageViewCreateInfo ivc = 
				{
					.sType 		= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
					.pNext 		= NULL,
					.image 		= VK_NULL_HANDLE,
					.viewType 	= VK_IMAGE_VIEW_TYPE_2D,
					.format 	= tex_format,
					.components =
						{
							VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
							VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
						},
					.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
					.flags = 0,
				};
				ivc.image = vk.textures[i].image;
				VkResult  err;
				err = vkCreateImageView(vk.device, &ivc, NULL, &vk.textures[i].imgview);
				assert(!err);
			}
			
		}
	}

}

//-----------------------------------------------------------------------------
void vk_setVert( VulkanInf& vk, void* pDataVert, int sizeVert )
//-----------------------------------------------------------------------------
{
	//-----------------------------------------------------
	// バーテックスバッファの作成
	//-----------------------------------------------------
	for (unsigned int i = 0; i < vk.swapchainImageCount; i++) 
	{
		//-----------------------------------------------------
		// バッファ作成
		//-----------------------------------------------------
		{
			VkBufferCreateInfo bci;
			{
				memset(&bci, 0, sizeof(bci));
				bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bci.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
				bci.size = sizeVert;//sizeof(dataVert);
			}
			VkResult  err;
			err = vkCreateBuffer(vk.device, &bci, NULL, &vk.swapchain_image_resources[i].uniform_buffer);
			assert(!err);
		}

		//-----------------------------------------------------
		// バッファのメモリ要求取得
		//-----------------------------------------------------
		{
			VkMemoryRequirements mr;
			vkGetBufferMemoryRequirements(vk.device, vk.swapchain_image_resources[i].uniform_buffer, &mr);

			VkMemoryAllocateInfo mai;
			mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			mai.pNext = NULL;
			mai.allocationSize = mr.size;
			mai.memoryTypeIndex = 0;

			bool  pass = false;
			{
				VkPhysicalDeviceMemoryProperties* 	pMemory_properties 	= &vk.memory_properties;
				uint32_t 							typeBits			= mr.memoryTypeBits;
				VkFlags 							requirements_mask	= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
				uint32_t*							typeIndex			= &mai.memoryTypeIndex;

				// Search memtypes to find first index with those properties
				for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) 
				{
					if ((typeBits & 1) == 1) 
					{
						// Type is available, does it match user properties?
						if ((pMemory_properties->memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) 
						{
							*typeIndex = i;
							pass = true;
							break;
						}
					}
					typeBits >>= 1;
				}
			}
			assert(pass);

			//-----------------------------------------------------
			// メモリの確保
			//-----------------------------------------------------
			{
				VkResult  err;
				err = vkAllocateMemory(vk.device, &mai, NULL,&vk.swapchain_image_resources[i].uniform_memory);
				assert(!err);
			}
		}

		//-----------------------------------------------------
		// メモリ転送
		//-----------------------------------------------------
		{
			//-----------------------------------------------------
			// マップメモリ
			//-----------------------------------------------------
			uint8_t *pData;
			{
				VkResult  err;
				err = vkMapMemory(vk.device, vk.swapchain_image_resources[i].uniform_memory, 0, VK_WHOLE_SIZE, 0, (void **)&pData);
				assert(!err);
			}

			//-----------------------------------------------------
			// 転送
			//-----------------------------------------------------
			memcpy(pData, pDataVert, sizeVert);

			//-----------------------------------------------------
			// マップ解放
			//-----------------------------------------------------
			vkUnmapMemory(vk.device, vk.swapchain_image_resources[i].uniform_memory);
		}


		//-----------------------------------------------------
		// バインド
		//-----------------------------------------------------
		{
			VkResult  err;
			err = vkBindBufferMemory(
				  vk.device
				, vk.swapchain_image_resources[i].uniform_buffer
				, vk.swapchain_image_resources[i].uniform_memory
				, 0
			);
			assert(!err);
		}
	}
}
//-----------------------------------------------------------------------------
void	vk_setPipeline( 
//-----------------------------------------------------------------------------
	  VulkanInf& vk
	, int _width
	, int _height
	, uint32_t _vertexCount
	, uint32_t _instanceCount
	, uint32_t _firstVertex
	, uint32_t _firstInstance
)
{
	//-----------------------------------------------------
	// パイプラインレイアウトの取得
	//-----------------------------------------------------
	{
		const VkDescriptorSetLayoutBinding dslb[2] = 
		{
			[0] =
				
				{
				 .binding = 0,
				 .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				 .descriptorCount = 1,
				 .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
				 .pImmutableSamplers = NULL,
				},
			[1] =
				
				{
				 .binding = 1,
				 .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				 .descriptorCount = DEMO_TEXTURE_COUNT,
				 .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
				 .pImmutableSamplers = NULL,
				},
		};
		{
			const VkDescriptorSetLayoutCreateInfo dslci = 
			{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.pNext = NULL,
				.bindingCount = 2,
				.pBindings = dslb,
			};
			VkResult  err;
			err = vkCreateDescriptorSetLayout(vk.device, &dslci, NULL, &vk.desc_layout);
			assert(!err);
		}
	}

	//-----------------------------------------------------
	// パイプラインレイアウトの作成
	//-----------------------------------------------------
	{
		const VkPipelineLayoutCreateInfo plci = 
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = NULL,
			.setLayoutCount = 1,
			.pSetLayouts = &vk.desc_layout,
		};

		VkResult  err;
		err = vkCreatePipelineLayout(vk.device, &plci, NULL, &vk.pipeline_layout);
		assert(!err);
	}

	//-----------------------------------------------------
	// レンダーパスの作成
	//-----------------------------------------------------
	{//demo_prepare_render_pass(&vk);
		// The initial layout for the color and depth_inf attachments will be LAYOUT_UNDEFINED
		// because at the start of the renderpass, we don't care about their contents.
		// At the start of the subpass, the color attachment's layout will be transitioned
		// to LAYOUT_COLOR_ATTACHMENT_OPTIMAL and the depth_inf stencil attachment's layout
		// will be transitioned to LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL.  At the end of
		// the renderpass, the color attachment's layout will be transitioned to
		// LAYOUT_PRESENT_SRC_KHR to be ready to present.  This is all done as part of
		// the renderpass, no barriers are necessary.
		const VkAttachmentDescription ad[2] = 
		{
			[0] =
				
				{
				 .format			= vk.format,
				 .flags 			= 0,
				 .samples 			= VK_SAMPLE_COUNT_1_BIT,
				 .loadOp 			= VK_ATTACHMENT_LOAD_OP_CLEAR,
				 .storeOp 			= VK_ATTACHMENT_STORE_OP_STORE,
				 .stencilLoadOp 	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				 .stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE,
				 .initialLayout 	= VK_IMAGE_LAYOUT_UNDEFINED,
				 .finalLayout 		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				},
			[1] =
				
				{
				 .format 			= vk.depth_inf.format,
				 .flags 			= 0,
				 .samples 			= VK_SAMPLE_COUNT_1_BIT,
				 .loadOp 			= VK_ATTACHMENT_LOAD_OP_CLEAR,
				 .storeOp 			= VK_ATTACHMENT_STORE_OP_DONT_CARE,
				 .stencilLoadOp 	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				 .stencilStoreOp 	= VK_ATTACHMENT_STORE_OP_DONT_CARE,
				 .initialLayout 	= VK_IMAGE_LAYOUT_UNDEFINED,
				 .finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				},
		};

		//---
		
		const VkAttachmentReference color_reference = 
		{
			.attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		};
		const VkAttachmentReference depth_reference = 
		{
			.attachment = 1,
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		};
		const VkSubpassDescription subpass = 
		{
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.flags = 0,
			.inputAttachmentCount = 0,
			.pInputAttachments = NULL,
			.colorAttachmentCount = 1,
			.pColorAttachments = &color_reference,
			.pResolveAttachments = NULL,
			.pDepthStencilAttachment = &depth_reference,
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = NULL,
		};

		//-----------------------------------------------------
		// レンダーパスの取得
		//-----------------------------------------------------
		{
			const VkRenderPassCreateInfo rp_info = 
			{
				.sType 				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
				.pNext 				= NULL,
				.flags 				= 0,
				.attachmentCount 	= 2,
				.pAttachments 		= ad,
				.subpassCount 		= 1,
				.pSubpasses 		= &subpass,
				.dependencyCount 	= 0,
				.pDependencies 		= NULL,
			};

			VkResult  err;
			err = vkCreateRenderPass(vk.device, &rp_info, NULL, &vk.render_pass);
			assert(!err);
		}
	}

	//-----------------------------------------------------
	// パイプラインキャッシュの作成
	//-----------------------------------------------------
	{
		VkPipelineCacheCreateInfo ppc;
		memset(&ppc, 0, sizeof(ppc));
		ppc.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

		VkResult  err;
		err = vkCreatePipelineCache(vk.device, &ppc, NULL, &vk.pipelineCache);
		assert(!err);
	}

	//-----------------------------------------------------
	//
	//-----------------------------------------------------
	VkPipelineVertexInputStateCreateInfo vi;
	{
		memset(&vi, 0, sizeof(vi));
		vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	}

	//-----------------------------------------------------
	//
	//-----------------------------------------------------
	VkPipelineInputAssemblyStateCreateInfo ia;
	{
		memset(&ia, 0, sizeof(ia));
		ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}

	//-----------------------------------------------------
	//
	//-----------------------------------------------------
	VkPipelineRasterizationStateCreateInfo rs;
	{
		memset(&rs, 0, sizeof(rs));
		rs.sType 					= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rs.polygonMode 				= VK_POLYGON_MODE_FILL;
		rs.cullMode 				= VK_CULL_MODE_BACK_BIT;
		rs.frontFace 				= VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rs.depthClampEnable 		= VK_FALSE;
		rs.rasterizerDiscardEnable 	= VK_FALSE;
		rs.depthBiasEnable 			= VK_FALSE;
		rs.lineWidth 				= 1.0f;
	}

	//-----------------------------------------------------
	//
	//-----------------------------------------------------
	VkPipelineColorBlendStateCreateInfo cb;
	{
		memset(&cb, 0, sizeof(cb));
		cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		VkPipelineColorBlendAttachmentState att_state[1];
		memset(att_state, 0, sizeof(att_state));
		att_state[0].colorWriteMask = 0xf;
		att_state[0].blendEnable = VK_FALSE;
		cb.attachmentCount = 1;
		cb.pAttachments = att_state;
	}

	//-----------------------------------------------------
	//
	//-----------------------------------------------------
	VkPipelineDepthStencilStateCreateInfo dsci;
	{
		memset(&dsci, 0, sizeof(dsci));
		dsci.sType 					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		dsci.depthTestEnable 		= VK_TRUE;
		dsci.depthWriteEnable 		= VK_TRUE;
		dsci.depthCompareOp 		= VK_COMPARE_OP_LESS_OR_EQUAL;
		dsci.depthBoundsTestEnable 	= VK_FALSE;
		dsci.back.failOp 			= VK_STENCIL_OP_KEEP;
		dsci.back.passOp 			= VK_STENCIL_OP_KEEP;
		dsci.back.compareOp			= VK_COMPARE_OP_ALWAYS;
		dsci.stencilTestEnable 		= VK_FALSE;
		dsci.front 					= dsci.back;
	}
	
	//-----------------------------------------------------
	//
	//-----------------------------------------------------
	VkPipelineViewportStateCreateInfo vp;
	{
		memset(&vp, 0, sizeof(vp));
		vp.sType 			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		vp.viewportCount 	= 1;
		vp.scissorCount 	= 1;
	}

	//-----------------------------------------------------
	//
	//-----------------------------------------------------
	VkPipelineMultisampleStateCreateInfo ms;
	{
		memset(&ms, 0, sizeof(ms));
		ms.sType 				= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		ms.pSampleMask 			= NULL;
		ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	}

	//-----------------------------------------------------
	//
	//-----------------------------------------------------
	VkDynamicState ds[VK_DYNAMIC_STATE_RANGE_SIZE];
	VkPipelineDynamicStateCreateInfo pdsci;
	{
		memset(&pdsci, 0, sizeof pdsci);
		pdsci.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

		memset(ds, 0, sizeof ds);
		pdsci.pDynamicStates = ds;
		ds[pdsci.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
		ds[pdsci.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
	}

	//-----------------------------------------------------
	// グラフィックパイプラインの作成
	//-----------------------------------------------------
	{
		VkShaderModule vert_sm;
		VkShaderModule flag_sm;
		//-----------------------------------------------------
		// シェーダーモジュールの作成
		//-----------------------------------------------------
		VkPipelineShaderStageCreateInfo pssci[2];
		{
			memset(&pssci, 0, 2 * sizeof(VkPipelineShaderStageCreateInfo));

			//-----------------------------------------------------
			// バーテックスシェーダー読み込みモジュールの作成
			//-----------------------------------------------------
			{
				pssci[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				pssci[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
				{
					void *vcode;
					size_t size;

					vcode = demo_read_spv("cube-vert.spv", &size);
					if (!vcode) 
					{
						ERR_EXIT("Failed to load cube-vert.spv", "Load Shader Failure");
					}

					{
						VkShaderModuleCreateInfo smci;
						smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
						smci.pNext = NULL;
						smci.codeSize = size;
						smci.pCode = (uint32_t*)vcode;
						smci.flags = 0;

						VkResult  err;
						err = vkCreateShaderModule(vk.device, &smci, NULL, &vert_sm);
						assert(!err);
					}

					free(vcode);

					pssci[0].module = vert_sm;
				}
				pssci[0].pName = "main";
			}

			//-----------------------------------------------------
			// フラグメントシェーダーモジュールの作成
			//-----------------------------------------------------
			{
				pssci[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				pssci[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				{
					void *fcode;
					size_t size;

					fcode = demo_read_spv("cube-frag.spv", &size);
					if (!fcode) 
					{
						ERR_EXIT("Failed to load cube-frag.spv", "Load Shader Failure");
					}

					{
						VkShaderModuleCreateInfo smci;
						smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
						smci.pNext = NULL;
						smci.codeSize = size;
						smci.pCode = (uint32_t*)fcode;
						smci.flags = 0;
						VkResult  err;
						err = vkCreateShaderModule(vk.device, &smci, NULL, &flag_sm);
						assert(!err);
					}

					free(fcode);

					pssci[1].module = flag_sm;//demo_prepare_fs(&vk);
				}
				pssci[1].pName = "main";
			}
		}

		//-----------------------------------------------------
		// グラフィックパイプラインの生成
		//-----------------------------------------------------
		{
			VkGraphicsPipelineCreateInfo gpci;
			memset(&gpci, 0, sizeof(gpci));
			gpci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			gpci.layout = vk.pipeline_layout;
			gpci.stageCount = 2; // Two stages: vs and fs

			gpci.pVertexInputState 		= &vi;
			gpci.pInputAssemblyState 	= &ia;
			gpci.pRasterizationState 	= &rs;
			gpci.pColorBlendState 		= &cb;
			gpci.pMultisampleState 		= &ms;
			gpci.pViewportState 		= &vp;
			gpci.pDepthStencilState 	= &dsci;
			gpci.pStages 				= pssci;
			gpci.renderPass 			= vk.render_pass;
			gpci.pDynamicState 			= &pdsci;
			gpci.renderPass 			= vk.render_pass;

			VkResult  err;
			err = vkCreateGraphicsPipelines(vk.device, vk.pipelineCache, 1, &gpci, NULL, &vk.pipeline);
			assert(!err);
		}

		//-----------------------------------------------------
		// シェーダーモジュールの廃棄
		//-----------------------------------------------------
		vkDestroyShaderModule(vk.device, flag_sm, NULL);
		vkDestroyShaderModule(vk.device, vert_sm, NULL);
	}

	//-----------------------------------------------------
	// コマンドバッファの作成
	//-----------------------------------------------------
	for (uint32_t i = 0; i < vk.swapchainImageCount; i++) 
	{
		const VkCommandBufferAllocateInfo cmai = 
		{
			.sType 				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext 				= NULL,
			.commandPool 		= vk.cmd_pool,
			.level 				= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};
		VkResult  err;
		err = vkAllocateCommandBuffers(vk.device, &cmai, &vk.swapchain_image_resources[i].cmdbuf);
		assert(!err);
	}

	//-----------------------------------------------------
	// セパレート描画キュー作成
	//-----------------------------------------------------
	if (vk.flg_separate_present_queue) 
	{

		//-----------------------------------------------------
		// コマンドプールの作成
		//-----------------------------------------------------
		{
			const VkCommandPoolCreateInfo cpci = 
			{
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.pNext = NULL,
				.queueFamilyIndex = vk.q_present_queue_family_index,
				.flags = 0,
			};
			VkResult  err;
			err = vkCreateCommandPool(vk.device, &cpci, NULL, &vk.present_cmd_pool);
			assert(!err);
		}

		//-----------------------------------------------------
		// スワップチェイン分のコマンドバッファの作成
		//-----------------------------------------------------
		for (uint32_t i = 0; i < vk.swapchainImageCount; i++) 
		{
			//-----------------------------------------------------
			// コマンドバッファの確保
			//-----------------------------------------------------
			{
				const VkCommandBufferAllocateInfo cbai = 
				{
					.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
					.pNext = NULL,
					.commandPool = vk.present_cmd_pool,
					.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
					.commandBufferCount = 1,
				};

				VkResult  err;
				err = vkAllocateCommandBuffers( vk.device, &cbai, &vk.swapchain_image_resources[i].graphics_to_present_cmdbuf);
				assert(!err);
			}

			//-----------------------------------------------------
			// コマンドバッファの開始
			//-----------------------------------------------------
			{
				const VkCommandBufferBeginInfo cb = 
				{
					.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
					.pNext = NULL,
					.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
					.pInheritanceInfo = NULL,
				};
				VkResult  err;
				err = vkBeginCommandBuffer(vk.swapchain_image_resources[i].graphics_to_present_cmdbuf, &cb);
				assert(!err);
			}

			//-----------------------------------------------------
			// コマンドパイプラインバリア
			//-----------------------------------------------------
			{
				VkImageMemoryBarrier imb = 
				{
					.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
					.pNext = NULL,
					.srcAccessMask = 0,
					.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					.srcQueueFamilyIndex = vk.q_graphics_queue_family_index,
					.dstQueueFamilyIndex = vk.q_present_queue_family_index,
					.image = vk.swapchain_image_resources[i].image,
					.subresourceRange = 
					{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
				};

				vkCmdPipelineBarrier(
					vk.swapchain_image_resources[i].graphics_to_present_cmdbuf,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0,
					NULL, 0, NULL, 1, &imb
				);
			}

			//-----------------------------------------------------
			// コマンドバッファ終了
			//-----------------------------------------------------
			{
				VkResult  err;
				err = vkEndCommandBuffer(vk.swapchain_image_resources[i].graphics_to_present_cmdbuf);
				assert(!err);
			}
		}
	}

	//-----------------------------------------------------
	// デスクリプタプールの作成
	//-----------------------------------------------------
	{//demo_prepare_dpci(&vk);
		const VkDescriptorPoolSize dps[2] = 
		{
			[0] =
				{
				 .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				 .descriptorCount = vk.swapchainImageCount,
				},
			[1] =
				{
				 .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				 .descriptorCount = vk.swapchainImageCount * DEMO_TEXTURE_COUNT,
				},
		};
		const VkDescriptorPoolCreateInfo dpci = 
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.pNext = NULL,
			.maxSets = vk.swapchainImageCount,
			.poolSizeCount = 2,
			.pPoolSizes = dps,
		};
		VkResult  err;
		err = vkCreateDescriptorPool(vk.device, &dpci, NULL, &vk.desc_pool);
		assert(!err);
	}
	
	//-----------------------------------------------------
	// ディスクリプターの作成
	//-----------------------------------------------------
	{
		//-----------------------------------------------------
		// 
		//-----------------------------------------------------
		VkDescriptorImageInfo dif[DEMO_TEXTURE_COUNT];
		memset(&dif, 0, sizeof(dif));
		for (unsigned int i = 0; i < DEMO_TEXTURE_COUNT; i++) 
		{
			dif[i].sampler = vk.textures[i].sampler;
			dif[i].imageView = vk.textures[i].imgview;
			dif[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		}

		//-----------------------------------------------------
		// 
		//-----------------------------------------------------
		VkDescriptorBufferInfo dbi;
		dbi.offset = 0;
		dbi.range = sizeof(struct vktexcube_vs_uniform);

		//-----------------------------------------------------
		// 
		//-----------------------------------------------------
		VkWriteDescriptorSet writes[2];
		memset(&writes, 0, sizeof(writes));

		writes[0].sType 			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[0].descriptorCount 	= 1;
		writes[0].descriptorType 	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writes[0].pBufferInfo 		= &dbi;

		writes[1].sType 			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[1].dstBinding 		= 1;
		writes[1].descriptorCount 	= DEMO_TEXTURE_COUNT;
		writes[1].descriptorType 	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writes[1].pImageInfo 		= dif;

		//-----------------------------------------------------
		// 
		//-----------------------------------------------------
		for (unsigned int i = 0; i < vk.swapchainImageCount; i++) 
		{
			//-----------------------------------------------------
			// ディスクリプターセットの確保
			//-----------------------------------------------------
			{
				VkDescriptorSetAllocateInfo dsai = 
				{
					.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
					.pNext 				= NULL,
					.descriptorPool 	= vk.desc_pool,
					.descriptorSetCount = 1,
					.pSetLayouts 		= &vk.desc_layout
				};
				VkResult  err;
				err = vkAllocateDescriptorSets(vk.device, &dsai, &vk.swapchain_image_resources[i].descriptor_set);
				assert(!err);
			}
			//-----------------------------------------------------
			// ディスクリプターセットの更新
			//-----------------------------------------------------
			{
				dbi.buffer 			= vk.swapchain_image_resources[i].uniform_buffer;
				writes[0].dstSet 	= vk.swapchain_image_resources[i].descriptor_set;
				writes[1].dstSet 	= vk.swapchain_image_resources[i].descriptor_set;
				vkUpdateDescriptorSets(vk.device, 2, writes, 0, NULL);
			}
		}
	}

	//-----------------------------------------------------
	// フレームバッファの作成
	//-----------------------------------------------------
	{
		VkImageView attachments[2];
		attachments[1] = vk.depth_inf.imgview;

		const VkFramebufferCreateInfo fci = 
		{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = NULL,
			.renderPass = vk.render_pass,
			.attachmentCount = 2,
			.pAttachments = attachments,
			.width = _width,
			.height = _height,
			.layers = 1,
		};
		VkResult  err;
		uint32_t i;

		for (i = 0; i < vk.swapchainImageCount; i++) 
		{
			attachments[0] = vk.swapchain_image_resources[i].imgview;
			err = vkCreateFramebuffer(vk.device, &fci, NULL, &vk.swapchain_image_resources[i].framebuffer);
			assert(!err);
		}
	}

	//-----------------------------------------------------
	// 描画コマンドバッファの作成
	//-----------------------------------------------------
	for (uint32_t i = 0; i < vk.swapchainImageCount; i++) 
	{
		vk.current_buffer = i;
		
		VkCommandBuffer cmd_buf = vk.swapchain_image_resources[i].cmdbuf;
		{

			//-----------------------------------------------------
			// コマンドバッファの開始
			//-----------------------------------------------------
			{
				const VkCommandBufferBeginInfo cbbi = 
				{
					.sType 				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
					.pNext 				= NULL,
					.flags 				= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
					.pInheritanceInfo 	= NULL,
				};
				VkResult  err;
				err = vkBeginCommandBuffer(cmd_buf, &cbbi);
				assert(!err);
			}

			//-----------------------------------------------------
			// コマンド・レンダーパスの開始
			//-----------------------------------------------------
			{
				const VkClearValue cv[2] = 
				{
					[0] = 
					{
						.color.float32 = {0.2f, 0.2f, 0.2f, 0.2f}
					}
					,
					[1] = 
					{
						.depthStencil = {1.0f, 0}
					},
				};
				const VkRenderPassBeginInfo rpbi = 
				{
					.sType 						= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
					.pNext 						= NULL,
					.renderPass 				= vk.render_pass,
					.framebuffer 				= vk.swapchain_image_resources[vk.current_buffer].framebuffer,
					.renderArea.offset.x 		= 0,
					.renderArea.offset.y 		= 0,
					.renderArea.extent.width 	= _width,
					.renderArea.extent.height 	= _height,
					.clearValueCount 			= 2,
					.pClearValues 				= cv,
				};
				vkCmdBeginRenderPass(cmd_buf, &rpbi, VK_SUBPASS_CONTENTS_INLINE);
			}
			
			//-----------------------------------------------------
			// コマンド・パイプラインのバインド
			//-----------------------------------------------------
			vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, vk.pipeline );

			//-----------------------------------------------------
			// コマンド・ディスクリプターのバインド
			//-----------------------------------------------------
			vkCmdBindDescriptorSets(
				 cmd_buf
				,VK_PIPELINE_BIND_POINT_GRAPHICS
				,vk.pipeline_layout
				,0
				,1
				,&vk.swapchain_image_resources[vk.current_buffer].descriptor_set
				,0
				,NULL
			);
			
			//-----------------------------------------------------
			// ビューポートの設定
			//-----------------------------------------------------
			{
				VkViewport viewport;
				memset(&viewport, 0, sizeof(viewport));
				viewport.height = (float)_height;
				viewport.width = (float)_width;
				viewport.minDepth = (float)0.0f;
				viewport.maxDepth = (float)1.0f;
				vkCmdSetViewport(cmd_buf, 0, 1, &viewport);
			}

			//-----------------------------------------------------
			// シザリングエリアの設定
			//-----------------------------------------------------
			{
				VkRect2D scissor;
				memset(&scissor, 0, sizeof(scissor));
				scissor.extent.width = _width;
				scissor.extent.height = _height;
				scissor.offset.x = 0;
				scissor.offset.y = 0;
				vkCmdSetScissor(cmd_buf, 0, 1, &scissor);
			}

			//-----------------------------------------------------
			// 描画コマンド発行
			//-----------------------------------------------------
			vkCmdDraw(cmd_buf, _vertexCount, _instanceCount, _firstVertex, _firstInstance);

			//-----------------------------------------------------
			// レンダーパス終了
			//-----------------------------------------------------
			// Note that ending the renderpass changes the image's layout from
			// COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR
			vkCmdEndRenderPass(cmd_buf);

			//-----------------------------------------------------
			// パイプラインバリアの設定
			//-----------------------------------------------------
			if (vk.flg_separate_present_queue) 
			{
				// We have to transfer ownership from the graphics queue family to the
				// present queue family to be able to present.  Note that we don't have
				// to transfer from present queue family back to graphics queue family at
				// the start of the next frame because we don't care about the image's
				// contents at that point.
				VkImageMemoryBarrier imb = 
				{
					.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
					.pNext = NULL,
					.srcAccessMask = 0,
					.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					.srcQueueFamilyIndex = vk.q_graphics_queue_family_index,
					.dstQueueFamilyIndex = vk.q_present_queue_family_index,
					.image = vk.swapchain_image_resources[vk.current_buffer].image,
					.subresourceRange = 
					{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

				vkCmdPipelineBarrier(
					   cmd_buf
					 , VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					 , VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
					 , 0
					 , 0
					 , NULL
					 , 0
					 , NULL
					 , 1
					 , &imb
				);
			}

			//-----------------------------------------------------
			// コマンドバッファ終了
			//-----------------------------------------------------
			{
				VkResult  err;
				err = vkEndCommandBuffer(cmd_buf);
				assert(!err);
			}
		}
	}
}

//-----------------------------------------------------------------------------
void vk_endSetup( VulkanInf& vk )
//-----------------------------------------------------------------------------
{
	/*
	 * Prepare functions above may generate pipeline commands
	 * that need to be flushed before beginning the render loop.
	 */

	// This function could get called twice if the texture uses a staging buffer
	// In that case the second call should be ignored
	if (vk.cmdbuf == VK_NULL_HANDLE)
	{
	//	return;
	}
	else
	{
		//-----------------------------------------------------
		// コマンドバッファ終了
		//-----------------------------------------------------
		{
			VkResult  err;
			err = vkEndCommandBuffer(vk.cmdbuf);
			assert(!err);
		}

		//-----------------------------------------------------
		// フェンス処理
		//-----------------------------------------------------
		{	
			//-----------------------------------------------------
			// フェンスの作成
			//-----------------------------------------------------
			VkFence fence;
			{
				VkFenceCreateInfo fci = 
				{
					.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
					.pNext = NULL,
					.flags = 0
				};
				VkResult  err;
				err = vkCreateFence(vk.device, &fci, NULL, &fence);
				assert(!err);
			}

			//-----------------------------------------------------
			// フェンス待ち
			//-----------------------------------------------------
			{
				const VkCommandBuffer cmd_bufs[] = {vk.cmdbuf};
				{
					VkSubmitInfo si = 
					{
						.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
						.pNext = NULL,
						.waitSemaphoreCount = 0,
						.pWaitSemaphores = NULL,
						.pWaitDstStageMask = NULL,
						.commandBufferCount = 1,
						.pCommandBuffers = cmd_bufs,
						.signalSemaphoreCount = 0,
						.pSignalSemaphores = NULL
					};
					VkResult  err;
					err = vkQueueSubmit(vk.graphics_queue, 1, &si, fence);
					assert(!err);
				}

				{
					VkResult  err;
					err = vkWaitForFences(vk.device, 1, &fence, VK_TRUE, UINT64_MAX);
					assert(!err);
				}

				vkFreeCommandBuffers(vk.device, vk.cmd_pool, 1, cmd_bufs);
			}

			//-----------------------------------------------------
			// フェンスの廃棄
			//-----------------------------------------------------
			vkDestroyFence(vk.device, fence, NULL);

		}
		vk.cmdbuf = VK_NULL_HANDLE;
	}

	//-----------------------------------------------------
	// 
	//-----------------------------------------------------
	if (vk.staging_texture.image) 
	{
		//demo_destroy_texture_image(&vk, &vk.staging_texture);
		{
			/* clean up staging resources */
			vkFreeMemory(vk.device, vk.staging_texture.devmem, NULL);
			vkDestroyImage(vk.device, vk.staging_texture.image, NULL);
		}
	}

	//-----------------------------------------------------
	// 
	//-----------------------------------------------------
	{
		vk.current_buffer = 0;
	}
}

//-----------------------------------------------------------------------------
void	vk_end( VulkanInf& vk )
//-----------------------------------------------------------------------------
{
	//---------------------------------------------------------
	// 終了
	//---------------------------------------------------------
	{
		uint32_t i;

		vkDeviceWaitIdle(vk.device);

		// Wait for fences from present operations
		for (i = 0; i < FRAME_LAG; i++) 
		{
			vkWaitForFences(vk.device, 1, &vk.fences[i], VK_TRUE, UINT64_MAX);
			vkDestroyFence(vk.device, vk.fences[i], NULL);
			vkDestroySemaphore(vk.device, vk.image_acquired_semaphores[i], NULL);
			vkDestroySemaphore(vk.device, vk.draw_complete_semaphores[i], NULL);
			if (vk.flg_separate_present_queue) 
			{
				vkDestroySemaphore(vk.device, vk.image_ownership_semaphores[i], NULL);
			}
		}

		for (i = 0; i < vk.swapchainImageCount; i++) 
		{
			vkDestroyFramebuffer(vk.device, vk.swapchain_image_resources[i].framebuffer, NULL);
		}
		vkDestroyDescriptorPool(vk.device, vk.desc_pool, NULL);

		vkDestroyPipeline(vk.device, vk.pipeline, NULL);
		vkDestroyPipelineCache(vk.device, vk.pipelineCache, NULL);
		vkDestroyRenderPass(vk.device, vk.render_pass, NULL);
		vkDestroyPipelineLayout(vk.device, vk.pipeline_layout, NULL);
		vkDestroyDescriptorSetLayout(vk.device, vk.desc_layout, NULL);

		for (i = 0; i < DEMO_TEXTURE_COUNT; i++) 
		{
			vkDestroyImageView(vk.device, vk.textures[i].imgview, NULL);
			vkDestroyImage(vk.device, vk.textures[i].image, NULL);
			vkFreeMemory(vk.device, vk.textures[i].devmem, NULL);
			vkDestroySampler(vk.device, vk.textures[i].sampler, NULL);
		}
		vk.fpDestroySwapchainKHR(vk.device, vk.swapchain, NULL);

		vkDestroyImageView(vk.device, vk.depth_inf.imgview, NULL);
		vkDestroyImage(vk.device, vk.depth_inf.image, NULL);
		vkFreeMemory(vk.device, vk.depth_inf.devmem, NULL);

		for (i = 0; i < vk.swapchainImageCount; i++) 
		{
			vkDestroyImageView(vk.device, vk.swapchain_image_resources[i].imgview, NULL);
			vkFreeCommandBuffers(vk.device, vk.cmd_pool, 1, &vk.swapchain_image_resources[i].cmdbuf);
			vkDestroyBuffer(vk.device, vk.swapchain_image_resources[i].uniform_buffer, NULL);
			vkFreeMemory(vk.device, vk.swapchain_image_resources[i].uniform_memory, NULL);
		}
		free(vk.swapchain_image_resources);
		free(vk.queue_props);
		vkDestroyCommandPool(vk.device, vk.cmd_pool, NULL);

		if (vk.flg_separate_present_queue) 
		{
			vkDestroyCommandPool(vk.device, vk.present_cmd_pool, NULL);
		}
		vkDeviceWaitIdle(vk.device);
		vkDestroyDevice(vk.device, NULL);

		vkDestroySurfaceKHR(vk.inst, vk.surface, NULL);


		vkDestroyInstance(vk.inst, NULL);
	}
}
	WindowInf win;

	bool vk_flgActive = false;

class VkInf
{
	VulkanInf vk;
public:

	//-----------------------------------------------------------------------------
	void v_init( HINSTANCE hInstance, HWND hWin, int _width, int _height )
	//-----------------------------------------------------------------------------
	{
		if ( vk_flgActive == false )
		{

			vk_init( vk );


			vk_setup( vk, hInstance, hWin, _width, _height );

			//---------------------------------------------------------
			// 透視変換行列の作成
			//---------------------------------------------------------
			{
				apr_spin_angle = 4.0f;
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

			//-----------------------------------------------------
			// モデルデータの作成
			//-----------------------------------------------------
			struct vktexcube_vs_uniform dataVert;
			{
				mat4x4 VP;
				mat4x4_mul(VP, apr_projection_matrix, apr_view_matrix);

				mat4x4 MVP;
				mat4x4_mul(MVP, VP, apr_model_matrix);

				memcpy(dataVert.mvp, MVP, sizeof(MVP));
				//	dumpMatrix("MVP", MVP);

				for (unsigned int i = 0; i < 12 * 3; i++) 
				{
					dataVert.position[i][0] = g_vertex_buffer_data[i * 3];
					dataVert.position[i][1] = g_vertex_buffer_data[i * 3 + 1];
					dataVert.position[i][2] = g_vertex_buffer_data[i * 3 + 2];
					dataVert.position[i][3] = 1.0f;
					dataVert.attr[i][0] = g_uv_buffer_data[2 * i];
					dataVert.attr[i][1] = g_uv_buffer_data[2 * i + 1];
					dataVert.attr[i][2] = 0;
					dataVert.attr[i][3] = 0;
				}

				vk_setVert( vk, (void*)&dataVert, sizeof(dataVert) );
			}

			//-----------------------------------------------------
			// パイプライン作成
			//-----------------------------------------------------
			{
				uint32_t _vertexCount	= 12*3;
				uint32_t _instanceCount	= 1;
				uint32_t _firstVertex	= 0;
				uint32_t _firstInstance = 0;

				vk_setPipeline( vk, _width, _height, _vertexCount, _instanceCount, _firstVertex, _firstInstance );
			}

			//-----------------------------------------------------
			// 終了待ち
			//-----------------------------------------------------
			vk_endSetup( vk );

			vk_flgActive = true;
			printf("initialized\n");
		}
	}
	
	//-----------------------------------------------------------------------------
	void v_release()
	//-----------------------------------------------------------------------------
	{
		if ( vk_flgActive == true )
		{
			vk_end( vk );
			vk_flgActive = false;
			printf("released\n");
		}
	}

	//-----------------------------------------------------------------------------
	void v_draw()
	//-----------------------------------------------------------------------------
	{
		if ( vk_flgActive == true ) vk_draw( vk );
	}

};

VkInf vkInf;
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
//-----------------------------------------------------------------------------
{
	win_init( win , "msb", 256, 256 );

	//-----------------------------------------------------
	// メインループ
	//-----------------------------------------------------
	MSG msg;   // message
	msg.wParam = 0;
	bool done; // flag saying when app is complete
	done = false; // initialize loop condition variable

	key_init(argc,argv);

	while (!done) 
	{
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (msg.message == WM_QUIT) // check for a flgQuit message
		
		{
			done = true; // if found, flgQuit app
		} else 
		{
			/* Translate and dispatch to event queue*/
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		if ( key.hi._1 )
		{
			vkInf.v_init( win.hInstance, win.hWin, win.win_width, win.win_height );
		}
		
		if ( key.hi._2 )
		{
			vkInf.v_release();
		}
		
		//-----------------------------------------------------
		// 描画
		//-----------------------------------------------------
		vkInf.v_draw();

		key_update();

		RedrawWindow(win.hWin, NULL, NULL, RDW_INTERNALPAINT);
	}

	//-----------------------------------------------------
	// 終了
	//-----------------------------------------------------

	return (int)msg.wParam;
}
//#endif
