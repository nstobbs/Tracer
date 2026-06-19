#pragma once

#include "Surface/Surface.hpp"

namespace Tracer {

/* Displays the Geometric Normals Created during Intersection Testing  */
class GeometricNormals : public Surface {
public:
    Color4 CalculateColor(const HitInfo& info) override {
        if (!info.isFrontFace) {
            return m_backfaceColor;
        };
        return Color4(info.normal, 1.0f);
    }
};

}