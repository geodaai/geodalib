// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import esbuild from 'esbuild';
import fs from 'fs';

const isDev = process.argv.includes('--dev');

// Create base configuration factory
export const createBaseConfig = (options = {}) => {
  return {
    bundle: true,
    minify: !isDev,
    sourcemap: true,
    metafile: true,
    target: ['esnext'],
    format: 'esm',
    platform: 'neutral',
    ...options,
  };
};

// Build function for different formats
//
// Formats:
//  - 'cjs'      — Node CommonJS (platform: 'node', native fs/path/process).
//  - 'node-esm' — Node ESM (platform: 'node', native builtins). Same native
//                 environment as CJS but ESM output, so Node ESM consumers
//                 (`import` in Node) get working fs/path instead of the
//                 browser polyfill stubs.
//  - 'esm'      — browser ESM (platform: 'neutral' + node-polyfills): the
//                 glue's require("fs")/require("path") are replaced with the
//                 empty `{}` stubs so browser bundlers can resolve the build;
//                 WASM loads from the jsdelivr CDN at runtime.
//
// The node-targeting builds (cjs, node-esm) resolve fs/path/process/__dirname
// natively, so the polyfills must not be applied there. Only the browser ESM
// build needs them.
export const buildFormat = async (config, format, outfile) => {
  const isNodeBuild = format === 'cjs' || format === 'node-esm';
  const esmFormat = format === 'node-esm' ? 'esm' : format;
  const plugins = isNodeBuild
    ? config.plugins?.filter((plugin) => plugin?.name !== 'node-polyfills')
    : config.plugins;

  const result = await esbuild.build({
    ...config,
    plugins,
    format: esmFormat,
    ...(isNodeBuild ? { platform: 'node' } : {}),
    ...(format === 'cjs' ? { target: ['es2017'] } : {}),
    // The node-esm build needs a real import.meta.url (the banner below uses it
    // to derive __dirname/require); the browser ESM and CJS builds don't, so pin
    // it to a placeholder to keep module-loading behavior consistent.
    define: {
      ...config.define,
      ...(format === 'node-esm'
        ? {}
        : {
            // Ensure consistent module loading behavior
            'import.meta.url': 'undefined',
          }),
    },
    // esbuild does not shim __dirname/__filename/require for ESM output, but the
    // Emscripten glue and init.ts reference them (the glue calls require("fs")
    // dynamically). Inject real shims via a banner for the Node ESM build only.
    ...(format === 'node-esm'
      ? {
          banner: {
            js: `import { createRequire } from 'module';
import { fileURLToPath } from 'url';
import path from 'path';
const require = createRequire(import.meta.url);
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);`,
          },
        }
      : {}),
    ...(outfile ? { outfile } : {}),
  });

  const metaFile = outfile ? outfile.replace(/\.(js|mjs|cjs)$/, '.meta.json') : 'dist/meta.json';
  if (isDev) {
    fs.writeFileSync(metaFile, JSON.stringify(result.metafile));
  }

  console.log(`${format.toUpperCase()} build complete! ✨`);

  return result;
};
