// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <algorithm>
#include <gtest/gtest.h>

#include "clustering/clustering-api.h"

// Points 0<->1<->2 in a chain.
static std::vector<std::vector<unsigned int>> TEST_NEIGHBORS = {{1}, {0, 2}, {1}};

namespace {

void expect_partition(const std::vector<std::vector<int>>& clusters, int num_obs) {
  std::vector<bool> seen(num_obs, false);
  for (const auto& c : clusters) {
    for (int e : c) {
      ASSERT_GE(e, 0);
      ASSERT_LT(e, num_obs);
      EXPECT_FALSE(seen[e]) << "observation " << e << " appears in more than one cluster";
      seen[e] = true;
    }
  }
  for (int i = 0; i < num_obs; ++i) {
    EXPECT_TRUE(seen[i]) << "observation " << i << " is missing from the result";
  }
}

void expect_contiguous(const std::vector<std::vector<int>>& clusters,
                       const std::vector<std::vector<unsigned int>>& neighbors) {
  for (const auto& c : clusters) {
    ASSERT_FALSE(c.empty());
    // BFS from the first element restricted to the cluster; every member must
    // be reachable for the cluster to form a connected subgraph.
    std::vector<int> cluster = c;
    std::sort(cluster.begin(), cluster.end());
    std::vector<bool> visited(neighbors.size(), false);
    std::vector<int> stack = {c[0]};
    visited[c[0]] = true;
    int reached = 0;
    while (!stack.empty()) {
      int cur = stack.back();
      stack.pop_back();
      ++reached;
      for (unsigned int nbr : neighbors[cur]) {
        if (!visited[nbr] && std::binary_search(cluster.begin(), cluster.end(), static_cast<int>(nbr))) {
          visited[nbr] = true;
          stack.push_back(nbr);
        }
      }
    }
    EXPECT_EQ(reached, static_cast<int>(c.size()))
        << "cluster is not connected under the spatial weights";
  }
}

}  // namespace

TEST(CLUSTERING, MAKE_SPATIAL) {
  // cluster 0 = {0,1}, cluster 1 = {2} (already spatially contiguous)
  std::vector<std::vector<int>> clusters = {{0, 1}, {2}};
  std::vector<std::vector<int>> result = geoda::make_spatial(clusters, TEST_NEIGHBORS);

  expect_partition(result, 3);
  expect_contiguous(result, TEST_NEIGHBORS);
}

TEST(CLUSTERING, MAKE_SPATIAL_REASSIGNS_DISCONNECTED) {
  // cluster 0 = {0,2} is disconnected in the chain 0-1-2: 0 and 2 are not
  // adjacent, so element 2 must be reassigned into cluster 1.
  std::vector<std::vector<int>> clusters = {{0, 2}, {1}};
  std::vector<std::vector<int>> result = geoda::make_spatial(clusters, TEST_NEIGHBORS);

  expect_partition(result, 3);
  expect_contiguous(result, TEST_NEIGHBORS);

  // 0 and 2 can no longer share a cluster (they are not spatially adjacent)
  for (const auto& c : result) {
    bool has0 = std::find(c.begin(), c.end(), 0) != c.end();
    bool has2 = std::find(c.begin(), c.end(), 2) != c.end();
    EXPECT_FALSE(has0 && has2);
  }
}
