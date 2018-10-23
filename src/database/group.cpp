#include "group.hpp"

#include "../util/const.hpp"

using namespace std;

Group::Group()
    : name_(UNDEFINED_STRING), fence_region_id_(UNDEFINED_ID), patterns_() {
}

Group::Group(const string& name)
    : name_(name), fence_region_id_(UNDEFINED_ID), patterns_() {
}

void Group::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Group:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "name_: " << name_ << endl;
  os << string(indent_level * indent_size, ' ')
     << "fence_region_id_: " << fence_region_id_ << endl;
  os << string(indent_level * indent_size, ' ') << "patterns_:" << endl;
  ++indent_level;
  for (int i = 0; i < patterns_.size(); ++i) {
    os << string(indent_level * indent_size, ' ') << "[" << i << "]"
       << ": " << patterns_[i] << endl;
  }
  --indent_level;
}

// Getters

const string& Group::name() const {
  return name_;
}

FenceRegionId Group::fence_region_id() const {
  return fence_region_id_;
}

int Group::num_patterns() const {
  return patterns_.size();
}

const string& Group::pattern(int id) const {
  return patterns_.at(id);
}

// Setters

void Group::set_fence_region_id(FenceRegionId id) {
  fence_region_id_ = id;
}

void Group::add_pattern(const string& pattern) {
  patterns_.push_back(pattern);
}
