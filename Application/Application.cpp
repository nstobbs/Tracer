#include "Application/Application.hpp"
#include "Tracer/Mesh.hpp"

#include <iostream>

namespace {
    const int kWindowWidth = 640;
    const int kWindowHeight = 480;
    const std::string kWindowTitle = "Tracer MainWindow";
}

Application::Application() {
    /* Application Init */
    std::printf("Starting Application.\n");
    if (SDL_Init(SDL_INIT_VIDEO) != 0 && IMG_Init(IMG_INIT_PNG) != 0) {
        std::printf("{Error} SDL Failed to Init Video or Image: %s\n", SDL_GetError());
    }
    m_window = SDL_CreateWindow(kWindowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                kWindowWidth, kWindowHeight, SDL_WINDOW_SHOWN);
    if (!m_window) {
        std::printf("{Error} SDL Failed to Create Window: %s\n", SDL_GetError());
    }
    m_windowRenderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
    if (!m_windowRenderer) {
        std::printf("{Error} SDL Failed to Create Window Renderer: %s\n", SDL_GetError());
    }

    /* Tracer Engine & Scene Setup */
    m_engine = std::make_unique<Tracer::Engine>();
    m_scene = std::make_unique<Tracer::Scene>();
    m_camera = std::make_unique<Tracer::Camera>();
    auto mesh = Tracer::Mesh::ColorfulTriangle();
    auto surface = Tracer::VertexColor();
    mesh.SetSurface(&surface);

    m_scene->AddObject(static_cast<Tracer::Object*>(&mesh));
    m_scene->AddSurface(&surface);
    
    m_engine->SetScene(m_scene.get());
    m_engine->SetCamera(m_camera.get());
    
    /* Image  */
    auto layerColor = "ColorLayer";

    m_image = std::make_unique<Tracer::Image>(kWindowWidth, kWindowHeight);
    m_image->CreateLayer(layerColor);
    m_image->GetLayer(layerColor)->FloodColor(Tracer::Color4(0.0f));

    m_engine->SetImage(m_image.get());
    m_engine->SetTargetLayer(layerColor);

    m_engine->StartRendering();

    /* Main Loop */
    SDL_Event event;
    while (!m_shutdown) {
        if (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                m_engine->StopRendering();
                m_shutdown = true;
            }
        };

        /* Rendering Stuff Here!*/
        m_engine->Tick();
        PresentLayerToWindow(m_image->GetLayer(layerColor), m_window);
    }
};

Application::~Application() {
    std::printf("Shuting Down Application.\n");
    m_engine->StopRendering();
    m_shutdown = true;
    SDL_DestroyRenderer(m_windowRenderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
};

void Application::PresentLayerToWindow(Tracer::Layer* layer, SDL_Window* window) {
    if (!layer) {
        return;
    }

    SDL_Texture* texture = nullptr;
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    if (!surface) {
        std::printf("{Error} Failed to Get WindowSurface: %s\n", IMG_GetError());
    }

    SDL_LockSurface(surface);
    auto sPixel = static_cast<uint8_t*>(surface->pixels);
    auto sPitch = surface->pitch;
    auto sBytesPerPixel = surface->format->BytesPerPixel;

    auto height = layer->GetRowCount();
    for (int y = 0; y < height; y++) {
        auto row = layer->GetRow(y);
        auto width = row.size();
        for (int x = 0; x < width; x++) {
            auto pixel = row[x];
            sPixel[y*sPitch + x*sBytesPerPixel + 0] = static_cast<uint8_t>(255*pixel.b);
            sPixel[y*sPitch + x*sBytesPerPixel + 1] = static_cast<uint8_t>(255*pixel.g);
            sPixel[y*sPitch + x*sBytesPerPixel + 2] = static_cast<uint8_t>(255*pixel.r);
        }
    }
    SDL_UnlockSurface(surface);

    texture = SDL_CreateTextureFromSurface(m_windowRenderer, surface);
    if (!texture) {
        std::printf("{Error} Failed to Create Texture From Surface: %s\n", SDL_GetError());
    }
    SDL_FreeSurface(surface);

    SDL_RenderClear(m_windowRenderer);
    SDL_RenderCopy(m_windowRenderer, texture, nullptr, nullptr);
    SDL_RenderPresent(m_windowRenderer);

    SDL_DestroyTexture(texture);
};