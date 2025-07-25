#include <kanima/core/light.h>

namespace krt
{

Light::Light(vec3& position, float intensity) : position(position), intensity(intensity) {};

void Light::changePosition(vec3& newPos)
{
    this->position = newPos;
}

void Light::changeIntensity(float newIntensity)
{
    this->intensity = newIntensity;
}

vec3 Light::getPosition() const
{
    return this->position;
}

float Light::getIntensity() const
{
    return this->intensity;
}

}
