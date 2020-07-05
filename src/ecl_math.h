#pragma once

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

#define U8_MAX UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX
#define I8_MAX INT8_MAX
#define I16_MAX INT16_MAX
#define I32_MAX INT32_MAX
#define I64_MAX INT64_MAX
#define F32_MAX FLT_MAX
#define F64_MAX DBL_MAX

#define U8_MIN UINT8_MIN
#define U16_MIN UINT16_MIN
#define U32_MIN UINT32_MIN
#define U64_MIN UINT64_MIN
#define I8_MIN INT8_MIN
#define I16_MIN INT16_MIN
#define I32_MIN INT32_MIN
#define I64_MIN INT64_MIN
#define F32_MIN FLT_MIN
#define F64_MIN DBL_MIN

const f32 pi = 3.1415926f;

static u32 xorshift32(u32 *state)
{
    //static u32 state = 1;
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    u32 x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static f32 randf01(u32 *state)
{
    // the top 23-bits of xorshift have better quality randomness; use those as our exponent
    // or-mask is to get us in the 2^0 mantissa range
    u32 randu = (xorshift32(state) >> 9) | 0x3f800000;
    f32 randf;
    memcpy(&randf, &randu, sizeof(randu)); // type pun
    randf -= 1.0f;
    return randf;
}

static f32 randf_range(u32 *state, f32 min, f32 max)
{
    return min + (max - min) * randf01(state);
}

/* Vectors */
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

static inline v3 v3_add(v3 a, v3 b)
{
    return (v3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline v3 v3_addf(v3 a, float f)
{
    return (v3){a.x + f, a.y + f, a.z + f};
}

static inline v3 v3_sub(v3 a, v3 b)
{
    return (v3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline v3 v3_subf(v3 a, float f)
{
    return (v3){a.x - f, a.y - f, a.z - f};
}

// hadamard
static inline v3 v3_mul(v3 a, v3 b)
{
    return (v3){a.x * b.x, a.y * b.y, a.z * b.z};
}

static inline v3 v3_mulf(v3 a, float f)
{
    return (v3){a.x * f, a.y * f, a.z * f};
}

static inline float v3_dot(v3 a, v3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline v3 v3_cross(v3 a, v3 b)
{
    return (v3){a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x};
}

static inline v3 v3_reflect(v3 v, v3 n)
{
    return v3_sub(v, v3_mulf(v3_mulf(n, v3_dot(v, n)), 2));
}

static inline float v3_length(v3 a)
{
    return sqrtf(v3_dot(a, a));
}

// note: doesn't handle zero vecs
static inline v3 v3_normalize(v3 a)
{
    return v3_mulf(a, 1.0f / v3_length(a));
}
