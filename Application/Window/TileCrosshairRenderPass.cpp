#include "Window/TileCrosshairRenderPass.hpp"

#include <vector>

bool TileCrosshairRenderPass::init() {
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

    m_inViewLocation = glGetUniformLocation(m_shaderProgram, "inView");
    if (m_inViewLocation == -1) {
        std::printf("{Error} Couldn't find inView in Vertex Shader.\n");
        return false;
    }

    /* Vertex Array Buffer */
    glGenVertexArrays(1, &m_vertexArrayBuffer);
    glBindVertexArray(m_vertexArrayBuffer);

    std::array<GLfloat, 16> vertices = {
        /* Top Down */
        -0.05f, 0.25f,
        -0.05f,-0.25f,
         0.05f,-0.25f,
         0.05f, 0.25f,

         /* Left Right */
        -0.25f, 0.05f,
        -0.25f,-0.05f,
         0.25f,-0.05f,
         0.25f, 0.05f
    };
    /*                       Faces:     0      1      2      3 */
    std::array<GLuint, 12> indices = {0,1,3, 2,3,1, 4,5,7, 6,7,5};

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    return true;
}

void TileCrosshairRenderPass::render(RenderContext& context) {
    /* Execute Gl Program */
    glUseProgram(m_shaderProgram);
    glBindVertexArray(m_vertexArrayBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glVertexAttribPointer(m_inPositionLocation, 2,
                         GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
    glEnableVertexAttribArray(m_inPositionLocation);

    /* Render Crosshair Per Tile */
    i32 tileSize;
    std::vector<std::pair<i32, i32>> tiles;
    {
        std::unique_lock lock(m_activeList->mutex);
        tileSize = m_activeList->tileSize;
        for (auto tile : m_activeList->active) {
            tiles.push_back(tile.second);
        }
    }

    i32 height = context.layer->GetRowCount();
    i32 width = context.layer->GetRow(0).size();

    Matrix4 view = Matrix4(1.0f);
    f32 aspectRatio = static_cast<f32>(height) / static_cast<f32>(width);
    Vector3 scale = Vector3(m_handleScale * aspectRatio, m_handleScale, 1.0f);

    for (auto tile : tiles) {
        i32 x = tile.first;
        i32 y = tile.second;

        f32 ndcX = (static_cast<f32>(x) / static_cast<f32>(width)) * 2.0f - 1.0f;
        f32 ndcY = 1.0f - (static_cast<f32>(y) / static_cast<f32>(height)) * 2.0f;

        /* Top Left */
        auto topLeftView = glm::translate(view, Vector3(ndcX, ndcY, 0.0f));
        topLeftView = glm::scale(topLeftView, scale);
        glUniformMatrix4fv(m_inViewLocation, 1, GL_FALSE, glm::value_ptr(topLeftView));
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, NULL);

        /* Top Right */
        ndcX = (static_cast<f32>(x + tileSize) / static_cast<f32>(width)) * 2.0f - 1.0f;
        auto topRightView = glm::translate(view, Vector3(ndcX, ndcY, 0.0f));
        topRightView = glm::scale(topRightView, scale);
        glUniformMatrix4fv(m_inViewLocation, 1, GL_FALSE, glm::value_ptr(topRightView));
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, NULL);

        /* Bottom Left */
        ndcX = (static_cast<f32>(x) / static_cast<f32>(width)) * 2.0f - 1.0f;
        ndcY = 1.0f - (static_cast<f32>(y + tileSize) / static_cast<f32>(height)) * 2.0f;
        auto bottomLeftView = glm::translate(view, Vector3(ndcX, ndcY, 0.0f));
        bottomLeftView = glm::scale(bottomLeftView, scale);
        glUniformMatrix4fv(m_inViewLocation, 1, GL_FALSE, glm::value_ptr(bottomLeftView));
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, NULL);

        /* Bottom Right */
        ndcX = (static_cast<f32>(x + tileSize) / static_cast<f32>(width)) * 2.0f - 1.0f;
        ndcY = 1.0f - (static_cast<f32>(y + tileSize) / static_cast<f32>(height)) * 2.0f;
        auto bottomRightView = glm::translate(view, Vector3(ndcX, ndcY, 0.0f));
        bottomRightView = glm::scale(bottomRightView, scale);
        glUniformMatrix4fv(m_inViewLocation, 1, GL_FALSE, glm::value_ptr(bottomRightView));
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, NULL);
    }
    
    glDisableVertexAttribArray(m_inPositionLocation);
    
    /* Cleanup Gl State */
    glUseProgram(NULL);
    glBindVertexArray(NULL);
}

void TileCrosshairRenderPass::cleanup() {
    glDeleteProgram(m_shaderProgram);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);

    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteBuffers(1, &m_elementBuffer);

    glDeleteVertexArrays(1, &m_vertexArrayBuffer);
}

/* Shader Source Code */
std::string TileCrosshairRenderPass::vertexSource() {
    std::string source = R"(
        #version 330 core
        layout (location = 0) in vec2 inPosition;
        uniform mat4 inView;

        void main() {
            gl_Position = inView * vec4(inPosition, 0.0f, 1.0);
        }
    )";
    return source;
}

std::string TileCrosshairRenderPass::fragmentSource() {
    std::string source = R"(
        #version 330 core
        out vec4 outColor;

        void main() {
            outColor = vec4(1.0f);
        }
    )";
    return source;
}