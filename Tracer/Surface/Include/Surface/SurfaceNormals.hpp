#pragma once

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
            Vector3 objectSpaceNormals = info.extra.triangle.v0.normals * static_cast<f32>(info.extra.triangle.u) +
                                         info.extra.triangle.v1.normals * static_cast<f32>(info.extra.triangle.v) +
                                         info.extra.triangle.v2.normals * static_cast<f32>(info.extra.triangle.w);
            Vector3 worldSpaceNormals {};
            if (info.object) {
                worldSpaceNormals = info.object->transform().vectorToWorld(objectSpaceNormals);
            }
            output = Color4(worldSpaceNormals, 1.0f);
        };
        return output;
    }
    
};
}