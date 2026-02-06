#include "Tracer/Vertex.hpp"

namespace Tracer {

Vertex multiply(Matrix4 matrix, Vertex vertex) {
    Vertex result{};
    Vector4 position = matrix * Vector4(vertex.position, 1.0f);
    result.position.x = position.x;
    result.position.z = position.z;
    result.position.y = position.y;

    Vector4 normals = matrix * Vector4(vertex.normals, 1.0f);
    result.normals.x = normals.x;
    result.normals.y = normals.y;
    result.normals.z = normals.z;

   // result.normals = vertex.normals;

    result.color = vertex.color;
    result.textureUV = vertex.textureUV;
    return result;
};

}