#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include "../database/line.hpp"
#include "../database/polygon.hpp"
#include "../database/rect.hpp"
#include "../util/type.hpp"

#include <string>
#include <vector>

class Plotter {
 public:
  Plotter();
  Plotter(const std::string& title);

  void Plot(std::ostream& os) const;

  // Setters

  LineStyleId add_line_style(const std::string& style);
  ArrowStyleId add_arrow_style(const std::string& style);

  void add_line(const Line& line, LineStyleId line_style_id);
  void add_rect(const Rect& rect, LineStyleId line_style_id);
  void add_polygon(const Polygon& polygon, LineStyleId line_style_id);
  void add_arrow(const Line& line, ArrowStyleId arrow_style_id);

 private:
  std::string title_;
  std::vector<std::string> line_styles_;
  std::vector<std::vector<Line>> lines_by_style_id_;
  std::vector<std::string> arrow_styles_;
  std::vector<std::vector<Line>> arrows_by_style_id_;
};

#endif
