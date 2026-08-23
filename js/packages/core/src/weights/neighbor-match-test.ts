// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { BinaryFeatureCollection } from '@loaders.gl/schema';
import { GeometryCollection } from '@geoda/common';

import {
  BinaryGeometryType,
  getGeometryCollectionFromBinaryGeometries,
} from '../geometry/binary-geometry';
import { initWASM } from '../init';
import { vecDoubleToNumber } from '@geoda/common';

/**
 * The result of a local neighbor match test.
 */
export type NeighborMatchTestResult = {
  /** number of common k-nearest neighbors in spatial and attribute space */
  cardinality: number[];
  /** hypergeometric probability of observing that overlap */
  probability: number[];
};

/**
 * Configuration for the local neighbor match test.
 */
export type NeighborMatchTestProps = {
  k: number;
  data: number[][] | Float32Array[];
  scaleMethod?: string;
  distType?: string;
  isMile?: boolean;
  binaryGeometryType: BinaryGeometryType;
  binaryGeometries: BinaryFeatureCollection[];
};

function vecVecDoubleToNumber(v: { size(): number; get(i: number): unknown }): number[][] {
  const rows: number[][] = [];
  for (let i = 0; i < v.size(); ++i) {
    rows.push(vecDoubleToNumber(v.get(i) as never));
  }
  return rows;
}

/**
 * Computes the local neighbor match test, assessing the overlap between k-nearest
 * neighbors in geographic space and k-nearest neighbors in attribute space.
 *
 * @returns {Promise<NeighborMatchTestResult>} the cardinality and probability arrays
 */
export async function getNeighborMatchTestFromBinaryGeometries({
  k,
  data,
  scaleMethod = 'standardize',
  distType = 'euclidean',
  isMile = false,
  binaryGeometryType,
  binaryGeometries,
}: NeighborMatchTestProps): Promise<NeighborMatchTestResult> {
  if (!binaryGeometries || binaryGeometries.length === 0) {
    return { cardinality: [], probability: [] };
  }

  const wasmInstance = await initWASM();
  const geomCollection = await getGeometryCollectionFromBinaryGeometries(
    binaryGeometryType,
    binaryGeometries,
    wasmInstance
  );

  const n = geomCollection.size();
  const wasmData = new wasmInstance.VecVecDouble();
  for (const varData of data) {
    const wasmVar = new wasmInstance.VectorDouble();
    wasmVar.resize(n, 0);
    for (let i = 0; i < n; ++i) wasmVar.set(i, Number(varData[i]));
    wasmData.push_back(wasmVar);
  }

  const result = wasmInstance.neighborMatchTest(
    geomCollection,
    k,
    wasmData,
    scaleMethod,
    distType,
    isMile
  );

  const rows = vecVecDoubleToNumber(result);
  return { cardinality: rows[0] ?? [], probability: rows[1] ?? [] };
}

export async function getNeighborMatchTestFromGeomCollection({
  k,
  data,
  scaleMethod = 'standardize',
  distType = 'euclidean',
  isMile = false,
  geomCollection,
}: {
  k: number;
  data: number[][] | Float32Array[];
  scaleMethod?: string;
  distType?: string;
  isMile?: boolean;
  geomCollection: GeometryCollection;
}): Promise<NeighborMatchTestResult> {
  const wasmInstance = await initWASM();
  const n = geomCollection.size();
  const wasmData = new wasmInstance.VecVecDouble();
  for (const varData of data) {
    const wasmVar = new wasmInstance.VectorDouble();
    wasmVar.resize(n, 0);
    for (let i = 0; i < n; ++i) wasmVar.set(i, Number(varData[i]));
    wasmData.push_back(wasmVar);
  }

  const result = wasmInstance.neighborMatchTest(
    geomCollection,
    k,
    wasmData,
    scaleMethod,
    distType,
    isMile
  );
  const rows = vecVecDoubleToNumber(result);
  return { cardinality: rows[0] ?? [], probability: rows[1] ?? [] };
}
