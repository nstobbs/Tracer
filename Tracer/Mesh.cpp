#include "Tracer/Mesh.hpp"

namespace Tracer {

bool Mesh::isHit(const Ray& ray, HitInfo& hitInfo) {
    return true;
};

Mesh Mesh::ColorfulTriangle() {
    Mesh triangleMesh;
    VertexInfo info {
        .hasPosition = true,
        .hasNormals = true,
        .hasTextureUVs = false,
        .hasColor = true };
    triangleMesh.m_info = info;

    Vertex A;
    Vertex B;
    Vertex C;
    std::vector<Vertex> vertices;

    A.position = Point3(0.0f, 1.0f, 0.0f);
    A.normals = Vector3(0.0f, 0.0f, -1.0f);
    A.color = Color4(1.0f, 0.0f, 0.0f, 1.0f);

    B.position = Point3(1.0f, 0.0f, 0.0f);
    A.normals = Vector3(0.0f, 0.0f, -1.0f);
    B.color = Color4(0.0f, 1.0f, 0.0f, 1.0f);

    C.position = Point3(-1.0f, 0.0f, 0.0f);
    A.normals = Vector3(0.0f, 0.0f, -1.0f);
    C.color = Color4(0.0f, 0.0f, 1.0f, 1.0f);
    
    vertices.push_back(A);
    vertices.push_back(B);
    vertices.push_back(C);

    triangleMesh.m_vertices = vertices;

    std::vector<u64> indices = {0, 1, 2};
    triangleMesh.m_indices = indices;
    
    triangleMesh.m_position = Point3(0.0f, 0.0f, 0.0f);

    return triangleMesh;
};

}
