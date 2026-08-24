// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

/**
 * The meta data for the weights
 */
export type WeightsMeta = {
  /**
   * The id of the weights
   */
  id?: string;
  /**
   * The type of the weights
   */
  type?: 'knn' | 'threshold' | 'queen' | 'rook' | 'kernel';
  /**
   * The symmetry of the weights
   */
  symmetry?: 'symmetric' | 'asymmetric';
  /**
   * The number of observations
   */
  numberOfObservations: number;
  /**
   * The minimum number of neighbors
   */
  minNeighbors: number;
  /**
   * The maximum number of neighbors
   */
  maxNeighbors: number;
  /**
   * The mean number of neighbors
   */
  meanNeighbors: number;
  /**
   * The median number of neighbors
   */
  medianNeighbors: number;
  /**
   * The percentage of non-zero neighbors
   */
  pctNoneZero: number;
  /**
   * The order of the weights
   */
  order?: number;
  /**
   * Whether to include lower order neighbors
   */
  includeLowerOrder?: boolean;
  /**
   * The k value for k-nearest neighbors
   */
  k?: number;
  /**
   * The threshold for the weights
   */
  threshold?: number;
  /**
   * The bandwidth for kernel weights
   */
  bandwidth?: number;
  /**
   * The kernel function for kernel weights
   */
  kernel?: string;
  /**
   * The power (or exponent) applied to the distance before normalizing by the bandwidth
   */
  power?: number;
  /**
   * Whether the distance is in miles
   */
  isMile?: boolean;
  /**
   * The distance metric for the weights
   */
  distanceMetric?: 'euclidean' | 'manhattan' | 'arc' | 'projected';
  /**
   * The unit of the distance metric
   */
  distanceUnit?:
    | 'Foot_US'
    | 'Yard_US'
    | 'Meter'
    | 'Kilometer'
    | 'NauticalMile'
    | 'Degree'
    | 'Radian';
};

/**
 * Get the meta data from the weights structure
 * @param weights the weights structure of every observation using row index
 * @returns WeightsMeta
 */
export function getMetaFromWeights(weights: number[][], isDistanceWeights = false): WeightsMeta {
  const n = weights.length;

  let minNeighbors = Infinity;
  let maxNeighbors = 0;
  let meanNeighbors = 0;
  let medianNeighbors = 0;
  let sumofNeighbors = 0;
  let pctNoneZero = 0;

  if (isDistanceWeights) {
    for (let i = 0; i < weights.length; i++) {
      const len = weights[i].length / 2;
      if (len < minNeighbors) minNeighbors = len;
      if (len > maxNeighbors) maxNeighbors = len;
      sumofNeighbors += len;
    }
    meanNeighbors = sumofNeighbors / n;
    pctNoneZero = sumofNeighbors / (n * n);
    medianNeighbors = weights.map(w => w.length / 2).sort((a, b) => a - b)[Math.floor(n / 2)];
  } else {
    for (let i = 0; i < weights.length; i++) {
      const len = weights[i].length;
      if (len < minNeighbors) minNeighbors = len;
      if (len > maxNeighbors) maxNeighbors = len;
      sumofNeighbors += len;
    }

    meanNeighbors = sumofNeighbors / n;
    pctNoneZero = sumofNeighbors / (n * n);
    medianNeighbors = weights.map(w => w.length).sort((a, b) => a - b)[Math.floor(n / 2)];
  }

  return {
    numberOfObservations: n,
    minNeighbors,
    maxNeighbors,
    meanNeighbors,
    medianNeighbors,
    pctNoneZero,
  };
}
