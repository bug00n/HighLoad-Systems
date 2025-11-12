#include "image.h"
#include <iostream>
#include "pixel.h"

void Normalize(double& x) {
    x = std::min(1.0, std::max(0.0, x));
}

bool InSquare(int32_t c_row, int32_t c_col, int32_t rows, int32_t cols) {
    return 0 <= c_row && c_row < rows && 0 <= c_col && c_col < cols;
}

void Image::Multiply(const Matrix& matrix) {
    const int32_t m_row = static_cast<int32_t>(matrix.matrix.size());
    const int32_t mid_row = m_row / 2;
    const int32_t m_col = static_cast<int32_t>(matrix.matrix[0].size());
    const int32_t mid_col = m_col / 2;

    int32_t c_row = 0;
    int32_t c_col = 0;

    const int32_t maxh = static_cast<int32_t>(InfoHeader::height);
    const int32_t maxw = static_cast<int32_t>(InfoHeader::width);

    for (int32_t row = 0; row < maxh; ++row) {
        for (int32_t col = 0; col < maxw; ++col) {
            buffer[row][col].r_ = 0.0;
            buffer[row][col].g_ = 0.0;
            buffer[row][col].b_ = 0.0;
            for (int32_t d_row = -mid_row; d_row <= mid_row; ++d_row) {
                for (int32_t d_col = -mid_col; d_col <= mid_col; ++d_col) {
                    c_row = row + d_row;
                    c_col = col + d_col;

                    if (c_row < 0) {
                        c_row = -c_row;
                    }

                    if (InfoHeader::height <= c_row) {
                        c_row = maxh - 1 - c_row;
                    }

                    if (c_col < 0) {
                        c_col = -c_col;
                    }

                    if (InfoHeader::width <= c_col) {
                        c_col = maxw - 1 - c_col;
                    }

                    if (!InSquare(c_row, c_col, maxh, maxw)) {
                        c_row = row;
                        c_col = col;
                    }

                    double coeff = matrix.matrix[d_row + mid_row][d_col + mid_col];
                    const Pixel& px = pixels[c_row][c_col];

                    buffer[row][col].r_ += coeff * px.r_;
                    buffer[row][col].g_ += coeff * px.g_;
                    buffer[row][col].b_ += coeff * px.b_;
                }
            }

            Normalize(buffer[row][col].r_);
            Normalize(buffer[row][col].g_);
            Normalize(buffer[row][col].b_);
        }
    }
    std::swap(pixels, buffer);
}
