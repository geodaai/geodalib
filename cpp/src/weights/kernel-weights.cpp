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
 * @param z The distance ratio (distance / bandwidth) in the range [0, 1].
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

std::vector<std::vector<double>> geoda::kernel_weights(const GeometryCollection& geoms, double bandwidth,
                                                       const std::string& kernel, bool is_mile,
                                                       bool use_kernel_diagonals, double power) {
  // create rtree
  std::vector<point_val> pts;
  size_t num_geoms = geoms.size();
  for (size_t i = 0; i < num_geoms; ++i) {
    point_type pt = geoms.get_centroid(i);
    pts.emplace_back(pt, i);
  }
  rtree_point_t rtree(pts);

  // normalize the kernel name once so it is validated up-front
  const std::string k = to_lower(kernel);

  std::vector<std::vector<double>> result(num_geoms);

  // visit all element in rtree
  for (rtree_point_t::const_query_iterator it = rtree.qbegin(bgi::intersects(rtree.bounds())); it != rtree.qend();
       ++it) {
    const point_val& v = *it;
    size_t orig_idx = v.second;

    // convert bandwidth to degree
    double bandwidth_deg = bandwidth / (is_mile ? mile_per_degree : km_per_degree);

    // create bbox using bandwidth
    double x = v.first.get<0>();
    double y = v.first.get<1>();
    box_type b(point_type(x - bandwidth_deg, y - bandwidth_deg),
               point_type(x + bandwidth_deg, y + bandwidth_deg));

    // each point "v" with index "obs"
    std::vector<point_val> q;
    rtree.query(bgi::intersects(b), std::back_inserter(q));

    for (auto& nbr : q) {
      if (nbr.second == v.second) {
        continue;
      }
      double x1 = nbr.first.get<0>();
      double y1 = nbr.first.get<1>();
      double d = haversine_distance(x, y, x1, y1, is_mile);
      if (d <= bandwidth) {
        // The kernel ratio is z = distance^power / bandwidth (power defaults to 1.0).
        double w_val = d;
        if (power != 1.0) {
          w_val = std::pow(d, power);
        }
        double z = w_val / bandwidth;
        result[orig_idx].push_back(nbr.second);
        result[orig_idx].push_back(kernel_value(k, z));
      }
    }

    // append self (diagonal) element
    double self_weight = use_kernel_diagonals ? kernel_value(k, 1.0) : 1.0;
    result[orig_idx].push_back(orig_idx);
    result[orig_idx].push_back(self_weight);
  }

  return result;
}
