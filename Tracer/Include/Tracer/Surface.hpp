#pragma once
#include "Tracer/Types.hpp"
#include "Tracer/Ray.hpp"

namespace Tracer {

class Surface {
public:
    ~Surface() = default;
    virtual Color4 CalculateColor(const HitInfo& info) = 0;
protected:
    Color4 m_backfaceColor = {1.0f, 0.2f, 1.0f, 1.0f};
};

// TODO: Move theses surfaces into they our files / folder!
namespace SurfaceShader {

class VertexColor : public Surface {
public:
    Color4 CalculateColor(const HitInfo& info) override;
};

class PreviewNormals : public Surface {
public:
    Color4 CalculateColor(const HitInfo& info) override;

};

class SolidColor : public Surface {
public:
    SolidColor(Color4 color) : m_color(color) { };
    Color4 CalculateColor(const HitInfo& info) override;
private:
    Color4 m_color;
};
}
}