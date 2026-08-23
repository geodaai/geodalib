// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#ifndef GEODA_DISTANCE_WEIGHTS_H
#define GEODA_DISTANCE_WEIGHTS_H
#include <vector>

#include "geometry/geometry.h"

namespace geoda {

const double pi = 3.141592653589793238463;
const double mile_per_degree = 69.0;
const double km_per_degree = 111.1;
const double pi_over_180 = pi / 180.0;
const double earth_radius_km = 6371.007180918475;
const double earth_radius_miles = earth_radius_km / 1.609344;

/**
 * @brief  Compute the haversine distance between two points
 * see https://www.movable-type.co.uk/scripts/latlong.html
 * @param lon1 The longitude of the first point
 * @param lat1 The latitude of the first point
 * @param lon2 The longitude of the second point
 * @param lat2 The latitude of the second point
 * @param is_mile If true, use mile as the distance unit, otherwise use kilometer
 * @return double The haversine distance
 */
double haversine_distance(double lon1, double lat1, double lon2, double lat2, bool is_mile);

/**
 * @brief Compute the distance weights for a collection of geometries
 *
 * @param geoms The geometry collection
 * @param distance_threshold The distance threshold
 * @param is_mile If true, use mile as the distance unit, otherwise use kilometer
 * @return std::vector<std::vector<unsigned int>>  The 2D vector of distance weights
 */
std::vector<std::vector<double>> distance_weights(const GeometryCollection& geoms, double distance_threshold,
                                                  bool is_mile);

/**
 * @brief Compute kernel weights for a collection of geometries using a fixed bandwidth.
 *
 * For each geometry, neighbors located within the given bandwidth are found. Each neighbor's
 * weight is computed by applying the chosen kernel to the distance ratio z = distance / bandwidth,
 * following Anselin and Rey (2010), table 5.4. Supported kernels: triangular, uniform, epanechnikov,
 * quartic and gaussian. Each row is stored as an interleaved list of [neighborIndex, weight] pairs,
 * with the diagonal (self) element appended last.
 *
 * @param geoms The geometry collection. If the collection is not a point collection, the centroids of
 * the geometries are used.
 * @param bandwidth The fixed bandwidth (in kilometers or miles) within which neighbors are considered.
 * @param kernel The kernel function to apply. One of: triangular, uniform, epanechnikov, quartic, gaussian.
 * @param is_mile If true, use mile as the distance unit, otherwise use kilometer.
 * @param use_kernel_diagonals If true, the diagonal (self) weight is kernel(1.0); otherwise it is 1.0.
 * @return std::vector<std::vector<double>> The 2D vector of kernel weights as [neighborIndex, weight] pairs.
 */
std::vector<std::vector<double>> kernel_weights(const GeometryCollection& geoms, double bandwidth,
                                                const std::string& kernel, bool is_mile,
                                                bool use_kernel_diagonals = false);

/**
 * @brief Get the distance thresholds, first threshold guarantee that each observation has at least one neighbor, and
 * the second threshold is the maximum distance between two observations
 *
 * @param geoms The geometry collection
 * @param is_mile If true, use mile as the distance unit, otherwise use kilometer
 * @return std::vector<double>
 */
std::vector<double> get_distance_thresholds(const GeometryCollection& geoms, bool is_mile);

/**
 * @brief  Compute k-nearest neighbors for a collection of geometries
 *
 * @param geoms The geometry collection. If the collection is not a point collection, the centroids of the geometries
 * are used.
 * @param k The number of nearest neighbors to compute
 * @return std::vector<std::vector<unsigned int>> The 2D vector of k-nearest neighbors
 */
std::vector<std::vector<unsigned int>> knearest_neighbors(const GeometryCollection& geoms, unsigned int k);

/**
 * @brief  Compute contiguity weights for a collection of geometries using the centroids of the geometries.
 * The centroids are used to create a voronoi diagram, which is then used to compute the contiguity weights.
 *
 * @param geoms The geometry collection, which could be point/line/polygon collection
 * @param is_queen If true, use queen contiguity, otherwise use rook contiguity
 * @param precision_threshold The precision threshold for comparing coordinates to determine if two points are the same
 * @return std::vector<std::vector<unsigned int>> The 2D vector of contiguity weights
 */
std::vector<std::vector<unsigned int>> point_contiguity_weights(const GeometryCollection& geoms, bool is_queen,
                                                                double precision_threshold = 0.0,
                                                                unsigned int order_contiguity = 1,
                                                                bool include_lower_order = false);

/**
 * @brief  Compute contiguity weights for a collection of polygons
 *
 * @param geoms The polygon collection.
 * @param is_queen If true, use queen contiguity, otherwise use rook contiguity
 * @param precision_threshold  The precision threshold for comparing coordinates to determine if two points are the same
 * @return std::vector<std::vector<unsigned int>> The 2D vector of contiguity weights
 */
std::vector<std::vector<unsigned int>> polygon_contiguity_weights(const GeometryCollection& geoms, bool is_queen,
                                                                  double precision_threshold = 0.0,
                                                                  unsigned int order_contiguity = 1,
                                                                  bool include_lower_order = false);

/**
 * @brief  Compute contiguity weights for a collection of polygons using a threshold for precision
 *
 * @param geoms The polygon collection
 * @param is_queen If true, use queen contiguity, otherwise use rook contiguity
 * @param precision_threshold The precision threshold for comparing coordinates to determine if two points are the same
 * @param order_contiguity The order of contiguity
 * @param include_lower_order If true, include lower order contiguity
 * @return std::vector<std::vector<unsigned int>> The 2D vector of contiguity weights
 */
std::vector<std::vector<unsigned int>> polygon_contiguity_weights_threshold(const GeometryCollection& geoms,
                                                                            bool is_queen,
                                                                            double precision_threshold = 0.0,
                                                                            unsigned int order_contiguity = 1,
                                                                            bool include_lower_order = false);
/**
 * @brief Compute queen contiguity weights for a collection of polygons using a simple algorithm
 *
 * @param geoms The polygon collection
 * @param order_contiguity The order of contiguity
 * @param include_lower_order If true, include lower order contiguity
 * @return std::vector<std::vector<unsigned int>> The 2D vector of contiguity weights
 */
std::vector<std::vector<unsigned int>> simple_polygon_queen_weights(const GeometryCollection& geoms,
                                                                    unsigned int order_contiguity = 1,
                                                                    bool include_lower_order = false);

/**
 * @brief Compute rook contiguity weights for a collection of polygons using a simple algorithm
 *
 * @param geoms The polygon collection
 * @param order_contiguity The order of contiguity
 * @param include_lower_order If true, include lower order contiguity
 * @return std::vector<std::vector<unsigned int>> The 2D vector of contiguity weights
 */
std::vector<std::vector<unsigned int>> simple_polygon_rook_weights(const GeometryCollection& geoms,
                                                                   unsigned int order_contiguity = 1,
                                                                   bool include_lower_order = false);

/**
 * @brief Check if two points are equal within a precision threshold
 *
 * @param p1 The first point
 * @param p2 The second point
 * @param precision_threshold The precision threshold
 * @return true
 * @return false
 */
bool points_equals(const point_type& p1, const point_type& p2, double precision_threshold);

/**
 * @brief Check if two bounding boxes intersect
 *
 * @param b1 The first bounding box
 * @param b2 The second bounding box
 * @return true
 * @return false
 */
bool bbox_intersects(const box_type& b1, const box_type& b2);

}  // namespace geoda
#endif  // GEODA_DISTANCE_WEIGHTS_H
