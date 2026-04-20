#include "Window/BBoxRenderPass.hpp"
#include "Object/Mesh.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

bool BBoxRenderPass::init() {
    auto vertexSourceString = vertexSource();
    m_vertexShader = OpenGlHelper::CreateVertexShader(vertexSourceString); 
    if (m_vertexShader == -1) {
        return false;
    }

    auto fragmentSourceString = fragmentSource();
    m_fragmentShader = OpenGlHelper::CreateFragmentShader(fragmentSourceString);
    if (m_fragmentShader == -1) {
        return false;
    }

    m_shaderProgram = OpenGlHelper::CreateProgram(m_vertexShader, m_fragmentShader);
    if (m_shaderProgram == -1) {
        return false;
    }

    /* Get Shader Uniform Locations  */
    m_inPositionLocation = glGetAttribLocation(m_shaderProgram, "inPosition");
    if (m_inPositionLocation == -1) {
        std::printf("{Error} Couldn't find inPosition in Vertex Shader.\n");
        return false;
    }

    m_inInstanceLocation = glGetAttribLocation(m_shaderProgram, "instanceModel");
    if (m_inInstanceLocation == -1) {
        std::printf("{Error} Couldn't find inView in Vertex Shader.\n");
        return false;
    }

    m_inViewLocation = glGetUniformLocation(m_shaderProgram, "view");
    if (m_inViewLocation == -1) {
        std::printf("{Error} Couldn't find inView in Vertex Shader.\n");
        return false;
    }

    m_inProjectionLocation = glGetUniformLocation(m_shaderProgram, "proj");
    if (m_inProjectionLocation == -1) {
        std::printf("{Error} Couldn't find inView in Vertex Shader.\n");
        return false;
    }

    /* Vertex Array Buffer */
    glGenVertexArrays(1, &m_vertexArrayBuffer);
    glBindVertexArray(m_vertexArrayBuffer);

    auto unitCubeVertices = std::array{
    // Bottom face
    -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,
     0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
    -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f,
    // Top face
    -0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f,
    // Vertical edges
    -0.5f,-0.5f,-0.5f, -0.5f, 0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
     0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
    -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f };
    
    /* Unit Cube Buffer */
    glGenBuffers(1, &m_vertexUnitCubeBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexUnitCubeBuffer);
    glBufferData(GL_ARRAY_BUFFER, unitCubeVertices.size() * sizeof(GLfloat),
                                  unitCubeVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_inPositionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
    glEnableVertexAttribArray(m_inPositionLocation);
    
    /* Instance Matrices Buffer */
    glGenBuffers(1, &m_instanceMatricesBuffer);
    return true;
}

void BBoxRenderPass::render(RenderContext& context) {
    glUseProgram(m_shaderProgram);
    glBindVertexArray(m_vertexArrayBuffer);

    /* Upload Camera */
    i32 height = context.layer->GetRowCount();
    i32 width = context.layer->GetRow(0).size();
    f32 aspectRatio = static_cast<f32>(width) / static_cast<f32>(height);

    Point3 position = context.camera->Position();
    Vector3 forward = context.camera->ForwardVector();
    Vector3 up = context.camera->UpVector();
    Matrix4 view = glm::lookAt(position, position+forward, up);
    glUniformMatrix4fv(m_inViewLocation, 1, GL_FALSE, glm::value_ptr(view));

    f32 focalLength = context.camera->GetFocalLength();
    f32 nearZ = 0.01f;
    f32 farZ = 1000.0f;
    f32 fov = 2.0f * atan(1.0f / focalLength);
    Matrix4 proj = glm::perspective(fov, aspectRatio, nearZ, farZ);
    proj[0][0] *= -1.0f;
    proj[0][0] *= 2.0f;
    proj[1][1] *= 2.0f;
    glUniformMatrix4fv(m_inProjectionLocation, 1, GL_FALSE, glm::value_ptr(proj));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArraysInstanced(GL_LINES, 0, 24, m_bboxMatrices.size());
    glDisable(GL_BLEND);
}

void BBoxRenderPass::cleanup() {
    glDeleteProgram(m_shaderProgram);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);

    glDeleteBuffers(1, &m_vertexUnitCubeBuffer);
    glDeleteBuffers(1, &m_instanceMatricesBuffer);

    glDeleteVertexArrays(1, &m_vertexArrayBuffer);
}

void BBoxRenderPass::setScene(Scene* scene) {
    constexpr bool skipEmptyNodes = false; // FIXME Move this into the class header!
    m_scene = scene;
    m_bboxMatrices.clear();
    for (auto object : m_scene->getObjects()) {
        auto nodes = static_cast<Mesh*>(object)->getMeshContainer()->AllNodes();
        for (auto node : nodes) {
            if (node.indices.empty() && skipEmptyNodes) {
                continue;
            }
            
            auto bbox = node.bbox;
            Vector3 center = (bbox.Min() + bbox.Max()) * 0.5f;
            Vector3 size = bbox.Max() - bbox.Min();
            Matrix4 model = glm::translate(Matrix4(1.0f), center)
                            * glm::scale(Matrix4(1.0f), size);
            m_bboxMatrices.push_back(model);
        }
    }

    glBindVertexArray(m_vertexArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceMatricesBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_bboxMatrices.size() * sizeof(Matrix4),
                 m_bboxMatrices.data(), GL_DYNAMIC_DRAW);
    for (i32 i = 0; i < 4; i++) {
        glEnableVertexAttribArray(m_inInstanceLocation + i);
        glVertexAttribPointer(m_inInstanceLocation + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4),
                              reinterpret_cast <void*>(i * sizeof(Vector4)));
        glVertexAttribDivisor(m_inInstanceLocation + i, 1);
    }

    glBindVertexArray(NULL);
}

/* Shader Source Code */
std::string BBoxRenderPass::vertexSource() {
    std::string source = R"(
        #version 330 core
        layout (location = 0) in vec3 inPosition;
        layout (location = 1) in mat4 instanceModel;

        uniform mat4 view;
        uniform mat4 proj;

        void main() {
            gl_Position = proj * view * instanceModel * vec4(inPosition, 1.0);
        }
    )";
    return source;
}

std::string BBoxRenderPass::fragmentSource() {
    std::string source = R"(
        #version 330 core
        out vec4 outColor;

        void main() {
            outColor = vec4(1.0f, 1.0f, 1.0f, 0.05f);
        }
    )";
    return source;
}