#pragma once

#include "Core/Types.hpp"
#include "Object/Vertex.hpp"

namespace Tracer {

struct Ray {
    Ray(Point3 orig, Vector3 dir) : origin(orig), direction(dir) { };
    Ray() : origin(Point3(0.0f)), direction(Vector3(0.0f)) { };
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
    f32 u, v, w;
};

union ShapeGen {
    Triangle triangle;
};


class Object;
struct HitInfo {
    Object* object = {nullptr};
    Point3 position;
    Vector3 normal;
    f32 distance;
    bool isFrontFace;
    bool hasHit = {false};

    /* Extra Data Based of Different Shapes. */
    ShapeType type = {ShapeType::eNone};
    ShapeGen extra;
};

}