#include "Core/StatusMessage.hpp"
#include "Engine/Engine.hpp"

#include "Material/Material.hpp"
#include "Object/LightSource.hpp"

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
    m_pool->abort(false);
    StatusMessage::Set("Tracer::Engine: Started Rendering.");
}

void Engine::StopRendering() {
    m_isRunning = false;
    m_pool->abort(true);
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
        for (const auto& object : m_scene->objects()) {
            object->transform().build();
        }

        /* Re-Cache LightSources Transform Matrixes */
        for (const auto& lightSource : m_scene->lightSources()) {
            lightSource->transform().build();
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
        if (m_pool->isAborting()) {
            continue;;
        }
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

Color4 Engine::renderLightSourceShapes(const Ray& ray) const {
    Color4 color{};
    for (const auto& lightSource : m_scene->findHitLightSources(ray)) {
        HitInfo info{};
        if (lightSource->isHit(ray, info, Interval())) {
            color += lightSource->calculateSurface(info);
        }
    }
    return color;
}

Color4 Engine::calculateLightSources(LightFilterRecord& record, const Ray& ray, const HitInfo& info, u64& depth) const {
    if (m_pool->isAborting()) {
        return {};
    }

    if (depth == m_maxRayDepth) {
        return m_missedColor; /* Max Ray Depth was Reached */
    } else {
        if (info.object) {
            /* Record Lighting Event */
            LightFilterEvent event = info.object->material()->createFilter(ray, info);
            record.push(event);

            /* Scatter From Material */
            Ray scatted{};
            HitInfo scattedInfo{};
            depth++;
            if (info.object->material()->scatter(ray, info, scatted)) {
                /* Check if we hit a LightSource */
                for (auto const& light : m_scene->findHitLightSources(scatted)) {
                    if (light->isHit(scatted, scattedInfo, Interval())) {
                        return Color4(light->applyRecord(record), 1.0f); /* Hit a LightSource */
                    }
                }
                
                /* Check if we hit any Objects */
                Object* frontObject = nullptr;
                HitInfo frontInfo{};
                frontInfo.distance = Interval().Max();

                for (auto const& object : m_scene->findHitObjects(scatted)) {
                    if (object->isHit(scatted, scattedInfo, Interval())) {
                        if (scattedInfo.distance < frontInfo.distance) {
                            frontObject = object;
                            frontInfo = scattedInfo;
                        }
                    }
                }

                if (frontInfo.object){
                    calculateLightSources(record, scatted, frontInfo, depth); /* Hit an Object */
                }
            }
        }
    }
    return m_missedColor; /* Ray Never Hit anything else... */
};

void Engine::CalculatePixelColor(u32 x, u32 y) const {
    /* Check Image Bounding Box */
    if (x >= m_image->GetWidth() || y >= m_image->GetHeight()) {
        return;
    }

    /* Clear Screen */
    if (m_targetLayer != "eInvalid") {
        auto& dest = m_image->GetLayer(m_targetLayer)->at(x, y);
        dest = m_missedColor;
    }

    auto sampleOffset = Color4(m_samplesPerPixel);

    /* Render Per Samples */
    for (i32 sample = 0; sample < m_samplesPerPixel; sample++) {
        if (m_pool->isAborting()) {
            return;
        }

        auto color = m_missedColor / sampleOffset; /* Starting Pixel Color*/

        /* Get Ray */
        Ray ray = m_camera->GetRay(*m_image, x, y);
        HitInfo info{};

        /* Depth Testing */
        Object* frontObject = nullptr;
        HitInfo frontInfo{};
        frontInfo.distance = Interval().Max();

        /* Find Closest Object */
        for (const auto& object : m_scene->findHitObjects(ray)) {
            if (object->isHit(ray, info, Interval())) {
                if (info.distance < frontInfo.distance) {
                    frontObject = object;
                    frontInfo = info;
                }
            };
        }

        /* If we didn't hit any objects, check if we hit a light source */
        if (!frontInfo.object) {
            color += renderLightSourceShapes(ray) / sampleOffset;
        }

        /* Calculate Lighting */
        if (frontInfo.object) {
            LightFilterRecord record = LightFilterRecord(false);
            u64 depth = 0;
            color += calculateLightSources(record, ray, frontInfo, depth) / sampleOffset;
        }

        /* Write to ImageLayer*/
        if (m_targetLayer != "eInvalid") {
            auto& dest = m_image->GetLayer(m_targetLayer)->at(x, y);
            dest = dest + color;
        }
    }
}

}