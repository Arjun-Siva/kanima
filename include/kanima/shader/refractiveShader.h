#ifndef REFRACTIVESHADER_H
#define REFRACTIVESHADER_H

#include <kanima/core/color.h>
#include <kanima/core/scene.h>

namespace krt
{

Color refractiveShader(const Ray& ray, IntersectionData& intersectData, Scene& scene, int max_depth);

}
#endif // REFRACTIVESHADER_H
