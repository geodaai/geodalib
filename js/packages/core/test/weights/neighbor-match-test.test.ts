// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { getBinaryGeometryTemplate } from '@loaders.gl/arrow';
import { BinaryFeatureCollection } from '@loaders.gl/schema';
import { describe, it, expect } from '@jest/globals';

import { getGeometryCollectionFromBinaryGeometries } from '../../src/geometry/binary-geometry';
import { initWASM } from '../../src/init';
import {
  getNeighborMatchTestFromBinaryGeometries,
  getNeighborMatchTestFromGeomCollection,
} from '../../src/weights/neighbor-match-test';

describe('Neighbor Match Test', () => {
  const binaryGeometryType = { point: true, line: false, polygon: false };

  const binaryGeometries: BinaryFeatureCollection[] = [
    {
      shape: 'binary-feature-collection',
      points: {
        ...getBinaryGeometryTemplate(),
        type: 'Point',
        globalFeatureIds: { value: new Uint32Array([0, 1, 2, 3, 4]), size: 1 },
        positions: {
          value: new Float64Array([1.4, 1.4, 0.2, 0.2, 2.4, 1.4, 21.0, 21.0, 15.4, 15.4]),
          size: 2,
        },
        properties: [{ index: 0 }, { index: 1 }, { index: 2 }, { index: 3 }, { index: 4 }],
        featureIds: { value: new Uint32Array([0, 1, 2, 3, 4]), size: 1 },
      },
      lines: {
        ...getBinaryGeometryTemplate(),
        type: 'LineString',
        pathIndices: { value: new Uint16Array(0), size: 1 },
      },
      polygons: {
        ...getBinaryGeometryTemplate(),
        type: 'Polygon',
        polygonIndices: { value: new Uint16Array(0), size: 1 },
        primitivePolygonIndices: { value: new Uint16Array(0), size: 1 },
      },
    },
  ];

  it('should compute the neighbor match test', async () => {
    const result = await getNeighborMatchTestFromBinaryGeometries({
      k: 1,
      data: [[1, 2, 3, 4, 5]],
      binaryGeometryType,
      binaryGeometries,
    });

    expect(result.cardinality).toEqual([0, 1, 0, 0, 1]);
    // k = 1 with 5 observations: universe = 4, P(0) = 3/4, P(1) = 1/4.
    // Probabilities are computed via log-gamma combinations, so compare with tolerance.
    const expectedProb = [0.75, 0.25, 0.75, 0.75, 0.25];
    result.probability.forEach((p, i) => expect(p).toBeCloseTo(expectedProb[i], 12));
  });

  it('should reject variables whose length does not match the observations', async () => {
    await expect(
      getNeighborMatchTestFromBinaryGeometries({
        k: 1,
        data: [[1, 2, 3, 4]],
        binaryGeometryType,
        binaryGeometries,
      })
    ).rejects.toThrow('must have 5 values');
  });

  it('should reject k at least as large as the number of observations', async () => {
    const result = await getNeighborMatchTestFromBinaryGeometries({
      k: 5,
      data: [[1, 2, 3, 4, 5]],
      binaryGeometryType,
      binaryGeometries,
    });
    expect(result.cardinality).toEqual([]);
    expect(result.probability).toEqual([]);
  });

  // The direct GeometryCollection entry point has its own WASM vector
  // construction and length validation, so it must be exercised directly rather
  // than only through the binary-geometries wrapper.
  it('should compute the neighbor match test from a GeometryCollection', async () => {
    const wasmInstance = await initWASM();
    const geomCollection = await getGeometryCollectionFromBinaryGeometries(
      binaryGeometryType,
      binaryGeometries,
      wasmInstance
    );

    const result = await getNeighborMatchTestFromGeomCollection({
      k: 1,
      data: [[1, 2, 3, 4, 5]],
      geomCollection,
    });

    expect(result.cardinality).toEqual([0, 1, 0, 0, 1]);
    const expectedProb = [0.75, 0.25, 0.75, 0.75, 0.25];
    result.probability.forEach((p, i) => expect(p).toBeCloseTo(expectedProb[i], 12));
  });

  it('should reject a length mismatch from a GeometryCollection', async () => {
    const wasmInstance = await initWASM();
    const geomCollection = await getGeometryCollectionFromBinaryGeometries(
      binaryGeometryType,
      binaryGeometries,
      wasmInstance
    );

    await expect(
      getNeighborMatchTestFromGeomCollection({
        k: 1,
        data: [[1, 2, 3, 4]],
        geomCollection,
      })
    ).rejects.toThrow('must have 5 values');
  });
});
