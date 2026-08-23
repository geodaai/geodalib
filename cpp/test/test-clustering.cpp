// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <gtest/gtest.h>

#include "clustering/clustering-api.h"

// Points 0<->1<->2 in a chain.
static std::vector<std::vector<unsigned int>> TEST_NEIGHBORS = {{1}, {0, 2}, {1}};

TEST(CLUSTERING, MAKE_SPATIAL) {
  // cluster 0 = {0,1}, cluster 1 = {2} (already spatially contiguous)
  std::vector<std::vector<int>> clusters = {{0, 1}, {2}};
  std::vector<std::vector<int>> result = geoda::make_spatial(clusters, TEST_NEIGHBORS);

  // union of result should cover all 3 observations
  int count = 0;
  for (const auto& c : result) {
    count += static_cast<int>(c.size());
  }
  EXPECT_EQ(count, 3);
}
