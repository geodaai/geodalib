[geodalib](../../../modules.md) / [core/src](../index.md) / maxpGreedy

# Function: maxpGreedy()

> **maxpGreedy**(`__namedParameters`): `Promise`\<`number`[][]\>

Defined in: core/src/clustering/maxp.ts:16

Max-P regionalization.

## Example
```ts
import { maxpGreedy } from '@geoda/core';

const result = await maxpGreedy({ data: [[1, 2, 3, 4]], neighbors: [[1], [0, 2], [1, 3], [2]] });
```

## Parameters

### \_\_namedParameters

#### data

`number`[][] \| `Float32Array`\<`ArrayBufferLike`\>[]

#### distanceMethod?

`string` = `'euclidean'`

#### iterations?

`number` = `10`

#### neighbors

`number`[][]

#### seed?

`number` = `1234567890`

## Returns

`Promise`\<`number`[][]\>
