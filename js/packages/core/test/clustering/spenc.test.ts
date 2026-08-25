// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { describe, it, expect } from '@jest/globals';

import { spenc } from '../../src/clustering/spenc';

describe('spenc', () => {
  const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];

  it('should cluster', async () => {
    const result = await spenc({ k: 2, data: [[1, 2, 3, 4, 5]], neighbors });
    expect(result.reduce((s, c) => s + c.length, 0)).toBe(5);
  }, 10000);

  it('should be deterministic for a fixed seed', async () => {
    const args = { k: 2, data: [[1, 2, 3, 4, 5]], neighbors };
    const r1 = await spenc(args);
    const r2 = await spenc(args);
    expect(r1).toEqual(r2);
  }, 10000);

  it('should reject k larger than the number of observations', async () => {
    await expect(spenc({ k: 6, data: [[1, 2, 3, 4, 5]], neighbors })).rejects.toThrow(
      'k must be between 1 and 5'
    );
  }, 10000);
});
