// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { describe, it, expect } from '@jest/globals';

import { makeSpatial } from '../../src/clustering/make-spatial';

describe('makeSpatial()', () => {
  const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];

  function expectPartition(clusters: number[][], numObs: number) {
    const seen = new Set<number>();
    for (const c of clusters) {
      for (const e of c) {
        expect(e).toBeGreaterThanOrEqual(0);
        expect(e).toBeLessThan(numObs);
        expect(seen.has(e)).toBe(false); // no duplicates
        seen.add(e);
      }
    }
    for (let i = 0; i < numObs; ++i) {
      expect(seen.has(i)).toBe(true); // no missing
    }
  }

  function expectContiguous(clusters: number[][], nbrs: number[][]) {
    for (const c of clusters) {
      expect(c.length).toBeGreaterThan(0);
      const member = new Set(c);
      const visited = new Set<number>([c[0]]);
      const stack = [c[0]];
      while (stack.length > 0) {
        const cur = stack.pop()!;
        for (const nbr of nbrs[cur]) {
          if (!visited.has(nbr) && member.has(nbr)) {
            visited.add(nbr);
            stack.push(nbr);
          }
        }
      }
      expect(visited.size).toBe(c.length); // connected subgraph
    }
  }

  it('should make clusters spatially contiguous', async () => {
    const clusters = [
      [0, 1, 2],
      [3, 4],
    ];

    const result = await makeSpatial({ clusters, neighbors });

    expectPartition(result, 5);
    expectContiguous(result, neighbors);
  });

  it('should reassign a disconnected cluster', async () => {
    // cluster {0, 2} is disconnected in the chain 0-1-2-3-4
    const clusters = [
      [0, 2],
      [1, 3],
      [4],
    ];

    const result = await makeSpatial({ clusters, neighbors });

    expectPartition(result, 5);
    expectContiguous(result, neighbors);
  });
});
