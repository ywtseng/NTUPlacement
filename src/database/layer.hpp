#ifndef LAYER_HPP
#define LAYER_HPP

#include "../util/type.hpp"

#include <iostream>
#include <string>
#include <vector>

class Layer {
 public:
  Layer();
  Layer(const std::string& name);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  const std::string& name() const;

  int num_ground_rails() const;
  int num_power_rails() const;

  RailId ground_rail_id(int idx) const;
  RailId power_rail_id(int idx) const;

  // Setters

  void add_ground_rail_id(RailId id);
  void add_power_rail_id(RailId id);

 private:
  std::string name_;
  std::vector<RailId> ground_rail_ids_;
  std::vector<RailId> power_rail_ids_;
};

#endif
