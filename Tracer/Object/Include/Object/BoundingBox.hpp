#pragma once
#include "Core/Types.hpp"
#include "Core/Ray.hpp"

namespace Tracer {

class BBox {
public:
    BBox() = default;
    BBox(const Point3& min, const Point3& max) : m_min(min), m_max(max) { };
    ~BBox() = default;

    void Expand(Point3 point);

    /* Returns true if hit, also records the distance via a f32* in outDistance */
    bool isHit(const Ray& ray) const;
    f32 distance(const Ray& ray) const;

    Point3 Max() const { return m_max; }
    Point3 Min() const { return m_min; }

    void SetMax(Point3 max) { m_max = max; }
    void SetMin(Point3 min) { m_min = min; }

    bool Contains(Point3 point) const;

private:
    Point3 m_min = {0.0f, 0.0f, 0.0f};
    Point3 m_max = {0.0f, 0.0f, 0.0f};
};
}