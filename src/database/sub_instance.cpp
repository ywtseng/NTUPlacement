#include "sub_instance.hpp"

#include "../util/const.hpp"

SubInstance::SubInstance()
    : instance_id_(UNDEFINED_ID),
      width_(0.0),
      position_(ORIGIN_POINT),
      interval_id_(UNDEFINED_ID) {
}

SubInstance::SubInstance(InstanceId instance_id, double width,
                         const Point& position)
    : instance_id_(instance_id),
      width_(width),
      position_(position),
      interval_id_(UNDEFINED_ID) {
}

// Getters

int SubInstance::num_variables() const {
	return variable_ids_.size();
}

InstanceId SubInstance::instance_id() const {
  return instance_id_;
}

double SubInstance::width() const {
  return width_;
}

const Point& SubInstance::position() const {
  return position_;
}

IntervalId SubInstance::interval_id() const {
  return interval_id_;
}
bool SubInstance::lefttop() const {
  return lefttop_;
}

bool SubInstance::leftbottom() const {
  return leftbottom_;
}

bool SubInstance::righttop() const {
  return righttop_;
}

bool SubInstance::rightbottom() const {
  return rightbottom_;
}

SubInstanceId SubInstance::right_subinstance(){
  return right_subinstance_id_;
}

SubInstanceId SubInstance::left_subinstance(){
  return left_subinstance_id_;
}

VariableId SubInstance::variable_id(int idx) const {
  return variable_ids_.at(idx);
}

double SubInstance::oxideheight(){
  return oxideheight_;
}
// Setters

void SubInstance::set_position(const Point& position) {
  position_ = position;
}

void SubInstance::set_interval_id(IntervalId id) {
  interval_id_ = id;
}

void SubInstance::set_gates_left(bool lefttop,bool leftbottom){
	lefttop_ = lefttop;
	leftbottom_ = leftbottom;
}
void SubInstance::set_gates_right(bool righttop,bool rightbottom){
	righttop_ = righttop;
	rightbottom_ = rightbottom;
}

void SubInstance::set_right_subinstance(SubInstanceId subinstance_id){
	right_subinstance_id_ = subinstance_id;
}

void SubInstance::set_left_subinstance(SubInstanceId subinstance_id){
	left_subinstance_id_ = subinstance_id;
}

void SubInstance::flipped(){
	//InstanceId instance_id =instance_id();
	//Instance instance = database_.instance(instance_id);
	bool temp_top = lefttop_;
	bool temp_bottom = leftbottom_;
	lefttop_ = righttop_;
	leftbottom_ = rightbottom_;
	righttop_ = temp_top;
	rightbottom_ = temp_bottom;
	
}

void SubInstance::set_oxideheight(double oxideheight){
	oxideheight_ = oxideheight;
}

void SubInstance::add_variable_id(VariableId id) {
  variable_ids_.push_back(id);
}