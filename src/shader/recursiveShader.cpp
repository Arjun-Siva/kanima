#include <cassert>
#include <kanima/shader/recursiveShader.h>


namespace krt
{
Color recursiveShader(const Ray &ray, Scene& scene, int max_depth)
{
    Color pixelColor = scene.bgColor;

    if (ray.pathDepth > max_depth)
    {
        return pixelColor;
    }

    IntersectionData iData;

    if (scene.useBVH)
        iData = scene.traceRayBVH(ray);
    else
        iData = scene.traceRay(ray);

    // no hit
    if(iData.triangleIdx == -1)
    {
        return pixelColor;
    }

    assert(iData.material != nullptr);
    const Material& hitMaterial = *iData.material;

    if (hitMaterial.type == MaterialType::Diffuse)
    {
        return diffuseShader(iData, scene);
    }

    else if (hitMaterial.type == MaterialType::Reflective)
    {
        return reflectiveShader(ray, iData, scene, max_depth);
    }

    else if (hitMaterial.type == MaterialType::Refractive)
    {
        return refractiveShader(ray, iData, scene, max_depth);
    }

    else if (hitMaterial.type == MaterialType::Constant)
    {
        return constantShader(iData, scene);
    }

    assert(false&&"Invalid material");
    return Color(0, 0, 0);

}
}
