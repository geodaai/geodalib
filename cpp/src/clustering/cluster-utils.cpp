// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "cluster.h"
#include "../weights/vector-weight.h"
#include <vector>

#include "cluster-utils.h"

namespace geoda {

// Weighted Euclidean distance between two rows (transpose == 0) or columns.
double euclid(int n, double** data1, double** data2, int** mask1, int** mask2, const double weight[], int index1,
              int index2, int transpose) {
  double result = 0.0;
  double tweight = 0.0;
  if (transpose == 0) {
    for (int i = 0; i < n; i++) {
      if (mask1[index1][i] && mask2[index2][i]) {
        double term = data1[index1][i] - data2[index2][i];
        result += weight[i] * term * term;
        tweight += weight[i];
      }
    }
  } else {
    for (int i = 0; i < n; i++) {
      if (mask1[i][index1] && mask2[i][index2]) {
        double term = data1[i][index1] - data2[i][index2];
        result += weight[i] * term * term;
        tweight += weight[i];
      }
    }
  }
  if (!tweight) return 0;
  return result;
}

// Weighted city-block (Manhattan) distance.
double cityblock(int n, double** data1, double** data2, int** mask1, int** mask2, const double weight[], int index1,
                 int index2, int transpose) {
  double result = 0.0;
  double tweight = 0.0;
  if (transpose == 0) {
    for (int i = 0; i < n; i++) {
      if (mask1[index1][i] && mask2[index2][i]) {
        double term = data1[index1][i] - data2[index2][i];
        result += weight[i] * std::abs(term);
        tweight += weight[i];
      }
    }
  } else {
    for (int i = 0; i < n; i++) {
      if (mask1[i][index1] && mask2[i][index2]) {
        double term = data1[i][index1] - data2[i][index2];
        result += weight[i] * std::abs(term);
        tweight += weight[i];
      }
    }
  }
  if (!tweight) return 0;
  return result;
}

double(*setmetric(char dist))(int, double**, double**, int**, int**, const double[], int, int, int) {
  if (dist == 'b') {
    return cityblock;
  }
  return euclid;
}

// Lower-triangular ragged distance matrix between rows (transpose == 0) or columns.
double** distancematrix(int nrows, int ncolumns, double** data, int** mask, double weights[], char dist,
                        int transpose) {
  int n = (transpose == 0) ? nrows : ncolumns;
  int ndata = (transpose == 0) ? ncolumns : nrows;
  double** matrix = new double*[n];
  matrix[0] = 0;
  for (int i = 1; i < n; i++) {
    matrix[i] = new double[i];
  }
  double (*metric)(int, double**, double**, int**, int**, const double[], int, int, int) = setmetric(dist);
  for (int i = 1; i < n; i++) {
    for (int j = 0; j < i; j++) {
      matrix[i][j] = metric(ndata, data, data, mask, mask, weights, i, j, transpose);
    }
  }
  return matrix;
}

std::vector<int> flat_2dclusters(int n, std::vector<std::vector<int>> clusters) {
  std::vector<int> cluster_ids(n, 0);
  int ncluster = static_cast<int>(clusters.size());
  if (ncluster == 0) return cluster_ids;

  std::sort(clusters.begin(), clusters.end(),
            [](const std::vector<int>& a, const std::vector<int>& b) { return a.size() < b.size(); });

  for (int i = 0; i < ncluster; i++) {
    int c = i + 1;
    for (size_t j = 0; j < clusters[i].size(); j++) {
      int idx = clusters[i][j];
      cluster_ids[idx] = c;
    }
  }
  return cluster_ids;
}

}  // namespace geoda

double cuttree(int nelements, GdaNode* tree, int nclusters, int clusterid[]) {
  int icluster = 0;
  const int n = nelements - nclusters;
  int* nodeid;
  for (int i = nelements - 2; i >= n; i--) {
    int k = tree[i].left;
    if (k >= 0) {
      clusterid[k] = icluster;
      icluster++;
    }
    k = tree[i].right;
    if (k >= 0) {
      clusterid[k] = icluster;
      icluster++;
    }
  }
  nodeid = (int*)malloc(n * sizeof(int));
  if (!nodeid) {
    for (int i = 0; i < nelements; i++) clusterid[i] = -1;
    return 0;
  }
  for (int i = 0; i < n; i++) nodeid[i] = -1;
  for (int i = n - 1; i >= 0; i--) {
    int j;
    if (nodeid[i] < 0) {
      j = icluster;
      nodeid[i] = j;
      icluster++;
    } else {
      j = nodeid[i];
    }
    int k = tree[i].left;
    if (k < 0) nodeid[-k - 1] = j;
    else clusterid[k] = j;
    k = tree[i].right;
    if (k < 0) nodeid[-k - 1] = j;
    else clusterid[k] = j;
  }
  free(nodeid);
  return tree[n - 1].distance;
}

namespace geoda {
double euclidean_distance(const std::vector<double>& a, const std::vector<double>& b) {
  double sum = 0.0;
  for (size_t i = 0; i < a.size(); ++i) { double d = a[i] - b[i]; sum += d * d; }
  return std::sqrt(sum);
}
double euclidean_distance(double* a, const std::vector<double>& b) {
  double sum = 0.0;
  for (size_t i = 0; i < b.size(); ++i) { double d = a[i] - b[i]; sum += d * d; }
  return std::sqrt(sum);
}
void shuffle(std::vector<int>& items, Xoroshiro128Random& rng) {
  for (size_t i = items.size() - 1; i > 0; --i) {
    size_t j = static_cast<size_t>(rng.nextInt(static_cast<int>(i + 1)));
    std::swap(items[i], items[j]);
  }
}

}  // namespace geoda

namespace Gda {
geoda::GalElement* GetGalElement(GeoDaWeight* w) {
  VectorWeight* vw = dynamic_cast<VectorWeight*>(w);
  return vw ? vw->gal : 0;
}
}  // namespace Gda
