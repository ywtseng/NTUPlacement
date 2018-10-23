#ifndef INSTANCE_PIN_PORT_HPP
#define INSTANCE_PIN_PORT_HPP

#include "../util/type.hpp"
#include "rect.hpp"

class InstancePinPort {
 public:
  InstancePinPort();
  InstancePinPort(InstancePinId instance_pin_id, const Rect& rect);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  InstancePinId instance_pin_id() const;
  const Rect& rect() const;

  // Setters

  void set_rect(const Rect& rect);

 private:
  InstancePinId instance_pin_id_;
  Rect rect_;
};

#endif
