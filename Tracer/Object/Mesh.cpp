#include "Object/Mesh.hpp"

#include "Core/StatusMessage.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <array>
#include <cmath>

namespace {
    const Tracer::f32 kThreshold = 0.000000000001f;
    Tracer::i32 highestNodeCount = 0;
}

namespace Tracer {

Mesh::Mesh() {
    m_model = BVH::MeshContainerModel(BVH::Algorithm::eSurfaceAreaHeuristic, this);
}

bool Mesh::isHit(const Ray& ray, HitInfo& hitInfo, Interval interval) {
    auto localRay = m_transform.transformRay(ray);
    
    /* Find All of the Triangles to Render from the BVH */
    auto meshContainerView = m_model.createMeshContainerView(localRay);

    while (!meshContainerView.finished()) {
        const auto* node = meshContainerView.next();

        if(!node) {
            continue;
        }

        if (node->indices.empty()) {
            continue;
        }

        assert(node->indices.size() % 3 == 0);
        u64 triangleCount = node->indices.size() / 3;
        for (u64 i = 0; i < triangleCount; i++) {
            /* Get Triangle Vertices */
            const u64 triangleIndex = 3 * i;
            Vertex v0 = m_vertices.at(node->indices.at(triangleIndex));
            Vertex v1 = m_vertices.at(node->indices.at(triangleIndex+1));
            Vertex v2 = m_vertices.at(node->indices.at(triangleIndex+2));

            /* Generate Geometric Normals */
            Vector3 edge0 = v1.position - v0.position;
            Vector3 edge1 = v2.position - v0.position;
            Vector3 triNormal = glm::normalize(glm::cross(edge0, edge1));

            /* Check if the Ray is Parallel */
            if (std::fabs(glm::dot(triNormal, localRay.direction)) <= kThreshold) {
                continue;
            }
           
            /* Calculate the Distance of the Ray and the intersection of the triangles plane  */
            f32 distance = glm::dot(triNormal, (v0.position - localRay.origin)) /
                                        glm::dot(triNormal, localRay.direction);
            if (distance < 0.0f) {
                continue;
            }

            if (!interval.Contains(distance)) {
                continue;
            }

            /* Check if we already have a closer hit recorded */
            if (hitInfo.hasHit && distance >= hitInfo.distance) {
                continue;
            }

            /* Calculate Hit Position on Triangle Plane */
            Point3 hitPosition = localRay.origin + localRay.direction * static_cast<f32>(distance);
    
            /* Calculate the barycentric coordinates for that given point.*/
            Vector3 e0 = v1.position - v0.position;
            Vector3 e1 = v2.position - v0.position;
            Vector3 e2 = hitPosition - v0.position;

            f32 d00 = glm::dot(e0, e0);
            f32 d01 = glm::dot(e0, e1);
            f32 d11 = glm::dot(e1, e1);
            f32 d20 = glm::dot(e2, e0);
            f32 d21 = glm::dot(e2, e1);

            /* Check for Degenerate Triangles */
            f32 denominator = d00 * d11 - d01 * d01;
            if (std::fabs(denominator) <= kThreshold) {
                continue;
            }

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

            /* Record Hit Information */
            hitInfo.object = static_cast<Object*>(this);
            hitInfo.hasHit = true;
            hitInfo.position = hitPosition;
            hitInfo.distance = distance;
            hitInfo.normal = triNormal;
            hitInfo.isFrontFace = (glm::dot(hitInfo.normal, localRay.direction) < 0.0f);

            /* Append Extra Shape Info to HitInfo */
            hitInfo.type = ShapeType::eTriangle;
            hitInfo.extra.triangle.u = w0;
            hitInfo.extra.triangle.v = w1;
            hitInfo.extra.triangle.w = w2;
            hitInfo.extra.triangle.v0 = v0;
            hitInfo.extra.triangle.v1 = v1;
            hitInfo.extra.triangle.v2 = v2;

            auto localInfo = m_transform.transformHitInfo(hitInfo, ray);
            hitInfo = localInfo;
            meshContainerView.record();
        }
    }

#if 0 /* Report the TestableContainer Tested Count */
    i32 testedCount = static_cast<i32>(meshContainerView.testedCount());
    std::printf("MeshContainerView: %i MeshNodes were tested\n", testedCount);
#endif
    return hitInfo.hasHit;
};

Mesh Mesh::TriangleMesh() {
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
    
    /* CounterClockWise Winding*/
    /* Red Vertex */
    A.position = Point3(0.0f, 1.0f, 2.0f);
    A.normals = Vector3(0.0f, 0.0f, 1.0f);
    A.color = Color4(1.0f, 0.0f, 0.0f, 1.0f);

    /* Blue Vertex */
    B.position = Point3(-0.75f, 0.0f, 2.0f);
    B.normals = Vector3(0.0f, 0.0f, 1.0f);
    B.color = Color4(0.0f, 0.0f, 1.0f, 1.0f);

    /* Green Vertex */
    C.position = Point3(0.75f, 0.0f, 2.0f);
    C.normals = Vector3(0.0f, 0.0f, 1.0f);
    C.color = Color4(0.0f, 1.0f, 0.0f, 1.0f);
    
    vertices.push_back(A);
    vertices.push_back(C);
    vertices.push_back(B);

    for (auto& v : vertices) {
        triangleMesh.m_bbox.Expand(v.position);
    }

    triangleMesh.m_vertices = vertices;

    std::vector<u64> indices = {0, 1, 2};
    triangleMesh.m_indices = indices;
    
    triangleMesh.m_model.SetTrianglesPerNode(1);
    triangleMesh.m_model.BuildBVH(); /* One Triangle */

    return triangleMesh;
};

Mesh Mesh::RetangleMesh() {
    Mesh rectangleMesh;
    VertexInfo info {
        .hasPosition = true,
        .hasNormals = true,
        .hasTextureUVs = false, // TODO: Add UVs
        .hasColor = false };
    rectangleMesh.m_info = info;

    Vertex A;
    Vertex B;
    Vertex C;
    Vertex D;
    std::vector<Vertex> vertices;
    
    /* Bottom Left */
    A.position = Point3(-1.0f, -1.0f, 2.0f);
    A.normals = Vector3(0.0f, 0.0f, 1.0f);

    /* Bottom Right */
    B.position = Point3(1.0f, -1.0f, 2.0f);
    B.normals = Vector3(0.0f, 0.0f, 1.0f);

    /* Top Right */
    C.position = Point3(1.0f, 1.0f, 2.0f);
    C.normals = Vector3(0.0f, 0.0f, 1.0f);

    /* Top Left*/
    D.position = Point3(-1.0f, 1.0f, 2.0f);
    D.normals = Vector3(0.0f, 0.0f, 1.0f);
    
    vertices.push_back(A);
    vertices.push_back(B);
    vertices.push_back(C);
    vertices.push_back(D);
    

    for (auto& v : vertices) {
        rectangleMesh.m_bbox.Expand(v.position);
    }

    rectangleMesh.m_vertices = vertices;

    std::vector<u64> indices = {0, 2, 1, 0, 3, 2};
    rectangleMesh.m_indices = indices;

    rectangleMesh.m_model.SetTrianglesPerNode(7);
    rectangleMesh.m_model.BuildBVH();

    return rectangleMesh;
};

Mesh Mesh::SphereMesh() {
    Mesh sphere = ReadFile("./Models/Sphere2.obj").back();
    return sphere;
};

std::vector<Mesh> Mesh::ReadFile(const std::string& filepath) {
    StatusMessage::Set("Tracer::Mesh: Reading File");
    std::vector<Mesh> outputScene;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate);

    if (!scene) {
        std::printf("Mesh.cpp: Failed to Read Model: %s\n", importer.GetErrorString());
        StatusMessage::Set("Tracer::Mesh: Failed to Read File " + std::string(importer.GetErrorString()));
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

    /* Read Mesh Data */
    for (auto node : nodesWithMeshes) {
        auto meshCount = node->mNumMeshes;
        for (i32 i = 0; i < meshCount; i++) {
            Mesh meshObject;
            VertexInfo meshInfo{};

            auto meshIndex = node->mMeshes[i];
            auto mesh = scene->mMeshes[meshIndex];

            /* Fill Vertex Information */
            meshInfo.hasPosition = mesh->HasPositions();
            meshInfo.hasNormals = mesh->HasNormals();
            meshInfo.hasTextureUVs = mesh->HasTextureCoords(0);
            meshObject.m_info = meshInfo;

            /* Loop over each face and store the vertex and index */
            auto triangleCount = mesh->mNumFaces;
            for (u64 triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++) {
                std::array<Vertex, 3> triangleVertices{};
                const auto face = mesh->mFaces[triangleIndex];

                for (auto x = 0; x < 3; x++) {
                    if (meshInfo.hasPosition) {
                        auto position = mesh->mVertices[face.mIndices[x]];
                        triangleVertices[x].position = Point3(position.x, position.y, position.z);
                        meshObject.m_bbox.Expand(triangleVertices[x].position);
                    }

                    if (meshInfo.hasNormals) {
                        auto normals = mesh->mNormals[face.mIndices[x]];
                        triangleVertices[x].normals = Vector3(normals.x, normals.y, normals.z);
                    }
                    
                    /* FIXME: Add Support for More UV Channels.
                    Currently we only take the first UV Channel.*/
                    if (meshInfo.hasTextureUVs) {
                        auto uv = mesh->mTextureCoords[0][face.mIndices[x]];
                        triangleVertices[x].textureUV = Point2(uv.x, uv.y);
                    }
                }

                for (auto iter = triangleVertices.begin(); iter != triangleVertices.end(); iter++) {
                    meshObject.m_vertices.push_back(*iter);
                }

                u64 base = meshObject.m_indices.size();
                meshObject.m_indices.push_back(base+0);
                meshObject.m_indices.push_back(base+1);
                meshObject.m_indices.push_back(base+2);
                
            }
            // Calculate TrianglesPerNode Based of Index Count and Node Max Limit
            auto targetSize = meshObject.m_indices.size() / 128; //FIXME: Get the Node Limit from the MeshContainer...
            meshObject.m_model.SetTrianglesPerNode(7);
            meshObject.m_model.BuildBVH();
            outputScene.push_back(meshObject);
        }
    }
    StatusMessage::Set("Tracer::Mesh: Finished Reading File");
    return outputScene;
};

}
