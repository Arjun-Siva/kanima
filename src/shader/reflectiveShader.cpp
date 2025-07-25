#include <kanima/shader/reflectiveShader.h>
#include <kanima/shader/recursiveShader.h>

namespace krt
{

const double REFLECTION_BIAS = 1e-3;

Color reflectiveShader(const Ray& ray, IntersectionData& intersectData, Scene& scene, int max_depth)
{
    const Material hitMaterial = *intersectData.material;
    Color albedo = scene.geometryObjects[intersectData.objectIdx].getAlbedo(intersectData.baryCentricCoords, intersectData.triangleIdx);
    const float albedoR = albedo.r;
    const float albedoG = albedo.g;
    const float albedoB = albedo.b;

    const vec3& normal = (hitMaterial.smoothShading) ? intersectData.interpolatedVertNormal : intersectData.hitPointNormal;
    const Ray reflectedR = ray.reflectedRay(normal, intersectData.hitPoint + (normal * REFLECTION_BIAS));

    const Color reflectedColor = recursiveShader(reflectedR, scene, max_depth);

    return Color(reflectedColor.r * albedoR,  reflectedColor.g * albedoG, reflectedColor.b * albedoB);
}

}
