#pragma once

#include "Tracer/Types.hpp"
#include "Object/BoundingBox.hpp"

#include <vector>

namespace Tracer {

class Light {
public:
    Light(Point3 position, Color3 color) : m_position(position), m_color(color) { };
    static std::vector<Light> CreateLightField(BBox bbox, u32 lightCount);
    Color3 getColor() const { return m_color; };
    Point3 getPosistion() const { return m_position; };
private:
    Point3 m_position;
    Color3 m_color;
};

} /* End of Tracer namespace */