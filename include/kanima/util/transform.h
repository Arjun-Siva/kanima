#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <cmath>
#include <kanima/linalg/mat3.h>

namespace krt
{

inline float deg2rad(float degrees) {
    return degrees * (M_PI / 180.0);
}

inline mat3 rotateX(float deg) {
    float rad = deg2rad(deg);
    float c = std::cos(rad);
    float s = std::sin(rad);
    return mat3(
        vec3(1, 0, 0),
        vec3(0, c, -s),
        vec3(0, s,  c)
    );
}

inline mat3 rotateY(float deg) {
    float rad = deg2rad(deg);
    float c = std::cos(rad);
    float s = std::sin(rad);
    return mat3(
        vec3( c, 0, s),
        vec3( 0, 1, 0),
        vec3(-s, 0, c)
    );
}

inline mat3 rotateZ(float deg) {
    float rad = deg2rad(deg);
    float c = std::cos(rad);
    float s = std::sin(rad);
    return mat3(
        vec3(c, -s, 0),
        vec3(s,  c, 0),
        vec3(0,  0, 1)
    );
}
}
#endif // TRANSFORM_H
