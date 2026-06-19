#pragma once

#include "Surface/Surface.hpp"

namespace Tracer {

/* Displays a Solid Colour */
class SolidColor : public Surface {
public:
    SolidColor(Color4 color) : m_color(color) { };
    Color4 CalculateColor(const HitInfo& info) override {
        // Disabled since I don't want this enabled when
        // rendering light sources shapes.
        //if (!info.isFrontFace) {
        //    return m_backfaceColor;
        //};
        return m_color;
    }

private:
    Color4 m_color;
};

}