#include "Surface/Surface.hpp"

#include <algorithm>

namespace Tracer {

/* Displays the Vertex Color */
class VertexColor : public Surface {
public:
    Color4 CalculateColor(const HitInfo& info) override {
        if (!info.isFrontFace) {
            return m_backfaceColor;
        };

        Color4 output(0.0f, 0.0f, 0.0f, 0.0f);
        if (info.type == ShapeType::eTriangle) {
            output = info.extra.triangle.v0.color * static_cast<f32>(info.extra.triangle.u);
            output += info.extra.triangle.v1.color * static_cast<f32>(info.extra.triangle.v);
            output += info.extra.triangle.v2.color * static_cast<f32>(info.extra.triangle.w);

            /* Clamp */
            output.r = std::clamp(output.r, 0.0f, 1.0f);
            output.g = std::clamp(output.g, 0.0f, 1.0f);
            output.b = std::clamp(output.b, 0.0f, 1.0f);
            output.a = std::clamp(output.a, 0.0f, 1.0f);
        };
        return output;
    }
};

}