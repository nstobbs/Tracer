#pragma once
#include "Tracer/BBox.hpp"
#include "Tracer/Surface.hpp"
#include "Tracer/Ray.hpp"
#include "Tracer/Interval.hpp"
#include "Tracer/Camera.hpp"

namespace Tracer {

class Object {
public:
    bool virtual isHit(const Ray& ray, HitInfo& hitInfo, Interval interval, Camera& camera) = 0;
    void SetSurface(Surface* surface) { m_surface = surface; };  
    Surface* getSurface() { return m_surface; };

protected:
    Surface* m_surface = {nullptr};
    //BBox m_boundingBox;
};

}; /* End of Tracer namespace */