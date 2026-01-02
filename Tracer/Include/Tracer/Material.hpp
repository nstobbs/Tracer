#pragma once
#include "Tracer/Types.hpp"

namespace Tracer{

class Material {
public:
    Material();
    ~Material() = default;

protected:
    Color4 m_BaseColor;
};

}