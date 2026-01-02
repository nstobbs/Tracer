#pragma once
#include "Tracer/Types.hpp"

/*
@name Tracer::BBox

@brief Defines the volumetric shape an objects takes up using an minimal size bounding box.
*/

namespace Tracer {

class BBox {
public:
    BBox();
    ~BBox() = default;
private:
    Point3 m_bottomLeft;
    Point3 m_topRight;
};
}