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

    // deterministic local join counts: obs 3 (nbrs 4,5 with data[5]=1) and obs 5
    // (nbrs 3,4 with data[3]=1) each join one 1-valued neighbor.
    expect(result.lisaValues).toEqual([0, 0, 0, 1, 0, 1]);
    // Pseudo-p values are seed-dependent draws from the lookup-table permutation
    // sampler (fixed perm=99), not analytic probabilities; these exact values are
    // validated by the test. obs 0/1/4 have lisa=0 (p-value -1); obs 2 is
    // isolated (p-value 0).
    expect(result.pValues).toEqual([-1, -1, 0, 0.42, -1, 0.33]);
    // obs 2 is isolated -> undefined category; all clusters not significant
    expect(result.sigCategories).toEqual([0, 0, 6, 0, 0, 0]);
    expect(result.clusters).toEqual([0, 0, 0, 0, 0, 0]);
    expect(result.nn).toEqual([1, 1, 0, 2, 2, 2]);
  });

  it('should calculate local join count on a larger dataset', async () => {
    const data = TEST_PO60.map(v => (v > 5 ? 1 : 0));
    const neighbors = TEST_QUEEN_WEIGHTS;
    const permutation = 99;

    const result = await localJoinCount({ data, neighbors, permutation });

    expect(result.isValid).toBe(true);
    expect(result.lisaValues.length).toBe(data.length);
  });

  it('should reject data length not matching the neighbor list', async () => {
    // The C++ wrapper treats the neighbor list as the population; data is passed
    // through at its actual length so the mismatch is caught instead of silently
    // padded.
    const data = [1, 0, 1, 1, 0, 1];
    const neighbors = [[1], [0], [], [4, 5], [3, 5]]; // only 5 rows

    const result = await localJoinCount({ data, neighbors });

    expect(result.isValid).toBe(false);
  });

  it('should reject non-binary data', async () => {
    // UniJoinCount treats any positive value as 1-valued, so a value like 2 would
    // silently skew the counts; the JS wrapper throws instead.
    const data = [1, 2, 1, 1, 0, 1];
    const neighbors = [[1], [0], [], [4, 5], [3, 5], [3, 4]];

    await expect(localJoinCount({ data, neighbors })).rejects.toThrow(
      'localJoinCount: data must be binary (0/1)'
    );
  });
});

describe('multivariateLocalJoinCount()', () => {
  beforeAll(async () => {
    await initWASM('./wasm/geoda-lisa.wasm');
  });

  it('should calculate multivariate local join count statistics', async () => {
    // two identical binary variables: every location has a colocation where both
    // variables are 1, so obs 0, 1, 3, 5 get a non-zero local join count.
    const data = [
      [1, 1, 0, 1, 0, 1],
      [1, 1, 0, 1, 0, 1],
    ];
    const neighbors = [[1], [0], [], [4, 5], [3, 5], [3, 4]];
    const permutation = 99;

    const result = await multivariateLocalJoinCount({ data, neighbors, permutation });

    expect(result.isValid).toBe(true);

    // deterministic multivariate local join counts
    expect(result.lisaValues).toEqual([1, 1, 0, 1, 0, 1]);
    // obs 2/4 have lisa=0 (p-value -1)
    expect(result.pValues).toEqual([0.38, 0.38, -1, 0.12, -1, 0.13]);
    expect(result.sigCategories).toEqual([0, 0, 0, 0, 0, 0]);
    expect(result.clusters).toEqual([0, 0, 0, 0, 0, 0]);
    expect(result.nn).toEqual([1, 1, 0, 2, 2, 2]);
  });

  it('should reject a single variable (not multivariate)', async () => {
    const data = [[1, 1, 0, 1, 0, 1]];
    const neighbors = [[1], [0], [], [4, 5], [3, 5], [3, 4]];

    const result = await multivariateLocalJoinCount({ data, neighbors });

    expect(result.isValid).toBe(false);
  });

  it('should reject a variable shorter than the neighbor list', async () => {
    // The first variable has 5 entries while the neighbor list has 6; the row is
    // passed through at its actual length so the C++ wrapper rejects it instead
    // of padding with NaN.
    const data = [
      [1, 1, 0, 1, 0, 1],
      [1, 1, 0, 1, 0],
    ];
    const neighbors = [[1], [0], [], [4, 5], [3, 5], [3, 4]];

    const result = await multivariateLocalJoinCount({ data, neighbors });

    expect(result.isValid).toBe(false);
  });

  it('should reject non-binary data in any variable', async () => {
    // MultiJoinCount multiplies raw values into its integer zz, so a value like 2
    // would produce an invalid colocation count; the JS wrapper throws instead.
    const data = [
      [1, 1, 0, 1, 0, 1],
      [1, 2, 0, 1, 0, 1],
    ];
    const neighbors = [[1], [0], [], [4, 5], [3, 5], [3, 4]];

    await expect(multivariateLocalJoinCount({ data, neighbors })).rejects.toThrow(
      'multivariateLocalJoinCount: data must be binary (0/1)'
    );
  });
});
