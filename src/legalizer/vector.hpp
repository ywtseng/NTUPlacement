#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <Eigen/Dense>

using Vector = Eigen::VectorXd;

Vector AbsVector(const Vector& vector);

bool operator<(const Vector& vector_a, const Vector& vector_b);

#endif
