#pragma once
#include "Tracer/Types.hpp"

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