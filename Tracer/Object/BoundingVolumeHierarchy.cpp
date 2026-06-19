#include "Object/BoundingVolumeHierarchy.hpp"
#include "Object/Mesh.hpp"
#include "Core/Interval.hpp"

#include "Core/StatusMessage.hpp"

#include <iostream>
#include <algorithm>
#include <map>

#define DEBUG_PRINT 1

namespace {
    const Tracer::u32 kBucketCount = 16;
}

namespace Tracer {

    namespace BVH {

std::array<u32, 3> MeshContainer::axisLengthOrder(BBox bbox) {
    /* Creates a Map of Axises and the Extent of that axis.
    Sorted in the order of largest extent first.      */
    Point3 extents = bbox.Max() - bbox.Min();
    std::multimap<f32, u32, std::greater<f32>> extentAxisMap{};
    for (u32 i = 0; i < 3; i++) {
        if (i == 0) {
            if (extents.x > 1e-6f) {
                extentAxisMap.emplace(extents.x, i);
            }
        } else if (i == 1) {
            if (extents.y > 1e-6f) {
                extentAxisMap.emplace(extents.y, i);
            }
        } else if (i == 2) {
            if (extents.z > 1e-6f) {
                extentAxisMap.emplace(extents.z, i);
            }
        }
    }

    std::array<u32, 3> output{};
    u32 index = 0;
    for (auto& [extent, axis] : extentAxisMap) {
        output[index] = axis;
        index++;
    }
    return output;
};

BBox MeshContainer::splitBBoxOnAxis(BBox& bbox, u32 axis) {
    BBox halfBBox(bbox.Min(), bbox.Max());
    Point3 max = halfBBox.Max();
    assert(axis < 3); /* Catch Invalid Axis */
    switch(axis) {
        case 0:
            max.x = (bbox.Min().x + bbox.Max().x) / 2.0f;
            break;
        case 1:
            max.y = (bbox.Min().y + bbox.Max().y) / 2.0f;
            break;
        case 2:
            max.z = (bbox.Min().z + bbox.Max().z) / 2.0f;
            break;
    }
    halfBBox.SetMax(max);
    return halfBBox;
}

void MeshContainer::transferIndicesToChildNodes(BBox anchorPoint, MeshNode& from, MeshNode& toA, MeshNode& toB) {
    toA.indices.clear();
    toB.indices.clear();
    
    toA.bbox = BBox(); 
    toB.bbox = BBox();

    u32 triangleCount = (from.indices.size() / 3);
    for (u32 i = 0; i < triangleCount; i++) {
        u32 triangleIndex = i * 3;
        Point3 v0 = m_pMesh->GetVertices().at(from.indices.at(triangleIndex)).position;
        Point3 v1 = m_pMesh->GetVertices().at(from.indices.at(triangleIndex+1)).position;
        Point3 v2 = m_pMesh->GetVertices().at(from.indices.at(triangleIndex+2)).position;
        Point3 centroid = (v0 + v1 + v2) / 3.0f;

        // Test if the Triangle is within the anchor point.
        if (anchorPoint.Contains(centroid)) {
            toA.indices.push_back(from.indices.at(triangleIndex));
            toA.indices.push_back(from.indices.at(triangleIndex+1));
            toA.indices.push_back(from.indices.at(triangleIndex+2));
            // Expand BBox for the Whole Triangle
            toA.bbox.Expand(v0);
            toA.bbox.Expand(v1);
            toA.bbox.Expand(v2);
        } else {
            toB.indices.push_back(from.indices.at(triangleIndex));
            toB.indices.push_back(from.indices.at(triangleIndex+1));
            toB.indices.push_back(from.indices.at(triangleIndex+2));
            // Expand BBox for the Whole Triangle
            toB.bbox.Expand(v0);
            toB.bbox.Expand(v1);
            toB.bbox.Expand(v2);
        };
    }
    from.indices.clear();
}

BBox MeshContainer::calculateCentroidBBox(MeshNode& node) {
    u32 triangleCount = node.indices.size() / 3;
    BBox bbox;
    if (triangleCount != 0) {
        for (u32 i = 0; i < triangleCount; i++) {
            u32 triangleIndex = i * 3;
            Vertex v0 = m_pMesh->GetVertices().at(node.indices.at(triangleIndex+0));
            Vertex v1 = m_pMesh->GetVertices().at(node.indices.at(triangleIndex+1));
            Vertex v2 = m_pMesh->GetVertices().at(node.indices.at(triangleIndex+2));

            Point3 centroid = (v0.position + v1.position + v2.position) / 3.0f;
            bbox.Expand(centroid);
        }
    }
    return bbox;
}

/* Returns an vector of MeshNodes across an Given Axis */
std::vector<MeshNode> MeshContainer::splitBBoxIntoBucketsOnAxis(MeshNode& node, const u32 axis) {
    std::vector<MeshNode> buckets(kBucketCount);
    BBox centroidBBox = calculateCentroidBBox(node);
    f32 axisMin = centroidBBox.Min()[axis];
    f32 axisLen = centroidBBox.Max()[axis] - axisMin;

    auto triangleCount = node.indices.size() / 3;
    for (i32 i = 0; i < triangleCount; i++) {
        u32 triangleIndex = i * 3;
        Point3 centroid = (m_pMesh->GetVertices().at(node.indices.at(triangleIndex)).position +
                           m_pMesh->GetVertices().at(node.indices.at(triangleIndex+1)).position +
                           m_pMesh->GetVertices().at(node.indices.at(triangleIndex+2)).position) / 3.0f;
        i32 bucketIndex = static_cast<i32>(kBucketCount * (centroid[axis] - axisMin) / axisLen);
        bucketIndex = std::clamp(bucketIndex, 0, static_cast<i32>(kBucketCount - 1));
        buckets.at(bucketIndex).indices.push_back(node.indices.at(triangleIndex));
        buckets.at(bucketIndex).indices.push_back(node.indices.at(triangleIndex+1));
        buckets.at(bucketIndex).indices.push_back(node.indices.at(triangleIndex+2));
        buckets.at(bucketIndex).bbox.Expand(centroid);
    }
    return buckets;
}

/* Returns the SAH from the given index of an given Buckets.*/
f32 MeshContainer::calculateSurfaceAreaHeuristic(std::vector<MeshNode>& nodes, const i32 index) {
    std::vector<MeshNode> nodesA{};
    std::vector<MeshNode> nodesB{};
    f32 triangleCountA = 0.0f;
    f32 triangleCountB = 0.0f;

    //TODO: Maybe a cleanly way to write this with iterators 
    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes.at(i).indices.size() != 0) {
            if (i < static_cast<size_t>(index)) {
                nodesA.emplace_back(nodes.at(i));
                triangleCountA += (static_cast<f32>(nodes.at(i).indices.size()) / 3.0f);
            } else {
                nodesB.emplace_back(nodes.at(i));
                triangleCountB += (static_cast<f32>(nodes.at(i).indices.size()) / 3.0f);
            }
        }
    }
    // calculate cost function 
    const f32 t_transval = 1.0f;
    const f32 t_intersect = 2.0f;

    f32 volumeA = 0.0f;
    for (const auto& node : nodesA) {
        auto extend = node.bbox.Min() - node.bbox.Max();
        volumeA += 2.0f * (extend.x * extend.y +  extend.y * extend.z + extend.z * extend.x);
    }

    f32 volumeB = 0.0f;
    for (const auto& node : nodesB) {
        auto extend = node.bbox.Min() - node.bbox.Max();
        volumeB += 2.0f * (extend.x * extend.y +  extend.y * extend.z + extend.z * extend.x);
    }

    f32 volumeSource = 0.0f;
    for (const auto& node : nodes) {
        auto extend = node.bbox.Min() - node.bbox.Max();
        volumeSource += 2.0f * (extend.x * extend.y +  extend.y * extend.z + extend.z * extend.x);
    }

    f32 pA = volumeA / volumeSource;
    f32 pB = volumeB / volumeSource;

    f32 cost = t_transval + (pA * (t_intersect*triangleCountA)) + (pB * (t_intersect*triangleCountB));
    return cost;
};

/* Combines a vector of MeshNodes into one MeshNode */
MeshNode MeshContainer::combineMeshNodes(std::vector<MeshNode> nodes) {
    MeshNode output{};
    for (auto& node : nodes) {
        for (auto& index : node.indices) {
            Vertex v = m_pMesh->GetVertices().at(index);
            output.bbox.Expand(v.position);
            output.indices.emplace_back(index);
        }
    }
    return output;
}

std::pair<MeshNode, MeshNode> MeshContainer::splitNode(MeshNode& node) {
    if (m_algorithm == Algorithm::eObjectMedian){
        BBox centroidBBox = calculateCentroidBBox(node);
        auto axisPrioOrder = axisLengthOrder(centroidBBox);
        MeshNode nodeA{}; MeshNode nodeB{};
        u32 attempts = 0;
        for (auto& axis : axisPrioOrder) {
            attempts++;
            const auto originalIndices = node.indices;
            transferIndicesToChildNodes(splitBBoxOnAxis(centroidBBox, axis), node, nodeA, nodeB);
            if ((nodeA.indices.empty() || nodeB.indices.empty()) && attempts < axisLengthOrder(centroidBBox).size()) {
                /* Copy the indices back and try again on a different axis */
                node.indices = originalIndices;
                continue;
            }

            if ((nodeA.indices.empty() || nodeB.indices.empty()) && attempts >= axisLengthOrder(centroidBBox).size()) {
                node.indices = originalIndices;
                return std::pair<MeshNode, MeshNode>(MeshNode(), MeshNode()); /* Return Emtpy */
            }
            return std::pair<MeshNode, MeshNode>(nodeA, nodeB); /* No issues */
        }
        return std::pair<MeshNode, MeshNode>(MeshNode(), MeshNode()); /* Return Emtpy */
    } else if (m_algorithm == Algorithm::eSurfaceAreaHeuristic) {
        /* Split the axis in n buckets */
        u32 attempts = 0;
        BBox centroidBBox = calculateCentroidBBox(node);
        for (const auto& axis : axisLengthOrder(centroidBBox)) {
            attempts++;
            const auto originalIndices = node.indices;
            auto buckets = splitBBoxIntoBucketsOnAxis(node, axis);
            /* loop over buckets and run the cost function of each spilt line */
            f32 bestCost;
            i32 bestIndex = -1;
            for (i32 index = 1; index < kBucketCount - 1; index++){
                if (index == 1) {
                    bestCost = calculateSurfaceAreaHeuristic(buckets, index);
                    bestIndex = index;
                } else {
                    /* record the best cost function results */
                    auto cost = calculateSurfaceAreaHeuristic(buckets, index);
                    if (cost < bestCost) {
                        bestCost = cost;
                        bestIndex = index;
                    }
                }
            }
            /* Create the two MeshNodes */
            MeshNode nodeA{}; MeshNode nodeB{};
            std::vector<MeshNode> groupA{}; std::vector<MeshNode> groupB{};
            if (bestIndex != -1) {
                //TODO: Maybe a cleanly way to write this with iterators 
                for (size_t i = 0; i < buckets.size(); i++) {
                    if (i < static_cast<size_t>(bestIndex)) {
                        groupA.emplace_back(buckets.at(i));
                    } else {
                        groupB.emplace_back(buckets.at(i));
                    }
                }
                nodeA = combineMeshNodes(groupA);
                nodeB = combineMeshNodes(groupB);

                if ((nodeA.indices.empty() || nodeB.indices.empty()) && attempts < axisLengthOrder(centroidBBox).size()) {
                    /* Copy the indices back and try again on a different axis */
                    node.indices = originalIndices;
                    continue;
                }

                if ((nodeA.indices.empty() || nodeB.indices.empty()) && attempts >= axisLengthOrder(centroidBBox).size()) {
                    node.indices = originalIndices;
                    return std::pair<MeshNode, MeshNode>(MeshNode(), MeshNode()); /* Return Emtpy */
                }

                return std::pair<MeshNode, MeshNode>(nodeA, nodeB);
            }
        }
    }
}

void MeshContainer::BuildBVH() {
    // Build the Root Node
    BBox rootBBox = m_pMesh->bbox();
    auto& rootVertices = m_pMesh->GetVertices();
    auto rootIndices = m_pMesh->GetIndices();
    m_nodes.push_back(MeshNode{
        .bbox = rootBBox,
        .leftIndex = -1,
        .rightIndex = -1,
        .indices = rootIndices});

    auto buildTree = [&](auto& self, u32 nodeIndex) {
        u32 nodeTriangleCount = m_nodes[nodeIndex].indices.size() / 3;
        if (nodeTriangleCount <= m_trianglesPerNode){
            return;
        }
        m_depthCount++;

/* Debug Print for Checking the Spilt In-Progress*/
#if DEBUG_PRINT
        i32 indicesCount = m_nodes[nodeIndex].indices.size();
        std::printf("Depth: %i | Triangle Count: %i\n", m_depthCount, indicesCount/3);
#endif

        auto result = splitNode(m_nodes[nodeIndex]);

        if (result.first.indices.empty() || result.second.indices.empty()) {
            m_depthCount--;
            return;
        }

        m_nodes[nodeIndex].leftIndex = m_nodes.size();
        m_nodes.push_back(result.first);

        m_nodes[nodeIndex].rightIndex = m_nodes.size();
        m_nodes.push_back(result.second);
        
        self(self, m_nodes[nodeIndex].leftIndex);
        self(self, m_nodes[nodeIndex].rightIndex);
        m_depthCount--;
    };

    std::printf("Starting BVH Tree Build...\n");
    StatusMessage::Set("Tracer::BVH::MeshContainer: Building BVH.");
    buildTree(buildTree, 0);
    StatusMessage::Set("Tracer::BVH::MeshContainer: Finished BVH Build.");

    std::printf("Final BVH Node Count: %i\n", static_cast<i32>(m_nodes.size()));

#if DEBUG_PRINT
    std::printf("Outputing: Final Node and Triangle Count Per Node\n");
    i32 nodeCountWithIndices = 0;
    for (auto node : m_nodes) {
        if (!node.indices.empty()) {
            nodeCountWithIndices++;
        std::printf("Node %i, Triangle Count: %i\n", nodeCountWithIndices, node.indices.size() / 3);
        }
    }
#endif

};

TestableContainer MeshContainer::FindAllHitNodes(const Ray& ray) const {
    std::map<f32, const MeshNode*> found{};
    auto findNodes = [&](auto& self,const Ray& ray, const MeshNode* node) -> void {
        if (node->leftIndex == -1 && node->rightIndex == -1) {
            found.emplace(node->bbox.distance(ray), node);
            return;
        }
        
        if (node->leftIndex != -1) {
            if (m_nodes.at(node->leftIndex).bbox.isHit(ray)) {
                self(self, ray, &m_nodes.at(node->leftIndex));
            }
        }

        if (node->rightIndex != -1) {
            if (m_nodes.at(node->rightIndex).bbox.isHit(ray)) {
                self(self, ray, &m_nodes.at(node->rightIndex));
            }
        }
    };

    const MeshNode* root = &m_nodes.front();

    if (m_nodes.front().bbox.isHit(ray)) {
        findNodes(findNodes, ray, root);
    }
    return TestableContainer(found);
}

void MeshContainer::SetTrianglesPerNode(u32 triangleCount) {
    m_trianglesPerNode = triangleCount;
};

/* TestableContainer */

TestableContainer::TestableContainer(std::map<f32, const MeshNode*> foundNodes) : m_found(foundNodes) {
    m_distanceThreshold = std::numeric_limits<f32>::max();
    m_rng = std::mt19937(std::random_device{}());
    m_nodeCount = foundNodes.size();
}

 /* Returns an random MeshNode to test.*/
const MeshNode* TestableContainer::next() {
    if (m_testedCount != 0) {
        m_found.erase(m_currentDistance);
    }

    cutoff();

    if (m_found.empty()) {
        return {};
    }

    //std::uniform_int_distribution<size_t> dist(0, m_found.size() - 1);
    auto it = m_found.begin();
    //std::advance(it, dist(m_rng));

    m_testedCount++;

    m_currentDistance = it->first;
    return it->second;
} 
/* Returns true if there is nothing else left to test.*/
const bool TestableContainer::finished() {
    return m_found.empty();
}

/* If the current node did hit, record that distance */
void TestableContainer::record() {
    m_distanceThreshold = m_currentDistance;
}

void TestableContainer::cutoff() {
    auto cutoff = m_found.upper_bound(m_distanceThreshold);
    m_found.erase(cutoff, m_found.end());
}

}
}