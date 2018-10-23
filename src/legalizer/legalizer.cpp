#include "legalizer.hpp"

#include "../util/const.hpp"
#include "sparse_matrix.hpp"
#include "vector.hpp"

#include <Eigen/SparseLU>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <set>
#include <stack>

using namespace std;

using Triplet = Eigen::Triplet<double>;

// Public members

Legalizer::Legalizer(Database& database)
    : database_(database),
      x_and_instance_id_sorted_by_x_(),
      x_and_instance_id_sorted_by_x_by_row_height_(
          database_.max_instance_row_height(),
          vector<pair<double, InstanceId>>()),
      illegal_instance_ids_by_row_height_(database_.max_instance_row_height(),
                                          vector<InstanceId>()) {
  x_and_instance_id_sorted_by_x_.reserve(database_.num_instances());
}

void Legalizer::Legalize() {
  PreMmsim();
  Mmsim();
  PostMmsim();
  PreDDA();
}

// Private members

void Legalizer::PreMmsim() {
  /* cout << "Spread instances..." << endl; */

  /* SpreadInstances(); */

  /* ofstream plot_sp(database_.design_name() + "_sp.plt"); */
  /* database_.Plot( */
  /*     plot_sp, */
  /*     "Spread Result: " + to_string(database_.ComputeTotalDisplacement())); */

  cout << "Align instances to rows..." << endl;

  AlignInstancesToRows();

  /* ofstream plot_al(database_.design_name() + "_al.plt"); */
  /* database_.Plot( */
  /*     plot_al, */
  /*     "Align Result: " + to_string(database_.ComputeTotalDisplacement())); */
}

void Legalizer::Mmsim() {
  //cout << "MMSIM..." << endl;

  const int num_rows = database_.num_rows();
  const double site_width = Site::width();
  const int num_instances = database_.num_instances();
  const int num_sub_instances = database_.num_sub_instances();

  vector<Triplet> non_zero_elements;
  int row_idx = 0;
  int col_idx = 0;
  vector<double> v;
  int dummy_variable_idx = 0;

  // Construct cell ordering constraint matrix.

  non_zero_elements.clear();
  row_idx = 0;
  col_idx = 0;
  dummy_variable_idx = 0;
  v.clear();

  vector<double> interval_end_sub_instance_xs;

  for (int i = 0; i < num_rows; ++i) {
    const RowId row_id(i);
    const Row& row = database_.row(row_id);

    SubInstanceId previous_sub_instance_id(UNDEFINED_ID);
    for (int j = 0; j < row.num_intervals(); ++j) {
      const IntervalId interval_id = row.interval_id(j);
      const Interval& interval = database_.interval(interval_id);

      for (int k = 0; k < interval.num_sub_instances(); ++k) {
        const SubInstanceId sub_instance_id = interval.sub_instance_id(k);

        if (previous_sub_instance_id != UNDEFINED_ID) {
          col_idx = previous_sub_instance_id;
          non_zero_elements.push_back(Triplet(row_idx, col_idx, -1));

          col_idx = sub_instance_id;
          non_zero_elements.push_back(Triplet(row_idx, col_idx, 1));

          ++row_idx;

          if (previous_sub_instance_id < num_sub_instances) {
            const SubInstance& previous_sub_instance =
                database_.sub_instance(previous_sub_instance_id);

            v.push_back(previous_sub_instance.width());
          } else {
            v.push_back(0.0);
          }
        }

        previous_sub_instance_id = sub_instance_id;
      }

      {
        const SubInstanceId sub_instance_id(num_sub_instances +
                                            dummy_variable_idx);

        interval_end_sub_instance_xs.push_back(interval.end());

        ++dummy_variable_idx;

        if (previous_sub_instance_id != UNDEFINED_ID) {
          col_idx = previous_sub_instance_id;
          non_zero_elements.push_back(Triplet(row_idx, col_idx, -1));

          col_idx = sub_instance_id;
          non_zero_elements.push_back(Triplet(row_idx, col_idx, 1));

          ++row_idx;

          if (previous_sub_instance_id < num_sub_instances) {
            const SubInstance& previous_sub_instance =
                database_.sub_instance(previous_sub_instance_id);

            v.push_back(previous_sub_instance.width());
          } else {
            v.push_back(0.0);
          }
        }

        previous_sub_instance_id = sub_instance_id;
      }
    }
  }

  const int num_interval_end_sub_instances = dummy_variable_idx;

  SparseMatrix cell_ordering_constraint_matrix(
      row_idx, num_sub_instances + num_interval_end_sub_instances);
  cell_ordering_constraint_matrix.setFromTriplets(non_zero_elements.begin(),
                                                  non_zero_elements.end());

  Vector width_vector(Eigen::Map<Vector>(v.data(), v.size()));

  // Construct interval constraint matrix.

  non_zero_elements.clear();
  row_idx = 0;
  col_idx = 0;
  v.clear();

  for (int i = 0; i < num_rows; ++i) {
    const RowId row_id(i);
    const Row& row = database_.row(row_id);

    for (int j = 0; j < row.num_intervals(); ++j) {
      const IntervalId interval_id = row.interval_id(j);
      const Interval& interval = database_.interval(interval_id);

      if (interval.num_sub_instances() > 0) {
        const SubInstanceId sub_instance_id = interval.first_sub_instance_id();

        col_idx = sub_instance_id;
        non_zero_elements.push_back(Triplet(row_idx, col_idx, 1));

        v.push_back(interval.begin());

        ++row_idx;
      }
    }
  }

  SparseMatrix interval_begin_constraint_matrix(
      row_idx, num_sub_instances + num_interval_end_sub_instances);
  interval_begin_constraint_matrix.setFromTriplets(non_zero_elements.begin(),
                                                   non_zero_elements.end());

  Vector interval_begin_vector(Eigen::Map<Vector>(v.data(), v.size()));

  SparseMatrix interval_constraint_matrix = interval_begin_constraint_matrix;
  Vector interval_vector = interval_begin_vector;

  // Construct multi-row-height instance constraint matrix.

  non_zero_elements.clear();
  row_idx = 0;
  col_idx = 0;
  dummy_variable_idx = 0;

  for (int i = 0; i < num_instances; ++i) {
    const InstanceId instance_id(i);
    const Instance& instance = database_.instance(instance_id);

    if (instance.is_fixed()) {
      continue;
    }

    for (int j = 1; j < instance.num_sub_instances(); ++j) {
      const SubInstanceId lower_sub_instance_id =
          instance.sub_instance_id(j - 1);
      const SubInstanceId sub_instance_id = instance.sub_instance_id(j);

      col_idx = lower_sub_instance_id;
      non_zero_elements.push_back(Triplet(row_idx, col_idx, -1));

      col_idx = sub_instance_id;
      non_zero_elements.push_back(Triplet(row_idx, col_idx, 1));

      if (instance.num_sub_instances() > 2) {
        if (j == 1) {
          col_idx = num_sub_instances + num_interval_end_sub_instances +
                    dummy_variable_idx;
          non_zero_elements.push_back(Triplet(row_idx, col_idx, 1));
        } else if (j == instance.num_sub_instances() - 1) {
          col_idx = num_sub_instances + num_interval_end_sub_instances +
                    dummy_variable_idx;
          non_zero_elements.push_back(Triplet(row_idx, col_idx, 1));

          ++dummy_variable_idx;
        } else {
          col_idx = num_sub_instances + num_interval_end_sub_instances +
                    dummy_variable_idx;
          non_zero_elements.push_back(Triplet(row_idx, col_idx, 1));

          ++dummy_variable_idx;

          col_idx = num_sub_instances + num_interval_end_sub_instances +
                    dummy_variable_idx;
          non_zero_elements.push_back(Triplet(row_idx, col_idx, 1));
        }
      }

      ++row_idx;
    }
  }

  SparseMatrix multi_row_height_instance_constraint_matrix(
      row_idx,
      num_sub_instances + num_interval_end_sub_instances + dummy_variable_idx);
  multi_row_height_instance_constraint_matrix.setFromTriplets(
      non_zero_elements.begin(), non_zero_elements.end());

  // Combine constraint matrices and make sure the result is of full row rank.

  const int num_dummy_variables =
      max(interval_constraint_matrix.rows(),
          multi_row_height_instance_constraint_matrix.cols() -
              num_sub_instances - num_interval_end_sub_instances);
  const int num_variables =
      num_sub_instances + num_interval_end_sub_instances + num_dummy_variables;

  SparseMatrix constraint_matrix = ConcatenateMatricesVertically(
      ConcatenateMatricesHorizontally(
          cell_ordering_constraint_matrix,
          SparseMatrix(cell_ordering_constraint_matrix.rows(),
                       num_dummy_variables)),
      ConcatenateMatricesHorizontally(
          interval_constraint_matrix,
          num_dummy_variables > interval_constraint_matrix.rows()
              ? ConcatenateMatricesHorizontally(
                    -1 * MakeIdentityMatrix(interval_constraint_matrix.rows()),
                    SparseMatrix(interval_constraint_matrix.rows(),
                                 num_dummy_variables -
                                     interval_constraint_matrix.rows()))
              : -1 * MakeIdentityMatrix(interval_constraint_matrix.rows())));

  Vector multiplier_vector(width_vector.rows() + interval_vector.rows());
  multiplier_vector << width_vector, interval_vector;

  if (num_variables > multi_row_height_instance_constraint_matrix.cols()) {
    multi_row_height_instance_constraint_matrix =
        ConcatenateMatricesHorizontally(
            multi_row_height_instance_constraint_matrix,
            SparseMatrix(
                multi_row_height_instance_constraint_matrix.rows(),
                num_variables -
                    multi_row_height_instance_constraint_matrix.cols()));
  }

  // Construct global placed position vector.

  const double interval_end_sub_instance_penalty_weight = 1.0e9;  // TODO: Tune.

  v = vector<double>(num_variables, 0.0);

  for (int i = 0; i < num_sub_instances; ++i) {
    const SubInstanceId sub_instance_id(i);
    const SubInstance& sub_instance = database_.sub_instance(sub_instance_id);

    v[i] = -1 * sub_instance.position().x();
  }
  for (int i = 0; i < num_interval_end_sub_instances; ++i) {
    v[num_sub_instances + i] = -1 * interval_end_sub_instance_penalty_weight *
                               interval_end_sub_instance_xs[i];
  }

  Vector global_placed_position_vector(Eigen::Map<Vector>(v.data(), v.size()));

  // Prepare matrices.

  /* const SparseMatrix Q = MakeIdentityMatrix(num_variables); */
  const SparseMatrix Q = ConcatenateMatricesVertically(
      ConcatenateMatricesHorizontally(
          ConcatenateMatricesVertically(
              ConcatenateMatricesHorizontally(
                  MakeIdentityMatrix(num_sub_instances),
                  SparseMatrix(num_sub_instances,
                               num_interval_end_sub_instances)),
              ConcatenateMatricesHorizontally(
                  SparseMatrix(num_interval_end_sub_instances,
                               num_sub_instances),
                  interval_end_sub_instance_penalty_weight *
                      MakeIdentityMatrix(num_interval_end_sub_instances))),
          SparseMatrix(num_sub_instances + num_interval_end_sub_instances,
                       num_dummy_variables)),
      ConcatenateMatricesHorizontally(
          SparseMatrix(num_dummy_variables,
                       num_sub_instances + num_interval_end_sub_instances),
          MakeIdentityMatrix(num_dummy_variables)));
  const SparseMatrix E = multi_row_height_instance_constraint_matrix;
  const SparseMatrix B = constraint_matrix;
  const Vector p = global_placed_position_vector;
  const Vector b = multiplier_vector;

  /*cout << "Q: " << Q.rows() << " X " << Q.cols() << endl;
  cout << "E: " << E.rows() << " X " << E.cols() << endl;
  cout << "B: " << B.rows() << " X " << B.cols() << endl;
  cout << "p: " << p.rows() << endl;
  cout << "b: " << b.rows() << endl;*/

  const SparseMatrix E_T = E.transpose();
  const SparseMatrix B_T = B.transpose();

  const double lambda = 1000;

  const SparseMatrix F = Q + lambda * E_T * E;

  //cout << "F: " << F.rows() << " X " << F.cols() << endl;

  const SparseMatrix A = ConcatenateMatricesVertically(
      ConcatenateMatricesHorizontally(F, -1 * B_T),
      ConcatenateMatricesHorizontally(B, SparseMatrix(B.rows(), B.rows())));
  const Vector q = [&]() {
    Vector tmp(p.rows() + b.rows());
    tmp << p, -1 * b;
    return tmp;
  }();

  //cout << "A: " << A.rows() << " X " << A.cols() << endl;
  //cout << "q: " << q.rows() << endl;

  const SparseMatrix D = TridiagMatrix(
      (B * B_T) -
      (B *
       (lambda * E_T *
        InverseDiagonalMatrix(MakeIdentityMatrix(E.rows()) + lambda * E * E_T) *
        E) *
       B_T));

  //cout << "D: " << D.rows() << " X " << D.cols() << endl;

  const double beta = 0.5;
  const double theta = 0.5;

  const SparseMatrix M = ConcatenateMatricesVertically(
      ConcatenateMatricesHorizontally((1 / beta) * F,
                                      SparseMatrix(B.cols(), B.rows())),
      ConcatenateMatricesHorizontally(B, (1 / theta) * D));
  const SparseMatrix N = ConcatenateMatricesVertically(
      ConcatenateMatricesHorizontally((1 / beta - 1) * F, B_T),
      ConcatenateMatricesHorizontally(SparseMatrix(B.rows(), B.cols()),
                                      (1 / theta) * D));
  const SparseMatrix I = MakeIdentityMatrix(M.rows());

  //cout << "M: " << M.rows() << " X " << M.cols() << endl;
  //cout << "N: " << N.rows() << " X " << N.cols() << endl;

  const double gamma = 10;
  const double epsilon = site_width / 4;  // TODO: Tune.

  v = vector<double>(q.rows(), epsilon);

  const Vector e(Eigen::Map<Vector>(v.data(), v.size()));

  // Start main algorithm.

  v = vector<double>(q.rows(), 0.0);

  Vector s(Eigen::Map<Vector>(v.data(), v.size()));

  v = vector<double>(q.rows(), 0.0);

  for (int i = 0; i < num_sub_instances; ++i) {
    const SubInstanceId sub_instance_id(i);
    const SubInstance& sub_instance = database_.sub_instance(sub_instance_id);

    v[i] = sub_instance.position().x();
  }
  for (int i = 0; i < num_interval_end_sub_instances; ++i) {
    v[num_sub_instances + i] = interval_end_sub_instance_xs[i];
  }

  Vector z(Eigen::Map<Vector>(v.data(), v.size()));

  Vector previous_z(z);
  Vector z_sub_instance_diff_abs;
  int k = 0;
  Eigen::SparseLU<SparseMatrix> lu_solver;
  const SparseMatrix M_plus_I = M + I;
  const SparseMatrix I_minus_A = I - A;
  const Vector gamma_multiply_q = gamma * q;
  lu_solver.compute(M_plus_I);
  do {
    s = lu_solver.solve(N * s + I_minus_A * AbsVector(s) - gamma_multiply_q);

    previous_z = z;
    z = (1 / gamma) * (AbsVector(s) + s);

    Vector z_sub_instance = z.head(num_sub_instances);
    Vector previous_z_sub_instance = previous_z.head(num_sub_instances);
    z_sub_instance_diff_abs =
        AbsVector(z_sub_instance - previous_z_sub_instance);

    /* cout << endl; */
    /* cout << "k: " << k << endl; */
    /* for (int i = 0; i < num_sub_instances; ++i) { */
    /*   cout << z(i) << " " << previous_z(i) << endl; */
    /* } */

    /* int count = 0; */
    /* for (Eigen::Index i = 0; i < z_sub_instance_diff_abs.rows(); ++i) { */
    /*   if (z_sub_instance_diff_abs(i) >= epsilon) { */
    /*     ++count; */
    /*   } */
    /* } */
    /* cout << count << endl; */

    ++k;
  } while (!(z_sub_instance_diff_abs < e));

  for (int i = 0; i < num_instances; ++i) {
    const InstanceId instance_id(i);
    Instance& instance = database_.instance(instance_id);

    if (instance.is_fixed()) {
      continue;
    }

    for (int j = 0; j < instance.num_sub_instances(); ++j) {
      const SubInstanceId sub_instance_id = instance.sub_instance_id(j);
      SubInstance& sub_instance = database_.sub_instance(sub_instance_id);

      sub_instance.set_position(
          Point(z(sub_instance_id), sub_instance.position().y()));

      if (j == 0) {
        instance.set_position(
            Point(sub_instance.position().x(), instance.position().y()));
      }
    }
  }
}

void Legalizer::PostMmsim() {
  //cout << "Align instances to sites..." << endl;

  AlignInstancesToSites();

  //cout << "Allocate illegal instances..." << endl;

  AllocateIllegalInstances();

  /* cout << "Meet edge spacing constraints..." << endl; */

  /* ResolveEdgeSpacingConstraint(); */

  cout << "Check if the result is legal..." << endl;

  if (IsResultLegal()) {
    cout << endl;
    cout << "Legal (^o^)v" << endl;
    cout << endl;
  } else {
    cout << endl;
    cout << "Illegal (ToT)" << endl;
    cout << endl;
  }
}

void Legalizer::PreDDA() {
	for (int i = 0; i < database_.num_instances(); ++i) {
		const InstanceId instance_id(i);
		Instance& instance = database_.instance(instance_id);
		instance.set_detail_initial_position(instance.position());
		//cout<<instance.position().x()<<"   "<<instance.detail_initial_position().x()<<endl;
	}
}

void Legalizer::SpreadInstances() {
  /* #pragma omp parallel for */
}

void Legalizer::AlignInstancesToRows() {
  const double row_height = Site::height();
  const RowId bottom_row_id(0);
  const RowId top_row_id(database_.num_rows() - 1);

  // Align instances from left to right.

  SortInstancesByX();

  for (int i = 0; i < database_.num_instances(); ++i) {
    const InstanceId current_instance_id =
        x_and_instance_id_sorted_by_x_[i].second;
    Instance& current_instance = database_.instance(current_instance_id);

    if (current_instance.is_fixed()) {
      continue;
    }

    const int current_instance_row_height =
        current_instance.num_sub_instances();

    const RowId nearest_row_id(
        static_cast<int>((current_instance.position().y() + 0.5 * row_height -
                          database_.die_rect().min_corner().y()) /
                         row_height));

    // Find best row.

    RowId best_row_id = nearest_row_id;
    vector<IntervalId> best_interval_ids;
    double best_x = current_instance.position().x();
    double best_cost = numeric_limits<double>::max();

    RowId current_row_id = nearest_row_id;
    for (int j = 0; j < 2 * database_.num_rows(); ++j) {
      if (j % 2 == 0) {
        current_row_id += j;
      } else {
        current_row_id -= j;
      }

      if (current_row_id < bottom_row_id ||
          current_row_id > top_row_id - current_instance_row_height + 1) {
        continue;
      }

      const Row& current_row = database_.row(current_row_id);
      const double current_row_y = current_row.position().y();
      const double y_intrinsic_displacement =
          abs(current_instance.global_placed_position().y() - current_row_y);

      if (y_intrinsic_displacement >= best_cost) {
        break;
      }

      // Meet P/G rail constraints.

      bool is_pg_rail_correct = true;

      // Only consider the rails on metal 1.
      if (current_row.has_rail_on_layer(LayerId(0))) {
        const RailId rail_id = current_row.rail_id_on_layer(LayerId(0));
        const Rail& rail = database_.rail(rail_id);

        // Wrong rail alignments of even-row-height instances can not be
        // resolved by just flipping vertically.
        if ((current_instance_row_height % 2 == 0) &&
            ((current_instance.is_bottom_ground() &&
              rail.type() == NetType::POWER) ||
             (!current_instance.is_bottom_ground() &&
              rail.type() == NetType::GROUND))) {
          is_pg_rail_correct = false;
        }
      }

      if (!is_pg_rail_correct) {
        continue;
      }

      // Meet fence region constraints.

      bool has_interval_of_current_instance_fence_region = true;

      for (int k = 0; k < current_instance_row_height; ++k) {
        const RowId row_id(current_row_id + k);
        const Row& row = database_.row(row_id);

        if (!row.has_interval_of_fence_region(
                current_instance.fence_region_id())) {
          has_interval_of_current_instance_fence_region = false;
        }

        if (!has_interval_of_current_instance_fence_region) {
          break;
        }
      }

      if (!has_interval_of_current_instance_fence_region) {
        continue;
      }

      // Compute cost.

      const double overlap_weight = 3.0;

      vector<IntervalId> interval_ids;

      bool can_be_placed_directly = true;
      for (int k = 0; k < current_instance_row_height; ++k) {
        const RowId row_id(current_row_id + k);
        const Row& row = database_.row(row_id);

        if (row.has_interval_of_fence_region_before(
                current_instance.fence_region_id(),
                current_instance.position().x() + current_instance.width())) {
          const IntervalId interval_id = row.interval_id_of_fence_region_before(
              current_instance.fence_region_id(),
              current_instance.position().x() + current_instance.width());
          const Interval& interval = database_.interval(interval_id);

          interval_ids.push_back(interval_id);

          if (interval.end() - interval.begin() < current_instance.width()) {
            can_be_placed_directly = false;
          }

          if (interval.end() <= current_instance.position().x()) {
            can_be_placed_directly = false;
          }
        } else {
          can_be_placed_directly = false;
        }

        if (!can_be_placed_directly) {
          break;
        }
      }

      if (can_be_placed_directly) {
        double min_interval_end = numeric_limits<double>::max();
        for (IntervalId id : interval_ids) {
          const Interval& interval = database_.interval(id);

          if (interval.end() < min_interval_end) {
            min_interval_end = interval.end();
          }
        }

        double max_interval_begin = 0.0;
        for (IntervalId id : interval_ids) {
          const Interval& interval = database_.interval(id);

          if (interval.begin() > max_interval_begin) {
            max_interval_begin = interval.begin();
          }
        }

        double current_instance_new_x = current_instance.position().x();
        if (current_instance_new_x + current_instance.width() >
            min_interval_end) {
          current_instance_new_x = min_interval_end - current_instance.width();
        } else if (current_instance_new_x < max_interval_begin) {
          current_instance_new_x = max_interval_begin;
        }
        const double x_intrinsic_displacement =
            abs(current_instance_new_x - current_instance.position().x());

        double overlap = 0.0;
        for (IntervalId id : interval_ids) {
          const Interval& interval = database_.interval(id);

          overlap += interval.ComputeAccumulatedOverlap(
              current_instance_new_x, current_instance.width());
        }

        const double cost = y_intrinsic_displacement +
                            x_intrinsic_displacement + overlap_weight * overlap;

        if (cost < best_cost) {
          best_cost = cost;
          best_x = current_instance_new_x;
          best_row_id = current_row_id;
          best_interval_ids = interval_ids;
        }
      }

      interval_ids.clear();

      bool can_be_placed_on_previous_interval = true;
      for (int k = 0; k < current_instance_row_height; ++k) {
        const RowId row_id(current_row_id + k);
        const Row& row = database_.row(row_id);

        if (row.has_interval_of_fence_region_before(
                current_instance.fence_region_id(),
                current_instance.position().x())) {
          const IntervalId interval_id = row.interval_id_of_fence_region_before(
              current_instance.fence_region_id(),
              current_instance.position().x());
          const Interval& interval = database_.interval(interval_id);

          interval_ids.push_back(interval_id);

          if (interval.end() - interval.begin() < current_instance.width()) {
            can_be_placed_on_previous_interval = false;
          }

          if (interval.end() > current_instance.position().x()) {
            can_be_placed_on_previous_interval = false;
          }
        } else {
          can_be_placed_on_previous_interval = false;
        }

        if (!can_be_placed_on_previous_interval) {
          break;
        }
      }

      if (can_be_placed_on_previous_interval) {
        double min_interval_end = numeric_limits<double>::max();
        for (IntervalId id : interval_ids) {
          const Interval& interval = database_.interval(id);

          if (interval.end() < min_interval_end) {
            min_interval_end = interval.end();
          }
        }

        const double current_instance_new_x =
            min_interval_end - current_instance.width();
        const double x_intrinsic_displacement =
            abs(current_instance_new_x - current_instance.position().x());

        double overlap = 0.0;
        for (IntervalId id : interval_ids) {
          const Interval& interval = database_.interval(id);

          overlap += interval.ComputeAccumulatedOverlap(
              current_instance_new_x, current_instance.width());
        }

        const double cost = y_intrinsic_displacement +
                            x_intrinsic_displacement + overlap_weight * overlap;

        if (cost < best_cost) {
          best_cost = cost;
          best_x = current_instance_new_x;
          best_row_id = current_row_id;
          best_interval_ids = interval_ids;
        }
      }

      interval_ids.clear();

      bool can_be_placed_on_next_interval = true;
      for (int k = 0; k < current_instance_row_height; ++k) {
        const RowId row_id(current_row_id + k);
        const Row& row = database_.row(row_id);

        if (row.has_interval_of_fence_region_after(
                current_instance.fence_region_id(),
                current_instance.position().x() + current_instance.width())) {
          const IntervalId interval_id = row.interval_id_of_fence_region_after(
              current_instance.fence_region_id(),
              current_instance.position().x() + current_instance.width());
          const Interval& interval = database_.interval(interval_id);

          interval_ids.push_back(interval_id);

          if (interval.end() - interval.begin() < current_instance.width()) {
            can_be_placed_on_next_interval = false;
          }
        } else {
          can_be_placed_on_next_interval = false;
        }

        if (!can_be_placed_on_next_interval) {
          break;
        }
      }

      if (can_be_placed_on_next_interval) {
        double max_interval_begin = 0.0;
        for (IntervalId id : interval_ids) {
          const Interval& interval = database_.interval(id);

          if (interval.begin() > max_interval_begin) {
            max_interval_begin = interval.begin();
          }
        }

        const double current_instance_new_x = max_interval_begin;
        const double x_intrinsic_displacement =
            abs(current_instance_new_x - current_instance.position().x());

        double overlap = 0.0;
        for (IntervalId id : interval_ids) {
          const Interval& interval = database_.interval(id);

          overlap += interval.ComputeAccumulatedOverlap(
              current_instance_new_x, current_instance.width());
        }

        const double cost = y_intrinsic_displacement +
                            x_intrinsic_displacement + overlap_weight * overlap;

        if (cost < best_cost) {
          best_cost = cost;
          best_x = current_instance_new_x;
          best_row_id = current_row_id;
          best_interval_ids = interval_ids;
        }
      }
    }

    // Save best result.

    const Row& best_row = database_.row(best_row_id);

    if (current_instance.orientation() != best_row.orientation()) {
      current_instance.FlipVertically();
    }

    current_instance.set_position(Point(best_x, best_row.position().y()));

    for (int j = 0; j < current_instance.num_sub_instances(); ++j) {
      const SubInstanceId sub_instance_id = current_instance.sub_instance_id(j);
      const IntervalId interval_id = best_interval_ids.at(j);
      SubInstance& sub_instance = database_.sub_instance(sub_instance_id);
      Interval& interval = database_.interval(interval_id);

      sub_instance.set_position(
          Point(best_x, best_row.position().y() + j * row_height));

      sub_instance.set_interval_id(interval_id);
      interval.add_sub_instance_id(sub_instance_id, best_x,
                                   sub_instance.width());
    }
  }
}

void Legalizer::AlignInstancesToSites() {
  const double die_right_x = database_.die_rect().max_corner().x();
  const double site_width = Site::width();
  const double row_height = Site::height();
  const int num_sites_limit = static_cast<int>(
      database_.num_rows() / 1 * row_height / site_width);  // TODO: Tune.

  SortInstancesByRowHeightByX();

  for (int h = x_and_instance_id_sorted_by_x_by_row_height_.size() - 1; h >= 0;
       --h) {
    const auto& x_and_instance_id_sorted_by_x =
        x_and_instance_id_sorted_by_x_by_row_height_[h];

    for (int i = 0; i < x_and_instance_id_sorted_by_x.size(); ++i) {
      const InstanceId current_instance_id =
          x_and_instance_id_sorted_by_x[i].second;
      Instance& current_instance = database_.instance(current_instance_id);

      if (current_instance.is_fixed()) {
        continue;
      }

      const int current_instance_site_width =
          static_cast<int>(current_instance.width() / site_width);
      const int nearest_site_idx_x =
          static_cast<int>((current_instance.position().x() + 0.5 * site_width -
                            database_.die_rect().min_corner().x()) /
                           site_width);
      const double nearest_site_x = database_.die_rect().min_corner().x() +
                                    nearest_site_idx_x * site_width;

      bool is_trial_successful = false;
      double best_x = die_right_x;
      for (int j = 0; j < num_sites_limit; ++j) {
        const double site_x = nearest_site_x + j * site_width;

        if (site_x + current_instance.width() > die_right_x) {
          break;
        }

        bool are_sites_available = true;
        bool is_overlap = false;
        for (int k = 0; k < current_instance.num_sub_instances(); ++k) {
          const SubInstanceId sub_instance_id =
              current_instance.sub_instance_id(k);
          const SubInstance& sub_instance =
              database_.sub_instance(sub_instance_id);

          for (int l = 0; l < current_instance_site_width; ++l) {
            const SiteId site_id = database_.site_id_by_position(
                Point(site_x + l * site_width, sub_instance.position().y()));
            const Site& site = database_.site(site_id);

            if (!site.is_valid() || site.has_sub_instance() ||
                site.fence_region_id() != current_instance.fence_region_id()) {
              are_sites_available = false;

              if (site.has_sub_instance()) {
                is_overlap = true;
              }

              break;
            }
          }

          if (!are_sites_available) {
            break;
          }
        }

        if (are_sites_available) {
          const double displacement_limit = 10 * site_width;  // TODO: Tune.
          const double displacement =
              abs(site_x - current_instance.global_placed_position().x());

          if (displacement < displacement_limit) {
            best_x = site_x;
            /* } else { */
            /*   if (TryPlaceInstance(current_instance_id, site_x, */
            /*                        displacement / 2)) { */
            /*     is_trial_successful = true; */
            /*   } */
          }

          break;
        }
      }

      /* if (is_trial_successful) { */
      /*   continue; */
      /* } */

      if (best_x == die_right_x) {
        if (TryPlaceInstance(current_instance_id, nearest_site_x,
                             10 * site_width)) {
          is_trial_successful = true;
        }
      }

      if (is_trial_successful) {
        continue;
      }

      if (best_x == die_right_x) {
        illegal_instance_ids_by_row_height_
            .at(current_instance.num_sub_instances() - 1)
            .push_back(current_instance_id);
      }

      current_instance.set_position(
          Point(best_x, current_instance.position().y()));
      database_.UpdateInstanceSubInstancePositions(current_instance_id);

      if (best_x != die_right_x) {
        FillInstanceSites(current_instance_id);
      }
    }
  }

  /*cout << "Illegal instances:" << endl;
  for (int i = 0; i < illegal_instance_ids_by_row_height_.size(); ++i) {
    cout << "  "
         << "# of " << i + 1
         << "-row-height: " << illegal_instance_ids_by_row_height_[i].size()
         << endl;
  }*/
}

void Legalizer::AllocateIllegalInstances() {
  const double row_height = Site::height();
  const double site_width = Site::width();
  const int num_rows = database_.num_rows();
  const int num_row_sites = database_.num_sites() / num_rows;
  const RowId bottom_row_id(0);
  const RowId top_row_id(database_.num_rows() - 1);
  const int left_site = 0;
  const int right_site = num_row_sites - 1;

  for (int h = database_.max_instance_row_height() - 1; h >= 0; --h) {
    for (int i = 0; i < illegal_instance_ids_by_row_height_.at(h).size(); i++) {
      const InstanceId current_instance_id =
          illegal_instance_ids_by_row_height_.at(h)[i];
      Instance& current_instance = database_.instance(current_instance_id);

      const double current_instance_x =
          current_instance.global_placed_position().x();
      const double current_instance_y =
          current_instance.global_placed_position().y();
      const double current_instance_width = current_instance.width();
      const int current_instance_site_width =
          static_cast<int>(current_instance.width() / site_width);
      const int current_instance_row_height =
          current_instance.num_sub_instances();
      RowId nearest_row_id(
          static_cast<int>((current_instance_y + 0.5 * row_height -
                            database_.die_rect().min_corner().y()) /
                           row_height));
      int nearest_row_site =
          static_cast<int>((current_instance_x + 0.5 * site_width -
                            database_.die_rect().min_corner().x()) /
                           site_width);

      // Fence Region information
      const FenceRegionId fenge_region_id = current_instance.fence_region_id();

      if (nearest_row_id < bottom_row_id) {
        nearest_row_id = bottom_row_id;
      } else if (nearest_row_id >
                 top_row_id - current_instance_row_height + 1) {
        nearest_row_id = top_row_id - current_instance_row_height + 1;
      }

      assert(nearest_row_id >= bottom_row_id &&
             nearest_row_id <= top_row_id - current_instance_row_height + 1);

      // Find start point in fence region position
      Row& near_row = database_.row(nearest_row_id);
      if (!near_row.has_interval_of_fence_region(fenge_region_id)) {
        for (int j = 0; j < 2 * database_.num_rows(); ++j) {
          if (j % 2 == 0) {
            nearest_row_id += j;
          } else {
            nearest_row_id -= j;
          }
          if (nearest_row_id < bottom_row_id ||
              nearest_row_id > top_row_id - current_instance_row_height + 1) {
            continue;
          }
          assert(nearest_row_id >= bottom_row_id &&
                 nearest_row_id <=
                     top_row_id - current_instance_row_height + 1);

          const Row& test_row = database_.row(nearest_row_id);
          if (test_row.has_interval_of_fence_region(fenge_region_id)) {
            near_row = test_row;
            break;
          }
        }
      }

      if (near_row.has_interval_of_fence_region_before(fenge_region_id,
                                                       current_instance_x)) {
        const IntervalId before_interval_id =
            near_row.interval_id_of_fence_region_before(fenge_region_id,
                                                        current_instance_x);
        const Interval& before_interval =
            database_.interval(before_interval_id);
        const double interval_start = before_interval.begin();
        const double interval_end = before_interval.end();
        if (interval_start < current_instance_x &&
            current_instance_x + current_instance_width < interval_end) {
          // case C
        } else {
          // case D case E
          nearest_row_site = static_cast<int>(
              (interval_end - site_width * current_instance_site_width +
               0.5 * site_width - database_.die_rect().min_corner().x()) /
              site_width);
        }
      } else {
        const IntervalId after_has_interval_id =
            near_row.interval_id_of_fence_region_after(fenge_region_id,
                                                       current_instance_x);
        const Interval& after_interval =
            database_.interval(after_has_interval_id);
        const double interval_start = after_interval.begin();
        nearest_row_site =
            static_cast<int>((interval_start + 0.5 * site_width -
                              database_.die_rect().min_corner().x()) /
                             site_width);
      }

      double displacement;
      double displacement_x;
      double displacement_y;
      double best_displacement = numeric_limits<double>::max();
      int best_site_x = 0;
      int best_site_y = 0;

      RowId current_row_id = nearest_row_id;
      bool out_of_loop = false;
      for (int j = 0; j < 2 * database_.num_rows(); ++j) {
        if (out_of_loop) {
          break;
        }
        if (j % 2 == 0) {
          current_row_id += j;
        } else {
          current_row_id -= j;
        }

        // Meet top and bottom row
        if (current_row_id < bottom_row_id ||
            current_row_id > top_row_id - current_instance_row_height + 1) {
          continue;
        }

        assert(current_row_id >= bottom_row_id &&
               current_row_id <= top_row_id - current_instance_row_height + 1);

        const Row& current_row = database_.row(current_row_id);
        // meet fence region constraint
        if (!current_row.has_interval_of_fence_region(fenge_region_id)) {
          continue;
        }
        const double site_y = current_row.position().y();

        // Meet P/G rail constraints.
        bool is_pg_rail_correct = true;
        // Only consider the rails on metal 1.
        if (current_row.has_rail_on_layer(LayerId(0))) {
          const RailId rail_id = current_row.rail_id_on_layer(LayerId(0));
          const Rail& rail = database_.rail(rail_id);
          // Wrong rail alignments of even-row-height instances can not be
          // resolved by just flipping vertically.
          if ((current_instance_row_height % 2 == 0) &&
              ((current_instance.is_bottom_ground() &&
                rail.type() == NetType::POWER) ||
               (!current_instance.is_bottom_ground() &&
                rail.type() == NetType::GROUND))) {
            is_pg_rail_correct = false;
          }
        }
        if (!is_pg_rail_correct) {
          continue;
        }

        int current_row_site = nearest_row_site;
        for (int k = 0; k < 2 * num_row_sites; k++) {
          if (k % 2 == 0) {
            current_row_site += k;
          } else {
            current_row_site -= k;
          }
          // Meet left and right site
          if ((current_row_site < left_site) ||
              ((current_row_site + current_instance_site_width) >
               (right_site + 1))) {
            continue;
          }

          // decide position
          const double site_x = current_row_site * site_width;

          // check sub_instance bottom to top
          bool jump_out_of_loop = false;
          for (int m = 0; m < current_instance.num_sub_instances(); ++m) {
            // check sub_instance left to right
            for (int n = 0; n < current_instance_site_width; ++n) {
              const SiteId site_id = database_.site_id_by_position(
                  Point(site_x + n * site_width, site_y + m * row_height));
              const Site& site = database_.site(site_id);
              const FenceRegionId& site_fenge_region_id =
                  site.fence_region_id();
              if (!site.is_valid() || site.has_sub_instance() ||
                  site_fenge_region_id != fenge_region_id) {
                jump_out_of_loop = true;
                break;
              }
            }
            if (jump_out_of_loop) {
              break;
            }
          }
          if (jump_out_of_loop) {
            continue;
          }

          displacement_x = fabs(site_x - current_instance_x);
          displacement_y = fabs(site_y - current_instance_y);
          displacement = displacement_x + displacement_y;
          if (displacement < best_displacement) {
            best_displacement = displacement;
            best_site_x = site_x;
            best_site_y = site_y;
            if (displacement_x < row_height) {
              out_of_loop = true;
            }
            break;
          }
        }
      }

      current_instance.set_position(Point(best_site_x, best_site_y));

      const RowId best_row_id(
          (best_site_y - database_.die_rect().min_corner().y()) / row_height);

      assert(best_row_id >= bottom_row_id &&
             best_row_id <= top_row_id - current_instance_row_height + 1);

      const Row& best_row = database_.row(best_row_id);

      if (current_instance.orientation() != best_row.orientation()) {
        current_instance.FlipVertically();
      }

      for (int j = 0; j < current_instance.num_sub_instances(); ++j) {
        const SubInstanceId sub_instance_id =
            current_instance.sub_instance_id(j);
        SubInstance& sub_instance = database_.sub_instance(sub_instance_id);
        sub_instance.set_position(
            Point(current_instance.position().x(),
                  current_instance.position().y() + j * row_height));

        if (sub_instance.position().x() <
            database_.die_rect().max_corner().x()) {
          for (int k = 0; k < current_instance_site_width; ++k) {
            const SiteId site_id = database_.site_id_by_position(
                Point(current_instance.position().x() + k * site_width,
                      current_instance.position().y() + j * row_height));
            Site& site = database_.site(site_id);
            site.set_sub_instance_id(sub_instance_id);
          }
        }
      }
    }
  }
}

void Legalizer::ResolveEdgeSpacingConstraint() {
  SortInstancesByX();

  for (int i = 0; i < x_and_instance_id_sorted_by_x_.size(); ++i) {
    const InstanceId instance_id = x_and_instance_id_sorted_by_x_[i].second;

    vector<InstanceId> right_instance_ids =
        FindAdjacentInstances(instance_id, false,
                              (database_.die_rect().max_corner().x() -
                               database_.die_rect().min_corner().x()));

    if (right_instance_ids.empty()) {
      continue;
    }

    const Instance& instance = database_.instance(instance_id);
    const EdgeType instance_right_edge_type = instance.right_edge_type();

    for (int i = 0; i < right_instance_ids.size(); ++i) {
      const InstanceId right_instance_id(right_instance_ids[i]);
      Instance& right_instance = database_.instance(right_instance_id);
      const double distance = right_instance.position().x() -
                              (instance.position().x() + instance.width());
      const EdgeType right_instance_left_edge_type =
          right_instance.left_edge_type();
      const double edge_spacing_limit = database_.edge_type_spacing(
          instance_right_edge_type, right_instance_left_edge_type);

      if (distance < edge_spacing_limit) {
        right_instance.FlipHorizontally();
      }
    }
  }
}

void Legalizer::SortInstancesByX() {
  x_and_instance_id_sorted_by_x_.clear();

  for (int i = 0; i < database_.num_instances(); ++i) {
    const InstanceId instance_id(i);
    const Instance& instance = database_.instance(instance_id);

    x_and_instance_id_sorted_by_x_.push_back(
        make_pair(instance.position().x(), instance_id));
  }

  sort(x_and_instance_id_sorted_by_x_.begin(),
       x_and_instance_id_sorted_by_x_.end(),
       [](const pair<double, InstanceId>& pair_a,
          const pair<double, InstanceId>& pair_b) {
         return pair_a.first < pair_b.first;
       });
}

void Legalizer::SortInstancesByRowHeightByX() {
  for (int i = 0; i < x_and_instance_id_sorted_by_x_by_row_height_.size();
       ++i) {
    const int instance_row_height = i + 1;
    auto& x_and_instance_id_sorted_by_x =
        x_and_instance_id_sorted_by_x_by_row_height_[i];

    x_and_instance_id_sorted_by_x.clear();

    for (int j = 0;
         j < database_.num_instances_of_row_height(instance_row_height); ++j) {
      const InstanceId instance_id(
          database_.instance_id_by_row_height(instance_row_height, j));
      const Instance& instance = database_.instance(instance_id);

      x_and_instance_id_sorted_by_x.push_back(
          make_pair(instance.position().x(), instance_id));
    }
  }

  for (int i = 0; i < x_and_instance_id_sorted_by_x_by_row_height_.size();
       ++i) {
    auto& x_and_instance_id_sorted_by_x =
        x_and_instance_id_sorted_by_x_by_row_height_[i];

    sort(x_and_instance_id_sorted_by_x.begin(),
         x_and_instance_id_sorted_by_x.end(),
         [](const pair<double, InstanceId>& pair_a,
            const pair<double, InstanceId>& pair_b) {
           return pair_a.first < pair_b.first;
         });
  }
}

bool Legalizer::IsResultLegal() {
  const double site_width = Site::width();
  const double row_height = Site::height();

  for (int i = 0; i < database_.num_instances(); ++i) {
    const InstanceId instance_id(i);
    const Instance& instance = database_.instance(instance_id);

    if (instance.is_fixed()) {
      continue;
    }

    if (instance.position().x() < database_.die_rect().min_corner().x() ||
        instance.position().y() < database_.die_rect().min_corner().y() ||
        instance.position().x() + instance.width() >
            database_.die_rect().max_corner().x() ||
        instance.position().y() + instance.height() >
            database_.die_rect().max_corner().y()) {
      cout << "[HARD] Out of chip:" << endl;
      cout << "  instance: " << instance.name() << " at " << instance.position()
           << endl;

      return false;
    }

    const int instance_site_width =
        static_cast<int>(instance.width() / site_width);

    for (int j = 0; j < instance.num_sub_instances(); ++j) {
      const SubInstanceId sub_instance_id(instance.sub_instance_id(j));
      const SubInstance& sub_instance = database_.sub_instance(sub_instance_id);

      for (int k = 0; k < instance_site_width; ++k) {
        const SiteId site_id(database_.site_id_by_position(
            Point(sub_instance.position().x() + k * site_width,
                  sub_instance.position().y())));
        const Site& site = database_.site(site_id);

        if (!site.is_valid() ||
            site.fence_region_id() != instance.fence_region_id() ||
            site.sub_instance_id() != sub_instance_id) {
          cout << "[HARD] Wrong site:" << endl;
          cout << "  instance: " << instance.name() << " at "
               << instance.position() << endl;

          return false;
        }
      }
    }
  }

  for (int i = 0; i < database_.num_instances(); ++i) {
    const InstanceId instance_id(i);
    const Instance& instance = database_.instance(instance_id);

    if (instance.is_fixed()) {
      continue;
    }

    const RowId row_id(static_cast<int>(
        (instance.position().y() - database_.die_rect().min_corner().y()) /
        row_height));
    const Row& row = database_.row(row_id);

    if (instance.orientation() != row.orientation() &&
        FlipOrientation(instance.orientation()) != row.orientation()) {
      cout << "[HARD] Wrong P/G rail alignment:" << endl;
      cout << "  instance: " << instance.name() << " at " << instance.position()
           << endl;

      return false;
    }
  }

  // Check edge spacing constraint.

  SortInstancesByX();

  /*for (int i = 0; i < x_and_instance_id_sorted_by_x_.size(); ++i) {
    const InstanceId instance_id = x_and_instance_id_sorted_by_x_[i].second;

    vector<InstanceId> right_instance_ids =
        FindAdjacentInstances(instance_id, false,
                              (database_.die_rect().max_corner().x() -
                               database_.die_rect().min_corner().x()));

    if (right_instance_ids.empty()) {
      continue;
    }

    const Instance& instance = database_.instance(instance_id);
    const EdgeType instance_right_edge_type = instance.right_edge_type();

    for (int i = 0; i < right_instance_ids.size(); ++i) {
      const InstanceId right_instance_id(right_instance_ids[i]);
      Instance& right_instance = database_.instance(right_instance_id);
      const double distance = right_instance.position().x() -
                              (instance.position().x() + instance.width());
      const EdgeType right_instance_left_edge_type =
          right_instance.left_edge_type();
      const double edge_spacing_limit = database_.edge_type_spacing(
          instance_right_edge_type, right_instance_left_edge_type);

      if (distance < edge_spacing_limit) {
        cout << "[SOFT] Violate edge spacing:" << endl;
        cout << "  " << distance << " < " << edge_spacing_limit << endl;
        cout << "  left instance: " << instance.name() << " at "
             << instance.position() << endl;
        cout << "  right instance: " << right_instance.name() << " at "
             << right_instance.position() << endl;
      }
    }
  }*/

  return true;
}

void Legalizer::ClearInstanceSites(InstanceId instance_id) {
  const double site_width = Site::width();

  const Instance& instance = database_.instance(instance_id);
  const int instance_site_width =
      static_cast<int>(ceil(instance.width() / site_width));

  assert(instance.position().x() >= database_.die_rect().min_corner().x());
  assert(instance.position().x() + instance.width() <=
         database_.die_rect().max_corner().x());

  for (int i = 0; i < instance.num_sub_instances(); ++i) {
    const SubInstanceId sub_instance_id = instance.sub_instance_id(i);
    const SubInstance& sub_instance = database_.sub_instance(sub_instance_id);

    for (int j = 0; j < instance_site_width; ++j) {
      const SiteId site_id = database_.site_id_by_position(
          Point(sub_instance.position().x() + j * site_width,
                sub_instance.position().y()));
      Site& site = database_.site(site_id);

      assert(site.is_valid());
      assert(site.has_sub_instance());
      assert(site.sub_instance_id() == sub_instance_id);

      site.remove_sub_instance_id();
    }
  }
}

void Legalizer::FillInstanceSites(InstanceId instance_id) {
  const double site_width = Site::width();

  const Instance& instance = database_.instance(instance_id);
  const int instance_site_width =
      static_cast<int>(ceil(instance.width() / site_width));

  assert(instance.position().x() >= database_.die_rect().min_corner().x());
  assert(instance.position().x() + instance.width() <=
         database_.die_rect().max_corner().x());

  for (int i = 0; i < instance.num_sub_instances(); ++i) {
    const SubInstanceId sub_instance_id = instance.sub_instance_id(i);
    const SubInstance& sub_instance = database_.sub_instance(sub_instance_id);

    for (int j = 0; j < instance_site_width; ++j) {
      const SiteId site_id = database_.site_id_by_position(
          Point(sub_instance.position().x() + j * site_width,
                sub_instance.position().y()));
      Site& site = database_.site(site_id);

      assert(site.is_valid());
      assert(!site.has_sub_instance());
      assert(site.fence_region_id() == instance.fence_region_id());

      site.set_sub_instance_id(sub_instance_id);
    }
  }
}

vector<InstanceId> Legalizer::FindAdjacentInstances(InstanceId instance_id,
                                                    bool is_left,
                                                    double adjacent_threshold) {
  const double site_width = Site::width();

  const Instance& instance = database_.instance(instance_id);

  vector<InstanceId> adjacent_instance_ids;
  if (is_left) {
    vector<bool> is_instance_met_by_id(database_.num_instances(), false);
    for (int i = 0; i < instance.num_sub_instances(); ++i) {
      const SubInstanceId sub_instance_id(instance.sub_instance_id(i));
      const SubInstance& sub_instance = database_.sub_instance(sub_instance_id);

      for (double x = sub_instance.position().x() - site_width;
           sub_instance.position().x() - x < adjacent_threshold &&
           x >= database_.die_rect().min_corner().x();
           x -= site_width) {
        const SiteId site_id(database_.site_id_by_position(
            Point(x, sub_instance.position().y())));
        const Site& site = database_.site(site_id);

        if (site.has_sub_instance()) {
          const SubInstanceId adjacent_sub_instance_id(site.sub_instance_id());
          const SubInstance& adjacent_sub_instance =
              database_.sub_instance(adjacent_sub_instance_id);
          const InstanceId adjacent_instance_id(
              adjacent_sub_instance.instance_id());

          if (!is_instance_met_by_id.at(adjacent_instance_id)) {
            adjacent_instance_ids.push_back(adjacent_instance_id);
            is_instance_met_by_id.at(adjacent_instance_id) = true;
          }

          break;
        }
      }
    }
  } else {
    vector<bool> is_instance_met_by_id(database_.num_instances(), false);
    for (int i = 0; i < instance.num_sub_instances(); ++i) {
      const SubInstanceId sub_instance_id(instance.sub_instance_id(i));
      const SubInstance& sub_instance = database_.sub_instance(sub_instance_id);

      for (double x = sub_instance.position().x() + sub_instance.width();
           x - (sub_instance.position().x() + sub_instance.width()) <
               adjacent_threshold &&
           x <= database_.die_rect().max_corner().x() - site_width;
           x += site_width) {
        const SiteId site_id(database_.site_id_by_position(
            Point(x, sub_instance.position().y())));
        const Site& site = database_.site(site_id);

        if (site.has_sub_instance()) {
          const SubInstanceId adjacent_sub_instance_id(site.sub_instance_id());
          const SubInstance& adjacent_sub_instance =
              database_.sub_instance(adjacent_sub_instance_id);
          const InstanceId adjacent_instance_id(
              adjacent_sub_instance.instance_id());

          if (!is_instance_met_by_id.at(adjacent_instance_id)) {
            adjacent_instance_ids.push_back(adjacent_instance_id);
            is_instance_met_by_id.at(adjacent_instance_id) = true;
          }

          break;
        }
      }
    }
  }

  return adjacent_instance_ids;
}

bool Legalizer::TryPlaceInstance(InstanceId root_instance_id,
                                 double root_instance_new_x,
                                 double displacement_limit) {
  const double die_right_x = database_.die_rect().max_corner().x();
  const double site_width = Site::width();
  const Instance& root_instance = database_.instance(root_instance_id);

  map<InstanceId, double> instance_original_x_by_id;
  instance_original_x_by_id.insert(
      make_pair(root_instance_id, root_instance_new_x));

  stack<pair<InstanceId, double>> s;
  s.push(make_pair(root_instance_id, root_instance_new_x));

  bool is_instance_placeable = true;
  set<InstanceId> unplaceable_instance_ids;
  while (!s.empty()) {
    const auto& top = s.top();
    const InstanceId current_instance_id(top.first);
    const double current_instance_new_x = top.second;

    s.pop();

    Instance& current_instance = database_.instance(current_instance_id);
    const int current_instance_site_width =
        static_cast<int>(ceil(current_instance.width() / site_width));

    /* cout << "instance: " << current_instance.name() << " at " */
    /*      << current_instance.position() << " " << current_instance.width() */
    /*      << " X " << current_instance.height() << " " << current_instance_new_x */
    /*      << endl; */

    double incurred_displacement = 0.0;
    for (const auto& pair : instance_original_x_by_id) {
      const InstanceId instance_id(pair.first);
      const double instance_original_x = pair.second;
      Instance& instance = database_.instance(instance_id);
      incurred_displacement +=
          (abs(instance.position().x() - instance_original_x));
    }

    if (current_instance_new_x < database_.die_rect().min_corner().x() ||
        current_instance_new_x + current_instance.width() >
            database_.die_rect().max_corner().x() ||
        incurred_displacement > displacement_limit) {
      /* cout << "  not placeable" << endl; */
      is_instance_placeable = false;
      unplaceable_instance_ids.insert(current_instance_id);
      while (!s.empty()) {
        unplaceable_instance_ids.insert(s.top().first);
        s.pop();
      }
      break;
    }

    set<InstanceId> overlap_instance_ids;
    for (int i = 0; i < current_instance.num_sub_instances(); ++i) {
      const SubInstanceId sub_instance_id(current_instance.sub_instance_id(i));
      const SubInstance& sub_instance = database_.sub_instance(sub_instance_id);

      for (int j = 0; j < current_instance_site_width; ++j) {
        const SiteId site_id = database_.site_id_by_position(
            Point(current_instance_new_x + j * site_width,
                  sub_instance.position().y()));
        const Site& site = database_.site(site_id);

        if (!site.is_valid() ||
            site.fence_region_id() != current_instance.fence_region_id()) {
          /* cout << "  not placeable" << endl; */
          is_instance_placeable = false;
          unplaceable_instance_ids.insert(current_instance_id);
          while (!s.empty()) {
            unplaceable_instance_ids.insert(s.top().first);
            s.pop();
          }
          break;
        }

        if (site.has_sub_instance()) {
          const SubInstanceId overlap_sub_instance_id(site.sub_instance_id());
          const SubInstance& overlap_sub_instance =
              database_.sub_instance(overlap_sub_instance_id);
          const InstanceId overlap_instance_id(
              overlap_sub_instance.instance_id());
          Instance& overlap_instance = database_.instance(overlap_instance_id);

          if (overlap_instance_ids.count(overlap_instance_id) == 0) {
            overlap_instance_ids.insert(overlap_instance_id);

            /* cout << "  overlap instance: " << overlap_instance.name() << " at " */
            /*      << overlap_instance.position() << " " */
            /*      << overlap_instance.width() << " X " */
            /*      << overlap_instance.height() << endl; */

            double overlap_instance_new_x = die_right_x;
            if (ComputeManhattanDistanceBetweenPoints(
                    overlap_instance.position(),
                    Point(current_instance_new_x - overlap_instance.width(),
                          overlap_instance.position().y())) <
                ComputeManhattanDistanceBetweenPoints(
                    overlap_instance.position(),
                    Point(current_instance_new_x + current_instance.width(),
                          overlap_instance.position().y()))) {
              overlap_instance_new_x =
                  current_instance_new_x - overlap_instance.width();
            } else {
              overlap_instance_new_x =
                  current_instance_new_x + current_instance.width();
            }

            /* const double overlap_instance_new_x = */
            /*     current_instance_new_x - overlap_instance.width(); */

            if (overlap_instance_id != root_instance_id ||
                instance_original_x_by_id.count(overlap_instance_id) != 0) {
              /* cout << "  clear overlap instance" << endl; */
              ClearInstanceSites(overlap_instance_id);

              if (instance_original_x_by_id.count(overlap_instance_id) == 0) {
                instance_original_x_by_id.insert(make_pair(
                    overlap_instance_id, overlap_instance.position().x()));
              }
            }

            /* overlap_instance.set_position( */
            /*     Point(overlap_instance_new_x, overlap_instance.position().y())); */
            /* database_.UpdateInstanceSubInstancePositions(overlap_instance_id); */

            s.push(make_pair(overlap_instance_id, overlap_instance_new_x));
          }
        }
      }

      if (!is_instance_placeable) {
        break;
      }
    }

    if (!is_instance_placeable) {
      break;
    }

    /* incurred_displacement -= */
    /*     (abs(current_instance.position().x() - */
    /*          current_instance.global_placed_position().x())); */

    current_instance.set_position(
        Point(current_instance_new_x, current_instance.position().y()));
    database_.UpdateInstanceSubInstancePositions(current_instance_id);

    /* cout << "  fill current instance" << endl; */
    FillInstanceSites(current_instance_id);

    /* cout << "  incurred_displacement: " << incurred_displacement << endl; */
  }

  if (!is_instance_placeable) {
    /* cout << "  clear all sites" << endl; */
    for (const auto& pair : instance_original_x_by_id) {
      const InstanceId instance_id(pair.first);

      if (unplaceable_instance_ids.count(instance_id) == 0) {
        /* const Instance& instance = database_.instance(instance_id); */
        /* cout << "    instance: " << instance.name() << " at " */
        /*      << instance.position() << " " << instance.width() << " X " */
        /*      << instance.height() << endl; */
        ClearInstanceSites(instance_id);
      }
    }

    /* cout << "  recover all sites" << endl; */
    for (const auto& pair : instance_original_x_by_id) {
      const InstanceId instance_id(pair.first);
      const double instance_original_x = pair.second;
      Instance& instance = database_.instance(instance_id);

      instance.set_position(
          Point(instance_original_x, instance.position().y()));
      database_.UpdateInstanceSubInstancePositions(instance_id);

      if (instance_id != root_instance_id) {
        /* cout << "    instance: " << instance.name() << " at " */
        /*      << instance.position() << " " << instance.width() << " X " */
        /*      << instance.height() << endl; */
        FillInstanceSites(instance_id);
      }
    }

    /* Instance& root_instance = database_.instance(root_instance_id); */
    /* root_instance.set_position( */
    /*     Point(die_right_x, root_instance.position().y())); */
  }

  return is_instance_placeable;
}
