import { defineConfig } from 'vitepress';
import { generateApiReferences } from './buildApiOverview.mts';

// https://vitepress.dev/reference/site-config
export default defineConfig({
  title: 'GeoDaLib',
  description: 'a modern Javascript library for spatial data analysis',
  base: '/geodalib/',
  markdown: {
    math: true,
  },
  lastUpdated: true,
  ignoreDeadLinks: true,
  async buildEnd() {
    await generateApiReferences();
  },
  themeConfig: {
    search: {
      provider: 'local',
    },

    // https://vitepress.dev/reference/default-theme-config
    nav: [{ text: 'Home', link: '/' }],

    sidebar: [
      {
        text: 'Developer Guide',
        items: [
          { text: 'Get Started', link: '/get-started' },
          { text: 'Architecture', link: '/architecture' },
          { text: 'Adding a New Function', link: '/adding-a-new-function' },
        ],
      },
      {
        text: 'API Reference',
        items: [
          { text: 'Mapping', link: '/reference/mapping' },
          { text: 'Data Exploration', link: '/reference/data-exploration' },
          { text: 'Spatial Weights', link: '/reference/spatial-weights' },
          {
            text: 'Spatial Autocorrelation Analysis',
            link: '/reference/spatial-autocorrelation-analysis',
          },
          { text: 'Spatial Clustering', link: '/reference/spatial-clustering' },
          { text: 'Spatial Regression', link: '/reference/spatial-regression' },
          { text: 'Spatial Operations', link: '/reference/spatial-operations' },
        ],
      },
      {
        text: 'TypeScript API',
        link: '/api/globals',
      },
    ],

    socialLinks: [
      { icon: 'github', link: 'https://github.com/geodaopenjs/geodalib' },
      {
        icon: 'linkedin',
        link: 'https://www.linkedin.com/company/center-for-spatial-data-science',
      },
      {
        icon: 'x',
        link: 'https://x.com/GeoDaCenter',
      },
    ],
  },
});
