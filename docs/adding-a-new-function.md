---
outline: deep
---

# Adding a New Function to GeoDaLib

This guide walks through the end-to-end process of adding a new function to the
GeoDaLib monorepo, using the **fixed-bandwidth kernel weights** feature
(`geoda::kernel_weights` / `getKernelWeights`) as a worked example. It is written
so that a coding agent can follow it step by step.

GeoDaLib is a monorepo with a **C++ core** compiled to **WebAssembly**, exposed through a
**JavaScript API**. Adding a function means touching the C++ core, the WASM binding, the JS
wrapper, tests, and docs — in that order.

---

## 1. Understand the architecture

```
cpp/src/            C++ core library (the "real" algorithm)
cpp/wasm/           Emscripten bindings that expose C++ to JS
cpp/test/           C++ unit tests (gtest/gmock)
js/packages/core/   JS package: wrappers + WASM glue + Jest tests
js/packages/common/ @geoda/common: shared types incl. the WASM interface (GeoDaModule)
docs/               VitePress docs (some hand-written, some typedoc-generated)
```

The data flow for a JS call is:

```
JS wrapper  ->  wasmInstance.<fn>()  ->  C++ (compiled to wasm)  ->  result back to JS
```

### Where things live (with the kernel-weights example)

| Layer | Location | Kernel example |
|-------|----------|----------------|
| C++ core | `cpp/src/weights/weights.h`, `cpp/src/weights/distance-weights.cpp` | `cpp/src/weights/kernel-weights.cpp` |
| Source list | `cpp/src/CMakeLists.txt` (`WEIGHTS_SOURCE_FILES`, `CORE_SOURCE_FILES`) | added `kernel-weights.cpp` |
| C++ tests | `cpp/test/test-weights.cpp`, `cpp/test/data.h` (`TEST_POINT_COLLECTION`) | `WEIGHTS.KERNEL_WEIGHTS*` |
| WASM binding | `cpp/wasm/wasm.cpp`, `cpp/wasm/wasm-core.cpp` | `emscripten::function("getKernelWeights", ...)` |
| JS wrapper | `js/packages/core/src/weights/` | `js/packages/core/src/weights/kernel-weights.ts` |
| JS export | `js/packages/core/src/index.ts` | `export * from './weights/kernel-weights'` |
| WASM interface type | `js/packages/common/src/wasm/index.d.ts` (`GeoDaModule`) | `getKernelWeights(...)` |
| JS tests | `js/packages/core/test/weights/` | `kernel-weights.test.ts` |
| Docs | `docs/reference/`, `docs/api-overview.md`, regenerated `docs/api/` | Kernel Weights section |

---

## 2. Implement the C++ core function

1. Add a declaration to the relevant header in `cpp/src/.../`. Give it a clear doc comment.
2. Add the implementation as a new `.cpp` file (mirroring an existing sibling file's structure
   — e.g. `kernel-weights.cpp` mirrors `distance-weights.cpp`, including the boost rtree logic
   and `haversine_distance`).

```cpp
// cpp/src/weights/weights.h
std::vector<std::vector<double>> kernel_weights(
    const GeometryCollection& geoms, double bandwidth, const std::string& kernel,
    bool is_mile, bool use_kernel_diagonals = false, double power = 1.0);
```

> Note: give new optional arguments sensible C++ defaults so existing callers keep working.

---

## 3. Register the new source file

The build uses an **explicit source list**, so a new `.cpp` will not be picked up automatically.
Add it to the relevant list in `cpp/src/CMakeLists.txt`:

```cmake
set (WEIGHTS_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/geometry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/weights/distance-weights.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/weights/kernel-weights.cpp   # <-- add here
    ...)
```

---

## 4. Add C++ tests

Add tests to the relevant `cpp/test/test-<feature>.cpp`. Use the in-memory test collections in
`cpp/test/data.h` (e.g. `TEST_POINT_COLLECTION`, `TEST_POLYGON_COLLECTION`). When a row's
neighbor order is not guaranteed, convert to pairs and sort (see `expect_pairs` in
`test-weights.cpp`).

Cover at least:
- the happy path with known expected values,
- optional arguments (defaults vs. explicit values),
- edge cases (empty neighbors, unit switches like `is_mile`),
- invalid input handling if the function throws.

---

## 5. Build and run the C++ tests

```bash
# cmake is at /Applications/CMake.app/Contents/bin/cmake on this machine
/Applications/CMake.app/Contents/bin/cmake -S cpp -B build
/Applications/CMake.app/Contents/bin/cmake --build build --target runUnitTests -j4
./build/test/runUnitTests                       # run everything
./build/test/runUnitTests --gtest_filter='WEIGHTS.*'   # just your area
```

> **Build-environment gotcha (boost 1.75 + Apple clang ≥ 21):** the native build may fail to
> compile geometry files with an error in `boost/mpl/aux_/integral_wrapper.hpp`
> (`non-type template argument is not a constant expression`). The known workaround is the
> upstream boost backport: in
> `build/_deps/boost-src/boost/numeric/conversion/detail/{int_float_mixture,sign_mixture,udt_builtin_mixture}.hpp`
> change `mpl::integral_c<...>` to `boost::integral_constant<...>` and include
> `boost/type_traits/integral_constant.hpp`. This lives only under `build/` (gitignored) — do
> **not** commit it.

---

## 6. Expose it through the WASM binding

Add an `emscripten::function(...)` call in the WASM binding files. The "core" bindings live in
both `cpp/wasm/wasm.cpp` and `cpp/wasm/wasm-core.cpp` — add it to **both** if the function
belongs to the core module.

```cpp
// cpp/wasm/wasm.cpp (and wasm-core.cpp), next to the other weight bindings
emscripten::function("getKernelWeights", &geoda::kernel_weights);
```

Note that Emscripten binds the function pointer at **full arity** — C++ default arguments are
**not** preserved. The JS side must pass every parameter explicitly.

---

## 7. Rebuild the WASM artifact

The JS package ships a **committed** `js/packages/core/wasm/geoda-core.wasm` and `index.cjs`.
The JS tests run against that committed wasm, so it must be regenerated whenever the binding
changes.

Emscripten is at `/opt/homebrew/bin` on this machine (add it to `PATH`). Rebuild with the
project's script, which writes to `js/packages/core/wasm/`:

```bash
cd cpp/wasm
export PATH="/opt/homebrew/bin:/Applications/CMake.app/Contents/bin:$PATH"
export EM_CACHE="<a writable cache dir, e.g. $HOME/.em_cache>"
./build-wasm.sh ../../js/packages/core/wasm geoda-core
```

> **Two Emscripten-version gotchas** (modern Emscripten 4.x vs. the original 3.1.55):
> 1. `emscripten/bind.h` requires `std::optional`, so the WASM build must use **C++17**
>    (`set(CMAKE_CXX_STANDARD 17)` in `cpp/wasm/CMakeLists.txt`).
> 2. The generated glue emits a **BigInt literal** by default, which fails the JS esbuild step
>    (es2017 target). Add `-s WASM_BIGINT=0` to the WASM linker flags and regenerate.

If the stale `build/out` cache points at a removed Emscripten, configure a fresh build dir with
`emcmake cmake ... -B ../../build/out-wasm -DUNITTEST=OFF -DUSE_EMCC=ON -DCMAKE_BUILD_TYPE=Release`
and `emmake make geoda-core`.

Verify the new symbol is present before committing:

```bash
strings js/packages/core/wasm/geoda-core.wasm | grep -c getKernelWeights   # expect >= 1
grep -oE "[0-9]+n\b" js/packages/core/wasm/index.cjs | wc -l              # expect 0
```

---

## 8. Add the JS wrapper

Add a wrapper in `js/packages/core/src/weights/` (or the appropriate subfolder) that calls the
WASM module. Follow the existing pattern (`distance-neighbors.ts`): a `...FromBinaryGeometries`
entry point and a `...FromGeomCollection` entry point, both calling `initWASM()` and then the
bound C++ function.

```ts
export async function getKernelWeightsFromGeomCollection({ ... }): Promise<number[][]> {
  const wasmInstance = await initWASM();
  const result = wasmInstance.getKernelWeights(geomCollection, bandwidth, kernel,
                                               isMile, useKernelDiagonals, power);
  // convert the returned vector-of-vectors into number[][]
}
```

Export it from `js/packages/core/src/index.ts`:

```ts
export * from './weights/kernel-weights';
```

---

## 9. Update the WASM interface type (`@geoda/common`)

The TypeScript type of the WASM instance is declared in
`js/packages/common/src/wasm/index.d.ts` (`GeoDaModule`). Add the new method with the exact
C++ signature types:

```ts
getKernelWeights(
  geometries: GeometryCollection,
  bandwidth: Double,
  kernel: string,
  isMile: boolean,
  useKernelDiagonals: boolean,
  power: Double
): VecVecDouble;
```

Then **rebuild the common package** so its `dist` reflects the change (TypeScript resolves
`@geoda/common` via the built `dist`):

```bash
cd js/packages/common && node esbuild.config.mjs
```

---

## 10. Add JS tests

Add `js/packages/core/test/weights/<feature>.test.ts` using the binary-geometry fixtures
(`getBinaryGeometryTemplate()`). These tests exercise the **real** compiled wasm (the test
`setup.ts` points `setDeliveryWASM` at `wasm/geoda-core.wasm`). Use `toBeCloseTo` for floating
point weights and compute expected values from first principles.

Run them:

```bash
cd js/packages/core
node ../../node_modules/.bin/jest --no-cache test/weights/kernel-weights.test.ts
node ../../node_modules/.bin/jest --no-cache      # full core suite
```

---

## 11. Lint, typecheck, and the full build

```bash
cd js/packages/core
node ../../node_modules/.bin/eslint --max-warnings 0 <changed files>
node esbuild.config.mjs    # esbuild build (this is what CI's "Build" step runs)
```

Lint rules that commonly trip up CI:
- `no-unused-vars` — remove any unused imports.
- `prettier/prettier` — match formatting (single-line imports etc.).

---

## 12. Documentation

Two kinds of docs:

- **Hand-written pages** (`docs/reference/spatial-weights.md`, `docs/api-overview.md`,
  `docs/api-examples.md`): add links to the new function page.
- **Generated API docs** (`docs/api/**`): run typedoc to regenerate:

```bash
cd docs
./node_modules/.bin/typedoc
```

This creates the per-function page (`docs/api/core/src/functions/getKernelWeightsFromBinaryGeometries.md`)
and updates `typedoc-sidebar.json` and `docs/api/core/src/index.md`.

> The typedoc regeneration rewrites many unrelated files (line-number / git-hash churn). Only
> commit the files that genuinely changed for the feature, and revert the pure-churn files.

Verify the docs build:

```bash
cd docs && ./node_modules/.bin/vitepress build .
```

---

## 13. Commit and open the PR

The repo requires **DCO sign-off** on every commit.

```bash
git checkout -b feat/<your-feature>
git add <changed files>
git commit -s -m "feat(...): <subject>

<body>"
git push -u origin feat/<your-feature>
```

The DCO job enforces the `Signed-off-by` trailer (git identity must be set). Open a PR with
`head = feat/<your-feature>` and `base = main`.

---

## 14. Watch CI and fix failures

The PR checks are:
- **dco** — requires the `Signed-off-by:` trailer.
- **build-test-publish** — runs **Build → Lint → Test** (JS only; it does NOT rebuild the wasm).

Common CI failures and fixes:
| Failure | Cause | Fix |
|---------|-------|-----|
| Lint `no-unused-vars` | unused import in a test/wrapper | remove it |
| Build `Big integer literals ... es2017` | wasm glue emits BigInt | rebuild with `-s WASM_BIGINT=0` |
| Test `getXxx is not a function` | committed wasm predates the binding | regenerate & commit `geoda-core.wasm`/`index.cjs` |
| Test fails on an exception that JS "should" see | Emscripten doesn't propagate native C++ exceptions by default | adjust the JS test (the C++ test still covers the throw) |

---

## Checklist

- [ ] C++ function declared in a `cpp/src/...` header with doc comment
- [ ] Implementation in a new `.cpp` added to the explicit CMake source list
- [ ] C++ unit tests added and passing (`runUnitTests`)
- [ ] WASM binding added in `wasm.cpp` and `wasm-core.cpp`
- [ ] WASM rebuilt and committed (`geoda-core.wasm`, `index.cjs`)
- [ ] JS wrapper added and exported in `index.ts`
- [ ] `GeoDaModule` type updated in `@geoda/common`, common dist rebuilt
- [ ] JS Jest tests added and passing (full core suite)
- [ ] ESLint passes on changed files
- [ ] esbuild build passes
- [ ] Docs updated (hand-written + typedoc regen) and `vitepress build` passes
- [ ] Commits are DCO-signed (`git commit -s`)
- [ ] CI green: `dco` and `build-test-publish`
