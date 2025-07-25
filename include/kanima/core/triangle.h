#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <kanima/core/color.h>
#include <kanima/linalg/vec3.h>
#include <kanima/core/ray.h>

#include <utility>

namespace krt
{
class Triangle
{
public:
    vec3 v0, v1, v2;
    vec3 normal;
    Color color;
    vec3 centroid;
    int meshIdx; // the mesh/object the triangle belongs to
    int triangleIdx; // the index of the triangle in the scene object

    Triangle(const vec3 &a,const vec3 &b, const vec3 &c);
    Triangle(const vec3 &a,const vec3 &b, const vec3 &c, const Color &color);
    Triangle(const vec3 &a,const vec3 &b, const vec3 &c, const int meshIdx, const int triangleIdx);
    double intersect(const Ray& r) const;
    void setColor(const Color& newColor);
    vec3 getBaryCentricCoords(vec3& point);


private:
    void computeNormal();
    void computeCentroid();

};

}
#endif // TRIANGLE_H
