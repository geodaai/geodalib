// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { localJoinCount, multivariateLocalJoinCount } from '../../src/sa/local-joincount';
import { initWASM } from '../../src/init';
import { TEST_PO60, TEST_QUEEN_WEIGHTS } from '../data';

describe('localJoinCount()', () => {
  beforeAll(async () => {
    await initWASM('./wasm/geoda-lisa.wasm');
  });

  it('should calculate local join count statistics correctly', async () => {
    // binary (0/1) data
    const data = [1, 0, 1, 1, 0, 1];
    const neighbors = [[1], [0], [], [4, 5], [3, 5], [3, 4]];
    const permutation = 99;

    const result = await localJoinCount({ data, neighbors, permutation });

    expect(result.isValid).toBe(true);
    expect(result.lisaValues.length).toBe(data.length);
    expect(result.pValues.length).toBe(data.length);
    expect(result.nn.length).toBe(data.length);
  });

  it('should calculate local join count on a larger dataset', async () => {
    const data = TEST_PO60.map(v => (v > 5 ? 1 : 0));
    const neighbors = TEST_QUEEN_WEIGHTS;
    const permutation = 99;

    const result = await localJoinCount({ data, neighbors, permutation });

    expect(result.isValid).toBe(true);
    expect(result.lisaValues.length).toBe(data.length);
  });
});

describe('multivariateLocalJoinCount()', () => {
  beforeAll(async () => {
    await initWASM('./wasm/geoda-lisa.wasm');
  });

  it('should calculate multivariate local join count statistics', async () => {
    const data = [
      [1, 0, 1, 1, 0, 1],
      [0, 1, 0, 1, 1, 0],
    ];
    const neighbors = [[1], [0], [], [4, 5], [3, 5], [3, 4]];
    const permutation = 99;

    const result = await multivariateLocalJoinCount({ data, neighbors, permutation });

    expect(result.isValid).toBe(true);
    expect(result.lisaValues.length).toBe(6);
  });
});
