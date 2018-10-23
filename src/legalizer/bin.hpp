#ifndef BIN_HPP
#define BIN_HPP

#include "../database/rect.hpp"
#include "../util/type.hpp"

#include <vector>

// Every bin is a square-shape area.

class Bin {
 public:
  Bin();
  Bin(const Point& position, double width);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  const Point& position() const;
  double width() const;
  double height() const;
  const Point upper_right_position() const;
  const Rect rect() const;
  double free_area() const;
  double instances_area() const;
  double density() const;
  bool is_spreaded() const;

  int num_instances() const;

  InstanceId instance_id(int idx) const;

  // Setters

  void set_free_area(double free_area);
  void set_is_spreaded(bool is_spreaded);

  void add_instance_id(InstanceId instance_id, bool is_instance_fixed,
                       double instance_area);

  void remove_instance_ids();

 private:
  Point position_;
  double width_;
  std::vector<InstanceId> instance_ids_;
  double free_area_;
  double instances_area_;
  bool is_spreaded_;
};

#endif
