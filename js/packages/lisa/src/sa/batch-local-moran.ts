// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { initWASM } from '../init';
import { vecDoubleToNumber, vecIntToNumber, vecStringToArray } from '@geoda/common';

/**
 * Result of a batch Local Moran computation (per-variable arrays).
 */
export type BatchLocalMoranResult = {
  isValid: boolean;
  /** per-variable LISA statistic values */
  lisaValues: number[][];
  /** per-variable pseudo-p values */
  pValues: number[][];
  /** per-variable cluster assignments */
  clusters: number[][];
  /** per-variable spatial lag values */
  lagValues: number[][];
  /** number of neighbors for each observation */
  nn: number[];
  labels: string[];
  colors: string[];
};

/**
 * Configuration for a batch Local Moran calculation.
 */
export type BatchLocalMoranProps = {
  data: number[][] | Float32Array[];
  neighbors: number[][];
  permutation: number;
  significanceCutoff?: number;
  seed?: number;
};

function vecVecIntToNumber(v: { size(): number; get(i: number): unknown }): number[][] {
  const rows: number[][] = [];
  for (let i = 0; i < v.size(); ++i) rows.push(vecIntToNumber(v.get(i) as never));
  return rows;
}

function vecVecDoubleToNumber(v: { size(): number; get(i: number): unknown }): number[][] {
  const rows: number[][] = [];
  for (let i = 0; i < v.size(); ++i) {
    rows.push(vecDoubleToNumber(v.get(i) as never));
  }
  return rows;
}

/**
 * Calculates batch (multivariate) Local Moran statistics across multiple variables.
 *
 * @param {BatchLocalMoranProps} props - Configuration object
 * @returns {Promise<BatchLocalMoranResult>} Promise resolving to per-variable Local Moran statistics
 */
export async function batchLocalMoran({
  data,
  neighbors,
  permutation = 999,
  significanceCutoff = 0.05,
  seed = 1234567890,
}: BatchLocalMoranProps): Promise<BatchLocalMoranResult> {
  const wasm = await initWASM();

  const n = neighbors.length;
  const wasmData = new wasm.VecVecDouble();
  for (const varData of data) {
    const wasmVar = new wasm.VectorDouble();
    wasmVar.resize(n, 0);
    for (let i = 0; i < n; ++i) wasmVar.set(i, Number(varData[i]));
    wasmData.push_back(wasmVar);
  }

  const wasmNeighbors = new wasm.VecVecUInt();
  for (let i = 0; i < n; ++i) {
    const nbrs = neighbors[i] ?? [];
    const wNbrs = new wasm.VectorUInt();
    for (let j = 0; j < nbrs.length; ++j) wNbrs.push_back(nbrs[j]);
    wasmNeighbors.push_back(wNbrs);
  }

  const wasmUndefs = new wasm.VecVecUInt();
  for (let v = 0; v < data.length; ++v) {
    const undef = new wasm.VectorUInt();
    undef.resize(n, 0);
    wasmUndefs.push_back(undef);
  }

  const result = wasm.batchLocalMoran(
    wasmData,
    wasmNeighbors,
    wasmUndefs,
    significanceCutoff,
    permutation,
    seed
  );

  return {
    isValid: result.isValid(),
    lisaValues: vecVecDoubleToNumber(result.getLisaValues()),
    pValues: vecVecDoubleToNumber(result.getPValues()),
    clusters: vecVecIntToNumber(result.getClusters()),
    lagValues: vecVecDoubleToNumber(result.getLagValues()),
    nn: vecIntToNumber(result.getNN()),
    labels: vecStringToArray(result.getLabels()),
    colors: vecStringToArray(result.getColors()),
  };
}
