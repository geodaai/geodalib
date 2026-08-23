[geodalib](../../../modules.md) / [core/src](../index.md) / schc

# Function: schc()

> **schc**(`__namedParameters`): `Promise`\<`number`[][]\>

Defined in: core/src/clustering/schc.ts:20

Spatially constrained hierarchical clustering (SCHC).

## Example
```ts
import { schc } from '@geoda/core';

const result = await schc({
  k: 3,
  data: [[1, 2, 3, 4]],
  neighbors: [[1], [0, 2], [1, 3], [2]],
});
```

## Parameters

### \_\_namedParameters

#### boundVals?

`number`[] = `[]`

optional bound values per observation

#### data

`number`[][] \| `Float32Array`\<`ArrayBufferLike`\>[]

multivariate data, one array per variable

#### distanceMethod?

`string` = `'euclidean'`

euclidean | manhattan

#### k

`number`

number of clusters

#### linkageMethod?

`string` = `'ward'`

single | complete | average | ward

#### minBound?

`number` = `0.0`

#### neighbors

`number`[][]

spatial weights matrix as adjacency list

#### scaleMethod?

`string` = `'standardize'`

raw | standardize

## Returns

`Promise`\<`number`[][]\>
