#include "row.hpp"

#include "../util/const.hpp"

#include <algorithm>

using namespace std;

Row::Row()
    : name_(UNDEFINED_STRING),
      position_(ORIGIN_POINT),
      orientation_(Orientation::N),
      rail_id_by_layer_id_(),
      interval_id_by_begin_by_fence_region_id_() {
}

Row::Row(const string& name, const Point& position, Orientation orientation,
         int num_layers)
    : name_(name),
      position_(position),
      orientation_(orientation),
      rail_id_by_layer_id_(num_layers, RailId(UNDEFINED_ID)),
      interval_id_by_begin_by_fence_region_id_() {
}

void Row::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Row:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "name_: " << name_ << endl;
  os << string(indent_level * indent_size, ' ') << "position_: " << position_
     << endl;
  os << string(indent_level * indent_size, ' ')
     << "orientation_: " << orientation_ << endl;

  os << string(indent_level * indent_size, ' ')
     << "rail_id_by_layer_id_:" << endl;
  ++indent_level;
  for (int i = 0; i < rail_id_by_layer_id_.size(); ++i) {
    const LayerId layer_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << layer_id << "]"
       << ": " << rail_id_by_layer_id_[i] << endl;
  }
  --indent_level;
}

// Getters

const string& Row::name() const {
  return name_;
}

const Point& Row::position() const {
  return position_;
}

Orientation Row::orientation() const {
  return orientation_;
}

int Row::num_intervals() const {
  return begin_and_interval_id_sorted_by_begin_.size();
}

IntervalId Row::interval_id(int idx) const {
  return begin_and_interval_id_sorted_by_begin_.at(idx).second;
}

bool Row::has_rail_on_layer(LayerId id) const {
  return rail_id_by_layer_id_.at(id) != UNDEFINED_ID;
}

bool Row::has_interval_of_fence_region(FenceRegionId id) const {
  return interval_id_by_begin_by_fence_region_id_.at(id + 1).size() != 0;
}

bool Row::has_interval_of_fence_region_before(FenceRegionId fence_region_id,
                                              double x) const {
  auto interval_id_by_begin =
      interval_id_by_begin_by_fence_region_id_.at(fence_region_id + 1);

  if (interval_id_by_begin.size() == 0) {
    return false;
  }

  auto it = interval_id_by_begin.lower_bound(x);

  if (it == interval_id_by_begin.begin()) {
    return false;
  }

  return true;
}

bool Row::has_interval_of_fence_region_after(FenceRegionId fence_region_id,
                                             double x) const {
  auto interval_id_by_begin =
      interval_id_by_begin_by_fence_region_id_.at(fence_region_id + 1);

  if (interval_id_by_begin.size() == 0) {
    return false;
  }

  auto it = interval_id_by_begin.upper_bound(x);

  if (it == interval_id_by_begin.end()) {
    return false;
  }

  return true;
}

RailId Row::rail_id_on_layer(LayerId id) const {
  return rail_id_by_layer_id_.at(id);
}

IntervalId Row::interval_id_of_fence_region_before(
    FenceRegionId fence_region_id, double x) const {
  auto interval_id_by_begin =
      interval_id_by_begin_by_fence_region_id_.at(fence_region_id + 1);

  if (interval_id_by_begin.size() == 0) {
    return IntervalId(UNDEFINED_ID);
  }

  auto it = interval_id_by_begin.lower_bound(x);

  if (it == interval_id_by_begin.begin()) {
    return IntervalId(UNDEFINED_ID);
  }

  --it;

  return it->second;
}

IntervalId Row::interval_id_of_fence_region_after(FenceRegionId fence_region_id,
                                                  double x) const {
  auto interval_id_by_begin =
      interval_id_by_begin_by_fence_region_id_.at(fence_region_id + 1);

  if (interval_id_by_begin.size() == 0) {
    return IntervalId(UNDEFINED_ID);
  }

  auto it = interval_id_by_begin.lower_bound(x);

  if (it == interval_id_by_begin.end()) {
    return IntervalId(UNDEFINED_ID);
  }

  return it->second;
}

// Setters

void Row::set_rail_id_on_layer(RailId rail_id, LayerId layer_id) {
  rail_id_by_layer_id_.at(layer_id) = rail_id;
}

void Row::add_interval_id(IntervalId id, double begin,
                          FenceRegionId fence_region_id) {
  interval_id_by_begin_by_fence_region_id_.at(fence_region_id + 1)
      .insert(make_pair(begin, id));
  begin_and_interval_id_sorted_by_begin_.push_back(make_pair(begin, id));
}

void Row::prepare_for_adding_interval_id(int num_fence_regions) {
  interval_id_by_begin_by_fence_region_id_.resize(num_fence_regions + 1);
}

void Row::SortIntervalsByBegin() {
  sort(begin_and_interval_id_sorted_by_begin_.begin(),
       begin_and_interval_id_sorted_by_begin_.end(),
       [](const pair<double, IntervalId>& pair_a,
          const pair<double, IntervalId>& pair_b) {
         return pair_a.first < pair_b.first;
       });
}
