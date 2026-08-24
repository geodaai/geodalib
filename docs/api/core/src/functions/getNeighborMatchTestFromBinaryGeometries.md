[geodalib](../../../modules.md) / [core/src](../index.md) / getNeighborMatchTestFromBinaryGeometries

# Function: getNeighborMatchTestFromBinaryGeometries()

> **getNeighborMatchTestFromBinaryGeometries**(`__namedParameters`): `Promise`\<[`NeighborMatchTestResult`](../type-aliases/NeighborMatchTestResult.md)\>

Defined in: core/src/weights/neighbor-match-test.ts:51

Computes the local neighbor match test, assessing the overlap between k-nearest
neighbors in geographic space and k-nearest neighbors in attribute space.

## Parameters

### \_\_namedParameters

[`NeighborMatchTestProps`](../type-aliases/NeighborMatchTestProps.md)

## Returns

`Promise`\<[`NeighborMatchTestResult`](../type-aliases/NeighborMatchTestResult.md)\>

the cardinality and probability arrays
