// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { initWASM } from '../init';

/**
 * Makes a set of clusters spatially contiguous by reassigning disconnected
 * components, mirroring pygeoda's `make_spatial`.
 *
 * ## Example
 * ```ts
 * import { makeSpatial } from '@geoda/core';
 *
 * const clusters = [[0, 1, 2], [3, 4]];
 * const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];
 * const result = await makeSpatial({ clusters, neighbors });
 * ```
 */
export async function makeSpatial({
  clusters,
  neighbors,
}: {
  /** list of clusters, each a list of observation indices */
  clusters: number[][];
  /** spatial weights matrix as adjacency list */
  neighbors: number[][];
}): Promise<number[][]> {
  // Validate inputs before touching WASM: out-of-range indices would be passed
  // to VectorWeight::GetNeighbors() in the C++ implementation, and an empty
  // cluster leaves the MakeSpatialCluster core null — both crash the WASM
  // runtime instead of producing a result.
  const n = neighbors.length;
  if (n === 0) {
    throw new Error('makeSpatial: neighbors must contain at least one observation');
  }
  if (clusters.length === 0) {
    throw new Error('makeSpatial: clusters must contain at least one cluster');
  }
  for (const c of clusters) {
    if (c.length === 0) {
      throw new Error('makeSpatial: clusters must not contain an empty cluster');
    }
    for (const e of c) {
      if (!Number.isInteger(e) || e < 0 || e >= n) {
        throw new Error(
          `makeSpatial: cluster element ${e} is out of range (expected an integer in 0..${n - 1})`
        );
      }
    }
  }
  for (const nbrs of neighbors) {
    for (const nb of nbrs) {
      if (!Number.isInteger(nb) || nb < 0 || nb >= n) {
        throw new Error(
          `makeSpatial: neighbor index ${nb} is out of range (expected an integer in 0..${n - 1})`
        );
      }
    }
  }

  const wasm = await initWASM();

  const wasmClusters = new wasm.VecVecInt();
  const wasmNeighbors = new wasm.VecVecUInt();
  try {
    for (const c of clusters) {
      const wc = new wasm.VectorInt();
      try {
        for (const e of c) wc.push_back(e);
        wasmClusters.push_back(wc);
      } finally {
        // push_back copies into wasmClusters; release the temporary handle.
        wc.delete();
      }
    }

    for (const nbrs of neighbors) {
      const wn = new wasm.VectorUInt();
      try {
        for (const n of nbrs) wn.push_back(n);
        wasmNeighbors.push_back(wn);
      } finally {
        wn.delete();
      }
    }

    const result = wasm.makeSpatial(wasmClusters, wasmNeighbors);

    const out: number[][] = [];
    for (let i = 0; i < result.size(); ++i) {
      const row = result.get(i);
      const vals: number[] = [];
      for (let j = 0; j < row.size(); ++j) vals.push(row.get(j));
      out.push(vals);
      row.delete();
    }
    result.delete();
    return out;
  } finally {
    wasmClusters.delete();
    wasmNeighbors.delete();
  }
}
