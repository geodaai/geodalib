// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <gtest/gtest.h>

#include "sa/lisa-api.h"

// Points at (0,0), (1,1), (2,2) form a 1D chain: 0 <-> 1 <-> 2.
static std::vector<std::vector<unsigned int>> TEST_NEIGHBORS = {{1}, {0, 2}, {1}};

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
  EXPECT_NEAR(result.sig_local_vec[0], 0.28, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[1], 0.15, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[2], 0.0, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[3], 0.14, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[4], 0.16, 1e-9);
  EXPECT_NEAR(result.sig_local_vec[5], 0.23, 1e-9);
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
