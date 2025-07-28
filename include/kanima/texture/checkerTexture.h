#ifndef CHECKERTEXTURE_H
#define CHECKERTEXTURE_H

#include <kanima/texture/texture.h>

namespace krt
{

class CheckerTexture : public Texture {
private:
    Color color_a;
    Color color_b;
    float square_size;
public:
    CheckerTexture(const std::string& name, const Color& color_a, const Color& color_b, float square_size)
        : Texture(name, "checker"), color_a(color_a), color_b(color_b), square_size(square_size) {}

    Color getTextureAlbedo(float u, float v, const BaryCoord& point) const override {
        int scaled_u = std::floor(u / square_size);
        int scaled_v = std::floor(v / square_size);

        if ((scaled_u + scaled_v)%2 == 0)
            return color_a;
        else
            return color_b;
    }
};

}
#endif // CHECKERTEXTURE_H
