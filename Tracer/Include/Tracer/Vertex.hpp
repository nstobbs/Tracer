#pragma once
#include "Tracer/Types.hpp"

namespace Tracer {

struct Vertex {
    Point3 position = {0, 0, 0};
    Vector3 normals = {0, 0, 0};
    Point2 textureUV = {0, 0};
    Color4 color = {0, 0, 0, 0};
};

struct VertexInfo {
    bool hasPosition = {false};
    bool hasNormals = {false};
    bool hasTextureUVs = {false};
    bool hasColor = {false};
};

}