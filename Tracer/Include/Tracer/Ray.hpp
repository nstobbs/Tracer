#pragma once

#include "Tracer/Types.hpp"
#include "Tracer/Material.hpp"

namespace Tracer {

struct Ray {
    Ray(Point3 orig, Vector3 dir) : origin(orig), direction(dir) { };
    Point3 origin;
    Vector3 direction;
};

}