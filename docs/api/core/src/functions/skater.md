[geodalib](../../../modules.md) / [core/src](../index.md) / skater

# Function: skater()

> **skater**(`__namedParameters`): `Promise`\<`number`[][]\>

Defined in: core/src/clustering/skater.ts:20

Spatially constrained clustering and regionalization (SKATER).

## Example
```ts
import { skater } from '@geoda/core';

const result = await skater({
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

#### scaleMethod?

`string` = `'standardize'`

## Returns

`Promise`\<`number`[][]\>
