#!/usr/bin/env node
/**
 * analyze.mjs — standalone node driver for GeoDa spatial analysis.
 *
 * Bundled with the geoda-analysis skill. Loads @geoda/core | lisa | regression
 * and runs one operation per invocation, reading a GeoJSON FeatureCollection
 * (variables in `properties`) and writing a JSON result to stdout or --out.
 *
 * Usage: node analyze.mjs <op> [options]     (see `--help`)
 *
 * Dependency resolution: the script loads @geoda/* via createRequire from
 * `GEODA_PKG_DIR` (default: this script's own directory). Install with:
 *   cd <skill dir> && npm i @geoda/core @geoda/lisa @geoda/regression
 */
import {createRequire} from 'node:module';
import {readFileSync, writeFileSync} from 'node:fs';
import {join, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';

const __dirname = dirname(fileURLToPath(import.meta.url));
const pkgDir = process.env.GEODA_PKG_DIR || __dirname;
const require = createRequire(join(pkgDir, 'noop.js'));

let _geoda = null;
function geoda() {
  if (_geoda) return _geoda;
  try {
    _geoda = {
      core: require('@geoda/core'),
      lisa: require('@geoda/lisa'),
      regression: require('@geoda/regression'),
    };
  } catch (err) {
    console.error(
      `Failed to load @geoda/* packages. Install them first:\n` +
        `  cd ${pkgDir}\n  npm i @geoda/core @geoda/lisa @geoda/regression\n` +
        `or set GEODA_PKG_DIR to a directory whose node_modules contains them.\n\n${err.message}`
    );
    process.exit(2);
  }
  return _geoda;
}

// --- tiny arg parser -------------------------------------------------------
function parseArgs(argv) {
  const opts = {independent: []};
  for (let i = 0; i < argv.length; i++) {
    const a = argv[i];
    if (!a.startsWith('--')) continue;
    const key = a.slice(2);
    const next = argv[i + 1];
    const hasValue = next !== undefined && !next.startsWith('--');
    if (key === 'independent') opts.independent.push(hasValue ? next : '');
    else opts[key] = hasValue ? next : true;
    if (hasValue) i++;
  }
  return opts;
}

const HELP = `analyze.mjs — GeoDa spatial analysis in node

Usage: node analyze.mjs <op> [options]

Global:
  --file <geojson>      Input GeoJSON FeatureCollection (variables in properties)
  --out <file>          Write JSON result to file instead of stdout
  --weights <file.json> Reuse a saved neighbor list (from the weights op)
  --weights-type <t>    Build weights on the fly: queen | rook | knn | threshold
  --pkg-dir <dir>       Where @geoda/* is installed (or set GEODA_PKG_DIR)

Ops:
  weights       Build spatial weights: [--weights-type] [--k N] [--order N]
                [--include-lower] [--threshold N] [--is-mile] [--use-centroids]
                [--precision-threshold N]
  lisa          LISA: --variable V --method localMoran|localGeary|localG|
                localGStar|quantileLisa [--k N --quantile N]
                [--permutation N] [--cutoff N]
  global-moran  Global Moran's I: --variable V [--permutation N]
  colocation    Local Join Count: --variable A [--variable-b B]
                (univariate when B omitted, bivariate otherwise). Binary 0/1 only.
                [--permutation N] [--cutoff N]
  regression    --dependent Y --independent X1 [--independent X2 ...]
                --model classic|spatial-lag|spatial-error
  classify      --variable V --method quantile|naturalBreaks|equalInterval|
                percentile|stddev|hinge15 [--k N]
  rate          --event E --base B --method rawRates|excessRisk|empiricalBayes
  standardize   --variable V --method standardize|standardizeMAD|
                deviationFromMean|rangeAdjust|rangeStandardize
  thiessen      Thiessen polygons -> --out out.geojson
  mst           Minimum Spanning Tree edges -> --out out.geojson
  cartogram     --weight W [--iterations N] -> --out out.geojson

Every statistical op needs weights: pass --weights saved.json or --weights-type.
`;

// --- data helpers ----------------------------------------------------------
function readFeatures(file) {
  if (!file) throw new Error('--file <geojson> is required');
  const raw = JSON.parse(readFileSync(file, 'utf8'));
  const features =
    raw.type === 'FeatureCollection' ? raw.features : raw.type === 'Feature' ? [raw] : raw.features;
  if (!Array.isArray(features) || !features.length) {
    throw new Error(`no features found in ${file}; expected a GeoJSON FeatureCollection`);
  }
  return features;
}

function variable(features, name) {
  if (!name) throw new Error('--variable <name> is required');
  const values = features.map((f) => f.properties?.[name]);
  if (values.some((v) => v === undefined || v === null)) {
    throw new Error(`variable "${name}" missing from some features`);
  }
  const nums = values.map(Number);
  if (nums.some(Number.isNaN)) throw new Error(`variable "${name}" is not numeric`);
  return nums;
}

function num(opts, key, fallback) {
  const v = opts[key];
  return v === undefined || v === true ? fallback : Number(v);
}

function flag(opts, key) {
  const v = opts[key];
  return v === true || v === 'true' || v === '1';
}

// --- weights ---------------------------------------------------------------
async function resolveWeights(core, features, opts) {
  if (opts.weights) {
    return JSON.parse(readFileSync(opts.weights, 'utf8'));
  }
  const type = opts['weights-type'] || 'queen';
  return core.createWeights({
    weightsType: type,
    isQueen: type === 'queen',
    k: opts.k === undefined ? undefined : Number(opts.k),
    distanceThreshold: opts.threshold === undefined ? undefined : Number(opts.threshold),
    isMile: flag(opts, 'is-mile'),
    useCentroids: flag(opts, 'use-centroids'),
    precisionThreshold: num(opts, 'precision-threshold', undefined),
    orderOfContiguity: opts.order === undefined ? undefined : Number(opts.order),
    includeLowerOrder: flag(opts, 'include-lower'),
    geometries: features,
  });
}

// --- result printing -------------------------------------------------------
function finish(result, opts) {
  const text = JSON.stringify(result, null, 2);
  if (opts.out) writeFileSync(opts.out, text + '\n');
  else process.stdout.write(text + '\n');
  return result;
}

function summarizeClusters(labels, colors, clusters) {
  const counts = new Map();
  (clusters ?? []).forEach((c, i) => counts.set(c, (counts.get(c) ?? 0) + 1));
  return (labels ?? []).map((label, i) => ({
    value: i,
    label,
    color: (colors ?? [])[i],
    numberOfObservations: counts.get(i) ?? 0,
  }));
}

// --- operations ------------------------------------------------------------
async function opWeights(core, features, opts) {
  const w = await resolveWeights(core, features, {...opts, weights: undefined});
  return {weights: w.weights, weightsMeta: w.weightsMeta};
}

async function opLisa(g, features, opts) {
  const data = variable(features, opts.variable);
  const weights = await resolveWeights(g.core, features, opts);
  const base = {
    data,
    neighbors: weights.weights,
    permutation: num(opts, 'permutation', 999),
    significanceCutoff: num(opts, 'cutoff', 0.05),
  };
  const method = opts.method || 'localMoran';
  let lm;
  if (method === 'localGeary') lm = await g.lisa.localGeary(base);
  else if (method === 'localG') lm = await g.lisa.localG(base);
  else if (method === 'localGStar') lm = await g.lisa.localGStar(base);
  else if (method === 'quantileLisa')
    lm = await g.lisa.quantileLisa({...base, k: num(opts, 'k', 4), quantile: num(opts, 'quantile', 4)});
  else lm = await g.lisa.localMoran(base);
  return {
    method,
    lisaValues: lm.lisaValues,
    pValues: lm.pValues,
    clusters: lm.clusters,
    sigCategories: lm.sigCategories,
    clusterColorAndLabels: summarizeClusters(lm.labels, lm.colors, lm.clusters),
    totalObservations: data.length,
  };
}

async function opGlobalMoran(g, features, opts) {
  const data = variable(features, opts.variable);
  const weights = await resolveWeights(g.core, features, opts);
  const lag = await g.lisa.spatialLag(data, weights.weights);
  const n = data.length;
  const mean = data.reduce((a, b) => a + b, 0) / n;
  let numerator = 0;
  let denom = 0;
  for (let i = 0; i < n; i++) {
    const dx = data[i] - mean;
    const dy = lag[i] - mean;
    numerator += dx * dy;
    denom += dx * dx;
  }
  return {globalMoranI: denom > 0 ? numerator / denom : 0, totalObservations: n};
}

async function opColocation(g, features, opts) {
  if (typeof g.lisa.localJoinCount !== 'function') {
    throw new Error(
      'colocation (Local Join Count) needs @geoda/lisa >= 0.0.24. This plugin pins ' +
        '@geoda/* >= 0.0.24; run `npm install` inside skills/geoda-analysis if the ' +
        'locally installed @geoda/lisa is older.'
    );
  }
  const weights = await resolveWeights(g.core, features, opts);
  const perm = num(opts, 'permutation', 999);
  const cutoff = num(opts, 'cutoff', 0.05);
  let lm;
  if (opts['variable-b']) {
    const a = variable(features, opts.variable);
    const b = variable(features, opts['variable-b']);
    lm = await g.lisa.localBiJoinCount({data: [a, b], neighbors: weights.weights, permutation: perm, significanceCutoff: cutoff});
  } else {
    const a = variable(features, opts.variable);
    lm = await g.lisa.localJoinCount({data: a, neighbors: weights.weights, permutation: perm, significanceCutoff: cutoff});
  }
  return {
    type: opts['variable-b'] ? 'bivariate-local-join-count' : 'local-join-count',
    variables: opts['variable-b'] ? [opts.variable, opts['variable-b']] : [opts.variable],
    lisaValues: lm.lisaValues,
    pValues: lm.pValues,
    clusters: lm.clusters,
    sigCategories: lm.sigCategories,
    clusterColorAndLabels: summarizeClusters(lm.labels, lm.colors, lm.clusters),
    totalObservations: weights.weights.length,
  };
}

async function opRegression(g, features, opts) {
  const y = variable(features, opts.dependent);
  const xNames = opts.independent.filter(Boolean);
  if (!xNames.length) throw new Error('--independent <name> (at least one) is required');
  const x = xNames.map((n) => variable(features, n));
  const model = opts.model || 'classic';
  const weights = model === 'classic' ? undefined : await resolveWeights(g.core, features, opts);
  const props = {
    x,
    y,
    xNames,
    yName: opts.dependent,
    datasetName: opts.file ? opts.file.replace(/\.[^.]+$/, '') : 'dataset',
    ...(weights ? {weights: weights.weights} : {}),
  };
  let r;
  if (model === 'spatial-lag') r = await g.regression.spatialLagRegression(props);
  else if (model === 'spatial-error') r = await g.regression.spatialError(props);
  else r = await g.regression.linearRegression(props);
  return {model, result: r};
}

async function opClassify(g, features, opts) {
  const data = variable(features, opts.variable);
  const method = opts.method || 'quantile';
  const k = num(opts, 'k', 5);
  let breaks;
  if (method === 'naturalBreaks') breaks = await g.core.naturalBreaks(k, data);
  else if (method === 'equalInterval') breaks = await g.core.equalIntervalBreaks(k, data);
  else if (method === 'percentile') breaks = await g.core.percentileBreaks(data);
  else if (method === 'stddev') breaks = await g.core.standardDeviationBreaks(data);
  else if (method === 'hinge15') breaks = await g.core.hinge15Breaks(data);
  else breaks = await g.core.quantileBreaks(k, data);
  return {method, breaks};
}

async function opRate(g, features, opts) {
  const base = variable(features, opts.base);
  const event = variable(features, opts.event);
  const method = opts.method || 'excessRisk';
  let result;
  if (method === 'rawRates') result = g.core.rawRates(base, event);
  else if (method === 'empiricalBayes') result = g.core.empiricalBayes(base, event);
  else result = g.core.excessRisk(base, event);
  return {method, result, eventVariable: opts.event, baseVariable: opts.base};
}

async function opStandardize(g, features, opts) {
  const data = variable(features, opts.variable);
  const method = opts.method || 'standardize';
  let result;
  if (method === 'standardizeMAD') result = await g.core.standardizeMAD(data);
  else if (method === 'deviationFromMean') result = await g.core.deviationFromMean(data);
  else if (method === 'rangeAdjust') result = await g.core.rangeAdjust(data);
  else if (method === 'rangeStandardize') result = await g.core.rangeStandardize(data);
  else result = await g.core.standardize(data);
  return {method, result};
}

async function opGeometry(g, features, opts, kind) {
  let featuresOut;
  if (kind === 'thiessen') featuresOut = await g.core.getThiessenPolygons({geoms: features});
  else if (kind === 'mst') featuresOut = await g.core.getMinimumSpanningTree({geoms: features});
  else {
    const wv = variable(features, opts.weight);
    const iterations = num(opts, 'iterations', 100);
    featuresOut = await g.core.getCartogram(features, wv, iterations);
  }
  // Emit a proper FeatureCollection so --out produces a loadable GeoJSON file.
  return {type: 'FeatureCollection', features: featuresOut};
}

// --- main ------------------------------------------------------------------
async function main() {
  const argv = process.argv.slice(2);
  const [op, ...rest] = argv;
  if (!op || op === '--help' || op === '-h') {
    process.stdout.write(HELP);
    return;
  }
  const opts = parseArgs(rest);
  const g = geoda();

  let result;
  try {
    const features = readFeatures(opts.file);
    switch (op) {
      case 'weights': result = await opWeights(g.core, features, opts); break;
      case 'lisa': result = await opLisa(g, features, opts); break;
      case 'global-moran': result = await opGlobalMoran(g, features, opts); break;
      case 'colocation': result = await opColocation(g, features, opts); break;
      case 'regression': result = await opRegression(g, features, opts); break;
      case 'classify': result = await opClassify(g, features, opts); break;
      case 'rate': result = await opRate(g, features, opts); break;
      case 'standardize': result = await opStandardize(g, features, opts); break;
      case 'thiessen': result = await opGeometry(g, features, opts, 'thiessen'); break;
      case 'mst': result = await opGeometry(g, features, opts, 'mst'); break;
      case 'cartogram': result = await opGeometry(g, features, opts, 'cartogram'); break;
      default:
        console.error(`Unknown op "${op}". Run with --help for usage.`);
        process.exit(1);
    }
    finish(result, opts);
  } catch (err) {
    console.error(`Error: ${err.message}`);
    process.exit(1);
  }
}

main();
