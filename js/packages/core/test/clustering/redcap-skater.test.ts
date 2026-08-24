// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { describe, it, expect } from '@jest/globals';

import { redcap } from '../../src/clustering/redcap';
import { skater } from '../../src/clustering/skater';

describe('clustering', () => {
  const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];

  it('redcap should cluster', async () => {
    const result = await redcap({ k: 2, data: [[1, 2, 3, 4, 5]], neighbors });
    expect(result.reduce((s, c) => s + c.length, 0)).toBe(5);
  });

  it('skater should cluster', async () => {
    const result = await skater({ k: 2, data: [[1, 2, 3, 4, 5]], neighbors });
    expect(result.reduce((s, c) => s + c.length, 0)).toBe(5);
  });

  it('should reject a variable whose length does not match the observations', async () => {
    await expect(
      redcap({ k: 2, data: [[1, 2, 3, 4]], neighbors })
    ).rejects.toThrow('must have 5 values');
    await expect(
      skater({ k: 2, data: [[1, 2, 3, 4]], neighbors })
    ).rejects.toThrow('must have 5 values');
  });
});
