#include "thread_pool.h"

#include <Logger.h>

ThreadPool::ThreadPool(size_t n) : stop(false) {
    for (size_t i = 0; i < n; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this] { return stop || !tasks.empty(); });
                    if (stop && tasks.empty()) return;
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all();
    for (auto& t : workers) t.join();
    Logger::debug("The threads have been released");
}

void ThreadPool::enqueue(std::function<void()> job) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.push(std::move(job));
    }
    cv.notify_one();
    Logger::debug("A task has been pushed into the queue");
}