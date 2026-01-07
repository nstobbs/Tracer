#pragma once
#include "Tracer/Types.hpp"
#include "Tracer/Ray.hpp"

namespace Tracer{

class Surface {
public:
    Surface();
    ~Surface() = default;

    Color4 calculateSurfaceColor(const HitInfo& info);

protected:
    
};

}