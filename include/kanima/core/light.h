#ifndef LIGHT_H
#define LIGHT_H

#include <kanima/linalg/vec3.h>
#include <kanima/core/ray.h>

namespace krt
{
class Light
{
private:
    vec3 position;
    float intensity;
public:
    Light(vec3& position, float intensity);

    void changePosition(vec3& newPos);
    void changeIntensity(float newIntensity);

    vec3 getPosition() const;
    float getIntensity() const;
    double distanceFromPoint(vec3 point) const;

};

}
#endif // LIGHT_H
