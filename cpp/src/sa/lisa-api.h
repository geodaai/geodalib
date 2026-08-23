// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#ifndef GEODA_LOCAL_STATISTICS_H
#define GEODA_LOCAL_STATISTICS_H

#include <string>
#include <vector>

#include "lisa/LISA.h"
#include "lisa/BatchLISA.h"

namespace geoda {

enum LocalMoranClusterType { HH = 1, LL = 2, HL = 3, LH = 4, NEIGHBORLESS = 5, UNDEFINED = 6 };

std::vector<std::vector<unsigned int>> create_perm_table(const std::vector<std::vector<unsigned int>>& neighbors,
                                                         unsigned int num_permutations, unsigned int max_num_neighbors);

struct LisaResult {
  bool is_valid;
  std::vector<double> sig_local_vec;
  std::vector<int> sig_cat_vec;
  std::vector<int> cluster_vec;
  std::vector<double> lag_vec;
  std::vector<double> lisa_vec;
  std::vector<int> nn_vec;
  std::vector<std::string> labels;
  std::vector<std::string> colors;

  bool get_is_valid() { return is_valid; }
  std::vector<double> get_sig_local() { return sig_local_vec; }
  std::vector<int> get_sig_cat() { return sig_cat_vec; }
  std::vector<int> get_cluster() { return cluster_vec; }
  std::vector<double> get_lag() { return lag_vec; }
  std::vector<double> get_lisa() { return lisa_vec; }
  std::vector<int> get_nn() { return nn_vec; }
  std::vector<std::string> get_labels() { return labels; }
  std::vector<std::string> get_colors() { return colors; }
};

static void set_lisa_content(LISA* lisa, LisaResult& rst) {
  rst.is_valid = true;
  rst.sig_local_vec = lisa->GetLocalSignificanceValues();
  rst.sig_cat_vec = lisa->GetSigCatIndicators();
  rst.cluster_vec = lisa->GetClusterIndicators();
  rst.lag_vec = lisa->GetSpatialLagValues();
  rst.lisa_vec = lisa->GetLISAValues();
  rst.nn_vec = lisa->GetNumNeighbors();
  rst.labels = lisa->GetLabels();
  rst.colors = lisa->GetColors();
}

struct BatchLisaResult {
  bool is_valid;
  std::vector<std::vector<double>> lisa_values;
  std::vector<std::vector<double>> sig_values;
  std::vector<std::vector<int>> cluster_values;
  std::vector<std::vector<double>> lag_values;
  std::vector<int> nn;
  std::vector<std::string> labels;
  std::vector<std::string> colors;

  bool get_is_valid() { return is_valid; }
  std::vector<std::vector<double>> get_lisa_values() { return lisa_values; }
  std::vector<std::vector<double>> get_sig_values() { return sig_values; }
  std::vector<std::vector<int>> get_cluster_values() { return cluster_values; }
  std::vector<std::vector<double>> get_lag_values() { return lag_values; }
  std::vector<int> get_nn() { return nn; }
  std::vector<std::string> get_labels() { return labels; }
  std::vector<std::string> get_colors() { return colors; }
};

static void set_batch_lisa_content(BatchLISA* lisa, BatchLisaResult& rst, size_t num_vars) {
  rst.is_valid = true;
  for (size_t i = 0; i < num_vars; ++i) {
    rst.lisa_values.push_back(lisa->GetLISAValues(static_cast<int>(i)));
    rst.sig_values.push_back(lisa->GetLocalSignificanceValues(static_cast<int>(i)));
    rst.cluster_values.push_back(lisa->GetClusterIndicators(static_cast<int>(i)));
    rst.lag_values.push_back(lisa->GetSpatialLagValues(static_cast<int>(i)));
  }
  rst.nn = lisa->GetNumNeighbors();
  rst.labels = lisa->GetLabels();
  rst.colors = lisa->GetColors();
}

LisaResult local_moran(const std::vector<double>& data, const std::vector<std::vector<unsigned int>>& neighbors,
                       const std::vector<unsigned int>& undefs, double significance_cutoff, unsigned int perm,
                       int last_seed);

LisaResult local_bivariate_moran(const std::vector<double>& data1, const std::vector<double>& data2,
                                 const std::vector<std::vector<unsigned int>>& neighbors,
                                 const std::vector<unsigned int>& undefs, double significance_cutoff, unsigned int perm,
                                 int last_seed);

LisaResult local_g(const std::vector<double>& data, const std::vector<std::vector<unsigned int>>& neighbors,
                   const std::vector<unsigned int>& undefs, double significance_cutoff, unsigned int perm,
                   int last_seed, int is_gstar);

LisaResult local_geary(const std::vector<double>& data, const std::vector<std::vector<unsigned int>>& neighbors,
                       const std::vector<unsigned int>& undefs, double significance_cutoff, unsigned int perm,
                       int last_seed);

LisaResult local_multivariate_geary(const std::vector<std::vector<double>>& data,
                                    const std::vector<std::vector<unsigned int>>& neighbors,
                                    const std::vector<std::vector<unsigned int>>& undefs, double significance_cutoff,
                                    unsigned int perm, int last_seed);

LisaResult quantile_lisa(int k, int quantile, const std::vector<double>& data,
                         const std::vector<std::vector<unsigned int>>& neighbors,
                         const std::vector<unsigned int>& undefs, double significance_cutoff, unsigned int perm,
                         int last_seed);

BatchLisaResult batch_local_moran(const std::vector<std::vector<double>>& data,
                                  const std::vector<std::vector<unsigned int>>& neighbors,
                                  const std::vector<std::vector<unsigned int>>& undefs, double significance_cutoff,
                                  unsigned int perm, int last_seed);
}  // namespace geoda

#endif  // GEODA_LOCAL_STATISTICS_H
