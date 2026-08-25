# Change Log

All notable changes to this project will be documented in this file.
See [Conventional Commits](https://conventionalcommits.org) for commit guidelines.

## [0.0.24](https://github.com/geodaai/geodalib/compare/@geoda/core@0.0.23...@geoda/core@0.0.24) (2026-08-25)

### Bug Fixes

* **js:** load WASM from local file in Node without fs/path polyfills ([fa1cb96](https://github.com/geodaai/geodalib/commit/fa1cb9629cfc7350af39b2232e52309ddd3ed1c3))

## [0.0.23](https://github.com/geodaai/geodalib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.23) (2026-08-24)

### Bug Fixes

* **clustering:** address schc/redcap/azp/maxp review comments ([f7ace91](https://github.com/geodaai/geodalib/commit/f7ace914844cf90b0f7ea4d0b14056fecf2e2a83))
* **clustering:** resolve azp/maxp infinite loop on degenerate inputs ([5900cdf](https://github.com/geodaai/geodalib/commit/5900cdf87ab9f1841259a3eee9bff5ae22fbadf4))
* **clustering:** validate data/boundVals/geometries lengths before WASM ([fb2acae](https://github.com/geodaai/geodalib/commit/fb2acae4efa868ebb3c7f5d4a2f1c8ebcf07f7ca))
* **clustering:** validate inputs and remove non-standard patterns in clustering API ([4a59d11](https://github.com/geodaai/geodalib/commit/4a59d11d04d0fcfe37f376db64317ad73f7950af))

### Features

* **clustering:** add azp and max-p (greedy) ([f811b39](https://github.com/geodaai/geodalib/commit/f811b3963642b62e0a1f5367d019edd753f146d9))
* **clustering:** add azp and max-p simulated annealing and tabu variants ([35618bb](https://github.com/geodaai/geodalib/commit/35618bb25e66bc0fb82a8c6a6c6ee02820e8e23d))
* **clustering:** add make spatial ([#24](https://github.com/geodaai/geodalib/issues/24)) ([a2d1c32](https://github.com/geodaai/geodalib/commit/a2d1c32cad992254d79835fd0a78839141b2695c))
* **clustering:** add redcap and skater ([da39611](https://github.com/geodaai/geodalib/commit/da396110b032faef485b2c84b36f1e03d9e7a5c6))
* **clustering:** add spatial validation ([33a270c](https://github.com/geodaai/geodalib/commit/33a270cd438c7aee3d06a03bc36fa5d8730774d1))
* **clustering:** add spatially constrained hierarchical clustering (schc) ([f69369a](https://github.com/geodaai/geodalib/commit/f69369aae9a4a8b736c458aa856611e27f0eea97))
* **weights:** add kernel k-nearest neighbors weights ([#17](https://github.com/geodaai/geodalib/issues/17)) ([4c1c4bd](https://github.com/geodaai/geodalib/commit/4c1c4bd228762a279369141402cfc8b3d1ff91c3))
* **weights:** add kernel weights creation ([#16](https://github.com/geodaai/geodalib/issues/16)) ([eafbe1f](https://github.com/geodaai/geodalib/commit/eafbe1f45c54b9fa44aceed9bc426f3c2938882f))
* **weights:** add local neighbor match test ([#23](https://github.com/geodaai/geodalib/issues/23)) ([1c05f22](https://github.com/geodaai/geodalib/commit/1c05f221fe92c5325cd33aedb2902ff3e9cb96a8))

## 0.0.22 (2025-06-20)

## 0.0.21 (2025-06-18)

## 0.0.20 (2025-06-18)

## 0.0.19 (2025-06-17)

### Bug Fixes

* cartogram produces overlapped circles ([d3a7111](https://github.com/geodaai/geodalib/commit/d3a7111e76785bf1de6fade18c8d715b10af5d2b))

## 0.0.18 (2025-06-17)

### Bug Fixes

* cartogram not scaling to coordinates ([5950f7e](https://github.com/geodaai/geodalib/commit/5950f7e7eb84b14a5392f693a59a8c26d3c0cda2))

## 0.0.17 (2025-06-17)

### Features

* add cartogram ([47fa1de](https://github.com/geodaai/geodalib/commit/47fa1de30ad6b6f6bc0a0636fe44b85ef7dc4cf0))

## 0.0.16 (2025-06-11)

## 0.0.15 (2025-06-11)

## 0.0.14 (2025-06-10)

### Bug Fixes

* MST incorrect distance matrix (half) calcuation ([432c79f](https://github.com/geodaai/geodalib/commit/432c79fce0a7469941039d0ef2113e8007a6cf19))

## 0.0.13 (2025-06-10)

### Bug Fixes

* dissolve function return feature[] instead of a single multipolygon ([5db5bbb](https://github.com/geodaai/geodalib/commit/5db5bbbbcd74ebdd532435cca114db20be64abad))

## 0.0.12 (2025-06-09)

### Features

* MST + fix: ThiessenPolygon + update docs ([c1d5098](https://github.com/geodaai/geodalib/commit/c1d5098e85a3c762189363c65f65346c385d946c))

## 0.0.11 (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/geodaai/geodalib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/geodaai/geodalib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/geodaai/geodalib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/geodaai/geodalib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/geodaai/geodalib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.22](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.22) (2025-06-20)

## 0.0.21 (2025-06-18)

## 0.0.20 (2025-06-18)

## 0.0.19 (2025-06-17)

### Bug Fixes

* cartogram produces overlapped circles ([d3a7111](https://github.com/GeoDaCenter/geoda-lib/commit/d3a7111e76785bf1de6fade18c8d715b10af5d2b))

## 0.0.18 (2025-06-17)

### Bug Fixes

* cartogram not scaling to coordinates ([5950f7e](https://github.com/GeoDaCenter/geoda-lib/commit/5950f7e7eb84b14a5392f693a59a8c26d3c0cda2))

## 0.0.17 (2025-06-17)

### Features

* add cartogram ([47fa1de](https://github.com/GeoDaCenter/geoda-lib/commit/47fa1de30ad6b6f6bc0a0636fe44b85ef7dc4cf0))

## 0.0.16 (2025-06-11)

## 0.0.15 (2025-06-11)

## 0.0.14 (2025-06-10)

### Bug Fixes

* MST incorrect distance matrix (half) calcuation ([432c79f](https://github.com/GeoDaCenter/geoda-lib/commit/432c79fce0a7469941039d0ef2113e8007a6cf19))

## 0.0.13 (2025-06-10)

### Bug Fixes

* dissolve function return feature[] instead of a single multipolygon ([5db5bbb](https://github.com/GeoDaCenter/geoda-lib/commit/5db5bbbbcd74ebdd532435cca114db20be64abad))

## 0.0.12 (2025-06-09)

### Features

* MST + fix: ThiessenPolygon + update docs ([c1d5098](https://github.com/GeoDaCenter/geoda-lib/commit/c1d5098e85a3c762189363c65f65346c385d946c))

## 0.0.11 (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/GeoDaCenter/geoda-lib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.21](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.21) (2025-06-18)

## 0.0.20 (2025-06-18)

## 0.0.19 (2025-06-17)

### Bug Fixes

* cartogram produces overlapped circles ([d3a7111](https://github.com/GeoDaCenter/geoda-lib/commit/d3a7111e76785bf1de6fade18c8d715b10af5d2b))

## 0.0.18 (2025-06-17)

### Bug Fixes

* cartogram not scaling to coordinates ([5950f7e](https://github.com/GeoDaCenter/geoda-lib/commit/5950f7e7eb84b14a5392f693a59a8c26d3c0cda2))

## 0.0.17 (2025-06-17)

### Features

* add cartogram ([47fa1de](https://github.com/GeoDaCenter/geoda-lib/commit/47fa1de30ad6b6f6bc0a0636fe44b85ef7dc4cf0))

## 0.0.16 (2025-06-11)

## 0.0.15 (2025-06-11)

## 0.0.14 (2025-06-10)

### Bug Fixes

* MST incorrect distance matrix (half) calcuation ([432c79f](https://github.com/GeoDaCenter/geoda-lib/commit/432c79fce0a7469941039d0ef2113e8007a6cf19))

## 0.0.13 (2025-06-10)

### Bug Fixes

* dissolve function return feature[] instead of a single multipolygon ([5db5bbb](https://github.com/GeoDaCenter/geoda-lib/commit/5db5bbbbcd74ebdd532435cca114db20be64abad))

## 0.0.12 (2025-06-09)

### Features

* MST + fix: ThiessenPolygon + update docs ([c1d5098](https://github.com/GeoDaCenter/geoda-lib/commit/c1d5098e85a3c762189363c65f65346c385d946c))

## 0.0.11 (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/GeoDaCenter/geoda-lib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.20](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.20) (2025-06-18)

## 0.0.19 (2025-06-17)

### Bug Fixes

* cartogram produces overlapped circles ([d3a7111](https://github.com/GeoDaCenter/geoda-lib/commit/d3a7111e76785bf1de6fade18c8d715b10af5d2b))

## 0.0.18 (2025-06-17)

### Bug Fixes

* cartogram not scaling to coordinates ([5950f7e](https://github.com/GeoDaCenter/geoda-lib/commit/5950f7e7eb84b14a5392f693a59a8c26d3c0cda2))

## 0.0.17 (2025-06-17)

### Features

* add cartogram ([47fa1de](https://github.com/GeoDaCenter/geoda-lib/commit/47fa1de30ad6b6f6bc0a0636fe44b85ef7dc4cf0))

## 0.0.16 (2025-06-11)

## 0.0.15 (2025-06-11)

## 0.0.14 (2025-06-10)

### Bug Fixes

* MST incorrect distance matrix (half) calcuation ([432c79f](https://github.com/GeoDaCenter/geoda-lib/commit/432c79fce0a7469941039d0ef2113e8007a6cf19))

## 0.0.13 (2025-06-10)

### Bug Fixes

* dissolve function return feature[] instead of a single multipolygon ([5db5bbb](https://github.com/GeoDaCenter/geoda-lib/commit/5db5bbbbcd74ebdd532435cca114db20be64abad))

## 0.0.12 (2025-06-09)

### Features

* MST + fix: ThiessenPolygon + update docs ([c1d5098](https://github.com/GeoDaCenter/geoda-lib/commit/c1d5098e85a3c762189363c65f65346c385d946c))

## 0.0.11 (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/GeoDaCenter/geoda-lib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.19](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.19) (2025-06-17)

### Bug Fixes

* cartogram produces overlapped circles ([d3a7111](https://github.com/GeoDaCenter/geoda-lib/commit/d3a7111e76785bf1de6fade18c8d715b10af5d2b))

## 0.0.18 (2025-06-17)

### Bug Fixes

* cartogram not scaling to coordinates ([5950f7e](https://github.com/GeoDaCenter/geoda-lib/commit/5950f7e7eb84b14a5392f693a59a8c26d3c0cda2))

## 0.0.17 (2025-06-17)

### Features

* add cartogram ([47fa1de](https://github.com/GeoDaCenter/geoda-lib/commit/47fa1de30ad6b6f6bc0a0636fe44b85ef7dc4cf0))

## 0.0.16 (2025-06-11)

## 0.0.15 (2025-06-11)

## 0.0.14 (2025-06-10)

### Bug Fixes

* MST incorrect distance matrix (half) calcuation ([432c79f](https://github.com/GeoDaCenter/geoda-lib/commit/432c79fce0a7469941039d0ef2113e8007a6cf19))

## 0.0.13 (2025-06-10)

### Bug Fixes

* dissolve function return feature[] instead of a single multipolygon ([5db5bbb](https://github.com/GeoDaCenter/geoda-lib/commit/5db5bbbbcd74ebdd532435cca114db20be64abad))

## 0.0.12 (2025-06-09)

### Features

* MST + fix: ThiessenPolygon + update docs ([c1d5098](https://github.com/GeoDaCenter/geoda-lib/commit/c1d5098e85a3c762189363c65f65346c385d946c))

## 0.0.11 (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/GeoDaCenter/geoda-lib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.18](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.18) (2025-06-17)

### Bug Fixes

* cartogram not scaling to coordinates ([5950f7e](https://github.com/GeoDaCenter/geoda-lib/commit/5950f7e7eb84b14a5392f693a59a8c26d3c0cda2))

## 0.0.17 (2025-06-17)

### Features

* add cartogram ([47fa1de](https://github.com/GeoDaCenter/geoda-lib/commit/47fa1de30ad6b6f6bc0a0636fe44b85ef7dc4cf0))

## 0.0.16 (2025-06-11)

## 0.0.15 (2025-06-11)

## 0.0.14 (2025-06-10)

### Bug Fixes

* MST incorrect distance matrix (half) calcuation ([432c79f](https://github.com/GeoDaCenter/geoda-lib/commit/432c79fce0a7469941039d0ef2113e8007a6cf19))

## 0.0.13 (2025-06-10)

### Bug Fixes

* dissolve function return feature[] instead of a single multipolygon ([5db5bbb](https://github.com/GeoDaCenter/geoda-lib/commit/5db5bbbbcd74ebdd532435cca114db20be64abad))

## 0.0.12 (2025-06-09)

### Features

* MST + fix: ThiessenPolygon + update docs ([c1d5098](https://github.com/GeoDaCenter/geoda-lib/commit/c1d5098e85a3c762189363c65f65346c385d946c))

## 0.0.11 (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/GeoDaCenter/geoda-lib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.17](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.17) (2025-06-17)

### Features

* add cartogram ([47fa1de](https://github.com/GeoDaCenter/geoda-lib/commit/47fa1de30ad6b6f6bc0a0636fe44b85ef7dc4cf0))

## 0.0.16 (2025-06-11)

## 0.0.15 (2025-06-11)

## 0.0.14 (2025-06-10)

### Bug Fixes

* MST incorrect distance matrix (half) calcuation ([432c79f](https://github.com/GeoDaCenter/geoda-lib/commit/432c79fce0a7469941039d0ef2113e8007a6cf19))

## 0.0.13 (2025-06-10)

### Bug Fixes

* dissolve function return feature[] instead of a single multipolygon ([5db5bbb](https://github.com/GeoDaCenter/geoda-lib/commit/5db5bbbbcd74ebdd532435cca114db20be64abad))

## 0.0.12 (2025-06-09)

### Features

* MST + fix: ThiessenPolygon + update docs ([c1d5098](https://github.com/GeoDaCenter/geoda-lib/commit/c1d5098e85a3c762189363c65f65346c385d946c))

## 0.0.11 (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/GeoDaCenter/geoda-lib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.16](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.16) (2025-06-11)

## 0.0.15 (2025-06-11)

## 0.0.14 (2025-06-10)

### Bug Fixes

* MST incorrect distance matrix (half) calcuation ([432c79f](https://github.com/GeoDaCenter/geoda-lib/commit/432c79fce0a7469941039d0ef2113e8007a6cf19))

## 0.0.13 (2025-06-10)

### Bug Fixes

* dissolve function return feature[] instead of a single multipolygon ([5db5bbb](https://github.com/GeoDaCenter/geoda-lib/commit/5db5bbbbcd74ebdd532435cca114db20be64abad))

## 0.0.12 (2025-06-09)

### Features

* MST + fix: ThiessenPolygon + update docs ([c1d5098](https://github.com/GeoDaCenter/geoda-lib/commit/c1d5098e85a3c762189363c65f65346c385d946c))

## 0.0.11 (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/GeoDaCenter/geoda-lib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.15](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.15) (2025-06-11)

## 0.0.14 (2025-06-10)

### Bug Fixes

* MST incorrect distance matrix (half) calcuation ([432c79f](https://github.com/GeoDaCenter/geoda-lib/commit/432c79fce0a7469941039d0ef2113e8007a6cf19))

## 0.0.13 (2025-06-10)

### Bug Fixes

* dissolve function return feature[] instead of a single multipolygon ([5db5bbb](https://github.com/GeoDaCenter/geoda-lib/commit/5db5bbbbcd74ebdd532435cca114db20be64abad))

## 0.0.12 (2025-06-09)

### Features

* MST + fix: ThiessenPolygon + update docs ([c1d5098](https://github.com/GeoDaCenter/geoda-lib/commit/c1d5098e85a3c762189363c65f65346c385d946c))

## 0.0.11 (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/GeoDaCenter/geoda-lib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.14](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.14) (2025-06-10)

### Bug Fixes

* MST incorrect distance matrix (half) calcuation ([432c79f](https://github.com/GeoDaCenter/geoda-lib/commit/432c79fce0a7469941039d0ef2113e8007a6cf19))

## 0.0.13 (2025-06-10)

### Bug Fixes

* dissolve function return feature[] instead of a single multipolygon ([5db5bbb](https://github.com/GeoDaCenter/geoda-lib/commit/5db5bbbbcd74ebdd532435cca114db20be64abad))

## 0.0.12 (2025-06-09)

### Features

* MST + fix: ThiessenPolygon + update docs ([c1d5098](https://github.com/GeoDaCenter/geoda-lib/commit/c1d5098e85a3c762189363c65f65346c385d946c))

## 0.0.11 (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/GeoDaCenter/geoda-lib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.13](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.13) (2025-06-10)

### Bug Fixes

* dissolve function return feature[] instead of a single multipolygon ([5db5bbb](https://github.com/GeoDaCenter/geoda-lib/commit/5db5bbbbcd74ebdd532435cca114db20be64abad))

## 0.0.12 (2025-06-09)

### Features

* MST + fix: ThiessenPolygon + update docs ([c1d5098](https://github.com/GeoDaCenter/geoda-lib/commit/c1d5098e85a3c762189363c65f65346c385d946c))

## 0.0.11 (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/GeoDaCenter/geoda-lib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.12](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.12) (2025-06-09)

### Features

* MST + fix: ThiessenPolygon + update docs ([c1d5098](https://github.com/GeoDaCenter/geoda-lib/commit/c1d5098e85a3c762189363c65f65346c385d946c))

## 0.0.11 (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/GeoDaCenter/geoda-lib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.11](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.11) (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/GeoDaCenter/geoda-lib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.10](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.10) (2025-06-06)

### Features

* mad, range adjust, range std, deviation, thiessen polygon ([08d7ccd](https://github.com/GeoDaCenter/geoda-lib/commit/08d7ccdf78b651ff798dc6ac7058d2a42bbef988))

## 0.0.10 (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.9](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.9) (2025-04-25)

## 0.0.8 (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.8](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.8) (2025-04-25)

### Features

* support arc; centroid=false when contiguity weights ([b2b16c4](https://github.com/GeoDaCenter/geoda-lib/commit/b2b16c4d7395beb2f5af70ea1bb8a10254d6256c))

## 0.0.7 (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.7](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.7) (2025-04-24)

### Bug Fixes

* weights not working correctly with perm lookup table in lisa ([e4d9ba4](https://github.com/GeoDaCenter/geoda-lib/commit/e4d9ba432ebc5342285468c972fcaed429ea8c7d))

## 0.0.6 (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.6](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.6) (2025-04-19)

### Features

* add spatial dissolve; add spatial ops ([e4f09e1](https://github.com/GeoDaCenter/geoda-lib/commit/e4f09e1954ecff6a054b7b51f06ed1074750ffbb))

## 0.0.5 (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.5](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.5) (2025-04-16)

## 0.0.4 (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.4](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.4) (2025-04-15)

## 0.0.3 (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.3](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.3) (2025-04-02)

## 0.0.3-alpha.4 (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.3-alpha.4](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.3-alpha.4) (2025-04-02)

## 0.0.3-alpha.3 (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.3-alpha.3](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.3-alpha.3) (2025-04-02)

## 0.0.3-alpha.2 (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.3-alpha.2](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.3-alpha.2) (2025-04-02)

## 0.0.3-alpha.1 (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.3-alpha.1](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.3-alpha.1) (2025-04-02)

## 0.0.3-alpha.0 (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.3-alpha.0](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.3-alpha.0) (2025-04-02)

## 0.0.2 (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.2](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.2) (2025-04-01)

## 0.0.2-alpha.13 (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.2-alpha.13](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.2-alpha.13) (2025-04-01)

## 0.0.2-alpha.12 (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.2-alpha.12](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.2-alpha.12) (2025-04-01)

## 0.0.2-alpha.11 (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.2-alpha.11](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.2-alpha.11) (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.2-alpha.10](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.2-alpha.10) (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.2-alpha.9](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.2-alpha.9) (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.2-alpha.8](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.2-alpha.8) (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.2-alpha.7](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.2-alpha.7) (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.2-alpha.6](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.2-alpha.6) (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.2-alpha.5](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.4...@geoda/core@0.0.2-alpha.5) (2025-04-01)

### Features

* release packages core, lisa and regression ([347dcf7](https://github.com/GeoDaCenter/geoda-lib/commit/347dcf7f1a9fa11cae215856da02d4f664b84636))

## [0.0.2-alpha.4](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.3...@geoda/core@0.0.2-alpha.4) (2025-04-01)

**Note:** Version bump only for package @geoda/core

## [0.0.2-alpha.3](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.2...@geoda/core@0.0.2-alpha.3) (2025-04-01)

**Note:** Version bump only for package @geoda/core

## [0.0.2-alpha.2](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.1...@geoda/core@0.0.2-alpha.2) (2025-04-01)

**Note:** Version bump only for package @geoda/core

## [0.0.2-alpha.1](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.2-alpha.0...@geoda/core@0.0.2-alpha.1) (2025-04-01)

**Note:** Version bump only for package @geoda/core

## [0.0.2-alpha.0](https://github.com/GeoDaCenter/geoda-lib/compare/@geoda/core@0.0.1...@geoda/core@0.0.2-alpha.0) (2025-04-01)

**Note:** Version bump only for package @geoda/core

## 0.0.1 (2025-04-01)

**Note:** Version bump only for package @geoda/core
