// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <cmath>

#include "lisa/UniLocalMoran.h"
#include "sa/lisa-api.h"
#include "weights/geoda-weight.h"
#include "weights/vector-weight.h"

namespace {

// Empirical Bayes rate standardization (rateStandardizeEB), inlined here so the
// lisa WASM target does not depend on the mapping/rates module.
std::vector<double> eb_rate_standardize(const std::vector<double>& P, const std::vector<double>& E,
                                        const std::vector<unsigned int>& undefs) {
  size_t obs = P.size();
  std::vector<double> results(obs, 0.0);
  std::vector<double> p(obs, 0.0);

  double sP = 0.0, sE = 0.0;
  for (size_t i = 0; i < obs; i++) {
    if (i < undefs.size() && undefs[i] == 1) continue;
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
    // an observation is undefined only when a flag exists and is set; a missing
    // or shorter undefs vector means every observation is valid
    if (i >= undefs.size() || undefs[i] == 0) {
      gamma += P[i] * ((p[i] - b_hat) * (p[i] - b_hat));
      obs_valid += 1.0;
    }
  }

  double a = (gamma / sP) - (b_hat / (sP / obs_valid));
  const double a_hat = a > 0 ? a : 0.0;

  for (size_t i = 0; i < obs; i++) {
    if (i >= undefs.size() || undefs[i] == 0) {
      const double se = P[i] > 0 ? sqrt(a_hat + b_hat / P[i]) : 0.0;
      results[i] = se > 0 ? (p[i] - b_hat) / se : 0.0;
    }
  }

  return results;
}

}  // namespace

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
  std::string perm_method = "complete";
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
