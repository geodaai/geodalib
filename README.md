# GeoDaLib 

GeoDaLib is a WebAssembly module that provides spatial data analysis capabilities from the GeoDa C++ library for use in web browsers and Node.js applications.

## ✨ Features

@geoda/core
- 🗺️ Mapping
- 📊 Data Exploration
- ⚖️ Spatial Weights
- ⚡ Spatial Operations

@geoda/lisa
- 📐 Spatial Autocorrelation Analysis

@geoda/regression
- 📈 Spatial Regression

@geoda/clustering (WIP)
- 📍 Spatial Clustering

Dive deeper in our [API Overview](https://geodacenter.github.io/geoda-lib/api-overview)!

## 🚀 Installation

Get started in a flash with your favorite package manager:

```bash
npm install @geoda/core
```

Each package works independently, so you can install only the packages you need.

## 🛠️ Example

Here is a quick example of using GeoDaLib to apply Local Moran statistics on some example data:

```js
import {localMoran} from '@geoda/lisa';

// exmaple data
const data = [3.0, 3.0, 0.0, 9.0, 8.0, 8.5];
const neighbors = [[1], [0], [], [4, 5], [3, 5], [3, 4]];
const permutation = 99;

// call local moran
const result = await localMoran({data, neighbors, permutation});
```

## 🤖 Claude Code plugin & standalone skill

`skills/geoda-analysis/` packages the analysis engine for agent harnesses: it's
installable as a Claude Code plugin, runnable standalone in node (no GUI, no map
host), and sourced by kepler-assistant at build time.

### Claude Code plugin

```bash
claude plugin marketplace add geodaai/geodalib
claude plugin install geoda-analysis@geodalib
```

The plugin auto-installs `@geoda/core`, `@geoda/lisa`, `@geoda/regression`
(`>= 0.0.24`) into the skill directory on install, and runs every operation
through `scripts/analyze.mjs`. Full reference: `skills/geoda-analysis/SKILL.md`.

### Standalone node driver

```bash
cd skills/geoda-analysis
npm i @geoda/core @geoda/lisa @geoda/regression
node scripts/analyze.mjs --help
node scripts/analyze.mjs lisa --file data.geojson --variable income --weights-type queen
```

Ops: spatial weights, LISA (local Moran / Geary / local G / G* / quantile),
global Moran, colocation join counts (univariate + bivariate no-colocation),
spatial regression (classic / spatial-lag / spatial-error), classification,
rates, standardization, Thiessen polygons, MST, cartogram.

### kepler-assistant host

kepler-assistant sources this skill at build time (see its `GEODA_SKILL_DIR`
env var) and appends its own map-host command surface, so the standalone skill
and the map-hosted assistant stay in sync.

## 🏗️ Development

### 📋 Prerequisites

- CMake (3.5 or higher)
- Emscripten
- Node.js (22.11.0 recommended)
- Yarn (4.0.0 recommended)

### 🛠️ Build Steps

1. Clone the repository
2. Build the WASM module:

```bash
cd js
yarn install
yarn wasm
```

3. Build the JavaScript wrapper:

```bash
yarn build
```

4. Run the tests:

```bash
yarn test
```

## 📜 License

This project is licensed under the MIT License. Authors: Luc Anselin and Xun Li.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

### Developer Certificate of Origin (DCO)

This project requires all contributors to sign their commits with a Developer Certificate of Origin (DCO). This certifies that you have the right to submit your contribution to the project.

**All commits must be signed with:**
```
Signed-off-by: Your Name <your.email@example.com>
```

You can sign commits using:
```bash
git commit -s -m "Your commit message"
```

For more information, see our [Contributing Guide](contributing/CONTRIBUTING.md) and [DCO Documentation](DCO.md).
