#pragma once
#include "Tracer/BBox.hpp"
#include "Tracer/Material.hpp"
#include "Tracer/Ray.hpp"

namespace Tracer {

class Object {
public:
    bool virtual isHit(const Ray& ray, HitInfo& hitInfo) = 0;

protected:
    Material* m_material;
    Point3 m_position;
    BBox m_boundingBox;
};

struct HitInfo {
    Point3 position;
    Vector3 normal;
    f64 distance;
    bool frontFace;
    Material* material;
};

}; /* End of Tracer namespace */