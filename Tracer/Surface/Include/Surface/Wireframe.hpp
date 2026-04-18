#include "Surface/Surface.hpp"

namespace Tracer {

/* Displays a Wireframe */
class Wireframe : public Surface {
public:
    Wireframe(Color4 color) : m_color(color) { };
    Color4 CalculateColor(const HitInfo& info) override {
        Color4 output(0.0f, 0.0f, 0.0f, 0.0f);
        if (info.type == ShapeType::eTriangle) {
            f32 u, v, w;

            u = info.extra.triangle.u;
            v = info.extra.triangle.v;
            w = info.extra.triangle.w;

            if (u < m_lineThickness ||
                v < m_lineThickness ||
                w < m_lineThickness) {
                    output = m_color;
                };
        };
        return output;
    }
private:
    Color4 m_color;
    f32 m_lineThickness = {0.05f};
};

}