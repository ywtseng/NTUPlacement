#ifndef GROUP_HPP
#define GROUP_HPP

#include "../util/type.hpp"

#include <iostream>
#include <string>
#include <vector>

class Group {
 public:
  Group();
  Group(const std::string& name);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  const std::string& name() const;
  FenceRegionId fence_region_id() const;

  int num_patterns() const;

  const std::string& pattern(int idx) const;

  // Setters

  void set_fence_region_id(FenceRegionId id);

  void add_pattern(const std::string& pattern);

 private:
  std::string name_;
  FenceRegionId fence_region_id_;
  std::vector<std::string> patterns_;
};

#endif
