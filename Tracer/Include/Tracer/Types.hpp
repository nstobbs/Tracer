#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Tracer {

using Point3 = glm::vec3;
using Point2 = glm::vec2;
using Vector3 = glm::vec3;
using Color4 = glm::vec4;

using u32 = uint32_t;
using u64 = uint64_t;
using i32 = int;
using f32 = float;
using f64 = double;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

template<typename T>
using UniquePtr = std::unique_ptr<T>;

}