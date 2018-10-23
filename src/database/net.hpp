#ifndef NET_HPP
#define NET_HPP

#include "../util/type.hpp"

#include <string>
#include <vector>

class Net {
 public:
  Net();
  Net(const std::string& name);

  void Print(std::ostream& os = std::cout, int indent_level = 0) const;

  // Getters

  const std::string& name() const;

  int num_pins() const;
  int num_instance_pins() const;

  PinId pin_id(int idx) const;
  InstancePinId instance_pin_id(int idx) const;

  // Setters

  void add_pin_id(PinId id);
  void add_instance_pin_id(InstancePinId id);

 private:
  std::string name_;
  std::vector<PinId> pin_ids_;
  std::vector<InstancePinId> instance_pin_ids_;
};

#endif
