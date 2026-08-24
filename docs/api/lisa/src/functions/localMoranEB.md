[geodalib](../../../modules.md) / [lisa/src](../index.md) / localMoranEB

# Function: localMoranEB()

> **localMoranEB**(`props`): `Promise`\<[`LocalMoranEBResult`](../type-aliases/LocalMoranEBResult.md)\>

Defined in: lisa/src/sa/local-moran-eb.ts:67

Calculates Empirical Bayes smoothed Local Moran's I statistics.

## Example
```ts
import { localMoranEB } from '@geoda/lisa';

const eventData = [3, 5, 1, 4, 2];
const baseData = [10, 20, 15, 25, 12];
const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];

const result = await localMoranEB({ eventData, baseData, neighbors });
console.log(result);
```

## Parameters

### props

[`LocalMoranEBProps`](../type-aliases/LocalMoranEBProps.md)

Configuration object

## Returns

`Promise`\<[`LocalMoranEBResult`](../type-aliases/LocalMoranEBResult.md)\>

Promise resolving to EB Local Moran statistics
