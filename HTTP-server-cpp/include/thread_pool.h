#pragma once

#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <functional>
#include <condition_variable>

class ThreadPool {
 private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex mtx;
    std::condition_variable cv;
    bool stop;

 public:
    ThreadPool(size_t n);
    ~ThreadPool();
    void enqueue(std::function<void()> job);
};