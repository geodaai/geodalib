set (WEIGHTS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/geometry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/weights/distance-weights.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/weights/neighbor-match-test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/weights/kernel-knn-weights.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/weights/kernel-weights.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/weights/contiguity-weights.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/weights/geoda-weight.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/weights/vector-weight.cpp
)
set (WEIGHTS_SOURCE_FILES ${WEIGHTS_SOURCE_FILES} PARENT_SCOPE)

set (CORE_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/spatial-join.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/spatial-dissolve.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/point.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/line.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/polygon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/thiessen-polygon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/mst.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/cartogram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mapping/quantile-breaks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mapping/natural-breaks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mapping/equal-interval-breaks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mapping/percentile-breaks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mapping/box-breaks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mapping/stddev-breaks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mapping/rates-map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/data/deviation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/data/mad.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/data/range_adjust.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/data/range_standardize.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/data/standardize.cpp
    ${WEIGHTS_SOURCE_FILES}
)
set (CORE_SOURCE_FILES ${CORE_SOURCE_FILES} PARENT_SCOPE)

set (LISA_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/mapping/quantile-breaks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/data/standardize.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sa/local-moran.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sa/local-g.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sa/local-geary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sa/quantile-lisa.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sa/extra-lisa.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lisa/LISA.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lisa/UniLocalMoran.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lisa/BiLocalMoran.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lisa/UniG.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lisa/UniJoinCount.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lisa/MultiJoinCount.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lisa/BatchLISA.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lisa/BatchLocalMoran.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lisa/UniGstar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lisa/UniGeary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lisa/MultiGeary.cpp
)
set (LISA_SOURCE_FILES ${LISA_SOURCE_FILES} PARENT_SCOPE)

set (REGRESSION_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/regression/regression.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/regression/dense-vector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/regression/mix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/regression/diagnostic-report.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/regression/ML_im.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/regression/PowerLag.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/regression/PowerSymLag.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/regression/SparseVector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/regression/SparseMatrix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/regression/SparseRow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/regression/Weights.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/regression/smile2.cpp
)
set (REGRESSION_SOURCE_FILES ${REGRESSION_SOURCE_FILES} PARENT_SCOPE)

set (GEODA_SOURCE_FILES
    ${REGRESSION_SOURCE_FILES}
    ${CORE_SOURCE_FILES}
    ${LISA_SOURCE_FILES}
    PARENT_SCOPE)

set (GEODA_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/geometry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/point.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/line.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/polygon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/thiessen-polygon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/mst.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/spatial-join.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/spatial-dissolve.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/cartogram.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sa/lisa.h
    ${CMAKE_CURRENT_SOURCE_DIR}/weights/weights.h
    ${CMAKE_CURRENT_SOURCE_DIR}/weights/gal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/weights/partition.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mapping/mapping.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/jc_voronoi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/UTM.h
    ${CMAKE_CURRENT_SOURCE_DIR}/regression/regression.h
    ${CMAKE_CURRENT_SOURCE_DIR}/data/data.h
    PARENT_SCOPE)
