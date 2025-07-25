#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <kanima/linalg/vec3.h>
#include <kanima/core/baryCoord.h>
#include <kanima/core/color.h>

namespace krt
{
class Texture {
public:
    std::string name;
    std::string type;

    Texture(const std::string& name, const std::string& type)
        : name(name), type(type) {}

    virtual ~Texture() = default;

    // u and v are interpolated texture coordinates. point is the barycentric coordinates of intersection point
    virtual Color getTextureAlbedo(float u, float v, const BaryCoord& point) const = 0;
};

}
#endif // TEXTURE_H
