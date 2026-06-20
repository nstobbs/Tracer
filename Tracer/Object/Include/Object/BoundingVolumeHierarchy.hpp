#pragma once
#include "Object/BoundingBox.hpp"

#include "Core/Ray.hpp"
#include "Core/Scene.hpp"

#include <vector>
#include <array>
#include <map>
#include <utility>
#include <random>
#include <stack>

namespace Tracer {
    class Mesh;

    namespace BVH {

struct MeshNode {
    BBox bbox;
    i32 leftIndex = {-1};
    i32 rightIndex = {-1};
    std::vector<u64> indices;
};

class MeshContainerModel;

/* @class Tracer::BVH::MeshContainerView
   @brief BVH::MeshContainerView is a state machine for performing hit tests from 
   a given BVH::MeshContainerModel and Ray. For speeding up the search by only requiring
   the Mesh::isHit to only test prims that are closer than the last given MeshNode.  
*/
class MeshContainerView {
public:
    MeshContainerView(const MeshContainerModel* model, const Ray& ray);
    const MeshNode* next(); /* Returns a MeshNode to test.*/
    const bool finished(); /* Returns true if there is nothing else left to test.*/
    void record(); /* If the current node did hit, record the distance */

    u32 testedCount() const { return m_testedCount; }

private:
    const MeshContainerModel* m_model;
    const Ray& m_ray;
    f32 m_distanceThreshold = {std::numeric_limits<f32>::max()};

    std::stack<const MeshNode*> m_stack;
    f32 m_selectedDistance = {0.0f};

    bool m_finished = {false};
    u32 m_testedCount = {0};
};

/* @class Tracer::BVH::MeshContainerModel
   @brief BVH::MeshContainerModel is the model for storing and building the tree of MeshNodes
    with a target count of prims per node. The tree is built from top-down
    using the given BVH::Algorithm as the strategy to use for spliting MeshNodes into smaller
    pieces.
*/
enum class Algorithm {
    eInvalid = -1, 
    eObjectMedian = 0, /* Halves the Centeroid BBox On the Longest Axis. */
    eSurfaceAreaHeuristic = 1 /* Uses a Cost Function to Find the Best Cut on the Longest Axis.*/
};

class MeshContainerModel {
public:
    friend class MeshContainerView;

    MeshContainerModel() = default;
    MeshContainerModel(Algorithm algorithm, Mesh* mesh) : m_algorithm(algorithm), m_pMesh(mesh) { };
    ~MeshContainerModel() = default;

    void SetTrianglesPerNode(u32 triangleCount);
    void BuildBVH();
    /* Return a MeshContainerView that can be used to search for Hit MeshNode* */
    MeshContainerView createMeshContainerView(const Ray& ray) const;
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