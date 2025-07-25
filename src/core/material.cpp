#include <kanima/core/material.h>

namespace krt
{
Material::Material()
    : albedoTex(nullptr), type(MaterialType::Diffuse), smoothShading(false), ior(1) {}

Material::Material(std::shared_ptr<Texture> texture, MaterialType type, bool doSmoothShading)
    : albedoTex(texture), type(type),  smoothShading(doSmoothShading), ior(1) {}

Material::Material(std::shared_ptr<Texture> texture, MaterialType type, bool doSmoothShading, float ior)
    : albedoTex(texture), type(type),  smoothShading(doSmoothShading), ior(ior) {}
}
