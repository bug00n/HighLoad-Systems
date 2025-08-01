#include "cache.h"
#include <chrono>

std::unordered_map<std::string, Cache::Entry> Cache::data;
std::mutex Cache::mtx;

void Cache::put(const std::string& key, const std::string& value, int ttl_seconds) {
    std::lock_guard<std::mutex> lock(mtx);
    data[key] = {
        value,
        std::chrono::steady_clock::now() + std::chrono::seconds(ttl_seconds)
    };
}

std::string Cache::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = data.find(key);
    if (it != data.end()) {
        if (std::chrono::steady_clock::now() < it->second.expires_at) {
            return it->second.value;
        }
        data.erase(it);
    }
    return "Not Founded";
}

void Cache::cleanup() {
    std::lock_guard <std::mutex> lock(mtx);
    auto now = std::chrono::steady_clock::now();
    for (auto it = data.begin(); it != data.end();) {
        if (now >= it->second.expires_at) {
            it = data.erase(it);
        } else {
            ++it;
        }
    }
}