#include "Object/BoundingBox.hpp"
#include "Core/Interval.hpp"

#include <algorithm>

namespace Tracer {

void BBox::Expand(Point3 point) {
    m_min.x = std::min(point.x, m_min.x);
    m_min.y = std::min(point.y, m_min.y);
    m_min.z = std::min(point.z, m_min.z);

    m_max.x = std::max(point.x, m_max.x);
    m_max.y = std::max(point.y, m_max.y);
    m_max.z = std::max(point.z, m_max.z);
}

bool BBox::isHit(const Ray& ray) const {
    Interval range;
    f32 tMin = range.Min();
    f32 tMax = range.Max();

    f32 xt1 = (m_min.x - ray.origin.x) / ray.direction.x;
    f32 xt2 = (m_max.x - ray.origin.x) / ray.direction.x;
    tMin = std::max(tMin, std::min(xt1, xt2));
    tMax = std::min(tMax, std::max(xt1, xt2));

    f32 yt1 = (m_min.y - ray.origin.y) / ray.direction.y;
    f32 yt2 = (m_max.y - ray.origin.y) / ray.direction.y;
    tMin = std::max(tMin, std::min(yt1, yt2));
    tMax = std::min(tMax, std::max(yt1, yt2));

    f32 zt1 = (m_min.z - ray.origin.z) / ray.direction.z;
    f32 zt2 = (m_max.z - ray.origin.z) / ray.direction.z;
    tMin = std::max(tMin, std::min(zt1, zt2));
    tMax = std::min(tMax, std::max(zt1, zt2));

    return tMin < tMax;
}

bool BBox::Contains(Point3 point) const {
    auto rangeX = Interval(static_cast<f32>(m_min.x), static_cast<f32>(m_max.x));
    auto rangeY = Interval(static_cast<f32>(m_min.y), static_cast<f32>(m_max.y));
    auto rangeZ = Interval(static_cast<f32>(m_min.z), static_cast<f32>(m_max.z));

    return rangeX.Contains(point.x) &&
           rangeY.Contains(point.y) &&
           rangeZ.Contains(point.z); 
}

}