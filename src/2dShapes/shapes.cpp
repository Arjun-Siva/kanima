#include <kanima/2dShapes/shapes.h>
#include <cmath>

namespace krt
{

// ----- Base Shape -----
Shape::Shape(unsigned int r, unsigned int g, unsigned int b)
    : r(r), g(g), b(b) {}

Shape::~Shape() {}

// ----- Circle -----
Circle::Circle(int cx, int cy, int radius, unsigned int r, unsigned int g, unsigned int b)
    : Shape(r, g, b), cx(cx), cy(cy), radius(radius) {}

bool Circle::pixelInside(int x, int y) const {
    int dx = x - cx;
    int dy = y - cy;
    return dx * dx + dy * dy <= radius * radius;
}

// ----- Heart -----
Heart::Heart(int cx, int cy, float scale, unsigned int r, unsigned int g, unsigned int b)
    : Shape(r, g, b), cx(cx), cy(cy), scale(scale) {}

bool Heart::pixelInside(int x, int y) const {

    // Evaluate the implicit equation
    // (x^2 + y^2 - 1)^3 - x^2 y^3 <= 0
    float normX = (static_cast<float>(x - cx) / scale);
    float normY = (static_cast<float>(y - cx) / scale);

    // Invert Y-axis for standard mathematical orientation (y increases upwards)
    // PPM coordinates usually have y increasing downwards.
    normY = -normY;
    float term1 = std::pow(normX * normX + normY * normY - 1.0, 3.0);
    float term2 = normX * normX * std::pow(normY, 3.0);

    return term1 - term2 <= 0.0;
}

}
