#ifndef DIFFUSESHADER_H
#define DIFFUSESHADER_H

#include <kanima/core/color.h>
#include <kanima/core/scene.h>
#include <kanima/core/intersectionData.h>

namespace krt
{
Color diffuseShader(const Ray& ray, IntersectionData& intersectData, Scene& scene, int max_depth);

}

#endif // DIFFUSESHADER_H
