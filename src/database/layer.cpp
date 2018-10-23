#include "layer.hpp"

#include "../util/const.hpp"

using namespace std;

Layer::Layer()
    : name_(UNDEFINED_STRING), ground_rail_ids_(), power_rail_ids_() {
}

Layer::Layer(const string& name)
    : name_(name), ground_rail_ids_(), power_rail_ids_() {
}

void Layer::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Layer:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "name_: " << name_ << endl;
  os << string(indent_level * indent_size, ' ') << "ground_rail_ids_:" << endl;
  ++indent_level;
  for (int i = 0; i < ground_rail_ids_.size(); ++i) {
    os << string(indent_level * indent_size, ' ') << "[" << i << "]"
       << ": " << ground_rail_ids_[i] << endl;
  }
  --indent_level;
  os << string(indent_level * indent_size, ' ') << "power_rail_ids_:" << endl;
  ++indent_level;
  for (int i = 0; i < power_rail_ids_.size(); ++i) {
    os << string(indent_level * indent_size, ' ') << "[" << i << "]"
       << ": " << power_rail_ids_[i] << endl;
  }
  --indent_level;
}

// Getters

const string& Layer::name() const {
  return name_;
}

int Layer::num_ground_rails() const {
  return ground_rail_ids_.size();
}

int Layer::num_power_rails() const {
  return power_rail_ids_.size();
}

RailId Layer::ground_rail_id(int idx) const {
  return ground_rail_ids_.at(idx);
}

RailId Layer::power_rail_id(int idx) const {
  return power_rail_ids_.at(idx);
}

// Setters

void Layer::add_ground_rail_id(RailId id) {
  ground_rail_ids_.push_back(id);
}

void Layer::add_power_rail_id(RailId id) {
  power_rail_ids_.push_back(id);
}
