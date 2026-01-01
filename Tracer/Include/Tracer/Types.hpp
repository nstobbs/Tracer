#pragma once

#include <memory>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Tracer {

using Point3 = glm::vec3;
using Color4 = glm::vec4;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

template<typename T>
using UniquePtr = std::unique_ptr<T>;

}