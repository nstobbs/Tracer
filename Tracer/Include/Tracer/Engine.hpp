#pragma once

#include "Tracer/Image.hpp"
#include "Tracer/Camera.hpp"
#include "Tracer/Scene.hpp"
#include "Tracer/Ray.hpp"
#include "Tracer/Interval.hpp"
#include "Tracer/ThreadPool.hpp"
#include "Tracer/Tasker.hpp"

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
    void SetTargetLayer(const std::string& layer);

    void StartRendering();
    void StopRendering();

    void Tick();

private:
    void RenderBucket(u32 x, u32 y);

    Layer* GetTargetLayer() { return m_image->GetLayer(m_targetLayer); };

    Ray GetRay(u32 x, u32 y) const;
    Color4 GetRayColor(const Ray& ray, HitInfo hitInfo, i32 maxDepth, Scene* scene) const;
    void CalculatePixelColor(u32 x, u32 y);
    Vector3 SampleSquare() const;

    u32 m_samplesPerPixel = {10};
    u32 m_bucketSize = {64};

    bool m_isRunning = {false};

    Camera* m_camera = {nullptr};
    Scene* m_scene = {nullptr};
    Image* m_image = {nullptr};

    u64 m_version = {0};
    u64 m_prevVersion = {0};
    u64 m_prevCameraVersion = {0};

    std::string m_targetLayer = "eInvalid";

    /* Execution Objects */
    friend class Tasker;
    UniquePtr<Tasker> m_tasker;
    UniquePtr<ThreadPool> m_pool;
};

}