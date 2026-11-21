#pragma once

#include <cstdint>
#include <fstream>
#include "image.h"
#include <memory>

class ImageReader {
private:
    inline static const uint8_t PATTERN = UINT8_MAX;
    std::shared_ptr<Image> img_;

    void Read(std::ifstream& file, uint16_t& obj);
    void Read(std::ifstream& file, uint32_t& obj);
    void Read(std::ifstream& file, std::vector<uint8_t>& obj);

    void ReadFileHeader(std::ifstream& file);

    void ReadInfoHeader(std::ifstream& file);

    void ReadPixels(std::ifstream& file);

public:
    ImageReader() = default;

    void ReadFile(const std::string& path);

    std::shared_ptr<Image> GetImage();
};

class ImageWriter {
private:
    inline static const uint8_t PATTERN = UINT8_MAX;
    std::shared_ptr<Image> img_;

    void Write(std::ofstream& file, uint16_t& obj);
    void Write(std::ofstream& file, uint32_t& obj);
    void Write(std::ofstream& file, std::vector<uint8_t>& obj);

    void WriteFileHeader(std::ofstream& file);

    void WriteInfoHeader(std::ofstream& file);

    void WritePixels(std::ofstream& file);

public:
    explicit ImageWriter(std::shared_ptr<Image> img) : img_(img) {
    }

    void WriteFile(const std::string& path);
};
