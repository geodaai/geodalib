[geodalib](../../../modules.md) / [lisa/src](../index.md) / LocalBiJoinCountProps

# Type Alias: LocalBiJoinCountProps

> **LocalBiJoinCountProps**: `Omit`\<[`MultivariateLocalJoinCountProps`](MultivariateLocalJoinCountProps.md), `"permutation"`\> & `object`

Defined in: lisa/src/sa/local-bijoincount.ts:21

Calculates (no-colocation) bivariate Local Join Count statistics for two binary variables.

Mirrors pygeoda's `local_bijoincount`: the two binary variables must have no colocation
(an observation cannot be 1 in both), and the underlying computation is a multivariate
local join count.

## Example
```ts
import { localBiJoinCount } from '@geoda/lisa';

const nsa = [1, 0, 1, 1, 0, 1];
const nsaInv = nsa.map(v => 1 - v);
const neighbors = [[1], [0], [], [4, 5], [3, 5], [3, 4]];

const result = await localBiJoinCount({ data: [nsa, nsaInv], neighbors });
console.log(result);
```

## Type declaration

### permutation?

> `optional` **permutation**: `number`
