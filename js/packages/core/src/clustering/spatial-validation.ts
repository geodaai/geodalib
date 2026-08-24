// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { GeometryCollection } from '@geoda/common';
import { initWASM } from '../init';

/**
 * Result of spatial validation.
 */
export type SpatialValidationResult = {
  spatiallyConstrained: boolean;
  fragmentation: {
    n: number;
    entropy: number;
    simpson: number;
    minClusterSize: number;
    maxClusterSize: number;
    meanClusterSize: number;
    spatiallyContiguous: boolean;
  };
  clusterFragmentation: {
    n: number;
    entropy: number;
    simpson: number;
    minClusterSize: number;
    maxClusterSize: number;
    meanClusterSize: number;
    spatiallyContiguous: boolean;
  }[];
  clusterDiameter: { steps: number; ratio: number }[];
  clusterCompactness: { area: number; perimeter: number; isoperimeterQuotient: number }[];
  joincountRatio: { cluster: number; n: number; ratio: number }[];
};

/**
 * Computes spatial validation metrics for a clustering result.
 */
export async function spatialValidation({
  clusters,
  geometries,
  neighbors,
}: {
  /** cluster id per observation */
  clusters: number[];
  /** geometry collection (use @geoda/core to build it) */
  geometries: GeometryCollection;
  /** spatial weights matrix as adjacency list */
  neighbors: number[][];
}): Promise<SpatialValidationResult> {
  const wasm = await initWASM();

  // The C++ implementation indexes clusters[i] and geoms.get_centroid(i) for
  // i < neighbors.size(); reject mismatched inputs before touching WASM.
  const n = neighbors.length;
  if (n === 0) {
    throw new Error('spatialValidation: neighbors must contain at least one observation');
  }
  if (clusters.length !== n) {
    throw new Error(
      `spatialValidation: clusters must have one entry per observation (${n}, got ${clusters.length})`
    );
  }
  if (geometries.size() !== n) {
    throw new Error(
      `spatialValidation: geometries must have one feature per observation (${n}, got ${geometries.size()})`
    );
  }

  const wasmClusters = new wasm.VectorInt();
  for (const c of clusters) wasmClusters.push_back(c);

  const wasmNeighbors = new wasm.VecVecUInt();
  for (const nbrs of neighbors) {
    const wn = new wasm.VectorUInt();
    for (const nb of nbrs) wn.push_back(nb);
    wasmNeighbors.push_back(wn);
  }

  const result = wasm.spatialValidation(wasmClusters, wasmNeighbors, geometries);

  const frag = (f: {
    n: number;
    entropy: number;
    simpson: number;
    minClusterSize: number;
    maxClusterSize: number;
    meanClusterSize: number;
    spatiallyContiguous: boolean;
  }) => ({
    n: f.n,
    entropy: f.entropy,
    simpson: f.simpson,
    minClusterSize: f.minClusterSize,
    maxClusterSize: f.maxClusterSize,
    meanClusterSize: f.meanClusterSize,
    spatiallyContiguous: f.spatiallyContiguous,
  });

  const clusterFragmentation: SpatialValidationResult['clusterFragmentation'] = [];
  for (let i = 0; i < result.clusterFragmentation.size(); ++i) {
    clusterFragmentation.push(frag(result.clusterFragmentation.get(i)));
  }
  const clusterDiameter: { steps: number; ratio: number }[] = [];
  for (let i = 0; i < result.clusterDiameter.size(); ++i) {
    clusterDiameter.push({
      steps: result.clusterDiameter.get(i).steps,
      ratio: result.clusterDiameter.get(i).ratio,
    });
  }
  const clusterCompactness: { area: number; perimeter: number; isoperimeterQuotient: number }[] =
    [];
  for (let i = 0; i < result.clusterCompactness.size(); ++i) {
    clusterCompactness.push({
      area: result.clusterCompactness.get(i).area,
      perimeter: result.clusterCompactness.get(i).perimeter,
      isoperimeterQuotient: result.clusterCompactness.get(i).isoperimeterQuotient,
    });
  }
  const joincountRatio: { cluster: number; n: number; ratio: number }[] = [];
  for (let i = 0; i < result.joincountRatio.size(); ++i) {
    joincountRatio.push({
      cluster: result.joincountRatio.get(i).cluster,
      n: result.joincountRatio.get(i).n,
      ratio: result.joincountRatio.get(i).ratio,
    });
  }

  return {
    spatiallyConstrained: result.spatiallyConstrained,
    fragmentation: frag(result.fragmentation),
    clusterFragmentation,
    clusterDiameter,
    clusterCompactness,
    joincountRatio,
  };
}
