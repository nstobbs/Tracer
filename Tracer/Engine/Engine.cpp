#include "Core/StatusMessage.hpp"
#include "Engine/Engine.hpp"
#include "Material/Material.hpp"

#include <cmath>
#include <chrono>

#include <glm/geometric.hpp>
#include <random>
#include <utility>

namespace Tracer {

namespace {
    constexpr bool kSingleThreaded = false;
    const u32 kMaxDepth = 4;
}

Engine::Engine() {
    constexpr int threadCount = (kSingleThreaded) ? 1 : 12;
    std::printf("Tracer::Engine - ThreadCount: %i\n", threadCount);
    m_pool = std::make_unique<ThreadPool>(threadCount);
    m_tasker = std::make_unique<Tasker>(this);
}

void Engine::SetScene(Scene* scene) {
    m_version++;
    m_scene = scene;
}

void Engine::SetCamera(Camera* camera) {
    m_version++;
    m_lastCameraVersion = camera->GetVersion();
    m_camera = camera;
}

void Engine::SetImage(Image* image) {
    m_version++;
    m_image = image;
}

void Engine::SetSamplesPerPixel(u32 numOfSamples) {
    m_version++;
    m_samplesPerPixel = numOfSamples;
}

void Engine::SetTileSize(u32 size) {
    m_version++;
    m_tileSize = size;
    if (m_activeList) {
        m_activeList->tileSize = size;
    }
}

void Engine::SetMissedColor(Color4 color) {
    m_version++; 
    m_missedColor = color;
}

void Engine::SetMaxRayDepth(u32 depth) {
    m_version++;
    m_maxRayDepth = depth;
}

void Engine::SetTargetLayer(const std::string& layer) {
    m_version++;
    m_targetLayer = layer;
}

void Engine::SetActiveTilesRecord(ActiveTilesRecord* activeList) {
    m_version++;
    m_activeList = activeList;
    m_activeList->tileSize = m_tileSize;
}

void Engine::StartRendering() {
    m_isRunning = true;
    StatusMessage::Set("Tracer::Engine: Started Rendering.");
}

void Engine::StopRendering() {
    m_isRunning = false;
    m_pool->clearQueue();
    StatusMessage::Set("Tracer::Engine: Stopped Render.");
}

bool Engine::hasVersionChanged() {
    if (m_version == m_lastVersion && m_lastCameraVersion == m_camera->GetVersion()) {
        return false;
    }
    return true;
}

void Engine::updateLastVerion() {
    m_lastVersion = m_version;
    m_lastCameraVersion = m_camera->GetVersion();
}

void Engine::Tick() {
    if (m_isRunning && hasVersionChanged()) {
        StatusMessage::Set("Tracer::Engine: Requesting Frame");
        /* Re-Cache Objects Transform Matrixes */
        for (const auto& object :m_scene->getObjects()) {
            object->transform().build();
        }

        m_tasker->SetTileOrder(TileOrder::eCenterOut);
        m_tasker->requestFrame();
        updateLastVerion();
    }

    bool isPoolRendering = m_pool->isRendering(); 

    if (m_isRunning && isPoolRendering) {
        StatusMessage::Set("Tracer::Engine: Render In Progress");
    } else if (m_isRunning && !isPoolRendering) {
        StatusMessage::Set("Tracer::Engine: Waiting For Tasks");
    }
}

void Engine::RenderTile(u32 x, u32 y) {
    i32 tileID;
    /* Record Active Tile - For TileCrosshair HUD */
    if (m_activeList) {
        {
            std::unique_lock<std::mutex> lock(m_activeList->mutex);
            tileID = m_activeList->lifetimeTileCount++;
            m_activeList->active.emplace(tileID, std::pair<i32, i32>(x, y));
        }
    }

    /* Randomly Shuffle Calculate Pixel Tasks */
    std::vector<std::pair<u32, u32>> pixelCoords;
    for (u32 bY = 0; bY < m_tileSize; bY++) {
        for (u32 bX = 0; bX < m_tileSize; bX++) {
            pixelCoords.emplace_back(x + bX, y + bY);
        }
    }
    std::mt19937 rng(std::random_device{}());
    std::shuffle(pixelCoords.begin(), pixelCoords.end(), rng);
    for (auto& [x, y] : pixelCoords) {
        CalculatePixelColor(x, y);
    }

    /* Remove Tile From Record - For TileCrosshair HUD*/
    if (m_activeList) {
        {
            std::unique_lock<std::mutex> lock(m_activeList->mutex);
            m_activeList->active.erase(tileID);
        }
    }
}

Color4 Engine::GetRayColor(const Ray& ray, HitInfo hitInfo, i32 maxDepth, Scene* scene) const {
    return Color4(0.0f);
}

void Engine::CalculatePixelColor(u32 x, u32 y) const {
    /* Check Image Bounding Box */
    if (x >= m_image->GetWidth() || y >= m_image->GetHeight()) {
        return;
    }

    auto color = m_missedColor;
    // FIXME: Pre adjusting before sample div later.
    color.x *= m_samplesPerPixel;
    color.y *= m_samplesPerPixel;
    color.z *= m_samplesPerPixel;

    /* Render Per Samples */
    for (i32 sample = 0; sample < m_samplesPerPixel; sample++) {
        /* Get Ray */
        Ray ray = m_camera->GetRay(*m_image, x, y);
        HitInfo info{};

        /* Depth Testing */
        Object* frontObject = nullptr;
        f32 distanceToObject = Interval().Max();

        for (auto object : m_scene->findHitObjects(ray)) {
            if (object->isHit(ray, info, Interval())) {
                if (info.distance < distanceToObject) {
                    frontObject = object;
                    distanceToObject = info.distance;
                }
            };
        }
        if (frontObject) {
            //auto materialOutput = frontObject->getMaterial()->rayColor(ray, kMaxDepth, m_missedColor, m_scene); 
            //color += materialOutput;

            color += frontObject->getSurface()->CalculateColor(info);
        }
    }

    color.r = color.r / m_samplesPerPixel;
    color.g = color.g / m_samplesPerPixel;
    color.b = color.b / m_samplesPerPixel;
    color.a = color.a / m_samplesPerPixel;

    /* Write to ImageLayer*/
    if (m_targetLayer != "eInvalid") {
        auto& dest = m_image->GetLayer(m_targetLayer)->at(x, y);
        dest = color;
    }
}

Vector3 Engine::SampleSquare() const {
    return Vector3(0.0f);
}

}