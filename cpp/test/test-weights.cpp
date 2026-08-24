// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>

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

TEST(WEIGHTS, NEIGHBOR_MATCH_TEST) {
  // points at (0,0), (1,1), (2,2) with values increasing along the chain
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0}};
  std::vector<std::vector<double>> result =
      geoda::neighbor_match_test(TEST_POINT_COLLECTION, 1, data, "standardize", "euclidean", false);
  ASSERT_EQ(result.size(), 2u);  // Cardinality + Probability
  ASSERT_EQ(result[0].size(), 3u);
  ASSERT_EQ(result[1].size(), 3u);
  // k = 1: every observation's attribute 1-NN matches its spatial 1-NN
  EXPECT_EQ(result[0][0], 1.0);
  EXPECT_EQ(result[0][1], 1.0);
  EXPECT_EQ(result[0][2], 1.0);
  // P(X = 1) = C(1,1) * C(1,0) / C(2,1) = 0.5
  EXPECT_NEAR(result[1][0], 0.5, 1e-9);
  EXPECT_NEAR(result[1][1], 0.5, 1e-9);
  EXPECT_NEAR(result[1][2], 0.5, 1e-9);

  // k = 2: both neighbor draws are the other two observations, so the overlap is
  // always 2 and P(X = 2) = C(2,2) * C(0,0) / C(2,2) = 1
  std::vector<std::vector<double>> result2 =
      geoda::neighbor_match_test(TEST_POINT_COLLECTION, 2, data, "standardize", "euclidean", false);
  ASSERT_EQ(result2.size(), 2u);
  ASSERT_EQ(result2[0].size(), 3u);
  ASSERT_EQ(result2[1].size(), 3u);
  EXPECT_EQ(result2[0][0], 2.0);
  EXPECT_EQ(result2[0][1], 2.0);
  EXPECT_EQ(result2[0][2], 2.0);
  EXPECT_NEAR(result2[1][0], 1.0, 1e-9);
  EXPECT_NEAR(result2[1][1], 1.0, 1e-9);
  EXPECT_NEAR(result2[1][2], 1.0, 1e-9);

  // a variable shorter than the geometry collection is rejected with an empty result
  std::vector<std::vector<double>> bad = {{1.0, 2.0, 3.0}, {1.0, 2.0}};
  std::vector<std::vector<double>> empty =
      geoda::neighbor_match_test(TEST_POINT_COLLECTION, 1, bad, "standardize", "euclidean", false);
  EXPECT_TRUE(empty.empty());
}

TEST(WEIGHTS, NEIGHBOR_MATCH_EDGE_CASES) {
  // k >= num_obs is impossible (at most num_obs - 1 neighbors exist); it must
  // be rejected up front rather than leaving non-probability sentinels behind.
  std::vector<std::vector<double>> data3 = {{1.0, 2.0, 3.0}};
  std::vector<std::vector<double>> rejected =
      geoda::neighbor_match_test(TEST_POINT_COLLECTION, 3, data3, "standardize", "euclidean", false);
  EXPECT_TRUE(rejected.empty());

  // Symmetric geometry: A(0,0) is equidistant from B(1,0) and C(0,1), and B and
  // C share an attribute value. The (distance, index) tie-break must pick B for
  // A in both spaces, giving cardinality [1, 0, 0] deterministically.
  geoda::PointCollection sym(std::vector<double>{0, 1, 0}, std::vector<double>{0, 0, 1},
                             std::vector<unsigned int>{0, 1, 2}, std::vector<unsigned int>{1, 1, 1});
  std::vector<std::vector<double>> sym_data = {{0.0, 1.0, 1.0}};
  std::vector<std::vector<double>> sym_result =
      geoda::neighbor_match_test(sym, 1, sym_data, "raw", "euclidean", false);
  ASSERT_EQ(sym_result.size(), 2u);
  EXPECT_THAT(sym_result[0], ElementsAre(1.0, 0.0, 0.0));
  // k = 1 with universe = 2: P(0) = P(1) = 0.5
  EXPECT_NEAR(sym_result[1][0], 0.5, 1e-9);
  EXPECT_NEAR(sym_result[1][1], 0.5, 1e-9);
  EXPECT_NEAR(sym_result[1][2], 0.5, 1e-9);

  // Large n with k near the middle (300 observations, k = 150) must not overflow
  // the intermediate combination products, and every probability must be a real
  // number in [0, 1].
  std::vector<double> xs, ys, vals;
  for (int i = 0; i < 30; ++i) {
    for (int j = 0; j < 10; ++j) {
      xs.push_back(static_cast<double>(i));
      ys.push_back(static_cast<double>(j));
      vals.push_back(static_cast<double>(i + j));
    }
  }
  std::vector<unsigned int> parts(xs.size()), sizes(xs.size(), 1);
  for (size_t i = 0; i < parts.size(); ++i) parts[i] = static_cast<unsigned int>(i);
  geoda::PointCollection grid(xs, ys, parts, sizes);
  std::vector<std::vector<double>> large =
      geoda::neighbor_match_test(grid, 150, {vals}, "raw", "euclidean", false);
  ASSERT_EQ(large.size(), 2u);
  ASSERT_EQ(large[0].size(), 300u);
  ASSERT_EQ(large[1].size(), 300u);
  for (double p : large[1]) {
    EXPECT_TRUE(std::isfinite(p));
    EXPECT_GE(p, 0.0);
    EXPECT_LE(p, 1.0);
  }
}
