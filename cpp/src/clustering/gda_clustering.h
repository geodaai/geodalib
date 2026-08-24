// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project
//
// Result types ported from libgeoda's gda_clustering.h.

#ifndef GEODA_CENTER_GDA_CLUSTERING_H
#define GEODA_CENTER_GDA_CLUSTERING_H

#include <vector>

struct Fragmentation {
  int n;
  double entropy;
  double std_entropy;
  double simpson;
  double std_simpson;
  int min_cluster_size;
  int max_cluster_size;
  double mean_cluster_size;
  bool is_spatially_contiguous;
  double fraction;

  Fragmentation()
      : n(0), entropy(0), std_entropy(0), simpson(0), std_simpson(0), min_cluster_size(0), max_cluster_size(0),
        mean_cluster_size(0), is_spatially_contiguous(true), fraction(0) {}
};

struct Compactness {
  double isoperimeter_quotient;
  double area;
  double perimeter;
  Compactness() : isoperimeter_quotient(0), area(0), perimeter(0) {}
};

struct Diameter {
  int steps;
  double ratio;
  Diameter() : steps(0), ratio(0) {}
};

struct JoinCountRatio {
  int cluster;
  int n;
  int totalNeighbors;
  int totalJoinCount;
  double ratio;
  JoinCountRatio() : cluster(0), n(0), totalNeighbors(0), totalJoinCount(0), ratio(0) {}
};

struct ValidationResult {
  bool spatially_constrained;
  Fragmentation fragmentation;
  std::vector<Fragmentation> cluster_fragmentation;
  std::vector<Diameter> cluster_diameter;
  std::vector<Compactness> cluster_compactness;
  std::vector<JoinCountRatio> joincount_ratio;
};

#endif
