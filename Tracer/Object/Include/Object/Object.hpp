#pragma once
#include "Object/BoundingBox.hpp"
#include "Object/Transform.hpp"

#include "Core/Ray.hpp"
#include "Core/Interval.hpp"
#include "Core/Camera.hpp"

namespace Tracer {
class Material;
class Surface;


class Object {
public:
    bool virtual isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) = 0;
    //void setMaterial(Material* material) { m_material = material; };  
    //Material* getMaterial() const { return m_material; };
    void setSurface(Surface* surface) { m_surface = surface; };  
    Surface* getSurface() const { return m_surface; };
    BBox getBBox() const { return m_bbox; };
    Transform& transform() { return m_transform; }

protected:
    //Material* m_material = {nullptr};
    Surface* m_surface = {nullptr};
    BBox m_bbox;
    Transform m_transform;
};
}