#pragma once

#include <math.h>

typedef struct {
    union {
        struct {
            float x, y, z;
        };
        struct {
            float r, g, b;
        };
    };
} v3;

static inline v3 v3_add(v3 v,  v3 u) {
    return (v3){ v.x + u.x, v.y + u.y, v.z + u.z };
}

static inline v3 v3_addf(v3 v, float f) {
    return (v3){ v.x + f, v.y + f, v.z + f };
}

static inline v3 v3_sub(v3 v,  v3 u) {
    return (v3){ v.x - u.x, v.y - u.y, v.z - u.z };
}

static inline v3 v3_subf(v3 v, float f) {
    return (v3){ v.x - f, v.y - f, v.z - f };
}

static inline v3 v3_mul(v3 v,  v3 u) {
    return (v3){ v.x * u.x, v.y * u.y, v.z * u.z };
}

static inline v3 v3_mulf(v3 v, float f) {
    return (v3){ v.x * f, v.y * f, v.z * f };
}

static inline float v3_dot(v3 v,  v3 u) {
    return v.x * u.x + v.y * u.y + v.z * u.z;
}

static inline v3 v3_cross(v3 v,  v3 u) {
    return (v3) { v.y * u.z - v.z * u.y,
                  v.z * u.x - v.x * u.z,
                  v.x * u.y - v.y * u.x };
}

static inline float v3_length(v3 v) {
    return sqrtf(v3_dot(v, v));
}

// TODO handle zero vec
static inline v3 v3_normalize(v3 v) {
    return v3_mulf(v, 1.0f / v3_length(v));
}
