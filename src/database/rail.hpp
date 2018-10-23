#ifndef RAIL_HPP
#define RAIL_HPP

#include "../util/type.hpp"
#include "line.hpp"

class Rail {
 public:
  Rail();
  Rail(NetType type, LayerId layer_id, const Line& middle_line, double width);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  NetType type() const;
  LayerId layer_id() const;
  const Line& middle_line() const;
  double width() const;

 private:
  NetType type_;
  LayerId layer_id_;
  Line middle_line_;
  double width_;
};

#endif
