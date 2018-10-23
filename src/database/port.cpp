#include "port.hpp"

#include "../util/const.hpp"

using namespace std;

Port::Port() : layer_id_(UNDEFINED_ID), rect_(ORIGIN_POINT, ORIGIN_POINT) {
}

Port::Port(int layer_id, const Rect& rect) : layer_id_(layer_id), rect_(rect) {
}

void Port::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Port:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "layer_id_: " << layer_id_
     << endl;
  os << string(indent_level * indent_size, ' ') << "rect_: " << rect_ << endl;
}

// Getters

int Port::layer_id() const {
  return layer_id_;
}

const Rect& Port::rect() const {
  return rect_;
}
