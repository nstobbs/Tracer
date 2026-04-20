#pragma once
#include "Object/BoundingBox.hpp"

#include "Tracer/Ray.hpp"
#include "Tracer/Interval.hpp"
#include "Tracer/Camera.hpp"

namespace Tracer {
class Material;


class Object {
public:
    bool virtual isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) = 0;
    void setMaterial(Material* material) { m_material = material; };  
    Material* getMaterial() const { return m_material; };
    BBox getBBox() { return m_bbox; };

protected:
    Material* m_material = {nullptr};
    BBox m_bbox;
};
}