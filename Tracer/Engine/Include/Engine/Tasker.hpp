#pragma once

#include "Core/Types.hpp"

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

namespace {
    using SubmitTask = std::function<void()>;
    using RenderTask = std::function<void()>;
    
    struct TileTask {
        RenderTask task;
        Tracer::u32 x, y;
    };
}

namespace Tracer {

class Engine;

enum class TileOrder {
    eInvalid = -1,
    eLeftToRight = 0,
    eCenterOut = 1,
    eDifferenceFirst = 2
};

class Tasker {
public:
    Tasker(Engine* engine);
    ~Tasker();

    void SetTileOrder(TileOrder order);

    void requestFrame();
    void requestFrame(TileOrder override);

private:
    std::queue<TileTask> createTilesQueue();
    std::queue<TileTask> sortTiles(std::queue<TileTask> queue, TileOrder order);
    void execute();
    
    std::thread m_thread;
    std::queue<SubmitTask> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    bool m_stop = {false};
    bool m_submittingFrame = {false};

    TileOrder m_tileOrder = {TileOrder::eCenterOut};

    friend class Engine;
    Engine* m_engine = {nullptr};
};

};