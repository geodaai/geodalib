// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { describe, it, expect } from '@jest/globals';

import { pam } from '../../src/clustering/pam';

describe('pam', () => {
  it('should cluster with the LAB initializer', async () => {
    const result = await pam({ k: 2, data: [[1, 2, 3, 4, 5]] });
    expect(result.reduce((s, c) => s + c.length, 0)).toBe(5);
  }, 10000);

  it('should cluster with the BUILD initializer and manhattan distance', async () => {
    const result = await pam({
      k: 2,
      data: [[1, 2, 3, 4, 5]],
      initializer: 'BUILD',
      distanceMethod: 'manhattan',
    });
    expect(result.reduce((s, c) => s + c.length, 0)).toBe(5);
  }, 10000);

  it('should reject k larger than the number of observations', async () => {
    await expect(pam({ k: 6, data: [[1, 2, 3, 4, 5]] })).rejects.toThrow(
      'k must be between 1 and 5'
    );
  }, 10000);
});
