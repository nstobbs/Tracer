#include "Application/Application.hpp"
#include "Tracer/Mesh.hpp"

#include <iostream>

namespace {
    const bool kHalfRes = false;

    constexpr int kWindowWidth = kHalfRes ? 640 / 2 : 640;
    constexpr int kWindowHeight = kHalfRes ? 480 / 2 : 480;
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

#if 0

    auto surface = Tracer::SurfaceShader::PreviewNormals();
    auto meshes = Tracer::Mesh::ReadFile("./Models/cube.obj");
    for (auto& mesh : meshes) {
        mesh.SetSurface(&surface);
        m_scene->AddObject(static_cast<Tracer::Object*>(&mesh));
    }
    
#else
    auto surface = Tracer::SurfaceShader::VertexColor();
    auto mesh = Tracer::Mesh::TriangleMesh();
    mesh.SetSurface(&surface);
    m_scene->AddObject(static_cast<Tracer::Object*>(&mesh));

#endif

    m_scene->AddSurface(&surface);
    
    m_engine->SetScene(m_scene.get());
    m_engine->SetCamera(m_camera.get());
    
    /* Image Layer Setup */
    std::string renderLayer = "Color";

    m_image = std::make_unique<Tracer::Image>(kWindowWidth, kWindowHeight);
    m_image->CreateLayer(renderLayer);
    m_image->GetLayer(renderLayer)->FloodColor(Tracer::Color4(0.0f));

    m_engine->SetImage(m_image.get());
    m_engine->SetTargetLayer(renderLayer);
    m_engine->SetSamplesPerPixel(32);
    m_engine->StartRendering();

    /* Main Loop */
    SDL_Event event;
    while (!m_shutdown) {
        if (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                m_engine->StopRendering();
                m_shutdown = true;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_UP) {
                    m_camera->MoveCamera(1.0f, Tracer::CameraDirection::eForward);
                } else if (event.key.keysym.sym == SDLK_DOWN) {
                    m_camera->MoveCamera(1.0f, Tracer::CameraDirection::eBackward);
                } else if (event.key.keysym.sym == SDLK_LEFT) {
                    m_camera->MoveCamera(1.0f, Tracer::CameraDirection::eLeft);
                } else if (event.key.keysym.sym == SDLK_RIGHT) {
                    m_camera->MoveCamera(1.0f, Tracer::CameraDirection::eRight);
                }
            }
        };

        /* Rendering Stuff Here!*/
        m_engine->Tick();
        PresentLayerToWindow(m_image->GetLayer(renderLayer), m_window);
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