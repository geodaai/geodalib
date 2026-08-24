// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { multivariateLocalJoinCount, MultivariateLocalJoinCountProps } from './local-joincount';

/**
 * Calculates (no-colocation) bivariate Local Join Count statistics for two binary variables.
 *
 * Mirrors pygeoda's `local_bijoincount`: the two binary variables must have no colocation
 * (an observation cannot be 1 in both), and the underlying computation is a multivariate
 * local join count.
 *
 * ## Example
 * ```ts
 * import { localBiJoinCount } from '@geoda/lisa';
 *
 * const nsa = [1, 0, 1, 1, 0, 1];
 * const nsaInv = nsa.map(v => 1 - v);
 * const neighbors = [[1], [0], [], [4, 5], [3, 5], [3, 4]];
 *
 * const result = await localBiJoinCount({ data: [nsa, nsaInv], neighbors });
 * console.log(result);
 * ```
 */
export type LocalBiJoinCountProps = Omit<MultivariateLocalJoinCountProps, 'permutation'> & {
  permutation?: number;
};

/**
 * Calculates (no-colocation) bivariate local join count statistics.
 */
export async function localBiJoinCount({
  data,
  neighbors,
  permutation = 999,
  significanceCutoff = 0.05,
  seed = 1234567890,
}: LocalBiJoinCountProps) {
  if (data.length !== 2) {
    throw new Error('The bivariate local join count only applies on two variables.');
  }

  const n = neighbors.length;
  const a = Array.from(data[0]);
  const b = Array.from(data[1]);
  if (a.length !== n || b.length !== n) {
    throw new Error('The size of data does not match the number of observations.');
  }

  // Validate that both variables are binary.
  for (const d of [a, b]) {
    for (const v of d) {
      if (v !== 0 && v !== 1) {
        throw new Error('The input data is not binary.');
      }
    }
  }

  // No-colocation constraint.
  for (let i = 0; i < n; ++i) {
    if (a[i] === 1 && b[i] === 1) {
      throw new Error(
        'The bivariate local join count only applies on two variables with no-colocation.'
      );
    }
  }

  return multivariateLocalJoinCount({
    data: [a, b],
    neighbors,
    permutation,
    significanceCutoff,
    seed,
  });
}
