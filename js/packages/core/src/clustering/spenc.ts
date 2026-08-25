// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { initWASM } from '../init';

/**
 * Spatially-Encouraged Spectral Clustering (SPENC).
 *
 * SPENC is a spectral regionalization method: it computes an RBF affinity
 * between observations, masks it with the spatial adjacency (`W`) to get a
 * spatially-encoded affinity `S`, and embeds the observations in the top
 * eigenvectors of the normalized matrix `D^{-1/2} S D^{-1/2}` before running
 * k-means on the embedding. Because the k-means step is seeded, results are
 * deterministic for a fixed `seed`.
 *
 * ## Example
 * ```ts
 * import { spenc } from '@geoda/core';
 *
 * const result = await spenc({
 *   k: 2,
 *   data: [[1, 2, 3, 4, 5]],
 *   neighbors: [[1], [0, 2], [1, 3], [2, 4], [3]],
 * });
 * ```
 */
export async function spenc({
  k,
  data,
  neighbors,
  scaleMethod = 'standardize',
  gamma = 1.0,
  nInit = 10,
  seed = 123456789,
}: {
  k: number;
  data: number[][] | Float32Array[];
  neighbors: number[][];
  scaleMethod?: string;
  gamma?: number;
  nInit?: number;
  seed?: number;
}): Promise<number[][]> {
  const wasm = await initWASM();
  const n = neighbors.length;

  // Validate before copying into WASM: a shorter variable would leave
  // Number(undefined) = NaN in the data matrix and skew the clustering.
  if (n === 0) {
    throw new Error('spenc: neighbors must contain at least one observation');
  }
  if (data.length === 0) {
    throw new Error('spenc: data must contain at least one variable');
  }
  if (k < 1 || k > n) {
    throw new Error(`spenc: k must be between 1 and ${n} (got ${k})`);
  }
  for (const varData of data) {
    if (varData.length !== n) {
      throw new Error(`spenc: each variable must have ${n} values (got ${varData.length})`);
    }
  }

  const wasmData = new wasm.VecVecDouble();
  for (const varData of data) {
    const wv = new wasm.VectorDouble();
    wv.resize(n, 0);
    for (let i = 0; i < n; ++i) wv.set(i, Number(varData[i]));
    wasmData.push_back(wv);
  }
  const wasmNeighbors = new wasm.VecVecUInt();
  for (const nbrs of neighbors) {
    const wn = new wasm.VectorUInt();
    for (const nb of nbrs) wn.push_back(nb);
    wasmNeighbors.push_back(wn);
  }

  const result = wasm.spenc(k, wasmNeighbors, wasmData, scaleMethod, gamma, nInit, seed);
  const out: number[][] = [];
  for (let i = 0; i < result.size(); ++i) {
    const row = result.get(i);
    const vals: number[] = [];
    for (let j = 0; j < row.size(); ++j) vals.push(row.get(j));
    out.push(vals);
  }
  return out;
}
