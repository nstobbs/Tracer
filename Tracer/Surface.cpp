#include "Tracer/Surface.hpp"

namespace Tracer {

Color4 SurfaceShader::VertexColor::CalculateColor(const HitInfo& info) {
    Color4 output(0.0f, 0.0f, 0.0f, 0.0f);
    if (info.type == ShapeType::eTriangle) {
        Vertex v0, v1, v2;
        f64 u, v, w;
        v0 = info.extra.pTriangle->v0;
        v1 = info.extra.pTriangle->v1;
        v2 = info.extra.pTriangle->v2;

        u = info.extra.pTriangle->u;
        v = info.extra.pTriangle->v;
        w = info.extra.pTriangle->w;

        output = v0.color * static_cast<f32>(u);
        output += v1.color * static_cast<f32>(v);
        output += v2.color * static_cast<f32>(w);
        output.w = 1.0f;
    }
    return output;
};

Color4 SurfaceShader::PreviewNormals::CalculateColor(const HitInfo& info) {
    Color4 output(0.0f, 0.0f, 0.0f, 1.0f);
    if (info.type == ShapeType::eTriangle) {
        Vertex v0, v1, v2;
        f64 u, v, w;
        v0 = info.extra.pTriangle->v0;
        v1 = info.extra.pTriangle->v1;
        v2 = info.extra.pTriangle->v2;

        u = info.extra.pTriangle->u;
        v = info.extra.pTriangle->v;
        w = info.extra.pTriangle->w;

        output = Color4(-v0.normals, 1.0f) * static_cast<f32>(u);
        output += Color4(-v1.normals, 1.0f) * static_cast<f32>(v);
        output += Color4(-v2.normals, 1.0f) * static_cast<f32>(w);
        output.w = 1.0f;
    };

    return Color4(1.0f, 0.0f, 0.0f, 1.0f);
};

}
