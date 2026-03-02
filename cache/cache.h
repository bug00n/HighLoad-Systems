#pragma once
#pragma GCC optimize ("O3")

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <vector>


template <typename KeyType, typename ValueType>
class alignas(64) Shard {
 private:
    const int MAX_KEYS = 50'000;
    char padding[64];

    std::unordered_map <KeyType, ValueType> storage;
    mutable std::shared_mutex mutex;

    std::queue <std::function <void()>> tasks;
    std::mutex tasks_mutex;
    std::condition_variable tasks_cv;
    std::atomic <bool> stop = false;
    std::thread worker;

    void worker_loop() {
        while (!stop) {
            std::function <void()> task;
            {
                std::unique_lock lock(tasks_mutex);
                tasks_cv.wait(lock, [&]{ return !tasks.empty() || stop; });
                if (tasks.empty() && stop) {
                    break;
                }
                if (!tasks.empty()) {
                    task = std::move(tasks.front());
                    tasks.pop();
                }
            }
            if (task) {
                task();
            }
        }
    }

 public:
    Shard() : worker([this]{ worker_loop(); }) {
        storage.reserve(MAX_KEYS);
    }

    ~Shard() {
        stop = true;
        tasks_cv.notify_all();
        if (worker.joinable()) {
            worker.join();
        }
    }

    std::optional<ValueType> get(const KeyType& key) const {
        std::shared_lock lock(mutex);
        auto it = storage.find(key);
        if (it == storage.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    void set(const KeyType& key, const ValueType& value) {
        std::unique_lock lock(mutex);
        storage[key] = value;
    }

    void remove(const KeyType& key) {
        std::unique_lock lock(mutex);
        storage.erase(key);
    }

    void push_task(std::function<void()> task) {
        {
            std::lock_guard lock(tasks_mutex);
            tasks.push(std::move(task));
        }
        tasks_cv.notify_one();
    }
};

template <typename KeyType, typename ValueType>
class Cache {
 private:
    std::vector<std::unique_ptr<Shard<KeyType, ValueType>>> shards;

    size_t getId(const KeyType& key) const {
        return std::hash<KeyType>{}(key) % shards.size();
    }

 public:
    explicit Cache(int shard_count) {
        shards.reserve(shard_count);
        for (int i = 0; i < shard_count; ++i) {
            shards.push_back(std::make_unique<Shard<KeyType, ValueType>>());
        }
    }

    std::optional<ValueType> get_sync(const KeyType& key) const {
        auto id = getId(key);
        return shards[id]->get(key);
    }

    std::future<std::optional<ValueType>> get_async(const KeyType& key) {
        auto promise = std::make_shared<std::promise<std::optional<ValueType>>>();
        size_t id = getId(key);

        shards[id]->push_task([this, id, key, promise]{
            promise->set_value(shards[id]->get(key));
        });

        return promise->get_future();
    }

    void set_async(const KeyType& key, const ValueType& value) {
        size_t id = getId(key);
        shards[id]->push_task([this, id, key, value]{
            shards[id]->set(key, value);
        });
    }

    void remove_async(const KeyType& key) {
        size_t id = getId(key);
        shards[id]->push_task([this, id, key]{
            shards[id]->remove(key);
        });
    }
};
