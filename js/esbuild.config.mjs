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
export const buildFormat = async (config, format, outfile) => {
  // The node-polyfills plugin (esbuild-plugin-polyfill-node) defaults
  // polyfills.fs/path to empty objects, so it turns the Emscripten glue's
  // require("fs")/require("path") into `{}` and injects a fake __dirname,
  // which makes initWASM() fail in Node ("readFileSync is not a function").
  // The CJS build targets Node (platform: 'node'), where fs/path/process/
  // __dirname are native, so the polyfills must not be applied there. Only
  // the ESM build (browser/CDN) needs them.
  const plugins =
    format === 'cjs'
      ? config.plugins?.filter((plugin) => plugin?.name !== 'node-polyfills')
      : config.plugins;

  const result = await esbuild.build({
    ...config,
    plugins,
    format,
    ...(format === 'cjs' ? { platform: 'node', target: ['es2017'] } : {}),
    define: {
      ...config.define,
      // Ensure consistent module loading behavior
      'import.meta.url': 'undefined',
    },
    ...(outfile ? { outfile } : {}),
  });

  const metaFile = outfile ? outfile.replace(/\.(js|cjs)$/, '.meta.json') : 'dist/meta.json';
  if (isDev) {
    fs.writeFileSync(metaFile, JSON.stringify(result.metafile));
  }

  console.log(`${format.toUpperCase()} build complete! ✨`);

  return result;
};
