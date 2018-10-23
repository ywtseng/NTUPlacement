#include "bin.hpp"

#include "../util/const.hpp"

#include <cassert>

using namespace std;

Bin::Bin()
    : position_(ORIGIN_POINT),
      width_(0.0),
      instance_ids_(),
      free_area_(0.0),
      instances_area_(0.0),
      is_spreaded_(false) {
}

Bin::Bin(const Point& position, double width)
    : position_(position),
      width_(width),
      instance_ids_(),
      free_area_(width_ * width_),
      instances_area_(0.0),
      is_spreaded_(false) {
}

void Bin::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;

  os << string(indent_level * indent_size, ' ') << "Bin:" << endl;

  ++indent_level;

  os << string(indent_level * indent_size, ' ') << "position_: " << position_
     << endl;
  os << string(indent_level * indent_size, ' ') << "width_: " << width_ << endl;
  os << string(indent_level * indent_size, ' ') << "free_area_: " << free_area_
     << endl;
  os << string(indent_level * indent_size, ' ')
     << "instances_area_: " << instances_area_ << endl;

  os << string(indent_level * indent_size, ' ') << "instance_ids_:" << endl;
  ++indent_level;
  for (int i = 0; i < instance_ids_.size(); ++i) {
    os << string(indent_level * indent_size, ' ') << "[" << i << "]"
       << ": " << instance_ids_[i] << endl;
  }
  --indent_level;
}

// Getters

const Point& Bin::position() const {
  return position_;
}

double Bin::width() const {
  return width_;
}

double Bin::height() const {
  return width_;
}

const Point Bin::upper_right_position() const {
  return position_ + Point(width_, width_);
}

const Rect Bin::rect() const {
  return Rect(position_, position_ + Point(width_, width_));
}

double Bin::free_area() const {
  return free_area_;
}

double Bin::instances_area() const {
  return instances_area_;
}

double Bin::density() const {
  if (free_area_ == 0.0) {
    return 0.0;  // TODO: Does it make sense?
  }

  return instances_area_ / free_area_;
}

bool Bin::is_spreaded() const {
  return is_spreaded_;
}

int Bin::num_instances() const {
  return instance_ids_.size();
}

InstanceId Bin::instance_id(int idx) const {
  assert(idx >= 0 && idx < instance_ids_.size());

  return instance_ids_.at(idx);
}

// Setters

void Bin::set_free_area(double free_area) {
  free_area_ = free_area;
}

void Bin::set_is_spreaded(bool is_spreaded) {
  is_spreaded_ = is_spreaded;
}

void Bin::add_instance_id(InstanceId instance_id, bool is_instance_fixed,
                          double instance_area) {
  assert(instance_id != UNDEFINED_ID);
  assert(instance_area > 0.0);

  instance_ids_.push_back(instance_id);

  if (!is_instance_fixed) {
    instances_area_ += instance_area;
  } else {
    free_area_ -= instance_area;
  }
}

void Bin::remove_instance_ids() {
  instance_ids_.clear();  // TODO:
  free_area_ = width_ * width_ - instances_area_;
  instances_area_ = 0.0;
}
