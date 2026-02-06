#include "Tracer/Camera.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Tracer {

Matrix4 Camera::GetViewMatrix() {
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

Ray Camera::TransformRay(const Ray& cameraRay) {
    Ray ray(cameraRay.origin, cameraRay.direction);
    Matrix4 view = GetViewMatrix();
    Matrix4 invView = glm::inverse(view);

    auto tempRayOrigin = Vector4(ray.origin, 1.0f) * invView;
    auto tempRayDirection = glm::normalize(Vector4(ray.direction, 0.0f) * invView);

    ray.origin = Point3(tempRayOrigin.x, tempRayOrigin.y, tempRayOrigin.z);
    ray.direction = Vector3(tempRayDirection.x, tempRayDirection.y, tempRayDirection.z);

    return ray;
};

};