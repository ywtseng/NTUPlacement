#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "../util/type.hpp"
#include "blockage.hpp"
#include "cell.hpp"
#include "cell_pin.hpp"
#include "fence_region.hpp"
#include "group.hpp"
#include "instance.hpp"
#include "instance_pin.hpp"
#include "interval.hpp"
#include "window.hpp"
#include "layer.hpp"
#include "net.hpp"
#include "pin.hpp"
#include "rail.hpp"
#include "row.hpp"
#include "site.hpp"
#include "sub_instance.hpp"
#include "variable.hpp"

#include <array>
#include <iostream>
#include <unordered_map>
#include <vector>

class Database {
 public:
  static const int num_edge_types_ = 3;

  Database();

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;
  void Summary(std::ostream& os = std::cout) const;
  void Plot(std::ostream& os = std::cout,
            const std::string& title = "Placement") const;

  // Getters

  int lef_unit() const;
  double edge_type_spacing(EdgeType edge_type_a, EdgeType edge_type_b) const;
  const std::string& design_name() const;
  int def_unit() const;
  const Rect& die_rect() const;
  double density_target() const;
  double displacement_limit() const;
  int max_instance_row_height() const;

  int num_layers() const;
  int num_cells() const;
  int num_cell_pins() const;
  int num_rows() const;
  int num_sites() const;
  int num_fence_regions() const;
  int num_rails() const;
  int num_instances() const;
  int num_instance_pins() const;
  int num_pins() const;
  int num_blockages() const;
  int num_nets() const;
  int num_groups() const;
  int num_intervals() const;
  int num_sub_instances() const;
  int num_windows() const;
  int num_variables() const;

  int num_instances_of_row_height(int row_height) const;
  int num_fixed_instances() const;

  const Layer& layer(LayerId id) const;
  const Cell& cell(CellId id) const;
  const CellPin& cell_pin(CellPinId id) const;
  const Row& row(RowId id) const;
  const Site& site(SiteId id) const;
  const FenceRegion& fence_region(FenceRegionId id) const;
  const Rail& rail(RailId id) const;
  const Instance& instance(InstanceId id) const;
  const InstancePin& instance_pin(InstancePinId id) const;
  const Pin& pin(PinId id) const;
  const Blockage& blockage(BlockageId id) const;
  const Net& net(NetId id) const;
  const Group& group(GroupId id) const;
  const Interval& interval(IntervalId id) const;
  const SubInstance& sub_instance(SubInstanceId id) const;
  const Window& window(WindowId id) const;
  const Variable& variable(VariableId id) const;

  LayerId layer_id_by_name(const std::string& name) const;
  CellId cell_id_by_name(const std::string& name) const;
  CellPinId cell_pin_id_by_name(const std::string& name) const;
  RowId row_id_by_name(const std::string& name) const;
  SiteId site_id_by_position(const Point& position) const;
  FenceRegionId fence_region_id_by_name(const std::string& name) const;
  InstanceId instance_id_by_name(const std::string& name) const;
  InstancePinId instance_pin_id_by_name(const std::string& name) const;
  PinId pin_id_by_name(const std::string& name) const;
  NetId net_id_by_name(const std::string& name) const;
  GroupId group_id_by_name(const std::string& name) const;
  GroupId group_id_by_pattern(const std::string& pattern) const;

  InstanceId instance_id_by_row_height(int row_height, int idx) const;
  InstanceId fixed_instance_id(int idx) const;

  double ComputeTotalDisplacement() const;
  double ComputeTotalHpwl() const;  // TODO:
  double ComputeTotalInstanceArea() const;

  // Setters

  // Pre-allocate memory to speed up.
  void reserve_instances(int num_instances);
  void reserve_instance_pins(int num_instance_pins);
  void reserve_sub_instances(int num_sub_instances);
  void reserve_rows(int num_rows);
  void reserve_sites(int num_sites);
  void reserve_pins(int num_pins);
  void reserve_nets(int num_nets);

  // Must be called in advance.
  void initialize_site_id_by_position(int num_y_sites, int num_x_sites);

  void set_lef_unit(int lef_unit);
  void set_edge_type_spacing(EdgeType edge_type_a, EdgeType edge_type_b,
                             double spacing);
  void set_design_name(const std::string& design_name);
  void set_def_unit(int def_unit);
  void set_die_rect(const Rect& rect);
  void set_density_target(double density_target);
  void set_displacement_limit(double displacement_limit);

  Layer& layer(LayerId id);
  Cell& cell(CellId id);
  CellPin& cell_pin(CellPinId id);
  Row& row(RowId id);
  Site& site(SiteId id);
  FenceRegion& fence_region(FenceRegionId id);
  Rail& rail(RailId id);
  Instance& instance(InstanceId id);
  InstancePin& instance_pin(InstancePinId id);
  Pin& pin(PinId id);
  Blockage& blockage(BlockageId id);
  Net& net(NetId id);
  Group& group(GroupId id);
  Interval& interval(IntervalId id);
  SubInstance& sub_instance(SubInstanceId id);
  Window& window(WindowId id);
  Variable& variable(VariableId id);

  LayerId add_layer(const Layer& layer);
  CellId add_cell(const Cell& cell);
  CellPinId add_cell_pin(const CellPin& cell_pin);
  RowId add_row(const Row& row);
  SiteId add_site(const Site& site);
  FenceRegionId add_fence_region(const FenceRegion& fence_region);
  RailId add_rail(const Rail& rail);
  InstanceId add_instance(const Instance& instance);
  InstancePinId add_instance_pin(const InstancePin& instance_pin);
  PinId add_pin(const Pin& pin);
  BlockageId add_blockage(const Blockage& blockage);
  NetId add_net(const Net& net);
  GroupId add_group(const Group& group);
  IntervalId add_interval(const Interval& interval);
  SubInstanceId add_sub_instance(const SubInstance& sub_instance);
  WindowId add_window(const Window& window);
  VariableId add_variable(const Variable& variable);
  
  void add_instance_id_of_row_height(int row_height, InstanceId id);
  void add_fixed_instance_id(InstanceId id);

  void index_layer_by_name(LayerId id, const std::string& name);
  void index_cell_by_name(CellId id, const std::string& name);
  void index_cell_pin_by_name(CellPinId id, const std::string& name);
  void index_row_by_name(RowId id, const std::string& name);
  void index_site_by_position(SiteId id, const Point& position);
  void index_fence_region_by_name(FenceRegionId id, const std::string& name);
  void index_instance_by_name(InstanceId id, const std::string& name);
  void index_instance_pin_by_name(InstancePinId id, const std::string& name);
  void index_pin_by_name(PinId id, const std::string& name);
  void index_net_by_name(NetId id, const std::string& name);
  void index_group_by_name(GroupId id, const std::string& name);
  void index_group_by_pattern(GroupId id, const std::string& pattern);

  // Helpers.

  void ConvertLefMicronsToDefUnits();
  void SortRowsByY();  // Must be called to access rows by y.
  void AssignRailsToRows();
  void SplitRowsIntoIntervals();
  void AssignFenceRegionsToInstances();

  void UpdateInstanceSubInstancePositions(InstanceId instance_id);

 private:
  // Tech lef

  int lef_unit_;
  std::array<std::array<double, num_edge_types_>, num_edge_types_>
      edge_type_spacing_table_;

  // Cell lef

  std::vector<Layer> layers_;
  std::unordered_map<std::string, LayerId> layer_id_by_name_;

  std::vector<Cell> cells_;
  std::unordered_map<std::string, CellId> cell_id_by_name_;

  std::vector<CellPin> cell_pins_;
  std::unordered_map<std::string, CellPinId> cell_pin_id_by_name_;

  // Global placed def

  std::string design_name_;
  int def_unit_;
  Rect die_rect_;

  std::vector<Row> rows_;
  std::unordered_map<std::string, RowId> row_id_by_name_;

  std::vector<Site> sites_;
  std::vector<std::vector<SiteId>> site_id_by_position_;

  std::vector<FenceRegion> fence_regions_;
  std::unordered_map<std::string, FenceRegionId> fence_region_id_by_name_;

  std::vector<Rail> rails_;

  std::vector<Instance> instances_;
  std::unordered_map<std::string, InstanceId> instance_id_by_name_;
  std::vector<std::vector<InstanceId>> instance_ids_by_row_height_;
  std::vector<InstanceId> fixed_instance_ids_;

  std::vector<InstancePin> instance_pins_;
  std::unordered_map<std::string, InstancePinId> instance_pin_id_by_name_;

  std::vector<Pin> pins_;
  std::unordered_map<std::string, PinId> pin_id_by_name_;

  std::vector<Blockage> blockages_;

  std::vector<Net> nets_;
  std::unordered_map<std::string, NetId> net_id_by_name_;

  std::vector<Group> groups_;
  std::unordered_map<std::string, GroupId> group_id_by_name_;
  std::unordered_map<std::string, GroupId> group_id_by_pattern_;

  std::vector<Interval> intervals_;

  std::vector<SubInstance> sub_instances_;
  
  std::vector<Window> windows_;
  
  std::vector<Variable> variables_;

  // Placement constraints

  double density_target_;
  double displacement_limit_;
};

#endif
