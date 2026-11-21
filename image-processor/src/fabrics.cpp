#include "fabrics.h"
#include "exception.h"

void IsInteger(const char* value) {
    std::string s(value);
    if (s.length() > MaxIntegerLength) {
        throw ParserException("The argument \"" + s + "\" is too long");
    }
    for (auto& c : s) {
        if (!std::isdigit(c)) {
            throw ParserException("The argument \"" + s + "\" is not integer");
        }
    }
}

void IsFloat(const char* value) {
    std::string s(value);
    if (s.length() > MaxIntegerLength) {
        throw ParserException("The argument \"" + s + "\" is too long");
    }
    int cnt_dots = 0;
    for (auto& c : s) {
        if (!std::isdigit(c) && c != '.') {
            throw ParserException("The argument \"" + s + "\" is not integer");
        }
        if (c == '.') {
            ++cnt_dots;
        }
    }
    if (cnt_dots > 1) {
        throw ParserException("The argument \"" + s + "\" is not float");
    }
}

void IsNotNull(const char* value) {
    std::string s(value);
    for (auto& c : s) {
        if (c != '.' && c != '0') {
            return;
        }
    }
    throw ParserException("The argument \"" + s + "\" is null");
}

std::unique_ptr<Filter> CropFabric::Make(const char* width, const char* height) {
    IsInteger(height);
    IsNotNull(height);
    IsInteger(width);
    IsNotNull(width);
    return std::make_unique<Crop>(std::atoi(width), std::atoi(height));
}

std::unique_ptr<Filter> GrayScaleFabric::Make() {
    return std::make_unique<Grayscale>();
}

std::unique_ptr<Filter> NegativeFabric::Make() {
    return std::make_unique<Negative>();
}

std::unique_ptr<Filter> SharpFabric::Make() {
    return std::make_unique<Sharpening>();
}

std::unique_ptr<Filter> EdgeFabric::Make(const char* value) {
    IsFloat(value);
    return std::make_unique<EdgeDetection>(std::atof(value));
}

std::unique_ptr<Filter> BlurFabric::Make(const char* value) {
    IsFloat(value);
    return std::make_unique<GaussianBlur>(std::atof(value));
}

std::unique_ptr<Filter> TunnelFabric::Make(const char* pixel_h, const char* pixel_w, const char* strongth,
                                           const char* radius) {
    IsInteger(pixel_h);
    IsInteger(pixel_w);
    IsFloat(strongth);
    IsFloat(radius);
    return std::make_unique<Tunnel>(std::atoi(pixel_h), std::atoi(pixel_w), std::atof(strongth), std::atof(radius));
}
