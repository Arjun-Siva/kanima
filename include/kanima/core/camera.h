#ifndef CAMERA_H
#define CAMERA_H

#include <kanima/linalg/vec3.h>
#include <kanima/linalg/mat3.h>
#include <kanima/core/ray.h>
#include <kanima/util/transform.h>

namespace krt
{

class Camera
{
private:
    vec3 eye;
    vec3 right;
    vec3 up;
    vec3 forward;

    float aspectRatio;
    float focalLength;


public:
    Camera(float aspectRatio);

    Camera(const vec3& eye, const vec3& right, const vec3& up, const vec3& forward, float aspectRatio, float focalLength);

    Camera(const vec3& eye, const vec3& target, const vec3& upVec, float aspectRatio, float focalLength);

    Ray generateRay(float u, float v) const;

    vec3 getPosition() const;

    mat3 getOrientation() const;

    void setOrientation(mat3& matrix);

    // movements

    // displacement
    void dolly(float distance);
    void boom(float distance);
    void truck(float distance);

    // rotation
    void pan(float degrees);
    void tilt(float degrees);
    void roll(float degrees);

    void transformBasis(mat3& rotationMatrix);

    void turntable(const vec3& target, float angleYDeg);

};
}
#endif // CAMERA_H
