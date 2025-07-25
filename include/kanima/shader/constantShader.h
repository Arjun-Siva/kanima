#ifndef CONSTANTSHADER_H
#define CONSTANTSHADER_H

#include <kanima/core/intersectionData.h>
#include <kanima/core/scene.h>
#include <kanima/core/color.h>

namespace krt
{
Color constantShader(IntersectionData& intersectData, Scene& scene);
}

#endif // CONSTANTSHADER_H
