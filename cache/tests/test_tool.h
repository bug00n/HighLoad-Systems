#pragma once

#include <iostream>
#include <cstdlib>

#define TEST(name) void name()

#define RUN_TEST(name) \
        std::cout << "Running " #name "... "; \
        name(); \
        std::cout << "OK\n";

#define EXPECT_TRUE(x) \
        if (!(x)) { \
        std::cerr << "FAIL: " #x << "\n"; \
        std::exit(1); \
        }

#define EXPECT_EQ(a,b) \
        if (!((a)==(b))) { \
        std::cerr << "FAIL: " #a " == " #b << "\n"; \
        std::exit(1); \
        }
