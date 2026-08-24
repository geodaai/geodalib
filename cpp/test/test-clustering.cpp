// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <stdexcept>

#include <gtest/gtest.h>

#include "clustering/clustering-api.h"
#include "test/data.h"

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

  // Variables of unequal length must be rejected instead of silently scaling
  // an inconsistent data matrix.
  std::vector<std::vector<double>> mismatched = {{1.0, 2.0, 3.0, 4.0, 5.0}, {1.0, 2.0, 3.0}};
  EXPECT_THROW(geoda::schc(2, TEST_NEIGHBORS, mismatched, "standardize", "ward", "euclidean", bound_vals, 0.0),
               std::invalid_argument);
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

TEST(CLUSTERING, SPATIAL_VALIDATION) {
  // non-collinear points so the convex hull is not degenerate
  geoda::PointCollection pts(std::vector<double>{0, 1, 0, 2, 2}, std::vector<double>{0, 0, 2, 1, 2},
                             std::vector<unsigned int>{0, 1, 2, 3, 4}, std::vector<unsigned int>{1, 1, 1, 1, 1});
  std::vector<int> clusters = {1, 1, 1, 2, 2};
  ValidationResult result = geoda::spatial_validation(clusters, TEST_NEIGHBORS, pts);
  EXPECT_EQ(result.spatially_constrained, true);

  // A cluster label list shorter than the neighbor list would index out of
  // bounds; it must be rejected up front.
  std::vector<int> short_clusters = {1, 1, 1};
  EXPECT_THROW(geoda::spatial_validation(short_clusters, TEST_NEIGHBORS, pts), std::invalid_argument);
}

TEST(CLUSTERING, AZP_GREEDY) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<std::vector<int>> result = geoda::azp_greedy(2, TEST_NEIGHBORS, data, 10, "euclidean", 1234567);
  int count = 0;
  for (const auto& c : result) count += static_cast<int>(c.size());
  EXPECT_EQ(count, 5);
}

TEST(CLUSTERING, MAXP_GREEDY) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<std::vector<int>> result = geoda::maxp_greedy(TEST_NEIGHBORS, data, 10, "euclidean", 1234567);
  int count = 0;
  for (const auto& c : result) count += static_cast<int>(c.size());
  EXPECT_EQ(count, 5);
}
