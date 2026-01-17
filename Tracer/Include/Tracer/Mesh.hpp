#pragma once

#include "Tracer/Object.hpp"
#include "Tracer/Vertex.hpp"

#include <vector>
#include <string>

namespace Tracer {

class Mesh : public Object {
public:
    Mesh() = default;
    Mesh(u64 vertexCount, void* verticesPtr);
    ~Mesh() = default;

    static std::vector<Mesh> ReadFile(const std::string& filepath); /* Create meshes from File */
    static Mesh ColorfulTriangle(); /* Create an Single Triangle */

    bool isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) override;

private:
    VertexInfo m_info;
    std::vector<Vertex> m_vertices;
    std::vector<u64> m_indices;
};

}