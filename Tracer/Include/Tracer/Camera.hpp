#pragma once

#include "Tracer/Types.hpp"

namespace Tracer {

class Camera {
public:
    Camera() = default;
    ~Camera() = default;

    f32 GetFoV() {return m_fov;};
    Vector3 GetUp() {return m_Up;};

private:
    Point3 m_position;
    Vector3 m_lookAt;

    Vector3 m_Up = {0.0f, 1.0f, 0.0f};
    f32 m_fov = {90.0f};
};

}