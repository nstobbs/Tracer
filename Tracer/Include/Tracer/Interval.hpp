#pragma once
#include "Tracer/Types.hpp"

#include <limits>

namespace Tracer {

const f32 kInfinity = std::numeric_limits<f32>::infinity();

class Interval {
public:
    Interval() : m_min(-kInfinity), m_max(+kInfinity) {}
    Interval(f32 min, f32 max) : m_min(min), m_max(max) {}

    f32 Max() const { return m_max; }
    f32 Min() const { return m_min; }

    f32 Size() const { return m_max - m_min; }
    f32 Contains(f32 value) const { return m_min <= value && value <= m_max; }
    bool Surrounds(f32 value) const { return m_min < value && value < m_max; }
    //f64 Clamp(f64 value) const { if(x < m_min) return m_min; if(x < m_max) return m_max; return x; }
    
private:
    f32 m_min, m_max;
};

}