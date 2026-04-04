#include "Window/DisplayLayerRenderPass.hpp"

bool DisplayLayerRenderPass::init() {
    /* Create and Compile Shaders */
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

    m_inTextureUVLocation = glGetAttribLocation(m_shaderProgram, "inUV");
    if (m_inTextureUVLocation == -1) {
        std::printf("{Error} Couldn't find inUV in Vertex Shader.\n");
        return false;
    }

    /* Vertex Array Buffer */
    glGenVertexArrays(1, &m_vertexArrayBuffer);
    glBindVertexArray(m_vertexArrayBuffer);

    /* Vertex and Index Buffer */
    std::array<GLfloat, 16> vertices = {
        // Position  // UV
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f };

    std::array<GLuint, 6> indices = {0, 1, 2, 0, 2, 3};

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    /* Pixel Buffer and Texture  */
    imageBufferSize = m_width * m_height * 3 * sizeof(GLfloat);
    glGenTextures(1, &m_displayTexture);
    glBindTexture(GL_TEXTURE_2D, m_displayTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_width, m_height, 0,
                GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenBuffers(2, m_pixelBuffers.data());
    for (i32 i = 0; i < 2; i++) {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pixelBuffers[i]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, imageBufferSize, nullptr, GL_STREAM_DRAW);
    }
    return true;
}

void DisplayLayerRenderPass::render(RenderContext& context) {
    /* Copy Layer to Device */
    i32 upload = m_swapIndex;
    i32 map = (m_swapIndex + 1) % 2;
    glBindTexture(GL_TEXTURE_2D, m_displayTexture);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pixelBuffers[upload]);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pixelBuffers[map]);
    void* ptr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, imageBufferSize,
                                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    if (ptr) {
        //FIXME: Change the memory layout for Layer Class to make
        // Copying to Device faster.
        std::vector<Color3> outImage;
        for (i32 y = 0; y < m_height; y++) {
            auto& row = context.layer->GetRow(y);
            for (i32 x = 0; x < m_width; x++) {
                outImage.push_back(Color3(row.at(x).r, row.at(x).g, row.at(x).b));
            }
        }
        memcpy(ptr, outImage.data(), imageBufferSize);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    m_swapIndex = (m_swapIndex + 1) % 2;

    /* Execute Gl Program */
    glUseProgram(m_shaderProgram);

    GLuint samplerLocation = glGetUniformLocation(m_shaderProgram, "inImage");
    glUniform1i(samplerLocation, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_displayTexture);

    glBindVertexArray(m_vertexArrayBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glVertexAttribPointer(m_inPositionLocation, 2,
                         GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
    glVertexAttribPointer(m_inTextureUVLocation, 2,
                         GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(m_inPositionLocation);
    glEnableVertexAttribArray(m_inTextureUVLocation);

    glBindTexture(GL_TEXTURE_2D, m_displayTexture);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

    glDisableVertexAttribArray(m_inPositionLocation);
    glDisableVertexAttribArray(m_inTextureUVLocation);

    glUseProgram(NULL);
    glBindVertexArray(NULL);
}

void DisplayLayerRenderPass::cleanup() {
    glDeleteProgram(m_shaderProgram);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);

    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteBuffers(1, &m_elementBuffer);

    glDeleteVertexArrays(1, &m_vertexArrayBuffer);

    glDeleteTextures(1, &m_displayTexture);

    for (auto& buffer : m_pixelBuffers) {
        glDeleteBuffers(1, &buffer);
    }
}

/* Shader Source Code */
std::string DisplayLayerRenderPass::vertexSource() {
    std::string source = R"(
        #version 330 core
        layout (location = 0) in vec2 inPosition;
        layout (location = 1) in vec2 inUV;
        out vec2 fragUV;

        void main() {
            gl_Position = vec4(inPosition, 0.0f, 1.0);
            fragUV = vec2(inUV.x, -inUV.y);
        }
    )";
    return source;
}

std::string DisplayLayerRenderPass::fragmentSource() {
    std::string source = R"(
        #version 330 core
        uniform sampler2D inImage;
        in vec2 fragUV;
        out vec4 outColor;

        void main() {
            outColor = vec4(texture(inImage, fragUV).rgb, 1.0f);
        }
    )";
    return source;
}