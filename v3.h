#pragma once

typedef struct {
    float x, y, z;
} v3;

inline v3 v3_add(const v3 u, const v3 v) {
    return (v3){ u.x + v.x, u.y + v.y, u.z + v.z };
}

inline v3 v3_sub(const v3 u, const v3 v) {
    return (v3){ u.x - v.x, u.y - v.y, u.z - v.z };
}

inline v3 v3_mul(const v3 u, const v3 v) {
    return (v3){ u.x * v.x, u.y * v.y, u.z * v.z };
}

inline float v3_dot(const v3 u, const v3 v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline v3 v3_cross(const v3 u, const v3 v) {
    return (v3) { u.y * v.z - u.z * v.y,
                  u.z * v.x - u.x * v.z,
                  u.x * v.y - u.y * v.x };
}
