#include "Tracer/Engine.hpp"
#include <cmath>
#include <chrono>

#include <glm/geometric.hpp>
#include <random>
#include <utility>

namespace Tracer {

namespace {
    constexpr bool kSingleThreaded = false;
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
    m_prevCameraVersion = camera->GetCameraVersion();
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

void Engine::SetTargetLayer(const std::string& layer) {
    m_version++;
    m_targetLayer = layer;
}

void Engine::StartRendering() {
    m_isRunning = true;
}

void Engine::StopRendering() {
    m_isRunning = false;
}


void Engine::Tick() {
    if (m_isRunning) {
        if (m_version != m_prevVersion || m_prevCameraVersion != m_camera->GetCameraVersion()) { // TODO: Clean this up.
            m_prevVersion = m_version;
            m_prevCameraVersion = m_camera->GetCameraVersion();
            m_tasker->SetBucketOrder(BucketOrder::eCenterOut);
            m_tasker->SubmitFrameToPool();
        }
    }
}

void Engine::RenderBucket(u32 x, u32 y) const {
    std::vector<std::pair<u32, u32>> pixelCoords;
    for (u32 bY = 0; bY < m_bucketSize; bY++) {
        for (u32 bX = 0; bX < m_bucketSize; bX++) {
            pixelCoords.emplace_back(x + bX, y + bY);
        }
    }

    std::mt19937 rng(std::random_device{}());
    std::shuffle(pixelCoords.begin(), pixelCoords.end(), rng);

    for (auto& [x, y] : pixelCoords) {
        CalculatePixelColor(x, y);
    }  
}

Color4 Engine::GetRayColor(const Ray& ray, HitInfo hitInfo, i32 maxDepth, Scene* scene) const {
    return Color4(0.0f);
}

void Engine::CalculatePixelColor(u32 x, u32 y) const {
    /* Check Image Bounding Box */
    if (x > m_image->GetWidth() || y > m_image->GetHeight()) {
        return;
    }

    /* Prep Scene, Viewport */
    auto color = m_missedColor;
    // FIXME: Pre adjusting before sample div later.
    color.x *= m_samplesPerPixel;
    color.y *= m_samplesPerPixel;
    color.z *= m_samplesPerPixel;

    auto scene = m_scene->GetObjects();

    /* Render Per Samples */
    for (i32 sample = 0; sample < m_samplesPerPixel; sample++) {
        /* Ray Tracing Starts */
        Ray ray = m_camera->GetRay(*m_image, x, y);
        HitInfo info{};

        for (auto& object : scene) {
            if (object->isHit(ray, info, Interval(), *m_camera)) {
                color += object->GetSurface()->CalculateColor(info);
            };
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