// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

#include "lisa/MultiJoinCount.h"
#include "lisa/UniJoinCount.h"
#include "lisa/UniLocalMoran.h"
#include "mapping/mapping.h"
#include "lisa/BatchLocalMoran.h"
#include "sa/lisa-api.h"
#include "weights/geoda-weight.h"
#include "weights/vector-weight.h"

namespace {

// Convert a vector<vector<unsigned int>> of per-variable undefined flags into a
// vector<vector<bool>> for the LISA constructors. Rows may be shorter than
// num_obs; like MultiJoinCount, only the entries that exist are consulted and
// missing entries stay false.
std::vector<std::vector<bool>> to_bool_undefs(const std::vector<std::vector<unsigned int>>& undefs, size_t num_vars,
                                              size_t num_obs) {
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

// Empirical Bayes rate standardization (rateStandardizeEB), inlined here so the
// lisa WASM target does not depend on the mapping/rates module.
std::vector<double> eb_rate_standardize(const std::vector<double>& P, const std::vector<double>& E,
                                        const std::vector<unsigned int>& undefs) {
  size_t obs = P.size();
  std::vector<double> results(obs, 0.0);
  std::vector<double> p(obs, 0.0);

  // A missing or shorter undefs vector means every observation is valid; only
  // a full-length undefs vector is authoritative, matching local_moran_eb().
  const bool has_undefs = undefs.size() == obs;
  double sP = 0.0, sE = 0.0;
  for (size_t i = 0; i < obs; i++) {
    if (has_undefs && undefs[i] == 1) continue;
    if (P[i] == 0.0) {
      p[i] = 0.0;
    } else {
      sP += P[i];
      sE += E[i];
      p[i] = E[i] / P[i];
    }
  }

  if (sP == 0.0) return results;

  const double b_hat = sE / sP;

  double obs_valid = 0.0;
  double gamma = 0.0;
  for (size_t i = 0; i < obs; i++) {
    // an observation is undefined only when a full-length undefs flag is set
    if (!has_undefs || undefs[i] == 0) {
      gamma += P[i] * ((p[i] - b_hat) * (p[i] - b_hat));
      obs_valid += 1.0;
    }
  }

  double a = (gamma / sP) - (b_hat / (sP / obs_valid));
  const double a_hat = a > 0 ? a : 0.0;

  for (size_t i = 0; i < obs; i++) {
    if (!has_undefs || undefs[i] == 0) {
      const double se = P[i] > 0 ? sqrt(a_hat + b_hat / P[i]) : 0.0;
      results[i] = se > 0 ? (p[i] - b_hat) / se : 0.0;
    }
  }

  return results;
}

// Map original observation ids onto a compacted dataset that contains only the
// defined observations. comp_to_orig[c] gives the original id of compacted id c;
// orig_to_comp[i] is UINT_MAX for undefined observations.
static std::vector<unsigned int> build_compact_map(size_t num_obs,
                                                   const std::vector<unsigned int>& undefs,
                                                   std::vector<size_t>& comp_to_orig) {
  const unsigned int kUndefined = std::numeric_limits<unsigned int>::max();
  std::vector<unsigned int> orig_to_comp(num_obs, kUndefined);
  comp_to_orig.clear();
  comp_to_orig.reserve(num_obs);
  for (size_t i = 0; i < num_obs; ++i) {
    bool is_undef = i < undefs.size() && undefs[i] == 1;
    if (!is_undef) {
      orig_to_comp[i] = static_cast<unsigned int>(comp_to_orig.size());
      comp_to_orig.push_back(i);
    }
  }
  return orig_to_comp;
}

// Compact the neighbors of the defined observations: undefined neighbors are
// dropped and the remaining ones are remapped to their compacted ids. Returns
// false when a neighbor ID is out of range for the input (a negative JS index
// arrives as a huge unsigned value), so the caller can reject the input instead
// of indexing past the map.
static bool compact_neighbors(const std::vector<std::vector<unsigned int>>& neighbors,
                              const std::vector<unsigned int>& orig_to_comp, size_t num_valid,
                              std::vector<std::vector<unsigned int>>& comp_nbrs) {
  const unsigned int kUndefined = std::numeric_limits<unsigned int>::max();
  // Validate every neighbor ID up front, including rows whose source observation
  // is undefined (they are skipped below but must still be well-formed), so a
  // malformed adjacency list is rejected rather than partially analyzed.
  for (size_t i = 0; i < neighbors.size(); ++i) {
    const std::vector<unsigned int>& nbrs = neighbors[i];
    for (unsigned int nb : nbrs) {
      if (nb >= orig_to_comp.size()) return false;
    }
  }
  comp_nbrs.resize(num_valid);
  for (size_t i = 0; i < neighbors.size(); ++i) {
    unsigned int c = orig_to_comp[i];
    if (c == kUndefined) continue;
    const std::vector<unsigned int>& nbrs = neighbors[i];
    for (unsigned int nb : nbrs) {
      unsigned int comp_nb = orig_to_comp[nb];
      // Drop undefined neighbors, self-loops, and duplicates. The join-count
      // loops skip self, but VectorWeight::GetNbrStats excludes self while
      // GetNbrSize includes it, so a self entry would make the lookup-table
      // permutation too small and read past it; duplicate-heavy rows can make
      // the permutation sampler loop forever drawing unique candidates it cannot
      // produce. A weights adjacency list should contain neither, so filter both
      // rather than propagate them.
      if (comp_nb == kUndefined || comp_nb == c) continue;
      bool seen = false;
      for (size_t k = 0; k < comp_nbrs[c].size(); ++k) {
        if (comp_nbrs[c][k] == comp_nb) {
          seen = true;
          break;
        }
      }
      if (!seen) comp_nbrs[c].push_back(comp_nb);
    }
  }
  return true;
}

// Expand a result computed on the compacted dataset back to the original
// observation order. Undefined observations keep the standard undefined markers
// (significance category 6, zero statistic/lag) and their original neighbor count.
static void expand_result(geoda::LisaResult& result, size_t num_obs,
                          const std::vector<size_t>& comp_to_orig,
                          const std::vector<std::vector<unsigned int>>& neighbors) {
  std::vector<double> sig_local(num_obs, 0.0);
  std::vector<int> sig_cat(num_obs, 6);
  std::vector<int> cluster(num_obs, 0);
  std::vector<double> lag(num_obs, 0.0);
  std::vector<double> lisa(num_obs, 0.0);
  std::vector<int> nn(num_obs, 0);

  // Result vectors may be empty when the wrapper bails before running the
  // analysis (e.g. a rejected input); only copy from a populated result and
  // leave every observation at the undefined marker in that case.
  size_t num_comp = std::min(comp_to_orig.size(), result.sig_local_vec.size());
  for (size_t c = 0; c < num_comp; ++c) {
    size_t o = comp_to_orig[c];
    sig_local[o] = result.sig_local_vec[c];
    sig_cat[o] = result.sig_cat_vec[c];
    cluster[o] = result.cluster_vec[c];
    lag[o] = result.lag_vec[c];
    lisa[o] = result.lisa_vec[c];
    nn[o] = result.nn_vec[c];
  }
  // Neighbor counts are reported for every observation, matching the spatial
  // weights structure rather than the compacted analysis set.
  for (size_t i = 0; i < num_obs; ++i) {
    nn[i] = static_cast<int>(neighbors[i].size());
  }

  result.sig_local_vec = std::move(sig_local);
  result.sig_cat_vec = std::move(sig_cat);
  result.cluster_vec = std::move(cluster);
  result.lag_vec = std::move(lag);
  result.lisa_vec = std::move(lisa);
  result.nn_vec = std::move(nn);
}

}  // namespace

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

geoda::LisaResult geoda::local_moran_eb(const std::vector<double>& event_data,
                                        const std::vector<double>& base_data,
                                        const std::vector<std::vector<unsigned int>>& neighbors,
                                        const std::vector<unsigned int>& undefs, double significance_cutoff,
                                        unsigned int perm, int last_seed) {
  LisaResult result;
  result.is_valid = false;

  size_t num_obs = neighbors.size();

  // Empirical Bayes standardization of the event/base rates.
  std::vector<double> smoothed = eb_rate_standardize(base_data, event_data, undefs);

  std::vector<bool> copy_undefs(num_obs, false);
  if (undefs.size() == num_obs) {
    for (size_t i = 0; i < num_obs; ++i) {
      copy_undefs[i] = undefs[i] == 1;
    }
  }

  int nCPUs = 1;
  // Use the same permutation path as local_moran() (avoids per-observation
  // permutation generation and is much faster for large N/perm, especially in WASM).
  std::string perm_method = "LookupTable";
  GeoDaWeight* w = new VectorWeight(neighbors);

  UniLocalMoran* lisa =
      new UniLocalMoran(static_cast<int>(num_obs), w, smoothed, copy_undefs, significance_cutoff, nCPUs,
                        static_cast<int>(perm), perm_method, static_cast<uint64_t>(last_seed));

  if (lisa) {
    set_lisa_content(lisa, result);
    delete lisa;
  }

  delete w;

  return result;
}

geoda::LisaResult geoda::local_joincount(const std::vector<double>& data,
                                         const std::vector<std::vector<unsigned int>>& neighbors,
                                         const std::vector<unsigned int>& undefs, double significance_cutoff,
                                         unsigned int perm, int last_seed) {
  LisaResult result;
  result.is_valid = false;

  size_t num_obs = neighbors.size();

  // Compact the dataset to the defined observations. The lookup-table permutation
  // path builds one permutation table over the whole population and only skips an
  // undefined observation after the table is fixed, so an undefined draw still
  // occupies a slot (contributing 0 to the permuted join count) and biases
  // p-values relative to the valid-only "complete" path. Removing undefined
  // observations up front makes every permutation candidate valid while keeping
  // the no-hang property of the lookup-table sampler.
  std::vector<size_t> comp_to_orig;
  std::vector<unsigned int> orig_to_comp = build_compact_map(num_obs, undefs, comp_to_orig);
  size_t num_valid = comp_to_orig.size();

  // An empty (or all-undefined) input compacts to zero observations. Skipping the
  // weight construction keeps VectorWeight::GetNbrStats from indexing an empty
  // neighbor-count vector (nnbrs_array[num_obs / 2 - 1] with num_obs == 0 reads
  // before the buffer). Every observation is reported as undefined, matching the
  // "no defined observations" contract; is_valid stays false.
  if (num_valid == 0) {
    expand_result(result, num_obs, comp_to_orig, neighbors);
    return result;
  }

  // Mismatched input (data not aligned with the neighbor list) would read past
  // the data buffer while compacting; reject with the invalid contract.
  if (data.size() != num_obs) {
    expand_result(result, num_obs, comp_to_orig, neighbors);
    return result;
  }

  std::vector<double> comp_data(num_valid);
  for (size_t c = 0; c < num_valid; ++c) {
    comp_data[c] = data[comp_to_orig[c]];
  }
  // This is a binary (0/1) join count: UniJoinCount treats any positive value as
  // 1-valued and adds the raw neighbor value to the lag, so non-binary data would
  // return meaningless counts. Reject values that are not exactly 0 or 1.
  for (size_t c = 0; c < num_valid; ++c) {
    if (comp_data[c] != 0.0 && comp_data[c] != 1.0) {
      expand_result(result, num_obs, comp_to_orig, neighbors);
      return result;
    }
  }
  std::vector<std::vector<unsigned int>> comp_nbrs;
  if (!compact_neighbors(neighbors, orig_to_comp, num_valid, comp_nbrs)) {
    // A neighbor ID out of range would have read past the compaction map;
    // reject with the invalid contract.
    expand_result(result, num_obs, comp_to_orig, neighbors);
    return result;
  }

  int nCPUs = 1;
  // After compaction every observation is defined, so the lookup-table
  // permutation cannot hang and samples only valid candidates.
  std::string perm_method = "LookupTable";
  GeoDaWeight* w = new VectorWeight(comp_nbrs);
  std::vector<bool> comp_undefs(num_valid, false);

  UniJoinCount* lisa =
      new UniJoinCount(static_cast<int>(num_valid), w, comp_data, comp_undefs, significance_cutoff, nCPUs,
                       static_cast<int>(perm), perm_method, static_cast<uint64_t>(last_seed));

  if (lisa) {
    set_lisa_content(lisa, result);
    delete lisa;
  }

  delete w;

  expand_result(result, num_obs, comp_to_orig, neighbors);

  return result;
}

geoda::LisaResult geoda::local_multijoincount(const std::vector<std::vector<double>>& data,
                                              const std::vector<std::vector<unsigned int>>& neighbors,
                                              const std::vector<std::vector<unsigned int>>& undefs,
                                              double significance_cutoff, unsigned int perm, int last_seed) {
  LisaResult result;
  result.is_valid = false;

  size_t num_obs = neighbors.size();
  size_t num_vars = data.size();

  std::vector<size_t> comp_to_orig;

  // Reject an empty or single-variable set or variables misaligned with the
  // neighbor list: with zero variables MultiJoinCount defaults zz to 1 for every
  // observation and reports a bogus valid result; a short variable would read
  // past its buffer; and a single variable is not a multivariate colocation
  // (pygeoda rejects n_vars <= 1).
  if (num_vars <= 1) {
    expand_result(result, num_obs, comp_to_orig, neighbors);
    return result;
  }
  for (size_t v = 0; v < num_vars; ++v) {
    if (data[v].size() != num_obs) {
      expand_result(result, num_obs, comp_to_orig, neighbors);
      return result;
    }
  }

  // Merge per-variable undefined flags the same way MultiJoinCount does (an
  // observation is undefined if any of its variables is undefined), then compact
  // to the defined observations so the lookup-table permutation population is
  // valid-only (see local_joincount for the rationale).
  std::vector<unsigned int> merged_undefs(num_obs, 0);
  for (size_t i = 0; i < num_obs; ++i) {
    for (size_t v = 0; v < num_vars; ++v) {
      if (v < undefs.size() && i < undefs[v].size() && undefs[v][i] == 1) {
        merged_undefs[i] = 1;
        break;
      }
    }
  }

  std::vector<unsigned int> orig_to_comp = build_compact_map(num_obs, merged_undefs, comp_to_orig);
  size_t num_valid = comp_to_orig.size();

  // Same empty-input contract as local_joincount: never construct VectorWeight
  // over zero observations (GetNbrStats reads before an empty buffer).
  if (num_valid == 0) {
    expand_result(result, num_obs, comp_to_orig, neighbors);
    return result;
  }

  std::vector<std::vector<double>> comp_data(num_vars, std::vector<double>(num_valid, 0.0));
  for (size_t v = 0; v < num_vars; ++v) {
    for (size_t c = 0; c < num_valid; ++c) {
      comp_data[v][c] = data[v][comp_to_orig[c]];
    }
  }

  // MultiJoinCount multiplies raw values into its integer zz, so non-binary data
  // would produce invalid colocations and statistics; reject values that are not
  // exactly 0 or 1.
  for (size_t v = 0; v < num_vars; ++v) {
    for (size_t c = 0; c < num_valid; ++c) {
      if (comp_data[v][c] != 0.0 && comp_data[v][c] != 1.0) {
        expand_result(result, num_obs, comp_to_orig, neighbors);
        return result;
      }
    }
  }

  std::vector<std::vector<unsigned int>> comp_nbrs;
  if (!compact_neighbors(neighbors, orig_to_comp, num_valid, comp_nbrs)) {
    // A neighbor ID out of range would have read past the compaction map;
    // reject with the invalid contract.
    expand_result(result, num_obs, comp_to_orig, neighbors);
    return result;
  }

  int nCPUs = 1;
  std::string perm_method = "LookupTable";
  GeoDaWeight* w = new VectorWeight(comp_nbrs);
  std::vector<std::vector<bool>> comp_undefs(num_vars, std::vector<bool>(num_valid, false));

  MultiJoinCount* lisa =
      new MultiJoinCount(static_cast<int>(num_valid), w, comp_data, comp_undefs, significance_cutoff, nCPUs,
                         static_cast<int>(perm), perm_method, static_cast<uint64_t>(last_seed));

  if (lisa) {
    set_lisa_content(lisa, result);
    delete lisa;
  }

  delete w;

  expand_result(result, num_obs, comp_to_orig, neighbors);

  return result;
}
