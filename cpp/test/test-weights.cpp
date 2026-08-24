// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
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

// Validate each supported kernel function against a known neighbor pair.
// Point 0 of TEST_POINT_COLLECTION is at (0,0); point 1 is at (1,1). With a
// bandwidth of 200 km, point 1 is the only neighbor of point 0 (d(0,1) ~=
// 157.2495585 km), so the row is deterministic: [1, kernel(z), 0, 1.0].
TEST(WEIGHTS, KERNEL_WEIGHTS_KERNEL_FUNCTIONS) {
  bool is_mile = false;
  double bandwidth = 200.0;
  double d01 = 157.2495585117787;
  double z01 = d01 / bandwidth;

  struct KernelCase {
    const char* name;
    double expected;
  };

  // Reference values from Anselin & Rey (2010), table 5.4 at z = d01 / bandwidth.
  KernelCase cases[] = {
      {"triangular", 1.0 - z01},
      {"uniform", 0.5},
      {"epanechnikov", (3.0 / 4.0) * (1.0 - z01 * z01)},
      {"quartic", (15.0 / 16.0) * std::pow(1.0 - z01 * z01, 2.0)},
      {"gaussian", (1.0 / std::sqrt(2.0 * geoda::pi)) * std::exp(-z01 * z01 / 2.0)},
  };

  for (const auto& c : cases) {
    std::vector<std::vector<double>> result =
        geoda::kernel_weights(TEST_POINT_COLLECTION, bandwidth, c.name, is_mile);
    // point 0: only neighbor is 1, with weight kernel(z01); self weight 1.0.
    EXPECT_THAT(result[0], ElementsAre(1, DoubleNear(c.expected, 1e-9), 0.0, 1.0))
        << "kernel: " << c.name;
  }
}

// Verify that an unsupported kernel is rejected.
TEST(WEIGHTS, KERNEL_WEIGHTS_INVALID_KERNEL) {
  bool is_mile = false;
  EXPECT_THROW(geoda::kernel_weights(TEST_POINT_COLLECTION, 200.0, "not-a-kernel", is_mile),
               std::invalid_argument);
  // The kernel name comparison is case-insensitive.
  std::vector<std::vector<double>> ok =
      geoda::kernel_weights(TEST_POINT_COLLECTION, 200.0, "GAUSSIAN", is_mile);
  EXPECT_EQ(ok.size(), 3);
}

// A non-positive or non-finite bandwidth must be rejected before any division/bbox work.
TEST(WEIGHTS, KERNEL_WEIGHTS_INVALID_BANDWIDTH) {
  bool is_mile = false;
  EXPECT_THROW(geoda::kernel_weights(TEST_POINT_COLLECTION, 0.0, "gaussian", is_mile),
               std::invalid_argument);
  EXPECT_THROW(geoda::kernel_weights(TEST_POINT_COLLECTION, -1.0, "gaussian", is_mile),
               std::invalid_argument);
  EXPECT_THROW(geoda::kernel_weights(TEST_POINT_COLLECTION,
                                     std::numeric_limits<double>::quiet_NaN(), "gaussian", is_mile),
               std::invalid_argument);
  EXPECT_THROW(geoda::kernel_weights(TEST_POINT_COLLECTION,
                                     std::numeric_limits<double>::infinity(), "gaussian", is_mile),
               std::invalid_argument);
  // A non-finite power is also rejected.
  EXPECT_THROW(geoda::kernel_weights(TEST_POINT_COLLECTION, 200.0, "gaussian", is_mile, false,
                                     std::numeric_limits<double>::quiet_NaN()),
               std::invalid_argument);
}

// Verify the distance unit switch: weights must reflect the haversine distance in miles.
TEST(WEIGHTS, KERNEL_WEIGHTS_MILES) {
  double bandwidth = 150.0;  // miles; >= d(0,1) but < d(0,2) so point 0 keeps a single neighbor
  double d01_miles = 97.71034565125832;
  double d12_miles = 97.69546427048152;
  double z01 = d01_miles / bandwidth;
  double z12 = d12_miles / bandwidth;

  std::vector<std::vector<double>> result =
      geoda::kernel_weights(TEST_POINT_COLLECTION, bandwidth, "triangular", true);
  ASSERT_EQ(result.size(), 3);

  // point 0: neighbor 1 with triangular(z01); self 1.0.
  EXPECT_THAT(result[0], ElementsAre(1, DoubleNear(1.0 - z01, 1e-9), 0.0, 1.0));
  // point 2: neighbor 1 with triangular(z12); self 1.0.
  EXPECT_THAT(result[2], ElementsAre(1.0, DoubleNear(1.0 - z12, 1e-9), 2.0, 1.0));
}

// When the bandwidth is smaller than the nearest neighbor distance, every observation
// has no neighbors and only the diagonal (self) element is present.
TEST(WEIGHTS, KERNEL_WEIGHTS_NO_NEIGHBORS) {
  bool is_mile = false;
  // nearest neighbor distance ~= 157.25 km, so a 100 km bandwidth yields no neighbors.
  std::vector<std::vector<double>> result =
      geoda::kernel_weights(TEST_POINT_COLLECTION, 100.0, "gaussian", is_mile);
  ASSERT_EQ(result.size(), 3);
  for (size_t i = 0; i < result.size(); ++i) {
    EXPECT_THAT(result[i], ElementsAre(static_cast<double>(i), 1.0));
  }
}

// A bandwidth larger than every pairwise distance connects all observations.
TEST(WEIGHTS, KERNEL_WEIGHTS_ALL_NEIGHBORS) {
  bool is_mile = false;
  double bandwidth = 400.0;  // larger than the max pairwise distance (~314.48 km)
  std::vector<std::vector<double>> result =
      geoda::kernel_weights(TEST_POINT_COLLECTION, bandwidth, "uniform", is_mile);
  ASSERT_EQ(result.size(), 3);

  // uniform kernel: every non-self neighbor gets weight 0.5; self weight 1.0.
  expect_pairs(result[0], {{1.0, 0.5}, {2.0, 0.5}, {0.0, 1.0}});
  expect_pairs(result[1], {{0.0, 0.5}, {2.0, 0.5}, {1.0, 1.0}});
  expect_pairs(result[2], {{0.0, 0.5}, {1.0, 0.5}, {2.0, 1.0}});
}

// For polygon collections the centroids are used as the point locations.
TEST(WEIGHTS, KERNEL_WEIGHTS_POLYGON_CENTROIDS) {
  bool is_mile = false;
  double bandwidth = 200.0;
  // TEST_POLYGON_COLLECTION holds two unit squares with centroids (0.5,0.5) and (1.5,1.5).
  // Their haversine distance is ~157.2405768 km.
  double centroid_distance = 157.24057682632696;

  std::vector<std::vector<double>> result =
      geoda::kernel_weights(TEST_POLYGON_COLLECTION, bandwidth, "triangular", is_mile);
  ASSERT_EQ(result.size(), 2);

  EXPECT_THAT(result[0], ElementsAre(1, DoubleNear(1.0 - centroid_distance / bandwidth, 1e-9), 0.0, 1.0));
  EXPECT_THAT(result[1], ElementsAre(0.0, DoubleNear(1.0 - centroid_distance / bandwidth, 1e-9), 1.0, 1.0));
}

// Verify the power argument: the kernel ratio is z = distance^power / bandwidth.
TEST(WEIGHTS, KERNEL_WEIGHTS_POWER) {
  bool is_mile = false;
  double bandwidth = 200.0;
  double d01 = 157.2495585117787;
  double power = 2.0;

  std::vector<std::vector<double>> result =
      geoda::kernel_weights(TEST_POINT_COLLECTION, bandwidth, "triangular", is_mile, false, power);
  ASSERT_EQ(result.size(), 3);

  double z = std::pow(d01, power) / bandwidth;
  // point 0: neighbor 1 with triangular(d01^power / bandwidth); self 1.0.
  EXPECT_THAT(result[0], ElementsAre(1, DoubleNear(1.0 - z, 1e-9), 0.0, 1.0));

  // power == 1.0 must match the default behavior.
  std::vector<std::vector<double>> default_result =
      geoda::kernel_weights(TEST_POINT_COLLECTION, bandwidth, "triangular", is_mile);
  std::vector<std::vector<double>> power_one_result =
      geoda::kernel_weights(TEST_POINT_COLLECTION, bandwidth, "triangular", is_mile, false, 1.0);
  EXPECT_EQ(default_result, power_one_result);
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
