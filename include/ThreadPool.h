#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <chrono>
#include "SimpleLogger.h"


template <typename V>
class ThreadPool
{
public:

    ThreadPool(int threads);
    ThreadPool(ThreadPool const&);
    ThreadPool& operator=(ThreadPool const&);
    ~ThreadPool();
    void doJob(V);

protected:

    void threadEntry(int i);

    std::mutex lock_;
    std::condition_variable condVar_;
    bool shutdown_;
    std::queue <V> jobs_;
    std::vector <std::thread> threads_;
    SimpleLogger tpLogger_;
};

template <typename V>
ThreadPool<V>::ThreadPool(int threads) : shutdown_(false), tpLogger_(SimpleLogger("harness.txt"))
{
    // Create the specified number of threads
    threads_.reserve(threads);
    for (int i = 0; i < threads; ++i)
        threads_.emplace_back(std::bind(&ThreadPool::threadEntry, this, i));
}

template <typename V>
ThreadPool<V>::ThreadPool(ThreadPool const& tp)
{
    shutdown_ = tp.shutdown_;
    jobs_ = tp.jobs_;
    tpLogger_ = tp.tpLogger_;
}

template <typename V>
ThreadPool<V>& ThreadPool<V>::operator=(ThreadPool const& tp)
{
    if (&tp != this) {
        shutdown_ = tp.shutdown_;
        jobs_ = tp.jobs_;
        tpLogger_ = tp.tpLogger_;
    }
    return *this;
}

template <typename V>
ThreadPool<V>::~ThreadPool()
{
    {
        // Unblock any threads and tell them to stop
        std::unique_lock <std::mutex> l(lock_);

        shutdown_ = true;
        condVar_.notify_all();
    }

    // Wait for all threads to stop
    tpLogger_.info("Joining threads");
    for (auto& thread : threads_)
        thread.join();
}

template <typename V>
void ThreadPool<V>::doJob(V func)
{
    // Place a job on the queue and unblock a thread
    std::unique_lock <std::mutex> l(lock_);

    jobs_.emplace(std::move(func));
    condVar_.notify_one();
}

template <typename V>
void ThreadPool<V>::threadEntry(int i)
{
    V job;
    SimpleLogger workerLogger = SimpleLogger("workers.txt");
    while (1)
    {
        {
            std::unique_lock <std::mutex> l(lock_);
            while (!shutdown_ && jobs_.empty())
                condVar_.wait(l);

            if (jobs_.empty())
            {
                // No jobs to do and we are shutting down
                workerLogger.info("Worker: " + std::to_string(i) + " Terminating");
                return;
            }

            workerLogger.info("Worker: " + std::to_string(i) + " PROCESSING");
            job = std::move(jobs_.front());
            jobs_.pop();
        }

        // Do the job without holding any locks
        std::string result = ((job()) ? "True." : "False.");
        workerLogger.debug("Worker: " + std::to_string(i) + " Result of job: " +  result);
        workerLogger.info("Worker: " + std::to_string(i) + " READY");
    }

}