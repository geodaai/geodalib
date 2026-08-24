// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <algorithm>
#include <cctype>
#include <cmath>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "geometry/geometry.h"
#include "weights/weights.h"

namespace {

// log|C(n, k)| computed via lgamma so the intermediate products never overflow
// for large n/k (e.g. 300 observations with k = 150). C(n, k) = 0 when k > n.
double ln_combination(unsigned int n, unsigned int k) {
  if (k > n) return -std::numeric_limits<double>::infinity();
  return std::lgamma(n + 1.0) - std::lgamma(k + 1.0) - std::lgamma(n - k + 1.0);
}

std::string to_lower(const std::string& s) {
  std::string r = s;
  for (char& c : r) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return r;
}

std::vector<double> standardize_variable(const std::vector<double>& data) {
  double sum = 0.0;
  for (auto v : data) sum += v;
  double mean = data.empty() ? 0.0 : sum / data.size();
  double var = 0.0;
  for (auto v : data) var += (v - mean) * (v - mean);
  double sd = data.size() > 1 ? std::sqrt(var / (data.size() - 1)) : 0.0;
  std::vector<double> r(data.size());
  for (size_t i = 0; i < data.size(); ++i) r[i] = (sd > 0) ? (data[i] - mean) / sd : 0.0;
  return r;
}

std::vector<double> scale_variable(const std::vector<double>& data, const std::string& method) {
  // neighbor_match_test validates the method against the documented
  // 'raw'/'standardize' values before invoking this helper.
  if (method == "raw") return data;
  return standardize_variable(data);
}

double attribute_distance(const std::vector<double>& a, const std::vector<double>& b, bool manhattan) {
  double sum = 0.0;
  for (size_t j = 0; j < a.size(); ++j) {
    double d = a[j] - b[j];
    sum += manhattan ? std::abs(d) : d * d;
  }
  return manhattan ? sum : std::sqrt(sum);
}

}  // namespace

std::vector<std::vector<double>> geoda::neighbor_match_test(const GeometryCollection& geoms, unsigned int k,
                                                            const std::vector<std::vector<double>>& data,
                                                            const std::string& scale_method,
                                                            const std::string& dist_type, bool is_mile) {
  size_t num_obs = geoms.size();

  // Only the documented scaling methods and distance metrics are supported;
  // anything else (including a typo) must be rejected instead of silently
  // treated as 'standardize' or 'euclidean', which would change the result.
  const std::string scale = to_lower(scale_method);
  const std::string dist = to_lower(dist_type);
  if (scale != "raw" && scale != "standardize") {
    return {};
  }
  if (dist != "euclidean" && dist != "manhattan") {
    return {};
  }
  bool manhattan = dist == "manhattan";

  // k >= num_obs is impossible (an observation can have at most num_obs - 1
  // neighbors), and the hypergeometric model below draws k neighbors from a
  // universe of num_obs - 1 candidates. Reject it before the spatial search so
  // the p-value table is never left with non-probability sentinels.
  if (num_obs == 0 || k == 0 || k >= num_obs) {
    return {};
  }

  // Non-finite values produce NaN distances whose comparator violates the
  // strict weak ordering std::sort requires below (and a single NaN collapses a
  // standardized variable to zeros). Reject them before building any distances.
  for (const auto& var : data) {
    for (double v : var) {
      if (!std::isfinite(v)) {
        return {};
      }
    }
  }

  // Spatial k-NN with an explicit (distance, index) tie-break. boost's R-tree
  // nearest query (used by knearest_neighbors) does not define a stable order
  // for equidistant candidates, so the spatial neighbor selection is computed
  // directly here to keep the overlap cardinality deterministic across runs.
  std::vector<std::vector<unsigned int>> spatial_nbrs(num_obs);
  std::vector<std::vector<double>> centroids = geoms.get_centroids();
  for (size_t i = 0; i < num_obs; ++i) {
    std::vector<std::pair<double, unsigned int>> dists;
    for (size_t j = 0; j < num_obs; ++j) {
      if (i == j) continue;
      const std::vector<double>& c1 = centroids[i];
      const std::vector<double>& c2 = centroids[j];
      double dx = c1[0] - c2[0], dy = c1[1] - c2[1];
      dists.emplace_back(std::sqrt(dx * dx + dy * dy), static_cast<unsigned int>(j));
    }
    std::sort(dists.begin(), dists.end(),
              [](const std::pair<double, unsigned int>& x, const std::pair<double, unsigned int>& y) {
                if (x.first != y.first) return x.first < y.first;
                return x.second < y.second;
              });
    for (unsigned int m = 0; m < k; ++m) {
      spatial_nbrs[i].push_back(dists[m].second);
    }
  }

  std::vector<std::vector<double>> scaled;
  for (const auto& var : data) {
    // scale is the normalized (lowercased) value that passed validation above;
    // the raw input could be a valid case variant like "RAW".
    scaled.push_back(scale_variable(var, scale));
  }

  size_t num_vars = scaled.size();
  if (num_vars == 0) {
    return {};
  }
  // every variable must hold one value per observation
  for (size_t v = 0; v < num_vars; ++v) {
    if (scaled[v].size() != num_obs) {
      return {};
    }
  }

  std::vector<std::vector<unsigned int>> var_nbrs(num_obs);
  for (size_t i = 0; i < num_obs; ++i) {
    std::vector<std::pair<double, unsigned int>> dists;
    for (size_t j = 0; j < num_obs; ++j) {
      if (i == j) continue;
      std::vector<double> a(num_vars), b(num_vars);
      for (size_t v = 0; v < num_vars; ++v) {
        a[v] = scaled[v][i];
        b[v] = scaled[v][j];
      }
      dists.emplace_back(attribute_distance(a, b, manhattan), static_cast<unsigned int>(j));
    }
    // tie-break equal distances by the candidate index so the neighbor choice
    // (and therefore the overlap cardinality) is deterministic across builds
    std::sort(dists.begin(), dists.end(),
              [](const std::pair<double, unsigned int>& x, const std::pair<double, unsigned int>& y) {
                if (x.first != y.first) return x.first < y.first;
                return x.second < y.second;
              });
    unsigned int take = k < dists.size() ? k : static_cast<unsigned int>(dists.size());
    for (unsigned int m = 0; m < take; ++m) {
      var_nbrs[i].push_back(dists[m].second);
    }
  }

  std::vector<double> val_cnbrs(num_obs, 0.0);
  for (size_t i = 0; i < num_obs; ++i) {
    int common = 0;
    for (auto s : spatial_nbrs[i]) {
      if (std::find(var_nbrs[i].begin(), var_nbrs[i].end(), s) != var_nbrs[i].end()) {
        ++common;
      }
    }
    val_cnbrs[i] = static_cast<double>(common);
  }

  // Hypergeometric P(X = v) for the overlap of two k-sized neighbor draws from
  // (rows - 1) candidates (self excluded): C(k, v) * C(rows - 1 - k, k - v) /
  // C(rows - 1, k). One entry per cardinality 0..k; combinations outside the
  // hypergeometric support have probability 0.
  // With k < num_obs guaranteed above, universe = num_obs - 1 >= k so the
  // combinations below never receive invalid arguments and the endpoint
  // cardinalities (v = 0 and v = k) always get real probabilities.
  std::vector<double> pval_dict(k + 1, 0.0);
  unsigned int universe = static_cast<unsigned int>(num_obs) - 1;
  for (unsigned int v = 0; v <= k; ++v) {
    if (k - v <= universe - k) {
      pval_dict[v] = std::exp(ln_combination(k, v) + ln_combination(universe - k, k - v) -
                              ln_combination(universe, k));
    }
  }
  // Every cardinality is in [0, k] and maps to a real pval_dict entry above, so
  // the NaN default below only guards against an unexpected out-of-range
  // cardinality; -1 is not a probability and must not be returned to callers.
  std::vector<double> val_p(num_obs, std::numeric_limits<double>::quiet_NaN());
  for (size_t i = 0; i < num_obs; ++i) {
    unsigned int c = static_cast<unsigned int>(val_cnbrs[i]);
    if (c < pval_dict.size()) {
      val_p[i] = pval_dict[c];
    }
  }

  std::vector<std::vector<double>> result;
  result.push_back(val_cnbrs);
  result.push_back(val_p);
  return result;
}
