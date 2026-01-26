#pragma once

#include "Tracer/Types.hpp"

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

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    
    std::mutex m_queue_mutex;
    std::condition_variable m_cv;

    bool m_stop = {false};
    PoolMode m_mode = {PoolMode::eInvalid};
};

}