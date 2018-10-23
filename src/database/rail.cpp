#include "rail.hpp"

#include "../util/const.hpp"

using namespace std;

Rail::Rail()
    : type_(NetType::GROUND),
      layer_id_(UNDEFINED_ID),
      middle_line_(ORIGIN_POINT, ORIGIN_POINT),
      width_(0.0) {
}

Rail::Rail(NetType type, LayerId layer_id, const Line& middle_line,
           double width)
    : type_(type),
      layer_id_(layer_id),
      middle_line_(middle_line),
      width_(width) {
}

void Rail::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Rail:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "layer_id_: " << layer_id_
     << endl;
  os << string(indent_level * indent_size, ' ') << "type_: " << type_ << endl;
  os << string(indent_level * indent_size, ' ')
     << "middle_line_: " << middle_line_ << endl;
  os << string(indent_level * indent_size, ' ') << "width_: " << width_ << endl;
}

// Getters

NetType Rail::type() const {
  return type_;
}

LayerId Rail::layer_id() const {
  return layer_id_;
}

const Line& Rail::middle_line() const {
  return middle_line_;
}

double Rail::width() const {
  return width_;
}
