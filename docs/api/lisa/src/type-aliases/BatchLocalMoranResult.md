[geodalib](../../../modules.md) / [lisa/src](../index.md) / BatchLocalMoranResult

# Type Alias: BatchLocalMoranResult

> **BatchLocalMoranResult**: `object`

Defined in: lisa/src/sa/batch-local-moran.ts:10

Result of a batch Local Moran computation (per-variable arrays).

## Type declaration

### clusters

> **clusters**: `number`[][]

per-variable cluster assignments

### colors

> **colors**: `string`[]

### isValid

> **isValid**: `boolean`

### labels

> **labels**: `string`[]

### lagValues

> **lagValues**: `number`[][]

per-variable spatial lag values

### lisaValues

> **lisaValues**: `number`[][]

per-variable LISA statistic values

### nn

> **nn**: `number`[]

number of neighbors for each observation

### pValues

> **pValues**: `number`[][]

per-variable pseudo-p values
