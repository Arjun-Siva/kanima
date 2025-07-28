#ifndef BITMAPTEXTURE_H
#define BITMAPTEXTURE_H

#include <kanima/stb_image/stb_image.h>
#include <kanima/texture/texture.h>

#include <unistd.h>
#include <iostream>

namespace krt
{

class BitmapTexture : public Texture {
private:
    int width, height, channels;
    unsigned char* data;

public:
    BitmapTexture(const std::string& name, const std::string& filename)
        : Texture(name, "bitmap") {

        std::string fixedPath = filename;

        if (!filename.empty() && filename[0] == '/')
        {
            fixedPath = filename.substr(1); // removes leading '/'
        }

        data = stbi_load(fixedPath.c_str(), &width, &height, &channels, 3); // force 3 channels (RGB)
        if (!data)
        {
            std::cerr << "Failed to load texture image: " << filename << std::endl;
            char buffer[1024];
            if (getcwd(buffer, sizeof(buffer)) != NULL)
                std::cout << "CWD: " << buffer << std::endl;
            else
                perror("getcwd() error");

            width = height = channels = 0;
        }
    }

    ~BitmapTexture()
    {
        if (data)
        {
            stbi_image_free(data);
        }
    }

    Color getTextureAlbedo(float u, float v, const BaryCoord& point) const override {
        // Convert to pixel coords
        int x = static_cast<int>(u * width);
        int y = static_cast<int>((1.0f - v) * height); // flip v since images are top-down

        // Clamp to image bounds
        x = std::min(std::max(x, 0), width - 1);
        y = std::min(std::max(y, 0), height - 1);

        int index = (y * width + x) * channels;

        float r = data[index] / 255.0f;
        float g = data[index + 1] / 255.0f;
        float b = data[index + 2] / 255.0f;

        return Color(r, g, b);
    }
};

}
#endif // BITMAPTEXTURE_H
