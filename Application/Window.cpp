#include "Application/Window.hpp"
#include "Application/OpenGlHelper.hpp"

#include <vector>

namespace {
    const std::string kWindowTitle = "Tracer MainWindow";
}

Window::Window(i32 width, i32 height) : m_width(width), m_height(height) {
    /* Init SDL */
    std::printf("Creating Window.\n");
    if (SDL_Init(SDL_INIT_VIDEO) != 0 && IMG_Init(IMG_INIT_PNG) != 0) {
        std::printf("{Error} SDL Failed to Init Video or Image: %s\n", SDL_GetError());
    }

    /* OpenGL Attributes */
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0) {
        std::printf("{Error} SDL Failed to Set OpenGL Attribute: %s\n", SDL_GetError());
    }

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) != 0) {
        std::printf("{Error} SDL Failed to Set OpenGL Attribute: %s\n", SDL_GetError());
    }
    
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2)  != 0) {
        std::printf("{Error} SDL Failed to Set OpenGL Attribute: %s\n", SDL_GetError());
    }

    /* Init SDL Window */
    m_window = SDL_CreateWindow(kWindowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        m_width, m_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (!m_window) {
        std::printf("{Error} SDL Failed to Create Window: %s\n", SDL_GetError());
    }
    
    /* Init GL, GlEW*/
    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        std::printf("{Error} SDL Failed to Create GL Context: %s\n", SDL_GetError());
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::printf("{Error} GLEW Failed to Init: %s\n", glewGetErrorString(glewError));
    }
    
    if (SDL_GL_SetSwapInterval(0) != 0) {
        std::printf("{Error} SDL Failed to Set VSync Setting: %s", SDL_GetError());
    }

    if (!createGLResources()) {
        std::printf("{Error} Failed to Allocate OpenGL Resources.\n");
    };
}

bool Window::createGLResources() {
    std::printf("Creating OpenGL Resources.\n");

    /* Create and Compile Shaders */
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    auto vertexSourceString = vertexSource();
    const GLchar* vertexSource = vertexSourceString.c_str();
    glShaderSource(m_vertexShader, 1, &vertexSource, NULL);
    glCompileShader(m_vertexShader);
    GLint vertexShaderCompiled = GL_FALSE;
    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &vertexShaderCompiled);
    if (vertexShaderCompiled != GL_TRUE) {
        const char* vertexLogs = OpenGlHelper::GetShaderLogs(m_vertexShader);
        std::printf("{Error} Failed to Compile Vertex Shader: %s\n", vertexLogs);
        return false;
    }

    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    auto fragmentSourceString = fragmentSource();
    const GLchar* fragmentSource = fragmentSourceString.c_str();
    glShaderSource(m_fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(m_fragmentShader);
    GLint fragmentShaderCompiled = GL_FALSE;
    glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompiled);
    if (fragmentShaderCompiled != GL_TRUE) {
        const char* fragmentLog = OpenGlHelper::GetShaderLogs(m_fragmentShader);
        std::printf("{Error} Failed to Compile Fragment Shader: %s\n", fragmentLog);
        return false;
    }

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);
    glLinkProgram(m_shaderProgram);
    GLint programLinked = GL_FALSE;
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &programLinked);
    if (programLinked != GL_TRUE) {
        const char* programLog = OpenGlHelper::GetProgramLogs(m_shaderProgram);
        std::printf("{Error} Failed to Link Shaders to Program: %s\n", programLog);
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

    /* Set Clear Colour  */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
};

void Window::resizeWindow(i32 width, i32 height) {

}

void Window::displayLayerToWindow(Tracer::Layer* layer) {
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
            auto& row = layer->GetRow(y);
            for (i32 x = 0; x < m_width; x++) {
                outImage.push_back(Color3(row.at(x).r, row.at(x).g, row.at(x).b));
            }
        }
        memcpy(ptr, outImage.data(), imageBufferSize);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    m_swapIndex = (m_swapIndex + 1) % 2;
}

void Window::presentWindow() {
    glClear(GL_COLOR_BUFFER_BIT);

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

    SDL_GL_SwapWindow(m_window);
}

Window::~Window() {
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

    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
}

/* Shader Source Code */
std::string Window::vertexSource() {
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

std::string Window::fragmentSource() {
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