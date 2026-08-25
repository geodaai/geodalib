[geodalib](../../../modules.md) / [core/src](../index.md) / pam

# Function: pam()

> **pam**(`__namedParameters`): `Promise`\<`number`[][]\>

Defined in: core/src/clustering/pam.ts:23

Partitioning Around Medoids (PAM) / k-medoids clustering.

PAM clusters observations around a set of `k` medoids (actual data points)
that minimize the total distance to the other observations. Unlike k-means
it only needs a distance measure, not a Euclidean centroid, and is robust to
outliers. The `initializer` controls the starting medoids: `"BUILD"` (the
classic farthest-first PAM construction) or `"LAB"` (linear approximate
BUILD, seeded by `seed`).

## Example
```ts
import { pam } from '@geoda/core';

const result = await pam({ k: 2, data: [[1, 2, 3, 4, 5]] });
```

## Parameters

### \_\_namedParameters

#### data

`number`[][] \| `Float32Array`\<`ArrayBufferLike`\>[]

#### distanceMethod?

`string` = `'euclidean'`

#### fasttol?

`number` = `0.01`

#### initializer?

`"BUILD"` \| `"LAB"` = `'LAB'`

#### k

`number`

#### maxiter?

`number` = `100`

#### seed?

`number` = `123456789`

## Returns

`Promise`\<`number`[][]\>
