#include "io.h"
#include "exception.h"

const int32_t SIGN = 0x4D42;
const int8_t SIZE = 54;
const int8_t SIZE_H = 40;
const int8_t SIZE_P = 24;

uint8_t GetByte(double x) {
    return static_cast<uint8_t>(x * UINT8_MAX);
}
void Check(std::shared_ptr<Image> img) {
    if (img->signature != SIGN) {
        throw ReadImageException("Incorrect signature");
    }
    if (img->header_size != SIZE_H) {
        throw ReadImageException("Incorrect header size");
    }
    if (img->bits_per_pixel != SIZE_P) {
        throw ReadImageException("Incorrect size of pixels");
    }
    if (img->compression != 0) {
        throw ReadImageException("There is compression");
    }
    if (img->colors_used != 0) {
        throw ReadImageException("There is colors table");
    }
}

void ImageReader::Read(std::ifstream& file, uint16_t& obj) {
    file.read(reinterpret_cast<char*>(&obj), sizeof(obj));
}
void ImageReader::Read(std::ifstream& file, uint32_t& obj) {
    file.read(reinterpret_cast<char*>(&obj), sizeof(obj));
}
void ImageReader::Read(std::ifstream& file, std::vector<uint8_t>& obj) {
    file.read(reinterpret_cast<char*>(obj.data()), static_cast<std::streamsize>(obj.size()));
}

void ImageReader::ReadFileHeader(std::ifstream& file) {
    try {
        Read(file, img_->FileHeader::signature);
        Read(file, img_->FileHeader::file_size);
        Read(file, img_->FileHeader::reserved1);
        Read(file, img_->FileHeader::reserved2);
        Read(file, img_->FileHeader::data_offset);
    } catch (std::exception& e) {
        throw ReadImageException("File header hasn't been read: error of reading file.");
    }
}
void ImageReader::ReadInfoHeader(std::ifstream& file) {
    try {
        Read(file, img_->InfoHeader::header_size);
        Read(file, img_->InfoHeader::width);
        Read(file, img_->InfoHeader::height);
        Read(file, img_->InfoHeader::planes);
        Read(file, img_->InfoHeader::bits_per_pixel);
        Read(file, img_->InfoHeader::compression);
        Read(file, img_->InfoHeader::image_size);
        Read(file, img_->InfoHeader::x_pixels_per_meter);
        Read(file, img_->InfoHeader::y_pixels_per_meter);
        Read(file, img_->InfoHeader::colors_used);
        Read(file, img_->InfoHeader::colors_important);
    } catch (std::exception& e) {
        throw ReadImageException("Info header hasn't been read: error of reading file.");
    }
}
void ImageReader::ReadPixels(std::ifstream& file) {
    try {
        uint32_t line_size = img_->InfoHeader::width * 3;
        uint8_t padding = (4 - line_size % 4) % 4;

        std::vector<uint8_t> buffer(line_size + padding, 0);
        for (uint32_t row = 0; row < img_->InfoHeader::height; ++row) {
            Read(file, buffer);

            uint32_t real_row = img_->InfoHeader::height - row - 1;
            for (int col = 0; col < img_->InfoHeader::width; ++col) {
                int offset = col * 3;

                uint8_t blue = buffer[offset];
                uint8_t green = buffer[offset + 1];
                uint8_t red = buffer[offset + 2];

                img_->pixels[real_row][col] = Pixel(red, green, blue);
            }
        }
    } catch (std::exception& e) {
        throw ReadImageException("Image hasn't been read: error of reading pixels.");
    }
}

void ImageReader::ReadFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        throw ReadImageException("Cannot open read file." + path);
    }

    img_ = std::make_shared<Image>();

    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (file_size < SIZE) {
        throw ReadImageException("File is incorrect, size is lower then need");
    }

    ReadFileHeader(file);
    ReadInfoHeader(file);

    Check(img_);

    img_->pixels.resize(img_->InfoHeader::height, std::vector<Pixel>(img_->InfoHeader::width));
    img_->buffer.resize(img_->InfoHeader::height, std::vector<Pixel>(img_->InfoHeader::width));

    ReadPixels(file);
}

std::shared_ptr<Image> ImageReader::GetImage() {
    return img_;
}

void ImageWriter::Write(std::ofstream& file, uint16_t& obj) {
    file.write(reinterpret_cast<char*>(&obj), sizeof(obj));
}
void ImageWriter::Write(std::ofstream& file, uint32_t& obj) {
    file.write(reinterpret_cast<char*>(&obj), sizeof(obj));
}
void ImageWriter::Write(std::ofstream& file, std::vector<uint8_t>& obj) {
    file.write(reinterpret_cast<char*>(obj.data()), static_cast<std::streamsize>(obj.size()));
}

void ImageWriter::WriteFileHeader(std::ofstream& file) {
    try {
        Write(file, img_->FileHeader::signature);
        Write(file, img_->FileHeader::file_size);
        Write(file, img_->FileHeader::reserved1);
        Write(file, img_->FileHeader::reserved2);
        Write(file, img_->FileHeader::data_offset);
    } catch (std::exception& e) {
        throw WriteImageException("File header hasn't been written: error of writing into file.");
    }
}
void ImageWriter::WriteInfoHeader(std::ofstream& file) {
    try {
        Write(file, img_->InfoHeader::header_size);
        Write(file, img_->InfoHeader::width);
        Write(file, img_->InfoHeader::height);
        Write(file, img_->InfoHeader::planes);
        Write(file, img_->InfoHeader::bits_per_pixel);
        Write(file, img_->InfoHeader::compression);
        Write(file, img_->InfoHeader::image_size);
        Write(file, img_->InfoHeader::x_pixels_per_meter);
        Write(file, img_->InfoHeader::y_pixels_per_meter);
        Write(file, img_->InfoHeader::colors_used);
        Write(file, img_->InfoHeader::colors_important);
    } catch (std::exception& e) {
        throw WriteImageException("Info header hasn't been written: error of writing into file.");
    }
}
void ImageWriter::WritePixels(std::ofstream& file) {
    try {
        uint32_t line_size = img_->InfoHeader::width * 3;
        uint8_t padding = (4 - line_size % 4) % 4;

        std::vector<uint8_t> buffer(line_size + padding, 0);
        for (uint32_t row = 0; row < img_->InfoHeader::height; ++row) {
            uint32_t real_row = img_->InfoHeader::height - row - 1;

            for (int col = 0; col < img_->InfoHeader::width; ++col) {
                int offset = col * 3;

                buffer[offset] = GetByte(img_->pixels[real_row][col].b_);
                buffer[offset + 1] = GetByte(img_->pixels[real_row][col].g_);
                buffer[offset + 2] = GetByte(img_->pixels[real_row][col].r_);
            }

            Write(file, buffer);
        }
    } catch (std::exception& e) {
        throw ReadImageException("Image hasn't been read: error of reading pixels.");
    }
}

void ImageWriter::WriteFile(const std::string& path) {
    std::ofstream file(path, std::ios::binary);

    if (!file.is_open()) {
        throw WriteImageException("Cannot open write file.");
    }

    WriteFileHeader(file);
    WriteInfoHeader(file);

    WritePixels(file);
}
