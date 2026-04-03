#pragma once

#include "Application/Window.hpp"

#include "Tracer/Camera.hpp"
#include "Tracer/Engine.hpp"
#include "Tracer/Image.hpp"
#include "Tracer/Types.hpp"

class ApplicationSettings {
public:
    ApplicationSettings(int argc, char** argv);
    ~ApplicationSettings() = default;

    const std::string& inputFile() const { return m_inputFile; }
    const Tracer::i32& width() const { return m_width; }
    const Tracer::i32& height() const { return m_height; }
    const Tracer::i32& bucketSize() const { return m_bucketSize; }
    const Tracer::i32& sampleCount() const { return m_sampleCount; }

private:
    std::string m_inputFile = {};

    Tracer::i32 m_width = {0};
    Tracer::i32 m_height = {0};

    Tracer::i32 m_bucketSize = {0};
    Tracer::i32 m_sampleCount = {0};
};

class Application {
public:
    Application(ApplicationSettings settings);
    ~Application();

private:
    Tracer::UniquePtr<Tracer::Engine> m_engine;
    Tracer::UniquePtr<Tracer::Image> m_image;
    Tracer::UniquePtr<Tracer::Scene> m_scene;
    Tracer::UniquePtr<Tracer::Camera> m_camera;

    Tracer::UniquePtr<Window> m_window;
    bool m_shutdown = {false};
};