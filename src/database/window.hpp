#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "../util/type.hpp"
#include "rect.hpp"
#include "site.hpp"
#include "sub_instance.hpp"
#include "point.hpp"
#include <vector>

class Window {
	public:
		Window();
		Window(const Point& position_top,const Point& position_down,std::vector<SiteId> windowspace,std::vector<SubInstanceId> subinstance);
		//Getter
		int num_site();
		int num_subinstance();
		double density();
		double width();
		double height();
		
		//Setter
		void add_subinstance(SubInstanceId id);
		void add_site(SiteId id);
	private:
		int num_site_;
		int num_subinstance_;
		double width_;
		double height_;
		
		Point position_top_;
		Point position_down_;
		std::vector<SiteId> site_;
		std::vector<SubInstanceId> subinstance_;
	
};


#endif