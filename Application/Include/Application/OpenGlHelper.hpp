#pragma once

#include <SDL.h>
#include "gl/glew.h"
#include <SDL_opengl.h>
#include <GL/glu.h>

class OpenGlHelper {
public:
    static char* GetShaderLogs(GLuint shader);
    static char* GetProgramLogs(GLuint program);
private:
};