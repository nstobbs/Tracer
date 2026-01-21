#pragma once

#include "Tracer/Types.hpp"
#include "Tracer/Surface.hpp"
#include "Tracer/Vertex.hpp"

namespace Tracer {

struct Ray {
    Ray(Point3 orig, Vector3 dir) : origin(orig), direction(dir) { };
    Point3 origin;
    Vector3 direction;
};

enum class ShapeType {
    eNone = -1,
    eTriangle = 1,
    ePolygon = 2
};

class Triangle {
public:
    Vertex v0, v1, v2;
    f64 u, v, w;
};

union ShapeGen {
    Triangle* pTriangle;
};

struct HitInfo {
    Point3 position;
    Vector3 normal;
    f64 distance;
    bool isFrontFace;

    /* Extra Data Based of Different Shapes. */
    ShapeType type = {ShapeType::eNone};
    ShapeGen extra;
};

}