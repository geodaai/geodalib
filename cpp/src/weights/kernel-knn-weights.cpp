// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <cctype>
#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "geometry/geometry.h"
#include "weights/weights.h"

namespace bg = boost::geometry;
typedef std::pair<point_type, unsigned> point_val;

namespace bgi = boost::geometry::index;
typedef bgi::rtree<point_val, bgi::quadratic<32>> rtree_point_t;

using namespace geoda;

namespace {

std::string to_lower(const std::string& str) {
  std::string lower = str;
  for (char& c : lower) {
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  }
  return lower;
}

/**
 * @brief Apply a kernel function to the distance ratio z = distance / bandwidth.
 *
 * The kernel functions follow Anselin and Rey (2010), table 5.4.
 *
 * @param kernel The name of the kernel. Supported: triangular, uniform, epanechnikov, quartic, gaussian.
 * @param z The distance ratio (distance / bandwidth).
 * @return double The kernel weight.
 */
double kernel_value(const std::string& kernel, double z) {
  const std::string k = to_lower(kernel);
  if (k == "triangular") {
    return 1.0 - z;
  }
  if (k == "uniform") {
    return 0.5;
  }
  if (k == "epanechnikov") {
    return (3.0 / 4.0) * (1.0 - z * z);
  }
  if (k == "quartic") {
    return (15.0 / 16.0) * std::pow(1.0 - z * z, 2.0);
  }
  if (k == "gaussian") {
    return (1.0 / std::sqrt(2.0 * geoda::pi)) * std::exp(-z * z / 2.0);
  }
  throw std::invalid_argument("Unsupported kernel: " + kernel);
}

}  // namespace

std::vector<std::vector<double>> geoda::kernel_knn_weights(const GeometryCollection& geoms, unsigned int k,
                                                           const std::string& kernel, bool is_mile,
                                                           bool use_kernel_diagonals, double power,
                                                           bool adaptive_bandwidth, bool is_inverse) {
  // create rtree
  std::vector<point_val> pts;
  size_t num_geoms = geoms.size();
  for (size_t i = 0; i < num_geoms; ++i) {
    point_type pt = geoms.get_centroid(i);
    pts.emplace_back(pt, i);
  }
  rtree_point_t rtree(pts);

  const std::string kern = to_lower(kernel);

  // store, for each observation, the interleaved [neighborIndex, distance] pairs
  std::vector<std::vector<double>> raw_weights(num_geoms);
  std::vector<double> local_bandwidth(num_geoms, 0.0);
  double global_bandwidth = 0.0;

  const unsigned int query_k = k + 1;

  for (rtree_point_t::const_query_iterator it = rtree.qbegin(bgi::intersects(rtree.bounds())); it != rtree.qend();
       ++it) {
    const point_val& v = *it;
    size_t orig_idx = v.second;

    double x = v.first.get<0>();
    double y = v.first.get<1>();

    std::vector<point_val> q;
    rtree.query(bgi::nearest(v.first, query_k), std::back_inserter(q));

    unsigned int cnt = 0;
    for (auto& nbr : q) {
      if (nbr.second == v.second) {
        continue;
      }
      double x1 = nbr.first.get<0>();
      double y1 = nbr.first.get<1>();
      double d = haversine_distance(x, y, x1, y1, is_mile);
      if (d > local_bandwidth[orig_idx]) local_bandwidth[orig_idx] = d;
      if (d > global_bandwidth) global_bandwidth = d;
      raw_weights[orig_idx].push_back(static_cast<double>(nbr.second));
      raw_weights[orig_idx].push_back(d);
      ++cnt;
      if (cnt >= k) {
        break;
      }
    }
  }

  std::vector<std::vector<double>> result(num_geoms);
  for (size_t i = 0; i < num_geoms; ++i) {
    double b = adaptive_bandwidth ? local_bandwidth[i] : global_bandwidth;
    // normalize and apply kernel to each neighbor pair
    for (size_t j = 0; j + 1 < raw_weights[i].size(); j += 2) {
      double d = raw_weights[i][j + 1];
      if (is_inverse) d = std::pow(d, power);
      double z = (b > 0.0) ? d / b : 0.0;
      result[i].push_back(raw_weights[i][j]);
      result[i].push_back(kernel_value(kern, z));
    }
    // append self (diagonal) element
    double self_weight = use_kernel_diagonals ? kernel_value(kern, 0.0) : 1.0;
    result[i].push_back(static_cast<double>(i));
    result[i].push_back(self_weight);
  }

  return result;
}
