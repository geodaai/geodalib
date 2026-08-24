// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { BinaryFeatureCollection } from '@loaders.gl/schema';
import { GeometryCollection } from '@geoda/common';

import {
  BinaryGeometryType,
  getGeometryCollectionFromBinaryGeometries,
} from '../geometry/binary-geometry';
import { initWASM } from '../init';

/**
 * Type of Kernel K-Nearest Neighbors weights from binary geometries arguments.
 */
type KernelKnnWeightsFromBinaryGeometriesProps = {
  k: number;
  kernel: string;
  isMile?: boolean;
  useKernelDiagonals?: boolean;
  power?: number;
  adaptiveBandwidth?: boolean;
  isInverse?: boolean;
  binaryGeometryType: BinaryGeometryType;
  binaryGeometries: BinaryFeatureCollection[];
};

/**
 * Calculates k-nearest neighbor kernel weights for a given set of geometries or
 * latitude/longitude arrays. Each row is an interleaved list of [neighborIndex, weight]
 * pairs, with the diagonal (self) element appended last.
 * @param {KernelKnnWeightsFromBinaryGeometriesProps} input - The input parameters.
 * @returns {Promise<number[][]>} - The kernel weights as rows of interleaved [index, weight] pairs.
 */
export async function getKernelKnnWeightsFromBinaryGeometries({
  k,
  kernel,
  isMile = false,
  useKernelDiagonals = false,
  power = 1.0,
  adaptiveBandwidth = true,
  isInverse = false,
  binaryGeometryType,
  binaryGeometries,
}: KernelKnnWeightsFromBinaryGeometriesProps): Promise<number[][]> {
  if (!binaryGeometries || binaryGeometries.length === 0) {
    return [];
  }

  const wasmInstance = await initWASM();
  const geomCollection = await getGeometryCollectionFromBinaryGeometries(
    binaryGeometryType,
    binaryGeometries,
    wasmInstance
  );

  return await getKernelKnnWeightsFromGeomCollection({
    k,
    kernel,
    geomCollection,
    isMile,
    useKernelDiagonals,
    power,
    adaptiveBandwidth,
    isInverse,
  });
}

/**
 * Calculates k-nearest neighbor kernel weights for a given set of geometries.
 *
 * ## Example
 * ```ts
 * import { getKernelKnnWeightsFromGeomCollection } from '@geoda/core';
 *
 * const geometries = [
 *   { type: 'Feature', geometry: { type: 'Point', coordinates: [0, 0] } },
 *   { type: 'Feature', geometry: { type: 'Point', coordinates: [1, 0] } },
 *   { type: 'Feature', geometry: { type: 'Point', coordinates: [0, 1] } },
 * ];
 *
 * const weights = await getKernelKnnWeightsFromGeomCollection({
 *   k: 2,
 *   kernel: 'gaussian',
 *   geomCollection: geometries,
 * });
 *
 * console.log(weights);
 * ```
 *
 * @returns {Promise<number[][]>} - The kernel weights as rows of interleaved [index, weight] pairs.
 */
export async function getKernelKnnWeightsFromGeomCollection({
  k,
  kernel,
  geomCollection,
  isMile = false,
  useKernelDiagonals = false,
  power = 1.0,
  adaptiveBandwidth = true,
  isInverse = false,
}: {
  /**
   * The number of nearest neighbors.
   */
  k: number;
  /**
   * The kernel function (triangular, uniform, epanechnikov, quartic, gaussian).
   */
  kernel: string;
  /**
   * The geometry collection to calculate the weights for.
   */
  geomCollection: GeometryCollection;
  /**
   * The unit of distance (mile or km).
   */
  isMile?: boolean;
  /**
   * Whether the diagonal (self) weight is kernel(0.0) instead of 1.0.
   */
  useKernelDiagonals?: boolean;
  /**
   * The power (or exponent) used by the inverse distance weighting
   * (1 / distance^power). Only applied when isInverse is true.
   */
  power?: number;
  /**
   * Whether to use each observation's k-th nearest neighbor distance as its bandwidth (true)
   * or a single global maximum distance (false).
   */
  adaptiveBandwidth?: boolean;
  /**
   * Whether to apply inverse distance weighting (1 / distance^power) before the kernel.
   */
  isInverse?: boolean;
}): Promise<number[][]> {
  const wasmInstance = await initWASM();
  const weights: number[][] = [];
  if (geomCollection) {
    const result = wasmInstance.getKernelKnnWeights(
      geomCollection,
      k,
      kernel,
      isMile,
      useKernelDiagonals,
      power,
      adaptiveBandwidth,
      isInverse
    );
    for (let i = 0; i < result.size(); ++i) {
      const row = result.get(i);
      const rowValues: number[] = Array(row.size());
      for (let j = 0, rowSize = row.size(); j < rowSize; ++j) {
        rowValues[j] = row.get(j);
      }
      weights[i] = rowValues;
    }
  }
  return weights;
}
