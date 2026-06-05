#include "Object/BoundingVolumeHierarchy.hpp"
#include "Object/Mesh.hpp"
#include "Core/Interval.hpp"

#include "Core/StatusMessage.hpp"

#include <iostream>
#include <algorithm>
#include <map>

#define DEBUG_PRINT 1

namespace Tracer {

    namespace BVH {

std::array<u32, 3> MeshContainer::axisLengthOrder(BBox bbox) {
    /* Creates a Map of Axises and the Extent of that axis.
    Sorted in the order of largest extent first.      */
    Point3 extents = bbox.Max() - bbox.Min();
    std::multimap<f32, u32, std::greater<f32>> extentAxisMap{};
    for (u32 i = 0; i < 3; i++) {
        if (i == 0) {
            extentAxisMap.emplace(extents.x, i);
        } else if (i == 1) {
            extentAxisMap.emplace(extents.y, i);
        } else if (i == 2) {
            extentAxisMap.emplace(extents.z, i);
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

std::pair<MeshNode, MeshNode> MeshContainer::splitNode(MeshNode& node) {
    BBox centroidBBox = calculateCentroidBBox(node);
    auto axisPrioOrder = axisLengthOrder(centroidBBox);
    MeshNode nodeA{}; MeshNode nodeB{};
    u32 attempts = 0;
    for (auto& axis : axisPrioOrder) {
        attempts++;
        const auto originalIndices = node.indices;
        transferIndicesToChildNodes(splitBBoxOnAxis(centroidBBox, axis), node, nodeA, nodeB);
        if ((nodeA.indices.empty() || nodeB.indices.empty()) && attempts < 3) {
            /* Copy the indices back and try again on a different axis */
            node.indices = originalIndices;
            continue;
        }

        if ((nodeA.indices.empty() || nodeB.indices.empty()) && attempts >= 3) {
            node.indices = originalIndices;
            return std::pair<MeshNode, MeshNode>(MeshNode(), MeshNode()); /* Return Emtpy */
        }
        return std::pair<MeshNode, MeshNode>(nodeA, nodeB); /* No issues */
    }
    return std::pair<MeshNode, MeshNode>(MeshNode(), MeshNode()); /* Return Emtpy */
}

void MeshContainer::BuildBVH() {
    // Build the Root Node
    BBox rootBBox = m_pMesh->getBBox();
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

std::vector<MeshNode> MeshContainer::FindAllHitNodes(const Ray& ray) const {
    std::vector<MeshNode> results;
    auto findNodes = [&](auto& self,const Ray& ray, MeshNode node) -> void {
        if (node.leftIndex == -1 && node.rightIndex == -1) {
            results.push_back(node);
            return;
        }
        
        if (node.leftIndex != -1) {
            if (m_nodes.at(node.leftIndex).bbox.isHit(ray)) {
                self(self, ray, m_nodes.at(node.leftIndex));
            }
        }

        if (node.rightIndex != -1) {
            if (m_nodes.at(node.rightIndex).bbox.isHit(ray)) {
                self(self, ray, m_nodes.at(node.rightIndex));
            }
        }
    };

    if (m_nodes.front().bbox.isHit(ray)) {
        findNodes(findNodes, ray, m_nodes.front());
    }
    return results;
}

void MeshContainer::SetTrianglesPerNode(u32 triangleCount) {
    m_trianglesPerNode = triangleCount;
};

}
}