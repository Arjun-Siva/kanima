#ifndef AABB_H
#define AABB_H

#include <kanima/linalg/vec3.h>
#include <kanima/core/ray.h>
#include <limits>

const double EPSILON = 1e-6;

namespace krt
{
class AABB
{
private:
    vec3 min_vertex;
    vec3 max_vertex;

public:
    AABB() : min_vertex(vec3(0.f,0.f,0.f)), max_vertex(vec3(0.f,0.f,0.f)) {}
    AABB(vec3& minv, vec3& maxv)
    {
        // padding for flat surfaces
        if (std::abs(minv.x - maxv.x) < EPSILON)
        {
            minv.x -= 1e-4;
            maxv.x += 1e-4;
        }

        if (std::abs(minv.y - maxv.y) < EPSILON)
        {
            minv.y -= 1e-4;
            maxv.y += 1e-4;
        }

        if (std::abs(minv.z - maxv.z) < EPSILON)
        {
            minv.z -= 1e-4;
            maxv.z += 1e-4;
        }

        this->min_vertex = minv;
        this->max_vertex = maxv;

    }


    bool rayIntersectBox(const Ray& ray) const
    {
        float tMin = -std::numeric_limits<float>::infinity();
        float tMax = std::numeric_limits<float>::infinity();

        // X-axis
        if (std::abs(ray.d.x) < EPSILON)
        {
            if (ray.o.x < min_vertex.x || ray.o.x > max_vertex.x)
                return false;
        }
        else
        {
            float invD = 1.0f / ray.d.x;
            float t0 = (min_vertex.x - ray.o.x) * invD;
            float t1 = (max_vertex.x - ray.o.x) * invD;

            if (invD < 0.0f)
                std::swap(t0, t1);

            tMin = std::max(t0, tMin);
            tMax = std::min(t1, tMax);

            if (tMin > tMax)
                return false;
        }

        // Y-axis
        if (std::abs(ray.d.y) < EPSILON)
        {
            if (ray.o.y < min_vertex.y || ray.o.y > max_vertex.y)
                return false;
        }
        else
        {
            float invD = 1.0f / ray.d.y;
            float t0 = (min_vertex.y - ray.o.y) * invD;
            float t1 = (max_vertex.y - ray.o.y) * invD;

            if (invD < 0.0f)
                std::swap(t0, t1);

            tMin = std::max(t0, tMin);
            tMax = std::min(t1, tMax);

            if (tMin > tMax)
                return false;
        }

        // Z-axis
        if (std::abs(ray.d.z) < EPSILON)
        {
            if (ray.o.z < min_vertex.z || ray.o.z > max_vertex.z)
                return false;
        }
        else
        {
            float invD = 1.0f / ray.d.z;
            float t0 = (min_vertex.z - ray.o.z) * invD;
            float t1 = (max_vertex.z - ray.o.z) * invD;

            if (invD < 0.0f)
                std::swap(t0, t1);

            tMin = std::max(t0, tMin);
            tMax = std::min(t1, tMax);

            if (tMin > tMax)
                return false;
        }

        // Final check - accepts negative tMin (internal rays)
        return tMax > 0.0f;
    }

};
}
#endif // AABB_H
