#include "net.hpp"

#include "../util/const.hpp"

using namespace std;

Net::Net() : name_(UNDEFINED_STRING), pin_ids_(), instance_pin_ids_() {
}

Net::Net(const string& name) : name_(name), pin_ids_(), instance_pin_ids_() {
}

void Net::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Net:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "name_: " << name_ << endl;
  os << string(indent_level * indent_size, ' ') << "pin_ids_:" << endl;
  ++indent_level;
  for (int i = 0; i < pin_ids_.size(); ++i) {
    os << string(indent_level * indent_size, ' ') << "[" << i << "]"
       << ": " << pin_ids_[i] << endl;
  }
  --indent_level;
  os << string(indent_level * indent_size, ' ') << "instance_pin_ids_:" << endl;
  ++indent_level;
  for (int i = 0; i < instance_pin_ids_.size(); ++i) {
    os << string(indent_level * indent_size, ' ') << "[" << i << "]"
       << ": " << instance_pin_ids_[i] << endl;
  }
  --indent_level;
}

// Getters

const string& Net::name() const {
  return name_;
}

int Net::num_pins() const {
  return pin_ids_.size();
}

int Net::num_instance_pins() const {
  return instance_pin_ids_.size();
}

PinId Net::pin_id(int idx) const {
  return pin_ids_.at(idx);
}

InstancePinId Net::instance_pin_id(int idx) const {
  return instance_pin_ids_.at(idx);
}

// Setters

void Net::add_pin_id(PinId id) {
  pin_ids_.push_back(id);
}

void Net::add_instance_pin_id(InstancePinId id) {
  instance_pin_ids_.push_back(id);
}
