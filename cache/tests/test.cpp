#include "../cache.h"
#include "test_tool.h"

#include <thread>
#include <vector>
#include <random>
#include <chrono>

TEST(basic_set_get) {
    Cache <int,int> c(4);

    c.set_async(1, 100);
    auto v = c.get_sync(1);

    EXPECT_EQ(v, 100);
}

TEST(overwrite) {
    Cache <int,int> c(4, 4);

    c.set_async(1, 1);
    c.set_async(1, 2);

    EXPECT_EQ(c.get_sync(1), 2);
}

TEST(remove) {
    Cache <int,int> c(4, 4);

    c.set_async(1, 10);
    c.remove_async(1);

    auto v = c.get_sync(1);
    EXPECT_TRUE(!v.has_value());
}

TEST(concurrent_writers) {
    Cache <int,int> c(8, 16);

    std::vector<std::thread> ths;

    for (int i = 0; i < 8; ++i) {
        ths.emplace_back([&]{
            for (int k = 0; k < 5000; ++k)
                c.set_async(k, k);
        });
    }

    for (auto& t : ths)
        t.join();

    EXPECT_EQ(c.get_async(123).get(), 123);
}

TEST(stress_random_ops) {
    Cache<int,int> c(16, 32);

    const int THREADS = 16;
    const int OPS = 50000;

    std::vector<std::thread> ths;

    for (int t = 0; t < THREADS; ++t) {
        ths.emplace_back([&]{
            std::mt19937 rng(std::random_device{}());
            std::uniform_int_distribution<int> key(0, 1000);
            std::uniform_int_distribution<int> op(0, 2);

            for (int i = 0; i < OPS; ++i) {
                int k = key(rng);

                if (op(rng) == 0)
                    c.set_async(k, i);
                else if (op(rng) == 1)
                    c.get_async(k);
                else
                    c.remove_async(k);
            }
        });
    }

    for (auto& t : ths)
        t.join();

    EXPECT_TRUE(true);
}

TEST(hot_key_race) {
    Cache<int,int> c(4, 8);

    std::vector<std::thread> ths;

    for (int i = 0; i < 16; ++i) {
        ths.emplace_back([&,i]{
            for (int k = 0; k < 10000; ++k)
                c.set_async(1, i);
        });
    }

    for (auto& t : ths) {
        t.join();
    }

    auto v = c.get_async(1).get();
    EXPECT_TRUE(v >= 0);
}

int main() {
    RUN_TEST(basic_set_get);
    RUN_TEST(overwrite);
    RUN_TEST(remove);

    RUN_TEST(concurrent_writers);
    RUN_TEST(hot_key_race);
    RUN_TEST(stress_random_ops);

    std::cout << "\nTESTS PASSED\n";
}
