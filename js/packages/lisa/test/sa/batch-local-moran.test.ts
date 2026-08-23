// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { batchLocalMoran } from '../../src/sa/batch-local-moran';
import { initWASM } from '../../src/init';
import { TEST_PO60, TEST_QUEEN_WEIGHTS } from '../data';

describe('batchLocalMoran()', () => {
  beforeAll(async () => {
    await initWASM('./wasm/geoda-lisa.wasm');
  });

  it('should calculate batch local moran statistics', async () => {
    const data = [
      [1.0, 2.0, 3.0, 4.0, 5.0, 6.0],
      [6.0, 5.0, 4.0, 3.0, 2.0, 1.0],
    ];
    const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3, 5], [4]];

    const result = await batchLocalMoran({ data, neighbors, permutation: 99 });

    expect(result.isValid).toBe(true);
    expect(result.lisaValues.length).toBe(2);
    expect(result.lisaValues[0].length).toBe(6);
    expect(result.pValues.length).toBe(2);
    expect(result.nn.length).toBe(6);
  });

  it('should calculate batch local moran on a larger dataset', async () => {
    const data = [TEST_PO60, TEST_PO60.map(v => 100 - v)];
    const result = await batchLocalMoran({ data, neighbors: TEST_QUEEN_WEIGHTS, permutation: 99 });
    expect(result.isValid).toBe(true);
  });
});
