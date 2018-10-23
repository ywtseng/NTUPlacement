#include "cell.hpp"

#include "../util/const.hpp"

using namespace std;

Cell::Cell()
    : name_(UNDEFINED_STRING),
      type_(UNDEFINED_STRING),
      origin_(ORIGIN_POINT),
      width_(0.0),
      height_(0.0),
      left_edge_type_(0),
      right_edge_type_(0),
      pin_ids_(),
      ground_pin_id_(UNDEFINED_ID),
      power_pin_id_(UNDEFINED_ID),
      is_bottom_ground_(true) {
}

Cell::Cell(const string& name)
    : name_(name),
      type_(UNDEFINED_STRING),
      origin_(ORIGIN_POINT),
      width_(0.0),
      height_(0.0),
      left_edge_type_(0),
      right_edge_type_(0),
      pin_ids_(),
      ground_pin_id_(UNDEFINED_ID),
      power_pin_id_(UNDEFINED_ID),
      is_bottom_ground_(true) {
}

void Cell::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Cell:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "name_: " << name_ << endl;
  os << string(indent_level * indent_size, ' ') << "type_: " << type_ << endl;
  os << string(indent_level * indent_size, ' ') << "origin_: " << origin_
     << endl;
  os << string(indent_level * indent_size, ' ') << "width_: " << width_ << endl;
  os << string(indent_level * indent_size, ' ') << "height_: " << height_
     << endl;
  os << string(indent_level * indent_size, ' ')
     << "left_edge_type_: " << left_edge_type_ << endl;
  os << string(indent_level * indent_size, ' ')
     << "right_edge_type_: " << right_edge_type_ << endl;
  os << string(indent_level * indent_size, ' ')
     << "ground_pin_id_: " << ground_pin_id_ << endl;
  os << string(indent_level * indent_size, ' ')
     << "power_pin_id_: " << power_pin_id_ << endl;
  os << string(indent_level * indent_size, ' ')
     << "is_bottom_ground_: " << is_bottom_ground_ << endl;
  os << string(indent_level * indent_size, ' ') << "pin_ids_:" << endl;
  ++indent_level;
  for (int i = 0; i < pin_ids_.size(); ++i) {
    os << string(indent_level * indent_size, ' ') << "[" << i << "]"
       << ": " << pin_ids_[i] << endl;
  }
  --indent_level;
}

// Getters

const string& Cell::name() const {
  return name_;
}

const string& Cell::type() const {
  return type_;
}

const Point& Cell::origin() const {
  return origin_;
}

double Cell::width() const {
  return width_;
}

double Cell::height() const {
  return height_;
}

EdgeType Cell::left_edge_type() const {
  return left_edge_type_;
}

EdgeType Cell::right_edge_type() const {
  return right_edge_type_;
}

CellPinId Cell::ground_pin_id() const {
  return ground_pin_id_;
}

CellPinId Cell::power_pin_id() const {
  return power_pin_id_;
}

bool Cell::is_bottom_ground() const {
  return is_bottom_ground_;
}

int Cell::num_pins() const {
  return pin_ids_.size();
}

CellPinId Cell::pin_id(int idx) const {
  return pin_ids_.at(idx);
}

// Setters

void Cell::set_type(const string& type) {
  type_ = type;
}

void Cell::set_origin(const Point& origin) {
  origin_ = origin;
}

void Cell::set_size(double width, double height) {
  width_ = width;
  height_ = height;
}

void Cell::set_edge_types(EdgeType left_edge_type, EdgeType right_edge_type) {
  left_edge_type_ = left_edge_type;
  right_edge_type_ = right_edge_type;
}

void Cell::set_ground_pin_id(CellPinId id) {
  ground_pin_id_ = id;
}

void Cell::set_power_pin_id(CellPinId id) {
  power_pin_id_ = id;
}

void Cell::set_is_bottom_ground(bool is_bottom_ground) {
  is_bottom_ground_ = is_bottom_ground;
}

void Cell::add_pin_id(CellPinId id) {
  pin_ids_.push_back(id);
}
