#ifndef INSTANCE_PIN_HPP
#define INSTANCE_PIN_HPP

#include "../util/type.hpp"
#include "instance_pin_port.hpp"

#include <string>
#include <vector>

class InstancePin {
 public:
  InstancePin();
  InstancePin(InstanceId instance_id, CellPinId cell_pin_id,
              const std::string& name);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  InstanceId instance_id() const;
  CellPinId cell_pin_id() const;
  const std::string& name() const;

  int num_ports() const;

  const InstancePinPort& port(int idx) const;

  // Setters

  InstancePinPort& port(int idx);

  void add_port(const InstancePinPort& port);

 private:
  InstanceId instance_id_;
  CellPinId cell_pin_id_;
  std::string name_;
  std::vector<InstancePinPort> ports_;
};

#endif
