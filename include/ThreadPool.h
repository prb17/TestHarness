#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <chrono>
#include "../include/Logger.h"


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
    Logger tpLogger_;
    std::string loggerPrefix_;
};

template <typename V>
ThreadPool<V>::ThreadPool(int threads) : shutdown_(false), tpLogger_(Logger()), loggerPrefix_("ThreadPool: ")
{
    // Create the specified number of threads
    threads_.reserve(threads);
    for (int i = 0; i < threads; ++i)
        threads_.emplace_back(std::bind(&ThreadPool::threadEntry, this, i));
    tpLogger_.set_prefix(loggerPrefix_);
}

template <typename V>
ThreadPool<V>::ThreadPool(ThreadPool const& tp)
{
    shutdown_ = tp.shutdown_;
    jobs_ = tp.jobs_;
    tpLogger_ = tp.tpLogger_;
    loggerPrefix_ = tp.loggerPrefix_;
}

template <typename V>
ThreadPool<V>& ThreadPool<V>::operator=(ThreadPool const& tp)
{
    if (&tp != this) {
        shutdown_ = tp.shutdown_;
        jobs_ = tp.jobs_;
        tpLogger_ = tp.tpLogger_;
        loggerPrefix_ = tp.loggerPrefix_;
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
    tpLogger_.log(Logger::LOG_LEVELS::LOW, "Joining threads");
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
    Logger workerLogger = Logger();
    workerLogger.set_prefix("Worker " + std::to_string(i) + ": ");
    while (1)
    {
        {
            std::unique_lock <std::mutex> l(lock_);

            while (!shutdown_ && jobs_.empty())
                condVar_.wait(l);

            if (jobs_.empty())
            {
                // No jobs to do and we are shutting down
                workerLogger.log(Logger::LOG_LEVELS::LOW, "Terminating");
                return;
            }

            workerLogger.log(Logger::LOG_LEVELS::LOW, "PROCESSING");
            job = std::move(jobs_.front());
            jobs_.pop();
        }

        // Do the job without holding any locks
        std::string result = ((job()) ? "True." : "False.");
        workerLogger.log(Logger::LOG_LEVELS::MED, "Result of job: " +  result);
        workerLogger.log(Logger::LOG_LEVELS::LOW, "READY");
    }

}