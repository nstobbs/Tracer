#pragma once

#include "Core/Types.hpp"
#include "Core/Image.hpp"

#include "Engine/Engine.hpp"

#include "Window/OpenGlHelper.hpp"
#include "Window/RenderPass.hpp"

#include "Window/DisplayLayerRenderPass.hpp"
#include "Window/TileCrosshairRenderPass.hpp"
#include "Window/BBoxRenderPass.hpp"

#include <array>

using namespace Tracer;

class Window {
public:
	Window(i32 width, i32 height);
	~Window();

	void addRenderPass(RenderPass* renderPass);

	void resizeWindow(i32 width, i32 height);
	void setTarget(Layer* layer, Camera* camera);
	void renderWindow();
	void processEvent(const SDL_Event* event);

	void setEngine(Engine* engine) { m_engine = engine; }

	TileCrosshairRenderPass* getTileCrossHairHUD() const { return m_tileCrosshairPass.get(); }
	BBoxRenderPass* getBBoxDisplayHUD() const { return m_bboxDisplayPass.get(); }
	ActiveTilesRecord* getActiveTilesRecord() const;

private:
	bool createGLResources();

	/* SDL Resources */
	SDL_Window* m_window;
	SDL_GLContext m_glContext;

	std::vector<RenderPass*> m_renderPasses;
	UniquePtr<DisplayLayerRenderPass> m_displayLayerPass;
	UniquePtr<TileCrosshairRenderPass> m_tileCrosshairPass;
	UniquePtr<ActiveTilesRecord> m_activeTilesRecord;
	UniquePtr<BBoxRenderPass> m_bboxDisplayPass;
	Layer* m_displayLayer = {nullptr};
	Camera* m_tracerCamera = {nullptr};

	Engine* m_engine = {nullptr};

	/* Window Info */
	i32 m_width;
	i32 m_height;
};