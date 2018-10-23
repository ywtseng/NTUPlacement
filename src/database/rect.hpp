#ifndef RECT_HPP
#define RECT_HPP

#include "point.hpp"

#include <boost/geometry/geometries/box.hpp>

using Rect = boost::geometry::model::box<Point>;

std::ostream& operator<<(std::ostream& os, const Rect& rect);

Rect MakeRect(const Point& lower_left_point, double width, double height);

bool AreRectsOverlapped(const Rect& rect_a, const Rect& rect_b);

double ComputeOverlapBetweenRects(const Rect& rect_a, const Rect& rect_b);

#endif
