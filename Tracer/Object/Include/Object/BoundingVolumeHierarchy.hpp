#pragma once
#include "Object/BoundingBox.hpp"

#include "Core/Ray.hpp"
#include "Core/Scene.hpp"

#include <vector>
#include <array>
#include <map>
#include <utility>
#include <random>

namespace Tracer {
    class Mesh;

    namespace BVH {

struct MeshNode {
    BBox bbox;
    i32 leftIndex = {-1};
    i32 rightIndex = {-1};
    std::vector<u64> indices;
};

enum class Algorithm {
    eInvalid = -1, 
    eObjectMedian = 0, /* Halves the Centeroid BBox On the Longest Axis. */
    eSurfaceAreaHeuristic = 1 /* Uses a Cost Function to Find the Best Cut on the Longest Axis.*/
};

/* @class TestableContainer
   @brief Reduces the numbers of tests required 
   to find the closest prim with an given 
*/
class TestableContainer {
public:
    TestableContainer(std::map<f32, const MeshNode*> foundNodes);
    const MeshNode* next(); /* Returns an random MeshNode to test.*/
    const bool finished(); /* Returns true if there is nothing else left to test.*/
    void record(); /* If the current node did hit, record that distance */

    u32 testedCount() const { return m_testedCount; }
    u32 nodeCount() const { return m_nodeCount;}

private:
    void cutoff();
    std::map<f32, const MeshNode*>  m_found; /* Key: Distance, Value: MeshNode&  */
    f32 m_distanceThreshold;
    std::mt19937 m_rng;

    f32 m_currentDistance = {0.0f};
    u32 m_testedCount = {0};
    u32 m_nodeCount = {0};
};

/* MeshContainer handles the building and store of a given Mesh. */
class MeshContainer {
public:
    MeshContainer() = default;
    MeshContainer(Algorithm algorithm, Mesh* mesh) : m_algorithm(algorithm), m_pMesh(mesh) { };
    ~MeshContainer() = default;

    void SetTrianglesPerNode(u32 triangleCount);
    void BuildBVH();
    TestableContainer FindAllHitNodes(const Ray& ray) const;
    std::vector<MeshNode> AllNodes() const { return m_nodes; }
    void SetMesh(Mesh* mesh) { m_pMesh = mesh; }

    u64 GetVersion() const { return m_version; }

private:
    std::pair<MeshNode, MeshNode> splitNode(MeshNode& node);
    
    /* Return a array Axises based of the order of their length. Largest first */
    std::array<u32, 3> axisLengthOrder(BBox bbox);
    /* Returns one Half of a Spilt BBox on a Given Axis */
    BBox splitBBoxOnAxis(BBox& bbox, u32 axis);
    /* Moves Indices into two different MeshNode based if the centroid is within or outside of the given BBox. */
    void transferIndicesToChildNodes(BBox anchorPoint, MeshNode& from, MeshNode& toA, MeshNode& toB);
    /* Creates a new bbox based of the Centroid of all of the Triangles within a given MeshNode */
    BBox calculateCentroidBBox(MeshNode& node);
    /* Returns an vector of MeshNodes across an Given Axis */
    std::vector<MeshNode> splitBBoxIntoBucketsOnAxis(MeshNode& node, const u32 axis);
    /* Returns the SAH from the given index of an given vector Buckets.*/
    f32 calculateSurfaceAreaHeuristic(std::vector<MeshNode>& nodes, const i32 index);
    /* Combines a vector of MeshNodes into one MeshNode */
    MeshNode combineMeshNodes(std::vector<MeshNode> nodes);
    
    Algorithm m_algorithm;

    Mesh* m_pMesh = {nullptr};
    u64 m_version = {0};
    std::vector<MeshNode> m_nodes;
    u32 m_trianglesPerNode = {3};
    i32 m_depthCount = {0};
};

};
};