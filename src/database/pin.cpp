#include "pin.hpp"

#include "../util/const.hpp"

using namespace std;

Pin::Pin()
    : net_id_(UNDEFINED_ID),
      name_(UNDEFINED_STRING),
      type_(PinType::NONE),
      position_(ORIGIN_POINT),
      orientation_(Orientation::N),
      ports_() {
}

Pin::Pin(NetId net_id, const string& name, PinType type, const Point& position,
         Orientation orientation)
    : net_id_(net_id),
      name_(name),
      type_(type),
      position_(position),
      orientation_(orientation),
      ports_() {
}

void Pin::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Pin:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "net_id_: " << net_id_
     << endl;
  os << string(indent_level * indent_size, ' ') << "name_: " << name_ << endl;
  os << string(indent_level * indent_size, ' ') << "type_: " << type_ << endl;
  os << string(indent_level * indent_size, ' ') << "position_: " << position_
     << endl;
  os << string(indent_level * indent_size, ' ')
     << "orientation_: " << orientation_ << endl;
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

NetId Pin::net_id() const {
  return net_id_;
}

const string& Pin::name() const {
  return name_;
}

PinType Pin::type() const {
  return type_;
}

const Point& Pin::position() const {
  return position_;
}

Orientation Pin::orientation() const {
  return orientation_;
}

int Pin::num_ports() const {
  return ports_.size();
}

const Port& Pin::port(int idx) const {
  return ports_.at(idx);
}

bool Pin::is_input() const {
  return type_ == PinType::INPUT;
}

bool Pin::is_output() const {
  return type_ == PinType::OUTPUT;
}

// Setters

Port& Pin::port(int idx) {
  return ports_.at(idx);
}

void Pin::add_port(const Port& port) {
  ports_.push_back(port);
}
