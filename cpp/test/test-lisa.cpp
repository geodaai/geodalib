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

TEST(LISA, LOCAL_JOIN_COUNT_DETERMINISTIC) {
  // data = {1, 1, 0}: obs 0 and obs 1 are 1-valued and adjacent, so the local
  // join counts are known and non-zero: lisa_vec = [1, 1, 0]. The p-values and
  // significance categories are deterministic for a fixed seed (12345, 99 perms).
  std::vector<double> data = {1, 1, 0};
  std::vector<unsigned int> undefs = {0, 0, 0};

  geoda::LisaResult result = geoda::local_joincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 3u);

  // deterministic local join counts (RNG-free)
  EXPECT_DOUBLE_EQ(result.lisa_vec[0], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[1], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[2], 0.0);

  // deterministic p-values for the fixed seed
  EXPECT_NEAR(result.sig_local_vec[0], 0.44, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[1], 0.01, 1e-9);
  EXPECT_DOUBLE_EQ(result.sig_local_vec[2], -1.0);  // lisa = 0, not permuted

  // significance categories: 0 = not significant, 2 = p <= 0.01
  EXPECT_EQ(result.sig_cat_vec[0], 0);
  EXPECT_EQ(result.sig_cat_vec[1], 2);
  EXPECT_EQ(result.sig_cat_vec[2], 0);

  // clusters: 0 = not significant, 1 = significant
  EXPECT_EQ(result.cluster_vec[0], 0);
  EXPECT_EQ(result.cluster_vec[1], 1);
  EXPECT_EQ(result.cluster_vec[2], 0);
}

TEST(LISA, MULTIVARIATE_JOIN_COUNT_DETERMINISTIC) {
  // two binary variables with a colocation: obs 0 and obs 1 both have all
  // variables = 1 and are adjacent, so lisa_vec = [1, 1, 0].
  std::vector<std::vector<double>> data = {{1, 1, 0}, {1, 1, 1}};
  std::vector<std::vector<unsigned int>> undefs = {{0, 0, 0}, {0, 0, 0}};

  geoda::LisaResult result =
      geoda::local_multijoincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 3u);

  EXPECT_DOUBLE_EQ(result.lisa_vec[0], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[1], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[2], 0.0);

  EXPECT_NEAR(result.sig_local_vec[0], 0.44, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[1], 0.01, 1e-9);
  EXPECT_DOUBLE_EQ(result.sig_local_vec[2], -1.0);

  EXPECT_EQ(result.sig_cat_vec[0], 0);
  EXPECT_EQ(result.sig_cat_vec[1], 2);
  EXPECT_EQ(result.sig_cat_vec[2], 0);

  EXPECT_EQ(result.cluster_vec[0], 0);
  EXPECT_EQ(result.cluster_vec[1], 1);
  EXPECT_EQ(result.cluster_vec[2], 0);
}

TEST(LISA, LOCAL_JOIN_COUNT_UNDEFINED_NEIGHBOR_NO_HANG) {
  // Regression: the "complete" permutation sampler drew exactly numNeighbors valid
  // candidates and rejected undefined ones, so with a non-zero local join count and
  // an undefined neighbor it could loop forever. The lookup-table path must not hang.
  // obs 1 has neighbors {0, 2} with obs 2 undefined: it still gets a non-zero join
  // count from obs 0, but only one valid permutation candidate remains.
  std::vector<double> data = {1, 1, 0};
  std::vector<unsigned int> undefs = {0, 0, 1};

  geoda::LisaResult result = geoda::local_joincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 3u);

  EXPECT_DOUBLE_EQ(result.lisa_vec[0], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[1], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[2], 0.0);

  EXPECT_NEAR(result.sig_local_vec[0], 0.44, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[1], 0.01, 1e-9);

  // significance categories: 2 = p <= 0.01, 6 = undefined
  EXPECT_EQ(result.sig_cat_vec[0], 0);
  EXPECT_EQ(result.sig_cat_vec[1], 2);
  EXPECT_EQ(result.sig_cat_vec[2], 6);

  EXPECT_EQ(result.cluster_vec[0], 0);
  EXPECT_EQ(result.cluster_vec[1], 1);
  EXPECT_EQ(result.cluster_vec[2], 0);
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
