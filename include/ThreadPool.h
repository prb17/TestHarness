#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <chrono>


class ThreadPool
{
public:

    ThreadPool(int threads);
    ThreadPool(ThreadPool const&);
    ThreadPool& operator=(ThreadPool const&);
    ~ThreadPool();
    void doJob(std::function <void(uint64_t)> func);

protected:

    void threadEntry(int i);

    std::mutex lock_;
    std::condition_variable condVar_;
    bool shutdown_;
    std::queue <std::function <void(uint64_t)>> jobs_;
    std::vector <std::thread> threads_;
};