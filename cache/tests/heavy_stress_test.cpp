#include "../cache.h"
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <atomic>

// 90 -> MAX 42525239
// 100 -> MAX

const int CNT = 115;

int main() {
    long long max_ops = 0;
    long long min_ops = LLONG_MAX;
    long long avg_ops = 0;
    for (int s = 0; s < 30; ++s) {
        Cache<std::string, int> cache(CNT);

        for (int i = 0; i < 100000; ++i) {
            cache.set_async(std::to_string(i), i);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        std::atomic <uint64_t> ops = 0;

        int workers = std::thread::hardware_concurrency();
        std::vector <std::thread> pool;

        auto end = std::chrono::steady_clock::now() + std::chrono::seconds(3);

        for (int t = 0; t < workers; ++t) {
            pool.emplace_back([&]{
                std::mt19937 rng(std::random_device{}());
                std::uniform_int_distribution<> key_dist(0, 100000);
                std::uniform_int_distribution<> op_dist(0, 99);

                uint64_t local = 0;

                while (std::chrono::steady_clock::now() < end) {
                    int k = key_dist(rng);

                    if (op_dist(rng) < 80) {
                        cache.get_sync(std::to_string(k)); // FAST PATH
                    } else {
                        cache.set_async(std::to_string(k), k);
                    }

                    ++local;
                }

                ops += local;
            });
        }

        for (auto& th : pool) {
            th.join();
        }

        std::cout << "OPS/sec = " << ops / 3 << "\n";
        max_ops = std::max((long long) (ops / 3), max_ops);
        min_ops = std::min((long long) (ops / 3), min_ops);
        avg_ops += (long long) ops;
    }

    avg_ops /= 90;

    std::cout << "MAX" << ' ' << max_ops << '\n';
    std::cout << "MIN" << ' ' << min_ops << '\n';
    std::cout << "AVG" << ' ' << avg_ops << '\n';
    std::cout << std::thread::hardware_concurrency() << "\n";
}
