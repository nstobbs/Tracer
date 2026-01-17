#pragma once
#include "Tracer/Types.hpp"

#include <limits>

namespace Tracer {

const f64 kInfinity = std::numeric_limits<f64>::infinity();

class Interval {
public:
    Interval() : m_min(-kInfinity), m_max(+kInfinity) {}
    Interval(f64 min, f64 max) : m_min(min), m_max(max) {}

    f64 Max() const { return m_max; }
    f64 Min() const { return m_min; }

    f64 Size() const { return m_max - m_min; }
    f64 Contains(f64 value) const { return m_min <= value && value <= m_max; }
    bool Surrounds(f64 value) const { return m_min < value && value < m_max; }
    //f64 Clamp(f64 value) const { if(x < m_min) return m_min; if(x < m_max) return m_max; return x; }
    
private:
    double m_min, m_max;
};

}