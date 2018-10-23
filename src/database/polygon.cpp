#include "polygon.hpp"

#include <boost/geometry.hpp>

using namespace std;
namespace bg = boost::geometry;

ostream& operator<<(ostream& os, const Polygon& polygon) {
  os << "(";
  for (const Point& point : polygon.outer()) {
    os << point << ", ";
  }
  os << "), ";

  for (const auto& ring : polygon.inners()) {
    os << "(";
    for (const Point& point : ring) {
      os << point << ", ";
    }
    os << "), ";
  }

  return os;
}

ostream& operator<<(ostream& os, const MultiPolygon& multi_polygon) {
  os << "[";
  for (const Polygon& polygon : multi_polygon) {
    os << polygon << ", ";
  }
  os << "]";

  return os;
}

Polygon ConvertRectToPolygon(const Rect& rect) {
  Polygon polygon;

  bg::convert(rect, polygon);

  return polygon;
}

bool IsPolygonCoveredbyMutliPolygon(const Polygon& polygon,
                                    const MultiPolygon& multi_polygon) {
  return bg::covered_by(polygon, multi_polygon);
}

double ComputeMultiPolygonArea(const MultiPolygon& multi_polygon) {
  return bg::area(multi_polygon);
}

Rect EnvelopeMultiPolygon(const MultiPolygon& multi_polygon) {
  Rect result;

  bg::envelope(multi_polygon, result);

  return result;
}

MultiPolygon UnionRects(vector<Rect> rects) {
  assert(rects.size() > 0);

  MultiPolygon result;
  bg::convert(rects[0], result);

  for (int i = 1; i < rects.size(); ++i) {
    MultiPolygon tmp_result;
    bg::union_(result, rects[i], tmp_result);
    result = tmp_result;
  }

  return result;
}

MultiPolygon IntersectPolygons(const MultiPolygon& multi_polygon,
                               const Polygon& polygon) {
  MultiPolygon result;

  bg::intersection(multi_polygon, polygon, result);

  return result;
}

MultiPolygon DifferencePolygons(const MultiPolygon& multi_polygon,
                                const Polygon& polygon) {
  MultiPolygon result;

  bg::difference(multi_polygon, polygon, result);

  return result;
}
