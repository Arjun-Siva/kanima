#include <kanima/core/color.h>

namespace krt
{
Color::Color(float r, float g, float b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

Color::Color() : r(1.0), g(1.0), b(1.0)
{}

Color Color::operator*(float scalar) const
{
    return Color(r * scalar, g * scalar, b * scalar);
}

Color operator*(float scalar, const Color& c)
{
    return c * scalar;
}

Color Color::operator+(const Color& other)
{
    return Color(r + other.r, g + other.g, b + other.b);
}
}
