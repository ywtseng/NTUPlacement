#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include "../util/type.hpp"
#include "rect.hpp"
#include "site.hpp"
#include "sub_instance.hpp"
#include "point.hpp"
#include <vector>

class Variable {
	public:
		Variable();
		Variable(SubInstanceId subinstance_id,bool flipped,Point point);
		Variable(InstanceId instance_id,bool flipped,Point point);
		
		Variable(InstanceId instance_id,std::vector<SiteId> site_id,Point point,int num_width,int num_height,bool flipped);
		//Getter
		int num_width();
		int num_height();
		int num_sites();
		
		
		SiteId site_id(int idx) const;
		
		Point point();
		bool flipped();
		SubInstanceId subinstance_id();
		InstanceId instance_id();
		
		bool selected = false;
		
	private:
		int num_width_;
		int num_height_;
		
		
		SubInstanceId subinstance_id_;
		InstanceId instance_id_;
		Point point_;
		bool flipped_;
		
		std::vector<SiteId> site_id_;
		//std::vector< std::pair<SiteId,bool> > left_node_;
		//std::vector< std::pair<SiteId,bool> > right_node_;
};

#endif
