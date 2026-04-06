#pragma once
#include "Tracer/Ray.hpp"
#include "Tracer/Image.hpp"

#include <random>

namespace Tracer {

enum class CameraDirection {
    eInvalid = -1,
    eForward = 0,
    eBackward = 1,
    eLeft = 2,
    eRight = 4,
    eUp = 5,
    eDown = 6,
    ePanLeft = 7,
    ePanRight = 8,
    eTiltUp = 9,
    eTiltDown = 10
};

class Ray;

class Camera {
public:
    Camera();
    ~Camera() = default;

    f32 GetFocalLength() const {return m_focalLength;}; 
    void Transform(f32 delta, CameraDirection direction);
    u64 GetVersion() const { return m_version; };

    inline constexpr Vector3 UpVector() const {return m_Up;};
    Vector3 RightVector(Vector3 direction) const;
    Vector3 ForwardVector() const;
    Vector3 Rotate(Vector3 direction) const;
    inline constexpr Point3 Position() const { return m_position; }

    Ray GetRay(const Image& image, u32 x, u32 y);

private:
    Point3 m_position = {0.0f, 0.0f, -2.0f};
    f32 m_pan = {0.0f};
    f32 m_tilt = {0.0f};

    Vector3 m_Up = {0.0f, 1.0f, 0.0f};
    f32 m_focalLength = {0.5f};

    u64 m_version = {0};
    
    /* Jitter Samples */
    std::default_random_engine m_rd;
    std::uniform_real_distribution<f32> m_dist;

};

}