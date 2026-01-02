#pragma once

#include "Tracer/Object.hpp"
#include "Tracer/Vertex.hpp"

#include <vector>
#include <string>

namespace Tracer {

class Mesh : public Object {
public:
    Mesh(std::vector<Vertex>& vertices);
    ~Mesh() = default;

    static std::vector<Mesh> ReadFile(const std::string& filepath);

    bool isHit() override;

private:
    VertexInfo m_info;
    std::vector<Vertex> m_vertices;
    std::vector<u64> m_indices;
};

}