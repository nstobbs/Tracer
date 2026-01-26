#include "Tracer/ThreadPool.hpp"

namespace Tracer {

ThreadPool::ThreadPool(size_t numThreads) {
    for (i32 threadID = 0; threadID < numThreads; threadID++) {
        std::printf("Createing Thread: %i\n", threadID);
        m_threads.emplace_back([this, threadID] {
            while (true) {
                std::function<void()> task;
                {
                    std::printf("Locking...ThreadID: %i\n", threadID);
                    std::unique_lock<std::mutex> lock(m_queue_mutex);
                    m_cv.wait(lock, [this] {
                        return !m_tasks.empty() || m_stop;
                    });

                    if (m_stop && m_tasks.empty()) {
                        return;
                    }

                    std::printf("Taking Task...ThreadID: %i\n", threadID);
                    task = std::move(m_tasks.front());
                    m_tasks.pop();
                }
                std::printf("Executing Task...ThreadID: %i\n", threadID);
                if(!task) {
                    std::printf("Empty Function was called!.. ThreadID: %i\n", threadID);
                    __debugbreak();
                    return;
                };
                task();
            }
        });
    }
};

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_stop = true;
        m_mode = PoolMode::eForceStop;
    }

    m_cv.notify_all();
    for (auto& thread : m_threads) {
        thread.join();
    }
};

void ThreadPool::sumbitTask(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock (m_queue_mutex);
        m_tasks.emplace(std::move(task));
    }
    m_cv.notify_one();
};

void ThreadPool::clearQueue() {
    {
        std::unique_lock<std::mutex> lock (m_queue_mutex);
        if (auto size = m_tasks.size() != 0) {
            for (int i = 0; i < size; i++) {
                m_tasks.pop();
            }
        }
    }
};

void ThreadPool::abortCurrent() {

};

}