#ifndef LINE_HPP
#define LINE_HPP

#include "point.hpp"

#include <boost/geometry/geometries/segment.hpp>

using Line = boost::geometry::model::segment<Point>;

std::ostream& operator<<(std::ostream& os, const Line& line);

bool IsLineVertical(const Line& line);
bool IsLineHorizontal(const Line& line);

// Check if two vertical parallel lines are overlapped.
// For example, lines a and b below
//
//   a  b
//
//   |
//   |
//   |  |
//   |  |
//      |
//      |
//
// are overlapped.
bool AreLinesOverlappedVertically(const Line& line_a, const Line& line_b);
bool AreLinesOverlappedHorizontally(const Line& line_a, const Line& line_b);

double ComputeVerticalOverlapBetweenLines(const Line& line_a,
                                          const Line& line_b);
double ComputeHorizontalOverlapBetweenLines(const Line& line_a,
                                            const Line& line_b);

#endif
