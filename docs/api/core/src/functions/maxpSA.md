[geodalib](../../../modules.md) / [core/src](../index.md) / maxpSA

# Function: maxpSA()

> **maxpSA**(`__namedParameters`): `Promise`\<`number`[][]\>

Defined in: [core/src/clustering/maxp.ts:58](https://github.com/geodaai/geodalib/blob/f811b3963642b62e0a1f5367d019edd753f146d9/js/packages/core/src/clustering/maxp.ts#L58)

Max-P regionalization with simulated annealing.

## Parameters

### \_\_namedParameters

#### coolingRate?

`number` = `0.85`

#### data

`number`[][] \| `Float32Array`\<`ArrayBufferLike`\>[]

#### distanceMethod?

`string` = `'euclidean'`

#### iterations?

`number` = `10`

#### neighbors

`number`[][]

#### saMaxit?

`number` = `1`

#### seed?

`number` = `1234567890`

## Returns

`Promise`\<`number`[][]\>
