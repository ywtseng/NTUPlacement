#include "parser.hpp"

#include "../util/const.hpp"
#include "simple_parser.hpp"

#include <cstdio>
#include <sstream>

using namespace std;

// Public members

Parser::Parser(Database& database, const string& tech_lef_name,
               const string& cell_lef_name,
               const string& global_placed_def_name,
               const string& placement_constraints_name)
    : database_(database) {
  ParseTechLef(tech_lef_name);
  ParseCellLef(cell_lef_name);
  ParseGlobalPlacedDef(global_placed_def_name);
  ParsePlacementConstraints(placement_constraints_name);
}

// Tech lef callbacks

int LefUnitCallback(lefrCallbackType_e typ, lefiUnits* unit,
                    lefiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);

  if (unit->hasDatabase()) {
    const int lef_unit = unit->databaseNumber();
    database.set_lef_unit(lef_unit);
  }

  return 0;
}

int PropertyCallback(lefrCallbackType_e typ, lefiProp* property,
                     lefiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const string property_name(property->propName());

  // Parse edge type spacing rules.
  if (property_name == "LEF58_CELLEDGESPACINGTABLE") {
    istringstream iss(string(property->string()));
    simple_parser::Parser parser(iss);
    parser.Parse([&](const simple_parser::Tokens& tokens) -> bool {
      if (!tokens.empty()) {
        string first_word = tokens[0];
        if (first_word == "EDGETYPE") {
          const EdgeType edge_type_a(stoi(tokens[1]));
          const EdgeType edge_type_b(stoi(tokens[2]));
          const double spacing = stod(tokens[3]);
          database.set_edge_type_spacing(edge_type_a, edge_type_b, spacing);
          database.set_edge_type_spacing(edge_type_b, edge_type_a, spacing);
        }
      }
      return true;
    });
  }

  return 0;
}

int LayerCallback(lefrCallbackType_e typ, lefiLayer* layer,
                  lefiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);

  if (layer->hasType()) {
    const string type(layer->type());

    if (type == "ROUTING") {
      const string name(layer->name());
      const LayerId layer_id = database.add_layer(name);
      database.index_layer_by_name(layer_id, name);
    }
  }

  return 0;
}

int SiteCallback(lefrCallbackType_e typ, lefiSite* site,
                 lefiUserData database_ptr) {
  const double width = site->sizeX();
  const double height = site->sizeY();

  Site::set_size(width, height);

  return 0;
}

// Cell lef callbacks

int MacroBeginCallback(lefrCallbackType_e typ, const char* macroName,
                       lefiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const string name(macroName);

  const CellId cell_id = database.add_cell(Cell(name));
  database.index_cell_by_name(cell_id, name);

  return 0;
}

int MacroCallback(lefrCallbackType_e typ, lefiMacro* macro,
                  lefiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const string type(macro->hasClass() ? macro->macroClass() : UNDEFINED_STRING);
  const CellId current_cell_id(database.num_cells() - 1);
  Cell& current_cell = database.cell(current_cell_id);
  const Point origin(macro->hasOrigin()
                         ? Point(macro->originX(), macro->originY())
                         : Point(ORIGIN_POINT));
  const double width = macro->sizeX();
  const double height = macro->sizeY();

  current_cell.set_type(type);
  current_cell.set_origin(origin);
  current_cell.set_size(width, height);

  for (int i = 0; i < macro->numProperties(); ++i) {
    const string property_name(macro->propName(i));
    if (property_name == "LEF58_EDGETYPE") {
      istringstream iss(string(macro->propValue(i)));
      simple_parser::Parser parser(iss);
      EdgeType left_edge_type(0);
      EdgeType right_edge_type(0);
      parser.Parse([&](const simple_parser::Tokens& tokens) -> bool {
        if (!tokens.empty()) {
          string first_word = tokens[0];
          if (first_word == "EDGETYPE") {
            const EdgeType edge_type(stoi(tokens[2]));
            if (tokens[1] == "LEFT") {
              left_edge_type = edge_type;
            } else {
              right_edge_type = edge_type;
            }
          }
        }
        return true;
      });

      current_cell.set_edge_types(left_edge_type, right_edge_type);
    }
  }

  return 0;
}

int MacroPinCallback(lefrCallbackType_e typ, lefiPin* pin,
                     lefiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const CellId current_cell_id(database.num_cells() - 1);
  Cell& current_cell = database.cell(current_cell_id);
  const string name(pin->name());
  const NetType type = [&]() {
    if (pin->hasUse()) {
      const string use(pin->use());
      if (use == "GROUND") {
        return NetType::GROUND;
      }
      if (use == "POWER") {
        return NetType::POWER;
      }
      return NetType::NONE;
    }
    return NetType::NONE;
  }();

  const string cell_pin_name(current_cell.name() + "." + name);
  CellPinId cell_pin_id =
      database.add_cell_pin(CellPin(current_cell_id, cell_pin_name, type));
  /* database.index_cell_pin_by_name(cell_pin_id, cell_pin_name); */

  CellPin& cell_pin = database.cell_pin(cell_pin_id);
  current_cell.add_pin_id(cell_pin_id);

  if (type == NetType::GROUND) {
    current_cell.set_ground_pin_id(cell_pin_id);
  } else if (type == NetType::POWER) {
    current_cell.set_power_pin_id(cell_pin_id);
  }

  for (int i = 0; i < pin->numPorts(); ++i) {
    const lefiGeometries* geo = pin->port(i);
    LayerId layer_id(UNDEFINED_ID);
    Rect rect;

    for (int j = 0; j < geo->numItems(); ++j) {
      switch (geo->itemType(j)) {
        case lefiGeomLayerE: {
          const string layer_name(geo->getLayer(j));
          layer_id = database.layer_id_by_name(layer_name);
          break;
        }
        case lefiGeomRectE: {
          const lefiGeomRect* geo_rect = geo->getRect(j);
          rect = Rect(Point(geo_rect->xl, geo_rect->yl),
                      Point(geo_rect->xh, geo_rect->yh));
          if (type == NetType::POWER && rect.min_corner().y() < 0.0) {
            current_cell.set_is_bottom_ground(false);
          }
          break;
        }
        default: { break; }
      }
    }

    cell_pin.add_port(CellPinPort(cell_pin_id, layer_id, rect));
  }

  return 0;
}

// Global placed def callbacks

int DesignCallback(defrCallbackType_e typ, const char* designName,
                   defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const string design_name(designName);

  database.set_design_name(design_name);

  return 0;
}

int DieAreaCallback(defrCallbackType_e typ, defiBox* dieArea,
                    defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);

  database.set_die_rect(Rect(Point(dieArea->xl(), dieArea->yl()),
                             Point(dieArea->xh(), dieArea->yh())));

  const double row_height = Site::height();
  const double site_width = Site::width();
  const int num_rows = static_cast<int>((database.die_rect().max_corner().y() -
                                         database.die_rect().min_corner().y()) /
                                        row_height);
  const int num_sites =
      num_rows * static_cast<int>((database.die_rect().max_corner().x() -
                                   database.die_rect().min_corner().x()) /
                                  site_width);
  const int num_row_sites = num_sites / num_rows;

  database.reserve_rows(num_rows);
  database.reserve_sites(num_sites);
  database.initialize_site_id_by_position(num_rows, num_row_sites);

  for (int i = 0; i < num_rows; ++i) {
    for (int j = 0; j < num_row_sites; ++j) {
      const Point& site_position =
          Point(database.die_rect().min_corner().x() + j * site_width,
                database.die_rect().min_corner().y() + i * row_height);
      const SiteId site_id = database.add_site(Site(site_position));

      database.index_site_by_position(site_id, site_position);
    }
  }

  return 0;
}

int DefUnitCallback(defrCallbackType_e typ, double unit,
                    defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);

  database.set_def_unit(unit);
  database.ConvertLefMicronsToDefUnits();

  return 0;
}

int RowCallback(defrCallbackType_e typ, defiRow* row,
                defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const int num_layers = database.num_layers();
  const string name(row->name());
  const double x = row->x();
  const double y = row->y();
  const Orientation orientation = static_cast<Orientation>(row->orient());

  /* const RowId row_id = */
  /*     database.add_row(Row(name, Point(x, y), orientation, num_layers)); */
  /* database.index_row_by_name(row_id, name); */

  database.add_row(Row(name, Point(x, y), orientation, num_layers));

  return 0;
}

int RegionCallback(defrCallbackType_e typ, defiRegion* reg,
                   defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);

  if (reg->hasType()) {
    const string type(reg->type());

    if (type == "FENCE") {
      const string name(reg->name());

      const FenceRegionId fence_region_id =
          database.add_fence_region(FenceRegion(name));

      database.index_fence_region_by_name(fence_region_id, name);

      FenceRegion& fence_region = database.fence_region(fence_region_id);

      for (int i = 0; i < reg->numRectangles(); ++i) {
        const Rect rect(Point(max(database.die_rect().min_corner().x(),
                                  static_cast<double>(reg->xl(i))),
                              max(database.die_rect().min_corner().y(),
                                  static_cast<double>(reg->yl(i)))),
                        Point(min(database.die_rect().max_corner().x(),
                                  static_cast<double>(reg->xh(i))),
                              min(database.die_rect().max_corner().y(),
                                  static_cast<double>(reg->yh(i)))));
        fence_region.add_rect(rect);
      }

      // TODO: Union rects of fence region if needed.
      /* fence_region.UnionRects(); */
    }
  }

  return 0;
}

int SpecialNetCallback(defrCallbackType_e typ, defiNet* net,
                       defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);

  if (net->hasUse()) {
    const string name(net->name());
    const string type(net->use());

    for (int i = 0; i < net->numWires(); ++i) {
      const defiWire* wire = net->wire(i);
      for (int j = 0; j < wire->numPaths(); ++j) {
        const defiPath* path = wire->path(j);
        path->initTraverse();
        LayerId current_layer_id(UNDEFINED_ID);
        int current_width = 0;
        vector<Point> current_points;
        int p = path->next();
        while (p != DEFIPATH_DONE) {
          switch (p) {
            case DEFIPATH_LAYER: {
              current_points.clear();
              const string layer_name(path->getLayer());
              current_layer_id = database.layer_id_by_name(layer_name);
              break;
            }
            case DEFIPATH_WIDTH: {
              current_width = path->getWidth();
              break;
            }
            case DEFIPATH_POINT: {
              int x, y;
              path->getPoint(&x, &y);
              current_points.push_back(Point(x, y));
              if (current_points.size() == 2) {
                Layer& current_layer = database.layer(current_layer_id);
                if (type == "GROUND") {
                  const RailId rail_id = database.add_rail(
                      Rail(NetType::GROUND, current_layer_id,
                           Line(current_points[0], current_points[1]),
                           current_width));
                  current_layer.add_ground_rail_id(rail_id);
                } else {
                  const RailId rail_id = database.add_rail(
                      Rail(NetType::POWER, current_layer_id,
                           Line(current_points[0], current_points[1]),
                           current_width));
                  current_layer.add_power_rail_id(rail_id);
                }
              }
              break;
            }
            default: { break; }
          }
          p = path->next();
        }
      }
    }
  }

  return 0;
}

int SpecialNetEndCallback(defrCallbackType_e typ, void*,
                          defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);

  database.SortRowsByY();

  database.AssignRailsToRows();

  return 0;
}

int ComponentBeginCallback(defrCallbackType_e typ, int numComponents,
                           defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const int num_instances = numComponents;

  database.reserve_instances(num_instances);
  database.reserve_instance_pins(num_instances * 5);  // Just an estimation.
  database.reserve_sub_instances(num_instances * 2);  // Just an estimation.

  return 0;
}

int ComponentCallback(defrCallbackType_e typ, defiComponent* comp,
                      defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const double row_height = Site::height();

  if (comp->isPlaced() || comp->isFixed()) {
    const string cell_name(comp->name());
    const CellId cell_id(database.cell_id_by_name(cell_name));
    const Cell& cell = database.cell(cell_id);
    const string name(comp->id());
    const bool is_fixed = comp->isFixed();
    const double x = comp->placementX();
    const double y = comp->placementY();
    const double width = cell.width();
    const double height = cell.height();
    const Orientation orientation =
        static_cast<Orientation>(comp->placementOrient());

    const InstanceId instance_id = database.add_instance(Instance(
        cell_id, name, is_fixed, Point(x, y), width, height, orientation));
    database.index_instance_by_name(instance_id, name);

    Instance& instance = database.instance(instance_id);

    if (is_fixed) {
      database.add_fixed_instance_id(instance_id);
    } else {
      const int h = static_cast<int>(height / row_height);

      database.add_instance_id_of_row_height(h, instance_id);

      for (int i = 0; i < h; ++i) {
        SubInstanceId sub_instance_id = database.add_sub_instance(
            SubInstance(instance_id, width, Point(x, y + i * row_height)));
        instance.add_sub_instance_id(sub_instance_id);
      }
    }

    // TODO: Consider orientation S and FS.
    // Assume initial orientations of all instances are N or FN.
    instance.set_edge_types(cell.left_edge_type(), cell.right_edge_type());
    instance.set_is_bottom_ground(cell.is_bottom_ground());

    // Construct instance pins.
    for (int i = 0; i < cell.num_pins(); ++i) {
      const CellPinId cell_pin_id = cell.pin_id(i);
      const CellPin& cell_pin = database.cell_pin(cell_pin_id);
      const string cell_pin_name(cell_pin.name());
      const string instance_pin_name(
          name + cell_pin_name.substr(cell_pin_name.rfind('.')));

      const InstancePinId instance_pin_id = database.add_instance_pin(
          InstancePin(instance_id, cell_pin_id, instance_pin_name));
      database.index_instance_pin_by_name(instance_pin_id, instance_pin_name);

      instance.add_pin_id(instance_pin_id);

      if (cell_pin_id == cell.ground_pin_id()) {
        instance.set_ground_pin_id(instance_pin_id);
      } else if (cell_pin_id == cell.power_pin_id()) {
        instance.set_power_pin_id(instance_pin_id);
      }

      InstancePin& instance_pin = database.instance_pin(instance_pin_id);
      for (int j = 0; j < cell_pin.num_ports(); ++j) {
        const CellPinPort& cell_pin_port = cell_pin.port(j);

        // TODO: Just construct zero rect here, then update it later.
        // Assume initial orientations of all instances are N, and origins of
        // all cells are (0, 0).
        instance_pin.add_port(InstancePinPort(
            instance_pin_id,
            Rect(Point(x + cell_pin_port.rect().min_corner().x(),
                       y + cell_pin_port.rect().min_corner().y()),
                 Point(x + cell_pin_port.rect().max_corner().x(),
                       y + cell_pin_port.rect().max_corner().y()))));
      }
    }
  }

  return 0;
}

int ComponentEndCallback(defrCallbackType_e typ, void*,
                         defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);

  database.SplitRowsIntoIntervals();

  const double site_width = Site::width();

  for (int i = 0; i < database.num_rows(); ++i) {
    const RowId row_id(i);
    Row& row = database.row(row_id);

    row.SortIntervalsByBegin();
  }

  for (int i = 0; i < database.num_rows(); ++i) {
    const RowId row_id(i);
    const Row& row = database.row(row_id);
    const double site_y = row.position().y();

    for (int j = 0; j < row.num_intervals(); ++j) {
      const IntervalId current_interval_id = row.interval_id(j);
      const Interval& current_interval = database.interval(current_interval_id);

      for (int k = static_cast<int>((current_interval.begin() -
                                     database.die_rect().min_corner().x()) /
                                    site_width);
           k < static_cast<int>((current_interval.end() -
                                 database.die_rect().min_corner().x()) /
                                site_width);
           ++k) {
        const double site_x =
            database.die_rect().min_corner().x() + k * site_width;
        const SiteId site_id =
            database.site_id_by_position(Point(site_x, site_y));
        Site& site = database.site(site_id);

        if (site_x >= current_interval.begin() &&
            site_x + site_width <= current_interval.end()) {
          site.set_is_valid(true);
          site.set_fence_region_id(current_interval.fence_region_id());
        } else {
          site.set_is_valid(false);
        }
      }
    }
  }

  return 0;
}

int PinBeginCallback(defrCallbackType_e typ, int numPins,
                     defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const int num_pins = numPins;

  database.reserve_pins(num_pins);

  return 0;
}

int PinCallback(defrCallbackType_e typ, defiPin* pin,
                defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const string name(pin->pinName());
  const string net_name(pin->netName());
  const PinType type = [&]() {
    if (pin->hasDirection()) {
      const string direction(pin->direction());
      if (direction == "INPUT") {
        return PinType::INPUT;
      }
      if (direction == "OUTPUT") {
        return PinType::OUTPUT;
      }
      if (direction == "INOUT") {
        return PinType::INOUT;
      }
    }
    return PinType::NONE;
  }();
  const Point position(pin->placementX(), pin->placementY());
  const Orientation orientation = static_cast<Orientation>(pin->orient());

  const NetId net_id = database.add_net(Net(net_name));
  database.index_net_by_name(net_id, net_name);
  const PinId pin_id =
      database.add_pin(Pin(net_id, name, type, position, orientation));
  database.index_pin_by_name(pin_id, name);

  Pin& p = database.pin(pin_id);

  for (int i = 0; i < pin->numLayer(); ++i) {
    const string layer_name(pin->layer(i));
    const LayerId layer_id = database.layer_id_by_name(layer_name);
    int lower_left_x, lower_left_y, upper_right_x, upper_right_y;
    pin->bounds(i, &lower_left_x, &lower_left_y, &upper_right_x,
                &upper_right_y);
    p.add_port(Port(layer_id, Rect(Point(lower_left_x, lower_left_y),
                                   Point(upper_right_x, upper_right_y))));
  }

  for (int i = 0; i < pin->numPorts(); ++i) {
    const defiPinPort* port = pin->pinPort(i);
    for (int j = 0; j < port->numLayer(); ++j) {
      const string layer_name(port->layer(j));
      const LayerId layer_id = database.layer_id_by_name(layer_name);
      int lower_left_x, lower_left_y, upper_right_x, upper_right_y;
      port->bounds(j, &lower_left_x, &lower_left_y, &upper_right_x,
                   &upper_right_y);
      p.add_port(Port(layer_id, Rect(Point(lower_left_x, lower_left_y),
                                     Point(upper_right_x, upper_right_y))));
    }
  }

  return 0;
}

int BlockageCallback(defrCallbackType_e typ, defiBlockage* blockage,
                     defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);

  if (blockage->hasLayer()) {
    const string layer_name(blockage->layerName());
    const LayerId layer_id = database.layer_id_by_name(layer_name);
    const BlockageId blockage_id = database.add_blockage(Blockage(layer_id));

    Blockage& b = database.blockage(blockage_id);
    for (int i = 0; i < blockage->numRectangles(); ++i) {
      int lower_left_x = blockage->xl(i);
      int lower_left_y = blockage->yl(i);
      int upper_right_x = blockage->xh(i);
      int upper_right_y = blockage->yh(i);
      b.add_rect(Rect(Point(lower_left_x, lower_left_y),
                      Point(upper_right_x, upper_right_y)));
    }
  }

  return 0;
}

int NetBeginCallback(defrCallbackType_e typ, int numNets,
                     defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const int num_nets = numNets;

  database.reserve_nets(num_nets);

  return 0;
}

int NetCallback(defrCallbackType_e typ, defiNet* net,
                defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const string name(net->name());

  const NetId net_id = [&]() {
    if (database.net_id_by_name(name) != UNDEFINED_ID) {
      return database.net_id_by_name(name);
    }

    const NetId id = database.add_net(Net(name));
    database.index_net_by_name(id, name);
    return id;
  }();

  Net& n = database.net(net_id);
  for (int i = 0; i < net->numConnections(); ++i) {
    const string instance_name(net->instance(i));
    const string pin_name(net->pin(i));

    if (instance_name == "PIN") {
      const PinId pin_id = database.pin_id_by_name(pin_name);
      n.add_pin_id(pin_id);
    } else {
      const InstancePinId instance_pin_id =
          database.instance_pin_id_by_name(instance_name + "." + pin_name);
      n.add_instance_pin_id(instance_pin_id);
    }
  }

  return 0;
}

int GroupNameCallback(defrCallbackType_e typ, const char* groupName,
                      defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const string name(groupName);

  const GroupId group_id = database.add_group(Group(name));
  database.index_group_by_name(group_id, name);

  return 0;
}

int GroupMemberCallback(defrCallbackType_e typ, const char* groupMember,
                        defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const GroupId current_group_id(database.num_groups() - 1);
  Group& current_group = database.group(current_group_id);
  const string pattern(groupMember);

  current_group.add_pattern(pattern);
  database.index_group_by_pattern(current_group_id, pattern);

  return 0;
}

int GroupCallback(defrCallbackType_e typ, defiGroup* group,
                  defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);
  const GroupId current_group_id(database.num_groups() - 1);
  Group& current_group = database.group(current_group_id);
  const string fence_region_name(group->regionName());
  const FenceRegionId fence_region_id =
      database.fence_region_id_by_name(fence_region_name);

  current_group.set_fence_region_id(fence_region_id);

  return 0;
}

int GroupEndCallback(defrCallbackType_e typ, void*, defiUserData database_ptr) {
  Database& database = *static_cast<Database*>(database_ptr);

  database.AssignFenceRegionsToInstances();

  return 0;
}

// Private members

void Parser::ParseTechLef(const string& tech_lef_name) {
  lefrInit();

  lefrSetUnitsCbk(LefUnitCallback);
  lefrSetPropCbk(PropertyCallback);
  lefrSetLayerCbk(LayerCallback);
  lefrSetSiteCbk(SiteCallback);

  FILE* lef = fopen(tech_lef_name.c_str(), "r");
  lefrRead(lef, tech_lef_name.c_str(), static_cast<void*>(&database_));
  fclose(lef);
}

void Parser::ParseCellLef(const string& cell_lef_name) {
  lefrInit();

  lefrSetMacroBeginCbk(MacroBeginCallback);
  lefrSetMacroCbk(MacroCallback);
  lefrSetPinCbk(MacroPinCallback);

  FILE* lef = fopen(cell_lef_name.c_str(), "r");
  lefrRead(lef, cell_lef_name.c_str(), static_cast<void*>(&database_));
  fclose(lef);
}

void Parser::ParseGlobalPlacedDef(const string& global_placed_def_name) {
  defrInit();

  defrSetDesignCbk(DesignCallback);
  defrSetDieAreaCbk(DieAreaCallback);
  defrSetUnitsCbk(DefUnitCallback);
  defrSetRowCbk(RowCallback);
  defrSetRegionCbk(RegionCallback);
  defrSetSNetCbk(SpecialNetCallback);
  defrSetSNetEndCbk(SpecialNetEndCallback);
  defrSetComponentStartCbk(ComponentBeginCallback);
  defrSetComponentCbk(ComponentCallback);
  defrSetComponentEndCbk(ComponentEndCallback);
  defrSetStartPinsCbk(PinBeginCallback);
  defrSetPinCbk(PinCallback);
  defrSetBlockageCbk(BlockageCallback);
  defrSetNetStartCbk(NetBeginCallback);
  defrSetNetCbk(NetCallback);
  defrSetGroupNameCbk(GroupNameCallback);
  defrSetGroupMemberCbk(GroupMemberCallback);
  defrSetGroupCbk(GroupCallback);
  defrSetGroupsEndCbk(GroupEndCallback);

  // Must be called to parse nets.
  defrSetAddPathToNet();

  FILE* def = fopen(global_placed_def_name.c_str(), "r");
  defrRead(def, global_placed_def_name.c_str(), static_cast<void*>(&database_),
           1);
  fclose(def);
}

void Parser::ParsePlacementConstraints(
    const string& placement_constraints_name) {
  const double row_height = Site::height();

  ifstream in(placement_constraints_name.c_str());

  simple_parser::Parser parser(in, "=");
  parser.Parse([&](const simple_parser::Tokens& tokens) -> bool {
    if (!tokens.empty()) {
      string first_word = tokens[0];
      string second_word = tokens[1];

      if (first_word == "maximum_utilization") {
        second_word.pop_back();

        database_.set_density_target(stod(second_word) / 100.0);
      }

      if (first_word == "maximum_movement") {
        second_word.pop_back();
        second_word.pop_back();
        second_word.pop_back();
        second_word.pop_back();

        database_.set_displacement_limit(stod(second_word) * row_height);
      }
    }

    return true;
  });
}

void Parser::OutputDef(const string& global_placed_def_name,
                       const string& output_def_name) {
  ifstream def;
  def.open(global_placed_def_name.c_str(), ios::in);

  ofstream out;
  out.open(output_def_name.c_str(), ios::out);

  string line;
  bool component = false;
  int k = 0;

  while (getline(def, line)) {
    if (component == false) {
      out << line << endl;
      //calculator
      if (line.substr(0, 10) == "COMPONENTS") {
        component = true;
      }
    } else {
      out << line << endl;
      if (line == "END COMPONENTS") {
        break;
      }
      getline(def, line);
      const InstanceId instance_id(k);
      const Instance& instance = database_.instance(instance_id);
      if (line.substr(8, 6) == "PLACED") {
        out << "      + PLACED ( " << instance.position().x() << " "
            << instance.position().y() << " "
            << ") " << instance.orientation() << " ;" << endl;
      } else {
        out << line << endl;
      }
      k++;
    }
  }
  while (getline(def, line)) {
    out << line << endl;
  }
}
