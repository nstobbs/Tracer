#pragma once
#include "Tracer/BoundingBox.hpp"
#include "Tracer/Ray.hpp"
#include "Tracer/Scene.hpp"

#include <vector>
#include <utility>

namespace Tracer {
    class Mesh;

    namespace BVH {

struct MeshNode {
    BBox bbox;
    i32 leftIndex = {-1};
    i32 rightIndex = {-1};
    std::vector<u64> indices;
};

class MeshContainer {
public:
    MeshContainer() = default;
    MeshContainer(Mesh* mesh) : m_pMesh(mesh) { };
    ~MeshContainer() = default;

    void SetTrianglesPerNode(u32 triangleCount);
    void BuildBVH();
    std::vector<MeshNode> FindAllHitNodes(const Ray& ray) const;
    void SetMesh(Mesh* mesh) { m_pMesh = mesh; }

    u64 GetVersion() const { return m_version; }

private:
    std::pair<BBox, BBox> splitBBox(BBox& bbox);
    std::pair<MeshNode, MeshNode> splitNode(MeshNode& node);
    void transferIndicesToChildNodes(BBox anchorPoint, MeshNode& from, MeshNode& toA, MeshNode& toB);
    

    Mesh* m_pMesh = {nullptr};
    u64 m_version = {0};
    std::vector<MeshNode> m_nodes;
    u32 m_trianglesPerNode = {3};
};

};
};