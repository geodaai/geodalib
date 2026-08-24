// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { GeometryCollection } from '@geoda/common';
import { describe, it, expect } from '@jest/globals';

import { spatialValidation } from '../../src/clustering/spatial-validation';

describe('spatialValidation()', () => {
  it('should compute spatial validation metrics', async () => {
    const { initWASM } = await import('../../src/init');
    const wasm = await initWASM();

    // non-collinear points so the convex hull is not degenerate
    const xs = new wasm.VectorDouble();
    for (const v of [0, 1, 0, 2, 2]) xs.push_back(v);
    const ys = new wasm.VectorDouble();
    for (const v of [0, 0, 2, 1, 2]) ys.push_back(v);
    const parts = new wasm.VectorUInt();
    for (let i = 0; i < 5; ++i) parts.push_back(i);
    const sizes = new wasm.VectorUInt();
    for (let i = 0; i < 5; ++i) sizes.push_back(1);
    const geoms = new wasm.PointCollection(xs, ys, parts, sizes, false);

    const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];
    const result = await spatialValidation({
      clusters: [1, 1, 1, 2, 2],
      geometries: geoms,
      neighbors,
    });

    expect(result.spatiallyConstrained).toBe(true);
    geoms.delete();
  });

  it('should reject clusters whose length does not match the observations', async () => {
    const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];
    await expect(
      spatialValidation({
        clusters: [1, 1, 1],
        geometries: null as unknown as GeometryCollection,
        neighbors,
      })
    ).rejects.toThrow('clusters must have one entry per observation');
  });

  it('should reject geometries whose size does not match the observations', async () => {
    const { initWASM } = await import('../../src/init');
    const wasm = await initWASM();
    const xs = new wasm.VectorDouble();
    for (const v of [0, 1]) xs.push_back(v);
    const ys = new wasm.VectorDouble();
    for (const v of [0, 0]) ys.push_back(v);
    const parts = new wasm.VectorUInt();
    for (let i = 0; i < 2; ++i) parts.push_back(i);
    const sizes = new wasm.VectorUInt();
    for (let i = 0; i < 2; ++i) sizes.push_back(1);
    const geoms = new wasm.PointCollection(xs, ys, parts, sizes, false);

    const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];
    await expect(
      spatialValidation({ clusters: [1, 1, 1, 2, 2], geometries: geoms, neighbors })
    ).rejects.toThrow('geometries must have one feature per observation');
    geoms.delete();
  });
});
