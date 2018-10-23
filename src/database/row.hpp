#ifndef ROW_HPP
#define ROW_HPP

#include "../util/type.hpp"
#include "point.hpp"

#include <iostream>
#include <map>
#include <vector>

class Row {
 public:
  Row();
  Row(const std::string& name, const Point& position, Orientation orientation,
      int num_layers);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  const std::string& name() const;
  const Point& position() const;
  Orientation orientation() const;

  int num_intervals() const;

  IntervalId interval_id(int idx) const;

  bool has_rail_on_layer(LayerId id) const;
  bool has_interval_of_fence_region(FenceRegionId id) const;
  bool has_interval_of_fence_region_before(FenceRegionId fence_region_id,
                                           double x) const;
  bool has_interval_of_fence_region_after(FenceRegionId fence_region_id,
                                          double x) const;

  RailId rail_id_on_layer(LayerId id) const;
  IntervalId interval_id_of_fence_region_before(FenceRegionId fence_region_id,
                                                double x) const;
  IntervalId interval_id_of_fence_region_after(FenceRegionId fence_region_id,
                                               double x) const;

  // Setters

  void set_rail_id_on_layer(RailId rail_id, LayerId layer_id);

  void add_interval_id(IntervalId id, double begin,
                       FenceRegionId fence_region_id);

  // Must be called before has_interval_of_fence_region and add_interval_id.
  void prepare_for_adding_interval_id(int num_fence_regions);

  void SortIntervalsByBegin();

 private:
  std::string name_;
  Point position_;
  Orientation orientation_;
  std::vector<RailId> rail_id_by_layer_id_;
  // Since we need to query the intervals with fence_region_id equals to
  // UNDEFINED_ID, which is -1, we shift every fence_region_id by 1.
  std::vector<std::map<double, IntervalId>>
      interval_id_by_begin_by_fence_region_id_;
  std::vector<std::pair<double, IntervalId>>
      begin_and_interval_id_sorted_by_begin_;
};

#endif
