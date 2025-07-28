#include <kanima/core/camera.h>

namespace krt
{

Camera::Camera(float aspectRatio) : aspectRatio(aspectRatio)
{
    eye = vec3(0,0,0);
    forward = vec3(0,0,-1);
    right = vec3(1,0,0);
    up = vec3(0,1,0);
    focalLength = 1.0f;
}

Camera::Camera(const vec3& eye, const vec3& right, const vec3& up, const vec3& forward, float aspectRatio, float focalLength)
    : eye(eye), right(right), up(up), forward(forward), aspectRatio(aspectRatio), focalLength(focalLength)
{}

Camera::Camera(const vec3& eye, const vec3& target, const vec3& upVec, float aspectRatio, float focalLength)
       : eye(eye), aspectRatio(aspectRatio), focalLength(focalLength)
{
   forward = (target - eye).normalized();
   right = forward.cross(upVec).normalized();
   up = right.cross(forward).normalized();
}

vec3 Camera::getPosition() const
{
    return this->eye;
}

mat3 Camera::getOrientation() const
{
    return mat3(right, up, forward);
}

void Camera::setOrientation(mat3& matrix)
{
    this->right = matrix.transpose() * this->right;
    this->forward = matrix.transpose() * this->forward;
    this->up = matrix.transpose() * this->up;
}

void Camera::setFocalLength(float focalLength)
{
    this->focalLength = focalLength;
}


Ray Camera::generateRay(float u, float v) const
{
    // Center of the image plane
    vec3 imageCenter = eye + forward * focalLength;

    float viewportHeight = 2.0f;
    float viewportWidth = viewportHeight * aspectRatio;

    vec3 horizontal = right * viewportWidth;
    vec3 vertical = up * viewportHeight;

    vec3 lowerLeft = imageCenter - horizontal * 0.5f - vertical * 0.5f;

    // Flip v to match image convention (top-to-bottom)
    v = 1.0f - v;

    vec3 pixelPos = lowerLeft + horizontal * u + vertical * v;
    vec3 rayDir = (pixelPos - eye).normalized();

    return Ray{ eye, rayDir, RayType::camera, 1 };
}

void Camera::dolly(float distance)
{
    eye = eye + forward * distance;
}

void Camera::boom(float distance)
{
    eye = eye + up * distance;
}

void Camera::truck(float distance)
{
    eye = eye + right * distance;
}

void Camera::pan(float degrees)
{
    right = rotateY(degrees) * right;
    forward = rotateY(degrees) * forward;
    up = rotateY(degrees) * up;
}

void Camera::tilt(float degrees)
{
    right = rotateX(degrees) * right;
    forward = rotateX(degrees) * forward;
    up = rotateX(degrees) * up;
}

void Camera::roll(float degrees)
{
    right = rotateZ(degrees) * right;
    forward = rotateZ(degrees) * forward;
    up = rotateZ(degrees) * up;
}

void Camera::transformBasis(mat3& rotationMatrix)
{
    right = rotationMatrix * right;
    forward = rotationMatrix * forward;
    up = rotationMatrix * up;
}

void Camera::turntable(const vec3& target, float angleYDeg)
{
    // Offset from target to camera
    vec3 offset = eye - target;

    // Rotate offset around global Y
    offset = rotateY(angleYDeg) * offset;

    eye = target + offset;
    forward = (target - eye).normalized();
    right = forward.cross(vec3(0, 1, 0)).normalized();
    up = right.cross(forward).normalized();
}

}



