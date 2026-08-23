// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { describe, it, expect } from '@jest/globals';

import { azpGreedy, azpSA, azpTabu } from '../../src/clustering/azp';
import { maxpGreedy, maxpSA, maxpTabu } from '../../src/clustering/maxp';

describe('azp/maxp', () => {
  const neighbors = [[1], [0, 2], [1, 3], [2, 4], [3]];

  it('azpGreedy should cluster', async () => {
    const result = await azpGreedy({ p: 2, data: [[1, 2, 3, 4, 5]], neighbors });
    expect(result.reduce((s, c) => s + c.length, 0)).toBe(5);
  }, 10000);

  it('azpSA should cluster', async () => {
    const result = await azpSA({ p: 2, data: [[1, 2, 3, 4, 5]], neighbors });
    expect(result.reduce((s, c) => s + c.length, 0)).toBe(5);
  }, 10000);

  it('azpTabu should cluster', async () => {
    const result = await azpTabu({ p: 2, data: [[1, 2, 3, 4, 5]], neighbors });
    expect(result.reduce((s, c) => s + c.length, 0)).toBe(5);
  }, 10000);

  it('maxpGreedy should cluster', async () => {
    const result = await maxpGreedy({ data: [[1, 2, 3, 4, 5]], neighbors });
    expect(result.reduce((s, c) => s + c.length, 0)).toBe(5);
  }, 10000);

  it('maxpSA should cluster', async () => {
    const result = await maxpSA({ data: [[1, 2, 3, 4, 5]], neighbors });
    expect(result.reduce((s, c) => s + c.length, 0)).toBe(5);
  }, 10000);

  it('maxpTabu should cluster', async () => {
    const result = await maxpTabu({ data: [[1, 2, 3, 4, 5]], neighbors });
    expect(result.reduce((s, c) => s + c.length, 0)).toBe(5);
  }, 10000);
});
