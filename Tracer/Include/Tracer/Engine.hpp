#pragma once

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