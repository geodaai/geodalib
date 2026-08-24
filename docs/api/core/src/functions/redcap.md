[geodalib](../../../modules.md) / [core/src](../index.md) / redcap

# Function: redcap()

> **redcap**(`__namedParameters`): `Promise`\<`number`[][]\>

Defined in: core/src/clustering/redcap.ts:20

Regionally constrained clustering (REDCAP).

## Example
```ts
import { redcap } from '@geoda/core';

const result = await redcap({
  k: 3,
  data: [[1, 2, 3, 4]],
  neighbors: [[1], [0, 2], [1, 3], [2]],
});
```

## Parameters

### \_\_namedParameters

#### boundVals?

`number`[] = `[]`

#### data

`number`[][] \| `Float32Array`\<`ArrayBufferLike`\>[]

#### distanceMethod?

`string` = `'euclidean'`

#### k

`number`

#### minBound?

`number` = `0.0`

#### neighbors

`number`[][]

#### redcapMethod?

`string` = `'firstorder-singlelinkage'`

#### scaleMethod?

`string` = `'standardize'`

## Returns

`Promise`\<`number`[][]\>
