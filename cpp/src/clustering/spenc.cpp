// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "clustering-api.h"
#include "rng.h"
#include "../data/data.h"

namespace {

// Standardize each variable (mean 0, std 1) unless scale_method == "raw".
std::vector<std::vector<double>> scale_data(const std::vector<std::vector<double>>& data,
                                            const std::string& scale_method) {
  if (data.empty()) {
    return {};
  }
  const size_t num_obs = data[0].size();
  for (size_t i = 0; i < data.size(); ++i) {
    if (data[i].size() != num_obs) {
      throw std::invalid_argument(
          "spenc: each variable must have the same number of observations");
    }
  }
  if (scale_method == "raw") {
    return data;
  }
  std::vector<std::vector<double>> scaled = data;
  std::vector<unsigned int> undef(num_obs, 0);
  for (size_t i = 0; i < scaled.size(); ++i) {
    scaled[i] = geoda::standardize_data_wasm(scaled[i], undef);
  }
  return scaled;
}

// Rotate the (p,q) plane of the symmetric matrix a by angle (c,s) so that
// a[p][q] -> 0, keeping a symmetric, and apply the same rotation to the
// eigenvector matrix v. p and q are swapped in the a[p][q] != 0 case only.
void jacobi_rotate(std::vector<std::vector<double>>& a, std::vector<std::vector<double>>& v, int p, int q,
                   double c, double s) {
  const int n = static_cast<int>(a.size());
  // Off-diagonal rows (k != p, q): rotate using the original column values so
  // the [p,q] sub-block below can use the untouched diagonal entries.
  for (int k = 0; k < n; ++k) {
    if (k == p || k == q) continue;
    const double a_kp = a[k][p];
    const double a_kq = a[k][q];
    a[k][p] = c * a_kp - s * a_kq;
    a[p][k] = a[k][p];
    a[k][q] = s * a_kp + c * a_kq;
    a[q][k] = a[k][q];
  }
  // The 2x2 diagonal block.
  const double app = a[p][p], aqq = a[q][q], apq = a[p][q];
  a[p][p] = c * c * app - 2.0 * s * c * apq + s * s * aqq;
  a[q][q] = s * s * app + 2.0 * s * c * apq + c * c * aqq;
  a[p][q] = 0.0;
  a[q][p] = 0.0;
  // Eigenvectors: rotate columns p and q.
  for (int k = 0; k < n; ++k) {
    const double v_kp = v[k][p];
    const double v_kq = v[k][q];
    v[k][p] = c * v_kp - s * v_kq;
    v[k][q] = s * v_kp + c * v_kq;
  }
}

// Dense Jacobi eigendecomposition of a real symmetric matrix (passed by value,
// so a is destroyed). eigenvalues[i] pairs with column i of eigenvectors.
void jacobi_eigen(std::vector<std::vector<double>> a, std::vector<double>& eigenvalues,
                  std::vector<std::vector<double>>& eigenvectors) {
  const int n = static_cast<int>(a.size());
  eigenvectors.assign(n, std::vector<double>(n, 0.0));
  for (int i = 0; i < n; ++i) eigenvectors[i][i] = 1.0;

  const double tol = 1e-12;
  const int max_sweeps = 200;
  for (int sweep = 0; sweep < max_sweeps; ++sweep) {
    // Largest off-diagonal magnitude.
    double max_off = 0.0;
    int p = 0, q = 1;
    for (int i = 0; i < n; ++i) {
      for (int j = i + 1; j < n; ++j) {
        const double m = std::fabs(a[i][j]);
        if (m > max_off) {
          max_off = m;
          p = i;
          q = j;
        }
      }
    }
    if (max_off < tol) break;
    // Rotation that zeroes a[p][q].
    const double theta = 0.5 * (a[q][q] - a[p][p]) / a[p][q];
    const double t = std::copysign(1.0, theta) / (std::fabs(theta) + std::sqrt(theta * theta + 1.0));
    const double c = 1.0 / std::sqrt(t * t + 1.0);
    const double s = t * c;
    jacobi_rotate(a, eigenvectors, p, q, c, s);
  }
  eigenvalues.resize(n);
  for (int i = 0; i < n; ++i) eigenvalues[i] = a[i][i];
}

// Deterministic Lloyd's k-means on rows of `points`. Runs n_init restarts with
// seeds derived from rnd_seed and returns the labels with the lowest inertia.
std::vector<int> kmeans(const std::vector<std::vector<double>>& points, int k, int n_init, int rnd_seed) {
  const int n = static_cast<int>(points.size());
  const int dim = static_cast<int>(points[0].size());
  std::vector<int> best_labels(n, 0);
  double best_inertia = std::numeric_limits<double>::max();

  for (int run = 0; run < n_init; ++run) {
    Xoroshiro128Random rng(static_cast<long long>(rnd_seed) * (run + 1) + 12345);
    std::vector<int> chosen = rng.randomSample(k, n);
    std::vector<std::vector<double>> centroids(k, std::vector<double>(dim));
    for (int c = 0; c < k; ++c) centroids[c] = points[chosen[c]];

    std::vector<int> labels(n, 0);
    std::vector<double> sqdist(n, 0.0);
    double inertia = 0.0;
    for (int iter = 0; iter < 100; ++iter) {
      // Assign each point to the nearest centroid.
      bool changed = false;
      for (int i = 0; i < n; ++i) {
        int best_c = 0;
        double best_d = std::numeric_limits<double>::max();
        for (int c = 0; c < k; ++c) {
          double d = 0.0;
          for (int m = 0; m < dim; ++m) {
            const double diff = points[i][m] - centroids[c][m];
            d += diff * diff;
          }
          if (d < best_d) {
            best_d = d;
            best_c = c;
          }
        }
        sqdist[i] = best_d;
        if (labels[i] != best_c) {
          labels[i] = best_c;
          changed = true;
        }
      }
      // Recompute centroids as the mean of their members.
      std::vector<int> counts(k, 0);
      std::vector<std::vector<double>> sums(k, std::vector<double>(dim, 0.0));
      for (int i = 0; i < n; ++i) {
        counts[labels[i]]++;
        for (int m = 0; m < dim; ++m) sums[labels[i]][m] += points[i][m];
      }
      bool empty = false;
      for (int c = 0; c < k; ++c) {
        if (counts[c] == 0) {
          empty = true;
          break;
        }
        for (int m = 0; m < dim; ++m) centroids[c][m] = sums[c][m] / static_cast<double>(counts[c]);
      }
      if (empty) {
        // Re-seed empty centroids from the point farthest from its centroid
        // (standard k-means fallback) so no cluster is lost.
        int far_i = 0;
        double far_d = -1.0;
        for (int i = 0; i < n; ++i) {
          if (sqdist[i] > far_d) {
            far_d = sqdist[i];
            far_i = i;
          }
        }
        for (int c = 0; c < k; ++c) {
          if (counts[c] == 0) centroids[c] = points[far_i];
        }
        continue;
      }
      inertia = 0.0;
      for (int i = 0; i < n; ++i) inertia += sqdist[i];
      if (!changed) break;
    }
    if (inertia < best_inertia) {
      best_inertia = inertia;
      best_labels = labels;
    }
  }
  return best_labels;
}

}  // namespace

std::vector<std::vector<int>> geoda::spenc(unsigned int k, const std::vector<std::vector<unsigned int>>& neighbors,
                                           const std::vector<std::vector<double>>& data,
                                           const std::string& scale_method, double gamma, unsigned int n_init,
                                           int rnd_seed) {
  const int n = static_cast<int>(neighbors.size());
  if (n == 0) {
    throw std::invalid_argument("spenc: neighbors must contain at least one observation");
  }
  if (k == 0 || k > static_cast<unsigned int>(n)) {
    throw std::invalid_argument("spenc: k must be between 1 and the number of observations");
  }
  if (n_init == 0) {
    throw std::invalid_argument("spenc: n_init must be at least 1");
  }
  if (gamma <= 0.0) {
    throw std::invalid_argument("spenc: gamma must be positive");
  }
  if (data.empty()) {
    throw std::invalid_argument("spenc: data must contain at least one variable");
  }

  std::vector<std::vector<double>> scaled = scale_data(data, scale_method);

  // Symmetrized binary adjacency (diagonal 0), matching spopt's W.
  std::vector<std::vector<unsigned int>> sym(n, std::vector<unsigned int>(n, 0));
  for (int i = 0; i < n; ++i) {
    for (unsigned int j : neighbors[i]) {
      if (j >= static_cast<unsigned int>(n)) {
        throw std::invalid_argument("spenc: neighbor index out of range");
      }
      sym[i][j] = 1;
      sym[j][i] = 1;
    }
  }

  // Observation vectors (standardized) for distance computation.
  const int num_vars = static_cast<int>(scaled.size());
  std::vector<std::vector<double>> obs(n, std::vector<double>(num_vars));
  for (int i = 0; i < n; ++i) {
    for (int v = 0; v < num_vars; ++v) obs[i][v] = scaled[v][i];
  }

  // Spatially encoded affinity S = A .* W with the RBF kernel
  // A[i][j] = exp(-gamma * ||x_i - x_j||^2).
  std::vector<std::vector<double>> S(n, std::vector<double>(n, 0.0));
  std::vector<double> degree(n, 0.0);
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      if (sym[i][j] == 0) continue;
      double d2 = 0.0;
      for (int v = 0; v < num_vars; ++v) {
        const double diff = obs[i][v] - obs[j][v];
        d2 += diff * diff;
      }
      const double val = std::exp(-gamma * d2);
      S[i][j] = val;
      S[j][i] = val;
    }
  }
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) degree[i] += S[i][j];
  }

  // Normalized matrix N = D^{-1/2} S D^{-1/2}; its largest eigenvectors are the
  // normalized-cut directions (smallest eigenvalues of the Laplacian I - N).
  std::vector<std::vector<double>> N(n, std::vector<double>(n, 0.0));
  for (int i = 0; i < n; ++i) {
    if (degree[i] == 0.0) continue;
    for (int j = i + 1; j < n; ++j) {
      if (S[i][j] == 0.0 || degree[j] == 0.0) continue;
      const double val = S[i][j] / std::sqrt(degree[i] * degree[j]);
      N[i][j] = val;
      N[j][i] = val;
    }
  }

  std::vector<double> eigenvalues;
  std::vector<std::vector<double>> eigenvectors;
  jacobi_eigen(N, eigenvalues, eigenvectors);

  // Order eigenpairs by descending eigenvalue and take the top k.
  std::vector<int> order(n);
  for (int i = 0; i < n; ++i) order[i] = i;
  std::sort(order.begin(), order.end(), [&](int a, int b) { return eigenvalues[a] > eigenvalues[b]; });

  const int kc = static_cast<int>(k);
  std::vector<std::vector<double>> embedding(n, std::vector<double>(kc, 0.0));
  for (int c = 0; c < kc; ++c) {
    const int col = order[c];
    // Deterministic sign flip: the entry with the largest magnitude is made
    // positive (mirrors sklearn's _deterministic_vector_sign_flip).
    int argmax = 0;
    for (int i = 0; i < n; ++i) {
      if (std::fabs(eigenvectors[i][col]) > std::fabs(eigenvectors[argmax][col])) argmax = i;
    }
    const double flip = (eigenvectors[argmax][col] < 0.0) ? -1.0 : 1.0;
    for (int i = 0; i < n; ++i) {
      const double e = flip * eigenvectors[i][col];
      embedding[i][c] = (degree[i] > 0.0) ? e / degree[i] : 0.0;
    }
  }

  std::vector<int> labels = kmeans(embedding, kc, static_cast<int>(n_init), rnd_seed);

  std::vector<std::vector<int>> clusters(kc);
  for (int i = 0; i < n; ++i) clusters[labels[i]].push_back(i);
  clusters.erase(std::remove_if(clusters.begin(), clusters.end(),
                                [](const std::vector<int>& c) { return c.empty(); }),
                 clusters.end());
  return clusters;
}
