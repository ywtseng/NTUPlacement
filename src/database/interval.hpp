#ifndef INTERVAL_HPP
#define INTERVAL_HPP

#include "../util/type.hpp"

#include <map>
#include <vector>

class Interval {
 public:
  Interval();
  Interval(RowId row_id, double begin, double end,
           FenceRegionId fence_region_id);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  RowId row_id() const;
  double begin() const;
  double end() const;
  double capacity() const;
  FenceRegionId fence_region_id() const;
  SubInstanceId first_sub_instance_id() const;
  SubInstanceId last_sub_instance_id() const;

  int num_sub_instances() const;

  SubInstanceId sub_instance_id(int idx) const;

  double ComputeAccumulatedOverlap(double sub_instance_x,
                                   double sub_instance_width) const;

  // Setters

  void set_begin(double begin);
  void set_end(double end);
  void set_fence_region_id(FenceRegionId id);

  void add_sub_instance_id(SubInstanceId id, double x, double width);

  void remove_sub_instance_ids();

  // Call this function if you did not add sub instance in sorted order.
  void SortSubInstancesByX();

 private:
  RowId row_id_;
  double begin_;
  double end_;
  double capacity_;
  FenceRegionId fence_region_id_;
  std::vector<std::pair<double, SubInstanceId>>
      x_and_sub_instance_id_sorted_by_x_;
  // TODO: Explain what cluster_x_and_overlap_by_right_x_ is.
  std::map<double, std::pair<double, double>> cluster_x_and_overlap_by_right_x_;
};

#endif
