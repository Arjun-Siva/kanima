#pragma once

#include <cmath>
#include <iostream>

namespace krt
{
class vec3 {
public:
    float x, y, z;

    vec3();
    vec3(float x, float y, float z);

    vec3 operator+(const vec3& v) const;
    vec3 operator-(const vec3& v) const;
    vec3 operator*(float t) const;
    friend vec3 operator*(float t, const vec3& v);
    vec3 operator/(float t) const;
    friend std::ostream& operator<<(std::ostream& os, const vec3& v);

    float dot(const vec3& v) const;
    vec3 cross(const vec3& v) const;
    vec3 normalized() const;
    float length() const;
};
}
