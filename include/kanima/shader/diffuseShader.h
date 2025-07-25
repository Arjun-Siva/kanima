#ifndef DIFFUSESHADER_H
#define DIFFUSESHADER_H

#include <kanima/core/color.h>
#include <kanima/core/scene.h>
#include <kanima/core/intersectionData.h>

namespace krt
{
Color diffuseShader(IntersectionData& intersectData, Scene& scene);

}

#endif // DIFFUSESHADER_H
