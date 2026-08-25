#include <stdexcept>

#include <gtest/gtest.h>

#include "clustering/clustering-api.h"
#include "test/data.h"

// Points 0<->1<->2<->3<->4 in a chain.
static std::vector<std::vector<unsigned int>> TEST_NEIGHBORS = {{1}, {0, 2}, {1, 3}, {2, 4}, {3}};

// Points 0<->1<->2 in a chain (used by the spatial-contiguity tests).
static std::vector<std::vector<unsigned int>> SPATIAL_TEST_NEIGHBORS = {{1}, {0, 2}, {1}};

TEST(CLUSTERING, SCHC) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<double> bound_vals;
  std::vector<std::vector<int>> result =
      geoda::schc(2, TEST_NEIGHBORS, data, "raw", "ward", "euclidean", bound_vals, 0.0);

  // 2 clusters, all 5 observations covered
  int count = 0;
  for (const auto& c : result) count += static_cast<int>(c.size());
  EXPECT_EQ(count, 5);

  // Variables of unequal length must be rejected instead of silently scaling
  // an inconsistent data matrix.
  std::vector<std::vector<double>> mismatched = {{1.0, 2.0, 3.0, 4.0, 5.0}, {1.0, 2.0, 3.0}};
  EXPECT_THROW(geoda::schc(2, TEST_NEIGHBORS, mismatched, "standardize", "ward", "euclidean", bound_vals, 0.0),
               std::invalid_argument);
  // the raw path must reject a length mismatch too (it would otherwise hand
  // out-of-bounds indices to the wrappers)
  EXPECT_THROW(geoda::schc(2, TEST_NEIGHBORS, mismatched, "raw", "ward", "euclidean", bound_vals, 0.0),
               std::invalid_argument);
}

TEST(CLUSTERING, REDCAP) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<double> bound_vals;
  std::vector<std::vector<int>> result =
      geoda::redcap(2, TEST_NEIGHBORS, data, "raw", "firstorder-singlelinkage", "euclidean", bound_vals, 0.0);
  int count = 0;
  for (const auto& c : result) count += static_cast<int>(c.size());
  EXPECT_EQ(count, 5);
}

TEST(CLUSTERING, SKATER) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<double> bound_vals;
  std::vector<std::vector<int>> result =
      geoda::skater(2, TEST_NEIGHBORS, data, "raw", "euclidean", bound_vals, 0.0);
  int count = 0;
  for (const auto& c : result) count += static_cast<int>(c.size());
  EXPECT_EQ(count, 5);
}

TEST(CLUSTERING, SPATIAL_VALIDATION) {
  // non-collinear points so the convex hull is not degenerate
  geoda::PointCollection pts(std::vector<double>{0, 1, 0, 2, 2}, std::vector<double>{0, 0, 2, 1, 2},
                             std::vector<unsigned int>{0, 1, 2, 3, 4}, std::vector<unsigned int>{1, 1, 1, 1, 1});
  std::vector<int> clusters = {1, 1, 1, 2, 2};
  ValidationResult result = geoda::spatial_validation(clusters, TEST_NEIGHBORS, pts);
  EXPECT_EQ(result.spatially_constrained, true);

  // A cluster label list shorter than the neighbor list would index out of
  // bounds; it must be rejected up front.
  std::vector<int> short_clusters = {1, 1, 1};
  EXPECT_THROW(geoda::spatial_validation(short_clusters, TEST_NEIGHBORS, pts), std::invalid_argument);
}

TEST(CLUSTERING, AZP_GREEDY) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<std::vector<int>> result = geoda::azp_greedy(2, TEST_NEIGHBORS, data, 10, "euclidean", 1234567);
  int count = 0;
  for (const auto& c : result) count += static_cast<int>(c.size());
  EXPECT_EQ(count, 5);
}

TEST(CLUSTERING, MAXP_GREEDY) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<std::vector<int>> result = geoda::maxp_greedy(TEST_NEIGHBORS, data, 10, "euclidean", 1234567);
  int count = 0;
  for (const auto& c : result) count += static_cast<int>(c.size());
  EXPECT_EQ(count, 5);
}

namespace {

void expect_partition(const std::vector<std::vector<int>>& clusters, int num_obs) {
  std::vector<bool> seen(num_obs, false);
  for (const auto& c : clusters) {
    for (int e : c) {
      ASSERT_GE(e, 0);
      ASSERT_LT(e, num_obs);
      EXPECT_FALSE(seen[e]) << "observation " << e << " appears in more than one cluster";
      seen[e] = true;
    }
  }
  for (int i = 0; i < num_obs; ++i) {
    EXPECT_TRUE(seen[i]) << "observation " << i << " is missing from the result";
  }
}

void expect_contiguous(const std::vector<std::vector<int>>& clusters,
                       const std::vector<std::vector<unsigned int>>& neighbors) {
  for (const auto& c : clusters) {
    ASSERT_FALSE(c.empty());
    // BFS from the first element restricted to the cluster; every member must
    // be reachable for the cluster to form a connected subgraph.
    std::vector<int> cluster = c;
    std::sort(cluster.begin(), cluster.end());
    std::vector<bool> visited(neighbors.size(), false);
    std::vector<int> stack = {c[0]};
    visited[c[0]] = true;
    int reached = 0;
    while (!stack.empty()) {
      int cur = stack.back();
      stack.pop_back();
      ++reached;
      for (unsigned int nbr : neighbors[cur]) {
        if (!visited[nbr] && std::binary_search(cluster.begin(), cluster.end(), static_cast<int>(nbr))) {
          visited[nbr] = true;
          stack.push_back(nbr);
        }
      }
    }
    EXPECT_EQ(reached, static_cast<int>(c.size()))
        << "cluster is not connected under the spatial weights";
  }
}

}  // namespace

TEST(CLUSTERING, SPENC) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<std::vector<int>> result = geoda::spenc(2, TEST_NEIGHBORS, data, "standardize", 1.0, 10, 123456789);
  expect_partition(result, 5);
  EXPECT_EQ(result.size(), 2u);
}

TEST(CLUSTERING, SPENC_DETERMINISTIC) {
  // Same seed must produce identical clusters across runs.
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<std::vector<int>> r1 = geoda::spenc(2, TEST_NEIGHBORS, data, "standardize", 1.0, 10, 123456789);
  std::vector<std::vector<int>> r2 = geoda::spenc(2, TEST_NEIGHBORS, data, "standardize", 1.0, 10, 123456789);
  ASSERT_EQ(r1.size(), 2u);
  ASSERT_EQ(r2.size(), 2u);
  EXPECT_EQ(r1, r2);
}

TEST(CLUSTERING, SPENC_VALIDATES) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  // k > n must be rejected (SCHC test pattern).
  EXPECT_THROW(geoda::spenc(6, TEST_NEIGHBORS, data, "standardize", 1.0, 10, 123456789), std::invalid_argument);
  // gamma must be positive.
  EXPECT_THROW(geoda::spenc(2, TEST_NEIGHBORS, data, "standardize", 0.0, 10, 123456789), std::invalid_argument);
  // n_init must be at least 1.
  EXPECT_THROW(geoda::spenc(2, TEST_NEIGHBORS, data, "standardize", 1.0, 0, 123456789), std::invalid_argument);
}

TEST(CLUSTERING, PAM) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<std::vector<int>> result = geoda::pam(2, data, "euclidean", 100, "LAB", 0.01, 123456789);
  expect_partition(result, 5);
  EXPECT_EQ(result.size(), 2u);
}

TEST(CLUSTERING, PAM_MANHATTAN) {
  // Manhattan distance is case-insensitively accepted.
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  std::vector<std::vector<int>> result = geoda::pam(2, data, "MANHATTAN", 100, "BUILD", 0.01, 123456789);
  expect_partition(result, 5);
}

TEST(CLUSTERING, PAM_VALIDATES) {
  std::vector<std::vector<double>> data = {{1.0, 2.0, 3.0, 4.0, 5.0}};
  // k > n must be rejected.
  EXPECT_THROW(geoda::pam(6, data, "euclidean", 100, "LAB", 0.01, 123456789), std::invalid_argument);
  // empty data must be rejected.
  std::vector<std::vector<double>> empty;
  EXPECT_THROW(geoda::pam(2, empty, "euclidean", 100, "LAB", 0.01, 123456789), std::invalid_argument);
}

TEST(CLUSTERING, MAKE_SPATIAL) {
  // cluster 0 = {0,1}, cluster 1 = {2} (already spatially contiguous)
  std::vector<std::vector<int>> clusters = {{0, 1}, {2}};
  std::vector<std::vector<int>> result = geoda::make_spatial(clusters, SPATIAL_TEST_NEIGHBORS);

  expect_partition(result, 3);
  expect_contiguous(result, SPATIAL_TEST_NEIGHBORS);
}

TEST(CLUSTERING, MAKE_SPATIAL_REASSIGNS_DISCONNECTED) {
  // cluster 0 = {0,2} is disconnected in the chain 0-1-2: 0 and 2 are not
  // adjacent, so element 2 must be reassigned into cluster 1.
  std::vector<std::vector<int>> clusters = {{0, 2}, {1}};
  std::vector<std::vector<int>> result = geoda::make_spatial(clusters, SPATIAL_TEST_NEIGHBORS);

  expect_partition(result, 3);
  expect_contiguous(result, SPATIAL_TEST_NEIGHBORS);

  // 0 and 2 can no longer share a cluster (they are not spatially adjacent)
  for (const auto& c : result) {
    bool has0 = std::find(c.begin(), c.end(), 0) != c.end();
    bool has2 = std::find(c.begin(), c.end(), 2) != c.end();
    EXPECT_FALSE(has0 && has2);
  }
}

TEST(CLUSTERING, MAKE_SPATIAL_REJECTS_EMPTY_CLUSTER) {
  // An empty cluster would leave MakeSpatialCluster with a null core; the
  // partition must be rejected without any accessor dereferencing null. Invalid
  // input returns the original clusters unchanged.
  std::vector<std::vector<int>> clusters = {{0, 1}, {2}, {}};
  std::vector<std::vector<int>> result = geoda::make_spatial(clusters, SPATIAL_TEST_NEIGHBORS);

  ASSERT_EQ(result.size(), 3u);
  EXPECT_TRUE(result[2].empty());
}

TEST(CLUSTERING, MAKE_SPATIAL_REJECTS_OUT_OF_RANGE_ELEMENT) {
  // An out-of-range cluster element would reach weights->GetNeighbors() with an
  // unchecked index inside MakeSpatialCluster; the partition must be rejected
  // and the input returned unchanged instead of crashing.
  std::vector<std::vector<int>> clusters = {{0, 1}, {2, 5}};
  std::vector<std::vector<int>> result = geoda::make_spatial(clusters, SPATIAL_TEST_NEIGHBORS);

  ASSERT_EQ(result.size(), 2u);
  EXPECT_EQ(result[1], (std::vector<int>{2, 5}));
}
