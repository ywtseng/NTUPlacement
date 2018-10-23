#include "sparse_matrix.hpp"

#include <Eigen/SparseCholesky>

#include <cassert>
#include <cmath>

using namespace std;

using Index = Eigen::Index;
using Triplet = Eigen::Triplet<double>;

bool IsMatrixSymmetric(const SparseMatrix& matrix) {
  assert(matrix.rows() == matrix.cols());

  for (Index i = 0; i < matrix.outerSize(); ++i) {
    for (SparseMatrix::InnerIterator it(matrix, i); it; ++it) {
      if (it.value() != matrix.coeff(it.col(), it.row())) {
        return false;
      }
    }
  }

  return true;
}

SparseMatrix MakeIdentityMatrix(Index num_rows) {
  SparseMatrix identity(num_rows, num_rows);
  identity.setIdentity();

  return identity;
}

SparseMatrix ConcatenateMatricesVertically(const SparseMatrix& upper_matrix,
                                           const SparseMatrix& lower_matrix) {
  assert(upper_matrix.cols() == lower_matrix.cols());

  vector<Triplet> non_zero_elements;
  non_zero_elements.reserve(upper_matrix.nonZeros() + lower_matrix.nonZeros());

  for (Index i = 0; i < upper_matrix.outerSize(); ++i) {
    for (SparseMatrix::InnerIterator it(upper_matrix, i); it; ++it) {
      non_zero_elements.push_back(Triplet(it.row(), it.col(), it.value()));
    }
  }

  for (Index i = 0; i < lower_matrix.outerSize(); ++i) {
    for (SparseMatrix::InnerIterator it(lower_matrix, i); it; ++it) {
      non_zero_elements.push_back(
          Triplet(upper_matrix.rows() + it.row(), it.col(), it.value()));
    }
  }

  SparseMatrix concatenated_matrix(upper_matrix.rows() + lower_matrix.rows(),
                                   upper_matrix.cols());
  concatenated_matrix.setFromTriplets(non_zero_elements.begin(),
                                      non_zero_elements.end());

  return concatenated_matrix;
}

SparseMatrix ConcatenateMatricesHorizontally(const SparseMatrix& left_matrix,
                                             const SparseMatrix& right_matrix) {
  assert(left_matrix.rows() == right_matrix.rows());

  vector<Triplet> non_zero_elements;
  non_zero_elements.reserve(left_matrix.nonZeros() + right_matrix.nonZeros());

  for (Index i = 0; i < left_matrix.outerSize(); ++i) {
    for (SparseMatrix::InnerIterator it(left_matrix, i); it; ++it) {
      non_zero_elements.push_back(Triplet(it.row(), it.col(), it.value()));
    }
  }

  for (Index i = 0; i < right_matrix.outerSize(); ++i) {
    for (SparseMatrix::InnerIterator it(right_matrix, i); it; ++it) {
      non_zero_elements.push_back(
          Triplet(it.row(), left_matrix.cols() + it.col(), it.value()));
    }
  }

  SparseMatrix concatenated_matrix(left_matrix.rows(),
                                   left_matrix.cols() + right_matrix.cols());
  concatenated_matrix.setFromTriplets(non_zero_elements.begin(),
                                      non_zero_elements.end());

  return concatenated_matrix;
}

// Solve AX = B.
SparseMatrix SolveLinearSystem(const SparseMatrix& A, const SparseMatrix& B) {
  SparseMatrix X;

  Eigen::SparseLU<SparseMatrix> solver;
  solver.compute(A);

  X = solver.solve(B);

  return X;
}

SparseMatrix InverseMatrix(const SparseMatrix& matrix) {
  assert(matrix.rows() == matrix.cols());

  return SolveLinearSystem(matrix, MakeIdentityMatrix(matrix.rows()));
}

SparseMatrix InverseDiagonalMatrix(const SparseMatrix& diagonal_matrix) {
  assert(diagonal_matrix.rows() == diagonal_matrix.cols());

  vector<Triplet> non_zero_elements;
  non_zero_elements.reserve(diagonal_matrix.nonZeros());

  for (Index i = 0; i < diagonal_matrix.outerSize(); ++i) {
    for (SparseMatrix::InnerIterator it(diagonal_matrix, i); it; ++it) {
      if (it.value() == 0) {
        continue;
      }

      assert(it.row() == it.col());

      non_zero_elements.push_back(Triplet(it.row(), it.col(), 1 / it.value()));
    }
  }

  SparseMatrix inversed_diagonal_matrix(diagonal_matrix.rows(),
                                        diagonal_matrix.cols());
  inversed_diagonal_matrix.setFromTriplets(non_zero_elements.begin(),
                                           non_zero_elements.end());

  return inversed_diagonal_matrix;
}

SparseMatrix TridiagMatrix(const SparseMatrix& matrix) {
  vector<Triplet> non_zero_elements;
  non_zero_elements.reserve(matrix.nonZeros());

  for (Index i = 0; i < matrix.outerSize(); ++i) {
    for (SparseMatrix::InnerIterator it(matrix, i); it; ++it) {
      if ((it.row() == it.col()) || (it.row() == it.col() + 1) ||
          (it.row() + 1 == it.col())) {
        non_zero_elements.push_back(Triplet(it.row(), it.col(), it.value()));
      }
    }
  }

  SparseMatrix tridiag_matrix(matrix.rows(), matrix.cols());
  tridiag_matrix.setFromTriplets(non_zero_elements.begin(),
                                 non_zero_elements.end());

  return tridiag_matrix;
}

Vector ConvertMatrixToVector(const SparseMatrix& matrix) {
  assert(matrix.cols() == 1);

  Vector v(matrix.rows());
  for (Index i = 0; i < matrix.outerSize(); ++i) {
    for (SparseMatrix::InnerIterator it(matrix, i); it; ++it) {
      v(it.row()) = it.value();
    }
  }

  return v;
}
