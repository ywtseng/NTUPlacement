#ifndef SITE_HPP
#define SITE_HPP

#include "../util/type.hpp"
#include "point.hpp"

#include <iostream>
#include <string>
#include <vector>

class Site {
 public:
  static double width();
  static double height();

  static void set_size(double width, double height);

  Site();
  Site(const Point& position);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters
  int num_variables() const;
  const Point& position() const;
  bool is_valid() const;
  FenceRegionId fence_region_id() const;

  bool has_sub_instance() const;

  SubInstanceId sub_instance_id() const;
  SiteId site_id() const;
  VariableId variable_id(int idx) const;
  int variable_layer(int idx) const;
  // Setters

  void set_is_valid(bool is_valid);
  void set_fence_region_id(FenceRegionId fence_region_id);
  void set_sub_instance_id(SubInstanceId sub_instance_id);
  void add_variable_id(VariableId id);
  void add_variable_layer(int id);
  
  void remove_sub_instance_id();
  void set_site_id(SiteId id);

 private:
  static double width_;
  static double height_;

  Point position_;
  bool is_valid_;
  FenceRegionId fence_region_id_;
  SubInstanceId sub_instance_id_;
  SiteId site_id_;
  std::vector<VariableId>  variable_ids_;
  std::vector<int>         variable_layers_;
};

#endif
