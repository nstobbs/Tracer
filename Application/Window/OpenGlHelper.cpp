#include "Window/OpenGlHelper.hpp"

#include <iostream>

char* OpenGlHelper::GetShaderLogs(GLuint shader) {
    if (glIsShader(shader)) {
        int infoLength = 0;
        int maxLength = infoLength;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        char* infoLog = new char[maxLength];
        glGetShaderInfoLog(shader, maxLength, &infoLength, infoLog);
        if (infoLength > 0) {
            return infoLog;
        }
    }
    return nullptr;
};

char* OpenGlHelper::GetProgramLogs(GLuint program) {
    if (glIsProgram(program)) {
        int infoLength = 0;
        int maxLength = infoLength;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        char* infoLog = new char[maxLength];
        glGetProgramInfoLog(program, maxLength, &infoLength, infoLog);
        if (infoLength > 0) {
            return infoLog;
        }
    }
    return nullptr;
};

GLuint OpenGlHelper::CreateVertexShader(const std::string& source) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertexSource = source.c_str();
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    GLint vertexShaderCompiled = GL_FALSE;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompiled);
    if (vertexShaderCompiled != GL_TRUE) {
        const char* vertexLogs = OpenGlHelper::GetShaderLogs(vertexShader);
        std::printf("{Error} Failed to Compile Vertex Shader: %s\n", vertexLogs);
        return -1;
    }
    return vertexShader;
}

GLuint OpenGlHelper::CreateFragmentShader(const std::string& source) {
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragSource = source.c_str();
    glShaderSource(fragShader, 1, &fragSource, NULL);
    glCompileShader(fragShader);
    GLint fragmentShaderCompiled = GL_FALSE;
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &fragmentShaderCompiled);
    if (fragmentShaderCompiled != GL_TRUE) {
        const char* fragLogs = OpenGlHelper::GetShaderLogs(fragShader);
        std::printf("{Error} Failed to Compile Vertex Shader: %s\n", fragLogs);
        return -1;
    }
    return fragShader;
}

GLuint OpenGlHelper::CreateProgram(GLuint vert, GLuint frag) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    GLint programLinked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &programLinked);
    if (programLinked != GL_TRUE) {
        const char* programLog = OpenGlHelper::GetProgramLogs(program);
        std::printf("{Error} Failed to Link Shaders to Program: %s\n", programLog);
        return -1;
    }
    return program;
}