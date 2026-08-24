// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <gtest/gtest.h>

#include "mapping/mapping.h"
#include "sa/lisa-api.h"

// Points at (0,0), (1,1), (2,2) form a 1D chain: 0 <-> 1 <-> 2.
static std::vector<std::vector<unsigned int>> TEST_NEIGHBORS = {{1}, {0, 2}, {1}};

TEST(LISA, MULTIVARIATE_QUANTILE_LISA) {
  // k must be strictly below num_obs (3), so use 2 classes here.
  std::vector<int> k_s = {2, 2};
  std::vector<int> quantile_s = {1, 2};
  std::vector<std::vector<double>> data = {{1, 2, 3}, {3, 2, 1}};
  std::vector<std::vector<unsigned int>> undefs = {{0, 0, 0}, {0, 0, 0}};

  geoda::LisaResult result =
      geoda::local_multiquantilelisa(k_s, quantile_s, data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  EXPECT_EQ(result.lisa_vec.size(), 3u);
}

TEST(LISA, MULTIVARIATE_QUANTILE_LISA_UNDEF_MERGE) {
  // obs 1 is undefined only in the first variable. MultiJoinCount merges the
  // per-variable undefined flags (an observation is undefined if any variable is
  // undefined), so obs 1 must be marked undefined. Regression for the merge loop
  // that used to break out on correctly-sized undef rows and ignore the flags.
  std::vector<std::vector<unsigned int>> undefs = {{0, 1, 0}, {0, 0, 0}};
  geoda::LisaResult result =
      geoda::local_multiquantilelisa({2, 2}, {1, 2}, {{1, 2, 3}, {3, 2, 1}}, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);
  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.sig_cat_vec.size(), 3u);

  // obs 1 is undefined: significance category 6 and zero local statistic
  EXPECT_EQ(result.sig_cat_vec[1], 6);
  EXPECT_DOUBLE_EQ(result.lisa_vec[1], 0.0);
}

TEST(LISA, MULTIVARIATE_QUANTILE_LISA_SHORT_UNDEF_ROW) {
  // The first undef row only carries a flag for obs 0 (shorter than num_obs).
  // to_bool_undefs and the undef_i guard must consult the existing entry instead
  // of dropping the row entirely: obs 0 is undefined (category 6), obs 1/2 stay
  // defined.
  std::vector<std::vector<unsigned int>> undefs = {{1}, {0, 0, 0}};
  geoda::LisaResult result =
      geoda::local_multiquantilelisa({2, 2}, {1, 2}, {{1, 2, 3}, {3, 2, 1}}, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);
  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.sig_cat_vec.size(), 3u);

  EXPECT_EQ(result.sig_cat_vec[0], 6);
  EXPECT_DOUBLE_EQ(result.lisa_vec[0], 0.0);
  EXPECT_NE(result.sig_cat_vec[1], 6);
  EXPECT_NE(result.sig_cat_vec[2], 6);
}

TEST(LISA, MULTIVARIATE_QUANTILE_LISA_K_GTE_NUM_OBS) {
  // k must be strictly below num_obs (3), matching the univariate quantile_lisa
  // contract. k == num_obs (each obs its own class) is rejected too.
  geoda::LisaResult result =
      geoda::local_multiquantilelisa({4, 4}, {1, 2}, {{1, 2, 3}, {3, 2, 1}}, TEST_NEIGHBORS, {{0, 0, 0}, {0, 0, 0}},
                                     0.05, 99, 12345);
  EXPECT_FALSE(result.is_valid);

  result =
      geoda::local_multiquantilelisa({3, 3}, {1, 2}, {{1, 2, 3}, {3, 2, 1}}, TEST_NEIGHBORS, {{0, 0, 0}, {0, 0, 0}},
                                    0.05, 99, 12345);
  EXPECT_FALSE(result.is_valid);
}

TEST(LISA, QUANTILE_BREAKS_EXCLUDE_UNDEFINED) {
  // data is deliberately UNSORTED with obs 4 undefined. quantile_breaks must
  // exclude the undefined 100 from the cut-point computation AND sort the
  // defined-only subset first, matching the breaks on {1,2,3,4}: {1.5, 2.5, 3.5}
  // for k=4. (Regression: the undef vector was built but never consulted, so the
  // 100 skewed the cuts; a later edit dropped the std::sort, which also broke the
  // cuts on unsorted input.)
  std::vector<double> data = {4, 100, 2, 1, 3};
  std::vector<unsigned int> undef = {0, 1, 0, 0, 0};
  std::vector<double> breaks = geoda::quantile_breaks(4, data, undef);
  ASSERT_EQ(breaks.size(), 3u);
  EXPECT_NEAR(breaks[0], 1.5, 1e-9);
  EXPECT_NEAR(breaks[1], 2.5, 1e-9);
  EXPECT_NEAR(breaks[2], 3.5, 1e-9);
}

TEST(LISA, QUANTILE_BREAKS_ALL_UNDEFINED_NO_CRASH) {
  // All observations undefined: no defined cut points exist. Return a degenerate
  // all-zero break vector instead of indexing into an empty percentile input.
  std::vector<double> data = {1, 2, 3};
  std::vector<unsigned int> undef = {1, 1, 1};
  std::vector<double> breaks = geoda::quantile_breaks(4, data, undef);
  ASSERT_EQ(breaks.size(), 3u);
  EXPECT_DOUBLE_EQ(breaks[0], 0.0);
  EXPECT_DOUBLE_EQ(breaks[1], 0.0);
  EXPECT_DOUBLE_EQ(breaks[2], 0.0);
}

TEST(LISA, MULTIVARIATE_QUANTILE_LISA_INVALID_INPUTS) {
  std::vector<std::vector<unsigned int>> undefs = {{0, 0, 0}, {0, 0, 0}};

  // mismatched k/quantile/data variable counts
  geoda::LisaResult result = geoda::local_multiquantilelisa({4}, {1, 2}, {{1, 2, 3}, {3, 2, 1}}, TEST_NEIGHBORS,
                                                            undefs, 0.05, 99, 12345);
  EXPECT_FALSE(result.is_valid);

  // empty data
  result = geoda::local_multiquantilelisa({4, 4}, {1, 2}, {}, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);
  EXPECT_FALSE(result.is_valid);

  // neighbors count does not match observations
  std::vector<std::vector<unsigned int>> nbrs2 = {{1}, {0, 2}};
  result = geoda::local_multiquantilelisa({4, 4}, {1, 2}, {{1, 2, 3}, {3, 2, 1}}, nbrs2, undefs, 0.05, 99, 12345);
  EXPECT_FALSE(result.is_valid);

  // variable lengths do not match
  result = geoda::local_multiquantilelisa({4, 4}, {1, 2}, {{1, 2, 3}, {3, 2}}, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);
  EXPECT_FALSE(result.is_valid);

  // k < 2
  result = geoda::local_multiquantilelisa({1, 4}, {1, 2}, {{1, 2, 3}, {3, 2, 1}}, TEST_NEIGHBORS, undefs, 0.05, 99,
                                          12345);
  EXPECT_FALSE(result.is_valid);

  // q < 1
  result = geoda::local_multiquantilelisa({4, 4}, {0, 2}, {{1, 2, 3}, {3, 2, 1}}, TEST_NEIGHBORS, undefs, 0.05, 99,
                                          12345);
  EXPECT_FALSE(result.is_valid);

  // q > k
  result = geoda::local_multiquantilelisa({4, 4}, {1, 5}, {{1, 2, 3}, {3, 2, 1}}, TEST_NEIGHBORS, undefs, 0.05, 99,
                                          12345);
  EXPECT_FALSE(result.is_valid);
}
