// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#ifndef GEODA_CLUSTER_UTILS_H
#define GEODA_CLUSTER_UTILS_H

#include <vector>
#include "rng.h"
#include "../weights/geoda-weight.h"
#include "../weights/gal.h"

namespace geoda {

double euclid(int n, double** data1, double** data2, int** mask1, int** mask2, const double weight[], int index1,
              int index2, int transpose);
double cityblock(int n, double** data1, double** data2, int** mask1, int** mask2, const double weight[], int index1,
                 int index2, int transpose);
double(*setmetric(char dist))(int, double**, double**, int**, int**, const double[], int, int, int);
double** distancematrix(int nrows, int ncolumns, double** data, int** mask, double weights[], char dist, int transpose);
std::vector<int> flat_2dclusters(int n, std::vector<std::vector<int>> clusters);

double euclidean_distance(const std::vector<double>& a, const std::vector<double>& b);
double euclidean_distance(double* a, const std::vector<double>& b);
void shuffle(std::vector<int>& items, Xoroshiro128Random& rng);

}  // namespace geoda

namespace Gda {
geoda::GalElement* GetGalElement(GeoDaWeight* w);
}  // namespace Gda

#endif
