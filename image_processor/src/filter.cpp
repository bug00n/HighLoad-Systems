#include <cmath>
#include <cstdint>
#include "exception.h"
#include "filter.h"
#include <iostream>
#include <string>
#include <unordered_map>

std::shared_ptr<Image> Crop::Apply(std::shared_ptr<Image> img) {
    if (height_ < img->InfoHeader::height) {
        img->pixels.resize(height_);
        img->buffer.resize(height_);
        img->InfoHeader::height = height_;
    }
    if (width_ < img->InfoHeader::width) {
        for (auto &line : img->pixels) {
            line.resize(width_);
        }
        for (auto &line : img->buffer) {
            line.resize(width_);
        }
        img->InfoHeader::width = width_;
    }
    img->image_size = height_ * ((width_ * 3 + 3) / 4 * 4);
    return img;
}

std::shared_ptr<Image> Grayscale::Apply(std::shared_ptr<Image> img) {
    for (auto &line : img->pixels) {
        for (auto &pixel : line) {
            double color = 0;
            color = pixel.r_ * RGray + pixel.g_ * GGray + pixel.b_ * BGray;
            color = std::max(0.0, std::min(1.0, color));
            pixel = Pixel(color, color, color);
        }
    }
    return img;
}

std::shared_ptr<Image> Negative::Apply(std::shared_ptr<Image> img) {
    for (auto &line : img->pixels) {
        for (auto &pixel : line) {
            pixel = Pixel(1.0 - pixel.r_, 1.0 - pixel.g_, 1.0 - pixel.b_);
        }
    }
    return img;
}

std::shared_ptr<Image> Sharpening::Apply(std::shared_ptr<Image> img) {
    img->Multiply(MATRIX);
    return img;
}

std::shared_ptr<Image> EdgeDetection::Apply(std::shared_ptr<Image> img) {
    img = Grayscale::Apply(img);
    img->Multiply(MATRIX);
    for (int32_t row = 0; row < img->InfoHeader::height; ++row) {
        for (int32_t col = 0; col < img->InfoHeader::width; ++col) {
            if (img->pixels[row][col].r_ <= threshold_) {
                img->pixels[row][col] = Pixel(0.0, 0.0, 0.0);
            } else {
                img->pixels[row][col] = Pixel(1.0, 1.0, 1.0);
            }
        }
    }
    return img;
}

std::shared_ptr<Image> GaussianBlur::Apply(std::shared_ptr<Image> img) {
    int32_t size = static_cast<int32_t>(sigma_ * 4);
    if (size % 2 == 0) {
        size += 1;
    }
    CalcMatrix(size);
    img->Multiply(matrix);
    matrix.Transpose();
    img->Multiply(matrix);
    return img;
}

void GaussianBlur::CalcMatrix(int32_t size) const {
    if (sigma_ == 0.0) {
        throw ApplyFilterException("Sigma can't be zero");  // bad
    }
    std::vector<std::vector<double>> n_matrix(1, std::vector<double>(size));
    double sum = 0;
    for (int col = 0; col < size; ++col) {
        int d_col = abs(size / 2 - col);

        d_col = d_col * d_col;

        double double_sigma = 2 * sigma_ * sigma_;

        double e = powf(M_E, static_cast<float>(-(d_col) / double_sigma));

        n_matrix[0][col] = e / M_PI / double_sigma;
        sum += n_matrix[0][col];
    }
    for (int col = 0; col < size; ++col) {
        n_matrix[0][col] /= sum;
    }
    std::swap(matrix.matrix, n_matrix);
}

std::shared_ptr<Image> Tunnel::Apply(std::shared_ptr<Image> img) {
    int32_t center_h = static_cast<int32_t>(img->InfoHeader::height / 2);
    int32_t center_w = static_cast<int32_t>(img->InfoHeader::width / 2);

    if (center_h_ != 0) {
        center_h = center_h_;
    }
    if (center_w_ != 0) {
        center_w = center_w_;
    }

    if (center_h < 0 || img->InfoHeader::height <= center_h) {
        throw ApplyFilterException("Incorrect center coordinates for -tunnel: " + std::to_string(center_h) + "\n");
    }
    if (center_w < 0 || img->InfoHeader::width <= center_w) {
        throw ApplyFilterException("Incorrect center coordinates for -tunnel: " + std::to_string(center_w) + "\n");
    }

    std::vector<std::vector<Pixel>> buffer(img->InfoHeader::height, std::vector<Pixel>(img->InfoHeader::width));

    for (int32_t row = 0; row < img->InfoHeader::height; ++row) {
        for (int32_t col = 0; col < img->InfoHeader::width; ++col) {
            double d_row = row - center_h;
            double d_col = col - center_w;
            double distance = std::sqrt(d_row * d_row + d_col * d_col);

            buffer[row][col] = img->pixels[row][col];
            if (distance < radius_ && distance > 0) {
                double angle = std::atan2(d_row, d_col);
                double new_angle = angle + strength_ * (1.0 - distance / radius_);

                int32_t src_row = center_h + static_cast<int32_t>(distance * std::sin(new_angle));
                int32_t src_col = center_w + static_cast<int32_t>(distance * std::cos(new_angle));

                if (src_col >= 0 && src_col < img->InfoHeader::width && src_row >= 0 &&
                    src_row < img->InfoHeader::height) {
                    buffer[row][col] = img->pixels[src_row][src_col];
                }
            }
        }
    }

    std::swap(buffer, img->pixels);

    return img;
}
