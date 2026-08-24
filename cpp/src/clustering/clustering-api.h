// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#ifndef GEODA_CLUSTERING_API_H
#define GEODA_CLUSTERING_API_H

#include <string>
#include <vector>
#include "geometry/geometry.h"
#include "gda_clustering.h"

namespace geoda {

std::vector<std::vector<int>> schc(unsigned int k, const std::vector<std::vector<unsigned int>>& neighbors,
                                   const std::vector<std::vector<double>>& data, const std::string& scale_method,
                                   const std::string& linkage_method, const std::string& distance_method,
                                   const std::vector<double>& bound_vals, double min_bound);

std::vector<std::vector<int>> redcap(unsigned int k, const std::vector<std::vector<unsigned int>>& neighbors,
                                     const std::vector<std::vector<double>>& data, const std::string& scale_method,
                                     const std::string& redcap_method, const std::string& distance_method,
                                     const std::vector<double>& bound_vals, double min_bound);

std::vector<std::vector<int>> skater(unsigned int k, const std::vector<std::vector<unsigned int>>& neighbors,
                                     const std::vector<std::vector<double>>& data, const std::string& scale_method,
                                     const std::string& distance_method, const std::vector<double>& bound_vals,
                                     double min_bound);

std::vector<std::vector<int>> azp_greedy(int p, const std::vector<std::vector<unsigned int>>& neighbors,
                                        const std::vector<std::vector<double>>& data, int inits,
                                        const std::string& distance_method, int rnd_seed);
std::vector<std::vector<int>> maxp_greedy(const std::vector<std::vector<unsigned int>>& neighbors,
                                          const std::vector<std::vector<double>>& data, int iterations,
                                          const std::string& distance_method, int rnd_seed);

std::vector<std::vector<int>> azp_sa(int p, const std::vector<std::vector<unsigned int>>& neighbors,
                                    const std::vector<std::vector<double>>& data, int inits, double cooling_rate,
                                    int sa_maxit, const std::string& distance_method, int rnd_seed);
std::vector<std::vector<int>> azp_tabu(int p, const std::vector<std::vector<unsigned int>>& neighbors,
                                       const std::vector<std::vector<double>>& data, int inits, int tabu_length,
                                       int conv_tabu, const std::string& distance_method, int rnd_seed);
std::vector<std::vector<int>> maxp_sa(const std::vector<std::vector<unsigned int>>& neighbors,
                                      const std::vector<std::vector<double>>& data, int iterations, double cooling_rate,
                                      int sa_maxit, const std::string& distance_method, int rnd_seed);
std::vector<std::vector<int>> maxp_tabu(const std::vector<std::vector<unsigned int>>& neighbors,
                                        const std::vector<std::vector<double>>& data, int iterations, int tabu_length,
                                        int conv_tabu, const std::string& distance_method, int rnd_seed);

ValidationResult spatial_validation(const std::vector<int>& clusters,
                                    const std::vector<std::vector<unsigned int>>& neighbors,
                                    const GeometryCollection& geoms);
std::vector<std::vector<int>> make_spatial(const std::vector<std::vector<int>>& clusters,
                                           const std::vector<std::vector<unsigned int>>& neighbors);

}  // namespace geoda

#endif
