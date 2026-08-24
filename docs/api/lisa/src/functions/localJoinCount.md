[geodalib](../../../modules.md) / [lisa/src](../index.md) / localJoinCount

# Function: localJoinCount()

> **localJoinCount**(`props`): `Promise`\<[`LocalJoinCountResult`](../type-aliases/LocalJoinCountResult.md)\>

Defined in: lisa/src/sa/local-joincount.ts:64

Calculates Local Join Count statistics for binary spatial data.

## Example
```ts
import { localJoinCount } from '@geoda/lisa';

const data = [1, 0, 1, 0, 1];
const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];

const result = await localJoinCount({ data, neighbors });
console.log(result);
```

## Parameters

### props

[`LocalJoinCountProps`](../type-aliases/LocalJoinCountProps.md)

Configuration object for Local Join Count

## Returns

`Promise`\<[`LocalJoinCountResult`](../type-aliases/LocalJoinCountResult.md)\>

Promise resolving to Local Join Count statistics
