// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { describe, it, expect } from '@jest/globals';

import { schc } from '../../src/clustering/schc';

describe('schc()', () => {
  it('should run spatially constrained hierarchical clustering', async () => {
    const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];
    const result = await schc({ k: 2, data: [[1, 2, 3, 4, 5]], neighbors });

    const count = result.reduce((s, c) => s + c.length, 0);
    expect(count).toBe(5);
  });
});
