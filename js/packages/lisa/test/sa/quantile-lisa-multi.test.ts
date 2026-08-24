// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { multivariateQuantileLisa } from '../../src/sa/quantile-lisa-multi';
import { initWASM } from '../../src/init';
import { TEST_PO60, TEST_QUEEN_WEIGHTS } from '../data';

describe('multivariateQuantileLisa()', () => {
  beforeAll(async () => {
    await initWASM('./wasm/geoda-lisa.wasm');
  });

  it('should calculate multivariate quantile LISA statistics', async () => {
    const data = [
      [1, 2, 3, 4, 5, 6],
      [6, 5, 4, 3, 2, 1],
    ];
    const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3, 5], [4]];
    const kValues = [4, 4];
    const quantileValues = [1, 2];

    const result = await multivariateQuantileLisa({
      kValues,
      quantileValues,
      data,
      neighbors,
      permutation: 99,
    });

    expect(result.isValid).toBe(true);
    expect(result.lisaValues.length).toBe(6);
  });

  it('should calculate multivariate quantile LISA on a larger dataset', async () => {
    const data = [TEST_PO60, TEST_PO60.map(v => 100 - v)];
    const kValues = [4, 4];
    const quantileValues = [1, 2];

    const result = await multivariateQuantileLisa({
      kValues,
      quantileValues,
      data,
      neighbors: TEST_QUEEN_WEIGHTS,
      permutation: 99,
    });

    expect(result.isValid).toBe(true);
  });

  it('should reject mismatched variable lengths', async () => {
    const data = [
      [1, 2, 3, 4, 5, 6],
      [6, 5, 4, 3, 2],
    ];
    const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3, 5], [4]];
    const kValues = [4, 4];
    const quantileValues = [1, 2];

    await expect(
      multivariateQuantileLisa({ kValues, quantileValues, data, neighbors, permutation: 99 })
    ).rejects.toThrow(/data variable 1/);
  });

  it('should reject k/quantile arrays that do not match the data variables', async () => {
    const data = [
      [1, 2, 3, 4, 5, 6],
      [6, 5, 4, 3, 2, 1],
    ];
    const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3, 5], [4]];
    const kValues = [4];
    const quantileValues = [1, 2];

    await expect(
      multivariateQuantileLisa({ kValues, quantileValues, data, neighbors, permutation: 99 })
    ).rejects.toThrow(/same length as data/);
  });

  it('should reject invalid k/quantile values', async () => {
    const data = [
      [1, 2, 3, 4, 5, 6],
      [6, 5, 4, 3, 2, 1],
    ];
    const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3, 5], [4]];
    const kValues = [1, 4];
    const quantileValues = [1, 2];

    const result = await multivariateQuantileLisa({
      kValues,
      quantileValues,
      data,
      neighbors,
      permutation: 99,
    });

    expect(result.isValid).toBe(false);
  });
});
