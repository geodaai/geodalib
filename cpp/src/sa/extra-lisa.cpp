// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <algorithm>
#include <limits>

#include "lisa/MultiJoinCount.h"
#include "mapping/mapping.h"
#include "sa/lisa-api.h"
#include "weights/geoda-weight.h"
#include "weights/vector-weight.h"

// Convert a vector<vector<unsigned int>> of per-variable undefined flags into a
// vector<vector<bool>> for the LISA constructors. Rows may be shorter than
// num_obs; like MultiJoinCount, only the entries that exist are consulted and
// missing entries stay false.
static std::vector<std::vector<bool>> to_bool_undefs(const std::vector<std::vector<unsigned int>>& undefs,
                                                     size_t num_vars, size_t num_obs) {
  std::vector<std::vector<bool>> copy_undefs(num_vars);
  for (size_t i = 0; i < num_vars; ++i) {
    // Keep rows sparse: a variable with no undef row stays empty (size 0) rather
    // than being zero-filled to num_obs. The LISA constructors merge per-variable
    // flags by iterating only the entries that exist (LISA.cpp:110), so missing
    // entries stay false and short rows are fully tolerated.
    if (i >= undefs.size()) continue;
    size_t row_len = std::min(undefs[i].size(), num_obs);
    copy_undefs[i].resize(row_len);
    for (size_t j = 0; j < row_len; ++j) {
      copy_undefs[i][j] = undefs[i][j] == 1;
    }
  }
  return copy_undefs;
}

geoda::LisaResult geoda::local_multiquantilelisa(const std::vector<int>& k_s, const std::vector<int>& quantile_s,
                                                 const std::vector<std::vector<double>>& data,
                                                 const std::vector<std::vector<unsigned int>>& neighbors,
                                                 const std::vector<std::vector<unsigned int>>& undefs,
                                                 double significance_cutoff, unsigned int perm, int last_seed) {
  LisaResult result;
  result.is_valid = false;

  size_t num_obs = 0;
  if (!data.empty()) num_obs = data[0].size();
  if (k_s.size() != quantile_s.size() || k_s.size() != data.size() || num_obs == 0) {
    return result;
  }
  if (neighbors.size() != num_obs) {
    return result;
  }
  for (size_t i = 0; i < data.size(); ++i) {
    if (data[i].size() != num_obs) {
      return result;
    }
  }

  size_t num_vars = k_s.size();

  // Binarize each variable into its q-th quantile class, then run a multivariate
  // (colocation) join count on the binarized data.
  std::vector<std::vector<double>> bin_data;
  for (size_t i = 0; i < num_vars; ++i) {
    int k = k_s[i];
    int q = quantile_s[i];
    // Match the univariate port: k must be strictly below num_obs, otherwise the
    // class count is not meaningful for this sample (quantile_breaks would also
    // allocate k-1 breakpoints for nothing).
    if (k < 2 || k >= static_cast<int>(num_obs) || q < 1 || q > k) {
      return result;
    }
    // Consult existing entries only; missing entries stay undefined-free so a
    // shorter row still contributes the flags it does carry. Keep the vector
    // sparse (sized to the row, empty when the variable has no undef row):
    // quantile_breaks only consults _undef.size() entries when it is non-zero.
    std::vector<unsigned int> undef_i;
    if (i < undefs.size()) {
      size_t row_len = std::min(undefs[i].size(), num_obs);
      undef_i.resize(row_len);
      for (size_t j = 0; j < row_len; ++j) {
        undef_i[j] = undefs[i][j];
      }
    }

    std::vector<double> breaks = geoda::quantile_breaks(k, data[i], undef_i);

    q = q - 1;
    double break_left = -std::numeric_limits<double>::max();
    double break_right = std::numeric_limits<double>::max();
    if (q == 0) {
      break_right = breaks[q];
    } else if (q == static_cast<int>(breaks.size())) {
      break_left = breaks[q - 1];
    } else {
      break_left = breaks[q - 1];
      break_right = breaks[q];
    }

    std::vector<double> bin(num_obs, 0.0);
    for (size_t j = 0; j < num_obs; ++j) {
      if (data[i][j] >= break_left && data[i][j] < break_right) {
        bin[j] = 1.0;
      }
    }
    bin_data.push_back(bin);
  }

  std::vector<std::vector<bool>> copy_undefs = to_bool_undefs(undefs, num_vars, num_obs);

  int nCPUs = 1;
  std::string perm_method = "LookupTable";
  GeoDaWeight* w = new VectorWeight(neighbors);

  MultiJoinCount* lisa =
      new MultiJoinCount(static_cast<int>(num_obs), w, bin_data, copy_undefs, significance_cutoff, nCPUs,
                         static_cast<int>(perm), perm_method, static_cast<uint64_t>(last_seed));

  if (lisa) {
    set_lisa_content(lisa, result);
    delete lisa;
  }

  delete w;

  return result;
}
