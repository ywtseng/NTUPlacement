#include "instance_pin.hpp"

#include "../util/const.hpp"

using namespace std;

InstancePin::InstancePin()
    : instance_id_(UNDEFINED_ID),
      cell_pin_id_(UNDEFINED_ID),
      name_(UNDEFINED_STRING),
      ports_() {
}

InstancePin::InstancePin(InstanceId instance_id, CellPinId cell_pin_id,
                         const string& name)
    : instance_id_(instance_id),
      cell_pin_id_(cell_pin_id),
      name_(name),
      ports_() {
}

void InstancePin::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "InstancePin:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ')
     << "instance_id_: " << instance_id_ << endl;
  os << string(indent_level * indent_size, ' ')
     << "cell_pin_id_: " << cell_pin_id_ << endl;
  os << string(indent_level * indent_size, ' ') << "name_: " << name_ << endl;
  os << string(indent_level * indent_size, ' ') << "ports_:" << endl;
  ++indent_level;
  for (int i = 0; i < ports_.size(); ++i) {
    os << string(indent_level * indent_size, ' ') << "[" << i << "]"
       << ":" << endl;
    ++indent_level;
    ports_[i].Print(os, indent_level);
    --indent_level;
  }
}

// Getters

InstanceId InstancePin::instance_id() const {
  return instance_id_;
}

CellPinId InstancePin::cell_pin_id() const {
  return cell_pin_id_;
}

const string& InstancePin::name() const {
  return name_;
}

int InstancePin::num_ports() const {
  return ports_.size();
}

const InstancePinPort& InstancePin::port(int idx) const {
  return ports_.at(idx);
}

// Setters

InstancePinPort& InstancePin::port(int idx) {
  return ports_.at(idx);
}

void InstancePin::add_port(const InstancePinPort& port) {
  ports_.push_back(port);
}
