[geodalib](../../../modules.md) / [core/src](../index.md) / spatialValidation

# Function: spatialValidation()

> **spatialValidation**(`__namedParameters`): `Promise`\<[`SpatialValidationResult`](../type-aliases/SpatialValidationResult.md)\>

Defined in: core/src/clustering/spatial-validation.ts:38

Computes spatial validation metrics for a clustering result.

## Parameters

### \_\_namedParameters

#### clusters

`number`[]

cluster id per observation

#### geometries

[`GeometryCollection`](../classes/GeometryCollection.md)

geometry collection (use @geoda/core to build it)

#### neighbors

`number`[][]

spatial weights matrix as adjacency list

## Returns

`Promise`\<[`SpatialValidationResult`](../type-aliases/SpatialValidationResult.md)\>
