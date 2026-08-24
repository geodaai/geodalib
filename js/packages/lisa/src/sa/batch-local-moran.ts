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
  const wasmNeighbors = new wasm.VecVecUInt();
  const wasmUndefs = new wasm.VecVecUInt();
  try {
    for (const varData of data) {
      const wasmVar = new wasm.VectorDouble();
      try {
        wasmVar.resize(n, 0);
        for (let i = 0; i < n; ++i) wasmVar.set(i, Number(varData[i]));
        wasmData.push_back(wasmVar);
      } finally {
        // push_back copies the data into wasmData; the JS handle is a separate
        // heap object that must be released to avoid WASM memory growth.
        wasmVar.delete();
      }
    }

    for (let i = 0; i < n; ++i) {
      const nbrs = neighbors[i] ?? [];
      const wNbrs = new wasm.VectorUInt();
      try {
        for (let j = 0; j < nbrs.length; ++j) wNbrs.push_back(nbrs[j]);
        wasmNeighbors.push_back(wNbrs);
      } finally {
        wNbrs.delete();
      }
    }

    // No undefined mask is supported for batch Local Moran; the C++ bridge turns
    // an empty VecVecUInt into all-false per-variable undef rows, so there is no
    // per-variable numObs x numVars zero matrix to build.
    const result = wasm.batchLocalMoran(
      wasmData,
      wasmNeighbors,
      wasmUndefs,
      significanceCutoff,
      permutation,
      seed
    );

    let isValid: boolean;
    let lisaValues: number[][];
    let pValues: number[][];
    let clusters: number[][];
    let lagValues: number[][];
    let nn: number[];
    let labels: string[];
    let colors: string[];
    try {
      isValid = result.isValid();
      lisaValues = vecVecDoubleToNumber(result.getLisaValues());
      pValues = vecVecDoubleToNumber(result.getPValues());
      clusters = vecVecIntToNumber(result.getClusters());
      lagValues = vecVecDoubleToNumber(result.getLagValues());
      nn = ownedVecIntToNumber(result.getNN());
      labels = ownedVecStringArray(result.getLabels());
      colors = ownedVecStringArray(result.getColors());
    } finally {
      // The getter-returned vectors are heap copies owned by JS and are released
      // by the conversion helpers above; the result wrapper itself must still be
      // released even if a conversion throws, so delete it here.
      result.delete();
    }

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
  } finally {
    // Release the outer embind heap objects now that their contents have been
    // copied into the result and converted to JS arrays.
    wasmData.delete();
    wasmNeighbors.delete();
    wasmUndefs.delete();
  }
}
