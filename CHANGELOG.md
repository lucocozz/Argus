# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Printing subcommand list on command execution that cannot be executed directly.
- Support for variadic positional arguments, allowing multiple values for a single positional argument.

### Changed
- Changed help output DEFAULT_MAX_LINE_WIDTH from 80 to 100.
- Changed `ARGUS_RELEASE` into `ARGUS_DEBUG`

### Fixed
- Disabled pedantic option on library compilation.
- Defining `argus_init()` as a `static inline` function.
- Fixed unexecuted validators in subcommand.

### Removed
- Group description option.
- Cleanup non essential regex patterns.


## [0.1.0] - 2025-07-02

### Added
- **Core Features**
  - Modern C library for command-line argument parsing
  - Declarative API for defining command-line options
  - Type-safe option handling (string, int, float, bool, flag)
  - Comprehensive validation system with built-in validators
  - Multi-value support for arrays and key-value maps
  - Nested subcommand support with inheritance
  - Environment variable integration
  - Smart help generation with automatic formatting
  - Version display functionality

- **Option Types**
  - String options with length and regex validation
  - Integer options with range validation
  - Float options with range validation  
  - Boolean options with choice validation
  - Flag options for simple toggles
  - Array options for multiple values
  - Map options for key-value pairs

- **Validation System**
  - Range validators for numeric types
  - Length validators for strings
  - Regex validators for pattern matching
  - Choice validators for predefined values
  - Count validators for array/map sizes
  - Custom validator support

- **Advanced Features**
  - Subcommand system with nesting support
  - Environment variable binding
  - Custom handlers and validators
  - Automatic help and usage generation
  - Smart hints for invalid inputs
  - Cross-platform support (Linux, macOS, Windows)

- **Build System & Packaging**
  - Modern Meson build system
  - Multi-architecture support (x86, x64)
  - Package manager integration (Conan, vcpkg, xmake, WrapDB)
  - pkg-config support
  - Both static and shared library builds

- **Documentation & Examples**
  - Comprehensive documentation website at argus-lib.com
  - 11 example programs covering all features
  - API reference documentation
  - Getting started guides and tutorials

- **Quality Assurance**
  - 21 comprehensive test suites using Criterion framework
  - GitHub Actions CI/CD for multiple platforms
  - Code formatting with clang-format
  - Static analysis with clang-tidy
  - Automated release workflow

- **Developer Experience**
  - Justfile for convenient build automation
  - Professional code quality tooling
  - Consistent coding standards
  - MIT license with proper headers

### Technical Highlights
- **Memory Management**: Safe memory handling with proper cleanup
- **Error Handling**: Comprehensive error reporting and validation
- **Performance**: Optimized parsing with minimal overhead
- **Portability**: Cross-platform compatibility layer
- **Extensibility**: Plugin system for custom handlers and validators

### Dependencies
- **Required**: C11 compiler
- **Optional**: PCRE2 library (for regex support, can be disabled)

### Platform Support
- Linux (tested on Ubuntu with GCC, Clang)
- macOS (tested with Apple Clang)
- Windows (tested with MSVC, MinGW)
- Both x86 and x64 architectures

### Breaking Changes
- This is the initial release, no breaking changes apply

### Security
- All user inputs are validated and sanitized
- No known security vulnerabilities
- Proper bounds checking and memory safety
- Security policy established for vulnerability reporting

---

## Release Notes

### v0.1.0 - Initial Public Release

This marks the first stable release of Argus, a modern C library for command-line argument parsing. After extensive development and testing, we're excited to offer developers a declarative, type-safe alternative to traditional argument parsing libraries.

**Key Benefits:**
- 🎯 **Type Safety**: Strong typing with automatic validation
- 🚀 **Performance**: Fast parsing with minimal memory footprint  
- 🛠️ **Developer Experience**: Intuitive API that reduces boilerplate
- 📚 **Comprehensive**: Supports all common CLI patterns and more
- 🔧 **Extensible**: Custom handlers and validators for specific needs

The library is production-ready and includes comprehensive documentation, examples, and automated testing across multiple platforms.

[0.1.0]: https://github.com/lucocozz/argus/releases/tag/v0.1.0
[Unreleased]: https://github.com/lucocozz/argus/compare/v0.1.0...HEAD