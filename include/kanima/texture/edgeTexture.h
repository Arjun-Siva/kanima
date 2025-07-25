#ifndef EDGETEXTURE_H
#define EDGETEXTURE_H

#include <kanima/texture/texture.h>

namespace krt
{

class EdgeTexture : public Texture {
private:
    Color inner_color;
    Color edge_color;
    float edge_width;
public:
    EdgeTexture(const std::string& name, const Color& inner_color, const Color& edge_color, float edge_width)
        : Texture(name, "edges"), inner_color(inner_color), edge_color(edge_color), edge_width(edge_width) {}

    Color getTextureAlbedo(float u, float v, const BaryCoord& point) const override {
        if (point.u < edge_width || point.v < edge_width || point.w < edge_width)
            return edge_color;
        else
            return inner_color;
    }
};

}
#endif // EDGETEXTURE_H
