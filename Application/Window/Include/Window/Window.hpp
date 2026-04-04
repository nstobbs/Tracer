#pragma once

#include "Tracer/Types.hpp"
#include "Tracer/Image.hpp"

#include "Window/OpenGlHelper.hpp"
#include "Window/RenderPass.hpp"

#include "Window/DisplayLayerRenderPass.hpp"
#include "Window/TileCrosshairRenderPass.hpp"

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
	Layer* m_displayLayer = {nullptr};
	Camera* m_tracerCamera = {nullptr};

	/* Window Info */
	i32 m_width;
	i32 m_height;
};