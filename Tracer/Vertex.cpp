#include "Tracer/Vertex.hpp"

namespace Tracer {

Vertex multiply(Matrix4 mat, Vertex vertex) {
    Vertex result{};
    result.position.x = (mat * Vector4(vertex.position, 1.0f)).x;
    result.position.z = (mat * Vector4(vertex.position, 1.0f)).z;
    result.position.y = (mat * Vector4(vertex.position, 1.0f)).y;

    result.normals.x = (mat * Vector4(vertex.normals, 1.0f)).x;
    result.normals.y = (mat * Vector4(vertex.normals, 1.0f)).y;
    result.normals.z = (mat * Vector4(vertex.normals, 1.0f)).z;

    result.color = vertex.color;
    result.textureUV = vertex.textureUV;
    return result;
};

}