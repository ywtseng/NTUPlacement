#include "cell_pin.hpp"

#include "../util/const.hpp"

using namespace std;

CellPin::CellPin()
    : cell_id_(UNDEFINED_ID),
      name_(UNDEFINED_STRING),
      type_(NetType::NONE),
      ports_() {
}

CellPin::CellPin(CellId cell_id, const string& name, NetType type)
    : cell_id_(cell_id), name_(name), type_(type), ports_() {
}

void CellPin::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "CellPin:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "cell_id_: " << cell_id_
     << endl;
  os << string(indent_level * indent_size, ' ') << "name_: " << name_ << endl;
  os << string(indent_level * indent_size, ' ') << "type_: " << type_ << endl;
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

CellId CellPin::cell_id() const {
  return cell_id_;
}

const string& CellPin::name() const {
  return name_;
}

NetType CellPin::type() const {
  return type_;
}

int CellPin::num_ports() const {
  return ports_.size();
}

const CellPinPort& CellPin::port(int idx) const {
  return ports_.at(idx);
}

bool CellPin::is_power() const {
  return type_ == NetType::POWER;
}

bool CellPin::is_ground() const {
  return type_ == NetType::GROUND;
}

// Setters

CellPinPort& CellPin::port(int idx) {
  return ports_.at(idx);
}

void CellPin::add_port(const CellPinPort& port) {
  ports_.push_back(port);
}
