#ifndef FENCE_REGION_HPP
#define FENCE_REGION_HPP

#include "instance.hpp"
#include "polygon.hpp"
#include "rect.hpp"

#include <iostream>
#include <limits>
#include <string>
#include <vector>

class FenceRegion {
 public:
  FenceRegion();
  FenceRegion(const std::string& name);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  const std::string& name() const;
  // UnionRects must be called beforehand or it will return an empty
  // MultiPolygon.
  const MultiPolygon& multi_polygon() const;

  int num_rects() const;

  const Rect& rect(int idx) const;

  // Setters

  void add_rect(const Rect& rect);

  void UnionRects();

 private:
  std::string name_;
  std::vector<Rect> rects_;
  MultiPolygon multi_polygon_;
};

#endif
