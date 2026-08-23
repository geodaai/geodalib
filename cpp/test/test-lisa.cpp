// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <gtest/gtest.h>

#include "sa/lisa-api.h"
#include "test/data.h"

// TEST_POINT_COLLECTION contains points at (0,0), (1,1), (2,2), which form a 1D chain:
// point 0 <-> point 1 <-> point 2.
static std::vector<std::vector<unsigned int>> TEST_NEIGHBORS = {{1}, {0, 2}, {1}};

TEST(LISA, LOCAL_JOIN_COUNT) {
  // binary (0/1) data for a univariate join count
  std::vector<double> data = {1, 0, 1};
  std::vector<unsigned int> undefs = {0, 0, 0};

  geoda::LisaResult result = geoda::local_joincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  EXPECT_EQ(result.lisa_vec.size(), 3u);
  EXPECT_EQ(result.sig_local_vec.size(), 3u);
  EXPECT_EQ(result.cluster_vec.size(), 3u);
  EXPECT_EQ(result.nn_vec.size(), 3u);
}

TEST(LISA, MULTIVARIATE_JOIN_COUNT) {
  // two binary variables
  std::vector<std::vector<double>> data = {{1, 0, 1}, {0, 1, 0}};
  std::vector<std::vector<unsigned int>> undefs = {{0, 0, 0}, {0, 0, 0}};

  geoda::LisaResult result =
      geoda::local_multijoincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  EXPECT_EQ(result.lisa_vec.size(), 3u);
  EXPECT_EQ(result.sig_local_vec.size(), 3u);
}
