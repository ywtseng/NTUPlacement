#include "line.hpp"

#include <algorithm>
#include <cassert>

using namespace std;

ostream& operator<<(ostream& os, const Line& line) {
  os << line.first << " - " << line.second;

  return os;
}

bool IsLineVertical(const Line& line) {
  return line.first.x() == line.second.x();
}

bool IsLineHorizontal(const Line& line) {
  return line.first.y() == line.second.y();
}

bool AreLinesOverlappedVertically(const Line& line_a, const Line& line_b) {
  assert(IsLineVertical(line_a) && IsLineVertical(line_b));

  const double line_a_lower_y = min(line_a.first.y(), line_a.second.y());
  const double line_a_upper_y = max(line_a.first.y(), line_a.second.y());
  const double line_b_lower_y = min(line_b.first.y(), line_b.second.y());
  const double line_b_upper_y = max(line_b.first.y(), line_b.second.y());

  return (line_a_lower_y < line_b_upper_y) && (line_a_upper_y > line_b_lower_y);
}

bool AreLinesOverlappedHorizontally(const Line& line_a, const Line& line_b) {
  assert(IsLineHorizontal(line_a) && IsLineHorizontal(line_b));

  const double line_a_left_x = min(line_a.first.x(), line_a.second.x());
  const double line_a_right_x = max(line_a.first.x(), line_a.second.x());
  const double line_b_left_x = min(line_b.first.x(), line_b.second.x());
  const double line_b_right_x = max(line_b.first.x(), line_b.second.x());

  return (line_a_left_x < line_b_right_x) && (line_a_right_x > line_b_left_x);
}

double ComputeVerticalOverlapBetweenLines(const Line& line_a,
                                          const Line& line_b) {
  assert(IsLineVertical(line_a) && IsLineVertical(line_b));

  const double line_a_lower_y = min(line_a.first.y(), line_a.second.y());
  const double line_a_upper_y = max(line_a.first.y(), line_a.second.y());
  const double line_b_lower_y = min(line_b.first.y(), line_b.second.y());
  const double line_b_upper_y = max(line_b.first.y(), line_b.second.y());

  return min(line_a_upper_y, line_b_upper_y) -
         max(line_a_lower_y, line_b_lower_y);
}

double ComputeHorizontalOverlapBetweenLines(const Line& line_a,
                                            const Line& line_b) {
  assert(IsLineHorizontal(line_a) && IsLineHorizontal(line_b));

  const double line_a_left_x = min(line_a.first.x(), line_a.second.x());
  const double line_a_right_x = max(line_a.first.x(), line_a.second.x());
  const double line_b_left_x = min(line_b.first.x(), line_b.second.x());
  const double line_b_right_x = max(line_b.first.x(), line_b.second.x());

  return min(line_a_right_x, line_b_right_x) -
         max(line_a_left_x, line_b_left_x);
}
