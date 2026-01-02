#pragma once

/*
@name Tracer::Engine

@brief Performs and manages ray tracing tasks within a pool of
    rendering threads.

*/

namespace Tracer {
class Engine {
public:
    Engine();
    ~Engine() = default;

    void setScene();
    void setCamera();
    void setImage();

    void startRendering();
    void stopRendering();

private:
    bool m_isRunning = {false};
    /* Camera */
    /* Scene */
    /* Image */
};

}