// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project


#ifdef __EMCC__
#include <emscripten/bind.h>

#include <iostream>

#include "geometry/geometry.h"
#include "geometry/spatial-join.h"
#include "sa/lisa-api.h"
#include "weights/weights.h"
#include "mapping/mapping.h"
#include "data/data.h"
#include "regression/diagnostic-report.h"
#include "regression/regression.h"

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

  emscripten::class_<geoda::LisaResult>("LisaResult")
      .function("isValid", &geoda::LisaResult::get_is_valid)
      .function("getPValues", &geoda::LisaResult::get_sig_local)
      .function("getClusters", &geoda::LisaResult::get_cluster)
      .function("getLagValues", &geoda::LisaResult::get_lag)
      .function("getLisaValues", &geoda::LisaResult::get_lisa)
      .function("getSignificanceCategories", &geoda::LisaResult::get_sig_cat)
      .function("getNN", &geoda::LisaResult::get_nn)
      .function("getLabels", &geoda::LisaResult::get_labels)
      .function("getColors", &geoda::LisaResult::get_colors);

  emscripten::function("localMoran", &geoda::local_moran);
  emscripten::function("localG", &geoda::local_g);
  emscripten::function("bivariateLocalMoran", &geoda::local_bivariate_moran);
  emscripten::function("localGeary", &geoda::local_geary);
  emscripten::function("multivariateLocalGeary", &geoda::local_multivariate_geary);
  emscripten::function("localJoinCount", &geoda::local_joincount);
  emscripten::function("multivariateLocalJoinCount", &geoda::local_multijoincount);
  emscripten::function("quantileLisa", &geoda::quantile_lisa);
}
#endif
