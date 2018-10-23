#include "variable.hpp"

#include "../util/const.hpp"

using namespace std;

Variable::Variable(){}
Variable::Variable(
	InstanceId instance_id,
	std::vector<SiteId> site_id,
	Point point,
	int num_width,
	int num_height,
	bool flipped)
	:instance_id_(instance_id),
	site_id_(site_id),
	point_(point),
	num_width_(num_width),
	num_height_(num_height),
	flipped_(flipped)
	{
		
	}

Variable::Variable(
	SubInstanceId subinstance_id,
	bool flipped,
	Point point)
	:subinstance_id_(subinstance_id),
	point_(point),
	flipped_(flipped)
	{
		
	}

Variable::Variable(
	InstanceId instance_id,
	bool flipped,
	Point point)
	:instance_id_(instance_id),
	point_(point),
	flipped_(flipped)
	{
		
	}
	
int Variable::num_width(){
	return num_width_;
}

int Variable::num_height(){
	return num_height_;
}

int Variable::num_sites(){
	return site_id_.size();
}

Point Variable::point(){
	return point_;
}

InstanceId Variable::instance_id(){
	return instance_id_;
}

SubInstanceId Variable::subinstance_id(){
	return subinstance_id_;
}


SiteId Variable::site_id(int idx) const {
  return site_id_.at(idx);
}

bool Variable::flipped(){
	return flipped_;
}