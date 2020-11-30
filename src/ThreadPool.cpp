#include "../include/ThreadPool.h"

ThreadPool::ThreadPool(int threads) : shutdown_(false)
{
    // Create the specified number of threads
    threads_.reserve(threads);
    for (int i = 0; i < threads; ++i)
        threads_.emplace_back(std::bind(&ThreadPool::threadEntry, this, i));
}

ThreadPool::ThreadPool(ThreadPool const& tp)
{
    shutdown_ = tp.shutdown_;
    jobs_ = tp.jobs_;
}

ThreadPool& ThreadPool::operator=(ThreadPool const& tp)
{
    // TODO: insert return statement here
    if (&tp != this) {
        shutdown_ = tp.shutdown_;
        jobs_ = tp.jobs_;
    }
    return *this;
}

ThreadPool::~ThreadPool()
{
    {
        // Unblock any threads and tell them to stop
        std::unique_lock <std::mutex> l(lock_);

        shutdown_ = true;
        condVar_.notify_all();
    }

    // Wait for all threads to stop
    std::cerr << "Joining threads" << std::endl;
    for (auto& thread : threads_)
        thread.join();
}

void ThreadPool::doJob(std::function <void(uint64_t)> func)
{
    // Place a job on the queue and unblock a thread
    std::unique_lock <std::mutex> l(lock_);

    jobs_.emplace(std::move(func));
    condVar_.notify_one();
}

void ThreadPool::threadEntry(int i)
{
    std::function <void(uint64_t)> job;

    while (1)
    {
        {
            std::unique_lock <std::mutex> l(lock_);

            while (!shutdown_ && jobs_.empty())
                condVar_.wait(l);

            if (jobs_.empty())
            {
                // No jobs to do and we are shutting down
                std::cerr << "Thread " << i << " terminates" << std::endl;
                return;
            }

            std::cerr << "Thread " << i << " does a job" << std::endl;
            job = std::move(jobs_.front());
            jobs_.pop();
        }

        // Do the job without holding any locks
        job(i);
    }

}