#pragma once

#include <cstdint>

struct Pixel {
    double r_, g_, b_;

    Pixel() = default;
    Pixel(double r, double g, double b) : r_(r), g_(g), b_(b) {
    }
    Pixel(uint8_t r, uint8_t g, uint8_t b) {
        r_ = static_cast<double>(r) / UINT8_MAX;
        g_ = static_cast<double>(g) / UINT8_MAX;
        b_ = static_cast<double>(b) / UINT8_MAX;
    }
};
