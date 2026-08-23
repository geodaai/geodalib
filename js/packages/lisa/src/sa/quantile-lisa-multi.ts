// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { initWASM } from '../init';
import { vecDoubleToNumber, vecIntToNumber, vecStringToArray } from '@geoda/common';

/**
 * Result object containing Multivariate Local Quantile LISA statistics
 */
export type MultivariateQuantileLisaResult = {
  isValid: boolean;
  clusters: number[];
  lagValues: number[];
  pValues: number[];
  lisaValues: number[];
  sigCategories: number[];
  nn: number[];
  labels: string[];
  colors: string[];
};

/**
 * Configuration properties for Multivariate Local Quantile LISA.
 */
export type MultivariateQuantileLisaProps = {
  /** number of quantile breaks per variable */
  kValues: number[];
  /** quantile class (1-based) per variable */
  quantileValues: number[];
  /** multiple data variables */
  data: number[][] | Float32Array[];
  /** spatial weights matrix as adjacency list */
  neighbors: number[][];
  permutation: number;
  significanceCutoff?: number;
  seed?: number;
};

/**
 * Calculates Multivariate Local Quantile LISA statistics.
 *
 * @param {MultivariateQuantileLisaProps} props - Configuration object
 * @returns {Promise<LocalQuantileLisarResult>} Promise resolving to quantile LISA statistics
 */
export async function multivariateQuantileLisa({
  kValues,
  quantileValues,
  data,
  neighbors,
  permutation = 99,
  significanceCutoff = 0.05,
  seed = 1234567890,
}: MultivariateQuantileLisaProps): Promise<MultivariateQuantileLisaResult> {
  const wasm = await initWASM();

  const n = neighbors.length;
  const wasmK = new wasm.VectorInt();
  wasmK.resize(kValues.length, 0);
  for (let i = 0; i < kValues.length; ++i) wasmK.set(i, kValues[i]);
  const wasmQ = new wasm.VectorInt();
  wasmQ.resize(quantileValues.length, 0);
  for (let i = 0; i < quantileValues.length; ++i) wasmQ.set(i, quantileValues[i]);

  const wasmData = new wasm.VecVecDouble();
  for (let v = 0; v < data.length; ++v) {
    const varData = new wasm.VectorDouble();
    varData.resize(n, 0);
    for (let i = 0; i < n; ++i) varData.set(i, Number(data[v][i]));
    wasmData.push_back(varData);
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

  const result = wasm.multivariateQuantileLisa(
    wasmK,
    wasmQ,
    wasmData,
    wasmNeighbors,
    wasmUndefs,
    significanceCutoff,
    permutation,
    seed
  );

  return {
    isValid: result.isValid(),
    clusters: vecDoubleToNumber(result.getClusters()),
    lagValues: vecDoubleToNumber(result.getLagValues()),
    lisaValues: vecDoubleToNumber(result.getLisaValues()),
    pValues: vecDoubleToNumber(result.getPValues()),
    sigCategories: vecIntToNumber(result.getSignificanceCategories()),
    nn: vecIntToNumber(result.getNN()),
    labels: vecStringToArray(result.getLabels()),
    colors: vecStringToArray(result.getColors()),
  };
}
