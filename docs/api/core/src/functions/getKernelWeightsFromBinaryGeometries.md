[geodalib](../../../modules.md) / [core/src](../index.md) / getKernelWeightsFromBinaryGeometries

# Function: getKernelWeightsFromBinaryGeometries()

> **getKernelWeightsFromBinaryGeometries**(`input`): `Promise`\<`number`[][]\>

Defined in: [core/src/weights/kernel-weights.ts:33](https://github.com/geodaai/geodalib/blob/a35d8c60f6a8f5c4c9dc279b0d4da4855554d6f7/js/packages/core/src/weights/kernel-weights.ts#L33)

Calculates the kernel weights within a fixed bandwidth for a given set of geometries or
latitude/longitude arrays. Each row is an interleaved list of [neighborIndex, weight] pairs,
with the diagonal (self) element appended last.

## Parameters

### input

`KernelWeightsFromBinaryGeometriesProps`

The input parameters.

## Returns

`Promise`\<`number`[][]\>

- The kernel weights as rows of interleaved [index, weight] pairs.
