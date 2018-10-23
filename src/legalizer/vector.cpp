#include "vector.hpp"

#include <cmath>

using namespace std;

using Index = Eigen::Index;

Vector AbsVector(const Vector& vector) {
  Vector abs_vector(vector);

  for (Index i = 0; i < abs_vector.size(); ++i) {
    abs_vector(i) = abs(vector(i));
  }

  return abs_vector;
}

bool operator<(const Vector& vector_a, const Vector& vector_b) {
  for (Index i = 0; i < vector_a.size(); ++i) {
    if (vector_a(i) >= vector_b(i)) {
      return false;
    }
  }

  return true;
}
