#pragma once
#include "Tracer/Types.hpp"
#include "Tracer/Ray.hpp"

namespace Tracer{

class Surface {
public:
    ~Surface() = default;

    Color4 virtual calculateSurfaceColor(const HitInfo& info) = 0;

protected:

};

/* TODO: Move theses surfaces into they our files */
class VertexColor : public Surface {
public:
    Color4 calculateSurfaceColor(const HitInfo& info) override;
};


}