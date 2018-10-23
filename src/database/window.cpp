#include "window.hpp"

#include "../util/const.hpp"

using namespace std;

Window::Window(){}
Window::Window(
	const Point& position_top,
	const Point& position_down,
	std::vector<SiteId> windowspace,
	std::vector<SubInstanceId> subinstance)
	:position_top_(position_top),
	position_down_(position_down),
	site_(windowspace),
	subinstance_(subinstance)
	{
		num_site_ = site_.size();
		num_subinstance_ = subinstance_.size();
		width_ = position_top_.x()-position_down_.x();
		height_ = position_top_.y()-position_down_.y();
	}		


void Window::add_subinstance(SubInstanceId id){
	subinstance_.push_back(id);
	num_subinstance_ = subinstance_.size();
}

void Window::add_site(SiteId id){
	site_.push_back(id);
	num_site_ = site_.size();
}

int Window::num_site(){
	return num_site_;
}

int Window::num_subinstance(){
	return num_subinstance_;
}

double Window::density(){
	double area_ = width_*height_;
	double capacity_ = Site::width()*Site::height()*num_site_;
    double density_ = capacity_/area_;
	return density_;
}

double Window::width(){
	return width_;
}

double Window::height(){
	return height_;
}
