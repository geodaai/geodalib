// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <algorithm>
#include <cmath>
#include <utility>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "weights/weights.h"
#include "test/data.h"

using namespace testing;

TEST(WEIGHTS, POLYGON_CONTIGUITY_WEIGHTS) {
  bool is_queen = true;
  double precision_threshold = 0.0;
  std::vector<std::vector<unsigned int>> nbrs =
      geoda::polygon_contiguity_weights(TEST_POLYGON_COLLECTION, is_queen, precision_threshold);
  EXPECT_EQ(nbrs.size(), 2);

  EXPECT_THAT(nbrs[0], ElementsAre(1));
  EXPECT_THAT(nbrs[1], ElementsAre(0));
}

TEST(WEIGHTS, POINT_CONTIGUITY_WEIGHTS) {
  bool is_queen = true;
  double precision_threshold = 0.0;
  std::vector<std::vector<unsigned int>> nbrs =
      geoda::point_contiguity_weights(TEST_POINT_COLLECTION, is_queen, precision_threshold);

  EXPECT_EQ(nbrs.size(), 3);
  EXPECT_THAT(nbrs[0], ElementsAre(1));
  EXPECT_THAT(nbrs[1], ElementsAre(0, 2));
  EXPECT_THAT(nbrs[2], ElementsAre(1));
}

TEST(WEIGHTS, MULTIPOLYGON_CONTIGUITY_WEIGHTS) {
  bool is_queen = true;
  double precision_threshold = 0.0;
  std::vector<std::vector<unsigned int>> nbrs =
      geoda::polygon_contiguity_weights(TEST_MULTIPOLYGON_COLLECTION, is_queen, precision_threshold);
  EXPECT_EQ(nbrs.size(), 2);

  EXPECT_THAT(nbrs[0], ElementsAre(1));
  EXPECT_THAT(nbrs[1], ElementsAre(0));
}

// Convert an interleaved [idx, weight, idx, weight, ...] row into a vector of pairs.
static std::vector<std::pair<double, double>> to_pairs(const std::vector<double>& row) {
  std::vector<std::pair<double, double>> pairs;
  for (size_t i = 0; i + 1 < row.size(); i += 2) {
    pairs.emplace_back(row[i], row[i + 1]);
  }
  return pairs;
}

// Assert that the neighbor pairs in a row match the expected set, ignoring neighbor order.
static void expect_pairs(const std::vector<double>& row,
                         const std::vector<std::pair<double, double>>& expected) {
  auto actual = to_pairs(row);
  ASSERT_EQ(actual.size(), expected.size());
  std::sort(actual.begin(), actual.end());
  std::vector<std::pair<double, double>> sorted_expected = expected;
  std::sort(sorted_expected.begin(), sorted_expected.end());
  for (size_t i = 0; i < actual.size(); ++i) {
    EXPECT_EQ(actual[i].first, sorted_expected[i].first);
    EXPECT_NEAR(actual[i].second, sorted_expected[i].second, 1e-9);
  }
}

TEST(WEIGHTS, KERNEL_WEIGHTS) {
  bool is_mile = false;
  double bandwidth = 200.0;

  // TEST_POINT_COLLECTION contains points at (0,0), (1,1), (2,2).
  // Haversine distances (km): d(0,1) ~= 157.2495585, d(1,2) ~= 157.2256093, d(0,2) ~= 314.475 > bandwidth.
  double d01 = 157.2495585117787;
  double d12 = 157.22560925091383;

  std::vector<std::vector<double>> result = geoda::kernel_weights(TEST_POINT_COLLECTION, bandwidth, "triangular", is_mile);

  ASSERT_EQ(result.size(), 3);

  // point 0: neighbor 1 only, plus self
  EXPECT_THAT(result[0], ElementsAre(1, DoubleNear(1.0 - d01 / bandwidth, 1e-9), 0.0, 1.0));

  // point 1: neighbors 0 and 2 (order not guaranteed), plus self
  expect_pairs(result[1], {{0.0, 1.0 - d01 / bandwidth}, {2.0, 1.0 - d12 / bandwidth}, {1.0, 1.0}});

  // point 2: neighbor 1 only, plus self
  EXPECT_THAT(result[2], ElementsAre(1.0, DoubleNear(1.0 - d12 / bandwidth, 1e-9), 2.0, 1.0));
}

TEST(WEIGHTS, KERNEL_WEIGHTS_DIAGONALS) {
  bool is_mile = false;
  double bandwidth = 200.0;

  // When use_kernel_diagonals is false (default), the self weight is 1.0.
  std::vector<std::vector<double>> default_result =
      geoda::kernel_weights(TEST_POINT_COLLECTION, bandwidth, "gaussian", is_mile);
  for (size_t i = 0; i < default_result.size(); ++i) {
    const auto& row = default_result[i];
    EXPECT_EQ(row.back(), 1.0);
    EXPECT_EQ(row[row.size() - 2], static_cast<double>(i));
  }

  // When use_kernel_diagonals is true, the self weight is kernel(1.0).
  std::vector<std::vector<double>> diag_result =
      geoda::kernel_weights(TEST_POINT_COLLECTION, bandwidth, "gaussian", is_mile, true);
  double gaussian_const = 1.0 / std::sqrt(2.0 * geoda::pi);
  double gaussian_self = gaussian_const * std::exp(-1.0 / 2.0);
  for (size_t i = 0; i < diag_result.size(); ++i) {
    const auto& row = diag_result[i];
    EXPECT_EQ(row[row.size() - 2], static_cast<double>(i));
    EXPECT_NEAR(row.back(), gaussian_self, 1e-9);
  }
}
