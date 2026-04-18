#include "Surface/Surface.hpp"

namespace Tracer {

/* Displays a Solid Colour */
class SolidColor : public Surface {
public:
    SolidColor(Color4 color) : m_color(color) { };
    Color4 CalculateColor(const HitInfo& info) override {
        if (!info.isFrontFace) {
            return m_backfaceColor;
        };

        return m_color;
    }

private:
    Color4 m_color;
};

}