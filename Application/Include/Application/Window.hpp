#pragma once

#include "Tracer/Types.hpp"
#include "Tracer/Image.hpp"

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/glu.h>

#include <SDL_image.h>
#include <SDL2/SDL_video.h>

#include <array>

using namespace Tracer;

class Window {
public:
	Window(i32 width, i32 height);
	~Window();

	void resizeWindow(i32 width, i32 height);
	void displayLayerToWindow(Tracer::Layer* layer);
	void presentWindow();
private:
	bool createGLResources();

	/* SDL Resources */
	SDL_Window* m_window;
	SDL_GLContext m_glContext;

	/* GL Resources */
	std::string fragmentSource();
	std::string vertexSource();

	GLuint m_fragmentShader;
	GLuint m_vertexShader;
	GLuint m_shaderProgram;

	GLuint m_vertexArrayBuffer;
	GLuint m_vertexBuffer;
	GLuint m_elementBuffer;

	GLuint m_displayTexture;

	GLuint m_inPositionLocation;
	GLuint m_inTextureUVLocation;

	std::array<GLuint, 2> m_pixelBuffers{};
	i32 m_swapIndex = {0};

	/* Window Info */
	i32 m_width;
	i32 m_height;

	size_t imageBufferSize;
};