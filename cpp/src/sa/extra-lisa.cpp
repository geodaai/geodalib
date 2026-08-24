// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include "lisa/BatchLocalMoran.h"
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

geoda::BatchLisaResult geoda::batch_local_moran(const std::vector<std::vector<double>>& data,
                                                const std::vector<std::vector<unsigned int>>& neighbors,
                                                const std::vector<std::vector<unsigned int>>& undefs,
                                                double significance_cutoff, unsigned int perm, int last_seed) {
  BatchLisaResult result;
  result.is_valid = false;

  size_t num_obs = neighbors.size();
  std::vector<std::vector<bool>> copy_undefs = to_bool_undefs(undefs, data.size(), num_obs);

  int nCPUs = 1;
  GeoDaWeight* w = new VectorWeight(neighbors);

  BatchLocalMoran* lisa =
      new BatchLocalMoran(static_cast<int>(num_obs), w, data, copy_undefs, significance_cutoff, nCPUs,
                          static_cast<int>(perm), static_cast<uint64_t>(last_seed));

  // new() throws on failure; lisa is never null here, so fill the result and
  // release it unconditionally.
  set_batch_lisa_content(lisa, result, data.size());
  delete lisa;

  delete w;

  return result;
}
