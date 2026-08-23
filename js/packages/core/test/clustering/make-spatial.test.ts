// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { describe, it, expect } from '@jest/globals';

import { makeSpatial } from '../../src/clustering/make-spatial';

describe('makeSpatial()', () => {
  it('should make clusters spatially contiguous', async () => {
    const clusters = [
      [0, 1, 2],
      [3, 4],
    ];
    const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];

    const result = await makeSpatial({ clusters, neighbors });

    const count = result.reduce((s, c) => s + c.length, 0);
    expect(count).toBe(5);
  });
});
