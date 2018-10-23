#ifndef BLOCKAGE_HPP
#define BLOCKAGE_HPP

#include "../util/type.hpp"
#include "rect.hpp"

#include <vector>

class Blockage {
 public:
  Blockage();
  Blockage(LayerId layer_id);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  LayerId layer_id() const;

  int num_rects() const;

  const Rect& rect(int idx) const;

  // Setters

  Rect& rect(int idx);

  void add_rect(const Rect& rect);

 private:
  LayerId layer_id_;
  std::vector<Rect> rects_;
};

#endif
