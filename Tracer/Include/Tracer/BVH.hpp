#pragma once
#include "Tracer/BBox.hpp"
#include "Tracer/Ray.hpp"
#include "Tracer/Scene.hpp"

#include <vector>

namespace Tracer {
    class Mesh;

    namespace BVH {

struct MeshNode {
    BBox bbox;
    i32 leftIndex = {-1};
    i32 rightIndex = {-2};
    std::vector<u64> indices;
};

class MeshContainer {
public:
    MeshContainer() = default;
    MeshContainer(Mesh* mesh) : m_pMesh(mesh) { };
    ~MeshContainer() = default;

    void BuildBVH(u32 indicesPerNode);
    MeshNode FindNodeWithIndices(const Ray& ray) const;
    void SetMesh(Mesh* mesh) { m_pMesh = mesh; }

    u64 GetVersion() const { return m_version; }

private:
    Mesh* m_pMesh = {nullptr};
    i32 m_rootNodeIndex = {-1}; // FIXME: Don't really need this...
    u64 m_version = {0};

    std::vector<MeshNode> m_nodes;
};

};
};