#include "Application/Application.hpp"
#include "raylib.h"

#include "Tracer/Mesh.hpp"

#include <iostream>

namespace {
    const int kWindowWidth = 800;
    const int kWindowHeight = 450;
    const int kTargetFPS = 60;
}

Application::Application() {
    std::printf("Application Running!");
    const int kWindowWidth = 800;
    InitWindow(kWindowWidth, kWindowHeight, "Tracer - Application");
    SetTargetFPS(kTargetFPS);

    auto meshes = Tracer::Mesh::ReadFile("C:/assets/");

    m_engine->setCamera();
    m_engine->setScene();
    m_engine->setImage();
    m_engine->startRendering();

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Quite Nice That!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }
};

Application::~Application() {
    m_engine->stopRendering();
    CloseWindow();
}