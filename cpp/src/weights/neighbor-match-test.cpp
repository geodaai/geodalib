// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include "geometry/geometry.h"
#include "weights/weights.h"

namespace {

double combinatorial(unsigned int n, unsigned int k) {
  double r = 1.0, s = 1.0;
  unsigned int kk = k > n / 2 ? k : n - k;
  for (unsigned int i = n; i > kk; --i) r *= i;
  for (unsigned int i = (n - kk); i > 0; --i) s *= i;
  return r / s;
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
  const std::string m = to_lower(method);
  if (m == "raw") return data;
  return standardize_variable(data);  // only 'raw' and 'standardize' supported in this port
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
  bool manhattan = to_lower(dist_type) == "manhattan";

  std::vector<std::vector<unsigned int>> spatial_nbrs = geoda::knearest_neighbors(geoms, k);

  std::vector<std::vector<double>> scaled;
  for (const auto& var : data) {
    scaled.push_back(scale_variable(var, scale_method));
  }

  size_t num_vars = scaled.size();
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
    std::sort(dists.begin(), dists.end(),
              [](const std::pair<double, unsigned int>& x, const std::pair<double, unsigned int>& y) {
                return x.first < y.first;
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

  std::vector<double> pval_dict(k, -1.0);
  unsigned int rows = static_cast<unsigned int>(num_obs);
  for (unsigned int v = 1; v < k; ++v) {
    if (rows > k) {
      pval_dict[v] = combinatorial(k, v) * combinatorial(rows - k - 1, k - v) / combinatorial(rows - 1, k);
    }
  }
  std::vector<double> val_p(num_obs, -1.0);
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
