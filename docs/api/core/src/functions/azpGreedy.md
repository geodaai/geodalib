[geodalib](../../../modules.md) / [core/src](../index.md) / azpGreedy

# Function: azpGreedy()

> **azpGreedy**(`__namedParameters`): `Promise`\<`number`[][]\>

Defined in: core/src/clustering/azp.ts:16

AZP (Automatic Zoning Procedure) regionalization.

## Example
```ts
import { azpGreedy } from '@geoda/core';

const result = await azpGreedy({ p: 3, data: [[1, 2, 3, 4]], neighbors: [[1], [0, 2], [1, 3], [2]] });
```

## Parameters

### \_\_namedParameters

#### data

`number`[][] \| `Float32Array`\<`ArrayBufferLike`\>[]

#### distanceMethod?

`string` = `'euclidean'`

#### inits?

`number` = `10`

#### neighbors

`number`[][]

#### p

`number`

number of regions

#### seed?

`number` = `1234567890`

## Returns

`Promise`\<`number`[][]\>
