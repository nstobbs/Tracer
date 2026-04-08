#include "Tracer/BoundingVolumeHierarchy.hpp"
#include "Tracer/Mesh.hpp"
#include "Tracer/Interval.hpp"

#include <iostream>
#include <algorithm>
#include <map>

namespace {
        void printNodeInfo(Tracer::BVH::MeshNode node, Tracer::i32 index) {
            std::printf("Node %I32i, Triangle Count: %i\n", index, node.indices.size() / 3);
            std::printf("leftIndex: %I32i, rightIndex: %I32i\n", node.leftIndex, node.rightIndex);
        };
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
    if (axis == 0) {
        max.x = (bbox.Min().x + bbox.Max().x) / 2.0f;
    } else if (axis == 1) {
        max.y = (bbox.Min().y + bbox.Max().y) / 2.0f;
    } else if (axis == 2) {
        max.y = (bbox.Min().z + bbox.Max().z) / 2.0f;
    }
    halfBBox.SetMax(max);
    return halfBBox;
};

std::pair<BBox, BBox> MeshContainer::splitBBox(BBox& bbox) {
    BBox a;
    BBox b;

    a.SetMax(bbox.Max());
    a.SetMin(bbox.Min());

    b.SetMax(bbox.Max());
    b.SetMin(bbox.Min());

    f32 mid = 0.0f;
    Point3 aMax = a.Max();
    Point3 bMin = b.Min();

    Point3 extents = bbox.Max() - bbox.Min();
    if (extents.x >= extents.y && extents.x >= extents.z) {
        mid = (bbox.Min().x + bbox.Max().x) / 2.0f;
        aMax.x = mid;
        bMin.x = mid;
    } else if (extents.y >= extents.z) {
        mid = (bbox.Min().y + bbox.Max().y) / 2.0f;
        aMax.y = mid;
        bMin.y = mid;
    } else {
        mid = (bbox.Min().z + bbox.Max().z) / 2.0f;
        aMax.z = mid;
        bMin.z = mid;
    }

    a.SetMax(aMax);
    b.SetMin(bMin);
    return std::pair<BBox, BBox>(a, b);
}

void MeshContainer::transferIndicesToChildNodes(BBox anchorPoint, MeshNode& from, MeshNode& toA, MeshNode& toB) {
    toA.indices = {}; toB.indices = {};
    toA.bbox = BBox(); toB.bbox = BBox();

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

std::pair<MeshNode, MeshNode> MeshContainer::splitNode(MeshNode& node) {
    auto axisPrioOrder = axisLengthOrder(node.bbox);
    MeshNode nodeA{}; MeshNode nodeB{};
    u32 attempts = 0;
    for (auto& axis : axisPrioOrder) {
        attempts++;
        auto anchorPoint = splitBBoxOnAxis(node.bbox, axis);
        transferIndicesToChildNodes(anchorPoint, node, nodeA, nodeB);
        if (nodeA.indices.empty() && attempts < 3) {
            /* Copy the indices back and try again on a different axis */
            node.indices = nodeB.indices; 
            continue;
        } else if (nodeB.indices.empty() && attempts < 3) {
            /* Copy the indices back and try again on a different axis */
            node.indices = nodeA.indices; 
            continue;
        }
        return std::pair<MeshNode, MeshNode>(nodeA, nodeB); /* No issues */
    }
    return std::pair<MeshNode, MeshNode>(nodeA, nodeB); /* Might be an issue */
}

void MeshContainer::BuildBVH() {
    // Build the Root Node
    BBox rootBBox = m_pMesh->GetBBox();
    auto& rootVertices = m_pMesh->GetVertices();
    auto rootIndices = m_pMesh->GetIndices();
    m_nodes.push_back(MeshNode{
        .bbox = rootBBox,
        .leftIndex = -1,
        .rightIndex = -1,
        .indices = rootIndices});

    auto buildTree = [&](auto& self, u32 nodeIndex) {
        u32 nodeTriangleCount = m_nodes[nodeIndex].indices.size() / 3;
        if (nodeTriangleCount <= m_trianglesPerNode ){
            return;
        }

        auto result = splitNode(m_nodes[nodeIndex]);

        if (result.first.indices.empty()) {
            m_nodes[nodeIndex] = result.second;
        } else if (result.second.indices.empty()) {
            m_nodes[nodeIndex] = result.first;
        }

        m_nodes[nodeIndex].leftIndex = m_nodes.size();
        m_nodes.push_back(result.first);

        m_nodes[nodeIndex].rightIndex = m_nodes.size();
        m_nodes.push_back(result.second);
        
        self(self, m_nodes[nodeIndex].leftIndex);
        self(self, m_nodes[nodeIndex].rightIndex);
    };

    std::printf("Starting BVH Tree Build...\n");
    buildTree(buildTree, 0);

    std::printf("Final BVH Node Count: %i\n", static_cast<i32>(m_nodes.size()));

#if 1
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