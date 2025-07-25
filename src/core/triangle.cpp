#include <kanima/core/triangle.h>

namespace krt
{

const double EPSILON = 1e-6;

Triangle::Triangle(const vec3 &a, const vec3 &b, const vec3 &c) : v0(a), v1(b), v2(c) {
    computeNormal();
}

Triangle::Triangle(const vec3 &a, const vec3 &b, const vec3 &c, const Color& color) : v0(a), v1(b), v2(c), color(color) {
    computeNormal();
}

// this constructor will be used by the scene object for BVH
Triangle::Triangle(const vec3 &a, const vec3 &b, const vec3 &c, const int meshIdx, const int triangleIdx) : v0(a), v1(b), v2(c), meshIdx(meshIdx), triangleIdx(triangleIdx) {
//    computeNormal(); - not necessary for BVH
    computeCentroid();
}


void Triangle::computeNormal() {
    vec3 edge1 = this->v1 - this->v0;
    vec3 edge2 = this->v2 - this->v0;

    this->normal = (edge1.cross(edge2)).normalized();
}

void Triangle::computeCentroid() {
    this->centroid = (this->v0 + this->v1 + this->v2) / 3;
}

double Triangle::intersect(const Ray &r) const {
    if (normal.dot(r.d) >= -EPSILON) return -1; //parallel, backface culling

    //t = -(normal . (O - v0)) / (normal . D)
    double t = -1 * normal.dot(r.o - v0) / normal.dot(r.d);
    if (t < 0) return -1;

    // intersection point
    vec3 p = r.o + r.d * t;

    vec3 e01 = v1 - v0;
    vec3 e12 = v2 - v1;
    vec3 e20 = v0 - v2;

    if (normal.dot(e01.cross(p-v0)) <= -EPSILON) return -1;
    if (normal.dot(e12.cross(p-v1)) <= -EPSILON) return -1;
    if (normal.dot(e20.cross(p-v2)) <= -EPSILON) return -1;

    return t;
}

void Triangle::setColor(const Color& newColor)
{
    this->color = newColor;
}

vec3 Triangle::getBaryCentricCoords(vec3& point)
{
    vec3 e01 = v1 - v0;
    vec3 e02 = v2 - v0;

    vec3 e0p = point - v0;

    float area_tri = e01.cross(e02).length() / 2.0f;
    float area_m = e0p.cross(e02).length() / 2.0f;
    float area_n = e01.cross(e0p).length() / 2.0f;

    float u = area_m / area_tri;
    float v = area_n / area_tri;
    float w = 1 - u - v;

    return vec3(u, v, w);
}

}
