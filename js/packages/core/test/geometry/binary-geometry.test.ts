// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { getBinaryGeometryTemplate } from '@loaders.gl/arrow';
import { BinaryFeatureCollection } from '@loaders.gl/schema';
import { describe, it, expect } from '@jest/globals';

import {
  createPointCollectionFromBinaryFeatures,
  createLineCollectionFromBinaryFeatures,
  createPolygonCollectionFromBinaryFeatures,
} from '../../src/geometry/binary-geometry';
import { initWASM } from '../../src/init';
import { reducePrecision } from '../utils';

/**
 * test createPointCollectionFromBinaryFeatures, simple points
 */
describe('createPointCollectionFromBinaryFeatures', () => {
  it('should handle simple points', async () => {
    const wasmInstance = await initWASM();

    // featureTypes: {polygon: false, point: true, line: false},
    const pointBinaryGeometry: Array<BinaryFeatureCollection> = [
      {
        shape: 'binary-feature-collection',
        points: {
          ...getBinaryGeometryTemplate(),
          type: 'Point',
          globalFeatureIds: { value: new Uint32Array([0, 1]), size: 1 },
          positions: { value: new Float64Array([1, 1, 2, 2]), size: 2 },
          properties: [{ index: 0 }, { index: 1 }],
          featureIds: { value: new Uint32Array([0, 1]), size: 1 },
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

    const pointsArray = pointBinaryGeometry.map(chunk => chunk.points);
    const points = await createPointCollectionFromBinaryFeatures(pointsArray, wasmInstance);

    // since we don't expose other function for PolygonCollection, we use getCentroids()
    // function to check if the returned value is correct
    const centroids = points.getCentroids();

    expect(centroids?.size()).toBe(2);

    const expectedCentroids = [
      [1, 1],
      [2, 2],
    ];

    const actualCentroids = [
      [centroids.get(0).get(0), centroids.get(0).get(1)],
      [centroids.get(1).get(0), centroids.get(1).get(1)],
    ];

    expect(actualCentroids).toEqual(expectedCentroids);
  });

  it('should handle multi-point features', async () => {
    const wasmInstance = await initWASM();

    const pointBinaryGeometry: Array<BinaryFeatureCollection> = [
      {
        shape: 'binary-feature-collection',
        points: {
          ...getBinaryGeometryTemplate(),
          type: 'Point',
          globalFeatureIds: { value: new Uint32Array([0, 0, 1, 1]), size: 1 },
          positions: { value: new Float64Array([1, 1, 2, 2, 3, 3, 4, 4]), size: 1 },
          properties: [{ index: 0 }, { index: 1 }],
          featureIds: { value: new Uint32Array([0, 0, 1, 1]), size: 1 },
        },
        lines: {
          ...getBinaryGeometryTemplate(),
          type: 'LineString',
          pathIndices: { value: new Uint16Array(0), size: 0 },
        },
        polygons: {
          ...getBinaryGeometryTemplate(),
          type: 'Polygon',
          polygonIndices: { value: new Uint16Array(0), size: 0 },
          primitivePolygonIndices: { value: new Uint16Array(0), size: 0 },
        },
      },
    ];

    const pointsArray = pointBinaryGeometry.map(chunk => chunk.points);
    const points = await createPointCollectionFromBinaryFeatures(pointsArray, wasmInstance);

    const centroids = points.getCentroids();

    expect(centroids?.size()).toBe(2);

    const expectedCentroids = [
      [1.5, 1.5],
      [3.5, 3.5],
    ];

    const actualCentroids = [
      [centroids.get(0).get(0), centroids.get(0).get(1)],
      [centroids.get(1).get(0), centroids.get(1).get(1)],
    ];

    expect(actualCentroids).toEqual(expectedCentroids);
  });

  it('should handle points split across multiple chunks', async () => {
    const wasmInstance = await initWASM();

    // two chunks, each holding two point features; featureIds restart from 0 in
    // every chunk, so the collection is only correct if parts are offset by the
    // points already accumulated from previous chunks.
    const pointBinaryGeometry: Array<BinaryFeatureCollection> = [
      {
        shape: 'binary-feature-collection',
        points: {
          ...getBinaryGeometryTemplate(),
          type: 'Point',
          globalFeatureIds: { value: new Uint32Array([0, 1]), size: 1 },
          positions: { value: new Float64Array([1, 1, 2, 2]), size: 2 },
          properties: [{ index: 0 }, { index: 1 }],
          featureIds: { value: new Uint32Array([0, 1]), size: 1 },
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
      {
        shape: 'binary-feature-collection',
        points: {
          ...getBinaryGeometryTemplate(),
          type: 'Point',
          globalFeatureIds: { value: new Uint32Array([2, 3]), size: 1 },
          positions: { value: new Float64Array([3, 3, 4, 4]), size: 2 },
          properties: [{ index: 2 }, { index: 3 }],
          featureIds: { value: new Uint32Array([0, 1]), size: 1 },
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

    const pointsArray = pointBinaryGeometry.map(chunk => chunk.points);
    const points = await createPointCollectionFromBinaryFeatures(pointsArray, wasmInstance);

    const centroids = points.getCentroids();

    expect(centroids?.size()).toBe(4);

    const expectedCentroids = [
      [1, 1],
      [2, 2],
      [3, 3],
      [4, 4],
    ];

    const actualCentroids = [
      [centroids.get(0).get(0), centroids.get(0).get(1)],
      [centroids.get(1).get(0), centroids.get(1).get(1)],
      [centroids.get(2).get(0), centroids.get(2).get(1)],
      [centroids.get(3).get(0), centroids.get(3).get(1)],
    ];

    expect(actualCentroids).toEqual(expectedCentroids);
  });
});

/**
 * test createLineCollectionFromBinaryFeatures
 */
describe('createLineCollectionFromBinaryFeatures', () => {
  it('should handle simple lines', async () => {
    const wasmInstance = await initWASM();

    const lineBinaryGeometry: Array<BinaryFeatureCollection> = [
      {
        shape: 'binary-feature-collection',
        points: {
          ...getBinaryGeometryTemplate(),
          type: 'Point',
        },
        lines: {
          ...getBinaryGeometryTemplate(),
          type: 'LineString',
          globalFeatureIds: { value: new Uint32Array([0, 0, 1, 1]), size: 1 },
          positions: { value: new Float64Array([0, 0, 1, 1, 2, 2, 3, 3]), size: 2 },
          properties: [{ index: 0 }, { index: 1 }],
          featureIds: { value: new Uint32Array([0, 0, 1, 1]), size: 1 },
          pathIndices: { value: new Int32Array([0, 2, 4]), size: 1 },
        },
        polygons: {
          ...getBinaryGeometryTemplate(),
          type: 'Polygon',
          polygonIndices: { value: new Uint16Array(0), size: 0 },
          primitivePolygonIndices: { value: new Uint16Array(0), size: 0 },
        },
      },
    ];

    const linesArray = lineBinaryGeometry.map(chunk => chunk.lines);
    const lines = await createLineCollectionFromBinaryFeatures(linesArray, wasmInstance);

    const centroids = lines.getCentroids();

    expect(centroids?.size()).toBe(2);

    const expectedCentroids = [
      [0.5, 0.5],
      [2.5, 2.5],
    ];

    const actualCentroids = [
      [centroids.get(0).get(0), centroids.get(0).get(1)],
      [centroids.get(1).get(0), centroids.get(1).get(1)],
    ];

    expect(actualCentroids).toEqual(expectedCentroids);
  });

  it('should handle multilines', async () => {
    const wasmInstance = await initWASM();

    const lineBinaryGeometry: Array<BinaryFeatureCollection> = [
      {
        shape: 'binary-feature-collection',
        points: {
          ...getBinaryGeometryTemplate(),
          type: 'Point',
        },
        lines: {
          ...getBinaryGeometryTemplate(),
          type: 'LineString',
          globalFeatureIds: { value: new Uint32Array([0, 0, 0, 0, 1, 1, 1, 1]), size: 1 },
          positions: {
            value: new Float64Array([1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8]),
            size: 2,
          },
          properties: [{ index: 0 }, { index: 1 }],
          featureIds: { value: new Uint32Array([0, 0, 0, 0, 1, 1, 1, 1]), size: 1 },
          pathIndices: { value: new Int32Array([0, 2, 4, 6, 8]), size: 1 },
        },
        polygons: {
          ...getBinaryGeometryTemplate(),
          type: 'Polygon',
          polygonIndices: { value: new Uint16Array(0), size: 1 },
          primitivePolygonIndices: { value: new Uint16Array(0), size: 1 },
        },
      },
    ];

    const linesArray = lineBinaryGeometry.map(chunk => chunk.lines);
    const lines = await createLineCollectionFromBinaryFeatures(linesArray, wasmInstance);

    const centroids = lines.getCentroids();

    expect(centroids?.size()).toBe(2);

    const expectedCentroids = [
      [2.5, 2.5],
      [6.5, 6.5],
    ];

    const actualCentroids = reducePrecision([
      [centroids.get(0).get(0), centroids.get(0).get(1)],
      [centroids.get(1).get(0), centroids.get(1).get(1)],
    ]);

    expect(actualCentroids).toEqual(expectedCentroids);
  });
});

/**
 * test createPolygonCollectionFromBinaryFeatures, simple polygons
 */
describe('createPolygonCollectionFromBinaryFeatures', () => {
  it('should handle simple polygons', async () => {
    const wasmInstance = await initWASM();

    // featureTypes: {polygon: false, point: true, line: false},
    const polygonBinaryGeometry: Array<BinaryFeatureCollection> = [
      {
        shape: 'binary-feature-collection',
        points: {
          ...getBinaryGeometryTemplate(),
          type: 'Point',
        },
        lines: {
          ...getBinaryGeometryTemplate(),
          type: 'LineString',
          pathIndices: { value: new Uint16Array(0), size: 1 },
        },
        polygons: {
          ...getBinaryGeometryTemplate(),
          type: 'Polygon',
          globalFeatureIds: {
            value: new Uint32Array([0, 0, 0, 0, 0, 1, 1, 1, 1, 1]),
            size: 1,
          },
          positions: {
            value: new Float64Array([
              0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 10, 10, 10, 11, 11, 11, 11, 10, 10, 10,
            ]),
            size: 2,
          },
          properties: [{ index: 0 }, { index: 1 }],
          featureIds: { value: new Uint32Array([0, 0, 0, 0, 0, 1, 1, 1, 1, 1]), size: 1 },
          polygonIndices: { value: new Int32Array([0, 5, 10]), size: 1 },
          primitivePolygonIndices: { value: new Int32Array([0, 5, 10]), size: 1 },
        },
      },
    ];

    const polygonsArray = polygonBinaryGeometry.map(chunk => chunk.polygons);
    const polygons = await createPolygonCollectionFromBinaryFeatures(polygonsArray, wasmInstance);

    // since we don't expose other function for PolygonCollection, we use getCentroids()
    // function to check if the returned value is correct
    const centroids = polygons.getCentroids();

    expect(centroids.size()).toBe(2);

    const expectedCentroids = [
      [0.5, 0.5],
      [10.5, 10.5],
    ];

    const actualCentroids = [
      [centroids.get(0).get(0), centroids.get(0).get(1)],
      [centroids.get(1).get(0), centroids.get(1).get(1)],
    ];

    expect(actualCentroids).toEqual(expectedCentroids);
  });

  it('should handle multi polygons', async () => {
    const wasmInstance = await initWASM();

    // featureTypes: {polygon: false, point: true, line: false},
    const polygonBinaryGeometry: Array<BinaryFeatureCollection> = [
      {
        shape: 'binary-feature-collection',
        points: {
          ...getBinaryGeometryTemplate(),
          type: 'Point',
        },
        lines: {
          ...getBinaryGeometryTemplate(),
          type: 'LineString',
          pathIndices: { value: new Uint16Array(0), size: 1 },
        },
        polygons: {
          ...getBinaryGeometryTemplate(),
          type: 'Polygon',
          globalFeatureIds: {
            value: new Uint32Array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0]),
            size: 1,
          },
          positions: {
            value: new Float64Array([0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2]),
            size: 2,
          },
          properties: [{ index: 0 }],
          featureIds: {
            value: new Uint32Array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0]),
            size: 1,
          },
          polygonIndices: { value: new Int32Array([0, 5, 10]), size: 1 },
          primitivePolygonIndices: { value: new Int32Array([0, 5, 10]), size: 1 },
        },
      },
    ];

    const polygonsArray = polygonBinaryGeometry.map(chunk => chunk.polygons);
    const polygons = await createPolygonCollectionFromBinaryFeatures(polygonsArray, wasmInstance);

    // since we don't expose other function for PolygonCollection, we use getCentroids()
    // function to check if the returned value is correct
    const centroids = polygons.getCentroids();

    expect(centroids?.size()).toBe(1);

    const expectedCentroids = [[1.5, 1.5]];

    const actualCentroids = [[centroids.get(0).get(0), centroids.get(0).get(1)]];

    expect(actualCentroids).toEqual(expectedCentroids);
  });

  it('should handle multi polygons with holes', async () => {
    const wasmInstance = await initWASM();

    // featureTypes: {polygon: false, point: true, line: false},
    const polygonBinaryGeometry: Array<BinaryFeatureCollection> = [
      {
        shape: 'binary-feature-collection',
        points: {
          ...getBinaryGeometryTemplate(),
          type: 'Point',
        },
        lines: {
          ...getBinaryGeometryTemplate(),
          type: 'LineString',
          pathIndices: { value: new Uint16Array(0), size: 1 },
        },
        polygons: {
          ...getBinaryGeometryTemplate(),
          type: 'Polygon',
          globalFeatureIds: {
            value: new Uint32Array([
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
              1,
            ]),
            size: 1,
          },
          positions: {
            value: new Float64Array([
              0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0.25, 0.25, 0.25, 0.75, 0.75, 0.75, 0.75, 0.25, 0.25,
              0.25, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 10, 10, 10, 11, 11, 11, 11, 10, 10, 10, 10.25,
              10.25, 10.25, 10.75, 10.75, 10.75, 10.75, 10.25, 10.25, 10.25, 12, 12, 12, 13, 13, 13,
              13, 12, 12, 12,
            ]),
            size: 2,
          },
          properties: [{ index: 0 }, { index: 1 }],
          featureIds: {
            value: new Uint32Array([
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
              1,
            ]),
            size: 1,
          },
          polygonIndices: { value: new Int32Array([0, 10, 15, 25, 30]), size: 1 },
          primitivePolygonIndices: { value: new Int32Array([0, 5, 10, 15, 20, 25, 30]), size: 1 },
        },
      },
    ];

    const polygonsArray = polygonBinaryGeometry.map(chunk => chunk.polygons);
    const polygons = await createPolygonCollectionFromBinaryFeatures(polygonsArray, wasmInstance);

    // since we don't expose other function for PolygonCollection, we use getCentroids()
    // function to check if the returned value is correct
    const centroids = polygons.getCentroids();

    expect(centroids?.size()).toBe(2);

    const expectedCentroids = [
      [1.3888888888888888, 1.3888888888888888],
      [11.388888888888889, 11.388888888888889],
    ];

    const actualCentroids = [
      [centroids.get(0).get(0), centroids.get(0).get(1)],
      [centroids.get(1).get(0), centroids.get(1).get(1)],
    ];

    expect(actualCentroids).toEqual(expectedCentroids);
  });
});
