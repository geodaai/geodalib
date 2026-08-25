---
name: geoda-analysis
description: Run statistical spatial analysis (spatial weights, LISA, global Moran, colocation join counts, spatial regression, classification, rates, standardization, thiessen polygons, MST, cartogram) directly with @geoda/* in a node environment. Harness-agnostic: works in Claude Code, any agent that can run node, or plain scripts.
when_to_use: Use when the user wants spatial statistical analysis on geographic data — spatial clusters / hot spots / outliers (LISA, global Moran), colocation of binary variables (join counts), spatial regression, data classification, rates, standardization, or geometry outputs (thiessen, MST, cartogram) — without a GUI or a kepler-assistant host.
allowed-tools: Bash(${CLAUDE_SKILL_DIR}/scripts/analyze.mjs *)
---

# GeoDa spatial analysis (standalone node)

Run GeoDa statistical spatial analysis directly against the `@geoda/core`,
`@geoda/lisa`, and `@geoda/regression` WASM packages, with no GUI and no
map/table host. All compute goes through one bundled driver:
`node ${CLAUDE_SKILL_DIR}/scripts/analyze.mjs <op> [options]`.

## Prerequisites

- **Node.js >= 18** with network access for the first WASM fetch (in Node the
  packages load the committed `.wasm` from their own `node_modules`).
- The packages must be resolvable from the script. Either install them next to
  the skill, or point at an existing install:

  ```bash
  cd ${CLAUDE_SKILL_DIR}
  npm i @geoda/core @geoda/lisa @geoda/regression
  # or, if they already exist elsewhere:
  export GEODA_PKG_DIR=/path/to/a/dir/with/node_modules
  ```

- Start with `node ${CLAUDE_SKILL_DIR}/scripts/analyze.mjs --help` to see the
  exact flags; the reference below is the source of truth for semantics.

## Data format

The driver reads a **GeoJSON FeatureCollection** file (`--file data.geojson`).
Variables are read from each feature's `properties`; geometry is used to build
spatial weights. Example:

```json
{ "type": "FeatureCollection", "features": [
  { "type": "Feature", "properties": { "income": 42, "crime": 1 },
    "geometry": { "type": "Polygon", "coordinates": [[...]] } },
  ...
]}
```

Weights are an **adjacency list** (`number[][]` — neighbor index per
observation). Build them first with the `weights` op, reuse the saved JSON via
`--weights file.json` on later ops, exactly as GeoDa reuses a weights object.

## Operations

### 1. Spatial weights

```bash
node analyze.mjs weights --file data.geojson \
  --weights-type queen            # queen | rook | knn | threshold
  [--order 2 --include-lower]     # higher-order contiguity (queen/rook)
  [--k 4]                         # knn
  [--threshold 1000 --is-mile]    # threshold (distance)
  [--precision-threshold 0.0] [--use-centroids]
```

Prints `weights` (neighbor list) + `weightsMeta` (min/max/mean neighbors, etc.).
Save the JSON with `--out weights.json` and pass it to later ops with
`--weights weights.json`. **Create weights FIRST** — every other statistical op
needs them.

### 2. LISA — local spatial autocorrelation

```bash
node analyze.mjs lisa --file data.geojson --variable income \
  --method localMoran            # localMoran | localGeary | localG | localGStar | quantileLisa
  [--weights weights.json | --weights-type queen]
  [--permutation 999] [--cutoff 0.05]
  [--k 4 --quantile 4]           # quantileLisa only
```

Output: `lisaValues`, `pValues`, `clusters`, `sigCategories`, `labels`, `colors`.
- `localMoran` / `localGeary` → cluster labels like "High-High", "Low-Low",
  "High-Low", "Low-High", "Not significant".
- `localG` / `localGStar` → hot spots / cold spots.
- `quantileLisa` → spatial autocorrelation of a quantile-transformed variable.

### 3. Global Moran's I

```bash
node analyze.mjs global-moran --file data.geojson --variable income \
  [--weights weights.json | --weights-type queen]
```

Output: `globalMoranI` (positive → clustering, negative → dispersion).

### 4. Colocation — Local Join Count (binary variables)

```bash
# univariate: where does a single binary (0/1) variable cluster?
node analyze.mjs colocation --file data.geojson --variable crime \
  [--weights weights.json | --weights-type queen] [--permutation 999]

# bivariate: do TWO binary variables co-locate? (no-colocation statistic)
node analyze.mjs colocation --file data.geojson --variable highIncome --variable-b highCrime \
  [--weights weights.json | --weights-type queen] [--permutation 999]
```

- Input variables **must be binary (0/1)**; binarize first (e.g. "above median
  → 1") or reuse classification breaks.
- Univariate → `localJoinCount` (Local Join Count). Bivariate →
  `localBiJoinCount` (Bivariate Local Join Count / no-colocation). For 3+
  variables use `multivariateLocalJoinCount` in a script.
- **Version requirement**: join counts exist in `@geoda/lisa >= 0.0.23`; the
  plugin installs `@geoda/* >= 0.0.24` (the published 0.0.23 core/lisa builds
  fail to load their WASM in node — do not downgrade to it).
- **Bivariate requires no-colocation**: `--variable-b` computes the
  no-colocation statistic and throws if any observation is `1` in BOTH
  variables. Use mutually exclusive categories or a variable split with its
  complement (e.g. `X` and `1-X`) to test the two states.
- This is the **statistical** colocation test — for a visual "where do two
  variables overlap" overlay, do that with a map/SQL tool instead.

### 5. Spatial regression

```bash
node analyze.mjs regression --file data.geojson \
  --dependent price --independent income --independent crime \
  --model spatial-lag           # classic | spatial-lag | spatial-error
  [--weights weights.json | --weights-type queen]
```

Output: coefficient estimates, standard errors, pseudo R², diagnostics.
- `classic` = OLS. `spatial-lag` / `spatial-error` require weights.
- Keep independent variables distinct from the dependent variable.

### 6. Classification (breaks)

```bash
node analyze.mjs classify --file data.geojson --variable income \
  --method quantile --k 5       # quantile | naturalBreaks | equalInterval | percentile | stddev | hinge15
```

Output: `breaks` (lower bound inclusive, upper exclusive except the last bin,
which is inclusive of both). Feed breaks into a map color scale:
`[{value: <break>, color: ...}, ..., {value: null, color: ...}]` — the final
`null` entry colors the highest bin.

### 7. Rates

```bash
node analyze.mjs rate --file data.geojson --event diseaseCount --base population \
  --method excessRisk           # rawRates | excessRisk | empiricalBayes
```

Output: per-observation rate array. `base` (population at risk) is the first
argument, `event` second.

### 8. Standardization

```bash
node analyze.mjs standardize --file data.geojson --variable income \
  --method standardize          # standardize | standardizeMAD | deviationFromMean | rangeAdjust | rangeStandardize
```

Output: per-observation standardized array.

### 9. Geometry outputs

```bash
node analyze.mjs thiessen --file data.geojson --out thiessen.geojson   # Thiessen polygons
node analyze.mjs mst --file data.geojson --out mst.geojson             # Minimum Spanning Tree edges
node analyze.mjs cartogram --file data.geojson --weight population --iterations 100 --out carto.geojson
```

Write GeoJSON with `--out` when the result is a feature set. Buffer, dissolve,
and spatial-join are **not** here — those are SQL/table-surface operations
(kepler-assistant / DuckDB spatial), kept out of the standalone library skill.

## Rules

- **Create weights FIRST**; reuse the saved neighbor list for LISA / Moran /
  regression / colocation.
- STRICT: never feed output from a previous LISA/join-count run back in as new
  input data — a result dataset describes clusters, not the original variable.
- For clustering/hot-spot questions, always run a statistical test (LISA,
  join count) rather than judging by eye on a map.
- Join-count colocation needs **binary 0/1** variables; binarize before running.
- When an op produces a derived value the user also wants stored, keep the raw
  result and compute/overlay derived columns downstream rather than re-running
  the analysis.
- If input is a road/line dataset, buffer it by 1 m first (a geometry/SQL op)
  so adjacency is meaningful, then run LISA on the buffered layer.
- For reproducibility keep the seed default (`1234567890`) or set it explicitly;
  permutations are random otherwise.

## Output interpretation

`lisa` / `colocation` return per-observation arrays aligned with the input
feature order plus a `labels`/`colors` legend (index `i` in `clusters` ↔ feature
`i`). `classify` returns ordered break points. `regression` returns model
diagnostics; report coefficients with their significance, not just the pseudo
R². When a result array has a `sigCategories` field, the "Not significant"
category is always the one with no spatial signal — don't present those
observations as clusters.
