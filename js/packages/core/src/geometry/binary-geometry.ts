// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { BinaryFeatureCollection } from '@loaders.gl/schema';

import {
  GeoDaModule,
  PolygonCollection,
  PointCollection,
  LineCollection,
  GeometryCollection,
} from '@geoda/common';

/**
 * Defines the geometry types supported in binary format. See DeckGlGeoTypes in kepler.gl/layers
 * @typedef {Object} BinaryGeometryType
 * @property {boolean} point - Whether the geometry contains point features
 * @property {boolean} line - Whether the geometry contains line features
 * @property {boolean} polygon - Whether the geometry contains polygon features
 */
export type BinaryGeometryType = {
  point: boolean;
  line: boolean;
  polygon: boolean;
};

/**
 * Creates a GeoDa GeometryCollection from binary geometry features
 * @param {BinaryGeometryType} geometryType - The type of geometry to create
 * @param {BinaryFeatureCollection[]} binaryFeaturesChunks - Array of binary feature collections. See BinaryFeatureCollection in `@loaders.gl/schema`
 * @param {GeoDaModule} wasm - The initialized GeoDa WASM module
 * @returns {Promise<GeometryCollection>} A GeoDa geometry collection
 * @throws {Error} If WASM module is not initialized or geometry type is unknown
 */
export async function getGeometryCollectionFromBinaryGeometries(
  geometryType: BinaryGeometryType,
  binaryFeaturesChunks: BinaryFeatureCollection[],
  wasm: GeoDaModule,
  fixPolygon?: boolean,
  convertToUTM?: boolean
): Promise<GeometryCollection> {
  if (!wasm) {
    throw new Error('GeoDa WASM module is not initialized');
  }

  if (geometryType.point) {
    const pointsArray = binaryFeaturesChunks.map(chunk => chunk.points);
    return createPointCollectionFromBinaryFeatures(pointsArray, wasm, convertToUTM);
  } else if (geometryType.line) {
    const linesArray = binaryFeaturesChunks.map(chunk => chunk.lines);
    return createLineCollectionFromBinaryFeatures(linesArray, wasm, convertToUTM);
  } else if (geometryType.polygon) {
    const polygonsArray = binaryFeaturesChunks.map(chunk => chunk.polygons);
    return createPolygonCollectionFromBinaryFeatures(polygonsArray, wasm, fixPolygon, convertToUTM);
  }
  throw new Error('getGeometryCollectionFromBinaryGeometries: Binary geometry type is unknown.');
}

/**
 * Creates a GeoDa PointCollection from binary point features
 * @param {Array<BinaryFeatureCollection['points']>} pointsArray - Array of binary point features from GeoArrow chunks
 * @param {GeoDaModule} wasm - The initialized GeoDa WASM module
 * @returns {PointCollection} A GeoDa point collection
 */
export function createPointCollectionFromBinaryFeatures(
  pointsArray: Array<BinaryFeatureCollection['points']>,
  wasm: GeoDaModule,
  convertToUTM?: boolean
): PointCollection {
  // create PointCollection from binaryFeatures
  const xs = new wasm.VectorDouble();
  const ys = new wasm.VectorDouble();
  const parts = new wasm.VectorUInt();
  const sizes = new wasm.VectorUInt();

  // parts index into the concatenated xs/ys arrays, so each chunk's part starts
  // must be offset by the coordinates already added from previous chunks.
  let pointOffset = 0;
  for (let chunkIndex = 0; chunkIndex < pointsArray.length; chunkIndex++) {
    const points = pointsArray[chunkIndex];
    if (points) {
      const coords = points.positions.value;
      for (let i = 0; i < coords.length; i += 2) {
        xs.push_back(coords[i]);
        ys.push_back(coords[i + 1]);
      }
      // get index as the start of each part when points.featureIds.value[i] changed
      const chunkStart = pointOffset;
      pointOffset += coords.length / 2;
      let index = chunkStart;
      for (let i = 0; i < points.featureIds.value.length; i++) {
        if (i === 0 || points.featureIds.value[i] !== points.featureIds.value[i - 1]) {
          parts.push_back(index);
        }
        index++;
      }
    }
  }
  // get sizes from the full parts list
  for (let i = 1; i < parts.size(); i++) {
    sizes.push_back(parts.get(i) - parts.get(i - 1));
  }
  // add the last size
  if (parts.size() > 0) {
    sizes.push_back(pointOffset - parts.get(parts.size() - 1));
  }
  const pointCollection = new wasm.PointCollection(xs, ys, parts, sizes, convertToUTM || false);
  return pointCollection;
}

/**
 * Creates a GeoDa LineCollection from binary line features
 * @param {Array<BinaryFeatureCollection['lines']>} linesArray - Array of binary line features from GeoArrow chunks
 * @param {GeoDaModule} wasm - The initialized GeoDa WASM module
 * @returns {LineCollection} A GeoDa line collection
 */
export function createLineCollectionFromBinaryFeatures(
  linesArray: Array<BinaryFeatureCollection['lines']>,
  wasm: GeoDaModule,
  convertToUTM?: boolean
): LineCollection {
  // create LineCollection from array of binaryFeatures
  const xs = new wasm.VectorDouble();
  const ys = new wasm.VectorDouble();
  const parts = new wasm.VectorUInt();
  const sizes = new wasm.VectorUInt();

  let lastStartPointIndex = 0;
  for (let lineIndex = 0; lineIndex < linesArray.length; lineIndex++) {
    const lines = linesArray[lineIndex];
    if (lines) {
      const coords = lines.positions.value;
      const geomOffsets = lines.pathIndices.value;

      // get xs, ys, parts, sizes
      for (let i = 0; i < coords.length; i += 2) {
        xs.push_back(coords[i]);
        ys.push_back(coords[i + 1]);
      }
      // parts is geomOffsets: store the point index of each part
      // get sizes from featureIds: store number of parts for each line/multiline
      let numParts = 0;
      for (let i = 0; i < geomOffsets.length - 1; i++) {
        const startPointIndex = geomOffsets[i];
        parts.push_back(startPointIndex + lastStartPointIndex);
        if (
          i > 0 &&
          lines.featureIds.value[startPointIndex] !== lines.featureIds.value[startPointIndex - 1]
        ) {
          sizes.push_back(numParts);
          numParts = 0;
        }
        numParts += 1;
      }
      // add the last size
      sizes.push_back(numParts);
      // update lastStartPointIndex
      lastStartPointIndex += geomOffsets[geomOffsets.length - 1];
    }
  }

  const lineCollection = new wasm.LineCollection(xs, ys, parts, sizes, convertToUTM || false);
  return lineCollection;
}

/**
 * Creates a GeoDa PolygonCollection from binary polygon features
 * @param {Array<BinaryFeatureCollection['polygons']>} polygonsArray - Array of binary polygon features from GeoArrow chunks
 * @param {GeoDaModule} wasm - The initialized GeoDa WASM module
 * @returns {PolygonCollection} A GeoDa polygon collection
 */
export function createPolygonCollectionFromBinaryFeatures(
  polygonsArray: Array<BinaryFeatureCollection['polygons']>,
  wasm: GeoDaModule,
  fixPolygon?: boolean,
  convertToUTM?: boolean
): PolygonCollection {
  // create PolygonCollection from array of binaryFeatures
  const xs = new wasm.VectorDouble();
  const ys = new wasm.VectorDouble();
  const parts = new wasm.VectorUInt();
  const holes = new wasm.VectorUInt();
  const sizes = new wasm.VectorUInt();

  let lastPrimitiveIndex = 0;
  for (let chunkIndex = 0; chunkIndex < polygonsArray.length; chunkIndex++) {
    const polygons = polygonsArray[chunkIndex];
    if (polygons) {
      const coords = polygons.positions.value;
      const polygonIndices = polygons.polygonIndices.value;
      const primitivePolygonIndices = polygons.primitivePolygonIndices.value;

      // get xs, ys, parts, sizes
      for (let i = 0; i < coords.length; i += 2) {
        xs.push_back(coords[i]);
        ys.push_back(coords[i + 1]);
      }
      let primitiveIndex = 0;
      let numParts = 0;
      for (let i = 0; i < polygonIndices.length - 1; i++) {
        const startIdx = polygonIndices[i];
        const endIdx = polygonIndices[i + 1];
        while (primitivePolygonIndices[primitiveIndex] < endIdx) {
          // parts: start index of each part
          // holes: true if the part is a hole
          if (primitivePolygonIndices[primitiveIndex] > startIdx) {
            // holeIndices.push(primitivePolygonIndices[primitiveIndex] - startIdx);
            holes.push_back(1);
          } else {
            holes.push_back(0);
          }
          parts.push_back(primitivePolygonIndices[primitiveIndex] + lastPrimitiveIndex);
          primitiveIndex++;
          numParts += 1;
        }
        if (polygons.featureIds.value[endIdx] !== polygons.featureIds.value[endIdx - 1]) {
          sizes.push_back(numParts);
          numParts = 0;
        }
      }
      lastPrimitiveIndex += primitivePolygonIndices[primitivePolygonIndices.length - 1];
    }
  }

  const polygonCollection = new wasm.PolygonCollection(
    xs,
    ys,
    parts,
    holes,
    sizes,
    fixPolygon || false,
    convertToUTM || false
  );
  return polygonCollection;
}
