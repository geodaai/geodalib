// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project


#ifdef __EMCC__
#include <emscripten/bind.h>

#include <iostream>

#include "geometry/cartogram.h"
#include "geometry/geometry.h"
#include "geometry/line.h"
#include "geometry/mst.h"
#include "geometry/point.h"
#include "geometry/polygon.h"
#include "geometry/spatial-dissolve.h"
#include "geometry/spatial-join.h"
#include "geometry/thiessen-polygon.h"
#include "mapping/mapping.h"
#include "regression/diagnostic-report.h"
#include "regression/regression.h"
#include "sa/lisa-api.h"
#include "data/data.h"
#include "weights/weights.h"
#include "clustering/clustering-api.h"

template <typename T>
emscripten::class_<std::vector<T>> register_vector_with_smart_ptrs(const char* name) {
  typedef std::vector<T> VecType;
  void (VecType::*push_back)(const T&) = &VecType::push_back;
  void (VecType::*resize)(const size_t, const T&) = &VecType::resize;
  return emscripten::class_<VecType>(name)
      .template constructor<>()
      .template smart_ptr<std::shared_ptr<VecType>>(name)
      .function("push_back", push_back)
      .function("resize", resize)
      .function("size", &VecType::size)
      .function("get", &emscripten::internal::VectorAccess<VecType>::get)
      .function("set", &emscripten::internal::VectorAccess<VecType>::set);
}

EMSCRIPTEN_BINDINGS(wasmgeoda) {
  emscripten::register_vector<unsigned int>("VectorUInt");
  emscripten::register_vector<std::vector<unsigned int>>("VecVecUInt");
  emscripten::register_vector<int>("VectorInt");
  emscripten::register_vector<std::vector<int>>("VecVecInt");
  emscripten::register_vector<double>("VectorDouble");
  emscripten::register_vector<std::vector<double>>("VecVecDouble");
  emscripten::register_vector<std::string>("VectorString");

  emscripten::class_<geoda::GeometryCollection>("GeometryCollection")
      .function("getCentroids", &geoda::GeometryCollection::get_centroids)
      .function("buffer", &geoda::GeometryCollection::buffer)
      .function("size", &geoda::GeometryCollection::size)
      .function("getArea", &geoda::GeometryCollection::get_area)
      .function("getLength", &geoda::GeometryCollection::get_length)
      .function("getPerimeter", &geoda::GeometryCollection::get_perimeter)
      .function("getType", &geoda::GeometryCollection::get_type);
  emscripten::class_<geoda::PolygonCollection, emscripten::base<geoda::GeometryCollection>>("PolygonCollection")
      .constructor<std::vector<double>, std::vector<double>, std::vector<unsigned int>, std::vector<unsigned int>,
                   std::vector<unsigned int>, bool, bool>()
      .function("getType", &geoda::PolygonCollection::get_type);
  emscripten::class_<geoda::LineCollection, emscripten::base<geoda::GeometryCollection>>("LineCollection")
      .constructor<std::vector<double>, std::vector<double>, std::vector<unsigned int>, std::vector<unsigned int>,
                   bool>()
      .function("getType", &geoda::LineCollection::get_type);
  emscripten::class_<geoda::PointCollection, emscripten::base<geoda::GeometryCollection>>("PointCollection")
      .constructor<std::vector<double>, std::vector<double>, std::vector<unsigned int>, std::vector<unsigned int>,
                   bool>()
      .function("getType", &geoda::PointCollection::get_type);

  emscripten::class_<geoda::Point>("Point")
      .constructor()
      .function("getX", &geoda::Polygon::get_x)
      .function("getY", &geoda::Polygon::get_y);

  emscripten::class_<geoda::Polygon>("Polygon")
      .constructor()
      .function("getX", &geoda::Polygon::get_x)
      .function("getY", &geoda::Polygon::get_y)
      .function("getHoles", &geoda::Polygon::get_holes)
      .function("getParts", &geoda::Polygon::get_parts)
      .function("addPart", &geoda::Polygon::add);

  emscripten::class_<geoda::Line>("Line")
      .constructor()
      .function("getX", &geoda::Line::get_x)
      .function("getY", &geoda::Line::get_y)
      .function("getParts", &geoda::Line::get_parts)
      .function("add", &geoda::Line::add);

  emscripten::class_<geoda::CartogramResult>("CartogramResult")
      .function("getX", &geoda::CartogramResult::get_x)
      .function("getY", &geoda::CartogramResult::get_y)
      .function("getRadius", &geoda::CartogramResult::get_radius)
      .function("getCircles", &geoda::CartogramResult::get_circles);

  emscripten::register_vector<geoda::Polygon>("VectorPolygon");
  emscripten::register_vector<geoda::Line>("VectorLine");
  emscripten::function("spatialJoin", &geoda::spatial_join);
  emscripten::function("spatialDissolve", &geoda::spatial_dissolve);
  emscripten::function("thiessenPolygon", &geoda::thiessen_polygon);
  emscripten::function("mst", &geoda::mst);
  emscripten::function("cartogram", &geoda::cartogram);

  emscripten::function("getNearestNeighbors", &geoda::knearest_neighbors);
  emscripten::function("makeSpatial", &geoda::make_spatial);
  emscripten::function("neighborMatchTest", &geoda::neighbor_match_test);
  emscripten::function("getKernelKnnWeights", &geoda::kernel_knn_weights);
  emscripten::function("getDistanceWeights", &geoda::distance_weights);
  emscripten::function("getKernelWeights", &geoda::kernel_weights);
  emscripten::function("getDistanceThresholds", &geoda::get_distance_thresholds);
  emscripten::function("getPolygonContiguityWeights", &geoda::polygon_contiguity_weights);
  emscripten::function("getPointContiguityWeights", &geoda::point_contiguity_weights);

  emscripten::function("quantileBreaks", &geoda::quantile_breaks);
  emscripten::function("naturalBreaks", &geoda::natural_breaks);
  emscripten::function("equalIntervalBreaks", &geoda::equal_interval_breaks);
  emscripten::function("percentileBreaks", &geoda::percentile_breaks);
  emscripten::function("boxBreaks", &geoda::box_breaks);
  emscripten::function("standardDeviationBreaks", &geoda::std_dev_breaks);

  emscripten::function("deviationFromMean", &geoda::deviation_from_mean);
  emscripten::function("standardizeMAD", &geoda::standardize_mad);
  emscripten::function("rangeAdjust", &geoda::range_adjust);
  emscripten::function("rangeStandardize", &geoda::range_standardize);
  emscripten::function("standardize", &geoda::standardize_data_wasm);

  emscripten::function("rawRate", &geoda::raw_rate);
  emscripten::function("excessRisk", &geoda::excess_risk);
  emscripten::function("empiricalBayes", &geoda::empirical_bayes);
  emscripten::function("spatialRate", &geoda::spatial_rate);
  emscripten::function("rateStandardizeEmpiricalBayes", &geoda::rate_standardize_empirical_bayes);
  emscripten::function("spatialEmpiricalBayes", &geoda::spatial_empirical_bayes);
}
#endif
