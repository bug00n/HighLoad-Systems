#pragma once

#include <vector>

struct Matrix {
    std::vector<std::vector<double>> matrix;

    void Transpose();
};