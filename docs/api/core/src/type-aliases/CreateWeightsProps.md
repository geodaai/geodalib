[geodalib](../../../modules.md) / [core/src](../index.md) / CreateWeightsProps

# Type Alias: CreateWeightsProps

> **CreateWeightsProps**: `object`

Defined in: [core/src/weights/utils.ts:12](https://github.com/geodaai/geodalib/blob/a35d8c60f6a8f5c4c9dc279b0d4da4855554d6f7/js/packages/core/src/weights/utils.ts#L12)

## Type declaration

### bandwidth?

> `optional` **bandwidth**: `number`

The bandwidth for kernel weights

### distanceThreshold?

> `optional` **distanceThreshold**: `number`

### geometries

> **geometries**: [`SpatialGeometry`](SpatialGeometry.md)

The geometries to create the weights for. See [SpatialGeometry](SpatialGeometry.md) for more information.
- GeoJSON features: Feature from geojson
- Binary feature collection: BinaryFeatureCollection from loaders.gl/schema
- Point layer data: [PointLayerData](PointLayerData.md) from kepler.gl
- Arc layer data: [ArcLayerData](ArcLayerData.md) from kepler.gl
- Hexagon id layer data: [HexagonIdLayerData](HexagonIdLayerData.md) from kepler.gl

### includeLowerOrder?

> `optional` **includeLowerOrder**: `boolean`

Whether to include lower order neighbors

### isMile?

> `optional` **isMile**: `boolean`

### isQueen?

> `optional` **isQueen**: `boolean`

### isRook?

> `optional` **isRook**: `boolean`

### k?

> `optional` **k**: `number`

### kernel?

> `optional` **kernel**: `string`

The kernel function for kernel weights

### orderOfContiguity?

> `optional` **orderOfContiguity**: `number`

The order of contiguity for neighbor calculations

### power?

> `optional` **power**: `number`

The power (or exponent) applied to the distance before normalizing by the bandwidth

### precisionThreshold?

> `optional` **precisionThreshold**: `number`

The precision threshold for neighbor calculations

### useCentroids?

> `optional` **useCentroids**: `boolean`

Whether to use centroids for neighbor calculations

### useKernelDiagonals?

> `optional` **useKernelDiagonals**: `boolean`

Whether the diagonal (self) weight is kernel(1.0) instead of 1.0

### weightsType

> **weightsType**: `"knn"` \| `"threshold"` \| `"queen"` \| `"rook"` \| `"kernel"`
