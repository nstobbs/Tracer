#include "Tracer/Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cmath>

namespace {
    const Tracer::f64 kThreshold = 0.01;
}

namespace Tracer {

bool Mesh::isHit(const Ray& ray, HitInfo& hitInfo, Interval interval, Camera camera) {
    Matrix4 view = camera.GetViewModel();

    /* Per Each Triangle Of this Mesh */
    u64 indexCount = m_indices.size();
    assert(indexCount % 3 == 0);

    u64 triangleCount = indexCount / 3;
    for (u64 i = 0; i < triangleCount; i++) {

        /* Transform From World to Camera Space */
        Vertex v0 = multiply(view, m_vertices.at(i));
        Vertex v1 = multiply(view, m_vertices.at(i+1));
        Vertex v2 = multiply(view, m_vertices.at(i+2));

        /* Check if the  Ray is Parallel Lines */
        if (glm::dot(ray.direction, v0.normals) <= kThreshold) {
            continue;
        }

        hitInfo.normal = v0.normals;

        /* Check where on the Plane of The Normals and Vertex A for the Intersection Of the Ray */
        Vector3 planar = v0.position - ray.direction;
        hitInfo.distance = glm::dot(planar, v0.normals);
        if (hitInfo.distance <= 0.0) {
            continue;
        }

        /* Planar Intersection Point */
        hitInfo.position = ray.origin + ray.direction * static_cast<f32>(hitInfo.distance);
 
        /* Calculate the barycentric coordinates for that given point.  */
        Vector3 e0 = v1.position - v0.position;
        Vector3 e1 = v2.position - v0.position;
        Vector3 e2 = hitInfo.position - v0.position;

        f32 d00 = glm::dot(e0, e0);
        f32 d01 = glm::dot(e0, e1);
        f32 d11 = glm::dot(e1, e1);
        f32 d20 = glm::dot(e2, e0);
        f32 d21 = glm::dot(e2, e1);

        f32 denominator = d00 * d11 - d01 * d01;

        f32 w1 = (d11 * d20 - d01 * d21) / denominator;
        if (w1 < 0.0f) {
            continue;
        };

        f32 w2 = (d00 * d21 - d01 * d20) / denominator;
        if (w2 < 0.0f) {
            continue;
        };

        f32 w0 = 1.0f - w1 - w2;
        if (w0 < 0.0f) {
            continue;
        };

        f32 combined = abs(w1 + w2 + w0);
        if (combined >= (1.0f + kThreshold) && combined <= (1.0f - kThreshold)) {
            continue;
        }
        
        /* Append Extra Shape Info to HitInfo */
        Triangle* thisTriangle = new Triangle();
        ShapeGen extra;
        extra.pTriangle = thisTriangle;
        hitInfo.type = ShapeType::eTriangle;
        hitInfo.extra = extra;

        thisTriangle->u = w0;
        thisTriangle->v = w1;
        thisTriangle->w = w2;
        thisTriangle->v0 = v0;
        thisTriangle->v1 = v1;
        thisTriangle->v2 = v2;

        return true; /* Found a Triangle Hit */
    }

    return false; /* Didn't find a Triangle Hit */
};

Mesh Mesh::ColorfulTriangle() {
    Mesh triangleMesh;
    VertexInfo info {
        .hasPosition = true,
        .hasNormals = true,
        .hasTextureUVs = false,
        .hasColor = true };
    triangleMesh.m_info = info;

    Vertex A;
    Vertex B;
    Vertex C;
    std::vector<Vertex> vertices;

    A.position = Point3(0.0f, 1.0f, -2.5f);
    A.normals = Vector3(0.0f, 0.0f, -1.0f);
    A.color = Color4(1.0f, 0.0f, 0.0f, 1.0f);

    B.position = Point3(0.75f, 0.0f, -2.5f);
    A.normals = Vector3(0.0f, 0.0f, -1.0f);
    B.color = Color4(0.0f, 1.0f, 0.0f, 1.0f);

    C.position = Point3(-0.75f, 0.0f, -2.5f);
    A.normals = Vector3(0.0f, 0.0f, -1.0f);
    C.color = Color4(0.0f, 0.0f, 1.0f, 1.0f);
    
    vertices.push_back(A);
    vertices.push_back(B);
    vertices.push_back(C);

    triangleMesh.m_vertices = vertices;

    std::vector<u64> indices = {0, 1, 2};
    triangleMesh.m_indices = indices;
    
    //triangleMesh.m_position = Point3(0.0f, 0.0f, 0.0f);

    return triangleMesh;
};

std::vector<Mesh> Mesh::ReadFile(const std::string& filepath) {
    std::vector<Mesh> outputScene;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath,
                                    aiProcess_CalcTangentSpace  |
                                    aiProcess_Triangulate);

    if (scene == nullptr) {
        std::printf("Mesh.cpp: Failed to Read Model: %s\n", importer.GetErrorString());
    };

    /* Find each node that contains a mesh. */
    std::vector<aiNode*> nodesWithMeshes;
    auto findMesh = [&](auto&& self, aiNode* node) {
        if (!node)
            return nullptr;
        
        if (node->mNumChildren > 0) {
            for (i32 i = 0; i < node->mNumChildren; i++) {
                self(self, node->mChildren[i]);
            };
        }

        if (node->mNumMeshes > 0)
            nodesWithMeshes.push_back(node);
        
        return nullptr;
    };
    findMesh(findMesh, scene->mRootNode);

    /* Fill Vertex */
    VertexInfo info{};
    for (auto node : nodesWithMeshes) {
        auto meshCount = node->mNumMeshes;
        for (i32 i = 0; i < meshCount; i++) {
            Mesh meshObject;

            auto index = node->mMeshes[i];
            auto mesh = scene->mMeshes[index];

            if (mesh->HasPositions() && !info.hasPosition) {
                info.hasPosition = true;
            }

            if (mesh->HasNormals() && !info.hasNormals) {
                info.hasNormals = true;
            }
            
            auto uvCount = mesh->GetNumUVChannels();
            for (i32 j = 0; j < uvCount; j++) {
                if (mesh->HasTextureCoords(j) && !info.hasTextureUVs) {
                    info.hasTextureUVs = true;
                }
            };

            meshObject.m_info = info;

            auto vertexCount = mesh->mNumVertices;
            for (i32 v = 0; v < vertexCount; v++) {
                Vertex vertex;
                if (info.hasPosition) {
                    auto position = mesh->mVertices[v];
                    vertex.position = Point3(position.x, position.y, position.z);
                }

                if (info.hasNormals) {
                    auto normals = mesh->mNormals[v];
                    vertex.normals = Point3(normals.x, normals.y, normals.z);
                }

                if (info.hasTextureUVs) {
                    for (i32 a = 0; a < uvCount; a++) {
                        if (mesh->HasTextureCoords(a)) {
                            auto uv = mesh->mTextureCoords[v][a];
                            vertex.textureUV = Point2(uv.x, uv.y);
                        } else {
                            vertex.textureUV = Point2(0.0f, 0.0f);
                        }
                    }
                }

                meshObject.m_vertices.push_back(vertex);
                meshObject.m_indices.push_back(static_cast<u64>(v));
                outputScene.push_back(meshObject);
            }
        }
    }
    return outputScene;
};

}
