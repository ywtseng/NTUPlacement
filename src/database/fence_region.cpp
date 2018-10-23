#include "fence_region.hpp"

#include "../util/const.hpp"

#include <climits>
#include <vector>

using namespace std;

FenceRegion::FenceRegion()
    : name_(UNDEFINED_STRING), rects_(), multi_polygon_() {
}

FenceRegion::FenceRegion(const string& name)
    : name_(name), rects_(), multi_polygon_() {
}

void FenceRegion::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "FenceRegion:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "name_: " << name_ << endl;
  os << string(indent_level * indent_size, ' ') << "rects_:" << endl;
  ++indent_level;
  for (const Rect& rect : rects_) {
    os << string(indent_level * indent_size, ' ') << rect << endl;
  }
  os << string(indent_level * indent_size, ' ')
     << "multi_polygon_: " << multi_polygon_ << endl;
}

// Getters

const string& FenceRegion::name() const {
  return name_;
}

const MultiPolygon& FenceRegion::multi_polygon() const {
  return multi_polygon_;
}

int FenceRegion::num_rects() const {
  return rects_.size();
}

const Rect& FenceRegion::rect(int id) const {
  return rects_.at(id);
}

// Setters

void FenceRegion::add_rect(const Rect& rect) {
  rects_.push_back(rect);
}

void FenceRegion::UnionRects() {
  multi_polygon_ = ::UnionRects(rects_);
}
