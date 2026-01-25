#pragma once

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

namespace {
    using SubmitTask = std::function<void()>;
    using BucketTask = std::function<void()>;
}

namespace Tracer {

class Engine;

enum class BucketOrder {
    eInvalid = -1,
    eLeftToRight = 0,
    eCenterOut = 1,
    eDifferenceFirst = 2
};

class Tasker {
public:
    Tasker(Engine* engine);
    ~Tasker();

    void SetBucketOrder(BucketOrder order);

    void SubmitFrameToPool();
    void SubmitFrameToPool(BucketOrder override);

private:
    std::queue<BucketTask> createBucketsQueue();
    std::queue<BucketTask> sortBuckets(std::queue<BucketTask> queue, BucketOrder order);
    void execute();
    
    std::thread m_thread;
    std::queue<SubmitTask> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    bool m_stop = {false};
    bool m_submittingFrame = {false};

    BucketOrder m_bucketOrder = {BucketOrder::eLeftToRight};

    friend class Engine;
    Engine* m_engine = {nullptr};
};

};