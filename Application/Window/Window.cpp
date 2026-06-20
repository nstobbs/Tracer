#include "Window/Window.hpp"

#include "Core/StatusMessage.hpp"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>

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
    //f32 windowScaling = ImGui_ImplSDL2_GetContentScaleForDisplay(0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Enable DoubleBuffer
    m_window = SDL_CreateWindow(kWindowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                static_cast<i32>(m_width),//* windowScaling),
                                static_cast<i32>(m_height),//* windowScaling),
                                SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
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

    /* Init ImGui */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    //auto& style = ImGui::GetStyle();
    //style.ScaleAllSizes(windowScaling);
    //style.FontScaleDpi = windowScaling;

    ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
    ImGui_ImplOpenGL3_Init("#version 130");

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

    /* ImGui Rendering */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    /* Engine Panel */
    ImGui::Begin("Tracer::Engine Panel");

    // Start/Stop Buttons
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
    if (ImGui::Button("Start")) {
        m_engine->StartRendering();
    }
    ImGui::PopStyleColor(1);

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    if (ImGui::Button("Stop")) {
        m_engine->StopRendering();
    }
    ImGui::PopStyleColor(1);

    // Render Progress Bar
    auto progress = 1.0f - m_engine->getRenderProgress();
    ImGui::PushItemWidth(200);
    ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("Render Progress");

    // Engine Settings 
    i32 samples = static_cast<i32>(m_engine->getSamplesPerPixel());
    i32 rayDepth = static_cast<i32>(m_engine->getMaxRayDepth());
    i32 tileSize = static_cast<i32>(m_engine->getTileSize());
    Color4 missedColor = m_engine->getMissedColor();
    
    auto oldMissedColor = missedColor;
    ImGui::PushItemWidth(200);
    ImGui::ColorEdit4("Missed Hit Color / Background Color", &missedColor.r);
    if (missedColor != oldMissedColor) {
        m_engine->SetMissedColor(missedColor);
    }

    auto oldSamples = samples;
    ImGui::PushItemWidth(100);
    ImGui::InputInt("Samples Per Pixel", &samples);
    if (samples != oldSamples) {
        m_engine->SetSamplesPerPixel(static_cast<u32>(samples));
    }

    auto oldRayDepth = rayDepth;
    ImGui::PushItemWidth(100);
    ImGui::InputInt("Max Ray Depth", &rayDepth);
    if (rayDepth != oldRayDepth) {
        m_engine->SetMaxRayDepth(static_cast<u32>(rayDepth));
    }

    auto oldTileSize = tileSize;
    ImGui::PushItemWidth(100);
    ImGui::InputInt("Tile Size", &tileSize);
    if (tileSize != oldTileSize) {
        m_engine->SetTileSize(static_cast<u32>(tileSize));
    }

    // StatusMessage 
    auto info = "Status: " + Tracer::StatusMessage::Get();
    ImGui::Text(info.c_str());

    // Write Image
    ImGui::SetNextItemWidth(300.0f);
    ImGui::InputText(": Filepath", &m_ImageOutFilepath);
    ImGui::SameLine();
    if (ImGui::Button("WriteToDisk")) {
        if (m_engine->isRendering()) {
            StatusMessage::Set("Can't Write Image while Tracer::Engine is rendering.\n");
        } else {
            auto message = "Writing Image to: " + m_ImageOutFilepath + "\n";
            StatusMessage::Set(message);
            auto* image = m_engine->GetTargetImage();
            image->WriteImage(m_ImageOutFilepath);
        }
    }

    ImGui::End();
    
    /*Window Overlays Panel*/
    ImGui::Begin("Window Overlays Panel");
    auto enableBBox = m_bboxDisplayPass->isEnabled();
    auto enableCrossHair = m_tileCrosshairPass->isEnabled();
    
    // Enable DisplayBoundingBox
    auto oldEnableBBox = enableBBox;
    ImGui::Checkbox("Toggle Display BoundingBox", &enableBBox);
    if (enableBBox != oldEnableBBox) {
        if (enableBBox) {
            m_bboxDisplayPass->enable();
        } else {
            m_bboxDisplayPass->disable();
        }
    }

    // Skip Empty Bounding Boxs
    auto skipEmpty = m_bboxDisplayPass->isSkipEmptyNodesEnabled();
    auto oldSkipEmpty = skipEmpty;
    ImGui::SameLine();
    ImGui::Checkbox("Skip Empty Bounding Boxs ", &skipEmpty);
    if (skipEmpty != oldSkipEmpty) {
        m_bboxDisplayPass->setSkipEmptyNodes(skipEmpty);
    }

    // Brightness
    auto bboxBrightness = m_bboxDisplayPass->getBrightness();
    auto oldBboxBrightness = bboxBrightness;
    ImGui::PushItemWidth(200);
    ImGui::SliderFloat("BoundingBox Brightness", &bboxBrightness, 0.0f, 1.0f);
    if (bboxBrightness != oldBboxBrightness) {
        m_bboxDisplayPass->setBrightness(bboxBrightness);
    }

    // Line Thickness
    auto bboxThickness = m_bboxDisplayPass->getThickness();
    auto oldBboxThickness = bboxThickness;
    ImGui::PushItemWidth(200);
    ImGui::SliderFloat("BoundingBox Thickness", &bboxThickness, 0.0f, 10.0f);
    if (bboxThickness != oldBboxThickness) {
        m_bboxDisplayPass->setThickness(bboxThickness);
    }

    // Enable TileCrossHairs
    auto oldEnableCrossHair = enableCrossHair;
    ImGui::Checkbox("Toggle Display Tile Crosshair", &enableCrossHair);
    if (enableCrossHair != oldEnableCrossHair) {
        if (enableCrossHair) {
            m_tileCrosshairPass->enable();
        } else {
            m_tileCrosshairPass->disable();
        }
    }

    ImGui::End();
    
    /* Demo Panel */
    bool openDemoPanel = true;
    ImGui::ShowDemoWindow(&openDemoPanel);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    SDL_GL_SwapWindow(m_window);
}

void Window::processEvent(const SDL_Event* event) {
    ImGui_ImplSDL2_ProcessEvent(event);
}

Window::~Window() {
    for (auto pass : m_renderPasses) {
        pass->cleanup();
    }

    /* ImGui Shutdown */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
}