#include "Tracer/Camera.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Tracer {

Matrix4 Camera::GetViewMatrix() const {
    return glm::lookAt(m_position, m_lookAt, m_Up);
};

void Camera::MoveCamera(f32 delta, CameraDirection direction) {
    m_version++;
    switch (direction) {
        case CameraDirection::eBackward:
            m_position = m_position + (Vector3(0.0f, 0.0f, -0.1f) * delta);
            break;
        case CameraDirection::eForward:
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

Ray Camera::TransformRay(const Ray& primaryRay) const {
    Ray ray(primaryRay.origin, primaryRay.direction);
    Matrix4 cameraView = GetViewMatrix();
    Matrix4 cameraToWorld = glm::inverse(cameraView);

    auto tempRayOrigin = cameraToWorld * Vector4(ray.origin, 1.0f);
    auto tempRayDirection = cameraToWorld * Vector4(ray.direction, 0.0f);

    ray.origin = Point3(tempRayOrigin.x, tempRayOrigin.y, tempRayOrigin.z);
    ray.direction = glm::normalize(Vector3(tempRayDirection.x, tempRayDirection.y, tempRayDirection.z));
    return ray;
}

}