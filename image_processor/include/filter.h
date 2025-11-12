#pragma once

#include <cmath>
#include <cstdint>
#include "image.h"
#include <memory>
#include <numbers>

constexpr double RGray = 0.299;
constexpr double GGray = 0.587;
constexpr double BGray = 0.114;

class Filter {
public:
    Filter() = default;

    virtual std::shared_ptr<Image> Apply(std::shared_ptr<Image> img) = 0;

    virtual ~Filter() = default;
};

class Crop final : public Filter {
private:
    int16_t width_;
    int16_t height_;

public:
    Crop() = default;

    Crop(int16_t width, int16_t height) : width_(width), height_(height) {
    }

    std::shared_ptr<Image> Apply(std::shared_ptr<Image> img) final;

    ~Crop() final = default;
};

class Grayscale : public Filter {
public:
    Grayscale() = default;

    std::shared_ptr<Image> Apply(std::shared_ptr<Image> img) override;

    ~Grayscale() override = default;
};

class Negative final : public Filter {
public:
    Negative() = default;

    std::shared_ptr<Image> Apply(std::shared_ptr<Image> img) final;

    ~Negative() final = default;
};

class Sharpening final : public Filter {
private:
    inline static const Matrix MATRIX = {{{0.0, -1.0, 0.0}, {-1.0, 5.0, -1.0}, {0.0, -1.0, 0.0}}};

public:
    Sharpening() = default;

    std::shared_ptr<Image> Apply(std::shared_ptr<Image> img) final;

    ~Sharpening() final = default;
};

class EdgeDetection final : public Grayscale {
private:
    double threshold_;
    inline static const Matrix MATRIX = {{{0.0, -1.0, 0.0}, {-1.0, 4.0, -1.0}, {0.0, -1.0, 0.0}}};

public:
    EdgeDetection() = default;
    explicit EdgeDetection(double threshold) : threshold_(threshold) {
    }

    std::shared_ptr<Image> Apply(std::shared_ptr<Image> img) final;

    ~EdgeDetection() final = default;
};

class GaussianBlur final : public Filter {
private:
    double sigma_;

    inline static Matrix matrix;

    void CalcMatrix(int32_t size) const;

public:
    GaussianBlur() = default;
    explicit GaussianBlur(double sigma) : sigma_(sigma) {
    }

    std::shared_ptr<Image> Apply(std::shared_ptr<Image> img) final;

    ~GaussianBlur() final = default;
};

class Tunnel final : public Filter {
private:
    int32_t center_h_, center_w_;
    double strength_;
    double radius_;

public:
    Tunnel() : center_h_(-1), center_w_(-1) {
    }
    explicit Tunnel(int32_t center_h, int32_t center_w, double strength, double radius)
        : center_h_(center_h), center_w_(center_w), strength_(strength), radius_(radius) {
    }

    std::shared_ptr<Image> Apply(std::shared_ptr<Image> img) final;

    ~Tunnel() final = default;
};
