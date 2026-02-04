#pragma once

#include "cache.h"

template <typename KeyType, typename ValueType>
std::optional<ValueType> get(const KeyType& key) const {
    auto it = shard_storage.find(key);
    if (it == shard_storage.end()) {
        return std::nullopt;
    }
    return it->second;
}

template <typename KeyType, typename ValueType>
void set(const KeyType& key, const ValueType& value) {
    shard_storage[key] = value;
}

template <typename KeyType, typename ValueType>
void remove(const KeyType& key) {
    shard_storage.erase(key);
}

template <typename KeyType, typename ValueType>
size_t getId(const KeyType& key) const {
    return std::hash<KeyType>{}(key) % cache_storage.size();
}

template <typename KeyType, typename ValueType>
std::optional<ValueType> get(const KeyType& key) const {
    return cache_storage[getId(key)].get(key);
}

template <typename KeyType, typename ValueType>
void set(const KeyType& key, const ValueType& value) {
    cache_storage[getId(key)].set(key, value);
}

template <typename KeyType, typename ValueType>
void remove(const KeyType& key) {
    cache_storage[getId(key)].remove(key);
}
