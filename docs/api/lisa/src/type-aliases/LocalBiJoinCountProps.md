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

const result = await localBiJoinCount({ data: [nsa, nsa_inv], neighbors });
console.log(result);
```

## Type declaration

### permutation?

> `optional` **permutation**: `number`
