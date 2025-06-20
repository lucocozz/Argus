const config = {
  title: 'Argus',
  tagline: 'Modern C library for command-line argument parsing',
  favicon: 'img/argus-logo.webp',

  url: 'https://argus-lib.netlify.app',
  baseUrl: '/',

  organizationName: 'lucocozz',
  projectName: 'argus',

  onBrokenLinks: 'throw',
  onBrokenMarkdownLinks: 'warn',
  i18n: {
    defaultLocale: 'en',
    locales: ['en'],
  },

  presets: [
    [
      'classic',
      {
        docs: {
          routeBasePath: '/',
          sidebarPath: require.resolve('./sidebars.js'),
          editUrl: 'https://github.com/lucocozz/argus/tree/main/docs/',
          showLastUpdateTime: true,
          showLastUpdateAuthor: true,
        },
        blog: false,
        theme: {
          customCss: require.resolve('./src/css/custom.css'),
        },
      },
    ],
  ],

  themeConfig: {
    navbar: {
      title: '',
      logo: {
        alt: 'Argus Logo',
        src: 'img/argus-title-light.webp',
        srcDark: 'img/argus-title.webp',
        width: 80,
        height: 80,
      },
      items: [
        {
          to: '/getting-started/intro',
          label: 'Get Started',
          position: 'left',
        },
        {
          to: '/api-reference/overview',
          label: 'API',
          position: 'left',
        },
        {
          to: '/examples/simple-cli',
          label: 'Examples',
          position: 'left',
        },
        {
          href: 'https://github.com/lucocozz/argus',
          label: 'GitHub',
          position: 'right',
        },
      ],
    },
    footer: {
      style: 'dark',
      links: [
        {
          title: 'Learn',
          items: [
            {
              label: 'Getting Started',
              to: '/getting-started/intro',
            },
            {
              label: 'Tutorial',
              to: '/getting-started/quickstart',
            },
          ],
        },
        {
          title: 'Reference',
          items: [
            {
              label: 'API Reference',
              to: '/api-reference/overview',
            },
            {
              label: 'Examples',
              to: '/examples/simple-cli',
            },
          ],
        },
        {
          title: 'Community',
          items: [
            {
              label: 'GitHub',
              href: 'https://github.com/lucocozz/argus',
            },
            {
              label: 'Issues',
              href: 'https://github.com/lucocozz/argus/issues',
            },
          ],
        },
      ],
      copyright: `Copyright © ${new Date().getFullYear()} lucocozz. Built with Docusaurus.`,
    },
    prism: {
      theme: require('prism-react-renderer').themes.github,
      darkTheme: require('prism-react-renderer').themes.dracula,
      additionalLanguages: ['c', 'bash'],
    },
    colorMode: {
      defaultMode: 'dark',
      respectPrefersColorScheme: true,
    },
    algolia: {
      appId: 'Z686PG1DUX',
      apiKey: '8a592f2cd2ef4e5c88c46aca825d1a3e',
      indexName: 'argus-lib',
      contextualSearch: true,
      searchPagePath: 'search',
      placeholder: 'Search docs...',
    },
  },
};

module.exports = config;
