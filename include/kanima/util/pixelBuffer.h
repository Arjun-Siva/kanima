#ifndef PIXELBUFFER_H
#define PIXELBUFFER_H

#include <kanima/core/color.h>
#include <kanima/util/clamp.h>

#include <vector>
#include <cassert>
#include <string>
#include <fstream>

namespace krt
{
class PixelBuffer
{
private:
    int width;
    int height;
    std::vector<Color> buffer;

public:
    PixelBuffer(int w, int h) : width(w), height(h), buffer(w * h) {}

    void setColor(int x, int y, const Color& color)
    {
        assert (x >= 0 && x < width && y >= 0 && y < height);
        buffer[y * width + x] = color;
    }

    Color getColor(int x, int y) const
    {
        assert (x >= 0 && x < width && y >= 0 && y < height);
        return buffer[y * width + x];
    }

    void writeToPPM(const std::string& filename)
    {
        std::ofstream out(filename);
        out << "P3\n" << width << ' ' << height << "\n255\n";

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                const Color& c = getColor(x, y);
                int r = static_cast<int>(clamp(c.r, 0.0f, 1.0f) * 255.0f);
                int g = static_cast<int>(clamp(c.g, 0.0f, 1.0f) * 255.0f);
                int b = static_cast<int>(clamp(c.b, 0.0f, 1.0f) * 255.0f);
                out << r << " " << g << " " << b << "\n";
            }
        }
        out.close();
    }

    int getWidth () const
    {
        return width;
    }

    int getHeight() const
    {
        return height;
    }

};

}
#endif // PIXELBUFFER_H
