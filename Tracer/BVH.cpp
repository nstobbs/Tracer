#include "Tracer/BVH.hpp"
#include "Tracer/Mesh.hpp"
#include "Tracer/Interval.hpp"

namespace Tracer {
    namespace BVH {


/* Mesh Vertex BuildBVH*/
template<>
void Container<Mesh, u64>::BuildBVH(u32 subObjectsPerNode) {
    // Build the Root Node
    BBox* rootBBox = m_rootObject->GetBBox();
    auto& rootVertices = m_rootObject->GetVertices();
    auto rootIndices = m_rootObject->GetIndices();
    m_nodes.push_back(Node<u64>{
        .bbox = *rootBBox,
        .next = nullptr,
        .subObject = rootIndices});
    m_rootNode = &m_nodes.at(0);

    // If node subObject count is greater than target
    // Split node into two.
    auto split = [&](auto&& self, Node<u64>* node){
        if (node) {
            return nullptr; 
        }

        if (node.subObject.size() =< subObjectsPerNode) {
            return nullptr;
        }
        // Split the bounding box into two.
        // Check each Vertex and place one of the two bbox
        
        Node<u64>* left = nullptr;
        Node<u64>* right = nullptr;

        self(self, left);
        self(self, right);

        return nullptr;
    };
    // Recursively keep spliting the left node. 
    // If node subObject count is less than target
    // Append to List 
};

template<typename Object, typename SubObject>
Node<SubObject>* Container<Object, SubObject>::FindNodeWithSubObjects(const Ray& ray) const {
    if (!m_rootNode->bbox.isHit(ray)) {
        return nullptr;
    }

    auto findSubObject = [&](auto&& self, Node<SubObject>* node){
        if(!node) {
            return nullptr;
        };

        if (node.next) {
            return node.next->bbox.isHit(ray) ? node.next : nullptr;
        }

        return self(self, node.next);
    };
    return findSubObject(findSubObject, m_rootNode->next);
};

}
}