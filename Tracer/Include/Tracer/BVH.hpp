#pragma once
#include "Tracer/BBox.hpp"
#include "Tracer/Ray.hpp"
#include "Tracer/Mesh.hpp"
#include "Tracer/Scene.hpp"

#include <vector>

/* Bounding Volume Hierarchy
These templated Container and Node uses two args:
    Object,
    SubObjects.

    The Object Represents an Root Object Containing all of the SubObjects
    of that Object. Like How a Mesh is the Object and the vertices would be the
    SubObject of the Mesh.
    A Container then can be used to determine intersection of different SubObjects within a
    Given Object using Tracer::Ray.
    Like: A Tracer::Scene with different Tracer::Objects;
    Like: A Tracer::Mesh with Tracer::Vertex;
    Like: A Tracer::Volume with Tracer::Vortex: *TBD
 */

namespace Tracer {
    namespace BVH {

template<typename SubObject>
struct Node {
    BBox bbox;
    Node* next = {nullptr};
    std::vector<SubObject> subObject;
};

template<typename Object, typename SubObject>
class Container {
public:
    Container(Object* object) : m_rootObject(object) { };
    ~Container() = default;

    void BuildBVH(u32 subObjectsPerNode) = delete;
    Node<SubObject>* FindNodeWithSubObjects(const Ray& ray) const;

    u64 GetVersion() const { return m_version; }

private:
    Object* m_rootObject = {nullptr};
    Node<SubObject>* m_rootNode = {nullptr};
    u64 m_version = {0};

    std::vector<Node<SubObject>> m_nodes;
};

using MeshContainer = Container<Mesh, u64>;

};
};