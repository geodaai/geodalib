// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { localBiJoinCount } from '../../src/sa/local-bijoincount';
import { initWASM } from '../../src/init';

describe('localBiJoinCount()', () => {
  beforeAll(async () => {
    await initWASM('./wasm/geoda-lisa.wasm');
  });

  it('should calculate bivariate local join count statistics', async () => {
    const nsa = [1, 0, 1, 1, 0, 1];
    const nsaInv = nsa.map(v => 1 - v);
    const neighbors = [[1], [0], [], [4, 5], [3, 5], [3, 4]];

    const result = await localBiJoinCount({ data: [nsa, nsaInv], neighbors, permutation: 99 });

    expect(result.isValid).toBe(true);
    expect(result.lisaValues.length).toBe(6);
  });

  it('should reject data with colocation', async () => {
    const data = [
      [1, 1, 0],
      [1, 0, 1],
    ];
    const neighbors = [[1], [0], []];

    await expect(localBiJoinCount({ data, neighbors, permutation: 99 })).rejects.toThrow(
      'no-colocation'
    );
  });
});
