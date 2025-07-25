#include <kanima/linalg/mat3.h>
#include <kanima/linalg/vec3.h>

namespace krt
{
// identity matrix
mat3::mat3(float i) : rows{vec3(i,0,0), vec3(0,i,0), vec3(0,0,i)} {} // identity

mat3::mat3(const vec3& r0, const vec3& r1, const vec3& r2) : rows{r0, r1, r2} {}

mat3 mat3::transpose() const
{
   return mat3(
       vec3(rows[0].x, rows[1].x, rows[2].x),
       vec3(rows[0].y, rows[1].y, rows[2].y),
       vec3(rows[0].z, rows[1].z, rows[2].z)
   );
}

mat3 mat3::operator*(const mat3& other) const
{
    mat3 t = other.transpose();
    return mat3(
        vec3(rows[0].dot(t.rows[0]), rows[0].dot(t.rows[1]), rows[0].dot(t.rows[2])),
        vec3(rows[1].dot(t.rows[0]), rows[1].dot(t.rows[1]), rows[1].dot(t.rows[2])),
        vec3(rows[2].dot(t.rows[0]), rows[2].dot(t.rows[1]), rows[2].dot(t.rows[2]))
    );
}

vec3 mat3::operator*(const vec3& v) const
{
    return vec3(
        rows[0].dot(v),
        rows[1].dot(v),
        rows[2].dot(v)
    );
}

}
