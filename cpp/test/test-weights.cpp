// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

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

TEST(WEIGHTS, KERNEL_KNN_WEIGHTS_ADAPTIVE) {
  bool is_mile = false;
  unsigned int k = 1;
  // Adaptive bandwidth, k=1: each observation's single neighbor sits at the bandwidth boundary
  // (z = d / d = 1.0), so the gaussian kernel weight is gaussian(1.0).
  std::vector<std::vector<double>> result =
      geoda::kernel_knn_weights(TEST_POINT_COLLECTION, k, "gaussian", is_mile);
  ASSERT_EQ(result.size(), 3);

  double gaussian_at_one = (1.0 / std::sqrt(2.0 * geoda::pi)) * std::exp(-0.5);

  for (size_t i = 0; i < result.size(); ++i) {
    // row is [neighborIdx, weight, selfIdx, selfWeight]
    ASSERT_EQ(result[i].size(), 4u);
    EXPECT_NE(result[i][0], static_cast<double>(i));  // neighbor is not self
    EXPECT_NEAR(result[i][1], gaussian_at_one, 1e-9);
    EXPECT_EQ(result[i][2], static_cast<double>(i));
    EXPECT_EQ(result[i][3], 1.0);
  }
}

TEST(WEIGHTS, KERNEL_KNN_WEIGHTS_INVERSE_TRUNCATES_SUPPORT) {
  // Points spaced ~0.16 km apart. With inverse distance weighting (1 / distance^1)
  // and an adaptive bandwidth equal to that same distance, the normalized ratio
  // z = (1 / d) / bandwidth > 1 for every neighbor. Compact-support kernels (e.g.
  // triangular) must clamp to 0 instead of producing a negative weight.
  geoda::PointCollection close_points(std::vector<double>{0, 0.001, 0.002}, std::vector<double>{0, 0.001, 0.002},
                                      std::vector<unsigned int>{0, 1, 2}, std::vector<unsigned int>{1, 1, 1});
  std::vector<std::vector<double>> result =
      geoda::kernel_knn_weights(close_points, 1, "triangular", false, false, 1.0, true, true);
  ASSERT_EQ(result.size(), 3);

  for (size_t i = 0; i < result.size(); ++i) {
    auto pairs = to_pairs(result[i]);
    ASSERT_EQ(pairs.size(), 2u);  // 1 neighbor + self
    for (const auto& p : pairs) {
      if (p.first == static_cast<double>(i)) {
        EXPECT_EQ(p.second, 1.0);  // self weight stays 1.0
      } else {
        // z > 1, so the triangular weight must be truncated to 0, never negative
        EXPECT_GE(p.second, 0.0);
        EXPECT_LE(p.second, 1.0);
      }
    }
  }
}

TEST(WEIGHTS, KERNEL_KNN_WEIGHTS_INVERSE_COINCIDENT_POINTS) {
  // Two coincident points plus one distant point. With inverse distance weighting
  // and k=2 the coincident neighbor has d == 0.0 while the bandwidth (> 0) comes
  // from the distant neighbor; the coincident point must receive the maximum
  // kernel weight (z == 0.0) instead of an infinite/non-finite weight.
  geoda::PointCollection pts(std::vector<double>{0, 0, 0.002}, std::vector<double>{0, 0, 0.002},
                             std::vector<unsigned int>{0, 1, 2}, std::vector<unsigned int>{1, 1, 1});
  std::vector<std::vector<double>> result =
      geoda::kernel_knn_weights(pts, 2, "triangular", false, false, 1.0, true, true);
  ASSERT_EQ(result.size(), 3);

  for (size_t i = 0; i < result.size(); ++i) {
    auto pairs = to_pairs(result[i]);
    ASSERT_EQ(pairs.size(), 3u);  // 2 neighbors + self
    for (const auto& p : pairs) {
      EXPECT_TRUE(std::isfinite(p.second));
      EXPECT_GE(p.second, 0.0);
      if (p.first == static_cast<double>(i)) {
        EXPECT_EQ(p.second, 1.0);  // self weight stays 1.0
      } else if (p.second > 0.0) {
        // only the coincident neighbor may carry the maximum weight (z == 0.0)
        EXPECT_EQ(p.second, 1.0);
      }
    }
  }
}

TEST(WEIGHTS, KERNEL_KNN_WEIGHTS_GLOBAL_UNIFORM) {
  bool is_mile = false;
  unsigned int k = 2;
  // Global (non-adaptive) bandwidth with the uniform kernel: every neighbor gets weight 0.5
  // regardless of distance, and each observation has exactly k=2 neighbors plus self.
  std::vector<std::vector<double>> result =
      geoda::kernel_knn_weights(TEST_POINT_COLLECTION, k, "uniform", is_mile, false, 1.0, false);
  ASSERT_EQ(result.size(), 3);

  for (size_t i = 0; i < result.size(); ++i) {
    auto pairs = to_pairs(result[i]);
    // 2 neighbors + self
    ASSERT_EQ(pairs.size(), 3u);
    // all neighbors have weight 0.5
    for (const auto& p : pairs) {
      if (p.first == static_cast<double>(i)) {
        EXPECT_EQ(p.second, 1.0);  // self weight 1.0
      } else {
        EXPECT_NEAR(p.second, 0.5, 1e-9);
      }
    }
  }
}
