#include "cell_pin_port.hpp"

#include "../util/const.hpp"

using namespace std;

CellPinPort::CellPinPort()
    : cell_pin_id_(UNDEFINED_ID),
      layer_id_(UNDEFINED_ID),
      rect_(ORIGIN_POINT, ORIGIN_POINT) {
}

CellPinPort::CellPinPort(CellPinId cell_pin_id, LayerId layer_id,
                         const Rect& rect)
    : cell_pin_id_(cell_pin_id), layer_id_(layer_id), rect_(rect) {
}

void CellPinPort::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "CellPinPort:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ')
     << "cell_pin_id_: " << cell_pin_id_ << endl;
  os << string(indent_level * indent_size, ' ') << "layer_id_: " << layer_id_
     << endl;
  os << string(indent_level * indent_size, ' ') << "rect_: " << rect_ << endl;
}

// Getters

CellPinId CellPinPort::cell_pin_id() const {
  return cell_pin_id_;
}

LayerId CellPinPort::layer_id() const {
  return layer_id_;
}

const Rect& CellPinPort::rect() const {
  return rect_;
}

// Setters

void CellPinPort::set_rect(const Rect& rect) {
  rect_ = rect;
}
