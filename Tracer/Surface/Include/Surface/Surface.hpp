#pragma once
#include "Core/Types.hpp"
#include "Core/Ray.hpp"

namespace Tracer {

class Surface {
public:
    ~Surface() = default;
    virtual Color4 CalculateColor(const HitInfo& info) = 0;
protected:
    Color4 m_backfaceColor = {1.0f, 0.2f, 1.0f, 1.0f};
};

}