#pragma once

#include <math.h>

typedef struct {
    float x, y, z;
} v3;

static inline v3 v3_add(const v3 v, const v3 u) {
    return (v3){ v.x + u.x, v.y + u.y, v.z + u.z };
}

static inline v3 v3_addf(const v3 v, float f) {
    return (v3){ v.x + f, v.y + f, v.z + f };
}

static inline v3 v3_sub(const v3 v, const v3 u) {
    return (v3){ v.x - u.x, v.y - u.y, v.z - u.z };
}

static inline v3 v3_subf(const v3 v, float f) {
    return (v3){ v.x - f, v.y - f, v.z - f };
}

static inline v3 v3_mul(const v3 v, const v3 u) {
    return (v3){ v.x * u.x, v.y * u.y, v.z * u.z };
}

static inline v3 v3_mulf(const v3 v, float f) {
    return (v3){ v.x * f, v.y * f, v.z * f };
}

static inline float v3_dot(const v3 v, const v3 u) {
    return v.x * u.x + v.y * u.y + v.z * u.z;
}

static inline v3 v3_cross(const v3 v, const v3 u) {
    return (v3) { v.y * u.z - v.z * u.y,
                  v.z * u.x - v.x * u.z,
                  v.x * u.y - v.y * u.x };
}

static inline float v3_length(const v3 v) {
    return sqrtf(v3_dot(v, v));
}

static inline v3 v3_normalize(const v3 v) {
    return v3_mulf(v, 1.0f / v3_length(v));
}
