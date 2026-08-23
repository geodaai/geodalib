[geodalib](../../../modules.md) / [lisa/src](../index.md) / MultivariateQuantileLisaProps

# Type Alias: MultivariateQuantileLisaProps

> **MultivariateQuantileLisaProps**: `object`

Defined in: lisa/src/sa/quantile-lisa-multi.ts:25

Configuration properties for Multivariate Local Quantile LISA.

## Type declaration

### data

> **data**: `number`[][] \| `Float32Array`[]

multiple data variables

### kValues

> **kValues**: `number`[]

number of quantile breaks per variable

### neighbors

> **neighbors**: `number`[][]

spatial weights matrix as adjacency list

### permutation

> **permutation**: `number`

### quantileValues

> **quantileValues**: `number`[]

quantile class (1-based) per variable

### seed?

> `optional` **seed**: `number`

### significanceCutoff?

> `optional` **significanceCutoff**: `number`
