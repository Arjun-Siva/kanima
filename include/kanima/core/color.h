#ifndef COLOR_H
#define COLOR_H

#include <kanima/util/clamp.h>
#include <cassert>
#include <iostream>

namespace krt
{
class Color
{
public:
    float r;
    float g;
    float b;

    Color();
    Color(float r, float g, float b);

    Color operator*(float scalar) const;
    friend Color operator*(float scalar, const Color& c);
    Color operator+(const Color& other);
};
}
#endif // COLOR_H
