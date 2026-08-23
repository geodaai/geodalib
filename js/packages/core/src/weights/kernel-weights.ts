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
 * Type of Kernel weights from binary geometries arguments.
 */
type KernelWeightsFromBinaryGeometriesProps = {
  bandwidth: number;
  kernel: string;
  isMile?: boolean;
  useKernelDiagonals?: boolean;
  power?: number;
  binaryGeometryType: BinaryGeometryType;
  binaryGeometries: BinaryFeatureCollection[];
};

/**
 * Calculates the kernel weights within a fixed bandwidth for a given set of geometries or
 * latitude/longitude arrays. Each row is an interleaved list of [neighborIndex, weight] pairs,
 * with the diagonal (self) element appended last.
 * @param {KernelWeightsFromBinaryGeometriesProps} input - The input parameters.
 * @returns {Promise<number[][]>} - The kernel weights as rows of interleaved [index, weight] pairs.
 */
export async function getKernelWeightsFromBinaryGeometries({
  bandwidth,
  kernel,
  isMile = false,
  useKernelDiagonals = false,
  power = 1.0,
  binaryGeometryType,
  binaryGeometries,
}: KernelWeightsFromBinaryGeometriesProps): Promise<number[][]> {
  if (!binaryGeometries || binaryGeometries.length === 0) {
    return [];
  }

  const wasmInstance = await initWASM();
  const geomCollection = await getGeometryCollectionFromBinaryGeometries(
    binaryGeometryType,
    binaryGeometries,
    wasmInstance
  );

  const weights = await getKernelWeightsFromGeomCollection({
    geomCollection,
    bandwidth,
    kernel,
    isMile,
    useKernelDiagonals,
    power,
  });

  return weights;
}

export async function getKernelWeightsFromGeomCollection({
  geomCollection,
  bandwidth,
  kernel,
  isMile = false,
  useKernelDiagonals = false,
  power = 1.0,
}: {
  geomCollection: GeometryCollection;
  bandwidth: number;
  kernel: string;
  isMile?: boolean;
  useKernelDiagonals?: boolean;
  power?: number;
}): Promise<number[][]> {
  const wasmInstance = await initWASM();
  const weights: number[][] = [];
  if (geomCollection) {
    const result = wasmInstance.getKernelWeights(
      geomCollection,
      bandwidth,
      kernel,
      isMile,
      useKernelDiagonals,
      power
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
