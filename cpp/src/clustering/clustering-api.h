// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#ifndef GEODA_CLUSTERING_API_H
#define GEODA_CLUSTERING_API_H

#include <string>
#include <vector>

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

}  // namespace geoda

#endif
