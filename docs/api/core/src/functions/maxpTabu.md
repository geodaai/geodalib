[geodalib](../../../modules.md) / [core/src](../index.md) / maxpTabu

# Function: maxpTabu()

> **maxpTabu**(`__namedParameters`): `Promise`\<`number`[][]\>

Defined in: [core/src/clustering/maxp.ts:104](https://github.com/geodaai/geodalib/blob/f811b3963642b62e0a1f5367d019edd753f146d9/js/packages/core/src/clustering/maxp.ts#L104)

Max-P regionalization with tabu search.

## Parameters

### \_\_namedParameters

#### convTabu?

`number` = `10`

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

#### tabuLength?

`number` = `10`

## Returns

`Promise`\<`number`[][]\>
