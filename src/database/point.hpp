#ifndef POINT_HPP
#define POINT_HPP

#include <boost/geometry/geometries/point_xy.hpp>

using Point = boost::geometry::model::d2::point_xy<double>;

std::ostream& operator<<(std::ostream& os, const Point& point);

bool operator==(const Point& point_a, const Point& point_b);
Point operator+(const Point& point_a, const Point& point_b);
Point operator-(const Point& point_a, const Point& point_b);

double ComputeManhattanDistanceBetweenPoints(const Point& point_a,
                                             const Point& point_b);

#endif
