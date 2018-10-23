#ifndef PORT_HPP
#define PORT_HPP

#include "rect.hpp"

#include <iostream>
#include <string>

class Port {
 public:
  Port();
  Port(int layer_id, const Rect& rect);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  int layer_id() const;
  const Rect& rect() const;

 private:
  int layer_id_;
  Rect rect_;
};

#endif
