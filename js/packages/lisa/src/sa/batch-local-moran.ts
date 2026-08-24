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

/**
 * Converts an embind VecVecInt (vector of VectorInt) to a JS array of arrays,
 * releasing the embind objects. The outer vector returned by the getter is a
 * heap copy owned by JS and must be .delete()d to free WASM memory; the inner
 * handles returned by get() are non-owning aliases that should still be
 * released so their wrappers do not accumulate.
 */
function vecVecIntToNumber(v: { size(): number; get(i: number): unknown; delete(): void }): number[][] {
  const rows: number[][] = [];
  try {
    for (let i = 0; i < v.size(); ++i) {
      const inner = v.get(i) as { delete(): void };
      try {
        rows.push(vecIntToNumber(inner as never));
      } finally {
        inner.delete();
      }
    }
  } finally {
    v.delete();
  }
  return rows;
}

/**
 * Converts a VecVecDouble to a JS array of arrays, releasing the outer vector
 * (owned by JS) and the inner VectorDouble handles, as in vecVecIntToNumber.
 */
function vecVecDoubleToNumber(v: { size(): number; get(i: number): unknown; delete(): void }): number[][] {
  const rows: number[][] = [];
  try {
    for (let i = 0; i < v.size(); ++i) {
      const inner = v.get(i) as { delete(): void };
      try {
        rows.push(vecDoubleToNumber(inner as never));
      } finally {
        inner.delete();
      }
    }
  } finally {
    v.delete();
  }
  return rows;
}

/**
 * Converts an embind Vector<T> to a JS array and then releases the heap object.
 * The common helpers convert without deleting, so callers that own the handle
 * must release it explicitly.
 */
function ownedVecIntToNumber(v: { delete(): void }): number[] {
  try {
    return vecIntToNumber(v as never);
  } finally {
    v.delete();
  }
}

function ownedVecStringArray(v: { delete(): void }): string[] {
  try {
    return vecStringToArray(v as never);
  } finally {
    v.delete();
  }
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
  if (n === 0) {
    throw new Error('batchLocalMoran: neighbors must contain at least one observation');
  }
  if (data.length === 0) {
    throw new Error('batchLocalMoran: data must contain at least one variable');
  }
  for (const varData of data) {
    if (varData.length !== n) {
      throw new Error(
        `batchLocalMoran: each variable must have ${n} values, got ${varData.length}`
      );
    }
  }

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

  const isValid = result.isValid();
  const lisaValues = vecVecDoubleToNumber(result.getLisaValues());
  const pValues = vecVecDoubleToNumber(result.getPValues());
  const clusters = vecVecIntToNumber(result.getClusters());
  const lagValues = vecVecDoubleToNumber(result.getLagValues());
  const nn = ownedVecIntToNumber(result.getNN());
  const labels = ownedVecStringArray(result.getLabels());
  const colors = ownedVecStringArray(result.getColors());
  // BatchLisaResult owns the vectors behind the getters above; by the time we
  // reach here all of them have been converted to JS arrays and released, so
  // it is safe to release the result object itself.
  result.delete();

  return {
    isValid,
    lisaValues,
    pValues,
    clusters,
    lagValues,
    nn,
    labels,
    colors,
  };
}
