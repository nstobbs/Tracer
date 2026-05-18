#pragma once

#include "Core/Types.hpp"

#include <cmath>
#include <random>

namespace Tracer {

//FIXME: Do we really need to normalize this vector?
static inline Vector3 _generateRandomVector(std::default_random_engine rd) {
    return glm::normalize(Vector3(m_dist(rd), m_dist(rd), m_dist(rd)));
}

static Vector3 randomVector() {
    std::default_random_engine m_rd;
    std::uniform_real_distribution<f32> m_dist (-1.0f, 1.0f);
    return _generateRandomVector(m_dist);
}

static Vector3 randomVector(f32 min, f32 max) {
    std::default_random_engine m_rd;
    std::uniform_real_distribution<f32> m_dist (min, max);
    
    return _generateRandomVector(m_dist);
}



}