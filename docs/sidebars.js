/** @type {import('@docusaurus/plugin-content-docs').SidebarsConfig} */
const sidebars = {
  tutorialSidebar: [
    {
      type: 'category',
      label: '🚀 Getting Started',
      collapsed: false,
      items: [
        'getting-started/intro',
        'getting-started/installation',
        'getting-started/quickstart',
      ],
    },
    {
      type: 'category',
      label: '📖 Fundamentals',
      items: [
        'fundamentals/basic-options',
        'fundamentals/option-formats',
        'fundamentals/accessing-values',
        'fundamentals/help-and-errors',
      ],
    },
    {
      type: 'category',
      label: '🔧 Core Features',
      items: [
        'features/validation',
        'features/environment',
        'features/subcommands',
        'features/collections',
      ],
    },
    {
      type: 'category',
      label: '⚡ Advanced Features',
      items: [
        'advanced/custom-handlers',
        'advanced/custom-validators',
      ],
    },
    {
      type: 'category',
      label: '🎯 Examples',
      items: [
        'examples/simple-cli',
        'examples/configuration-tool',
        'examples/git-like',
      ],
    },
    {
      type: 'category',
      label: '📚 API Reference',
      items: [
        'api-reference/overview',
        'api-reference/regex-patterns',
        'api-reference/error-codes',
      ],
    },
    {
      type: 'category',
      label: '📋 Appendices',
      items: [
        'appendices/cheat-sheet',
        'appendices/contributing',
      ],
    }
  ],
};

module.exports = sidebars;
