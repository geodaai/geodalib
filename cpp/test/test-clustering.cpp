// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <gtest/gtest.h>

#include "clustering/clustering-api.h"

// Points 0<->1<->2<->3<->4 in a chain.
static std::vector<std::vector<unsigned int>> TEST_NEIGHBORS = {{1}, {0, 2}, {1, 3}, {2, 4}, {3}};

TEST(CLUSTERING, SCHC) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<double> bound_vals;
  std::vector<std::vector<int>> result =
      geoda::schc(2, TEST_NEIGHBORS, data, "raw", "ward", "euclidean", bound_vals, 0.0);

  // 2 clusters, all 5 observations covered
  int count = 0;
  for (const auto& c : result) count += static_cast<int>(c.size());
  EXPECT_EQ(count, 5);
}

TEST(CLUSTERING, REDCAP) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<double> bound_vals;
  std::vector<std::vector<int>> result =
      geoda::redcap(2, TEST_NEIGHBORS, data, "raw", "firstorder-singlelinkage", "euclidean", bound_vals, 0.0);
  int count = 0;
  for (const auto& c : result) count += static_cast<int>(c.size());
  EXPECT_EQ(count, 5);
}

TEST(CLUSTERING, SKATER) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<double> bound_vals;
  std::vector<std::vector<int>> result =
      geoda::skater(2, TEST_NEIGHBORS, data, "raw", "euclidean", bound_vals, 0.0);
  int count = 0;
  for (const auto& c : result) count += static_cast<int>(c.size());
  EXPECT_EQ(count, 5);
}
