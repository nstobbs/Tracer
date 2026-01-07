#pragma once
#include "Tracer/Image.hpp"
#include "Tracer/Camera.hpp"
#include "Tracer/Scene.hpp"
#include "Tracer/Ray.hpp"

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

    void SetScene(Scene* scene);
    void SetCamera(Camera* camera);
    void SetImage(Image* image);
    void SetSamplesPerPixel(u32 numOfSamples);

    void StartRendering();
    void StopRendering();

private:
    Ray GetRay(u32 x, u32 y) const;
    Color4 GetRayColor(const Ray& ray, i32 maxDepth) const; //TODO: add scene
    void CalculatePixelColor(u32 x, u32 y) const;
    Vector3 SampleSquare() const;

    u32 m_samplesPerPixel = {1};
    bool m_isRunning = {false};

    Camera* m_camera = {nullptr};
    Scene* m_scene = {nullptr};
    Image* m_image = {nullptr};

};

}