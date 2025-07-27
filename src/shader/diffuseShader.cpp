#include <kanima/shader/diffuseShader.h>
#include <kanima/shader/recursiveShader.h>

namespace krt
{
const double SHADOW_BIAS = 1e-3;
const double RAY_HIT_BIAS = 1e-3;

Color diffuseShader(const Ray& ray, IntersectionData& intersectData, Scene& scene, int max_depth)
{
    const Material hitMaterial = *intersectData.material;

    Color albedo = scene.geometryObjects[intersectData.objectIdx].getAlbedo(intersectData.baryCentricCoords, intersectData.triangleIdx);
    const float albedoR = albedo.r;
    const float albedoG = albedo.g;
    const float albedoB = albedo.b;

    Color pixelColor = Color(0, 0, 0);

    // local hit matrix
    vec3 rightAxis = ray.d.cross(intersectData.hitPointNormal).normalized();
    vec3 upAxis = intersectData.hitPointNormal;
    vec3 forwardAxis = rightAxis.cross(upAxis);
    mat3 localHitMatrix = mat3(rightAxis, upAxis, forwardAxis);

    // global illumination
    for (int i = 0; i < scene.gi_ray_count; i++)
    {
        float randomAngleInXY = M_PI * static_cast<float>(rand()) / RAND_MAX;
        vec3 randomVecInXY = vec3(cos(randomAngleInXY), sin(randomAngleInXY), 0);

        float randomAngleInXZ = 2 * M_PI * static_cast<float>(rand()) / RAND_MAX;
        mat3 rotateAroundY = mat3(vec3(cos(randomAngleInXZ), 0, -sin(randomAngleInXZ)),
                                  vec3(0, 1, 0),
                                  vec3(sin(randomAngleInXZ), 0, cos(randomAngleInXZ))
                                  );

        vec3 randVecInXYRotated = rotateAroundY.transpose() * randomVecInXY;

        vec3 diffReflRayDir = (localHitMatrix.transpose() * randVecInXYRotated).normalized();
        vec3 diffReflRayOrg = intersectData.hitPoint + intersectData.hitPointNormal * RAY_HIT_BIAS;

        Ray diffReflRay = Ray(diffReflRayOrg, diffReflRayDir, RayType::reflection, ray.pathDepth + 1);

        pixelColor = pixelColor + recursiveShader(diffReflRay, scene, max_depth);
    }


    // Shadow ray
    vec3 shadowOrigin = intersectData.hitPoint + intersectData.hitPointNormal * SHADOW_BIAS;

    for (const Light& light : scene.lights)
    {
        vec3 shadowDir = (light.getPosition() - shadowOrigin).normalized();

        Ray shadowRay = Ray(shadowOrigin, shadowDir, RayType::shadow, 1);

        vec3 shadowHitPoint;
        vec3 shadowDHitNormal;
        int shadownHitTriangleIndex;
        double distanceToLight = (light.getPosition() - shadowOrigin).length();
        bool shadowReachLight = true;

        if (scene.useBVH)
        {
            IntersectionData t = scene.traceRayBVH(shadowRay);

            if (t.objectIdx != -1)
            {
                shadowHitPoint = t.hitPoint;
                double st = (shadowHitPoint - shadowOrigin).length();
                if (st < distanceToLight && st > EPSILON)
                    shadowReachLight = false;
            }
        }

        else
        {
            for (const Mesh& mesh : scene.geometryObjects)
            {
                double st = mesh.intersectRay(shadowRay, shadownHitTriangleIndex, shadowHitPoint, shadowDHitNormal, false);

                // intersection before reaching light
                if (st < distanceToLight && st > EPSILON)
                {
                    shadowReachLight = false;
                    break;
                }
            }
        }

        // add contribution of each light
        if (shadowReachLight)
        {
            vec3 lightDir = light.getPosition() - intersectData.hitPoint;
            float sphereRadius = lightDir.length();

            float cosLaw = 0;
            lightDir = lightDir.normalized();
            if (hitMaterial.smoothShading)
                cosLaw = std::max(0.0f, lightDir.dot(intersectData.interpolatedVertNormal));
            else
                cosLaw = std::max(0.0f, lightDir.dot(intersectData.hitPointNormal));

            float sphereArea = 4.0f * M_PI * sphereRadius * sphereRadius;

            float rContrib = scene.geometryObjects[intersectData.objectIdx].uniformColor.r * albedoR;
            float gContrib = scene.geometryObjects[intersectData.objectIdx].uniformColor.g * albedoG;
            float bContrib = scene.geometryObjects[intersectData.objectIdx].uniformColor.b * albedoB;

            pixelColor = pixelColor + (Color(rContrib, gContrib, bContrib) * (cosLaw/sphereArea)) * light.getIntensity();
        }

    } // lights loop end

    pixelColor = pixelColor * (1.0f / (scene.gi_ray_count + 1));


    return pixelColor;
}

}
