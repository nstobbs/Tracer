#include "Window/Window.hpp"

#include <vector>

namespace {
    const std::string kWindowTitle = "Tracer MainWindow";
}

Window::Window(i32 width, i32 height) : m_width(width), m_height(height) {
    /* Init SDL */
    std::printf("Creating Window.\n");
    if (SDL_Init(SDL_INIT_VIDEO) != 0 && IMG_Init(IMG_INIT_PNG) != 0) {
        std::printf("{Error} SDL Failed to Init Video or Image: %s\n", SDL_GetError());
    }

    /* OpenGL Attributes */
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0) {
        std::printf("{Error} SDL Failed to Set OpenGL Attribute: %s\n", SDL_GetError());
    }

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) != 0) {
        std::printf("{Error} SDL Failed to Set OpenGL Attribute: %s\n", SDL_GetError());
    }
    
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2)  != 0) {
        std::printf("{Error} SDL Failed to Set OpenGL Attribute: %s\n", SDL_GetError());
    }

    /* Init SDL Window */
    m_window = SDL_CreateWindow(kWindowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        m_width, m_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (!m_window) {
        std::printf("{Error} SDL Failed to Create Window: %s\n", SDL_GetError());
    }
    
    /* Init GL, GlEW*/
    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        std::printf("{Error} SDL Failed to Create GL Context: %s\n", SDL_GetError());
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::printf("{Error} GLEW Failed to Init: %s\n", glewGetErrorString(glewError));
    }
    
    if (SDL_GL_SetSwapInterval(0) != 0) {
        std::printf("{Error} SDL Failed to Set VSync Setting: %s", SDL_GetError());
    }

    /* Create Render Passes */
    m_displayLayerPass = std::make_unique<DisplayLayerRenderPass>(m_width, m_height);

    m_activeTilesRecord = std::make_unique<ActiveTilesRecord>();
    m_tileCrosshairPass = std::make_unique<TileCrosshairRenderPass>(m_activeTilesRecord.get());

    m_bboxDisplayPass = std::make_unique<BBoxRenderPass>();
    m_bboxDisplayPass->disable(); /* Disabled By Default */

    /* Define the Order to Render Each Pass */
    m_renderPasses.push_back(m_displayLayerPass.get());
    m_renderPasses.push_back(m_bboxDisplayPass.get());
    m_renderPasses.push_back(m_tileCrosshairPass.get());

    /* Allocate Resources  */
    if (!createGLResources()) {
        std::printf("{Error} Failed to Allocate OpenGL Resources.\n");
    };
}

bool Window::createGLResources() {
    std::printf("Creating OpenGL Resources.\n");

    /* Set Clear Colour  */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    for (auto pass : m_renderPasses) {
        if (!pass->init()) {
            return false;
        }
    }
    return true;
}

ActiveTilesRecord* Window::getActiveTilesRecord() const {
    return m_activeTilesRecord.get();
}

void Window::resizeWindow(i32 width, i32 height) {

}

void Window::setTarget(Layer* layer, Camera* camera) {
    m_displayLayer = layer;
    m_tracerCamera = camera;
}

void Window::renderWindow() {
    glClear(GL_COLOR_BUFFER_BIT);
    RenderContext context {
        .camera = m_tracerCamera,
        .layer = m_displayLayer
    };

    for (auto pass : m_renderPasses) {
        pass->process(context);
    }

    SDL_GL_SwapWindow(m_window);
}

Window::~Window() {
    for (auto pass : m_renderPasses) {
        pass->cleanup();
    }

    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
}