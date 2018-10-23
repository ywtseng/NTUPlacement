#include "instance.hpp"

#include "../util/const.hpp"

using namespace std;

Instance::Instance()
    : cell_id_(UNDEFINED_ID),
      name_(UNDEFINED_STRING),
      left_edge_type_(0),
      right_edge_type_(0),
      is_fixed_(false),
      global_placed_position_(ORIGIN_POINT),
      position_(global_placed_position_),
      last_position_(position_),
      width_(0.0),
      height_(0.0),
      orientation_(Orientation::N),
      fence_region_id_(UNDEFINED_ID),
      pin_ids_(),
      ground_pin_id_(UNDEFINED_ID),
      power_pin_id_(UNDEFINED_ID),
      is_bottom_ground_(true),
      sub_instance_ids_() {
}

Instance::Instance(CellId cell_id, const string& name, bool is_fixed,
                   const Point& global_placed_position, double width,
                   double height, Orientation orientation)
    : cell_id_(cell_id),
      name_(name),
      left_edge_type_(0),
      right_edge_type_(0),
      is_fixed_(is_fixed),
      global_placed_position_(global_placed_position),
      position_(global_placed_position_),
      last_position_(position_),
      width_(width),
      height_(height),
      orientation_(orientation),
      fence_region_id_(UNDEFINED_ID),
      pin_ids_(),
      ground_pin_id_(UNDEFINED_ID),
      power_pin_id_(UNDEFINED_ID),
      is_bottom_ground_(true),
      sub_instance_ids_() {
}

void Instance::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Instance:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "cell_id_: " << cell_id_
     << endl;
  os << string(indent_level * indent_size, ' ') << "name_: " << name_ << endl;
  os << string(indent_level * indent_size, ' ')
     << "left_edge_type_: " << left_edge_type_ << endl;
  os << string(indent_level * indent_size, ' ')
     << "right_edge_type_: " << right_edge_type_ << endl;
  os << string(indent_level * indent_size, ' ') << "is_fixed_: " << is_fixed_
     << endl;
  os << string(indent_level * indent_size, ' ') << "width_: " << width_ << endl;
  os << string(indent_level * indent_size, ' ') << "height_: " << height_
     << endl;
  os << string(indent_level * indent_size, ' ') << "position_: " << position_
     << endl;
  os << string(indent_level * indent_size, ' ')
     << "orientation_: " << orientation_ << endl;
  if (fence_region_id_ != UNDEFINED_ID) {
    os << string(indent_level * indent_size, ' ')
       << "fence_region_id_: " << fence_region_id_ << endl;
  } else {
    os << string(indent_level * indent_size, ' ') << "fence_region_id_: "
       << "N/A" << endl;
  }
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

  os << string(indent_level * indent_size, ' ')
     << "sub_instance_ids_sorted_by_y_:" << endl;
  ++indent_level;
  for (int i = 0; i < sub_instance_ids_.size(); ++i) {
    os << string(indent_level * indent_size, ' ') << "[" << i << "]"
       << ": " << sub_instance_ids_[i] << endl;
  }
  --indent_level;
}

// Getters

CellId Instance::cell_id() const {
  return cell_id_;
}

const string& Instance::name() const {
  return name_;
}

EdgeType Instance::left_edge_type() const {
  return left_edge_type_;
}

EdgeType Instance::right_edge_type() const {
  return right_edge_type_;
}

bool Instance::is_fixed() const {
  return is_fixed_;
}

const Point& Instance::global_placed_position() const {
  return global_placed_position_;
}

const Point& Instance::position() const {
  return position_;
}

const Point& Instance::last_position() const {
  return last_position_;
}

const Point& Instance::detail_initial_position() const {
  return detail_initial_position_;
}

const Point& Instance::detail_last_position() const {
  return detail_last_position_;
}

double Instance::width() const {
  return width_;
}

double Instance::height() const {
  return height_;
}

Orientation Instance::orientation() const {
  return orientation_;
}

bool Instance::has_fence_region() const {
  return fence_region_id_ != UNDEFINED_ID;
}

FenceRegionId Instance::fence_region_id() const {
  return fence_region_id_;
}

bool Instance::is_bottom_ground() const {
  return is_bottom_ground_;
}

InstancePinId Instance::ground_pin_id() const {
  return ground_pin_id_;
}

InstancePinId Instance::power_pin_id() const {
  return power_pin_id_;
}

SubInstanceId Instance::bottom_sub_instance_id() const {
  return sub_instance_ids_.front();
}

SubInstanceId Instance::top_sub_instance_id() const {
  return sub_instance_ids_.back();
}

int Instance::num_pins() const {
  return pin_ids_.size();
}

int Instance::num_sub_instances() const {
  return sub_instance_ids_.size();
}

int Instance::num_variables() const {
  return variable_ids_.size();
}

InstancePinId Instance::pin_id(int idx) const {
  return pin_ids_.at(idx);
}

SubInstanceId Instance::sub_instance_id(int idx) const {
  return sub_instance_ids_.at(idx);
}

VariableId Instance::variable_id(int idx) const {
  return variable_ids_.at(idx);
}

// Setters

void Instance::set_edge_types(EdgeType left_edge_type,
                              EdgeType right_edge_type) {
  left_edge_type_ = left_edge_type;
  right_edge_type_ = right_edge_type;
}

void Instance::set_position(const Point& position) {
  last_position_ = position_;
  position_ = position;
}

void Instance::set_detail_initial_position(const Point& position) {
  detail_initial_position_ = position;
}

void Instance::set_detail_last_position(const Point& position) {
  detail_last_position_ = position;
}


void Instance::set_orientation(Orientation orientation) {
  orientation_ = orientation;
}

void Instance::set_fence_region_id(FenceRegionId fence_region_id) {
  fence_region_id_ = fence_region_id;
}

void Instance::set_ground_pin_id(InstancePinId id) {
  ground_pin_id_ = id;
}

void Instance::set_power_pin_id(InstancePinId id) {
  power_pin_id_ = id;
}

void Instance::set_is_bottom_ground(bool is_bottom_ground) {
  is_bottom_ground_ = is_bottom_ground;
}

void Instance::add_pin_id(InstancePinId id) {
  pin_ids_.push_back(id);
}

void Instance::add_sub_instance_id(SubInstanceId id) {
  sub_instance_ids_.push_back(id);
}

void Instance::add_variable_id(VariableId id) {
  variable_ids_.push_back(id);
}

void Instance::FlipVertically() {
  switch (orientation_) {
    case Orientation::N: {
      orientation_ = Orientation::FS;
      break;
    }
    case Orientation::S: {
      orientation_ = Orientation::FN;
      break;
    }
    case Orientation::FN: {
      orientation_ = Orientation::S;
      break;
    }
    case Orientation::FS: {
      orientation_ = Orientation::N;
      break;
    }
    default: { break; }
  }

  is_bottom_ground_ = !is_bottom_ground_;
}

void Instance::FlipHorizontally() {
  switch (orientation_) {
    case Orientation::N: {
      orientation_ = Orientation::FN;
      break;
    }
    case Orientation::S: {
      orientation_ = Orientation::FS;
      break;
    }
    case Orientation::FN: {
      orientation_ = Orientation::N;
      break;
    }
    case Orientation::FS: {
      orientation_ = Orientation::S;
      break;
    }
    default: { break; }
  }

  swap(left_edge_type_, right_edge_type_);
}
