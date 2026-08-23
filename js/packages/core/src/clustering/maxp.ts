// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { initWASM } from '../init';

/**
 * Max-P regionalization.
 *
 * ## Example
 * ```ts
 * import { maxpGreedy } from '@geoda/core';
 *
 * const result = await maxpGreedy({ data: [[1, 2, 3, 4]], neighbors: [[1], [0, 2], [1, 3], [2]] });
 * ```
 */
export async function maxpGreedy({
  data,
  neighbors,
  iterations = 10,
  distanceMethod = 'euclidean',
  seed = 1234567890,
}: {
  data: number[][] | Float32Array[];
  neighbors: number[][];
  iterations?: number;
  distanceMethod?: string;
  seed?: number;
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
  const result = wasm.maxpGreedy(wasmNeighbors, wasmData, iterations, distanceMethod, seed);
  const out: number[][] = [];
  for (let i = 0; i < result.size(); ++i) {
    const row = result.get(i);
    const vals: number[] = [];
    for (let j = 0; j < row.size(); ++j) vals.push(row.get(j));
    out.push(vals);
  }
  return out;
}

/**
 * Max-P regionalization with simulated annealing.
 */
export async function maxpSA({
  data,
  neighbors,
  iterations = 10,
  coolingRate = 0.85,
  saMaxit = 1,
  distanceMethod = 'euclidean',
  seed = 1234567890,
}: {
  data: number[][] | Float32Array[];
  neighbors: number[][];
  iterations?: number;
  coolingRate?: number;
  saMaxit?: number;
  distanceMethod?: string;
  seed?: number;
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
  const result = wasm.maxpSA(
    wasmNeighbors,
    wasmData,
    iterations,
    coolingRate,
    saMaxit,
    distanceMethod,
    seed
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

/**
 * Max-P regionalization with tabu search.
 */
export async function maxpTabu({
  data,
  neighbors,
  iterations = 10,
  tabuLength = 10,
  convTabu = 10,
  distanceMethod = 'euclidean',
  seed = 1234567890,
}: {
  data: number[][] | Float32Array[];
  neighbors: number[][];
  iterations?: number;
  tabuLength?: number;
  convTabu?: number;
  distanceMethod?: string;
  seed?: number;
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
  const result = wasm.maxpTabu(
    wasmNeighbors,
    wasmData,
    iterations,
    tabuLength,
    convTabu,
    distanceMethod,
    seed
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
