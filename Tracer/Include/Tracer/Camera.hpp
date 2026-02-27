#pragma once
#include "Tracer/Ray.hpp"

namespace Tracer {

enum class CameraDirection {
    eInvalid = -1,
    eForward = 0,
    eBackward = 1,
    eLeft = 2,
    eRight = 4,
    eUp = 5,
    eDown = 6 
};

class Ray;

class Camera {
public:
    Camera() = default;
    ~Camera() = default;

    f32 GetFocalLength() const {return m_focalLength;}; 
    Vector3 GetUp() const {return m_Up;};
    Matrix4 GetViewMatrix() const;
    void MoveCamera(f32 delta, CameraDirection direction);
    u64 GetCameraVersion() const { return m_version; };
    Ray TransformRay(const Ray& cameraRay) const;

private:
    Point3 m_position = {0.0f, 0.0f, -2.0f};
    Vector3 m_lookAt = {0.0f, 0.0f, 0.0f};

    Vector3 m_Up = {0.0f, 1.0f, 0.0f};
    f32 m_focalLength = {0.5f};

    u64 m_version = {0};
};

}