#include "Tracer/Engine.hpp"
#include <cmath>
#include <chrono>

#include <glm/geometric.hpp>

namespace Tracer {

namespace {
    const f64 kPi = 3.1415926535897932385;
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
    for (u32 bY = 0; bY < m_bucketSize; bY++) {
        for (u32 bX = 0; bX < m_bucketSize; bX++) {
            CalculatePixelColor(x+bX, y+bY);
        }
    }
}

Ray Engine::GetRay(u32 x, u32 y) const {
    /* Tracer::Image - 0, 0 is the Top Left Pixel. 
    Viewport -1.0f, 1.0f is the Top Left Pixel */

    f32 width = m_image->GetWidth();
    f32 height =  m_image->GetHeight();

    f32 aspectRatio = width / height;
    f32 focalLength = m_camera->GetFocalLength();

    f32 ndcX = (-1.0f + ((static_cast<f32>(x) + 0.5f) * 2.0f / width));
    f32 ndcY = (1.0f - ((static_cast<f32>(y) + 0.5f) * 2.0f / height));

    Point3 origin = Point3(0.0f, 0.0f, -2.0f);
    Vector3 direction = glm::normalize(Vector3((ndcX * aspectRatio), ndcY, focalLength));
    Ray ray(origin, direction);
    return ray;
};

Color4 Engine::GetRayColor(const Ray& ray, HitInfo hitInfo, i32 maxDepth, Scene* scene) const {
    return Color4(0.0f);
}

void Engine::CalculatePixelColor(u32 x, u32 y) const {
    /* Check Image Bounding Box */
    if (x > m_image->GetWidth() || y > m_image->GetHeight()) {
        return;
    }

    /* !RayTracing! */
    Ray ray = m_camera->TransformRay(GetRay(x, y));
    //Ray ray = GetRay(x, y);

    /* Missed Colour */
    auto color = m_missedColor;

    /* Check for any hits */
    auto scene = m_scene->GetObjects();
    HitInfo info{};
    for (auto& object : scene) {
        if (object->isHit(ray, info, Interval(), *m_camera)) {
            color = object->GetSurface()->CalculateColor(info);
        };
    }

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