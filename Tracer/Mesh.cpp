#include "Tracer/Mesh.hpp"

namespace {
    const Tracer::f64 kThreshold = 0.01;
}

namespace Tracer {

bool Mesh::isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) {

    /* Per Each Triangle Of this Mesh */
    u64 vertexCount = m_indices.size();
    for (u64 i = 0; i < vertexCount; i+3) {
        Vertex v0 = m_vertices.at(i);
        Vertex v1 = m_vertices.at(i+1);
        Vertex v2 = m_vertices.at(i+2);

        /* Check if the  Ray is Parallel Lines */
        if (glm::dot(ray.direction, v0.normals) <= kThreshold) {
            return false;
        }

        /* Check where on the Plane of The Normals and Vertex A for the Intersection Of the Ray */
        Vector3 planar = v0.position - ray.direction;
        hitInfo.distance = glm::dot(planar, v0.normals);
        if (hitInfo.distance <= 0.0) {
            return false;
        }

        /* Planar Intersection Point */
        hitInfo.position = ray.origin + ray.direction * static_cast<f32>(hitInfo.distance);

        /* Calculate the barycentric coordinates for that given point.  */
        Vector3 C;
        f64 u, v;

        Vector3 v1p = hitInfo.position - v1.position;
        Vector3 v1v2 = v2.position - v1.position;
        C = glm::cross(v1v2, v1p);
        if ((u = glm::dot(v0.normals, C)) < 0.0) {
            return false;
        }

        Vector3 v2p = hitInfo.position - v2.position;
        Vector3 v2v0 = v0.position - v2.position;
        C = glm::cross(v2v0, v2p);
        if ((v = glm::dot(v0.normals, C)) < 0.0) {
            return false;
        }

        Vector3 v0p = hitInfo.position - v0.position;
        Vector3 v0v1 = v1.position - v0.position;
        C = glm::cross(v0v1, v0p);
        if (glm::dot(v0.normals, C) < 0.0) {
            return false;
        }

        return true;
    }

    return false;
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

    A.position = Point3(0.0f, 1.0f, -8.0f);
    A.normals = Vector3(0.0f, 0.0f, -1.0f);
    A.color = Color4(1.0f, 0.0f, 0.0f, 1.0f);

    B.position = Point3(1.0f, 0.0f, -8.0f);
    A.normals = Vector3(0.0f, 0.0f, -1.0f);
    B.color = Color4(0.0f, 1.0f, 0.0f, 1.0f);

    C.position = Point3(-1.0f, 0.0f, -8.0f);
    A.normals = Vector3(0.0f, 0.0f, -1.0f);
    C.color = Color4(0.0f, 0.0f, 1.0f, 1.0f);
    
    vertices.push_back(A);
    vertices.push_back(B);
    vertices.push_back(C);

    triangleMesh.m_vertices = vertices;

    std::vector<u64> indices = {0, 1, 2};
    triangleMesh.m_indices = indices;
    
    //triangleMesh.m_position = Point3(0.0f, 0.0f, 0.0f);

    return triangleMesh;
};

}
