#include "point.hpp"

#include <boost/geometry.hpp>

using namespace std;
namespace bg = boost::geometry;

ostream& operator<<(ostream& os, const Point& point) {
  os << "(" << point.x() << ", " << point.y() << ")";

  return os;
}

bool operator==(const Point& point_a, const Point& point_b) {
  return bg::equals(point_a, point_b);
}

Point operator+(const Point& point_a, const Point& point_b) {
  Point result(point_a);

  bg::add_point(result, point_b);

  return result;
}

Point operator-(const Point& point_a, const Point& point_b) {
  Point result(point_a);

  bg::subtract_point(result, point_b);

  return result;
}

double ComputeManhattanDistanceBetweenPoints(const Point& point_a,
                                             const Point& point_b) {
  return (abs(point_a.x() - point_b.x()) + abs(point_a.y() - point_b.y()));
}
