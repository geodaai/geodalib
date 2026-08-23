// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#ifndef GEODA_CLUSTERING_API_H
#define GEODA_CLUSTERING_API_H

#include <vector>

namespace geoda {

/**
 * @brief Make a set of clusters spatially contiguous.
 *
 * @param clusters The clusters as a list of lists of observation indices.
 * @param neighbors The spatial weights matrix (adjacency list).
 * @return std::vector<std::vector<int>> The spatially contiguous clusters.
 */
std::vector<std::vector<int>> make_spatial(const std::vector<std::vector<int>>& clusters,
                                           const std::vector<std::vector<unsigned int>>& neighbors);

}  // namespace geoda

#endif
