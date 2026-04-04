#pragma once

#include "Window/RenderPass.hpp"
#include "Window/OpenGlHelper.hpp"

#include <array>

class DisplayLayerRenderPass : public RenderPass {
public:
	DisplayLayerRenderPass(i32 width, i32 height) : m_width(width), m_height(height) { ; }
    bool init() override;
    void render(RenderContext& context) override;
    void cleanup() override;

private:
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