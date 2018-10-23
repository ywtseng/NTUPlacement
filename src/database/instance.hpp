#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include "../util/type.hpp"
#include "rect.hpp"

#include <vector>

class Instance {
 public:
  Instance();
  Instance(CellId cell_id, const std::string& name, bool is_fixed,
           const Point& global_placed_position, double width, double height,
           Orientation orientation);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  CellId cell_id() const;
  const std::string& name() const;
  EdgeType left_edge_type() const;
  EdgeType right_edge_type() const;
  bool is_fixed() const;
  const Point& global_placed_position() const;
  const Point& position() const;
  const Point& last_position() const;
  //DDA constraint add
  const Point& detail_initial_position() const;
  const Point& detail_last_position() const;
  
  double width() const;
  double height() const;
  Orientation orientation() const;
  bool has_fence_region() const;
  FenceRegionId fence_region_id() const;
  bool is_bottom_ground() const;
  InstancePinId ground_pin_id() const;
  InstancePinId power_pin_id() const;
  SubInstanceId bottom_sub_instance_id() const;
  SubInstanceId top_sub_instance_id() const;

  int num_pins() const;
  int num_sub_instances() const;
  int num_variables() const;
  InstancePinId pin_id(int idx) const;
  SubInstanceId sub_instance_id(int idx) const;
  VariableId variable_id(int idx) const;

  // Setters

  void set_edge_types(EdgeType left_edge_type, EdgeType right_edge_type);
  void set_position(const Point& position);
  //DDA constraint add
  void set_detail_initial_position(const Point& position);
  void set_detail_last_position(const Point& position);
  
  
  void set_orientation(Orientation orientation);
  void set_fence_region_id(FenceRegionId fence_region_id);
  void set_ground_pin_id(InstancePinId id);
  void set_power_pin_id(InstancePinId id);
  void set_is_bottom_ground(bool is_bottom_ground);

  void add_pin_id(InstancePinId id);
  // Must be added from bottom to top.
  void add_sub_instance_id(SubInstanceId id);
  void add_variable_id(VariableId id);

  void FlipVertically();
  void FlipHorizontally();
  
  bool is_set_variable = false;

  // TODO: Update pin ports.

  // Test
  bool outoffence = false;
  bool forbidden_cell = false;
 private:
  CellId cell_id_;
  std::string name_;
  EdgeType left_edge_type_;
  EdgeType right_edge_type_;
  bool is_fixed_;
  Point global_placed_position_;
  Point position_;
  Point last_position_;
  //DDA constraint add
  Point detail_initial_position_;
  Point detail_last_position_;
  std::vector<VariableId>  variable_ids_;
  
  double width_;
  double height_;
  Orientation orientation_;
  FenceRegionId fence_region_id_;
  std::vector<InstancePinId> pin_ids_;
  InstancePinId ground_pin_id_;
  InstancePinId power_pin_id_;
  bool is_bottom_ground_;
  std::vector<SubInstanceId> sub_instance_ids_;
};

#endif
