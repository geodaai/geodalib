// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { localMoranEB } from '../../src/sa/local-moran-eb';
import { initWASM } from '../../src/init';
import { TEST_PO60, TEST_QUEEN_WEIGHTS } from '../data';

describe('localMoranEB()', () => {
  beforeAll(async () => {
    await initWASM('./wasm/geoda-lisa.wasm');
  });

  it('should calculate EB smoothed local Moran statistics correctly', async () => {
    const eventData = [3.0, 3.0, 0.0, 9.0, 8.0, 8.5];
    const baseData = [10.0, 10.0, 5.0, 20.0, 18.0, 15.0];
    const neighbors = [[1], [0], [], [4, 5], [3, 5], [3, 4]];
    const permutation = 99;

    const result = await localMoranEB({ eventData, baseData, neighbors, permutation });

    expect(result.isValid).toBe(true);
    expect(result.lisaValues.length).toBe(eventData.length);
    expect(result.pValues.length).toBe(eventData.length);
  });

  it('should calculate EB smoothed local Moran on a larger dataset', async () => {
    const eventData = TEST_PO60;
    const baseData = TEST_PO60.map(v => v + 10);
    const neighbors = TEST_QUEEN_WEIGHTS;
    const permutation = 99;

    const result = await localMoranEB({ eventData, baseData, neighbors, permutation });

    expect(result.isValid).toBe(true);
  });
});
