#pragma once

#include <string>
#include <chrono>
#include <mutex>
#include <unordered_map>

class Cache {
 private:
    struct Entry {
        std::string value;
        std::chrono::steady_clock::time_point expires_at;
    };

    static std::unordered_map<std::string, Entry> data;
    static std::mutex mtx;

 public:
    static void put(const std::string& key, const std::string& value, int ttl_seconds = 60);
    static std::string get(const std::string& key);
    static void cleanup();
};
