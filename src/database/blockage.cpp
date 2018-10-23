#include "blockage.hpp"

#include "../util/const.hpp"

#include <string>

using namespace std;

Blockage::Blockage() : layer_id_(UNDEFINED_ID), rects_() {
}

Blockage::Blockage(LayerId layer_id) : layer_id_(layer_id), rects_() {
}

void Blockage::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;
  os << string(indent_level * indent_size, ' ') << "Blockage:" << endl;
  ++indent_level;
  os << string(indent_level * indent_size, ' ') << "layer_id_: " << layer_id_
     << endl;
  os << string(indent_level * indent_size, ' ') << "rects_:" << endl;
  ++indent_level;
  for (int i = 0; i < rects_.size(); ++i) {
    os << string(indent_level * indent_size, ' ') << "[" << i << "]"
       << ": " << rects_[i] << endl;
  }
}

// Getters

LayerId Blockage::layer_id() const {
  return layer_id_;
}

int Blockage::num_rects() const {
  return rects_.size();
}

const Rect& Blockage::rect(int idx) const {
  return rects_.at(idx);
}

// Setters

Rect& Blockage::rect(int idx) {
  return rects_.at(idx);
}

void Blockage::add_rect(const Rect& rect) {
  rects_.push_back(rect);
}
