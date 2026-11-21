#pragma once

#include <cstdint>
#include <vector>
#include "matrix.h"
#include "pixel.h"
#include <iostream>

struct FileHeader {
    uint16_t signature;
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t data_offset;
};

struct InfoHeader {
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    uint32_t x_pixels_per_meter;
    uint32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
};

class Image : public FileHeader, public InfoHeader {
public:
    std::vector<std::vector<Pixel>> pixels;
    inline static std::vector<std::vector<Pixel>> buffer;

    void Multiply(const Matrix& matrix);

    Image() = default;
};
