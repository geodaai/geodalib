// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

import { dtsPlugin } from 'esbuild-plugin-d.ts';
import { createBaseConfig, buildFormat } from '../../esbuild.config.mjs';
import { polyfillNode } from 'esbuild-plugin-polyfill-node';

const baseConfig = createBaseConfig({
  target: ['es2020'],
  mainFields: ['module', 'main'],
  external: ['fs', 'path'],
  entryPoints: ['src/index.ts'],
  plugins: [dtsPlugin(), polyfillNode()],
});

// Build all formats
Promise.all([
  buildFormat(baseConfig, 'esm', 'dist/index.js'),
  buildFormat(baseConfig, 'node-esm', 'dist/index.node.mjs'),
  buildFormat(baseConfig, 'cjs', 'dist/index.cjs'),
]).catch(error => {
  console.error(error);
  process.exit(1);
});
