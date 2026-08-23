// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { getBinaryGeometryTemplate } from '@loaders.gl/arrow';
import { BinaryFeatureCollection } from '@loaders.gl/schema';
import { describe, it, expect } from '@jest/globals';

import { getKernelWeightsFromBinaryGeometries } from '../../src/weights/kernel-weights';
import { getMetaFromWeights } from '../../src/weights/weights-stats';
import { createWeights } from '../../src/weights/utils';

// Gaussian kernel constant: 1 / sqrt(2 * pi)
const gaussianConst = 1 / Math.sqrt(2 * Math.PI);

describe('Kernel Weights', () => {
  const binaryGeometryType = {
    point: true,
    line: false,
    polygon: false,
  };

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

  it('should calculate gaussian kernel weights correctly', async () => {
    const isMile = false;
    // d(0,2) is the min pairwise distance; use it as the fixed bandwidth so
    // the only neighbor within bandwidth of point 0 is point 2 (z == 1.0).
    const bandwidth = 111.16185827369097;

    const result = await getKernelWeightsFromBinaryGeometries({
      bandwidth,
      kernel: 'gaussian',
      isMile,
      binaryGeometryType,
      binaryGeometries,
    });

    // Expected gaussian weight at z = 1.0 (distance == bandwidth).
    const gaussianAtOne = gaussianConst * Math.exp(-0.5);

    expect(result).toHaveLength(5);

    // point 0: neighbor 2, plus self
    expect(result[0][0]).toBe(2);
    expect(result[0][1]).toBeCloseTo(gaussianAtOne, 12);
    expect(result[0][2]).toBe(0);
    expect(result[0][3]).toBe(1);

    // point 1: no neighbor within bandwidth, only self
    expect(result[1]).toEqual([1, 1]);

    // point 2: neighbor 0, plus self
    expect(result[2][0]).toBe(0);
    expect(result[2][1]).toBeCloseTo(gaussianAtOne, 12);
    expect(result[2][2]).toBe(2);
    expect(result[2][3]).toBe(1);

    // points 3 and 4: no neighbors within bandwidth, only self
    expect(result[3]).toEqual([3, 1]);
    expect(result[4]).toEqual([4, 1]);

    // The interleaved [idx, weight] rows count as distance weights for meta.
    const wMeta = getMetaFromWeights(result, true);
    expect(wMeta.numberOfObservations).toBe(5);
  });

  it('should honor useKernelDiagonals for the self weight', async () => {
    const bandwidth = 111.16185827369097;

    const result = await getKernelWeightsFromBinaryGeometries({
      bandwidth,
      kernel: 'gaussian',
      useKernelDiagonals: true,
      binaryGeometryType,
      binaryGeometries,
    });

    const gaussianAtOne = gaussianConst * Math.exp(-0.5);
    // point 3 has no neighbors, so its only entry is the diagonal = kernel(1.0).
    expect(result[3]).toHaveLength(2);
    expect(result[3][0]).toBe(3);
    expect(result[3][1]).toBeCloseTo(gaussianAtOne, 12);
  });

  it('should apply each kernel function correctly', async () => {
    // With a 180 km bandwidth, point 0's only neighbor within range is point 2
    // (d(0,2) ~= 111.16186 km). This keeps the row deterministic with a single
    // neighbor, so the kernel weight can be validated independently.
    const bandwidth = 180;
    const z = 111.16185827369097 / bandwidth;

    // Reference kernel values at z, from Anselin & Rey (2010) table 5.4.
    const reference: Record<string, number> = {
      triangular: 1 - z,
      uniform: 0.5,
      epanechnikov: (3 / 4) * (1 - z * z),
      quartic: (15 / 16) * Math.pow(1 - z * z, 2),
      gaussian: gaussianConst * Math.exp((-z * z) / 2),
    };

    for (const [kernel, expectedWeight] of Object.entries(reference)) {
      const result = await getKernelWeightsFromBinaryGeometries({
        bandwidth,
        kernel,
        binaryGeometryType,
        binaryGeometries,
      });

      // point 0: neighbor 2 with weight kernel(z), plus self weight 1.0.
      expect(result[0][0]).toBe(2);
      expect(result[0][1]).toBeCloseTo(expectedWeight, 12);
      expect(result[0][2]).toBe(0);
      expect(result[0][3]).toBe(1);
    }
  });

  it('should return only the diagonal when no neighbors fall within the bandwidth', async () => {
    // Nearest neighbor distance is ~111.14 km, so a 100 km bandwidth has no neighbors.
    const bandwidth = 100;

    const result = await getKernelWeightsFromBinaryGeometries({
      bandwidth,
      kernel: 'gaussian',
      binaryGeometryType,
      binaryGeometries,
    });

    for (let i = 0; i < 5; i++) {
      expect(result[i]).toEqual([i, 1]);
    }
  });

  it('should create kernel weights via createWeights', async () => {
    const { weights, weightsMeta } = await createWeights({
      weightsType: 'kernel',
      bandwidth: 111.16185827369097,
      kernel: 'gaussian',
      geometries: binaryGeometries,
    });

    expect(weights).toHaveLength(5);
    expect(weightsMeta.type).toBe('kernel');
    expect(weightsMeta.symmetry).toBe('asymmetric');
    expect(weightsMeta.bandwidth).toBe(111.16185827369097);
    expect(weightsMeta.kernel).toBe('gaussian');
    expect(weightsMeta.numberOfObservations).toBe(5);
  });
});
