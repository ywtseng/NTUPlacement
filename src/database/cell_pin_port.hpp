#ifndef CELL_PIN_PORT_HPP
#define CELL_PIN_PORT_HPP

#include "../util/type.hpp"
#include "rect.hpp"

class CellPinPort {
 public:
  CellPinPort();
  CellPinPort(CellPinId cell_pin_id, LayerId layer_id, const Rect& rect);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  CellPinId cell_pin_id() const;
  LayerId layer_id() const;
  const Rect& rect() const;

  // Setters

  void set_rect(const Rect& rect);

 private:
  CellPinId cell_pin_id_;
  LayerId layer_id_;
  Rect rect_;
};

#endif
