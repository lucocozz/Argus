# Contributing to argus

Thank you for your interest in contributing to argus! We welcome contributions from everyone, whether it's in the form of bug reports, feature requests, documentation improvements, or code contributions.

## Contribution Workflow

We use the "Issue First" approach to manage contributions:

1. **Create an Issue First**: Before submitting a Pull Request, please create an issue to discuss the change you'd like to make:
   - For bugs, create a Bug Report
   - For new features, create a Feature Request
   - For questions or discussions, use GitHub Discussions

2. **Wait for Feedback**: Wait for project maintainers and community members to discuss the issue.

3. **Implement Changes**: Once there's agreement on how to proceed, implement your changes.

4. **Submit a Pull Request**: Reference the issue in your PR using the format "Fixes #ISSUE_NUMBER".

## Pull Request Guidelines

1. **One PR per issue**: Keep your changes focused and limited to addressing a single issue.

2. **Follow coding style**: Follow the coding style used throughout the project (use clang-format).

3. **Write tests**: Add tests to cover your changes where applicable.

4. **Update documentation**: Keep documentation up-to-date with your changes.

5. **Keep PRs small**: Smaller, focused PRs are easier to review and more likely to be accepted.

## Development Setup

To set up a development environment:

1. **Fork and clone the repository**:
   ```bash
   git clone https://github.com/YOUR_USERNAME/argus.git
   cd argus
   ```

2. **Install dependencies**:
   ```bash
   # For Ubuntu/Debian
   sudo apt-get install meson ninja-build just libpcre2-dev clang-format clang-tidy libcriterion-dev
   
   # For macOS
   brew install meson ninja just pcre2 clang-format clang-tidy criterion
   ```

3. **Build the project**:
   ```bash
   meson setup .build
   meson compile -C .build
   ```

4. **Run tests**:
   ```bash
   meson test -C .build
   ```

## Using the Justfile

This project includes a `Justfile` to simplify common tasks. If you have `just` installed, you can:

```bash
# Build and compile
just build

# Run tests
just test

# Format code
just format

# Check code quality
just check
```

## Code of Conduct

Please adhere to our code of conduct when participating in this project. Be respectful and considerate of others.

## Questions?

If you have any questions, feel free to open a discussion on GitHub or ask in an issue.

Thank you for contributing to argus! 🎉
