#ifndef REFLECTIVESHADER_H
#define REFLECTIVESHADER_H

#include <kanima/core/color.h>
#include <kanima/core/scene.h>

namespace krt
{
Color reflectiveShader(const Ray& ray, IntersectionData& intersectData, Scene& scene, int max_depth);
}

#endif // REFLECTIVESHADER_H
