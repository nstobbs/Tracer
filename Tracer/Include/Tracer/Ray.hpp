#pragma once

#include "Tracer/Types.hpp"
#include "Tracer/Surface.hpp"

namespace Tracer {

struct Ray {
    Ray(Point3 orig, Vector3 dir) : origin(orig), direction(dir) { };
    Point3 origin;
    Vector3 direction;
};

struct HitInfo {
    Point3 position;
    Vector3 normal;
    f64 distance;
    bool isFrontFace;
};

}