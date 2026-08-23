[geodalib](../../../modules.md) / [core/src](../index.md) / azpSA

# Function: azpSA()

> **azpSA**(`__namedParameters`): `Promise`\<`number`[][]\>

Defined in: [core/src/clustering/azp.ts:61](https://github.com/geodaai/geodalib/blob/f811b3963642b62e0a1f5367d019edd753f146d9/js/packages/core/src/clustering/azp.ts#L61)

AZP regionalization with simulated annealing.

## Parameters

### \_\_namedParameters

#### coolingRate?

`number` = `0.85`

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

#### saMaxit?

`number` = `1`

#### seed?

`number` = `1234567890`

## Returns

`Promise`\<`number`[][]\>
