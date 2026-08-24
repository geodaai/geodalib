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

TEST(LISA, MULTIVARIATE_JOIN_COUNT_NO_COLOCATION_REJECTED) {
  // Two variables with no observation where both are 1: the complementary
  // bivariate no-colocation case belongs to local_bijoincount, so the
  // multivariate wrapper rejects it (is_valid stays false, undefined markers).
  std::vector<std::vector<double>> data = {{1, 0, 1}, {0, 1, 0}};
  std::vector<std::vector<unsigned int>> undefs = {{0, 0, 0}, {0, 0, 0}};

  geoda::LisaResult result =
      geoda::local_multijoincount(data, TEST_NEIGHBORS, undefs, 0.05, 99, 12345);

  EXPECT_FALSE(result.is_valid);
  ASSERT_EQ(result.sig_cat_vec.size(), 3u);
  EXPECT_EQ(result.sig_cat_vec[0], 6);
  EXPECT_DOUBLE_EQ(result.lisa_vec[0], 0.0);
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
