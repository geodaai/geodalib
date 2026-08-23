// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <gtest/gtest.h>

#include "sa/lisa-api.h"

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
