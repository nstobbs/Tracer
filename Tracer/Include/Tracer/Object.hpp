#pragma once
#include "Tracer/BBox.hpp"
#include "Tracer/Surface.hpp"
#include "Tracer/Ray.hpp"

namespace Tracer {

class Object {
public:
    bool virtual isHit(const Ray& ray, HitInfo& hitInfo) = 0;
    Surface* getSurface() { return m_surface; };

protected:
    Surface* m_surface = {nullptr};
    Point3 m_position;
    //BBox m_boundingBox;
};

}; /* End of Tracer namespace */