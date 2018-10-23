#include "site.hpp"

#include "../util/const.hpp"

using namespace std;

double Site::width_ = 0.0;
double Site::height_ = 0.0;

double Site::width() {
  return width_;
}

double Site::height() {
  return height_;
}

void Site::set_size(double width, double height) {
  width_ = width;
  height_ = height;
}

Site::Site()
    : position_(ORIGIN_POINT),
      is_valid_(false),
      fence_region_id_(UNDEFINED_ID),
      sub_instance_id_(UNDEFINED_ID) {
}

Site::Site(const Point& position)
    : position_(position),
      is_valid_(false),
      fence_region_id_(UNDEFINED_ID),
      sub_instance_id_(UNDEFINED_ID) {
}

void Site::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Site:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "position_: " << position_
     << endl;
  os << string(indent_level * indent_size, ' ') << "is_valid_: " << is_valid_
     << endl;
  os << string(indent_level * indent_size, ' ')
     << "fence_region_id_: " << fence_region_id_ << endl;
  os << string(indent_level * indent_size, ' ')
     << "sub_instance_id_: " << sub_instance_id_ << endl;
}

// Getters


int Site::num_variables() const {
  return variable_ids_.size();
}

const Point& Site::position() const {
  return position_;
}

bool Site::is_valid() const {
  return is_valid_;
}

FenceRegionId Site::fence_region_id() const {
  return fence_region_id_;
}

bool Site::has_sub_instance() const {
  return sub_instance_id_ != UNDEFINED_ID;
}

SubInstanceId Site::sub_instance_id() const {
  return sub_instance_id_;
}

SiteId Site::site_id() const {
  return site_id_;
}

VariableId Site::variable_id(int idx) const {
  return variable_ids_.at(idx);
}

int Site::variable_layer(int idx) const {
  return variable_layers_.at(idx);
}

// Setters

void Site::set_is_valid(bool is_valid) {
  is_valid_ = is_valid;
}

void Site::set_fence_region_id(FenceRegionId fence_region_id) {
  fence_region_id_ = fence_region_id;
}

void Site::set_sub_instance_id(SubInstanceId sub_instance_id) {
  sub_instance_id_ = sub_instance_id;
}

void Site::remove_sub_instance_id() {
  sub_instance_id_ = UNDEFINED_ID;
}

void Site::set_site_id(SiteId id) {
  site_id_ = id;
}

void Site::add_variable_id(VariableId id) {
  variable_ids_.push_back(id);
}

void Site::add_variable_layer(int id) {
  variable_layers_.push_back(id);
}