#include "Engine/Tasker.hpp"
#include "Engine/Engine.hpp"

#include "Core/StatusMessage.hpp"

#include <map>

namespace Tracer {

Tasker::Tasker(Engine* engine) {
    m_engine = engine;
    m_thread = std::thread{[this]{ 
        while(true) {
            SubmitTask task;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait(lock, [this]{
                    return !m_tasks.empty() || m_stop;
                });

                if (m_stop && m_tasks.empty()) {
                    return;
                }

                task = std::move(m_tasks.front());
                m_tasks.pop();
            }
            if (!task) {
                return;
            }
            task();
        }
    }};
};

Tasker::~Tasker() {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_stop = true;
    }
    m_cv.notify_all();
    m_thread.join();
};

void Tasker::SetTileOrder(TileOrder order) {
    if (order != TileOrder::eInvalid) {
        m_tileOrder = order;
    }
};

std::queue<TileTask> Tasker::createTilesQueue() {
    std::queue<TileTask> tileQueue;
    if (m_engine->m_targetLayer != "eInvalid") {
        /* Calculate Tile Count */
        u32 width = m_engine->m_image->GetWidth();
        u32 height = m_engine->m_image->GetHeight();
        u32 tileWidthSize = (width / m_engine->m_tileSize) + 1;
        u32 tileHeightSize = (height / m_engine->m_tileSize) + 1;

        /* Create Queue of TileTasks */
        for (u32 Y = 0; Y < tileHeightSize; Y++) {
            for (u32 X = 0; X < tileWidthSize; X++) {
                TileTask tile{};
                u32 topLeftX = X * m_engine->m_tileSize - (m_engine->m_tileSize / 2);
                u32 topLeftY = Y * m_engine->m_tileSize - (m_engine->m_tileSize / 2);
                tile.x = topLeftX;
                tile.y = topLeftY;
                tile.task = [this, topLeftX, topLeftY]{
                    this->m_engine->RenderTile(topLeftX, topLeftY);
                };
                tileQueue.emplace(tile);
            }
        }
    }
    return tileQueue;
};

void Tasker::requestFrame() {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_tasks.emplace([this]{
            this->m_engine->m_pool->clearQueue();
            this->execute();
        });
    }
    m_cv.notify_one();
};

void Tasker::requestFrame(TileOrder override) {

};

std::queue<TileTask> Tasker::sortTiles(std::queue<TileTask> queue, TileOrder order) {
    std::multimap<f32, TileTask> distanceTilesMap;
    if (order != TileOrder::eInvalid) {
        switch (order) {
            case TileOrder::eLeftToRight:
                break;
            case TileOrder::eCenterOut:
                Point2 imageCenter = Point2(static_cast<f32>(m_engine->m_image->GetWidth()) / 2.0f,
                                            static_cast<f32>(m_engine->m_image->GetHeight()) / 2.0f);
                while (!queue.empty()) {
                    TileTask task = queue.front();
                    queue.pop();

                    f32 tileOffset = m_engine->m_tileSize / 2.0f;

                    Point2 taskPosition = Point2(static_cast<f32>(task.x + tileOffset),
                                                 static_cast<f32>(task.y - tileOffset));
                    Point2 delta = imageCenter - taskPosition;
                    delta.x = delta.x * delta.x;
                    delta.y = delta.y * delta.y;
                    f32 distance = delta.x + delta.y;
                    distance = std::sqrt(distance);
                    distanceTilesMap.emplace(distance, task);
                };
                
                for (auto task : distanceTilesMap) {
                    queue.emplace(task.second);
                }
                break;
        }
    }
    return queue;
};

void Tasker::execute() {
    m_submittingFrame = true;
    std::queue<TileTask> tasks = createTilesQueue();
    tasks = sortTiles(tasks, m_tileOrder);
    m_engine->m_pool->setStartingQueueSize(static_cast<u32>(tasks.size()));
    while (!tasks.empty()) {
        m_engine->m_pool->sumbitTask(std::move(tasks.front().task));
        tasks.pop();
    }
    m_submittingFrame = false;
};

}
