#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>

#include <kanima/linalg/vec3.h>
#include <kanima/core/color.h>
#include <kanima/texture/texture.h>

namespace krt
{

enum class MaterialType
{
    Diffuse,
    Reflective,
    Refractive,
    Constant
};

class Material
{
public:
    std::shared_ptr<Texture> albedoTex;
    MaterialType type;
    bool smoothShading;
    float ior;

    Material();
    Material(std::shared_ptr<Texture> texture, MaterialType type, bool doSmoothShading);
    Material(std::shared_ptr<Texture> texture, MaterialType type, bool doSmoothShading, float ior);
};

}
#endif // MATERIAL_H
