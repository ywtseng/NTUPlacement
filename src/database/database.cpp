#include "database.hpp"

#include "../util/const.hpp"
#include "../util/plotter.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <map>

using namespace std;

Database::Database()
    : lef_unit_(100),
      edge_type_spacing_table_({}),
      layers_(),
      layer_id_by_name_(),
      cells_(),
      cell_id_by_name_(),
      cell_pins_(),
      cell_pin_id_by_name_(),
      design_name_(UNDEFINED_STRING),
      def_unit_(100),
      die_rect_(),
      rows_(),
      row_id_by_name_(),
      sites_(),
      site_id_by_position_(),
      fence_regions_(),
      fence_region_id_by_name_(),
      rails_(),
      instances_(),
      instance_id_by_name_(),
      instance_ids_by_row_height_(),
      fixed_instance_ids_(),
      instance_pins_(),
      instance_pin_id_by_name_(),
      pins_(),
      pin_id_by_name_(),
      blockages_(),
      nets_(),
      net_id_by_name_(),
      groups_(),
      group_id_by_name_(),
      group_id_by_pattern_(),
      intervals_(),
      sub_instances_(),
      density_target_(1.0),
      displacement_limit_(numeric_limits<double>::max()) {
}

void Database::Print(ostream& os, int indent_level) const {
  const int indent_size = 2;

  os << string(indent_level * indent_size, ' ') << "Database:" << endl;

  ++indent_level;

  os << string(indent_level * indent_size, ' ') << "lef_unit_: " << lef_unit_
     << endl;

  os << string(indent_level * indent_size, ' ')
     << "edge_type_spacing_table_:" << endl;
  ++indent_level;
  for (int i = 0; i < edge_type_spacing_table_.size(); ++i) {
    for (int j = 0; j < edge_type_spacing_table_[i].size(); ++j) {
      const EdgeType edge_type_a(i);
      const EdgeType edge_type_b(j);
      os << string(indent_level * indent_size, ' ') << "[" << edge_type_a << "]"
         << " "
         << "[" << edge_type_b << "]"
         << ": " << edge_type_spacing_table_[i][j] << endl;
    }
  }
  --indent_level;

  os << string(indent_level * indent_size, ' ') << "layers_:" << endl;
  ++indent_level;
  for (int i = 0; i < layers_.size(); ++i) {
    const LayerId layer_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << layer_id << "]"
       << ":" << endl;
    ++indent_level;
    layers_[i].Print(os, indent_level);
    --indent_level;
  }
  --indent_level;

  os << string(indent_level * indent_size, ' ') << "cells_:" << endl;
  ++indent_level;
  for (int i = 0; i < cells_.size(); ++i) {
    const CellId cell_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << cell_id << "]"
       << ":" << endl;
    ++indent_level;
    cells_[i].Print(os, indent_level);
    --indent_level;
  }
  --indent_level;

  os << string(indent_level * indent_size, ' ') << "cell_pins_:" << endl;
  ++indent_level;
  for (int i = 0; i < cell_pins_.size(); ++i) {
    const CellPinId cell_pin_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << cell_pin_id << "]"
       << ":" << endl;
    ++indent_level;
    cell_pins_[i].Print(os, indent_level);
    --indent_level;
  }
  --indent_level;

  os << string(indent_level * indent_size, ' ')
     << "design_name_: " << design_name_ << endl;
  os << string(indent_level * indent_size, ' ') << "def_unit_: " << def_unit_
     << endl;
  os << string(indent_level * indent_size, ' ') << "die_rect_: " << die_rect_
     << endl;

  os << string(indent_level * indent_size, ' ') << "rows_:" << endl;
  ++indent_level;
  for (int i = 0; i < rows_.size(); ++i) {
    const RowId row_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << row_id << "]"
       << ":" << endl;
    ++indent_level;
    rows_[i].Print(os, indent_level);
    --indent_level;
  }
  --indent_level;

  os << string(indent_level * indent_size, ' ') << "sites_:" << endl;
  ++indent_level;
  for (int i = 0; i < sites_.size(); ++i) {
    const SiteId site_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << site_id << "]"
       << ":" << endl;
    ++indent_level;
    sites_[i].Print(os, indent_level);
    --indent_level;
  }
  --indent_level;

  os << string(indent_level * indent_size, ' ') << "fence_regions_:" << endl;
  ++indent_level;
  for (int i = 0; i < fence_regions_.size(); ++i) {
    const FenceRegionId fence_region_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << fence_region_id
       << "]"
       << ":" << endl;
    ++indent_level;
    fence_regions_[i].Print(os, indent_level);
    --indent_level;
  }
  --indent_level;

  os << string(indent_level * indent_size, ' ') << "rails_:" << endl;
  ++indent_level;
  for (int i = 0; i < rails_.size(); ++i) {
    const RailId rail_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << rail_id << "]"
       << ":" << endl;
    ++indent_level;
    rails_[i].Print(os, indent_level);
    --indent_level;
  }
  --indent_level;

  os << string(indent_level * indent_size, ' ') << "instances_:" << endl;
  ++indent_level;
  for (int i = 0; i < instances_.size(); ++i) {
    const InstanceId instance_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << instance_id << "]"
       << ":" << endl;
    ++indent_level;
    instances_[i].Print(os, indent_level);
    --indent_level;
  }
  --indent_level;

  os << string(indent_level * indent_size, ' ') << "instance_pins_:" << endl;
  ++indent_level;
  for (int i = 0; i < instance_pins_.size(); ++i) {
    const InstancePinId instance_pin_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << instance_pin_id
       << "]"
       << ":" << endl;
    ++indent_level;
    instance_pins_[i].Print(os, indent_level);
    --indent_level;
  }
  --indent_level;

  os << string(indent_level * indent_size, ' ') << "pins_:" << endl;
  ++indent_level;
  for (int i = 0; i < pins_.size(); ++i) {
    const PinId pin_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << pin_id << "]"
       << ":" << endl;
    ++indent_level;
    pins_[i].Print(os, indent_level);
    --indent_level;
  }
  --indent_level;

  os << string(indent_level * indent_size, ' ') << "blockages_:" << endl;
  ++indent_level;
  for (int i = 0; i < blockages_.size(); ++i) {
    const BlockageId blockage_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << blockage_id << "]"
       << ":" << endl;
    ++indent_level;
    blockages_[i].Print(os, indent_level);
    --indent_level;
  }
  --indent_level;

  os << string(indent_level * indent_size, ' ') << "nets_:" << endl;
  ++indent_level;
  for (int i = 0; i < nets_.size(); ++i) {
    const NetId net_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << net_id << "]"
       << ":" << endl;
    ++indent_level;
    nets_[i].Print(os, indent_level);
    --indent_level;
  }
  --indent_level;

  os << string(indent_level * indent_size, ' ') << "groups_:" << endl;
  ++indent_level;
  for (int i = 0; i < groups_.size(); ++i) {
    const GroupId group_id(i);
    os << string(indent_level * indent_size, ' ') << "[" << group_id << "]"
       << ":" << endl;
    ++indent_level;
    groups_[i].Print(os, indent_level);
    --indent_level;
  }
}

void Database::Summary(ostream& os) const {
  os << string(22, '=') << " DESIGN SUMMARY " << string(22, '=') << endl;
  os << "Design Name: " << design_name_ << endl;
  os << "Die Rect: " << die_rect_ << endl;
  os << "Site Size: " << Site::width() << " x " << Site::height() << endl;
  os << "# of Rows: " << rows_.size() << endl;
  os << "# of Sites: " << sites_.size() << endl;
  os << "# of Layers: " << layers_.size() << endl;
  os << "# of Cells: " << cells_.size() << endl;
  os << "# of Instances: " << instances_.size() << endl;
  os << "  # of Fixed: " << fixed_instance_ids_.size() << endl;
  for (int i = 0; i < instance_ids_by_row_height_.size(); ++i) {
    os << "  # of " << i + 1
       << "-Row-Height: " << instance_ids_by_row_height_[i].size() << endl;
  }
  os << "# of Nets: " << nets_.size() << endl;
  os << "# of Fence Regions: " << fence_regions_.size() << endl;
  os << "# of Blockages: " << blockages_.size() << endl;
  os << "# of Intervals: " << intervals_.size() << endl;
  os << "# of SubInstances: " << sub_instances_.size() << endl;
  os << string(60, '=') << endl;
}

void Database::Plot(ostream& os, const string& title) const {
  Plotter plotter(title);

  // TODO: Change colors.
  vector<string> fence_region_colors{
      "#B5A217", "#D1BD2B", "#EDD102", "#F4D800", "#F2D500", "#F4DF3D",
      "#D6BD02", "#BFA800", "#A59203", "#E2DCA7", "#A09439", "#7C7647"};

  LineStyleId die_rect_style_id = plotter.add_line_style("lc rgb 'green' lw 5");
  vector<LineStyleId> fence_region_style_ids;
  for (int i = 0; i < fence_regions_.size(); ++i) {
    fence_region_style_ids.push_back(
        plotter.add_line_style("lc rgb '" + fence_region_colors[i] + "' lw 3"));
  }
  LineStyleId row_style_id =
      plotter.add_line_style("lc rgb 'grey' lw 1 dt '-'");
  LineStyleId power_rail_style_id =
      plotter.add_line_style("lc rgb 'orange' lw 1 dt '-'");
  LineStyleId ground_rail_style_id =
      plotter.add_line_style("lc rgb 'blue' lw 1 dt '-'");
  LineStyleId instance_style_id = plotter.add_line_style("lc rgb 'black' lw 1");
  LineStyleId forbidden_instance_style_id = plotter.add_line_style("lc rgb 'red' lw 1");
  vector<LineStyleId> instance_fence_region_style_ids;
  for (int i = 0; i < fence_regions_.size(); ++i) {
    instance_fence_region_style_ids.push_back(
        plotter.add_line_style("lc rgb '" + fence_region_colors[i] + "' lw 1"));
  }
  ArrowStyleId displacement_style_id =
      plotter.add_arrow_style("head nofilled lc rgb 'purple' lw 1");

  // Plot die rect.

  plotter.add_rect(die_rect_, die_rect_style_id);

  // Plot fence regions.

  for (int i = 0; i < fence_regions_.size(); ++i) {
    const FenceRegion& fence_region = fence_regions_[i];
    for (int j = 0; j < fence_region.num_rects(); ++j) {
      const Rect& rect = fence_region.rect(j);
      plotter.add_rect(rect, fence_region_style_ids[i]);
    }
  }

  // Plot rails.

  for (const Row& row : rows_) {
    const RailId rail_id = row.rail_id_on_layer(LayerId(0));

    if (rail_id == UNDEFINED_ID) {
      plotter.add_line(
          Line(Point(die_rect_.min_corner().x(), row.position().y()),
               Point(die_rect_.max_corner().x(), row.position().y())),
          row_style_id);
      continue;
    }

    const Rail& rail = rails_.at(rail_id);
    if (rail.type() == NetType::GROUND) {
      plotter.add_line(
          Line(Point(die_rect_.min_corner().x(), row.position().y()),
               Point(die_rect_.max_corner().x(), row.position().y())),
          ground_rail_style_id);
    } else {
      plotter.add_line(
          Line(Point(die_rect_.min_corner().x(), row.position().y()),
               Point(die_rect_.max_corner().x(), row.position().y())),
          power_rail_style_id);
    }
  }

  // Plot instances.

  for (int i = 0; i < instances_.size(); ++i) {
    const Instance& instance = instances_[i];
    //if(instance.outoffence==false){ continue; }
    const double width = instance.width();
    const double height = instance.height();
    const Rect rect(instance.position(),
                    Point(instance.position().x() + width,
                          instance.position().y() + height));
    const FenceRegionId fence_region_id = instance.fence_region_id();
	if(instance.forbidden_cell == false){
		plotter.add_rect(rect,
                     fence_region_id != UNDEFINED_ID
                         ? instance_fence_region_style_ids.at(fence_region_id)
                         : instance_style_id);

		const Orientation orientation = instance.orientation();
		const double span = width < height ? width / 2.0 : height / 2.0;
		switch (orientation) {
		  case Orientation::N: {
			plotter.add_line(
				Line(
					Point(instance.position().x() + span, instance.position().y()),
					Point(instance.position().x(), instance.position().y() + span)),
				fence_region_id != UNDEFINED_ID
					? instance_fence_region_style_ids.at(fence_region_id)
					: instance_style_id);
			break;
		  }
		  case Orientation::S: {
			plotter.add_line(
				Line(Point(instance.position().x() + span,
						   instance.position().y() + height),
					 Point(instance.position().x() + width,
						   instance.position().y() + height - span)),
				fence_region_id != UNDEFINED_ID
					? instance_fence_region_style_ids.at(fence_region_id)
					: instance_style_id);
			break;
		  }
		  case Orientation::FN: {
			plotter.add_line(
				Line(Point(instance.position().x() + span, instance.position().y()),
					 Point(instance.position().x() + width,
						   instance.position().y() + span)),
				fence_region_id != UNDEFINED_ID
					? instance_fence_region_style_ids.at(fence_region_id)
					: instance_style_id);
			break;
		  }
		  case Orientation::FS: {
			plotter.add_line(
				Line(Point(instance.position().x() + span,
						   instance.position().y() + height),
					 Point(instance.position().x(),
						   instance.position().y() + height - span)),
				fence_region_id != UNDEFINED_ID
					? instance_fence_region_style_ids.at(fence_region_id)
					: instance_style_id);
			break;
		  }
		  default: { break; }
		}
	}
	else{
		plotter.add_rect(rect,
                     fence_region_id != UNDEFINED_ID
                         ? instance_fence_region_style_ids.at(fence_region_id)
                         : forbidden_instance_style_id);

		const Orientation orientation = instance.orientation();
		const double span = width < height ? width / 2.0 : height / 2.0;
		switch (orientation) {
		  case Orientation::N: {
			plotter.add_line(
				Line(
					Point(instance.position().x() + span, instance.position().y()),
					Point(instance.position().x(), instance.position().y() + span)),
				fence_region_id != UNDEFINED_ID
					? instance_fence_region_style_ids.at(fence_region_id)
					: forbidden_instance_style_id);
			break;
		  }
		  case Orientation::S: {
			plotter.add_line(
				Line(Point(instance.position().x() + span,
						   instance.position().y() + height),
					 Point(instance.position().x() + width,
						   instance.position().y() + height - span)),
				fence_region_id != UNDEFINED_ID
					? instance_fence_region_style_ids.at(fence_region_id)
					: forbidden_instance_style_id);
			break;
		  }
		  case Orientation::FN: {
			plotter.add_line(
				Line(Point(instance.position().x() + span, instance.position().y()),
					 Point(instance.position().x() + width,
						   instance.position().y() + span)),
				fence_region_id != UNDEFINED_ID
					? instance_fence_region_style_ids.at(fence_region_id)
					: forbidden_instance_style_id);
			break;
		  }
		  case Orientation::FS: {
			plotter.add_line(
				Line(Point(instance.position().x() + span,
						   instance.position().y() + height),
					 Point(instance.position().x(),
						   instance.position().y() + height - span)),
				fence_region_id != UNDEFINED_ID
					? instance_fence_region_style_ids.at(fence_region_id)
					: forbidden_instance_style_id);
			break;
		  }
		  default: { break; }
		}
		
	}
    
  }

  // Plot displacements.

 /* for (int i = 0; i < instances_.size(); ++i) {
    const Instance& instance = instances_[i];
    plotter.add_arrow(
        Line(Point(
                 instance.global_placed_position().x() + instance.width() / 2.0,
                 instance.global_placed_position().y() +
                     instance.height() / 2.0),
             instance.position() +
                 Point(instance.width() / 2.0, instance.height() / 2.0)),
        displacement_style_id);
  }
 */
  plotter.Plot(os);
}

// Getters

int Database::lef_unit() const {
  return lef_unit_;
}

double Database::edge_type_spacing(EdgeType edge_type_a,
                                   EdgeType edge_type_b) const {
  return edge_type_spacing_table_.at(edge_type_a).at(edge_type_b);
}

const string& Database::design_name() const {
  return design_name_;
}

int Database::def_unit() const {
  return def_unit_;
}

const Rect& Database::die_rect() const {
  return die_rect_;
}

double Database::density_target() const {
  return density_target_;
}

double Database::displacement_limit() const {
  return displacement_limit_;
}

int Database::max_instance_row_height() const {
  return instance_ids_by_row_height_.size();
}

int Database::num_layers() const {
  return layers_.size();
}

int Database::num_cells() const {
  return cells_.size();
}

int Database::num_cell_pins() const {
  return cell_pins_.size();
}

int Database::num_rows() const {
  return rows_.size();
}

int Database::num_sites() const {
  return sites_.size();
}

int Database::num_fence_regions() const {
  return fence_regions_.size();
}

int Database::num_rails() const {
  return rails_.size();
}

int Database::num_instances() const {
  return instances_.size();
}

int Database::num_instance_pins() const {
  return instance_pins_.size();
}

int Database::num_pins() const {
  return pins_.size();
}

int Database::num_blockages() const {
  return blockages_.size();
}

int Database::num_nets() const {
  return nets_.size();
}

int Database::num_groups() const {
  return groups_.size();
}

int Database::num_intervals() const {
  return intervals_.size();
}

int Database::num_sub_instances() const {
  return sub_instances_.size();
}

int Database::num_windows() const {
  return windows_.size();
}

int Database::num_variables() const {
  return variables_.size();
}

int Database::num_instances_of_row_height(int row_height) const {
  return instance_ids_by_row_height_.at(row_height - 1).size();
}

int Database::num_fixed_instances() const {
  return fixed_instance_ids_.size();
}

const Layer& Database::layer(LayerId id) const {
  return layers_.at(id);
}

const Cell& Database::cell(CellId id) const {
  return cells_.at(id);
}

const CellPin& Database::cell_pin(CellPinId id) const {
  return cell_pins_.at(id);
}

const Row& Database::row(RowId id) const {
  return rows_.at(id);
}

const Site& Database::site(SiteId id) const {
  return sites_.at(id);
}

const FenceRegion& Database::fence_region(FenceRegionId id) const {
  return fence_regions_.at(id);
}

const Rail& Database::rail(RailId id) const {
  return rails_.at(id);
}

const Instance& Database::instance(InstanceId id) const {
  return instances_.at(id);
}

const InstancePin& Database::instance_pin(InstancePinId id) const {
  return instance_pins_.at(id);
}

const Pin& Database::pin(PinId id) const {
  return pins_.at(id);
}

const Blockage& Database::blockage(BlockageId id) const {
  return blockages_.at(id);
}

const Net& Database::net(NetId id) const {
  return nets_.at(id);
}

const Group& Database::group(GroupId id) const {
  return groups_.at(id);
}

const Interval& Database::interval(IntervalId id) const {
  return intervals_.at(id);
}

const SubInstance& Database::sub_instance(SubInstanceId id) const {
  return sub_instances_.at(id);
}

const Window& Database::window(WindowId id) const {
  return windows_.at(id);
}

const Variable& Database::variable(VariableId id) const {
  return variables_.at(id);
}

LayerId Database::layer_id_by_name(const string& name) const {
  if (layer_id_by_name_.count(name) == 0) {
    return LayerId(UNDEFINED_ID);
  }

  return layer_id_by_name_.at(name);
}

CellId Database::cell_id_by_name(const string& name) const {
  if (cell_id_by_name_.count(name) == 0) {
    return CellId(UNDEFINED_ID);
  }

  return cell_id_by_name_.at(name);
}

CellPinId Database::cell_pin_id_by_name(const string& name) const {
  if (cell_pin_id_by_name_.count(name) == 0) {
    return CellPinId(UNDEFINED_ID);
  }

  return cell_pin_id_by_name_.at(name);
}

RowId Database::row_id_by_name(const string& name) const {
  if (row_id_by_name_.count(name) == 0) {
    return RowId(UNDEFINED_ID);
  }

  return row_id_by_name_.at(name);
}

SiteId Database::site_id_by_position(const Point& position) const {
  assert(position.x() >= die_rect_.min_corner().x() &&
         position.x() < die_rect_.max_corner().x());
  assert(position.y() >= die_rect_.min_corner().y() &&
         position.y() < die_rect_.max_corner().y());

  const double row_height = Site::height();
  const double site_width = Site::width();

  return site_id_by_position_
      .at(static_cast<int>((position.y() - die_rect_.min_corner().y()) /
                           row_height))
      .at(static_cast<int>((position.x() - die_rect_.min_corner().x()) /
                           site_width));
}

FenceRegionId Database::fence_region_id_by_name(const string& name) const {
  if (fence_region_id_by_name_.count(name) == 0) {
    return FenceRegionId(UNDEFINED_ID);
  }

  return fence_region_id_by_name_.at(name);
}

InstanceId Database::instance_id_by_name(const string& name) const {
  if (instance_id_by_name_.count(name) == 0) {
    return InstanceId(UNDEFINED_ID);
  }

  return instance_id_by_name_.at(name);
}

InstancePinId Database::instance_pin_id_by_name(const string& name) const {
  if (instance_pin_id_by_name_.count(name) == 0) {
    return InstancePinId(UNDEFINED_ID);
  }

  return instance_pin_id_by_name_.at(name);
}

PinId Database::pin_id_by_name(const string& name) const {
  if (pin_id_by_name_.count(name) == 0) {
    return PinId(UNDEFINED_ID);
  }

  return pin_id_by_name_.at(name);
}

NetId Database::net_id_by_name(const string& name) const {
  if (net_id_by_name_.count(name) == 0) {
    return NetId(UNDEFINED_ID);
  }

  return net_id_by_name_.at(name);
}

GroupId Database::group_id_by_name(const string& name) const {
  if (group_id_by_name_.count(name) == 0) {
    return GroupId(UNDEFINED_ID);
  }

  return group_id_by_name_.at(name);
}

GroupId Database::group_id_by_pattern(const string& pattern) const {
  if (group_id_by_pattern_.count(pattern) == 0) {
    return GroupId(UNDEFINED_ID);
  }

  return group_id_by_pattern_.at(pattern);
}

InstanceId Database::instance_id_by_row_height(int row_height, int idx) const {
  return instance_ids_by_row_height_.at(row_height - 1).at(idx);
}

InstanceId Database::fixed_instance_id(int idx) const {
  return fixed_instance_ids_.at(idx);
}

double Database::ComputeTotalDisplacement() const {
  double total_displacement = 0;

  for (int i = 0; i < instances_.size(); ++i) {
    const Instance& instance = instances_[i];

    total_displacement += ComputeManhattanDistanceBetweenPoints(
        instance.position(), instance.global_placed_position());
  }

  return total_displacement;
}

double Database::ComputeTotalInstanceArea() const {
  double total_instance_area = 0;

  for (int i = 0; i < instances_.size(); ++i) {
    const Instance& instance = instances_[i];

    total_instance_area += (instance.width() * instance.height());
  }

  return total_instance_area;
}

// Setters

void Database::reserve_instances(int num_instances) {
  instances_.reserve(num_instances);
  instance_id_by_name_.reserve(num_instances);
}

void Database::reserve_instance_pins(int num_instance_pins) {
  instance_pins_.reserve(num_instance_pins);
  instance_pin_id_by_name_.reserve(num_instance_pins);
}

void Database::reserve_sub_instances(int num_sub_instances) {
  sub_instances_.reserve(num_sub_instances);
}

void Database::reserve_rows(int num_rows) {
  rows_.reserve(num_rows);
}

void Database::reserve_sites(int num_sites) {
  sites_.reserve(num_sites);
}

void Database::reserve_pins(int num_pins) {
  pins_.reserve(num_pins);
  pin_id_by_name_.reserve(num_pins);
}

void Database::reserve_nets(int num_nets) {
  nets_.reserve(num_nets);
  net_id_by_name_.reserve(num_nets);
}

void Database::initialize_site_id_by_position(int num_y_sites,
                                              int num_x_sites) {
  site_id_by_position_ = vector<vector<SiteId>>(
      num_y_sites, vector<SiteId>(num_x_sites, SiteId(UNDEFINED_ID)));
}

void Database::set_lef_unit(int lef_unit) {
  lef_unit_ = lef_unit;
}

void Database::set_edge_type_spacing(EdgeType edge_type_a, EdgeType edge_type_b,
                                     double spacing) {
  edge_type_spacing_table_.at(edge_type_a).at(edge_type_b) = spacing;
}

void Database::set_design_name(const string& design_name) {
  design_name_ = design_name;
}

void Database::set_def_unit(int def_unit) {
  def_unit_ = def_unit;
}

void Database::set_die_rect(const Rect& rect) {
  die_rect_ = rect;
}

void Database::set_density_target(double density_target) {
  density_target_ = density_target;
}

void Database::set_displacement_limit(double displacement_limit) {
  displacement_limit_ = displacement_limit;
}

Layer& Database::layer(LayerId id) {
  return layers_.at(id);
}

Cell& Database::cell(CellId id) {
  return cells_.at(id);
}

CellPin& Database::cell_pin(CellPinId id) {
  return cell_pins_.at(id);
}

Row& Database::row(RowId id) {
  return rows_.at(id);
}

Site& Database::site(SiteId id) {
  return sites_.at(id);
}

FenceRegion& Database::fence_region(FenceRegionId id) {
  return fence_regions_.at(id);
}

Rail& Database::rail(RailId id) {
  return rails_.at(id);
}

Instance& Database::instance(InstanceId id) {
  return instances_.at(id);
}

InstancePin& Database::instance_pin(InstancePinId id) {
  return instance_pins_.at(id);
}

Pin& Database::pin(PinId id) {
  return pins_.at(id);
}

Blockage& Database::blockage(BlockageId id) {
  return blockages_.at(id);
}

Net& Database::net(NetId id) {
  return nets_.at(id);
}

Group& Database::group(GroupId id) {
  return groups_.at(id);
}

Interval& Database::interval(IntervalId id) {
  return intervals_.at(id);
}

SubInstance& Database::sub_instance(SubInstanceId id) {
  return sub_instances_.at(id);
}

Window& Database::window(WindowId id) {
  return windows_.at(id);
}

Variable& Database::variable(VariableId id) {
  return variables_.at(id);
}

LayerId Database::add_layer(const Layer& layer) {
  const LayerId layer_id(layers_.size());

  layers_.push_back(layer);

  return layer_id;
}

CellId Database::add_cell(const Cell& cell) {
  const CellId cell_id(cells_.size());

  cells_.push_back(cell);

  return cell_id;
}

CellPinId Database::add_cell_pin(const CellPin& cell_pin) {
  const CellPinId cell_pin_id(cell_pins_.size());

  cell_pins_.push_back(cell_pin);

  return cell_pin_id;
}

RowId Database::add_row(const Row& row) {
  const RowId row_id(rows_.size());

  rows_.push_back(row);

  return row_id;
}

SiteId Database::add_site(const Site& site) {
  const SiteId site_id(sites_.size());

  sites_.push_back(site);

  return site_id;
}

FenceRegionId Database::add_fence_region(const FenceRegion& fence_region) {
  const FenceRegionId fence_region_id(fence_regions_.size());

  fence_regions_.push_back(fence_region);

  return fence_region_id;
}

RailId Database::add_rail(const Rail& rail) {
  const RailId rail_id(rails_.size());

  rails_.push_back(rail);

  return rail_id;
}

InstanceId Database::add_instance(const Instance& instance) {
  const InstanceId instance_id(instances_.size());

  instances_.push_back(instance);

  return instance_id;
}

InstancePinId Database::add_instance_pin(const InstancePin& instance_pin) {
  const InstancePinId instance_pin_id(instance_pins_.size());

  instance_pins_.push_back(instance_pin);

  return instance_pin_id;
}

PinId Database::add_pin(const Pin& pin) {
  const PinId pin_id(pins_.size());

  pins_.push_back(pin);

  return pin_id;
}

BlockageId Database::add_blockage(const Blockage& blockage) {
  const BlockageId blockage_id(blockages_.size());

  blockages_.push_back(blockage);

  return blockage_id;
}

NetId Database::add_net(const Net& net) {
  const NetId net_id(nets_.size());

  nets_.push_back(net);

  return net_id;
}

GroupId Database::add_group(const Group& group) {
  const GroupId group_id(groups_.size());

  groups_.push_back(group);

  return group_id;
}

IntervalId Database::add_interval(const Interval& interval) {
  const IntervalId interval_id(intervals_.size());

  intervals_.push_back(interval);

  return interval_id;
}

SubInstanceId Database::add_sub_instance(const SubInstance& sub_instance) {
  const SubInstanceId sub_instance_id(sub_instances_.size());

  sub_instances_.push_back(sub_instance);

  return sub_instance_id;
}

WindowId Database::add_window(const Window& window) {
  const WindowId window_id(windows_.size());

  windows_.push_back(window);

  return window_id;
}

VariableId Database::add_variable(const Variable& variable) {
  const VariableId variable_id(variables_.size());

  variables_.push_back(variable);

  return variable_id;
}

void Database::add_instance_id_of_row_height(int row_height, InstanceId id) {
  if (row_height > instance_ids_by_row_height_.size()) {
    instance_ids_by_row_height_.resize(row_height);
  }

  instance_ids_by_row_height_.at(row_height - 1).push_back(id);
}

void Database::add_fixed_instance_id(InstanceId id) {
  fixed_instance_ids_.push_back(id);
}

void Database::index_layer_by_name(LayerId id, const string& name) {
  layer_id_by_name_.insert(make_pair(name, id));
}

void Database::index_cell_by_name(CellId id, const string& name) {
  cell_id_by_name_.insert(make_pair(name, id));
}

void Database::index_cell_pin_by_name(CellPinId id, const string& name) {
  cell_pin_id_by_name_.insert(make_pair(name, id));
}

void Database::index_row_by_name(RowId id, const string& name) {
  row_id_by_name_.insert(make_pair(name, id));
}

void Database::index_site_by_position(SiteId id, const Point& position) {
  assert(position.x() >= die_rect_.min_corner().x() &&
         position.x() < die_rect_.max_corner().x());
  assert(position.y() >= die_rect_.min_corner().y() &&
         position.y() < die_rect_.max_corner().y());

  const double row_height = Site::height();
  const double site_width = Site::width();

  site_id_by_position_
      .at(static_cast<int>((position.y() - die_rect_.min_corner().y()) /
                           row_height))
      .at(static_cast<int>((position.x() - die_rect_.min_corner().x()) /
                           site_width)) = id;
}

void Database::index_fence_region_by_name(FenceRegionId id,
                                          const string& name) {
  fence_region_id_by_name_.insert(make_pair(name, id));
}

void Database::index_instance_by_name(InstanceId id, const string& name) {
  instance_id_by_name_.insert(make_pair(name, id));
}

void Database::index_instance_pin_by_name(InstancePinId id,
                                          const string& name) {
  instance_pin_id_by_name_.insert(make_pair(name, id));
}

void Database::index_pin_by_name(PinId id, const string& name) {
  pin_id_by_name_.insert(make_pair(name, id));
}

void Database::index_net_by_name(NetId id, const string& name) {
  net_id_by_name_.insert(make_pair(name, id));
}

void Database::index_group_by_name(GroupId id, const string& name) {
  group_id_by_name_.insert(make_pair(name, id));
}

void Database::index_group_by_pattern(GroupId id, const string& pattern) {
  group_id_by_pattern_.insert(make_pair(pattern, id));
}

void Database::ConvertLefMicronsToDefUnits() {
  Site::set_size(Site::width() * def_unit_, Site::height() * def_unit_);

  for (int i = 0; i < num_edge_types_; ++i) {
    for (int j = 0; j < num_edge_types_; ++j) {
      const EdgeType edge_type_a(i);
      const EdgeType edge_type_b(j);
      const double spacing =
          edge_type_spacing_table_.at(edge_type_a).at(edge_type_b);
      edge_type_spacing_table_.at(edge_type_a).at(edge_type_b) =
          spacing * def_unit_;
    }
  }

  for (int i = 0; i < cells_.size(); ++i) {
    Cell& cell = cells_[i];
    cell.set_size(cell.width() * def_unit_, cell.height() * def_unit_);
  }

  for (int i = 0; i < cell_pins_.size(); ++i) {
    CellPin& pin = cell_pins_[i];
    for (int j = 0; j < pin.num_ports(); ++j) {
      CellPinPort& port = pin.port(j);
      const Rect& port_rect = port.rect();

      // Assume origins of cells are always (0, 0).
      port.set_rect(Rect(Point(port_rect.min_corner().x() * def_unit_,
                               port_rect.min_corner().y() * def_unit_),
                         Point(port_rect.max_corner().x() * def_unit_,
                               port_rect.max_corner().y() * def_unit_)));
    }
  }
}

void Database::SortRowsByY() {
  sort(rows_.begin(), rows_.end(), [](const Row& row_a, const Row& row_b) {
    return row_a.position().y() < row_b.position().y();
  });
}

void Database::AssignRailsToRows() {
  const double row_height = Site::height();

  for (int i = 0; i < rails_.size(); ++i) {
    const RailId rail_id(i);
    const Rail& rail = rails_[i];
    const double rail_y = rail.middle_line().first.y();

    const double n = rail_y / row_height;
    if (n == floor(n)) {  // Check if n is integer.
      const int row_idx = static_cast<int>(n);
      if (row_idx < rows_.size()) {
        rows_[row_idx].set_rail_id_on_layer(rail_id, rail.layer_id());
      }
    }
  }
}

void Database::SplitRowsIntoIntervals() {
  const int num_rows = rows_.size();
  const double row_height = Site::height();
  // Assume left and right boundaries of all rows are aligned with die.
  const double row_left_x = die_rect_.min_corner().x();
  const double row_right_x = die_rect_.max_corner().x();

  vector<map<double, Interval>> interval_by_begin_by_row_idx(
      num_rows, map<double, Interval>());

  // Initialize an interval of die width on each row.
  for (int i = 0; i < interval_by_begin_by_row_idx.size(); ++i) {
    const RowId row_id(i);
    interval_by_begin_by_row_idx[i].insert(
        make_pair(row_left_x, Interval(row_id, row_left_x, row_right_x,
                                       FenceRegionId(UNDEFINED_ID))));
  }

  // Fence regions

  for (int i = 0; i < fence_regions_.size(); ++i) {
    const FenceRegionId fence_region_id(i);
    const FenceRegion& fence_region = fence_regions_.at(fence_region_id);

    for (int j = 0; j < fence_region.num_rects(); ++j) {
      const Rect& rect = fence_region.rect(j);
      const double fence_region_left_x = rect.min_corner().x();
      const double fence_region_lower_y = rect.min_corner().y();
      double fence_region_right_x = rect.max_corner().x();
      const double fence_region_upper_y = rect.max_corner().y();

      int current_row_idx = static_cast<int>(fence_region_lower_y / row_height);
      while (current_row_idx < num_rows &&
             rows_.at(current_row_idx).position().y() < fence_region_upper_y) {
        const RowId current_row_id(current_row_idx);
        auto& current_interval_by_begin =
            interval_by_begin_by_row_idx.at(current_row_idx);

        auto it = current_interval_by_begin.upper_bound(fence_region_left_x);

        --it;

        Interval& interval = it->second;
        const double interval_old_end = interval.end();
        const FenceRegionId interval_old_fence_region_id =
            interval.fence_region_id();

        ++it;

        // Shrink the interval and insert a new interval corresponding to the
        // fence region, otherwise, just extend the interval and update its
        // fence_region_id.
        if (fence_region_left_x != interval.begin()) {
          interval.set_end(fence_region_left_x);

          current_interval_by_begin.insert(
              it, make_pair(fence_region_left_x,
                            Interval(current_row_id, fence_region_left_x,
                                     fence_region_right_x, fence_region_id)));
        } else {
          interval.set_fence_region_id(fence_region_id);
          interval.set_end(fence_region_right_x);
        }

        // Some fence regions are out of right boundary.
        if (fence_region_right_x > row_right_x) {
          fence_region_right_x = row_right_x;
        }

        if (fence_region_right_x != interval_old_end) {
          current_interval_by_begin.insert(
              it, make_pair(fence_region_right_x,
                            Interval(current_row_id, fence_region_right_x,
                                     interval_old_end,
                                     interval_old_fence_region_id)));
        }

        ++current_row_idx;
      }
    }
  }

  // Combine intervals.
  for (int i = 0; i < interval_by_begin_by_row_idx.size(); ++i) {
    auto& current_interval_by_begin = interval_by_begin_by_row_idx[i];

    auto it = current_interval_by_begin.begin();
    auto last_it = it;

    ++it;

    while (it != current_interval_by_begin.end()) {
      const Interval& interval = it->second;
      Interval& last_interval = last_it->second;

      if (interval.fence_region_id() == last_interval.fence_region_id() &&
          interval.begin() == last_interval.end()) {
        last_interval.set_end(interval.end());

        it = current_interval_by_begin.erase(it);

        continue;
      }

      last_it = it;
      ++it;
    }
  }

  // Fixed instances

  for (InstanceId fixed_instance_id : fixed_instance_ids_) {
    const Instance& fixed_instance = instances_.at(fixed_instance_id);
    const double fixed_instance_left_x = fixed_instance.position().x();
    const double fixed_instance_lower_y = fixed_instance.position().y();
    const double fixed_instance_width = fixed_instance.width();
    const double fixed_instance_height = fixed_instance.height();
    const double fixed_instance_right_x =
        fixed_instance_left_x + fixed_instance_width;
    const double fixed_instance_upper_y =
        fixed_instance_lower_y + fixed_instance_height;

    int current_row_idx = static_cast<int>(fixed_instance_lower_y / row_height);
    while (current_row_idx < num_rows &&
           rows_.at(current_row_idx).position().y() < fixed_instance_upper_y) {
      const RowId current_row_id(current_row_idx);
      auto& current_interval_by_begin =
          interval_by_begin_by_row_idx.at(current_row_idx);

      auto it_begin =
          current_interval_by_begin.upper_bound(fixed_instance_left_x);
      auto it_end =
          current_interval_by_begin.upper_bound(fixed_instance_right_x);

      --it_begin;
      --it_end;

      if (it_begin == it_end) {
        auto it = it_begin;
        Interval& interval = it->second;
        const double interval_old_end = interval.end();
        const FenceRegionId interval_old_fence_region_id =
            interval.fence_region_id();

        if (fixed_instance_left_x != interval.begin()) {
          interval.set_end(fixed_instance_left_x);

          ++it;
        } else {
          it = current_interval_by_begin.erase(it);
          // DO NOT use variable 'interval' afterwards since it is removed.
        }

        if (fixed_instance_right_x != interval_old_end) {
          current_interval_by_begin.insert(
              it, make_pair(fixed_instance_right_x,
                            Interval(current_row_id, fixed_instance_right_x,
                                     interval_old_end,
                                     interval_old_fence_region_id)));
        }
      } else {
        it_begin->second.set_end(fixed_instance_left_x);
        it_end->second.set_begin(fixed_instance_right_x);

        if (it_begin->second.capacity() == 0) {
          it_begin = current_interval_by_begin.erase(it_begin);
        } else {
          ++it_begin;
        }

        current_interval_by_begin.erase(it_begin, it_end);

        if (it_end->second.capacity() == 0) {
          current_interval_by_begin.erase(it_end);
        }
      }

      ++current_row_idx;
    }
  }

  // Save all intervals to database.

  for (int i = 0; i < rows_.size(); ++i) {
    Row& row = rows_[i];
    row.prepare_for_adding_interval_id(fence_regions_.size());
  }

  for (int i = 0; i < interval_by_begin_by_row_idx.size(); ++i) {
    const RowId row_id(i);
    const auto& interval_by_begin = interval_by_begin_by_row_idx[i];
    Row& row = rows_.at(row_id);
    for (auto it = interval_by_begin.begin(); it != interval_by_begin.end();
         ++it) {
      const double begin = it->first;
      const Interval& interval = it->second;
      const IntervalId interval_id = add_interval(interval);

      row.add_interval_id(interval_id, begin, interval.fence_region_id());
    }
  }
}

void Database::AssignFenceRegionsToInstances() {
  for (int i = 0; i < instances_.size(); ++i) {
    const InstanceId instance_id(i);
    Instance& instance = instances_.at(instance_id);
    const string instance_name(instance.name());

    bool is_pattern_found = false;
    for (int j = 0; j < groups_.size(); ++j) {
      const GroupId group_id(j);
      const Group& group = groups_.at(group_id);

      for (int k = 0; k < group.num_patterns(); ++k) {
        const int pattern_id = k;
        string pattern(group.pattern(pattern_id));

        if (pattern.back() == '*') {
          pattern.pop_back();
        }

        if (instance_name.find(pattern) != string::npos) {
          const FenceRegionId fence_region_id(group.fence_region_id());
          instance.set_fence_region_id(fence_region_id);
          is_pattern_found = true;
          break;
        }
      }

      if (is_pattern_found) {
        break;
      }
    }
  }
}

void Database::UpdateInstanceSubInstancePositions(InstanceId instance_id) {
  const double row_height = Site::height();
  const Instance& instance = instances_.at(instance_id);

  for (int i = 0; i < instance.num_sub_instances(); ++i) {
    const SubInstanceId sub_instance_id(instance.sub_instance_id(i));
    SubInstance& sub_instance = sub_instances_.at(sub_instance_id);

    sub_instance.set_position(instance.position() + Point(0.0, i * row_height));
  }
}
