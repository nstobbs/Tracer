#pragma once

#include "Window/RenderPass.hpp"
#include "Window/OpenGlHelper.hpp"

#include "Object/BoundingBox.hpp"
#include "Core/Scene.hpp"

#include <array>
#include <unordered_map>
#include <mutex>

using namespace Tracer;

//struct BBoxHitHeatmapRecord {
//	std::unordered_map<Tracer::BBox, i32> heatmap;
//	std::mutex mutex;
//};

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

	f32 getBrightness() const { return m_brightness; }
	void setBrightness(f32 value) { m_brightness = value; }

	f32 getThickness() const { return m_thickness; }
	void setThickness(f32 value) { m_thickness = value; }

	void setSkipEmptyNodes(bool value);
	bool isSkipEmptyNodesEnabled() const { return m_skipEmptyNodes; }

private:
	std::vector<Matrix4> m_bboxMatrices;
	Scene* m_scene;
	u64 m_lastSceneVersion;

	f32 m_brightness = {0.05f};
	f32 m_thickness = {1.0f}; 
	bool m_skipEmptyNodes = {false};

    /* GL Resources */
	std::string fragmentSource();
	std::string vertexSource();

	GLuint m_fragmentShader;
	GLuint m_vertexShader;
	GLuint m_shaderProgram;

	GLuint m_vertexArrayBuffer;
	GLuint m_vertexUnitCubeBuffer;
	GLuint m_instanceMatricesBuffer;

	GLuint m_brightnessLocation;
	GLuint m_inPositionLocation;
	GLuint m_inInstanceLocation;
	GLuint m_inViewLocation;
	GLuint m_inProjectionLocation;
};