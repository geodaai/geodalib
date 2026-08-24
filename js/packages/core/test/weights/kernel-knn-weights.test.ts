// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { getBinaryGeometryTemplate } from '@loaders.gl/arrow';
import { BinaryFeatureCollection } from '@loaders.gl/schema';
import { describe, it, expect } from '@jest/globals';

import { getKernelKnnWeightsFromBinaryGeometries } from '../../src/weights/kernel-knn-weights';

const gaussianConst = 1 / Math.sqrt(2 * Math.PI);

describe('Kernel KNN Weights', () => {
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

  it('should calculate adaptive kernel weights for k=1', async () => {
    // With k=1 and adaptive bandwidth, each observation's single neighbor sits exactly at the
    // bandwidth boundary (z = d / d = 1.0), so every gaussian weight equals gaussian(1.0).
    const gaussianAtOne = gaussianConst * Math.exp(-0.5);

    const result = await getKernelKnnWeightsFromBinaryGeometries({
      k: 1,
      kernel: 'gaussian',
      binaryGeometryType,
      binaryGeometries,
    });

    expect(result).toHaveLength(5);
    for (let i = 0; i < result.length; i++) {
      // each row: [neighborIdx, weight, selfIdx, selfWeight]
      expect(result[i]).toHaveLength(4);
      expect(result[i][0]).not.toBe(i); // neighbor is not self
      expect(result[i][1]).toBeCloseTo(gaussianAtOne, 12);
      expect(result[i][2]).toBe(i);
      expect(result[i][3]).toBe(1);
    }
  });

  it('should calculate global uniform weights for k=2', async () => {
    const result = await getKernelKnnWeightsFromBinaryGeometries({
      k: 2,
      kernel: 'uniform',
      adaptiveBandwidth: false,
      binaryGeometryType,
      binaryGeometries,
    });

    expect(result).toHaveLength(5);
    for (let i = 0; i < result.length; i++) {
      // 2 neighbors + self = 6 numbers
      expect(result[i]).toHaveLength(6);
      for (let j = 0; j < 6; j += 2) {
        if (result[i][j] === i) {
          expect(result[i][j + 1]).toBe(1);
        } else {
          expect(result[i][j + 1]).toBeCloseTo(0.5, 12);
        }
      }
    }
  });
});
