// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

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

TEST(WEIGHTS, NEIGHBOR_MATCH_TEST) {
  // points at (0,0), (1,1), (2,2) with values increasing along the chain
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0}};
  std::vector<std::vector<double>> result =
      geoda::neighbor_match_test(TEST_POINT_COLLECTION, 1, data, "standardize", "euclidean", false);
  ASSERT_EQ(result.size(), 2u);  // Cardinality + Probability
  ASSERT_EQ(result[0].size(), 3u);
  ASSERT_EQ(result[1].size(), 3u);
}
