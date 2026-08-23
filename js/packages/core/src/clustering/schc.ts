// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { initWASM } from '../init';

/**
 * Spatially constrained hierarchical clustering (SCHC).
 *
 * ## Example
 * ```ts
 * import { schc } from '@geoda/core';
 *
 * const result = await schc({
 *   k: 3,
 *   data: [[1, 2, 3, 4]],
 *   neighbors: [[1], [0, 2], [1, 3], [2]],
 * });
 * ```
 */
export async function schc({
  k,
  data,
  neighbors,
  scaleMethod = 'standardize',
  linkageMethod = 'ward',
  distanceMethod = 'euclidean',
  boundVals = [],
  minBound = 0.0,
}: {
  /** number of clusters */
  k: number;
  /** multivariate data, one array per variable */
  data: number[][] | Float32Array[];
  /** spatial weights matrix as adjacency list */
  neighbors: number[][];
  /** raw | standardize */
  scaleMethod?: string;
  /** single | complete | average | ward */
  linkageMethod?: string;
  /** euclidean | manhattan */
  distanceMethod?: string;
  /** optional bound values per observation */
  boundVals?: number[];
  minBound?: number;
}): Promise<number[][]> {
  const wasm = await initWASM();

  const n = neighbors.length;
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

  const wasmBound = new wasm.VectorDouble();
  for (const b of boundVals) wasmBound.push_back(b);

  const result = wasm.schc(
    k,
    wasmNeighbors,
    wasmData,
    scaleMethod,
    linkageMethod,
    distanceMethod,
    wasmBound,
    minBound
  );

  const out: number[][] = [];
  for (let i = 0; i < result.size(); ++i) {
    const row = result.get(i);
    const vals: number[] = [];
    for (let j = 0; j < row.size(); ++j) vals.push(row.get(j));
    out.push(vals);
  }
  return out;
}
