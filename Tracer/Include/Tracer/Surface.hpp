#pragma once
#include "Tracer/Types.hpp"
#include "Tracer/Ray.hpp"

namespace Tracer{

class Surface {
public:
    ~Surface() = default;

    Color4 virtual CalculateColor(const HitInfo& info) = 0;

protected:

};

/* TODO: Move theses surfaces into they our files */
class VertexColor : public Surface {
public:
    Color4 CalculateColor(const HitInfo& info) override;
};

class PreviewNormals : public Surface {
public:
    Color4 CalculateColor(const HitInfo& info) override;
};


}