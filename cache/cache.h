#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <optional>
#include <shared_mutex>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>


template <typename KeyType, typename ValueType>
class Shard {
 private:
    std::unordered_map <KeyType, ValueType> shard_storage;
    mutable std::shared_mutex mutex;

 public:
    Shard() = default;

    std::optional<ValueType> get(const KeyType& key) const;

    void set(const KeyType& key, const ValueType& value);

    void remove(const KeyType& key);
};

template <typename Task>
class TaskQueue {
 private:
    std::mutex mutex;
    std::condition_variable cv;
    std::queue <Task> queue;

 public:
    void push(const Task& task);

    Task pop(std::atomic<bool>& stop_flag);
};

template <typename KeyType, typename ValueType>
class Cache {
 private:
    std::vector <Shard<KeyType, ValueType>> cache_storage;
    std::vector <std::thread> threads;
    TaskQueue <std::function <void()>> tasks;
    std::atomic <bool> stop{false};

    size_t getId(const KeyType& key) const;

    void thread_loop();

 public:
    explicit Cache(int count_of_shards, int count_of_threads);
    ~Cache() noexcept;

    std::future<std::optional<ValueType>> get_async(const KeyType& key);

    void set_async(const KeyType& key, const ValueType& value);

    void remove_async(const KeyType& key);
};
