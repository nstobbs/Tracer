#include "Tracer/Camera.hpp"
#include "glm/ext/matrix_transform.hpp"

#include <algorithm>
#include <cmath>

namespace Tracer {

void Camera::Transform(f32 delta, CameraDirection direction) {
    m_version++;
    switch (direction) {
        case CameraDirection::eBackward:
            m_position = m_position - (ForwardVector() * delta);
            break;
        case CameraDirection::eForward:
            m_position = m_position + (ForwardVector() * delta);
            break;
        case CameraDirection::eRight:
            m_position = m_position + (RightVector(ForwardVector()) * delta);
            break;
        case CameraDirection::eLeft:
            m_position = m_position - (RightVector(ForwardVector()) * delta);
            break;
        case CameraDirection::ePanLeft:
            m_pan = m_pan + (1.0f * delta);
            break;
        case CameraDirection::ePanRight:
            m_pan = m_pan - (1.0f * delta);
            break;
        case CameraDirection::eTiltUp:
            m_tilt = std::clamp((m_tilt - (1.0f * delta)), f32(-kPi * 0.5f), f32(kPi * 0.5f));
            break;
        case CameraDirection::eTiltDown:
            m_tilt = std::clamp((m_tilt + (1.0f * delta)), f32(-kPi * 0.5f), f32(kPi * 0.5f));
            break;
    }
};

Vector3 Camera::ForwardVector() const {
    return glm::normalize(Rotate(Vector3(0.0f, 0.0f, m_focalLength)));
};

Vector3 Camera::RightVector(Vector3 direction) const {
    return glm::normalize(glm::cross(m_Up, direction));
}

Vector3 Camera::Rotate(Vector3 direction) const {
    /* Pan */
    float cosPan = std::cos(m_pan);
    float sinPan = std::sin(m_pan);
    Vector3 panned = Vector3(direction.x * cosPan - direction.z * sinPan,
                             direction.y,
                             direction.x * sinPan + direction.z * cosPan);
    /* Tilt */
    Vector3 right = RightVector(panned);
    float cosTilt = std::cos(m_tilt);
    float sinTilt = std::sin(m_tilt);
    Vector3 tilted = panned * cosTilt
                    + glm::cross(right, panned) * sinTilt;
    
    return glm::normalize(tilted);
};

Ray Camera::GetRay(const Image& image, u32 x, u32 y) const {
    /* Tracer::Image - 0, 0 is the Top Left Pixel. 
    Viewport -1.0f, 1.0f is the Top Left Pixel */

    f32 width = image.GetWidth();
    f32 height =  image.GetHeight();
    f32 aspectRatio = width / height;

    f32 ndcX = (-1.0f + ((static_cast<f32>(x) + 0.5f) * 2.0f / width));
    f32 ndcY = (1.0f - ((static_cast<f32>(y) + 0.5f) * 2.0f / height));

    Vector3 forward = Rotate(Vector3(0.0, 0.0f, m_focalLength));
    Vector3 right = RightVector(forward);
    Vector3 up = glm::normalize(glm::cross(forward, right));

    Vector3 direction = glm::normalize(forward
                        + right * (ndcX * aspectRatio)
                        + up * ndcY);
    return Ray(m_position, direction);
};

}