#pragma once

#include "Tracer/Types.hpp"
#include "Tracer/Engine.hpp"
#include "Tracer/Image.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL2/SDL_video.h>

class Application {
public:
    Application();
    ~Application();

private:
    /* Functions */
    void PresentLayerToWindow(Tracer::Layer* layer, SDL_Window* window);

    /* Tracer */
    Tracer::UniquePtr<Tracer::Engine> m_engine;
    Tracer::UniquePtr<Tracer::Image> m_image;

    /* SDL Platform */
    SDL_Window* m_window;
    SDL_Renderer* m_windowRenderer;

    bool m_shutdown = {false};
};