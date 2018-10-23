#include "rect.hpp"

#include "line.hpp"

using namespace std;

ostream& operator<<(ostream& os, const Rect& rect) {
  os << rect.min_corner() << " - " << rect.max_corner();

  return os;
}

Rect MakeRect(const Point& lower_left_point, double width, double height) {
  return Rect(lower_left_point, lower_left_point + Point(width, height));
}

bool AreRectsOverlapped(const Rect& rect_a, const Rect& rect_b) {
  return AreLinesOverlappedVertically(
             Line(Point(rect_a.min_corner().x(), rect_a.min_corner().y()),
                  Point(rect_a.min_corner().x(), rect_a.max_corner().y())),
             Line(Point(rect_b.min_corner().x(), rect_b.min_corner().y()),
                  Point(rect_b.min_corner().x(), rect_b.max_corner().y()))) &&
         AreLinesOverlappedHorizontally(
             Line(Point(rect_a.min_corner().x(), rect_a.min_corner().y()),
                  Point(rect_a.max_corner().x(), rect_a.min_corner().y())),
             Line(Point(rect_b.min_corner().x(), rect_b.min_corner().y()),
                  Point(rect_b.max_corner().x(), rect_b.min_corner().y())));
}

double ComputeOverlapBetweenRects(const Rect& rect_a, const Rect& rect_b) {
  assert(AreRectsOverlapped(rect_a, rect_b));

  return ComputeVerticalOverlapBetweenLines(
             Line(Point(rect_a.min_corner().x(), rect_a.min_corner().y()),
                  Point(rect_a.min_corner().x(), rect_a.max_corner().y())),
             Line(Point(rect_b.min_corner().x(), rect_b.min_corner().y()),
                  Point(rect_b.min_corner().x(), rect_b.max_corner().y()))) *
         ComputeHorizontalOverlapBetweenLines(
             Line(Point(rect_a.min_corner().x(), rect_a.min_corner().y()),
                  Point(rect_a.max_corner().x(), rect_a.min_corner().y())),
             Line(Point(rect_b.min_corner().x(), rect_b.min_corner().y()),
                  Point(rect_b.max_corner().x(), rect_b.min_corner().y())));
}
