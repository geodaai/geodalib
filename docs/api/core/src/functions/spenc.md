[geodalib](../../../modules.md) / [core/src](../index.md) / spenc

# Function: spenc()

> **spenc**(`__namedParameters`): `Promise`\<`number`[][]\>

Defined in: core/src/clustering/spenc.ts:27

Spatially-Encouraged Spectral Clustering (SPENC).

SPENC is a spectral regionalization method: it computes an RBF affinity
between observations, masks it with the spatial adjacency (`W`) to get a
spatially-encoded affinity `S`, and embeds the observations in the top
eigenvectors of the normalized matrix `D^{-1/2} S D^{-1/2}` before running
k-means on the embedding. Because the k-means step is seeded, results are
deterministic for a fixed `seed`.

## Example
```ts
import { spenc } from '@geoda/core';

const result = await spenc({
  k: 2,
  data: [[1, 2, 3, 4, 5]],
  neighbors: [[1], [0, 2], [1, 3], [2, 4], [3]],
});
```

## Parameters

### \_\_namedParameters

#### data

`number`[][] \| `Float32Array`\<`ArrayBufferLike`\>[]

#### gamma?

`number` = `1.0`

#### k

`number`

#### neighbors

`number`[][]

#### nInit?

`number` = `10`

#### scaleMethod?

`string` = `'standardize'`

#### seed?

`number` = `123456789`

## Returns

`Promise`\<`number`[][]\>
