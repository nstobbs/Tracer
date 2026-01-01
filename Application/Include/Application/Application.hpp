#pragma once

#include "Tracer/Types.hpp"
#include "Tracer/Engine.hpp"

class Application {
public:
    Application();
    ~Application();

private:
    Tracer::SharedPtr<Tracer::Engine> m_engine;
};