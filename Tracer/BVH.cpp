#include "Tracer/BVH.hpp"
#include "Tracer/Mesh.hpp"
#include "Tracer/Interval.hpp"

#include <utility>
#include <iostream>

namespace {
        const Tracer::u32 kMaxDepth = 128;
}

namespace Tracer {

    namespace BVH {

void MeshContainer::BuildBVH(u32 indicesPerNode) {
    // Build the Root Node
    BBox rootBBox = m_pMesh->GetBBox();
    auto& rootVertices = m_pMesh->GetVertices();
    auto rootIndices = m_pMesh->GetIndices();
    /* Stops us from reallocating during push_back and
     cause our pointers to be invalid within each node. */
    m_nodes.reserve(526);  
    m_nodes.push_back(MeshNode{
        .bbox = rootBBox,
        .leftIndex = -1,
        .rightIndex = -1,
        .indices = rootIndices});
    m_rootNodeIndex = 0; // FIXME: Don't really need this...

    u32 depthCounter = 1;

    // If node subObject count is greater than target
    // Split node into two.
    auto split = [&](auto&& self, MeshNode& node){
        if (node.indices.size() <= indicesPerNode) {
            return;
        }

        if (depthCounter == kMaxDepth) {
            return;
        };
        /* Use the depth to pick the
        axis to split on:
        0 = x
        1 = y
        2 = z */

        auto halfBBox = [&](BBox bbox, u32 axis, u32& depth) {
            depth++;
            BBox a;
            BBox b;

            a.SetMax(bbox.Max());
            a.SetMin(bbox.Min());
            
            b.SetMax(bbox.Max());
            b.SetMin(bbox.Min());

            f32 mid;
            Point3 aMax = a.Max();
            Point3 bMin = b.Min();
            // FIXME: This block could be a lot cleaner
            if (axis == 0) {
                auto sumMinMax = (bbox.Min().x + bbox.Max().x);
                if (sumMinMax != 0.0f){
                    mid = (sumMinMax / 2.0f);
                    aMax.x = mid;
                    bMin.x = mid;
                } else {
                    aMax.x = 0.0f;
                    bMin.x = 0.0f;
                }
            } else if (axis == 1) {
                auto sumMinMax = (bbox.Min().y + bbox.Max().y);
                if (sumMinMax != 0.0f){
                    mid = (sumMinMax / 2.0f);
                    aMax.y = mid;
                    bMin.y = mid;
                } else {
                    aMax.y = 0.0f;
                    bMin.y = 0.0f;
                }
            } else if (axis == 2) {
                auto sumMinMax = (bbox.Min().z + bbox.Max().z);
                if (sumMinMax != 0.0f){
                    mid = (sumMinMax / 2.0f);
                    aMax.z = mid;
                    bMin.z = mid;
                } else {
                    aMax.z = 0.0f;
                    bMin.z = 0.0f;
                }
            }
            a.SetMax(aMax);
            b.SetMin(bMin);
            return std::pair<BBox, BBox>(a, b);
        };

        u32 axis = depthCounter % 3;
        auto result = halfBBox(node.bbox, axis, depthCounter);
        
        MeshNode nodeA {
            .bbox = result.first,
            .leftIndex = -1,
            .rightIndex = -1,
            .indices = {}};


        MeshNode nodeB {
            .bbox = result.second,
            .leftIndex = -1,
            .rightIndex = -1,
            .indices = {}};

        u32 triangleCount = (node.indices.size() / 3);
        for (u32 i = 0; i < triangleCount; i++) {
            u32 triangleIndex = i * 3;
            Point3 v0 = rootVertices.at(node.indices.at(triangleIndex)).position;
            Point3 v1 = rootVertices.at(node.indices.at(triangleIndex+1)).position;
            Point3 v2 = rootVertices.at(node.indices.at(triangleIndex+2)).position;
            Point3 centroid = (v0 + v1 + v2) / 3.0f;

            if (nodeA.bbox.Contains(centroid)) {
                nodeA.indices.push_back(node.indices.at(triangleIndex));
                nodeA.indices.push_back(node.indices.at(triangleIndex+1));
                nodeA.indices.push_back(node.indices.at(triangleIndex+2));
                
                // Expand BBox for the Whole Triangle
                nodeA.bbox.Expand(v0);
                nodeA.bbox.Expand(v1);
                nodeA.bbox.Expand(v2);
            } else {
                nodeB.indices.push_back(node.indices.at(triangleIndex));
                nodeB.indices.push_back(node.indices.at(triangleIndex+1));
                nodeB.indices.push_back(node.indices.at(triangleIndex+2));
                
                // Expand BBox for the Whole Triangle
                nodeB.bbox.Expand(v0);
                nodeB.bbox.Expand(v1);
                nodeB.bbox.Expand(v2);
            }
        };

        node.indices.clear(); // Remove dup indices

        m_nodes.push_back(nodeA);
        node.leftIndex = m_nodes.size() - 1;
        self(self, m_nodes.at(node.leftIndex));

        m_nodes.push_back(nodeB);
        node.rightIndex = m_nodes.size() - 1;
        self(self, m_nodes.at(node.rightIndex));
    };

    std::printf("Building BVH.\n");
    split(split, m_nodes.at(0)); // Root node 
    std::printf("Finished Building BVH.\n");
};

MeshNode MeshContainer::FindNodeWithIndices(const Ray& ray) const {
    auto root = m_nodes.at(0);
    if (!root.bbox.isHit(ray)) {
        return MeshNode{
            .leftIndex = -1,
            .rightIndex = -1,
            .indices = {}
        };
    }

    auto findSubObject = [&](auto&& self, MeshNode node) -> MeshNode {
        if(node.leftIndex != -1) {
            if (m_nodes.at(node.leftIndex).bbox.isHit(ray)) {
                return self(self, m_nodes.at(node.leftIndex));
            }
        }

        if(node.rightIndex != -1) {
            if (m_nodes.at(node.rightIndex).bbox.isHit(ray)) {
                return self(self, m_nodes.at(node.rightIndex));
            }
        }
        return node;
    };

    if (root.leftIndex != -1) {
        return findSubObject(findSubObject, m_nodes.at(root.leftIndex));
    } else if (root.rightIndex != -1) {
        return findSubObject(findSubObject, m_nodes.at(root.rightIndex));
    }
    return root;
};

}
}