#include "cache.h"
#include <memory>

template <typename KeyType, typename ValueType>
std::optional<ValueType> Shard<KeyType, ValueType>::get(const KeyType& key) const {
	std::shared_lock lock(mutex);
    auto it = shard_storage.find(key);
    if (it == shard_storage.end()) {
        return std::nullopt;
    }
    return it->second;
}

template <typename KeyType, typename ValueType>
void Shard<KeyType, ValueType>::set(const KeyType& key, const ValueType& value) {
    std::unique_lock lock(mutex);
    shard_storage[key] = value;
}

template <typename KeyType, typename ValueType>
void Shard<KeyType, ValueType>::remove(const KeyType& key) {
	std::unique_lock lock(mutex);
    shard_storage.erase(key);
}

template <typename Task>
void TaskQueue<Task>::push(const Task& task) {
    {
        std::lock_guard lock(mutex);
        queue.push(std::move(task));
    }
    cv.notify_one();
}

template <typename Task>
Task TaskQueue<Task>::pop(std::atomic<bool>& stop) {
    std::unique_lock lock(mutex);
    cv.wait(lock, [&]{return !queue.empty() || stop.load();});

	if (queue.empty()) return std::function<void()>{};

    Task t = std::move(queue.front());
    queue.pop();

    return t;
}

template <typename KeyType, typename ValueType>
size_t Cache<KeyType, ValueType>::getId(const KeyType& key) const {
    return std::hash<KeyType>{}(key) % cache_storage.size();
}

template <typename KeyType, typename ValueType>
void Cache<KeyType, ValueType>::thread_loop() {
    while (!stop) {
        auto task = tasks.pop(stop);
        if (task) task();
    }
}

Cache<KeyType, ValueType>::Cache(int count_of_shards, int count_of_threads) {
	cache_storage.resize(count_of_shards);
	for (int i = 0; i < count_of_threads; ++i) {
		threads.emplace_back([this]{
			thread_loop();
		});
	}
}

Cache<KeyType, ValueType>::~Cache() noexcept {
	stop = true;

	for (size_t i = 0; i < threads.size(); ++i) {
        tasks.push([]{});
	}

	for (auto &thread : threads) {
		thread.join();
	}
}

template <typename KeyType, typename ValueType>
std::future<std::optional<ValueType>> Cache<KeyType, ValueType>::get_async(const KeyType& key) {
	auto promise = std::make_shared<std::promise<std::optional<ValueType>>>();
	auto f = promise->get_future();

	tasks.push([this, key, promise]() mutable {
		auto id = getId(key);
		promise->set_value(cache_storage[id].get(key));
	});

	return f;
}

template <typename KeyType, typename ValueType>
void Cache<KeyType, ValueType>::set_async(const KeyType& key, const ValueType& value) {
	tasks.push([this, key, value]{
		auto id = getId(key);
        cache_storage[id].set(key, value);
	});
}

template <typename KeyType, typename ValueType>
void Cache<KeyType, ValueType>::remove_async(const KeyType& key) {
	tasks.push([this, key]{
		auto id = getId(key);
        cache_storage[id].remove(key);
	});
}
