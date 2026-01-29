#include "Tracer/Camera.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Tracer {

Matrix4 Camera::GetViewModel() {
    return glm::lookAt(m_position, m_lookAt, m_Up);
};

void Camera::MoveCamera(f32 delta, CameraDirection direction) {
    m_version++;
    switch (direction) {
        case CameraDirection::eForward:
            m_position = m_position + (Vector3(0.0f, 0.0f, -0.1f) * delta);
            break;
        case CameraDirection::eBackward:
            m_position = m_position + (Vector3(0.0f, 0.0f, 0.1f) * delta);
            break;
        case CameraDirection::eLeft:
            m_position = m_position + (Vector3(-0.1f, 0.0f, 0.0f) * delta);
            break;
        case CameraDirection::eRight:
            m_position = m_position + (Vector3(0.1f, 0.0f, 0.0f) * delta);
            break;
    }
};

};