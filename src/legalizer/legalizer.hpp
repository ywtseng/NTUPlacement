#ifndef LEGALIZER_HPP
#define LEGALIZER_HPP

#include "../database/database.hpp"

class Legalizer {
 public:
  Legalizer(Database& database);

  void Legalize();

 private:
  void PreMmsim();
  void Mmsim();
  void PostMmsim();
  //add DDA constraint
  void PreDDA();

  void SpreadInstances();
  void AlignInstancesToRows();
  void AlignInstancesToSites();
  void AllocateIllegalInstances();
  void ResolveEdgeSpacingConstraint();

  void SortInstancesByX();
  void SortInstancesByRowHeightByX();
  bool IsResultLegal();
  void ClearInstanceSites(InstanceId instance_id);
  void FillInstanceSites(InstanceId instance_id);
  std::vector<InstanceId> FindAdjacentInstances(InstanceId instance_id,
                                                bool is_left,
                                                double adjacent_threshold);
  bool TryPlaceInstance(InstanceId root_instance_id, double root_instance_new_x,
                        double displacement_limit);

  Database& database_;
  std::vector<std::pair<double, InstanceId>> x_and_instance_id_sorted_by_x_;
  std::vector<std::vector<std::pair<double, InstanceId>>>
      x_and_instance_id_sorted_by_x_by_row_height_;
  std::vector<std::vector<InstanceId>> illegal_instance_ids_by_row_height_;
};

#endif
