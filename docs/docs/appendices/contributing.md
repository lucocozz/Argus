# Contributing to Argus

Thank you for your interest in contributing to Argus! This guide covers how to build, test, and contribute to the project.

## Quick Start

```bash
# Clone and build
git clone https://github.com/lucocozz/argus.git
cd argus
meson setup build
meson compile -C build

# Run tests
meson test -C build
```

## Development Setup

### Prerequisites

- **C11 compiler**: GCC 13+ or Clang 14+
- **Meson**: 1.1.0+
- **Ninja**: Recommended backend
- **PCRE2**: Optional for regex support
- **Git**: For version control

### Building from Source

```bash
# Basic build
meson setup build
meson compile -C build

# Development build with tests
meson setup build -Dtests=true -Dexamples=true
meson compile -C build

# Debug build
meson setup build --buildtype=debug -Db_coverage=true
```

## Project Structure

```
argus/
├── source/            # Library source code
│   ├── api/           # Public API functions
│   ├── core/          # Core parsing logic
│   ├── callbacks/     # Handlers and validators
│   ├── utils/         # Utility functions
│   └── display/       # Help and error display
├── includes/          # Public headers
│   ├── argus.h        # Main header
│   └── argus/         # API headers
├── tests/             # Test suite
├── examples/          # Example programs
├── benchmarks/        # Performance tests
└── docs/              # Documentation
```

## Making Changes

### Coding Standards

- **C11 standard** with GNU extensions
- **4-space indentation**, no tabs
- **Snake_case** for functions and variables
- **UPPER_CASE** for constants and macros
- **Line length**: 100 characters max

### Code Formatting

```bash
# Format all source files
just format
```

### Testing

```bash
# Run all tests
meson test -C build

# Run specific test suites
meson test -C build --suite unit
meson test -C build --suite integration

# Run with coverage
meson test -C build -Db_coverage=true
ninja -C build coverage
```

## Contribution Types

### Bug Fixes

1. Create issue describing the bug
2. Create feature branch: `git checkout -b fix/issue-description`
3. Write failing test that reproduces the bug
4. Fix the bug
5. Ensure all tests pass
6. Submit pull request

### New Features

1. Discuss feature in GitHub issue first
2. Create feature branch: `git checkout -b feature/feature-name`
3. Implement feature with tests
4. Update documentation
5. Submit pull request

### Documentation

- Update relevant `.md` files in `docs/`
- Include code examples
- Test examples compile and run
- Keep language clear and concise

## Pull Request Process

1. **Fork** the repository
2. **Create branch** from `main`
3. **Make changes** following coding standards
4. **Add tests** for new functionality
5. **Update docs** if needed
6. **Run tests**: `meson test -C build`
7. **Submit PR** with clear description

### PR Requirements

- [ ] All tests pass
- [ ] Code formatted with clang-format
- [ ] Documentation updated
- [ ] Examples work correctly
- [ ] No memory leaks (check with valgrind)

## Development Workflow

### Using Justfile

```bash
# Build and test
just build
just test

# Run specific test
just test-one unit_strings

# Format and lint
just format
just lint
```

### Manual Commands

```bash
# Configure build
meson setup build -Dtests=true

# Compile
meson compile -C build

# Test with verbose output
meson test -C build -v

# Install locally
meson install -C build --destdir /tmp/argus-install
```

## Adding New Features

### New Option Types

1. Add type to `argus_valtype_t` enum
2. Create handler function in `source/callbacks/handlers/`
3. Add handler macro to `includes/argus/options.h`
4. Write tests in `tests/`
5. Add documentation example

### New Validators

1. Create validator function in `source/callbacks/validators/`
2. Add validator macro to `includes/argus/options.h`
3. Write comprehensive tests
4. Document usage patterns

### API Changes

- Maintain backward compatibility
- Update version numbers appropriately
- Document breaking changes clearly
- Provide migration examples

## Testing Guidelines

### Unit Tests

```c
// Example test structure
void test_option_parsing(void)
{
    ARGUS_OPTIONS(options,
        OPTION_STRING('o', "output", HELP("Output"))
    );
    
    argus_t argus = argus_init(options, "test", "1.0.0");
    
    char *argv[] = {"test", "--output", "file.txt"};
    assert(argus_parse(&argus, 3, argv) == ARGUS_SUCCESS);
    
    const char *output = argus_get(&argus, "output").as_string;
    assert(strcmp(output, "file.txt") == 0);
    
    argus_free(&argus);
}
```

### Integration Tests

- Test complete CLI scenarios
- Verify error messages
- Check help output formatting
- Test environment variable integration

## Performance Considerations

- Minimize memory allocations
- Use efficient string operations
- Profile with representative workloads

## Common Issues

### Build Failures

```bash
# Clear build directory
rm -rf build
meson setup build

# Check dependencies
pkg-config --exists libpcre2-8
```

### Test Failures

```bash
# Run specific failing test
meson test -C build test_name --verbose

# Debug with gdb
gdb build/tests/test_binary
```

### Memory Issues

```bash
# Check for leaks
valgrind --leak-check=full build/tests/test_binary

# Static analysis
clang-static-analyzer source/*.c
```

## Getting Help

- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: Questions and general discussion
- **Documentation**: Check existing docs first

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

**Key Areas for Contribution:**

- Performance optimizations
- Platform compatibility
- Documentation improvements
- New feature implementations
- Bug fixes and stability improvements
- Test coverage enhancements
