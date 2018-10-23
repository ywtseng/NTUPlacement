#ifndef POLYGON_HPP
#define POLYGON_HPP

#include "rect.hpp"

#include <boost/geometry/geometries/multi_polygon.hpp>
#include <boost/geometry/geometries/polygon.hpp>

using Polygon = boost::geometry::model::polygon<Point>;
using MultiPolygon = boost::geometry::model::multi_polygon<Polygon>;

std::ostream& operator<<(std::ostream& os, const Polygon& polygon);
std::ostream& operator<<(std::ostream& os, const MultiPolygon& multi_polygon);

Polygon ConvertRectToPolygon(const Rect& rect);

bool IsPolygonCoveredbyMutliPolygon(const Polygon& polygon,
                                    const MultiPolygon& multi_polygon);

double ComputeMultiPolygonArea(const MultiPolygon& multi_polygon);

Rect EnvelopeMultiPolygon(const MultiPolygon& multi_polygon);
MultiPolygon UnionRects(std::vector<Rect> rects);
MultiPolygon IntersectPolygons(const MultiPolygon& multi_polygon,
                               const Polygon& polygon);
MultiPolygon DifferencePolygons(const MultiPolygon& multi_polygon,
                                const Polygon& polygon);

#endif
