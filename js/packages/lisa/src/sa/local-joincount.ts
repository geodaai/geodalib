// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { initWASM } from '../init';
import { vecDoubleToNumber, vecIntToNumber, vecStringToArray } from '@geoda/common';

/**
 * Result object containing Local Join Count statistics and cluster information
 *
 * @property {boolean} isValid - Indicates if the analysis was successful
 * @property {number[]} clusters - Cluster assignments for each observation
 * @property {number[]} lagValues - Spatially lagged values
 * @property {number[]} pValues - Statistical significance values
 * @property {number[]} lisaValues - Local Join Count statistics
 * @property {number[]} sigCategories - Significance categories
 * @property {number[]} nn - Number of neighbors for each observation
 * @property {string[]} labels - Descriptive labels for clusters
 * @property {string[]} colors - Color codes for visualization
 */
export type LocalJoinCountResult = {
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
 * Configuration properties for Local Join Count
 * @property {number[] | Float32Array} data - Input binary (0/1) data array
 * @property {number[][]} neighbors - Spatial weights matrix as adjacency list
 * @property {number} permutation - Number of permutations for significance testing
 * @property {number} [significanceCutoff=0.05] - Statistical significance threshold
 * @property {number} [seed=1234567890] - Random seed for reproducibility
 */
export type LocalJoinCountProps = {
  data: number[] | Float32Array;
  neighbors: number[][];
  permutation: number;
  significanceCutoff?: number;
  seed?: number;
};

/**
 * Calculates Local Join Count statistics for binary spatial data.
 *
 * ## Example
 * ```ts
 * import { localJoinCount } from '@geoda/lisa';
 *
 * const data = [1, 0, 1, 0, 1];
 * const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];
 *
 * const result = await localJoinCount({ data, neighbors });
 * console.log(result);
 * ```
 * @param {LocalJoinCountProps} props - Configuration object for Local Join Count
 * @returns {Promise<LocalJoinCountResult>} Promise resolving to Local Join Count statistics
 */
export async function localJoinCount({
  data,
  neighbors,
  permutation = 999,
  significanceCutoff = 0.05,
  seed = 1234567890,
}: LocalJoinCountProps): Promise<LocalJoinCountResult> {
  const wasm = await initWASM();

  const n = data.length;
  const wasmData = new wasm.VectorDouble();
  wasmData.resize(n, 0);
  for (let i = 0; i < n; ++i) {
    wasmData.set(i, Number(data[i]));
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

  const result = wasm.localJoinCount(
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

/**
 * Configuration properties for Multivariate Local Join Count calculation.
 * @property {number[][] | Float32Array[]} data - Multiple binary (0/1) data variables
 * @property {number[][]} neighbors - Spatial weights matrix as adjacency list
 * @property {number} permutation - Number of permutations for significance testing
 * @property {number} [significanceCutoff=0.05] - Statistical significance threshold
 * @property {number} [seed=1234567890] - Random seed for reproducibility
 */
export type MultivariateLocalJoinCountProps = {
  data: number[][] | Float32Array[];
  neighbors: number[][];
  permutation: number;
  significanceCutoff?: number;
  seed?: number;
};

/**
 * Calculates Multivariate Local Join Count statistics for multiple binary variables.
 *
 * @param {MultivariateLocalJoinCountProps} props - Configuration object
 * @returns {Promise<LocalJoinCountResult>} Promise resolving to Multivariate Local Join Count statistics
 */
export async function multivariateLocalJoinCount({
  data,
  neighbors,
  permutation = 999,
  significanceCutoff = 0.05,
  seed = 1234567890,
}: MultivariateLocalJoinCountProps): Promise<LocalJoinCountResult> {
  const wasm = await initWASM();

  const n = neighbors.length;
  const wasmData = new wasm.VecVecDouble();
  for (let v = 0; v < data.length; ++v) {
    const varData = new wasm.VectorDouble();
    varData.resize(n, 0);
    for (let i = 0; i < n; ++i) {
      varData.set(i, Number(data[v][i]));
    }
    wasmData.push_back(varData);
  }

  const wasmNeighbors = new wasm.VecVecUInt();
  for (let i = 0; i < n; ++i) {
    const nbrs = neighbors[i] ?? [];
    const wasmNeighborIndices = new wasm.VectorUInt();
    for (let j = 0, numNbrs = nbrs.length; j < numNbrs; ++j) {
      wasmNeighborIndices.push_back(nbrs[j]);
    }
    wasmNeighbors.push_back(wasmNeighborIndices);
  }

  const wasmUndefs = new wasm.VecVecUInt();
  for (let v = 0; v < data.length; ++v) {
    const undef = new wasm.VectorUInt();
    undef.resize(n, 0);
    wasmUndefs.push_back(undef);
  }

  const result = wasm.multivariateLocalJoinCount(
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
