// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include "lisa/MultiJoinCount.h"
#include "lisa/UniJoinCount.h"
#include "sa/lisa-api.h"
#include "weights/geoda-weight.h"
#include "weights/vector-weight.h"

#include <algorithm>
#include <limits>
#include <utility>
#include <vector>

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
  comp_nbrs.resize(num_valid);
  for (size_t i = 0; i < neighbors.size(); ++i) {
    unsigned int c = orig_to_comp[i];
    if (c == kUndefined) continue;
    const std::vector<unsigned int>& nbrs = neighbors[i];
    for (unsigned int nb : nbrs) {
      if (nb >= orig_to_comp.size()) return false;
      unsigned int comp_nb = orig_to_comp[nb];
      if (comp_nb != kUndefined) {
        comp_nbrs[c].push_back(comp_nb);
      }
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

  // The complementary bivariate no-colocation case (every observation has exactly
  // one variable = 1) is served by local_bijoincount, not the multivariate
  // colocation join count. Match that contract and reject only that case; other
  // bivariate inputs (e.g. with (0,0) observations) are forwarded to
  // MultiJoinCount.
  if (num_vars == 2) {
    size_t ones0 = 0, ones1 = 0;
    for (size_t c = 0; c < num_valid; ++c) {
      if (comp_data[0][c] == 1.0) ++ones0;
      if (comp_data[1][c] == 1.0) ++ones1;
    }
    if (ones0 + ones1 == num_valid) {
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
