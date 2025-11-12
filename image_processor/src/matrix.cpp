#include <cmath>
#include "matrix.h"

void Matrix::Transpose() {
    std::vector<std::vector<double>> n_matrix(matrix[0].size(), std::vector<double>(matrix.size()));
    for (int32_t i = 0; i < matrix.size(); ++i) {
        for (int32_t j = 0; j < matrix[0].size(); ++j) {
            n_matrix[j][i] = matrix[i][j];
        }
    }
    std::swap(matrix, n_matrix);
}