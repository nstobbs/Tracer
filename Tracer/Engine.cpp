#include "Tracer/Engine.hpp"
#include <cmath>
#include <chrono>
#include <glm/geometric.hpp>

namespace Tracer {

namespace {
    const f32 kPi = 3.14f; 
}

Engine::Engine() {
    m_pool = new ThreadPool;
}

Engine::~Engine() {
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_isRunning = false;
    }
    m_cv.notify_all();
    m_sumbitThread.join();
    
    delete m_pool;
}

void Engine::SetScene(Scene* scene) {
    m_version++;
    m_scene = scene;
}

void Engine::SetCamera(Camera* camera) {
    m_version++;
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
    /* Sumbit Thread */
    m_sumbitThread = std::thread([this]{
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_cv.wait(lock, [this]{
                return !m_tasks.empty() || !m_isRunning;
            });

            if (!m_isRunning && m_tasks.empty()) {
                return;
            }

            task = std::move(m_tasks.front());
            m_tasks.pop();
        }
        if(!task) {
            return;
        }
        task();
    });
}

void Engine::StopRendering() {
    m_isRunning = false;
}

void Engine::Tick() {
    if (m_isRunning && m_version != m_prevVersion) {
        m_prevVersion = m_version;
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_tasks.emplace(std::move([this]{
                this->SubmitTasks();
            }));
        }
        m_cv.notify_all();
    }
}

void Engine::SubmitBucket(u32 x, u32 y) {
    for (u32 bY = 0; bY < m_bucketSize; bY++) {
        for (u32 bX = 0; bX < m_bucketSize; bX++) {
            CalculatePixelColor(x+bX, y+bY);
        }
    }
}

void Engine::SubmitTasks() {
    if (m_targetLayer != "eInvalid") {
        u32 w = m_image->GetWidth();
        u32 h = m_image->GetHeight();

        u32 bucketWCount = w / m_bucketSize;
        u32 bucketHCount = h / m_bucketSize;

        for (u32 Y = 0; Y < bucketHCount; Y++) {
            for (u32 X = 0; X < bucketWCount; X++) {
                u32 pX = X * m_bucketSize;
                u32 pY = Y * m_bucketSize;
                m_pool->sumbitTask([this, pX, pY]{
                    this->SubmitBucket(pX, pY);
                });
            }
        }
    }
};

Ray Engine::GetRay(u32 x, u32 y) const {
    f32 width = m_image->GetWidth();
    f32 height =  m_image->GetHeight();
    f32 aspectRatio = width / height;

    f32 fov = m_camera->GetFoV();

    f32 Px = (2 * ((x + 0.5) / width) - 1) * tan(fov / 2 * kPi / 180) * aspectRatio;
    f32 Py = (1 - 2 * ((y + 0.5) / height)) * tan(fov / 2 * kPi / 180);
    Point3 origin = Point3(0.0f, 0.0f, 0.0f);
    Vector3 direction = glm::normalize(Vector3(Px, Py, -1.0f));
    Ray ray(origin, direction);
    return ray;
};

Color4 Engine::GetRayColor(const Ray& ray, HitInfo hitInfo, i32 maxDepth, Scene* scene) const {
    return Color4(0.0f);
}

void Engine::CalculatePixelColor(u32 x, u32 y) {
    /* Check Image Bounding Box */
    if (x > m_image->GetWidth() || y > m_image->GetHeight()) {
        return;
    }

    /* !RayTracing! */
    Ray ray = GetRay(x, y);

    auto scene = m_scene->GetObjects();
    HitInfo info;
    for (auto object : scene) {
        if (object->isHit(ray, info)) {
            Color4 rayColor;
            for (u32 sample = 0; sample < m_samplesPerPixel; sample++) {
                rayColor =+ object->getSurface()->calculateSurfaceColor(info);
            };
            rayColor = rayColor / Color4(m_samplesPerPixel);
        };
    }

    /* Calculate Pixel Color */
    auto w = m_image->GetWidth();
    auto h = m_image->GetHeight();
    auto pX = f32(x) / f32(w);
    auto pY = f32(y) / f32(h);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(0.1ms);

    auto color = Color4(pX, pY, 0.0f, 1.0f);

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