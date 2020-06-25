#pragma once

#include <stdint.h>
#include <float.h>
#include <immintrin.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef float f32;

#define U8_MAX UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX
#define S8_MAX INT8_MAX
#define S16_MAX INT16_MAX
#define S32_MAX INT32_MAX
#define S64_MAX INT64_MAX
#define F32_MAX FLT_MAX

#define U8_MIN UINT8_MIN
#define U16_MIN UINT16_MIN
#define U32_MIN UINT32_MIN
#define U64_MIN UINT64_MIN
#define S8_MIN INT8_MIN
#define S16_MIN INT16_MIN
#define S32_MIN INT32_MIN
#define S64_MIN INT64_MIN
#define F32_MIN FLT_MIN

u32 ecl_rand_r() {
    // https://www.pcg-random.org

}

inline f32 ecl_sqrt(f32 f) {
    f32 out;
    _mm_store_ss(&out, _mm_sqrt_ss(_mm_load_ss(&f)));
    return out;
}

// fast reciprocal sqrt; approximate
inline f32 ecl_sqrtish(f32 f) {
    f32 out;
    __m128 in = _mm_load_ss(&f);
    _mm_store_ss(&out, _mm_mul_ss(in, _mm_rsqrt_ss(in)));
    return out;
}
