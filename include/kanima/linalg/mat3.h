#ifndef MAT3_H
#define MAT3_H

#include <kanima/linalg/vec3.h>

namespace krt
{
class mat3
{
public:
    vec3 rows[3];

    mat3(float i);
    mat3(const vec3& r0, const vec3& r1, const vec3& r2);

    mat3 transpose() const;
    mat3 operator*(const mat3& other) const;
    vec3 operator*(const vec3& v) const;

};
}

#endif // MAT3_H
