#include <cmath>
#include <kanima/shader/refractiveShader.h>
#include <kanima/shader/recursiveShader.h>

const double REFRACTION_BIAS = 1e-3;
const double REFLECTION_BIAS = 1e-3;

namespace krt
{
Color refractiveShader(const Ray& ray, IntersectionData& intersectData, Scene& scene, int max_depth)
{
    vec3 normal = (intersectData.material->smoothShading) ? intersectData.interpolatedVertNormal : intersectData.hitPointNormal;

    // n1 is assumed to be air/vacuum and n2 is the object
    float n1 = 1.f;
    float n2 = intersectData.material->ior;


    float dotIN = ray.d.dot(normal);
    bool enteringObject = true;

    // incident rays leaves the object (normal and ray are in the same direction)
    if (dotIN > -EPSILON)
    {
        normal = -1*normal;
        dotIN *= -1;
        std::swap(n1, n2);
        enteringObject = false;
    }

    // alpha - angle of incidence, beta - angle of refraction

    float cosAlpha = -1 * ray.d.dot(normal);
    float sinAlpha = sqrt(1 - pow(cosAlpha, 2));

    // angle of incidence < critical angle
    if (sinAlpha < n2/n1)
    {
        float sinBeta = sinAlpha * n1 / n2;
        float cosBeta = sqrt(1 - pow(sinBeta, 2));
        vec3 C = (ray.d + (cosAlpha * normal)).normalized();
        vec3 B = C * sinBeta;
        vec3 A = cosBeta * -1 * normal;
        vec3 R = A + B;

        Ray refractionRay(intersectData.hitPoint + (-1* normal * REFRACTION_BIAS), R, RayType::refraction, ray.pathDepth + 1);
        Color refractionColor = recursiveShader(refractionRay, scene, max_depth);

        Ray reflectionRay = ray.reflectedRay(normal, intersectData.hitPoint + (normal * REFLECTION_BIAS));
        Color reflectionColor = recursiveShader(reflectionRay, scene, max_depth);

        float fresnel = 0.5 * std::pow(1.0 + dotIN, 5.0f);
        return (fresnel * reflectionColor) + ((1.0 - fresnel) * refractionColor);
    }

    Ray reflectionRay = ray.reflectedRay(normal, intersectData.hitPoint + (normal * REFLECTION_BIAS)); // depth will be incremented

    Color reflectionColor = recursiveShader(reflectionRay, scene, max_depth);
    return reflectionColor;

}
}
