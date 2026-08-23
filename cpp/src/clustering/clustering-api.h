// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#ifndef GEODA_CLUSTERING_API_H
#define GEODA_CLUSTERING_API_H

#include <string>
#include <vector>

namespace geoda {

/**
 * @brief Spatially constrained hierarchical clustering (SCHC).
 *
 * @param k The number of clusters.
 * @param neighbors The spatial weights matrix (adjacency list).
 * @param data The multivariate data (one vector per variable).
 * @param scale_method The scaling method ('raw' or 'standardize').
 * @param linkage_method The linkage method ('single', 'complete', 'average', 'ward').
 * @param distance_method The distance metric ('euclidean' or 'manhattan').
 * @param bound_vals Optional bound values for each observation.
 * @param min_bound The minimum bound.
 * @return std::vector<std::vector<int>> The clusters.
 */
std::vector<std::vector<int>> schc(unsigned int k, const std::vector<std::vector<unsigned int>>& neighbors,
                                   const std::vector<std::vector<double>>& data, const std::string& scale_method,
                                   const std::string& linkage_method, const std::string& distance_method,
                                   const std::vector<double>& bound_vals, double min_bound);

}  // namespace geoda

#endif
