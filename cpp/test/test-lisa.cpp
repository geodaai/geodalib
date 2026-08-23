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
  EXPECT_EQ(result.lisa_vec.size(), 3u);
  EXPECT_EQ(result.sig_local_vec.size(), 3u);
}
