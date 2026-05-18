#include "Engine/ThreadPool.hpp"

namespace Tracer {

ThreadPool::ThreadPool(size_t numThreads) {
    m_renderingState = std::vector<std::atomic<bool>>(numThreads);
    for (i32 threadID = 0; threadID < numThreads; threadID++) {
        //std::printf("Createing Thread: %i\n", threadID);
        m_threads.emplace_back([this, threadID] {
            while (true) {
                std::function<void()> task;
                {
                    //std::printf("Locking...ThreadID: %i\n", threadID);
                    std::unique_lock<std::mutex> lock(m_queue_mutex);
                    m_cv.wait(lock, [this] {
                        return !m_tasks.empty() || m_stop;
                    });

                    if (m_stop && m_tasks.empty()) {
                        return;
                    }

                    //std::printf("Taking Task...ThreadID: %i\n", threadID);
                    task = std::move(m_tasks.front());
                    m_tasks.pop();
                }
                //std::printf("Executing Task...ThreadID: %i\n", threadID);
                if(!task) {
                    std::printf("Empty Function was called!.. ThreadID: %i\n", threadID);
                    //__debugbreak(); FIXME: Create an CrossPlaform for Breaking within Threads!
                    return;
                };
                m_renderingState.at(threadID).store(true);
                task();
                m_renderingState.at(threadID).store(false);
            }
        });
    }
};

bool ThreadPool::isRendering() const {
    for (auto& threadState : m_renderingState) {
        if (threadState.load() == true){
            return true;
        }
    }
    return false;
}

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
        //std::printf("Clearing the Render Queue.\n");
        std::unique_lock<std::mutex> lock (m_queue_mutex);
        int size = m_tasks.size();
        if (size != 0) {
            for (int i = 0; i < size; i++) {
                m_tasks.pop();
            }
        }
    }
};

u32 ThreadPool::runningThreadCount() const {
    u32 count = 0;
    for (auto& threadState : m_renderingState) {
        if(threadState.load() == true) {
            count++;
        }
    }
    return count;
}

f32 ThreadPool::progress() const {
    auto inProgressCount = static_cast<f32>(m_tasks.size()) + static_cast<f32>(runningThreadCount());
    return inProgressCount / static_cast<f32>(m_startingQueueSize); 
}

void ThreadPool::abortCurrent() {

};

}