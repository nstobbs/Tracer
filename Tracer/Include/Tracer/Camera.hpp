#pragma once

#include "Tracer/Types.hpp"

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

class Camera {
public:
    Camera() = default;
    ~Camera() = default;

    f32 GetFoV() {return m_fov;};
    Vector3 GetUp() {return m_Up;};
    Matrix4 GetViewModel();
    void MoveCamera(f32 delta, CameraDirection direction);
    u64 GetCameraVersion() { return m_version; };

private:
    Point3 m_position = {0.0f, 0.0f, 0.0f};
    Vector3 m_lookAt = {0.0f, 1.0f, -2.5f};

    Vector3 m_Up = {0.0f, 1.0f, 0.0f};
    f32 m_fov = {90.0f};

    u64 m_version = {0};
};

}