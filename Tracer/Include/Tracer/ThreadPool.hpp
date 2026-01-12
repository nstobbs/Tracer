#pragma once

#include "Tracer/Types.hpp"

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

namespace Tracer {

class ThreadPool {
public:
    ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    ~ThreadPool();

    void sumbitTask(std::function<void()> task);
private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    
    std::mutex m_queue_mutex;
    std::condition_variable m_cv;

    bool m_stop = {false};
};

}