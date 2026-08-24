// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include "lisa/MultiJoinCount.h"
#include "lisa/UniJoinCount.h"
#include "sa/lisa-api.h"
#include "weights/geoda-weight.h"
#include "weights/vector-weight.h"

// Convert a vector<vector<unsigned int>> of per-variable undefined flags into a
// vector<vector<bool>> for the LISA constructors.
static std::vector<std::vector<bool>> to_bool_undefs(const std::vector<std::vector<unsigned int>>& undefs,
                                                     size_t num_vars, size_t num_obs) {
  std::vector<std::vector<bool>> copy_undefs(num_vars);
  for (size_t i = 0; i < num_vars; ++i) {
    copy_undefs[i].resize(num_obs, false);
    if (i < undefs.size() && undefs[i].size() == num_obs) {
      for (size_t j = 0; j < num_obs; ++j) {
        copy_undefs[i][j] = undefs[i][j] == 1;
      }
    }
  }
  return copy_undefs;
}

geoda::LisaResult geoda::local_joincount(const std::vector<double>& data,
                                         const std::vector<std::vector<unsigned int>>& neighbors,
                                         const std::vector<unsigned int>& undefs, double significance_cutoff,
                                         unsigned int perm, int last_seed) {
  LisaResult result;
  result.is_valid = false;

  size_t num_obs = neighbors.size();
  std::vector<bool> copy_undefs(num_obs, false);
  if (undefs.size() == num_obs) {
    for (size_t i = 0; i < num_obs; ++i) {
      copy_undefs[i] = undefs[i] == 1;
    }
  }
  int nCPUs = 1;
  // Use the lookup-table permutation path: the "complete" sampler draws exactly
  // numNeighbors valid candidates and rejects undefined ones, which can loop forever
  // when an observation with a non-zero local join count has undefined neighbors and
  // too few valid candidates remain. The lookup-table path filters undefined
  // observations while applying the permutation, so it cannot hang.
  std::string perm_method = "LookupTable";
  GeoDaWeight* w = new VectorWeight(neighbors);

  UniJoinCount* lisa =
      new UniJoinCount(static_cast<int>(num_obs), w, data, copy_undefs, significance_cutoff, nCPUs,
                       static_cast<int>(perm), perm_method, static_cast<uint64_t>(last_seed));

  if (lisa) {
    set_lisa_content(lisa, result);
    delete lisa;
  }

  delete w;

  return result;
}

geoda::LisaResult geoda::local_multijoincount(const std::vector<std::vector<double>>& data,
                                              const std::vector<std::vector<unsigned int>>& neighbors,
                                              const std::vector<std::vector<unsigned int>>& undefs,
                                              double significance_cutoff, unsigned int perm, int last_seed) {
  LisaResult result;
  result.is_valid = false;

  size_t num_obs = neighbors.size();
  std::vector<std::vector<bool>> copy_undefs = to_bool_undefs(undefs, data.size(), num_obs);

  int nCPUs = 1;
  std::string perm_method = "LookupTable";
  GeoDaWeight* w = new VectorWeight(neighbors);

  MultiJoinCount* lisa =
      new MultiJoinCount(static_cast<int>(num_obs), w, data, copy_undefs, significance_cutoff, nCPUs,
                         static_cast<int>(perm), perm_method, static_cast<uint64_t>(last_seed));

  if (lisa) {
    set_lisa_content(lisa, result);
    delete lisa;
  }

  delete w;

  return result;
}
