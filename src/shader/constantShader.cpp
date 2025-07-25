#include <kanima/shader/constantShader.h>

namespace krt
{

Color constantShader(IntersectionData& intersectData, Scene& scene)
{
    const Material hitMaterial = *intersectData.material;

    const Color meshColor = scene.geometryObjects[intersectData.objectIdx].uniformColor;

    Color albedo = scene.geometryObjects[intersectData.objectIdx].getAlbedo(intersectData.baryCentricCoords, intersectData.triangleIdx);
    const float albedoR = albedo.r;
    const float albedoG = albedo.g;
    const float albedoB = albedo.b;

    Color pixelColor = Color(meshColor.r * albedoR , meshColor.g * albedoG, meshColor.b * albedoB);
    return pixelColor;
}

}
