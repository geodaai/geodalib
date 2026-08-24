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

- [Standardization (Z-Score)](/api/core/src/functions/standardize)
- [Deviation from Mean](/api/core/src/functions/deviationFromMean)
- [Standardization (MAD)](/api/core/src/functions/standardizeMAD)
- [Range Adjustment](/api/core/src/functions/rangeAdjust)
- [Range Standardization](/api/core/src/functions/rangeStandardize)
- [Spatial Lagged Variable](/api/core/src/functions/spatialLag)



## Spatial Weights

- [Create Weights](/api/core/src/functions/createWeights)

### Contiguity Weights

- [Queen Weights](/api/core/src/functions/queenWeights)
- [Rook Weights](/api/core/src/functions/rookWeights)

### Distance Weights

- [Min Distance Threshold](/api/core/src/functions/getDistanceThresholds)
- [K-Nearest Neighbors](/api/core/src/functions/getNearestNeighborsFromBinaryGeometries)
- [Distance based Weights](/api/core/src/functions/getDistanceNeighborsFromBinaryGeometries)

### Kernel Weights

- Kernel Weights
- Kernel K-Nearest Neighbors Weights



## Spatial Autocorrelation Analysis


### Global Spatial Autocorrelation

- Global Moran's I

### Local Spatial Autocorrelation

- [Local Moran's I](/api/lisa/src/functions/localMoran)
- [Bivariate Local Moran's I](/api/lisa/src/functions/bivariateLocalMoran)
- [Local Getis-Ord G](/api/lisa/src/functions/localG)
- [Local Getis-Ord G*](/api/lisa/src/functions/localGStar)
- [Local Geary's C](/api/lisa/src/functions/localGeary)
- [Multivariate Local Geary's C](/api/lisa/src/functions/multivariateLocalGeary)
- Local Join Count
- [Quantile LISA](/api/lisa/src/functions/quantileLisa)
- [Spatial Lag](/api/lisa/src/functions/spatialLag)
- [Spatial Lag Median](/api/lisa/src/functions/spatialLagMedian)



## Spatial Clustering


### Non-Spatial Clustering

- [PCA](/api/functions/pca)
- [MDS](/api/functions/mds)
- [t-SNE](/api/functions/t-SNE)
- [k-Means](/api/functions/kmeans)
- [k-Medians](/api/functions/kmedians)
- [k-Medoids](/api/functions/kmedoids)
- [Spectral Clustering](/api/functions/spectralClustering)
- [Hierarchical Clustering](/api/functions/hierarchicalClustering)

### Spatial Constrained Clustering

- [DBSCAN](/api/functions/dbscan)
- [HDBSCAN](/api/functions/hdbscan)
- [Spatial K-Clusters](/api/functions/spatialKClusters)
- [Spatial Constrained Hierarchical Clustering](/api/functions/schc)

### Spatial Clustering

- [SKATER](/api/functions/skater)
- [REDCAP](/api/functions/redcap)
- [AZP](/api/functions/azp)
- [Max-p](/api/functions/max-p)



## Spatial Regression

- [OLS Regression](/api/regression/src/functions/linearRegression)
- [Spatial Lag Model](/api/regression/src/functions/spatialLagRegression)
- [Spatial Error Model](/api/regression/src/functions/spatialError)



## Spatial Operations


### Geometric Operations

- [Centroid](/api/core/src/functions/getCentroids)
- [Buffer](/api/core/src/functions/getBuffers)
- [Area](/api/core/src/functions/getArea)
- [Length](/api/core/src/functions/getLength)
- [Perimeter](/api/core/src/functions/getPerimeter)
- [Spatial Dissolve](/api/core/src/functions/spatialDissolve)
- [Spatial Join](/api/core/src/functions/spatialJoin)

### Other Operations

- [Thiessen Polygons](/api/core/src/functions/thiessenPolygons)
- [Minimum Spanning Tree](/api/core/src/functions/getMinimumSpanningTree)


