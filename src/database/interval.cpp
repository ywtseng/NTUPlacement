#include "interval.hpp"

#include "../util/const.hpp"

#include <algorithm>
#include <cassert>

using namespace std;

Interval::Interval()
    : row_id_(UNDEFINED_ID),
      begin_(0.0),
      end_(0.0),
      capacity_(end_ - begin_),
      fence_region_id_(UNDEFINED_ID),
      x_and_sub_instance_id_sorted_by_x_(),
      cluster_x_and_overlap_by_right_x_() {
}

Interval::Interval(RowId row_id, double begin, double end,
                   FenceRegionId fence_region_id)
    : row_id_(row_id),
      begin_(begin),
      end_(end),
      capacity_(end_ - begin_),
      fence_region_id_(fence_region_id),
      x_and_sub_instance_id_sorted_by_x_(),
      cluster_x_and_overlap_by_right_x_() {
}

void Interval::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Layer:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "row_id_: " << row_id_
     << endl;
  os << string(indent_level * indent_size, ' ') << "begin_: " << begin_ << endl;
  os << string(indent_level * indent_size, ' ') << "end_: " << end_ << endl;
  os << string(indent_level * indent_size, ' ') << "capacity_: " << capacity_
     << endl;
  os << string(indent_level * indent_size, ' ')
     << "fence_region_id_: " << fence_region_id_ << endl;

  os << string(indent_level * indent_size, ' ')
     << "x_and_sub_instance_id_sorted_by_x_:" << endl;
  ++indent_level;
  for (int i = 0; i < x_and_sub_instance_id_sorted_by_x_.size(); ++i) {
    os << string(indent_level * indent_size, ' ') << "[" << i << "]"
       << ": "
       << "(" << x_and_sub_instance_id_sorted_by_x_[i].first << ", "
       << x_and_sub_instance_id_sorted_by_x_[i].second << ")" << endl;
  }
  --indent_level;
}

// Getters

RowId Interval::row_id() const {
  return row_id_;
}

double Interval::begin() const {
  return begin_;
}

double Interval::end() const {
  return end_;
}

double Interval::capacity() const {
  return capacity_;
}

FenceRegionId Interval::fence_region_id() const {
  return fence_region_id_;
}

SubInstanceId Interval::first_sub_instance_id() const {
  return x_and_sub_instance_id_sorted_by_x_.front().second;
}

SubInstanceId Interval::last_sub_instance_id() const {
  return x_and_sub_instance_id_sorted_by_x_.back().second;
}

int Interval::num_sub_instances() const {
  return x_and_sub_instance_id_sorted_by_x_.size();
}

SubInstanceId Interval::sub_instance_id(int idx) const {
  return x_and_sub_instance_id_sorted_by_x_.at(idx).second;
}

double Interval::ComputeAccumulatedOverlap(double sub_instance_x,
                                           double sub_instance_width) const {
  const double sub_instance_right_x = sub_instance_x + sub_instance_width;

  auto it = cluster_x_and_overlap_by_right_x_.upper_bound(sub_instance_x);

  if (it == cluster_x_and_overlap_by_right_x_.end()) {
    return 0.0;
  }

  const double cluster_right_x = it->first;
  const auto& cluster_left_x_and_overlap = it->second;
  const double cluster_left_x = cluster_left_x_and_overlap.first;
  const double cluster_overlap = cluster_left_x_and_overlap.second;

  assert(cluster_left_x < sub_instance_right_x);

  const double overlap =
      cluster_overlap + (min(cluster_right_x, sub_instance_right_x) -
                         max(cluster_left_x, sub_instance_x));

  return overlap;
}

// Setters

void Interval::set_begin(double begin) {
  capacity_ += (begin_ - begin);
  begin_ = begin;
}

void Interval::set_end(double end) {
  capacity_ += (end - end_);
  end_ = end;
}

void Interval::set_fence_region_id(FenceRegionId id) {
  fence_region_id_ = id;
}

void Interval::add_sub_instance_id(SubInstanceId id, double x, double width) {
  capacity_ -= width;

  x_and_sub_instance_id_sorted_by_x_.push_back(make_pair(x, id));

  const double right_x = x + width;

  auto it = cluster_x_and_overlap_by_right_x_.upper_bound(x);

  if (it == cluster_x_and_overlap_by_right_x_.end()) {
    cluster_x_and_overlap_by_right_x_.insert(
        it, make_pair(right_x, make_pair(x, 0.0)));
  } else {
    const double cluster_right_x = it->first;
    const auto& cluster_left_x_and_overlap = it->second;
    const double cluster_left_x = cluster_left_x_and_overlap.first;
    const double cluster_overlap = cluster_left_x_and_overlap.second;

    assert(cluster_left_x < right_x);

    const double overlap = cluster_overlap + (min(cluster_right_x, right_x) -
                                              max(cluster_left_x, x));

    it = cluster_x_and_overlap_by_right_x_.erase(it);
    cluster_x_and_overlap_by_right_x_.insert(
        it, make_pair(right_x, make_pair(cluster_left_x, overlap)));
  }
}

void Interval::remove_sub_instance_ids() {
  capacity_ = end_ - begin_;
  x_and_sub_instance_id_sorted_by_x_.clear();
  cluster_x_and_overlap_by_right_x_.clear();
}

void Interval::SortSubInstancesByX() {
  sort(x_and_sub_instance_id_sorted_by_x_.begin(),
       x_and_sub_instance_id_sorted_by_x_.end(),
       [](const pair<double, SubInstanceId>& pair_a,
          const pair<double, SubInstanceId>& pair_b) {
         return pair_a.first < pair_b.first;
       });
}
