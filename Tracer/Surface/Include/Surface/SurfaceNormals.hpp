#include "Surface/Surface.hpp"

namespace Tracer {

/* Displays the Surface Normals Created Using the Vertex Normals and Barycentric system */
class SurfaceNormals : public Surface {
public:
    Color4 CalculateColor(const HitInfo& info) override {
        if (!info.isFrontFace) {
            return m_backfaceColor;
        };

        Color4 output(0.0f, 0.0f, 0.0f, 1.0f);
        if (info.type == ShapeType::eTriangle) {
            output = Color4(info.extra.triangle.v0.normals, 1.0f) * static_cast<f32>(info.extra.triangle.u);
            output += Color4(info.extra.triangle.v1.normals, 1.0f) * static_cast<f32>(info.extra.triangle.v);
            output += Color4(info.extra.triangle.v2.normals, 1.0f) * static_cast<f32>(info.extra.triangle.w);
            output.z = 1.0f;
        };
        return output;
    }
    
};
}