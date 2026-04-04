#pragma once

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/glu.h>

#include <SDL_image.h>
#include <SDL2/SDL_video.h>

#include <string>

class OpenGlHelper {
public:
    static char* GetShaderLogs(GLuint shader);
    static char* GetProgramLogs(GLuint program);
    static GLuint CreateVertexShader(const std::string& source);
    static GLuint CreateFragmentShader(const std::string& source);
    static GLuint CreateProgram(GLuint vert, GLuint frag);
private:
};