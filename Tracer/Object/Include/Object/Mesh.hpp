#pragma once

#include "Object/Object.hpp"
#include "Object/Vertex.hpp"
#include "Object/BoundingVolumeHierarchy.hpp"

#include <vector>
#include <string>

namespace Tracer {

class Mesh : public Object {
public:
    Mesh();
    Mesh(u64 vertexCount, void* verticesPtr);
    ~Mesh() = default;

    static std::vector<Mesh> ReadFile(const std::string& filepath); /* Create meshes from File */
    static Mesh TriangleMesh(); /* Create an Single Triangle */

    bool isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) override;
    const std::vector<Vertex>& GetVertices() const { return m_vertices; }
    const std::vector<u64>& GetIndices() const { return m_indices; }

    BVH::MeshContainer* getMeshContainer() { return &m_container; }

private:
    VertexInfo m_info;
    std::vector<Vertex> m_vertices;
    std::vector<u64> m_indices;

    BVH::MeshContainer m_container;
};

}