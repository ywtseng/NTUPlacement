#ifndef SPARSE_MATRIX_HPP
#define SPARSE_MATRIX_HPP

#include "vector.hpp"

#include <Eigen/Sparse>

using SparseMatrix = Eigen::SparseMatrix<double>;

bool IsMatrixSymmetric(const SparseMatrix& matrix);
SparseMatrix MakeIdentityMatrix(Eigen::Index num_rows);
SparseMatrix ConcatenateMatricesVertically(const SparseMatrix& upper_matrix,
                                           const SparseMatrix& lower_matrix);
SparseMatrix ConcatenateMatricesHorizontally(const SparseMatrix& left_matrix,
                                             const SparseMatrix& right_matrix);
SparseMatrix SolveLinearSystem(const SparseMatrix& A, const SparseMatrix& B);
SparseMatrix InverseMatrix(const SparseMatrix& matrix);
SparseMatrix InverseDiagonalMatrix(const SparseMatrix& diagonal_matrix);
SparseMatrix TridiagMatrix(const SparseMatrix& matrix);
Vector ConvertMatrixToVector(const SparseMatrix& matrix);

#endif
