#pragma once

#include "Engine/ThreadPool.hpp"
#include "Engine/Tasker.hpp"

#include "Core/Image.hpp"
#include "Core/Camera.hpp"
#include "Core/Scene.hpp"
#include "Core/Ray.hpp"
#include "Core/Interval.hpp"

#include "Window/TileCrosshairRenderPass.hpp"

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
    void SetTileSize(u32 size);
    void SetMaxRayDepth(u32 depth);
    void SetTargetLayer(const std::string& layer);
    void SetMissedColor(Color4 color);
    void SetActiveTilesRecord(ActiveTilesRecord* activeList);

    u32 getSamplesPerPixel() const { return m_samplesPerPixel; }
    u32 getTileSize() const { return m_tileSize; }
    u32 getMaxRayDepth() const { return m_maxRayDepth; }
    Color4 getMissedColor() const { return m_missedColor; }
    f32 getRenderProgress() const { return m_pool->progress(); }

    Image* GetTargetImage() { return m_image; }

    void StartRendering();
    void StopRendering();

    void Tick();

private:
    void RenderTile(u32 x, u32 y);

    bool hasVersionChanged(); /* Returns true if the engine or camera version has changed */
    void updateLastVerion(); /* Updates the Last Versions with the Currect Versions */ 
    Layer* GetTargetLayer() const { return m_image->GetLayer(m_targetLayer); }
    Color4 GetRayColor(const Ray& ray, HitInfo hitInfo, i32 maxDepth, Scene* scene) const;
    void CalculatePixelColor(u32 x, u32 y) const;
    Vector3 SampleSquare() const;

    u32 m_samplesPerPixel = {1};
    u32 m_tileSize = {32};
    u32 m_maxRayDepth = {4};

    Color4 m_missedColor = {0.0f, 0.0f, 0.0f, 1.0f};

    bool m_isRunning = {false};

    Camera* m_camera = {nullptr};
    Scene* m_scene = {nullptr};
    Image* m_image = {nullptr};

    u64 m_version = {0};
    u64 m_lastVersion = {0};
    u64 m_lastCameraVersion = {0};

    std::string m_targetLayer = "eInvalid";

    /* Execution Objects */
    friend class Tasker;
    UniquePtr<Tasker> m_tasker;
    UniquePtr<ThreadPool> m_pool;
    ActiveTilesRecord* m_activeList = {nullptr};
};
}