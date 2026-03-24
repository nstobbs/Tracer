#include "Tracer/BoundingVolumeHierarchy.hpp"
#include "Tracer/Mesh.hpp"
#include "Tracer/Interval.hpp"

#include <iostream>

namespace {
        const Tracer::u32 kMaxNodes = 128;
        void printNodeInfo(Tracer::BVH::MeshNode node, Tracer::i32 index) {
            std::printf("Node %I32i, Triangle Count: %i\n", index, node.indices.size() / 3);
            std::printf("leftIndex: %I32i, rightIndex: %I32i\n", node.leftIndex, node.rightIndex);
        };
}

namespace Tracer {

    namespace BVH {

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
    auto resultBBoxs = splitBBox(node.bbox);
    MeshNode nodeA{}; MeshNode nodeB{};
    transferIndicesToChildNodes(resultBBoxs.first, node, nodeA, nodeB);
    return std::pair<MeshNode, MeshNode>(nodeA, nodeB);
}

void MeshContainer::BuildBVH() {
    // Build the Root Node
    BBox rootBBox = m_pMesh->GetBBox();
    auto& rootVertices = m_pMesh->GetVertices();
    auto rootIndices = m_pMesh->GetIndices();
    m_nodes.reserve(kMaxNodes+1);  
    m_nodes.push_back(MeshNode{
        .bbox = rootBBox,
        .leftIndex = -1,
        .rightIndex = -1,
        .indices = rootIndices});

    auto buildTree = [&](auto& self, MeshNode& node) {
        u32 nodeTriangleCount = node.indices.size() / 3;
        if (nodeTriangleCount <= m_trianglesPerNode ){//|| m_nodes.size() >= kMaxNodes) {
            return;
        }
        auto result = splitNode(node);
        if (result.first.indices.empty()) {
            node = result.second;
            return;
        } else if (result.second.indices.empty()) {
            node = result.first;
            return;
        }

        u32 currentIndex = &node - m_nodes.data();

        node.leftIndex = m_nodes.size();
        m_nodes.push_back(result.first);
        printNodeInfo(m_nodes.at(node.leftIndex), node.leftIndex);
        self(self, m_nodes.at(node.leftIndex));

        m_nodes.at(currentIndex).rightIndex = m_nodes.size();
        m_nodes.push_back(result.second);
        printNodeInfo(m_nodes.at(m_nodes.at(currentIndex).rightIndex), m_nodes.at(currentIndex).rightIndex);
        self(self, m_nodes.at(m_nodes.at(currentIndex).rightIndex));
    };

    std::printf("Starting BVH Tree Build...\n");
    buildTree(buildTree, m_nodes.front());
    std::printf("Finished Building BVH.\n");
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