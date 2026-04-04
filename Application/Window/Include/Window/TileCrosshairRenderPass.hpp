#pragma once

#include "Window/OpenGlHelper.hpp"
#include "Window/RenderPass.hpp"

#include <array>
#include <unordered_map>
#include <mutex>

struct ActiveTilesRecord {
    std::unordered_map<u64, std::pair<i32, i32>> active;
    u64 lifetimeTileCount = {0};
    i32 tileSize = {0};
    std::mutex mutex;
};

class TileCrosshairRenderPass : public RenderPass {
public:
    TileCrosshairRenderPass(ActiveTilesRecord* activeList) : m_activeList(activeList) {;}
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

	GLuint m_inPositionLocation;
    GLuint m_inViewLocation;

    f32 m_handleScale = {0.05f};
    Color3 m_crosshairColor = {1.0f, 1.0f, 1.0f};
    bool m_enableDropShadow = {false};
    ActiveTilesRecord* m_activeList;
};