#ifndef BARYCOORD_H
#define BARYCOORD_H

#include <kanima/linalg/vec3.h>

namespace krt
{
class BaryCoord
{
public:
    float u;
    float v;
    float w;

    BaryCoord() : u(0), v(0), w(0) {}

    BaryCoord(float u, float v, float w) : u(u), v(v), w(w) {}

    BaryCoord(vec3 bv)
    {
        u = bv.x;
        v = bv.y;
        w = bv.z;
    }

};
}
#endif // BARYCOORD_H
