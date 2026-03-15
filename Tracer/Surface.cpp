#include "Tracer/Surface.hpp"

namespace Tracer {

Color4 SurfaceShader::VertexColor::CalculateColor(const HitInfo& info) {
    if (!info.isFrontFace) {
        return m_backfaceColor;
    };

    Color4 output(0.0f, 0.0f, 0.0f, 0.0f);
    if (info.type == ShapeType::eTriangle) {
        Vertex v0, v1, v2;
        f64 u, v, w;
        v0 = info.extra.triangle.v0;
        v1 = info.extra.triangle.v1;
        v2 = info.extra.triangle.v2;

        u = info.extra.triangle.u;
        v = info.extra.triangle.v;
        w = info.extra.triangle.w;

        output = v0.color * static_cast<f32>(u);
        output += v1.color * static_cast<f32>(v);
        output += v2.color * static_cast<f32>(w);
        output.w = 1.0f;
    };
    return output;
};

Color4 SurfaceShader::PreviewNormals::CalculateColor(const HitInfo& info) {
    if (!info.isFrontFace) {
        return m_backfaceColor;
    };

    Color4 output(0.0f, 0.0f, 0.0f, 1.0f);
    if (info.type == ShapeType::eTriangle) {
        Vertex v0, v1, v2;
        f64 u, v, w;
        v0 = info.extra.triangle.v0;
        v1 = info.extra.triangle.v1;
        v2 = info.extra.triangle.v2;

        u = info.extra.triangle.u;
        v = info.extra.triangle.v;
        w = info.extra.triangle.w;

        output = Color4(-v0.normals, 1.0f) * static_cast<f32>(u);
        output += Color4(-v1.normals, 1.0f) * static_cast<f32>(v);
        output += Color4(-v2.normals, 1.0f) * static_cast<f32>(w);
        output.w = 1.0f;
    };
    return output;
};

Color4 SurfaceShader::SolidColor::CalculateColor(const HitInfo& info) {
    //if (!info.isFrontFace) {
    //    return m_backfaceColor;
    //};

    return m_color;
};

Color4 SurfaceShader::Wireframe::CalculateColor(const HitInfo& info) {
    Color4 output(0.0f, 0.0f, 0.0f, 0.0f);
    if (info.type == ShapeType::eTriangle) {
        f64 u, v, w;

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
};

Color4 SurfaceShader::MergeSurfaceShader::CalculateColor(const HitInfo& info) {
    Color4 output(0.0f, 0.0f, 0.0f, 0.0f);
    if (m_surfaceA && m_surfaceB) {
        switch (m_operation) {
            case MergeOperation::Plus:
                output = m_surfaceA->CalculateColor(info) + m_surfaceB->CalculateColor(info);
                break;

            case MergeOperation::Over:
                break;
        }
    }
    return output;
};

Color4 SurfaceShader::UVTexture::CalculateColor(const HitInfo& info) {
    Color4 output(0.0f, 0.0f, 0.0f, 0.0f);
    if (!m_image) {
        return Color4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    /* Get UVs from Geo */
    f32 u, v;
    if (info.type == ShapeType::eTriangle) {
        u = (info.extra.triangle.v0.textureUV.x * static_cast<f32>(info.extra.triangle.u));
        u += (info.extra.triangle.v1.textureUV.x * static_cast<f32>(info.extra.triangle.v));
        u += (info.extra.triangle.v2.textureUV.x * static_cast<f32>(info.extra.triangle.w));
        u = u / 3.0f;

        v = (info.extra.triangle.v0.textureUV.y * static_cast<f32>(info.extra.triangle.u));
        v += (info.extra.triangle.v1.textureUV.y * static_cast<f32>(info.extra.triangle.v));
        v += (info.extra.triangle.v2.textureUV.y * static_cast<f32>(info.extra.triangle.w));
        v = v / 3.0f;
    }

    u32 requestX = u * m_image->GetWidth();
    u32 requestY = v * m_image->GetHeight();
    if (requestX > m_image->GetWidth() || requestY > m_image->GetHeight()) {
        return Color4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    output =+ m_image->GetLayer(m_layerName)->GetRow(requestY).at(requestX);
    return output;
}

}
