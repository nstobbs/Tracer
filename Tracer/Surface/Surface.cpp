#include "Surface/Surface.hpp"

#include <algorithm>

namespace Tracer {

Color4 SurfaceShader::VertexColor::CalculateColor(const HitInfo& info) {
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
};

Color4 SurfaceShader::GeometricNormals::CalculateColor(const HitInfo& info) {
    //if (!info.isFrontFace) {
    //    return m_backfaceColor;
    //};
    return Color4(info.normal, 1.0f);
};

Color4 SurfaceShader::SurfaceNormals::CalculateColor(const HitInfo& info) {
    //if (!info.isFrontFace) {
    //    return m_backfaceColor;
    //};

    Color4 output(0.0f, 0.0f, 0.0f, 1.0f);
    if (info.type == ShapeType::eTriangle) {
        output = Color4(info.extra.triangle.v0.normals, 1.0f) * static_cast<f32>(info.extra.triangle.u);
        output += Color4(info.extra.triangle.v1.normals, 1.0f) * static_cast<f32>(info.extra.triangle.v);
        output += Color4(info.extra.triangle.v2.normals, 1.0f) * static_cast<f32>(info.extra.triangle.w);
        output.z = 1.0f;
    };
    return output;
};

Color4 SurfaceShader::SolidColor::CalculateColor(const HitInfo& info) {
    if (!info.isFrontFace) {
        return m_backfaceColor;
    };

    return m_color;
};

Color4 SurfaceShader::Wireframe::CalculateColor(const HitInfo& info) {
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

    output = m_image->GetLayer(m_layerName)->GetRow(requestY).at(requestX);

    /* Clamp */
    output.r = std::clamp(output.r, 0.0f, 1.0f);
    output.g = std::clamp(output.g, 0.0f, 1.0f);
    output.b = std::clamp(output.b, 0.0f, 1.0f);
    output.a = std::clamp(output.a, 0.0f, 1.0f);

    return output;
}

}
