#pragma once

namespace krt
{

class Shape {
public:
    unsigned int r, g, b;

    Shape(unsigned int r, unsigned int g, unsigned int b);
    virtual bool pixelInside(int x, int y) const = 0;
    virtual ~Shape();
};

class Circle : public Shape {
    int cx, cy;
    int radius;

public:
    Circle(int cx, int cy, int radius, unsigned int r, unsigned int g, unsigned int b);
    bool pixelInside(int x, int y) const override;
};

class Heart : public Shape {
    int cx, cy;
    float scale;

public:
    Heart(int cx, int cy, float scale, unsigned int r, unsigned int g, unsigned int b);
    bool pixelInside(int x, int y) const override;
};

}
