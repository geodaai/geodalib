// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <algorithm>
#include <cctype>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "clustering-api.h"
#include "make_spatial.h"
#include "schc_wrapper.h"
#include "redcap_wrapper.h"
#include "azp_wrapper.h"
#include "maxp_wrapper.h"
#include "spatial_validation.h"
#include "geofeature.h"
#include "joincount_ratio.h"
#include "cluster-utils.h"
#include "pam.h"
#include "../data/data.h"
#include "../weights/vector-weight.h"

namespace {

std::vector<std::vector<double>> scale_data(const std::vector<std::vector<double>>& data,
                                            const std::string& scale_method) {
  if (data.empty()) {
    return {};
  }
  // Every variable must hold one value per observation, regardless of scaling
  // method. standardize_data_wasm bails out silently on a length mismatch, and
  // the "raw" path would hand mismatched lengths straight to the wrappers which
  // index data[i][r] out of bounds, so reject the mismatch up front.
  const size_t num_obs = data[0].size();
  for (size_t i = 0; i < data.size(); ++i) {
    if (data[i].size() != num_obs) {
      throw std::invalid_argument(
          "scale_data: each variable must have the same number of observations");
    }
  }
  if (scale_method == "raw") {
    return data;
  }
  std::vector<std::vector<double>> scaled = data;
  std::vector<unsigned int> undef(num_obs, 0);
  for (size_t i = 0; i < scaled.size(); ++i) {
    scaled[i] = geoda::standardize_data_wasm(scaled[i], undef);
  }
  return scaled;
}

}  // namespace

std::vector<std::vector<int>> geoda::schc(unsigned int k, const std::vector<std::vector<unsigned int>>& neighbors,
                                          const std::vector<std::vector<double>>& data,
                                          const std::string& scale_method, const std::string& linkage_method,
                                          const std::string& distance_method, const std::vector<double>& bound_vals,
                                          double min_bound) {
  GeoDaWeight* w = new VectorWeight(neighbors);
  unsigned int method = 0;
  if (linkage_method == "complete") {
    method = 1;
  } else if (linkage_method == "average") {
    method = 2;
  } else if (linkage_method == "ward") {
    method = 3;
  }

  std::vector<std::vector<double>> scaled = scale_data(data, scale_method);

  schc_wrapper wrapper(k, w, scaled, method, distance_method, bound_vals, min_bound, 0);
  std::vector<std::vector<int>> result = wrapper.GetClusters();
  delete w;
  return result;
}

std::vector<std::vector<int>> geoda::redcap(unsigned int k, const std::vector<std::vector<unsigned int>>& neighbors,
                                            const std::vector<std::vector<double>>& data,
                                            const std::string& scale_method, const std::string& redcap_method,
                                            const std::string& distance_method, const std::vector<double>& bound_vals,
                                            double min_bound) {
  GeoDaWeight* w = new VectorWeight(neighbors);
  unsigned int method = 0;
  if (redcap_method == "fullorder-completelinkage") {
    method = 1;
  } else if (redcap_method == "fullorder-averagelinkage") {
    method = 2;
  } else if (redcap_method == "fullorder-singlelinkage") {
    method = 3;
  } else if (redcap_method == "fullorder-wardlinkage") {
    method = 4;
  }

  std::vector<std::vector<double>> scaled = scale_data(data, scale_method);

  redcap_wrapper rc(k, w, scaled, method, distance_method, bound_vals, min_bound, 1234567, 1, 0);
  std::vector<std::vector<int>> result = rc.GetClusters();
  delete w;
  return result;
}

std::vector<std::vector<int>> geoda::skater(unsigned int k, const std::vector<std::vector<unsigned int>>& neighbors,
                                            const std::vector<std::vector<double>>& data,
                                            const std::string& scale_method, const std::string& distance_method,
                                            const std::vector<double>& bound_vals, double min_bound) {
  return geoda::redcap(k, neighbors, data, scale_method, "firstorder-singlelinkage", distance_method, bound_vals,
                       min_bound);
}

std::vector<std::vector<int>> geoda::azp_greedy(int p, const std::vector<std::vector<unsigned int>>& neighbors,
                                                const std::vector<std::vector<double>>& data, int inits,
                                                const std::string& distance_method, int rnd_seed) {
  GeoDaWeight* w = new VectorWeight(neighbors);
  std::vector<std::pair<double, std::vector<double>>> min_bounds, max_bounds;
  std::vector<int> init_regions;
  std::vector<std::vector<double>> scaled = scale_data(data, "standardize");
  azp_greedy_wrapper azp(p, w, scaled, inits, min_bounds, max_bounds, init_regions, distance_method, rnd_seed, 0);
  std::vector<std::vector<int>> result = azp.GetClusters();
  delete w;
  return result;
}

std::vector<std::vector<int>> geoda::maxp_greedy(const std::vector<std::vector<unsigned int>>& neighbors,
                                                 const std::vector<std::vector<double>>& data, int iterations,
                                                 const std::string& distance_method, int rnd_seed) {
  GeoDaWeight* w = new VectorWeight(neighbors);
  std::vector<std::pair<double, std::vector<double>>> min_bounds, max_bounds;
  std::vector<int> init_regions;
  // maxp_wrapper expects standardized data (matching azp_greedy and pygeoda);
  // forwarding raw data would make results depend on variable scales.
  std::vector<std::vector<double>> scaled = scale_data(data, "standardize");
  maxp_greedy_wrapper mp(w, scaled, iterations, min_bounds, max_bounds, init_regions, distance_method, rnd_seed, 1, 0);
  std::vector<std::vector<int>> result = mp.GetClusters();
  delete w;
  return result;
}

std::vector<std::vector<int>> geoda::azp_sa(int p, const std::vector<std::vector<unsigned int>>& neighbors,
                                            const std::vector<std::vector<double>>& data, int inits,
                                            double cooling_rate, int sa_maxit, const std::string& distance_method,
                                            int rnd_seed) {
  GeoDaWeight* w = new VectorWeight(neighbors);
  std::vector<std::pair<double, std::vector<double>>> min_bounds, max_bounds;
  std::vector<int> init_regions;
  // azp_wrapper expects standardized data (matching azp_greedy); forwarding raw
  // data would skew the objective function and make results scale-dependent.
  std::vector<std::vector<double>> scaled = scale_data(data, "standardize");
  azp_sa_wrapper azp(p, w, scaled, inits, cooling_rate, sa_maxit, min_bounds, max_bounds, init_regions,
                     distance_method, rnd_seed, 0);
  std::vector<std::vector<int>> result = azp.GetClusters();
  delete w;
  return result;
}

std::vector<std::vector<int>> geoda::azp_tabu(int p, const std::vector<std::vector<unsigned int>>& neighbors,
                                              const std::vector<std::vector<double>>& data, int inits,
                                              int tabu_length, int conv_tabu, const std::string& distance_method,
                                              int rnd_seed) {
  GeoDaWeight* w = new VectorWeight(neighbors);
  std::vector<std::pair<double, std::vector<double>>> min_bounds, max_bounds;
  std::vector<int> init_regions;
  // azp_wrapper expects standardized data (matching azp_greedy).
  std::vector<std::vector<double>> scaled = scale_data(data, "standardize");
  azp_tabu_wrapper azp(p, w, scaled, inits, tabu_length, conv_tabu, min_bounds, max_bounds, init_regions,
                       distance_method, rnd_seed, 0);
  std::vector<std::vector<int>> result = azp.GetClusters();
  delete w;
  return result;
}

std::vector<std::vector<int>> geoda::maxp_sa(const std::vector<std::vector<unsigned int>>& neighbors,
                                             const std::vector<std::vector<double>>& data, int iterations,
                                             double cooling_rate, int sa_maxit, const std::string& distance_method,
                                             int rnd_seed) {
  GeoDaWeight* w = new VectorWeight(neighbors);
  std::vector<std::pair<double, std::vector<double>>> min_bounds, max_bounds;
  std::vector<int> init_regions;
  // maxp_wrapper expects standardized data (matching maxp_greedy).
  std::vector<std::vector<double>> scaled = scale_data(data, "standardize");
  maxp_sa_wrapper mp(w, scaled, iterations, cooling_rate, sa_maxit, min_bounds, max_bounds, init_regions,
                     distance_method, rnd_seed, 1, 0);
  std::vector<std::vector<int>> result = mp.GetClusters();
  delete w;
  return result;
}

std::vector<std::vector<int>> geoda::maxp_tabu(const std::vector<std::vector<unsigned int>>& neighbors,
                                               const std::vector<std::vector<double>>& data, int iterations,
                                               int tabu_length, int conv_tabu, const std::string& distance_method,
                                               int rnd_seed) {
  GeoDaWeight* w = new VectorWeight(neighbors);
  std::vector<std::pair<double, std::vector<double>>> min_bounds, max_bounds;
  std::vector<int> init_regions;
  // maxp_wrapper expects standardized data (matching maxp_greedy).
  std::vector<std::vector<double>> scaled = scale_data(data, "standardize");
  maxp_tabu_wrapper mp(w, scaled, iterations, tabu_length, conv_tabu, min_bounds, max_bounds, init_regions,
                       distance_method, rnd_seed, 1, 0);
  std::vector<std::vector<int>> result = mp.GetClusters();
  delete w;
  return result;
}

std::vector<std::vector<int>> geoda::pam(unsigned int k, const std::vector<std::vector<double>>& data,
                                         const std::string& distance_method, int maxiter,
                                         const std::string& initializer, double fasttol, int rnd_seed) {
  if (data.empty()) {
    throw std::invalid_argument("pam: data must contain at least one variable");
  }
  const size_t num_obs = data[0].size();
  if (k == 0 || k > num_obs) {
    throw std::invalid_argument("pam: k must be between 1 and the number of observations");
  }
  // PAM is sensitive to variable scales (distances drive both medoid selection
  // and assignment), so standardize by default like the other clustering APIs.
  std::vector<std::vector<double>> scaled = scale_data(data, "standardize");

  const int n = static_cast<int>(num_obs);
  const int n_cols = static_cast<int>(scaled.size());

  double** matrix = new double*[n];
  int** mask = new int*[n];
  for (int i = 0; i < n; ++i) {
    matrix[i] = new double[n_cols];
    mask[i] = new int[n_cols];
    for (int j = 0; j < n_cols; ++j) mask[i][j] = 1;
  }
  // distancematrix() is row-wise: matrix[obs][var].
  for (int i = 0; i < n_cols; ++i) {
    for (int r = 0; r < n; ++r) matrix[r][i] = scaled[i][r];
  }

  char dist = 'e';
  std::string dm = distance_method;
  std::transform(dm.begin(), dm.end(), dm.begin(), [](unsigned char c) { return std::tolower(c); });
  if (dm == "manhattan") dist = 'b';

  double* weight = new double[n_cols];
  for (int i = 0; i < n_cols; ++i) weight[i] = 1.0;

  double** distances = distancematrix(n, n_cols, matrix, mask, weight, dist, 0);
  RawDistMatrix dmr(distances);
  PAMInitializer* init = (initializer == "LAB") ? static_cast<PAMInitializer*>(new LAB(&dmr, rnd_seed))
                                                : static_cast<PAMInitializer*>(new BUILD(&dmr));
  FastPAM pam(n, &dmr, init, static_cast<int>(k), maxiter, fasttol);
  pam.run();
  std::vector<int> results = pam.getResults();

  delete init;
  delete[] weight;
  for (int i = 1; i < n; ++i) delete[] distances[i];
  delete[] distances;
  for (int i = 0; i < n; ++i) {
    delete[] matrix[i];
    delete[] mask[i];
  }
  delete[] matrix;
  delete[] mask;

  // PAM::getResults() numbers clusters from 1 (matching the reference
  // implementation); convert to 0-based labels for the public API. A label
  // outside [1, k] can only appear when distances are degenerate (all-zero
  // assignment), so reject that rather than indexing out of bounds.
  std::vector<std::vector<int>> clusters(k);
  for (int i = 0; i < n; ++i) {
    if (results[i] < 1 || results[i] > static_cast<int>(k)) {
      throw std::invalid_argument("pam: clustering produced an invalid label");
    }
    clusters[results[i] - 1].push_back(i);
  }
  clusters.erase(std::remove_if(clusters.begin(), clusters.end(),
                                [](const std::vector<int>& c) { return c.empty(); }),
                 clusters.end());
  return clusters;
}

ValidationResult geoda::spatial_validation(const std::vector<int>& clusters,
                                           const std::vector<std::vector<unsigned int>>& neighbors,
                                           const GeometryCollection& geoms) {
  ValidationResult result;
  if (clusters.size() != neighbors.size()) {
    throw std::invalid_argument(
        "spatial_validation: clusters and neighbors must have the same number of observations");
  }
  int num_obs = static_cast<int>(neighbors.size());

  // Build point geometry contents from centroids (POINT shape type).
  std::vector<gda::GeometryContent*> geom_contents(num_obs);
  for (int i = 0; i < num_obs; ++i) {
    point_type pt = geoms.get_centroid(i);
    gda::PointContents* pc = new gda::PointContents();
    pc->x = pt.get<0>();
    pc->y = pt.get<1>();
    geom_contents[i] = pc;
  }

  // groups from clusters
  std::map<int, std::vector<int>> cluster_dict;
  for (int i = 0; i < num_obs; ++i) {
    cluster_dict[clusters[i]].push_back(i);
  }
  std::vector<std::vector<int>> groups;
  for (auto& kv : cluster_dict) {
    groups.push_back(kv.second);
  }

  GeoDaWeight* w = new VectorWeight(neighbors);
  SpatialValidation sv(num_obs, groups, w, geom_contents, gda::POINT_TYP);

  result.spatially_constrained = sv.IsSpatiallyConstrained();
  result.fragmentation = sv.GetFragmentation();
  result.cluster_fragmentation = sv.GetFragmentationFromClusters();
  result.cluster_diameter = sv.GetDiameterFromClusters();
  result.cluster_compactness = sv.GetCompactnessFromClusters();
  result.joincount_ratio = joincount_ratio(clusters, w);

  delete w;
  for (int i = 0; i < num_obs; ++i) delete geom_contents[i];
  return result;
}

std::vector<std::vector<int>> geoda::make_spatial(const std::vector<std::vector<int>>& clusters,
                                                  const std::vector<std::vector<unsigned int>>& neighbors) {
  int num_obs = static_cast<int>(neighbors.size());
  GeoDaWeight* w = new VectorWeight(neighbors);

  MakeSpatial ms(num_obs, clusters, w);
  ms.Run();
  std::vector<std::vector<int>> result = ms.GetClusters();
  delete w;
  return result;
}
