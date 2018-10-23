#include "const.hpp"
#include "plotter.hpp"

using namespace std;

Plotter::Plotter()
    : title_(UNDEFINED_STRING),
      line_styles_(),
      lines_by_style_id_(),
      arrow_styles_(),
      arrows_by_style_id_() {
}

Plotter::Plotter(const string& title)
    : title_(title),
      line_styles_(),
      lines_by_style_id_(),
      arrow_styles_(),
      arrows_by_style_id_() {
}

void Plotter::Plot(ostream& os) const {
  os << "set title '" << title_ << "'" << endl;
  os << endl;

  for (int i = 0; i < line_styles_.size(); ++i) {
    os << "set style line " << i + 1 << " " << line_styles_[i] << endl;
  }
  os << endl;

  for (int i = 0; i < arrow_styles_.size(); ++i) {
    os << "set style arrow " << i + 1 << " " << arrow_styles_[i] << endl;
  }
  os << endl;

  os << "set size ratio -1" << endl;
  os << "set clip two" << endl;
  os << "unset key" << endl;
  os << endl;

  os << "plot ";
  for (int i = 0; i < lines_by_style_id_.size(); ++i) {
    os << "'-' w l ls " << i + 1 << ", ";
  }
  for (int i = 0; i < arrows_by_style_id_.size(); ++i) {
    os << "'-' w vectors arrowstyle " << i + 1 << ", ";
  }
  os << endl;

  for (int i = 0; i < lines_by_style_id_.size(); ++i) {
    os << endl;
    for (const Line& line : lines_by_style_id_[i]) {
      os << line.first.x() << " " << line.first.y() << endl;
      os << line.second.x() << " " << line.second.y() << endl;
      os << endl;
    }
    os << "e" << endl;
    os << endl;
  }
  os << endl;

  for (int i = 0; i < arrows_by_style_id_.size(); ++i) {
    os << endl;
    for (const Line& arrow : arrows_by_style_id_[i]) {
      os << arrow.first.x() << " " << arrow.first.y() << " "
         << arrow.second.x() - arrow.first.x() << " "
         << arrow.second.y() - arrow.first.y() << endl;
    }
    os << "e" << endl;
    os << endl;
  }
  os << endl;

  os << "pause -1 'Press enter to exit.'" << endl;
}

// Setters

LineStyleId Plotter::add_line_style(const string& style) {
  const LineStyleId line_style_id(line_styles_.size());

  line_styles_.push_back(style);
  lines_by_style_id_.push_back(vector<Line>());

  return line_style_id;
}

ArrowStyleId Plotter::add_arrow_style(const string& style) {
  const ArrowStyleId arrow_style_id(arrow_styles_.size());

  arrow_styles_.push_back(style);
  arrows_by_style_id_.push_back(vector<Line>());

  return arrow_style_id;
}

void Plotter::add_line(const Line& line, LineStyleId line_style_id) {
  lines_by_style_id_.at(line_style_id).push_back(line);
}

void Plotter::add_rect(const Rect& rect, LineStyleId line_style_id) {
  lines_by_style_id_.at(line_style_id)
      .push_back(Line(Point(rect.min_corner().x(), rect.min_corner().y()),
                      Point(rect.min_corner().x(), rect.max_corner().y())));
  lines_by_style_id_.at(line_style_id)
      .push_back(Line(Point(rect.min_corner().x(), rect.max_corner().y()),
                      Point(rect.max_corner().x(), rect.max_corner().y())));
  lines_by_style_id_.at(line_style_id)
      .push_back(Line(Point(rect.max_corner().x(), rect.max_corner().y()),
                      Point(rect.max_corner().x(), rect.min_corner().y())));
  lines_by_style_id_.at(line_style_id)
      .push_back(Line(Point(rect.max_corner().x(), rect.min_corner().y()),
                      Point(rect.min_corner().x(), rect.min_corner().y())));
}

void Plotter::add_polygon(const Polygon& polygon, LineStyleId line_style_id) {
  bool is_first_point = true;
  Point last_point;

  for (const Point& point : polygon.outer()) {
    if (!is_first_point) {
      lines_by_style_id_.at(line_style_id).push_back(Line(last_point, point));
    }

    last_point = point;

    if (is_first_point) {
      is_first_point = false;
    }
  }

  for (const auto& ring : polygon.inners()) {
    is_first_point = true;

    for (const Point& point : ring) {
      if (!is_first_point) {
        lines_by_style_id_.at(line_style_id).push_back(Line(last_point, point));
      }

      last_point = point;

      if (is_first_point) {
        is_first_point = false;
      }
    }
  }
}

void Plotter::add_arrow(const Line& arrow, ArrowStyleId arrow_style_id) {
  arrows_by_style_id_.at(arrow_style_id).push_back(arrow);
}
