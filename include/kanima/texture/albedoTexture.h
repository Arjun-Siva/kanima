#ifndef ALBEDOTEXTURE_H
#define ALBEDOTEXTURE_H

#include <kanima/texture/texture.h>

namespace krt
{
class AlbedoTexture : public Texture {
private:
    Color albedo;
public:
    AlbedoTexture(const std::string& name, const Color& color)
        : Texture(name, "albedo"), albedo(color) {}

    Color getTextureAlbedo(float u, float v, const BaryCoord& point) const override {
        // return solid color
        return albedo;
    }
};

}
#endif // ALBEDOTEXTURE_H
