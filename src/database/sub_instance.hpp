#ifndef SUB_INSTANCE_HPP
#define SUB_INSTANCE_HPP

#include "../util/type.hpp"
#include "point.hpp"
#include "variable.hpp"

class SubInstance {
 public:
  SubInstance();
  SubInstance(InstanceId instance_id, double width, const Point& position);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  InstanceId instance_id() const;
  double width() const;
  const Point& position() const;
  IntervalId interval_id() const;
  bool lefttop() const;
  bool leftbottom() const;
  bool righttop() const;
  bool rightbottom() const;
  SubInstanceId right_subinstance();
  SubInstanceId left_subinstance();
  double oxideheight();
  
  VariableId variable_id(int idx) const;
  int num_variables() const;
  // Setters

  void set_position(const Point& position);
  void set_interval_id(IntervalId id);
  //source->true    drain->false
  void set_gates_left(bool lefttop,bool leftbottom);
  void set_gates_right(bool righttop,bool rightbottom);
  void set_right_subinstance(SubInstanceId subinstance_id);
  void set_left_subinstance(SubInstanceId subinstance_id);
  
  void set_oxideheight(double oxideheight);
  bool dda_forbidden = false ;
  bool od_forbidden = false ;
  void flipped();
  
  void add_variable_id(VariableId id);
  
 private:
  InstanceId instance_id_;
  double width_;
  Point position_;
  IntervalId interval_id_;
  bool lefttop_;
  bool leftbottom_;
  bool righttop_;
  bool rightbottom_;
  SubInstanceId right_subinstance_id_;
  SubInstanceId left_subinstance_id_;
  double oxideheight_;
  
  std::vector<VariableId>  variable_ids_;
};

#endif
