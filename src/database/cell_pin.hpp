#ifndef CELL_PIN_HPP
#define CELL_PIN_HPP

#include "../util/type.hpp"
#include "cell_pin_port.hpp"

#include <iostream>
#include <string>
#include <vector>

class CellPin {
 public:
  CellPin();
  CellPin(CellId cell_id, const std::string& name, NetType type);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  CellId cell_id() const;
  const std::string& name() const;
  NetType type() const;

  int num_ports() const;

  const CellPinPort& port(int idx) const;

  bool is_power() const;
  bool is_ground() const;

  // Setters

  CellPinPort& port(int idx);

  void add_port(const CellPinPort& port);

 private:
  CellId cell_id_;
  std::string name_;
  NetType type_;
  std::vector<CellPinPort> ports_;
};

#endif
