// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#ifndef GEODA_CLUSTER_UTILS_H
#define GEODA_CLUSTER_UTILS_H

#include <vector>

namespace geoda {

double euclid(int n, double** data1, double** data2, int** mask1, int** mask2, const double weight[], int index1,
              int index2, int transpose);
double cityblock(int n, double** data1, double** data2, int** mask1, int** mask2, const double weight[], int index1,
                 int index2, int transpose);
double(*setmetric(char dist))(int, double**, double**, int**, int**, const double[], int, int, int);
double** distancematrix(int nrows, int ncolumns, double** data, int** mask, double weights[], char dist, int transpose);
std::vector<int> flat_2dclusters(int n, std::vector<std::vector<int>> clusters);

}  // namespace geoda

#endif
