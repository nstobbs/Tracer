#pragma once

#include "Window/RenderPass.hpp"
#include "Window/OpenGlHelper.hpp"

#include "Object/BoundingBox.hpp"
#include "Tracer/Scene.hpp"

#include <array>
#include <unordered_map>
#include <mutex>

using namespace Tracer;

struct BBoxHitHeatmapRecord {
	std::unordered_map<Tracer::BBox, i32> heatmap;
	std::mutex mutex;
};

enum class OverlayMode {
	eInvalid = -1,
	eBoundingBoxWireframe = 0,
	eBoundingBoxHeatmap = 1
};

class BBoxRenderPass : public RenderPass {
public:
    bool init() override;
    void render(RenderContext& context) override;
    void cleanup() override;

	void setScene(Scene* scene);

private:
	std::vector<Matrix4> m_bboxMatrices;
	Scene* m_scene;
	u64 m_lastSceneVersion;

    /* GL Resources */
	std::string fragmentSource();
	std::string vertexSource();

	GLuint m_fragmentShader;
	GLuint m_vertexShader;
	GLuint m_shaderProgram;

	GLuint m_vertexArrayBuffer;
	GLuint m_vertexUnitCubeBuffer;
	GLuint m_instanceMatricesBuffer;

	GLuint m_inPositionLocation;
	GLuint m_inInstanceLocation;
	GLuint m_inViewLocation;
	GLuint m_inProjectionLocation;
};