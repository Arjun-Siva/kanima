#ifndef INTERSECTIONDATA_H
#define INTERSECTIONDATA_H

#include <kanima/linalg/vec3.h>
#include <kanima/core/material.h>
#include <kanima/core/baryCoord.h>

namespace krt
{
struct IntersectionData
{
    vec3 hitPoint;
    vec3 hitPointNormal; //triangle face normal
    vec3 interpolatedVertNormal; // interpolated normal from barycentric coords
    BaryCoord baryCentricCoords;
    const Material* material;
    int objectIdx = -1;
    int triangleIdx = -1;
};
}

#endif // INTERSECTIONDATA_H
