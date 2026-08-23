// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <string>
#include <vector>

#include "clustering-api.h"
#include "schc_wrapper.h"
#include "redcap_wrapper.h"
#include "azp_wrapper.h"
#include "maxp_wrapper.h"
#include "spatial_validation.h"
#include "geofeature.h"
#include "joincount_ratio.h"
#include <map>
#include "../data/data.h"
#include "../weights/vector-weight.h"

namespace {

std::vector<std::vector<double>> scale_data(const std::vector<std::vector<double>>& data,
                                            const std::string& scale_method) {
  std::vector<std::vector<double>> scaled = data;
  if (scale_method == "raw") {
    return scaled;
  }
  std::vector<unsigned int> undef(scaled[0].size(), 0);
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
  azp_greedy_wrapper azp(p, w, data, inits, min_bounds, max_bounds, init_regions, distance_method, rnd_seed, 0);
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
  maxp_greedy_wrapper mp(w, data, iterations, min_bounds, max_bounds, init_regions, distance_method, rnd_seed, 1, 0);
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
  azp_sa_wrapper azp(p, w, data, inits, cooling_rate, sa_maxit, min_bounds, max_bounds, init_regions, distance_method,
                     rnd_seed, 0);
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
  azp_tabu_wrapper azp(p, w, data, inits, tabu_length, conv_tabu, min_bounds, max_bounds, init_regions, distance_method,
                       rnd_seed, 0);
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
  maxp_sa_wrapper mp(w, data, iterations, cooling_rate, sa_maxit, min_bounds, max_bounds, init_regions, distance_method,
                     rnd_seed, 1, 0);
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
  maxp_tabu_wrapper mp(w, data, iterations, tabu_length, conv_tabu, min_bounds, max_bounds, init_regions,
                       distance_method, rnd_seed, 1, 0);
  std::vector<std::vector<int>> result = mp.GetClusters();
  delete w;
  return result;
}

ValidationResult geoda::spatial_validation(const std::vector<int>& clusters,
                                           const std::vector<std::vector<unsigned int>>& neighbors,
                                           const GeometryCollection& geoms) {
  ValidationResult result;
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
