#ifndef RAY_H
#define RAY_H

#include "../linalg/vec3.h"

namespace krt
{
enum class RayType
{
    invalid,
    camera,
    shadow,
    reflection,
    refraction
};

class Ray
{
public:
    vec3 o; // origin
    vec3 d; // direction
    RayType type;
    int pathDepth;

    Ray(const vec3& o, const vec3& d) : o(o), d(d), type(RayType::camera), pathDepth(1) {};

    Ray(const vec3& o, const vec3& d, RayType type, int pathDepth) : o(o), d(d), type(type), pathDepth(pathDepth) {};

    Ray reflectedRay(const vec3& normal, const vec3& point) const
    {
        vec3 newD = this->d - (normal * (this->d.dot(normal)) * 2.f);
        return Ray(point, newD.normalized(), this->type, this->pathDepth + 1);
    }
};

}
#endif // RAY_H
