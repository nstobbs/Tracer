#pragma once
#include "Object/BoundingBox.hpp"

#include "Tracer/Ray.hpp"
#include "Tracer/Scene.hpp"

#include <vector>
#include <array>
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
    std::vector<MeshNode> AllNodes() const { return m_nodes; }
    void SetMesh(Mesh* mesh) { m_pMesh = mesh; }

    u64 GetVersion() const { return m_version; }

private:
    std::pair<BBox, BBox> splitBBox(BBox& bbox);
    std::pair<MeshNode, MeshNode> splitNode(MeshNode& node);
    
    /* Return a array Axises based of the order of their length. Largest first */
    std::array<u32, 3> axisLengthOrder(BBox bbox);
    /* Returns one Half of a Spilt BBox on a Given Axis */
    BBox splitBBoxOnAxis(BBox& bbox, u32 axis);
    /* Moves Indices into two different MeshNode based if the centroid is within or outside of the given BBox. */
    void transferIndicesToChildNodes(BBox anchorPoint, MeshNode& from, MeshNode& toA, MeshNode& toB);
    /* Creates a new bbox based of the Centroid of all of the Triangles within a given MeshNode */
    BBox calculateCentroidBBox(MeshNode& node);
    

    Mesh* m_pMesh = {nullptr};
    u64 m_version = {0};
    std::vector<MeshNode> m_nodes;
    u32 m_trianglesPerNode = {3};
    i32 m_depthCount = {0};
};

};
};