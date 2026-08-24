// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include "clustering-api.h"
#include "make_spatial.h"
#include "../weights/vector-weight.h"

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
