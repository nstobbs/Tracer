#pragma once
#include "Tracer/Types.hpp"
#include "Tracer/Ray.hpp"

namespace Tracer {

class Surface {
public:
    ~Surface() = default;
    virtual Color4 CalculateColor(const HitInfo& info) = 0;
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


}
}