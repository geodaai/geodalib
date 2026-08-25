// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { initWASM } from '../init';

/**
 * Partitioning Around Medoids (PAM) / k-medoids clustering.
 *
 * PAM clusters observations around a set of `k` medoids (actual data points)
 * that minimize the total distance to the other observations. Unlike k-means
 * it only needs a distance measure, not a Euclidean centroid, and is robust to
 * outliers. The `initializer` controls the starting medoids: `"BUILD"` (the
 * classic farthest-first PAM construction) or `"LAB"` (linear approximate
 * BUILD, seeded by `seed`).
 *
 * ## Example
 * ```ts
 * import { pam } from '@geoda/core';
 *
 * const result = await pam({ k: 2, data: [[1, 2, 3, 4, 5]] });
 * ```
 */
export async function pam({
  k,
  data,
  distanceMethod = 'euclidean',
  maxiter = 100,
  initializer = 'LAB',
  fasttol = 0.01,
  seed = 123456789,
}: {
  k: number;
  data: number[][] | Float32Array[];
  distanceMethod?: string;
  maxiter?: number;
  initializer?: 'BUILD' | 'LAB';
  fasttol?: number;
  seed?: number;
}): Promise<number[][]> {
  const wasm = await initWASM();
  const n = data[0]?.length ?? 0;

  // Validate before copying into WASM: a shorter variable would leave
  // Number(undefined) = NaN in the data matrix.
  if (data.length === 0) {
    throw new Error('pam: data must contain at least one variable');
  }
  if (n === 0) {
    throw new Error('pam: data must contain at least one observation');
  }
  if (k < 1 || k > n) {
    throw new Error(`pam: k must be between 1 and ${n} (got ${k})`);
  }
  for (const varData of data) {
    if (varData.length !== n) {
      throw new Error(`pam: each variable must have ${n} values (got ${varData.length})`);
    }
  }

  const wasmData = new wasm.VecVecDouble();
  for (const varData of data) {
    const wv = new wasm.VectorDouble();
    wv.resize(n, 0);
    for (let i = 0; i < n; ++i) wv.set(i, Number(varData[i]));
    wasmData.push_back(wv);
  }

  const result = wasm.pam(k, wasmData, distanceMethod, maxiter, initializer, fasttol, seed);
  const out: number[][] = [];
  for (let i = 0; i < result.size(); ++i) {
    const row = result.get(i);
    const vals: number[] = [];
    for (let j = 0; j < row.size(); ++j) vals.push(row.get(j));
    out.push(vals);
  }
  return out;
}
