#pragma once
#include "Tracer/BBox.hpp"

namespace Tracer {

class Object {
public:
    bool virtual isHit();

protected:
    // Material* m_material;
    Point3 m_position;
    BBox m_boundingBox;
};

}; /* End of Tracer namespace */