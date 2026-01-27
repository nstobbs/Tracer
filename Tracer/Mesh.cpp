#include "Tracer/Mesh.hpp"

namespace {
    const Tracer::f64 kThreshold = 0.01;
}

namespace Tracer {

bool Mesh::isHit(const Ray& ray, HitInfo& hitInfo, Interval interval, Camera camera) {
    /* Apply Camera View Model */ 
    Matrix4 view = camera.GetViewModel();

    /* Per Each Triangle Of this Mesh */
    u64 vertexCount = m_indices.size();
    for (u64 i = 0; i < vertexCount; i+3) {
        Vertex v0 = multiply(view, m_vertices.at(i));
        Vertex v1 = multiply(view, m_vertices.at(i+1));
        Vertex v2 = multiply(view, m_vertices.at(i+2));

        /* Check if the  Ray is Parallel Lines */
        if (glm::dot(ray.direction, v0.normals) <= kThreshold) {
            return false;
        }

        hitInfo.normal = v0.normals;

        /* Check where on the Plane of The Normals and Vertex A for the Intersection Of the Ray */
        Vector3 planar = v0.position - ray.direction;
        hitInfo.distance = glm::dot(planar, v0.normals);
        if (hitInfo.distance <= 0.0) {
            return false;
        }

        /* Planar Intersection Point */
        hitInfo.position = ray.origin + ray.direction * static_cast<f32>(hitInfo.distance);
        /* ReWritten Notes */
        /* 1. Calculate the Area of the Triangle: Cross product of v0 and v1.
           2. Calculate the Area of the Triangle of Hit - v1 and v2 - v1.
           3. Using the Area of the Original Triangle and our current Triangle. Find out much of the area of the current triangle
           takes up of the original triangle as a percentage. 
         */
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
        f32 w2 = (d00 * d21 - d01 * d20) / denominator;
        f32 w0 = 1 - w1 - w2;

        /* Calculate the barycentric coordinates for that given point.  */
        Vector3 C;
        f64 u, v, w;

        Vector3 v1p = hitInfo.position - v1.position;
        Vector3 v1v2 = v2.position - v1.position;
        C = glm::cross(v1v2, v1p);
        if ((u = glm::dot(v0.normals, C)) < 0.0) {
            return false;
        }

        Vector3 v2p = hitInfo.position - v2.position;
        Vector3 v2v0 = v0.position - v2.position;
        C = glm::cross(v2v0, v2p);
        if ((v = glm::dot(v0.normals, C)) < 0.0) {
            return false;
        }

        Vector3 v0p = hitInfo.position - v0.position;
        Vector3 v0v1 = v1.position - v0.position;
        C = glm::cross(v0v1, v0p);
        if ((w = glm::dot(v0.normals, C)) < 0.0) {
            return false;
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

        return true;
    }

    return false;
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

}
