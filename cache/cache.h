#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <optional>


template <typename KeyType, typename ValueType>
class Shard {
 private:
    std::unordered_map <KeyType, ValueType> shard_storage;

 public:
    Shard() = default;

    std::optional<ValueType> get(const KeyType& key) const;

    void set(const KeyType& key, const ValueType& value);

    void remove(const KeyType& key);
};


template <typename KeyType, typename ValueType>
class Cache {
 private:
    std::vector <Shard<KeyType, ValueType>> cache_storage;

    size_t getId(const KeyType& key) const;

 public:
    Cache(int size) explicit;

    std::optional<ValueType> get(const KeyType& key) const;

    void set(const KeyType& key, const ValueType& value);

    void remove(const KeyType& key);
};
