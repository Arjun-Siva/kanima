#ifndef RECURSIVESHADER_H
#define RECURSIVESHADER_H

#include <kanima/core/ray.h>
#include <kanima/core/color.h>
#include <kanima/core/scene.h>
#include <kanima/shader/constantShader.h>
#include <kanima/shader/diffuseShader.h>
#include <kanima/shader/reflectiveShader.h>
#include <kanima/shader/refractiveShader.h>

namespace krt
{
Color recursiveShader(const Ray &ray, Scene& scene, int max_depth);
}

#endif // RECURSIVESHADER_H
