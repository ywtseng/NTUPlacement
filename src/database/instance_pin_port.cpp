#include "instance_pin_port.hpp"

#include "../util/const.hpp"

using namespace std;

InstancePinPort::InstancePinPort()
    : instance_pin_id_(UNDEFINED_ID), rect_(ORIGIN_POINT, ORIGIN_POINT) {
}

InstancePinPort::InstancePinPort(InstancePinId instance_pin_id,
                                 const Rect& rect)
    : instance_pin_id_(instance_pin_id), rect_(rect) {
}

void InstancePinPort::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "InstancePinPort:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ')
     << "instance_pin_id_: " << instance_pin_id_ << endl;
  os << string(indent_level * indent_size, ' ') << "rect_: " << rect_ << endl;
}

// Getters

InstancePinId InstancePinPort::instance_pin_id() const {
  return instance_pin_id_;
}

const Rect& InstancePinPort::rect() const {
  return rect_;
}

// Setters

void InstancePinPort::set_rect(const Rect& rect) {
  rect_ = rect;
}
