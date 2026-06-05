#pragma once
#include "Core/Types.hpp"
#include "Core/Ray.hpp"

namespace Tracer {

class Transform {
public:
    Transform() = default;
    ~Transform() = default;

    void setTranslate(Vector3 translate);
    void setRotation(Vector3 rotation);
    void setScale(Vector3 scale);

    Vector3 getTranslate() const { return m_translate; }
    Vector3 getRotation() const { return m_rotation; }
    Vector3 getScale() const { return m_scale; }

    /* Builds and Caches the Transformation Matrix */
    void build(); 

    Point3 pointToObject(Point3 point) const;
    Point3 pointToWorld(Point3 point) const;

    Vector3 vectorToObject(Vector3 vector) const;
    Vector3 vectorToWorld(Vector3 vector) const;

    /* Transform Ray from world space to object space. */
    Ray transformRay(const Ray& ray) const; 
    /* Transform HitInfo from object space to world space */
    HitInfo transformHitInfo(const HitInfo& info, const Ray& ray) const;

private:
    bool isDirty() const;
    u64 m_version = {1};
    u64 m_lastVersion = {0};
    Matrix4 m_model; /* Transformation Matrix */
    Matrix4 m_invModel; /* Inverse Transformation Matrix */
    Matrix4 m_invModelT; /* Inverse Transpose Matrix */

    Vector3 m_translate = {0.0f, 0.0f, 0.0f};
    Vector3 m_rotation = {0.0f, 0.0f, 0.0f};
    Vector3 m_scale = {1.0f, 1.0f, 1.0f};
};

}