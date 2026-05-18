#pragma once

#include "Core/Types.hpp"

#include <atomic>
#include <unordered_map>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

namespace Tracer {

enum class PoolMode {
    eInvalid,
    eRunning, /* Running and accepting new tasks.*/
    eFinishCurrent, /* Finishes Current Tasks and doesn't accept new tasks. */
    eDrain,   /* Stops after task queue is empty and doesn't accept new tasks. */
    eForceStop /* Stops all of the threads. */
};

class ThreadPool {
public:
    ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    ~ThreadPool();

    void sumbitTask(std::function<void()> task);
    void clearQueue();
    void abortCurrent();

    bool isRendering() const;

    void setStartingQueueSize(u32 size) { m_startingQueueSize = size; }
    u32 runningThreadCount() const;
    f32 progress() const;

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    u32 m_startingQueueSize = {0};
    
    std::mutex m_queue_mutex;
    std::condition_variable m_cv;

    bool m_stop = {false};
    PoolMode m_mode = {PoolMode::eInvalid};

    std::vector<std::atomic<bool>> m_renderingState;
};

}