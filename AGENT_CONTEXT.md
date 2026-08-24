# Agent Context — GeoDaLib

Quick-start context for a coding agent working in the `geoda-lib` monorepo.
For the full "add a new function" walkthrough, read **`docs/adding-a-new-function.md`**.

## What this repo is

A monorepo with a **C++ core** compiled to **WebAssembly**, exposed through a **JavaScript API**.
Adding a function touches: C++ core → WASM binding → JS wrapper → tests → docs.

```
cpp/src/            C++ core library
cpp/wasm/           Emscripten bindings (wasm.cpp, wasm-core.cpp)
cpp/test/           C++ unit tests (gtest/gmock); test data in cpp/test/data.h
js/packages/core/   JS package: wrappers + committed WASM + Jest tests
js/packages/common/ @geoda/common: shared types incl. the WASM interface (GeoDaModule)
docs/               VitePress docs (hand-written + typedoc-generated API)
```

C++ reference implementations live in the sibling repo `~/github/csds_libgeoda`
(e.g. `SpatialIndAlgs.cpp`, `gda_weights.cpp`).

## Tooling (specific to this machine)

| Tool | Location / notes |
|------|------------------|
| cmake | `/Applications/CMake.app/Contents/bin/cmake` (not on `PATH`) |
| Build native tests | `cmake -S cpp -B build && cmake --build build --target runUnitTests -j4` |
| Run C++ tests | `./build/test/runUnitTests` (filter: `--gtest_filter='WEIGHTS.*'`) |
| Emscripten | `/opt/homebrew/bin` (add to `PATH`); emcc is **4.0.23** |
| Rebuild WASM | `cd cpp/wasm && export PATH="/opt/homebrew/bin:/Applications/CMake.app/Contents/bin:$PATH" && ./build-wasm.sh ../../js/packages/core/wasm geoda-core` |
| JS build | `cd js/packages/core && node esbuild.config.mjs` |
| JS tests | `cd js/packages/core && node ../../node_modules/.bin/jest --no-cache` |
| Lint | `node ../../node_modules/.bin/eslint --max-warnings 0 <files>` |
| Docs | `cd docs && ./node_modules/.bin/typedoc` (regenerate API), `./node_modules/.bin/vitepress build .` |

## Gotchas / rules learned the hard way

1. **Boost 1.75 + Apple clang ≥ 21**: the native build can fail to compile geometry files with
   `non-type template argument is not a constant expression` in
   `boost/mpl/aux_/integral_wrapper.hpp`. Workaround: in
   `build/_deps/boost-src/boost/numeric/conversion/detail/{int_float_mixture,sign_mixture,udt_builtin_mixture}.hpp`
   change `mpl::integral_c<...>` to `boost::integral_constant<...>` (include
   `boost/type_traits/integral_constant.hpp`). This lives only under gitignored `build/` — do
   **not** commit it.

2. **CMake source list is explicit**: a new `.cpp` must be added to `cpp/src/CMakeLists.txt`
   (e.g. `WEIGHTS_SOURCE_FILES`, `CORE_SOURCE_FILES`) or it won't build.

3. **Committed WASM must be regenerated** whenever a binding changes. CI runs JS tests against
   the **committed** `js/packages/core/wasm/geoda-core.wasm` + `index.cjs` — it is **not** rebuilt
   in CI. If a binding is missing, tests fail with `getXxx is not a function`.

4. **Modern Emscripten (4.x)** needs:
   - **C++17** in the WASM build (`set(CMAKE_CXX_STANDARD 17)` in `cpp/wasm/CMakeLists.txt`).
   - `-s WASM_BIGINT=0` in the WASM linker flags, or the generated glue emits a BigInt literal
     that breaks the es2017 esbuild target (CI Build fails with
     `Big integer literals are not available ... es2017`).

5. **Emscripten binds at full arity** — C++ default arguments are NOT preserved; the JS side
   must pass every parameter.

6. **Emscripten does NOT propagate C++ exceptions to JS** — a JS "expect it to throw" test is not
   meaningful for a native `throw`; keep that assertion at the C++ level.

7. **Emscripten cache**: `emcc` needs a writable cache dir; set
   `export EM_CACHE=<writable dir>` (e.g. `$HOME/.em_cache`).

8. **Commits must be DCO-signed**: `git commit -s` (adds `Signed-off-by:`). CI checks are **dco**
   and **build-test-publish** (Build → Lint → Test, JS only).

9. **Leave pre-existing uncommitted changes alone**: `README.md` and `.github/workflows/deploy-docs.yml`
   are modified in the working tree and are NOT part of feature work.

10. **typedoc regen churn**: regenerating `docs/api/` rewrites many unrelated files (line-number /
    git-hash churn). Only commit the files that genuinely changed for the feature.

## Typical workflow for adding a function

1. C++ function in `cpp/src/...` (header + new `.cpp`), register in CMake source list.
2. C++ tests; build + run `runUnitTests`.
3. WASM binding in `wasm.cpp` + `wasm-core.cpp`; rebuild + commit the WASM.
4. JS wrapper in `js/packages/core/src/...`, export in `index.ts`.
5. Update `GeoDaModule` in `js/packages/common/src/wasm/index.d.ts`; rebuild common dist.
6. JS tests; run full jest suite; eslint; esbuild build.
7. Docs (hand-written + typedoc regen); `vitepress build`.
8. DCO-signed commit on a feature branch; push; open PR; watch CI.
