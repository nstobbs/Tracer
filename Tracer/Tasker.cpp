#include "Tracer/Tasker.hpp"
#include "Tracer/Engine.hpp"

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

void Tasker::SetBucketOrder(BucketOrder order) {
    if (order != BucketOrder::eInvalid) {
        m_bucketOrder = order;
    }
};

std::queue<BucketTask> Tasker::createBucketsQueue() {
    std::queue<BucketTask> bucketQueue;
    if (m_engine->m_targetLayer != "eInvalid") {
        /* Calculate Bucket Count */
        u32 width = m_engine->m_image->GetWidth();
        u32 height = m_engine->m_image->GetHeight();
        u32 bucketWidthSize = width / m_engine->m_bucketSize;
        u32 bucketHeightSize = height / m_engine->m_bucketSize;

        /* Create Queue of BucketTasks */
        for (u32 Y = 0; Y < bucketHeightSize; Y++) {
            for (u32 X = 0; X < bucketWidthSize; X++) {
                u32 topLeftX = X * m_engine->m_bucketSize;
                u32 topLeftY = Y * m_engine->m_bucketSize;
                bucketQueue.emplace([this, topLeftX, topLeftY]{
                    this->m_engine->RenderBucket(topLeftX, topLeftY);
                });
            }
        }
    }
    return bucketQueue;
};

void Tasker::SubmitFrameToPool() {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_tasks.emplace([this]{
            this->m_engine->m_pool->clearQueue();
            this->execute();
        });
    }
    m_cv.notify_one();
};

void Tasker::SubmitFrameToPool(BucketOrder override) {

};

std::queue<BucketTask> Tasker::sortBuckets(std::queue<BucketTask> queue, BucketOrder order) {
    return queue;
};

void Tasker::execute() {
    m_submittingFrame = true;
    std::queue<BucketTask> tasks = createBucketsQueue();
    tasks = sortBuckets(tasks, m_bucketOrder);
    while (!tasks.empty()) {
        m_engine->m_pool->sumbitTask(std::move(tasks.front()));
        tasks.pop();
    }
    m_submittingFrame = false;
};

}
