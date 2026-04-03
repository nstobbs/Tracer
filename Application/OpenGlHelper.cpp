#include "Application/OpenGlHelper.hpp"

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