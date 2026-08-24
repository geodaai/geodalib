[geodalib](../../../modules.md) / [core/src](../index.md) / getKernelKnnWeightsFromGeomCollection

# Function: getKernelKnnWeightsFromGeomCollection()

> **getKernelKnnWeightsFromGeomCollection**(`__namedParameters`): `Promise`\<`number`[][]\>

Defined in: core/src/weights/kernel-knn-weights.ts:93

Calculates k-nearest neighbor kernel weights for a given set of geometries.

## Example
```ts
import { getKernelKnnWeightsFromGeomCollection } from '@geoda/core';

const geometries = [
  { type: 'Feature', geometry: { type: 'Point', coordinates: [0, 0] } },
  { type: 'Feature', geometry: { type: 'Point', coordinates: [1, 0] } },
  { type: 'Feature', geometry: { type: 'Point', coordinates: [0, 1] } },
];

const weights = await getKernelKnnWeightsFromGeomCollection({
  k: 2,
  kernel: 'gaussian',
  geomCollection: geometries,
});

console.log(weights);
```

## Parameters

### \_\_namedParameters

#### adaptiveBandwidth?

`boolean` = `true`

Whether to use each observation's k-th nearest neighbor distance as its bandwidth (true)
or a single global maximum distance (false).

#### geomCollection

[`GeometryCollection`](../classes/GeometryCollection.md)

The geometry collection to calculate the weights for.

#### isInverse?

`boolean` = `false`

Whether to apply inverse distance weighting before the kernel.

#### isMile?

`boolean` = `false`

The unit of distance (mile or km).

#### k

`number`

The number of nearest neighbors.

#### kernel

`string`

The kernel function (triangular, uniform, epanechnikov, quartic, gaussian).

#### power?

`number` = `1.0`

The power (or exponent) used by the inverse distance weighting (1 / distance^power). Only applied when isInverse is true.

#### useKernelDiagonals?

`boolean` = `false`

Whether the diagonal (self) weight is kernel(0.0) instead of 1.0.

## Returns

`Promise`\<`number`[][]\>

- The kernel weights as rows of interleaved [index, weight] pairs.
