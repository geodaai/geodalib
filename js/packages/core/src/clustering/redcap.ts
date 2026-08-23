// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { initWASM } from '../init';

/**
 * Regionally constrained clustering (REDCAP).
 *
 * ## Example
 * ```ts
 * import { redcap } from '@geoda/core';
 *
 * const result = await redcap({
 *   k: 3,
 *   data: [[1, 2, 3, 4]],
 *   neighbors: [[1], [0, 2], [1, 3], [2]],
 * });
 * ```
 */
export async function redcap({
  k,
  data,
  neighbors,
  scaleMethod = 'standardize',
  redcapMethod = 'firstorder-singlelinkage',
  distanceMethod = 'euclidean',
  boundVals = [],
  minBound = 0.0,
}: {
  k: number;
  data: number[][] | Float32Array[];
  neighbors: number[][];
  scaleMethod?: string;
  redcapMethod?: string;
  distanceMethod?: string;
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

  const result = wasm.redcap(
    k,
    wasmNeighbors,
    wasmData,
    scaleMethod,
    redcapMethod,
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
