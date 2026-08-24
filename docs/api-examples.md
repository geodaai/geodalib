---
outline: deep
---

# API References

## Mapping


### Basic Mapping

- [Quantile Map](/api/core/src/functions/quantileBreaks)
- [Natural Breaks (Jenks)](/api/core/src/functions/naturalBreaks)
- [Equal Intervals](/api/core/src/functions/equalIntervalBreaks)
- [Percentile](/api/core/src/functions/percentileBreaks)
- [Box Breaks (hinge = 1.5)](/api/core/src/functions/hinge15Breaks)
- [Box Breaks (hinge = 3.0)](/api/core/src/functions/hinge30Breaks)
- [Standard Deviation Map](/api/core/src/functions/standardDeviationBreaks)

### Rate Mapping

- [Excess Risk](/api/core/src/functions/excessRisk)
- [Empirical Bayes](/api/core/src/functions/empiricalBayes)
- [Spatial Rate](/api/core/src/functions/spatialRate)
- [Spatial Empirical Bayes](/api/core/src/functions/spatialEmpiricalBayes)

### Cartogram

- [Cartogram](/api/core/src/functions/cartogram)



## Data Exploration

- [Standardization](/api/core/src/functions/standardization)
- [MAD](/api/core/src/functions/mad)
- [PCA](/api/core/src/functions/pca)
- [Diff-in-Diff](/api/core/src/functions/diff-in-diff)
- [Spatial Lagged Variable](/api/core/src/functions/spatialLag)



## Spatial Weights


### Contiguity Weights

- [Queen Weights](/api/core/src/functions/queenWeights)
- [Rook Weights](/api/core/src/functions/rookWeights)

### Distance Weights

- [Min Distance Threshold](/api/core/src/functions/minDistanceThreshold)
- [K-Nearest Neighbors](/api/core/src/functions/kNearestNeighbors)
- [Distance based Weights](/api/core/src/functions/distanceBasedWeights)

### Kernel Weights

- [Kernel Weights](/api/core/src/functions/getKernelWeightsFromBinaryGeometries)


## Spatial Autocorrelation Analysis


### Global Spatial Autocorrelation

- [Moran's I](/api/lisa/src/functions/moransI)

### Local Spatial Autocorrelation

- [Local Moran's I](/api/lisa/src/functions/localMoransI)
- [Local Getis-Ord Gi*](/api/lisa/src/functions/localGetisOrdGiStar)
- [Local Geary's C](/api/lisa/src/functions/localGearysC)
- [Local Join Count](/api/lisa/src/functions/localJoinCount)
- [Quantile LISA](/api/lisa/src/functions/quantileLISA)



## Spatial Clustering


### Non-Spatial Clustering

- [PCA](/api/clustering/src/functions/pca)
- [MDS](/api/clustering/src/functions/mds)
- [t-SNE](/api/clustering/src/functions/t-SNE)
- [k-Means](/api/clustering/src/functions/kmeans)
- [k-Medians](/api/clustering/src/functions/kmedians)
- [k-Medoids](/api/clustering/src/functions/kmedoids)
- [Spectral Clustering](/api/clustering/src/functions/spectralClustering)
- [Hierarchical Clustering](/api/clustering/src/functions/hierarchicalClustering)

### Spatial Constrained Clustering

- [DBSCAN](/api/clustering/src/functions/dbscan)
- [HDBSCAN](/api/clustering/src/functions/hdbscan)
- [Spatial K-Clusters](/api/clustering/src/functions/spatialKClusters)
- [Spatial Constrained Hierarchical Clustering](/api/clustering/src/functions/schc)

### Spatial Clustering

- [SKATER](/api/clustering/src/functions/skater)
- [REDCAP](/api/clustering/src/functions/redcap)
- [AZP](/api/clustering/src/functions/azp)
- [Max-p](/api/clustering/src/functions/max-p)



## Spatial Regression

- [OLS Regression](/api/regression/src/functions/classicRegression)
- [Spatial Lag Model](/api/regression/src/functions/spatialLag)
- [Spatial Error Model](/api/regression/src/functions/spatialError)



## Spatial Operations


### Geometric Operations

- [Centroid](/api/core/src/functions/centroid)
- [Buffer](/api/core/src/functions/buffer)
- [Convex Hull](/api/core/src/functions/convexHull)
- [Spatial Dissolve](/api/core/src/functions/dissolve)
- [Spatial Join](/api/core/src/functions/spatialJoin)

### Other Operations

- [Voronoi](/api/core/src/functions/voronoi)


