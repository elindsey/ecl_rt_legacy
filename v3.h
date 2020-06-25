#pragma once

#include "ecl.h"

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

static inline v3 v3_add(v3 a, v3 b) {
    return (v3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline v3 v3_addf(v3 a, float f) {
    return (v3){a.x + f, a.y + f, a.z + f};
}

static inline v3 v3_sub(v3 a, v3 b) {
    return (v3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline v3 v3_subf(v3 a, float f) {
    return (v3){a.x - f, a.y - f, a.z - f};
}

static inline v3 v3_mul(v3 a, v3 b) {
    return (v3){a.x * b.x, a.y * b.y, a.z * b.z};
}

static inline v3 v3_mulf(v3 a, float f) {
    return (v3){a.x * f, a.y * f, a.z * f};
}

static inline float v3_dot(v3 a, v3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline v3 v3_cross(v3 a, v3 b) {
    return (v3){a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x};
}

static inline float v3_length(v3 a) {
    return ecl_sqrtish(v3_dot(a, a));
}

// TODO handle zero vec
static inline v3 v3_normalize(v3 a) {
    return v3_mulf(a, 1.0f / v3_length(a));
}
