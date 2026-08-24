[geodalib](../../../modules.md) / [core/src](../index.md) / getKernelKnnWeightsFromBinaryGeometries

# Function: getKernelKnnWeightsFromBinaryGeometries()

> **getKernelKnnWeightsFromBinaryGeometries**(`input`): `Promise`\<`number`[][]\>

Defined in: core/src/weights/kernel-knn-weights.ts:35

Calculates k-nearest neighbor kernel weights for a given set of geometries or
latitude/longitude arrays. Each row is an interleaved list of [neighborIndex, weight]
pairs, with the diagonal (self) element appended last.

## Parameters

### input

`KernelKnnWeightsFromBinaryGeometriesProps`

The input parameters.

## Returns

`Promise`\<`number`[][]\>

- The kernel weights as rows of interleaved [index, weight] pairs.
