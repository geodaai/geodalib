[geodalib](../../../modules.md) / [core/src](../index.md) / makeSpatial

# Function: makeSpatial()

> **makeSpatial**(`__namedParameters`): `Promise`\<`number`[][]\>

Defined in: core/src/clustering/make-spatial.ts:19

Makes a set of clusters spatially contiguous by reassigning disconnected
components, mirroring pygeoda's `make_spatial`.

## Example
```ts
import { makeSpatial } from '@geoda/core';

const clusters = [[0, 1, 2], [3, 4]];
const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];
const result = await makeSpatial({ clusters, neighbors });
```

## Parameters

### \_\_namedParameters

#### clusters

`number`[][]

list of clusters, each a list of observation indices

#### neighbors

`number`[][]

spatial weights matrix as adjacency list

## Returns

`Promise`\<`number`[][]\>
