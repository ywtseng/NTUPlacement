#ifndef CELL_HPP
#define CELL_HPP

#include "../util/type.hpp"
#include "point.hpp"

#include <iostream>
#include <string>
#include <vector>

class Cell {
 public:
  Cell();
  Cell(const std::string& name);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  const std::string& name() const;
  const std::string& type() const;
  const Point& origin() const;
  double width() const;
  double height() const;
  EdgeType left_edge_type() const;
  EdgeType right_edge_type() const;
  CellPinId ground_pin_id() const;
  CellPinId power_pin_id() const;
  bool is_bottom_ground() const;

  int num_pins() const;

  CellPinId pin_id(int idx) const;

  // Setters

  void set_type(const std::string& type);
  void set_origin(const Point& origin);
  void set_size(double width, double height);
  void set_edge_types(EdgeType left_edge_type, EdgeType right_edge_type);
  void set_ground_pin_id(CellPinId id);
  void set_power_pin_id(CellPinId id);
  void set_is_bottom_ground(bool is_bottom_ground);

  void add_pin_id(CellPinId id);

 private:
  std::string name_;
  std::string type_;  // CLASS
  Point origin_;
  double width_;
  double height_;
  EdgeType left_edge_type_;
  EdgeType right_edge_type_;
  std::vector<CellPinId> pin_ids_;
  CellPinId ground_pin_id_;
  CellPinId power_pin_id_;
  bool is_bottom_ground_;
};

#endif
