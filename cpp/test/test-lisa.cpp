// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <gtest/gtest.h>

#include "lisa/BatchLocalMoran.h"
#include "mapping/mapping.h"
#include "sa/lisa-api.h"
#include "test/data.h"
#include "weights/vector-weight.h"

// Points at (0,0), (1,1), (2,2) form a 1D chain: 0 <-> 1 <-> 2.
static std::vector<std::vector<unsigned int>> TEST_NEIGHBORS = {{1}, {0, 2}, {1}};

TEST(LISA, BATCH_LOCAL_MORAN) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0}, {3.0, 2.0, 1.0}};
  std::vector<std::vector<unsigned int>> undefs = {{0, 0, 0}, {0, 0, 0}};

  geoda::BatchLisaResult result =
      geoda::batch_local_moran(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.lisa_values.size(), 2u);  // one per variable
  EXPECT_EQ(result.lisa_values[0].size(), 3u);
  EXPECT_EQ(result.lisa_values[1].size(), 3u);
  EXPECT_EQ(result.nn.size(), 3u);
}

TEST(LISA, SET_BATCH_LISA_CONTENT_IDEMPOTENT) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0}, {3.0, 2.0, 1.0}};
  std::vector<std::vector<bool>> undefs = {{false, false, false}, {false, false, false}};

  GeoDaWeight* w = new VectorWeight(TEST_NEIGHBORS);
  BatchLocalMoran* lisa =
      new BatchLocalMoran(3, w, data, undefs, 0.05, 1, 99, 12345);

  geoda::BatchLisaResult rst;
  rst.is_valid = false;
  geoda::set_batch_lisa_content(lisa, rst, 2);
  ASSERT_EQ(rst.lisa_values.size(), 2u);
  ASSERT_EQ(rst.sig_values.size(), 2u);

  // calling again on the same result must not accumulate duplicate rows
  geoda::set_batch_lisa_content(lisa, rst, 2);
  EXPECT_EQ(rst.lisa_values.size(), 2u);
  EXPECT_EQ(rst.sig_values.size(), 2u);
  EXPECT_EQ(rst.cluster_values.size(), 2u);
  EXPECT_EQ(rst.lag_values.size(), 2u);

  delete lisa;
  delete w;
}
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

TEST(LISA, MULTIVARIATE_QUANTILE_LISA_NO_COLOCATION_3_VARS) {
  // Three variables, none of which co-locate anywhere: the binned variables are
  // var0={1,0,0}, var1={0,1,1}, var2={1,0,0}, so every per-observation product
  // (zz) is 0. MultiJoinCount's no-colocation branch is bivariate-only (with
  // three or more variables it would silently use only data[0] and data[1]), so
  // the wrapper must fall through to the colocation branch and report all-zero
  // local statistics. Regression: the branch used to be entered for any variable
  // count, giving obs 0 a non-zero count of 1 from the dropped third variable.
  geoda::LisaResult result = geoda::local_multiquantilelisa(
      {2, 2, 2}, {1, 2, 1}, {{1, 3, 4}, {1, 5, 2}, {1, 2, 3}}, TEST_NEIGHBORS, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
      0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 3u);
  EXPECT_DOUBLE_EQ(result.lisa_vec[0], 0.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[1], 0.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[2], 0.0);
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
TEST(LISA, LOCAL_MORAN_EB) {
  std::vector<double> event_data = {3.0, 3.0, 4.0};
  std::vector<double> base_data = {10.0, 20.0, 15.0};
  std::vector<unsigned int> undefs = {0, 0, 0};

  geoda::LisaResult result = geoda::local_moran_eb(event_data, base_data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 3u);
  ASSERT_EQ(result.sig_local_vec.size(), 3u);

  // deterministic for a fixed seed (12345)
  EXPECT_NEAR(result.lisa_vec[0], -0.793958, 1e-4);
  EXPECT_NEAR(result.lisa_vec[1], -0.657177, 1e-4);
  EXPECT_NEAR(result.lisa_vec[2], -0.520397, 1e-4);
  EXPECT_NEAR(result.lag_vec[0], -1.14645, 1e-4);
  EXPECT_NEAR(result.lag_vec[1], 0.573226, 1e-4);
  EXPECT_NEAR(result.lag_vec[2], -1.14645, 1e-4);
  EXPECT_NEAR(result.sig_local_vec[0], 0.01, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[1], 0.01, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[2], 0.01, 1e-9);
  EXPECT_EQ(result.cluster_vec[0], 4);
  EXPECT_EQ(result.cluster_vec[1], 3);
  EXPECT_EQ(result.cluster_vec[2], 4);
  EXPECT_EQ(result.sig_cat_vec[0], 2);
  EXPECT_EQ(result.sig_cat_vec[1], 2);
  EXPECT_EQ(result.sig_cat_vec[2], 2);
}

TEST(LISA, LOCAL_MORAN_EB_JS_DATA) {
  // mirrors the JS wrapper's test input
  std::vector<double> event_data = {3.0, 3.0, 0.0, 9.0, 8.0, 8.5};
  std::vector<double> base_data = {10.0, 10.0, 5.0, 20.0, 18.0, 15.0};
  std::vector<std::vector<unsigned int>> nbrs = {{1}, {0}, {}, {4, 5}, {3, 5}, {3, 4}};
  std::vector<unsigned int> undefs = {};

  geoda::LisaResult result = geoda::local_moran_eb(event_data, base_data, nbrs, undefs, 0.05, 99, 1234567890);

  // deterministic for a fixed seed (1234567890); mirrors the JS wrapper test input
  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 6u);
  ASSERT_EQ(result.sig_local_vec.size(), 6u);

  EXPECT_NEAR(result.lisa_vec[0], 0.19301023686463248, 1e-9);
  EXPECT_NEAR(result.lisa_vec[1], 0.19301023686463248, 1e-9);
  EXPECT_NEAR(result.lisa_vec[2], 0.0, 1e-9);
  EXPECT_NEAR(result.lisa_vec[3], 0.5068767980725768, 1e-9);
  EXPECT_NEAR(result.lisa_vec[4], 0.46434548568689166, 1e-9);
  EXPECT_NEAR(result.lisa_vec[5], 0.6998879513302786, 1e-9);
  EXPECT_NEAR(result.lag_vec[0], -0.4393293034440481, 1e-9);
  EXPECT_NEAR(result.lag_vec[1], -0.4393293034440481, 1e-9);
  EXPECT_NEAR(result.lag_vec[2], 0.0, 1e-9);
  EXPECT_NEAR(result.lag_vec[3], 0.9156425120256456, 1e-9);
  EXPECT_NEAR(result.lag_vec[4], 0.9473554162403955, 1e-9);
  EXPECT_NEAR(result.lag_vec[5], 0.5218620897544903, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[0], 0.41, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[1], 0.41, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[2], 0.0, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[3], 0.06, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[4], 0.06, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[5], 0.06, 1e-9);
  EXPECT_EQ(result.cluster_vec[0], 0);
  EXPECT_EQ(result.cluster_vec[1], 0);
  EXPECT_EQ(result.cluster_vec[2], 6);
  EXPECT_EQ(result.cluster_vec[3], 0);
  EXPECT_EQ(result.cluster_vec[4], 0);
  EXPECT_EQ(result.cluster_vec[5], 0);
  EXPECT_EQ(result.sig_cat_vec[0], 0);
  EXPECT_EQ(result.sig_cat_vec[1], 0);
  EXPECT_EQ(result.sig_cat_vec[2], 5);
  EXPECT_EQ(result.sig_cat_vec[3], 0);
  EXPECT_EQ(result.sig_cat_vec[4], 0);
  EXPECT_EQ(result.sig_cat_vec[5], 0);
}

TEST(LISA, LOCAL_MORAN_EB_EMPTY_UNDEFS_NO_CRASH) {
  // The JS/WASM wrapper passes an empty undefs vector; eb_rate_standardize must
  // not index out of bounds when undefs is missing or shorter than num_obs.
  std::vector<double> event_data = {3.0, 3.0, 4.0};
  std::vector<double> base_data = {10.0, 20.0, 15.0};
  std::vector<unsigned int> undefs = {};

  geoda::LisaResult result = geoda::local_moran_eb(event_data, base_data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  EXPECT_EQ(result.lisa_vec.size(), 3u);
}

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

  // Pseudo-p values are seed-dependent, not uniform draws: obs 0 has a single
  // neighbor (degree 1), so the lookup-table permutation samples one varying
  // entry across the 99 permutations -> p ~ 0.44. obs 1 has degree 2 and always
  // samples both non-self values -> deterministic p = 0.01.
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
  // an undefined neighbor it could loop forever. The wrapper compacts undefined
  // observations away, so the lookup-table path samples only valid candidates and
  // must not hang. obs 1 has neighbors {0, 2} with obs 2 undefined: after
  // compaction each valid observation has a single valid permutation candidate
  // (the other observation), so the permuted join count always equals the observed
  // one and the pseudo-p is deterministically (0 + 1) / (99 + 1) = 0.01.
  std::vector<double> data = {1, 1, 0};
  std::vector<unsigned int> undefs = {0, 0, 1};

  geoda::LisaResult result = geoda::local_joincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 3u);

  EXPECT_DOUBLE_EQ(result.lisa_vec[0], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[1], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[2], 0.0);

  EXPECT_NEAR(result.sig_local_vec[0], 0.01, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[1], 0.01, 1e-9);
  EXPECT_DOUBLE_EQ(result.sig_local_vec[2], 0.0);

  // significance categories: 2 = p <= 0.01, 6 = undefined
  EXPECT_EQ(result.sig_cat_vec[0], 2);
  EXPECT_EQ(result.sig_cat_vec[1], 2);
  EXPECT_EQ(result.sig_cat_vec[2], 6);

  EXPECT_EQ(result.cluster_vec[0], 1);
  EXPECT_EQ(result.cluster_vec[1], 1);
  EXPECT_EQ(result.cluster_vec[2], 0);
}

TEST(LISA, MULTIVARIATE_JOIN_COUNT) {
  // two binary variables with a colocation (obs 0 and obs 2 have both = 1)
  std::vector<std::vector<double>> data = {{1, 0, 1}, {1, 0, 1}};
  std::vector<std::vector<unsigned int>> undefs = {{0, 0, 0}, {0, 0, 0}};

  geoda::LisaResult result =
      geoda::local_multijoincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  EXPECT_EQ(result.lisa_vec.size(), 3u);
  EXPECT_EQ(result.sig_local_vec.size(), 3u);
}

TEST(LISA, MULTIVARIATE_JOIN_COUNT_NO_COLOCATION_BIVARIATE) {
  // Two complementary variables with no colocation (each observation has exactly
  // one 1). MultiJoinCount's bivariate no-colocation branch counts the neighbor
  // joins where data[0] == 1, using data[1] as the join indicator: obs 0 and obs 2
  // are 1 in data[0] and each neighbor obs 1, whose data[1] == 1, so both report
  // a local count of 1; obs 1 is not a data[0] observation and reports 0.
  std::vector<std::vector<double>> data = {{1, 0, 1}, {0, 1, 0}};
  std::vector<std::vector<unsigned int>> undefs = {{0, 0, 0}, {0, 0, 0}};

  geoda::LisaResult result =
      geoda::local_multijoincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 3u);
  EXPECT_DOUBLE_EQ(result.lisa_vec[0], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[1], 0.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[2], 1.0);
}

TEST(LISA, MULTIVARIATE_JOIN_COUNT_EMPTY_VARIABLES) {
  // An empty variable set would make MultiJoinCount default zz to 1 and report a
  // bogus valid result; the wrapper rejects it.
  std::vector<std::vector<double>> data;
  std::vector<std::vector<unsigned int>> undefs;

  geoda::LisaResult result =
      geoda::local_multijoincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_FALSE(result.is_valid);
  ASSERT_EQ(result.sig_cat_vec.size(), 3u);
  EXPECT_EQ(result.sig_cat_vec[0], 6);
}

TEST(LISA, MULTIVARIATE_JOIN_COUNT_VARIABLE_LENGTH_MISMATCH) {
  // A variable shorter than the neighbor list must be rejected rather than read
  // past its buffer during compaction.
  std::vector<std::vector<double>> data = {{1, 0, 1}, {1, 0}};
  std::vector<std::vector<unsigned int>> undefs = {{0, 0, 0}, {0, 0, 0}};

  geoda::LisaResult result =
      geoda::local_multijoincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_FALSE(result.is_valid);
}

TEST(LISA, MULTIVARIATE_JOIN_COUNT_SINGLE_VARIABLE_REJECTED) {
  // One variable is not a multivariate colocation; pygeoda rejects n_vars <= 1.
  std::vector<std::vector<double>> data = {{1, 0, 1}};
  std::vector<std::vector<unsigned int>> undefs = {{0, 0, 0}};

  geoda::LisaResult result =
      geoda::local_multijoincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_FALSE(result.is_valid);
  ASSERT_EQ(result.sig_cat_vec.size(), 3u);
  EXPECT_EQ(result.sig_cat_vec[0], 6);
}

TEST(LISA, MULTIVARIATE_JOIN_COUNT_NON_COMPLEMENTARY_FORWARDED) {
  // No colocating observation, but not complementary either (obs 1 has both
  // variables 0; obs 0 and obs 2 each carry a single 1). Only the complementary
  // case is diverted to local_bijoincount, so this input is forwarded to
  // MultiJoinCount and yields a valid all-zero result.
  std::vector<std::vector<double>> data = {{1, 0, 0}, {0, 0, 1}};
  std::vector<std::vector<unsigned int>> undefs = {{0, 0, 0}, {0, 0, 0}};

  geoda::LisaResult result =
      geoda::local_multijoincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 3u);
  EXPECT_DOUBLE_EQ(result.lisa_vec[0], 0.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[1], 0.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[2], 0.0);
}

TEST(LISA, LOCAL_JOIN_COUNT_OUT_OF_RANGE_NEIGHBOR) {
  // A neighbor ID >= num_obs (a negative JS index arrives as a huge unsigned
  // value) must be rejected instead of indexing past the compaction map.
  std::vector<double> data = {1, 0, 1};
  std::vector<unsigned int> undefs = {0, 0, 0};
  std::vector<std::vector<unsigned int>> nbrs = {{1}, {0, 3}, {1}};

  geoda::LisaResult result = geoda::local_joincount(data, nbrs, undefs, 0.05, 99, 12345);

  EXPECT_FALSE(result.is_valid);
}

TEST(LISA, LOCAL_JOIN_COUNT_NON_BINARY_REJECTED) {
  // UniJoinCount treats any positive value as 1-valued and adds the raw neighbor
  // value to the lag, so a non-binary value (e.g. 2) would produce a meaningless
  // count; the wrapper rejects it with the invalid contract.
  std::vector<double> data = {1, 2, 1};
  std::vector<unsigned int> undefs = {0, 0, 0};

  geoda::LisaResult result = geoda::local_joincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_FALSE(result.is_valid);
  ASSERT_EQ(result.sig_cat_vec.size(), 3u);
  EXPECT_EQ(result.sig_cat_vec[0], 6);
}

TEST(LISA, MULTIVARIATE_JOIN_COUNT_NON_BINARY_REJECTED) {
  // MultiJoinCount multiplies raw values into its integer zz, so a non-binary
  // value would produce invalid colocations; the wrapper rejects it.
  std::vector<std::vector<double>> data = {{1, 0, 1}, {1, 2, 1}};
  std::vector<std::vector<unsigned int>> undefs = {{0, 0, 0}, {0, 0, 0}};

  geoda::LisaResult result =
      geoda::local_multijoincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_FALSE(result.is_valid);
  ASSERT_EQ(result.sig_cat_vec.size(), 3u);
  EXPECT_EQ(result.sig_cat_vec[0], 6);
}

TEST(LISA, MULTIVARIATE_JOIN_COUNT_COMPLEMENTARY_SUM_NOT_PAIRS) {
  // obs 0 and obs 1 are colocations (1,1), obs 2 and obs 3 are (0,0). The
  // marginal 1-counts sum to num_valid (2 + 2 == 4), but the rows are not
  // complementary, so this valid colocation input must be forwarded to
  // MultiJoinCount rather than rejected as a no-colocation case. Regression: the
  // guard used to compare only marginal sums and wrongly rejected inputs like
  // this.
  std::vector<std::vector<double>> data = {{1, 1, 0, 0}, {1, 1, 0, 0}};
  std::vector<std::vector<unsigned int>> undefs = {{0, 0, 0, 0}, {0, 0, 0, 0}};
  std::vector<std::vector<unsigned int>> nbrs = {{1}, {0}, {3}, {2}};

  geoda::LisaResult result =
      geoda::local_multijoincount(data, nbrs, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 4u);
  // Each colocation is the other's neighbor, so both report a local count of 1.
  EXPECT_DOUBLE_EQ(result.lisa_vec[0], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[1], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[2], 0.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[3], 0.0);
}

TEST(LISA, LOCAL_JOIN_COUNT_SELF_NEIGHBOR_FILTERED) {
  // obs 0 lists itself as a neighbor ({0, 1}). The join-count loops skip self,
  // but VectorWeight::GetNbrStats excludes self while GetNbrSize includes it,
  // so a self entry would make the lookup-table permutation too small and read
  // past it. The wrapper filters self-loops out of the compacted adjacency list.
  std::vector<double> data = {1, 1, 0};
  std::vector<unsigned int> undefs = {0, 0, 0};
  std::vector<std::vector<unsigned int>> nbrs = {{0, 1}, {0, 2}, {1}};

  geoda::LisaResult result = geoda::local_joincount(data, nbrs, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 3u);
  EXPECT_DOUBLE_EQ(result.lisa_vec[0], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[1], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[2], 0.0);
}

TEST(LISA, LOCAL_JOIN_COUNT_DUPLICATE_NEIGHBOR_FILTERED) {
  // obs 0 lists neighbor 1 twice ({1, 1}). A weights adjacency list should not
  // contain duplicates, and duplicate-heavy rows can make the permutation-table
  // sampler loop forever drawing unique candidates it cannot produce. The
  // wrapper deduplicates so each neighbor contributes once to the join count.
  std::vector<double> data = {1, 1, 0};
  std::vector<unsigned int> undefs = {0, 0, 0};
  std::vector<std::vector<unsigned int>> nbrs = {{1, 1}, {0, 2}, {1}};

  geoda::LisaResult result = geoda::local_joincount(data, nbrs, undefs, 0.05, 99, 12345);

  EXPECT_TRUE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 3u);
  EXPECT_DOUBLE_EQ(result.lisa_vec[0], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[1], 1.0);
  EXPECT_DOUBLE_EQ(result.lisa_vec[2], 0.0);
}

TEST(LISA, LOCAL_JOIN_COUNT_DATA_LENGTH_MISMATCH) {
  // data shorter than the neighbor list would read past the buffer during
  // compaction; the wrapper rejects it.
  std::vector<double> data = {1, 0};
  std::vector<unsigned int> undefs = {0, 0, 0};

  geoda::LisaResult result = geoda::local_joincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_FALSE(result.is_valid);
}

TEST(LISA, LOCAL_JOIN_COUNT_ALL_UNDEFINED) {
  // Every observation undefined compacts to zero observations. The wrapper must
  // not construct VectorWeight over an empty adjacency list (GetNbrStats would
  // read before its buffer when num_obs == 0); every observation is reported
  // undefined and is_valid stays false.
  std::vector<double> data = {1, 0, 1};
  std::vector<unsigned int> undefs = {1, 1, 1};

  geoda::LisaResult result = geoda::local_joincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_FALSE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 3u);
  EXPECT_EQ(result.sig_cat_vec[0], 6);
  EXPECT_EQ(result.sig_cat_vec[1], 6);
  EXPECT_EQ(result.sig_cat_vec[2], 6);
  EXPECT_DOUBLE_EQ(result.lisa_vec[0], 0.0);
  EXPECT_EQ(result.cluster_vec[0], 0);
  // neighbor counts still reported from the original weights structure
  EXPECT_EQ(result.nn_vec[0], 1);
  EXPECT_EQ(result.nn_vec[1], 2);
  EXPECT_EQ(result.nn_vec[2], 1);
}

TEST(LISA, MULTIVARIATE_JOIN_COUNT_ALL_UNDEFINED) {
  // Same contract as the univariate case: no VectorWeight over zero observations.
  std::vector<std::vector<double>> data = {{1, 0, 1}, {1, 0, 1}};
  std::vector<std::vector<unsigned int>> undefs = {{1, 1, 1}, {1, 1, 1}};

  geoda::LisaResult result =
      geoda::local_multijoincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_FALSE(result.is_valid);
  ASSERT_EQ(result.lisa_vec.size(), 3u);
  EXPECT_EQ(result.sig_cat_vec[0], 6);
  EXPECT_EQ(result.sig_cat_vec[1], 6);
  EXPECT_EQ(result.sig_cat_vec[2], 6);
  EXPECT_EQ(result.nn_vec[0], 1);
  EXPECT_EQ(result.nn_vec[1], 2);
  EXPECT_EQ(result.nn_vec[2], 1);
}
