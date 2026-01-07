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
    auto mesh = Tracer::Mesh::ColorfulTriangle();

    /* Image ColorChannel */
    m_image = std::make_unique<Tracer::Image>(kWindowWidth, kWindowHeight);
    m_image->CreateChannel("ColorChannel");
    m_image->GetChannel("ColorChannel")->DrawTestPatten(Tracer::TestPatten::eUVRamp);
    //m_engine->SetImage(m_image.get());

    //m_engine->startRendering();

    /* Main Loop */
    SDL_Event event;
    while (!m_shutdown) {
        if (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                //m_engine->stopRendering();
                m_shutdown = true;
            }
        };

        /* Rendering Stuff Here!*/
        PresentChannelToWindow(m_image->GetChannel("ColorChannel"), m_window);
    }
};

Application::~Application() {
    std::printf("Shuting Down Application.\n");
    //m_engine->stopRendering();
    SDL_DestroyRenderer(m_windowRenderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
};

void Application::PresentChannelToWindow(Tracer::Channel* channel, SDL_Window* window) {
    if (!channel) {
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

    auto height = channel->GetRowCount();
    for (int y = 0; y < height; y++) {
        auto row = channel->GetRow(y);
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