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

    // deterministic for a fixed seed (1234567890)
    const EXPECTED_LISA = [0.19301023686463248, 0.19301023686463248, 0, 0.5068767980725768, 0.46434548568689166, 0.6998879513302786];
    const EXPECTED_LAG = [-0.4393293034440481, -0.4393293034440481, 0, 0.9156425120256456, 0.9473554162403955, 0.5218620897544903];
    const EXPECTED_PVALS = [0.41, 0.41, 0, 0.06, 0.06, 0.06];
    const EXPECTED_CLUSTERS = [0, 0, 6, 0, 0, 0];
    const EXPECTED_CATEGORIES = [0, 0, 5, 0, 0, 0];
    const EXPECTED_NN = [1, 1, 0, 2, 2, 2];

    result.lisaValues.forEach((v, i) => expect(v).toBeCloseTo(EXPECTED_LISA[i], 10));
    result.lagValues.forEach((v, i) => expect(v).toBeCloseTo(EXPECTED_LAG[i], 10));
    expect(result.pValues).toEqual(EXPECTED_PVALS);
    expect(result.clusters).toEqual(EXPECTED_CLUSTERS);
    expect(result.sigCategories).toEqual(EXPECTED_CATEGORIES);
    expect(result.nn).toEqual(EXPECTED_NN);
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
