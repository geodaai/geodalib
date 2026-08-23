// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project
//
// Minimal geometry abstraction ported from libgeoda's geofeature.h, providing
// just what spatial_validation needs.

#ifndef GEODA_CENTER_GEOFATURE_H
#define GEODA_CENTER_GEOFATURE_H

#include <vector>

namespace gda {

enum ShapeType {
  NULL_SHAPE = 0, POINT_TYP = 1, POLY_LINE = 3, POLYGON = 5,
  MULTI_POINT = 8, POINT_Z = 11, POLY_LINE_Z = 13, POLYGON_Z = 15,
  MULTI_POINT_Z = 18, POINT_M = 21, POLY_LINE_M = 23, POLYGON_M = 25,
  MULTI_POINT_M = 28, MULTI_PATCH = 31
};

struct Point {
  Point() : x(0), y(0) {}
  Point(double x_s, double y_s) : x(x_s), y(y_s) {}
  double x;
  double y;
  double distance(const Point& p) const {
    return std::sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y));
  }
};

struct GeometryContent {
  GeometryContent() : shape_type(0) {}
  virtual ~GeometryContent() {}
  GeometryContent(gda::ShapeType st) : shape_type(st) {}
  int shape_type;
};

struct PointContents : public GeometryContent {
  PointContents() : GeometryContent(gda::POINT_TYP), x(0), y(0) {}
  virtual ~PointContents() {}
  double x;
  double y;
};

struct PolygonContents : public GeometryContent {
  PolygonContents() : GeometryContent(gda::POLYGON), num_parts(0), num_points(0), parts(0), points(0) {}
  virtual ~PolygonContents() {}
  int num_parts;
  int num_points;
  std::vector<int> parts;
  std::vector<Point> points;
};

}  // namespace gda

#endif
