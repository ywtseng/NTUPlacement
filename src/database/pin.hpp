#ifndef PIN_HPP
#define PIN_HPP

#include "../util/type.hpp"
#include "port.hpp"

#include <iostream>
#include <string>
#include <vector>

class Pin {
 public:
  Pin();
  Pin(NetId net_id, const std::string& name, PinType type,
      const Point& position, Orientation orientation);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  NetId net_id() const;
  const std::string& name() const;
  PinType type() const;
  const Point& position() const;
  Orientation orientation() const;

  int num_ports() const;

  const Port& port(int idx) const;

  bool is_input() const;
  bool is_output() const;

  // Setters

  Port& port(int idx);

  void add_port(const Port& port);

 private:
  NetId net_id_;
  std::string name_;
  PinType type_;
  Point position_;
  Orientation orientation_;
  std::vector<Port> ports_;
};

#endif
