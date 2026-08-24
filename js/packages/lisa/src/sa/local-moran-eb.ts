// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { initWASM } from '../init';
import { vecDoubleToNumber, vecIntToNumber, vecStringToArray } from '@geoda/common';

/**
 * Result object containing Empirical Bayes smoothed Local Moran statistics
 *
 * @property {boolean} isValid - Indicates if the analysis was successful
 * @property {number[]} clusters - Cluster assignments for each observation
 * @property {number[]} lagValues - Spatially lagged values
 * @property {number[]} pValues - Statistical significance values
 * @property {number[]} lisaValues - Local Moran statistics
 * @property {number[]} sigCategories - Significance categories
 * @property {number[]} nn - Number of neighbors for each observation
 * @property {string[]} labels - Descriptive labels for clusters
 * @property {string[]} colors - Color codes for visualization
 */
export type LocalMoranEBResult = {
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
 * Configuration properties for Empirical Bayes smoothed Local Moran calculation.
 * @property {number[] | Float32Array} eventData - Event (numerator) counts
 * @property {number[] | Float32Array} baseData - Base (denominator) counts
 * @property {number[][]} neighbors - Spatial weights matrix as adjacency list
 * @property {number} [permutation=999] - Number of permutations for significance testing
 * @property {number} [significanceCutoff=0.05] - Statistical significance threshold
 * @property {number} [seed=1234567890] - Random seed for reproducibility
 */
export type LocalMoranEBProps = {
  eventData: number[] | Float32Array;
  baseData: number[] | Float32Array;
  neighbors: number[][];
  permutation?: number;
  significanceCutoff?: number;
  seed?: number;
};

/**
 * Calculates Empirical Bayes smoothed Local Moran's I statistics.
 *
 * ## Example
 * ```ts
 * import { localMoranEB } from '@geoda/lisa';
 *
 * const eventData = [3, 5, 1, 4, 2];
 * const baseData = [10, 20, 15, 25, 12];
 * const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];
 *
 * const result = await localMoranEB({ eventData, baseData, neighbors });
 * console.log(result);
 * ```
 * @param {LocalMoranEBProps} props - Configuration object
 * @returns {Promise<LocalMoranEBResult>} Promise resolving to EB Local Moran statistics
 */
export async function localMoranEB({
  eventData,
  baseData,
  neighbors,
  permutation = 999,
  significanceCutoff = 0.05,
  seed = 1234567890,
}: LocalMoranEBProps): Promise<LocalMoranEBResult> {
  const wasm = await initWASM();

  const n = baseData.length;
  const wasmEvent = new wasm.VectorDouble();
  wasmEvent.resize(n, 0);
  for (let i = 0; i < n; ++i) {
    wasmEvent.set(i, Number(eventData[i]));
  }
  const wasmBase = new wasm.VectorDouble();
  wasmBase.resize(n, 0);
  for (let i = 0; i < n; ++i) {
    wasmBase.set(i, Number(baseData[i]));
  }

  const wasmNeighbors = new wasm.VecVecUInt();
  const wasmUndefs = new wasm.VectorUInt();

  for (let i = 0; i < n; ++i) {
    const nbrs = neighbors[i] ?? [];
    const wasmNeighborIndices = new wasm.VectorUInt();
    for (let j = 0, numNbrs = nbrs.length; j < numNbrs; ++j) {
      wasmNeighborIndices.push_back(nbrs[j]);
    }
    wasmNeighbors.push_back(wasmNeighborIndices);
  }

  const result = wasm.localMoranEB(
    wasmEvent,
    wasmBase,
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
