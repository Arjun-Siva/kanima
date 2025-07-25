#include <kanima/linalg/vec3.h>

// methods don't modify the object itself
namespace krt
{
vec3::vec3() : x(0), y(0), z(0) {}
vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}

vec3 vec3::operator+(const vec3& v) const {
    return vec3(x + v.x, y + v.y, z + v.z);
}

vec3 vec3::operator-(const vec3& v) const {
    return vec3(x - v.x, y - v.y, z - v.z);
}

vec3 vec3::operator*(float t) const {
    return vec3(x * t, y * t, z * t);
}

vec3 vec3::operator/(float t) const {
    return vec3(x / t, y / t, z / t);
}

vec3 operator*(float t, const vec3& v) {
    return vec3(v.x * t, v.y * t, v.z * t);
}

std::ostream& operator<<(std::ostream& os, const vec3& v)  {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

float vec3::dot(const vec3& v) const {
    return x * v.x + y * v.y + z * v.z;
}

vec3 vec3::cross(const vec3& v) const {
    return vec3(
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x
    );
}

float vec3::length() const {
    return std::sqrt(dot(*this));
}

vec3 vec3::normalized() const {
    return *this / length();
}
}
