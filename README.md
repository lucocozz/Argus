<p align="center">
  <img src="docs/assets/argus-logo.webp" alt="argus logo" width="200">
</p>

<h1 align="center">argus</h1>

<p align="center">
  <strong>Modern C library for command-line argument parsing with an elegant, macro-based API</strong>
</p>

<p align="center">
  <a href="https://github.com/lucocozz/argus/actions/workflows/ci.yml"><img src="https://github.com/lucocozz/argus/actions/workflows/ci.yml/badge.svg" alt="CI/CD Pipeline"></a>
  <a href="https://github.com/lucocozz/argus/actions/workflows/codeql.yml"><img src="https://github.com/lucocozz/argus/actions/workflows/codeql.yml/badge.svg" alt="CodeQL Analysis"></a>
  <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-blue.svg" alt="License: MIT"></a>
  <a href="https://conan.io/center/libargus"><img src="https://img.shields.io/badge/Conan-package-blue" alt="Conan Package"></a>
  <a href="https://vcpkg.io/en/packages.html"><img src="https://img.shields.io/badge/vcpkg-package-blue" alt="vcpkg Package"></a>
</p>

---

## 📋 Overview

**argus** is a powerful C library that simplifies command-line argument parsing with a modern, expressive API:

```c
// Define options with a clean, declarative syntax
ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_INT('p', "port", HELP("Port number"), RANGE(1, 65535), DEFAULT(8080))
)
```

Designed for both simplicity and flexibility, argus enables developers to create sophisticated command-line interfaces with minimal effort.

## ⚡ Quick Start

### Installation

```bash
# Using package managers
conan install libargus/1.0.1@
vcpkg install libargus

# From source with Meson
git clone https://github.com/lucocozz/argus.git
cd argus
meson setup builddir && meson compile -C builddir
sudo meson install -C builddir
```

### Basic Usage

```c
#include "argus.h"
#include <stdio.h>

// Define options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    POSITIONAL_STRING("input", HELP("Input file"))
)

int main(int argc, char **argv)
{
    // Initialize and parse
    argus_t argus = argus_init(options, "my_program", "1.0.0");
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS) {
        return 1;
    }

    // Access values
    const char *input = argus_get(argus, "input").as_string;
    const char *output = argus_get(argus, "output").as_string;
    bool verbose = argus_get(argus, "verbose").as_bool;

    printf("Input: %s\nOutput: %s\nVerbose: %s\n", 
           input, output, verbose ? "yes" : "no");

    argus_free(&argus);
    return 0;
}
```

## ✨ Key Features

| Feature | Description | Example |
|---------|-------------|---------|
| **Typed Options** | Type-safe options with strong validation | `OPTION_INT('p', "port", RANGE(1, 65535))` |
| **Subcommands** | Git/Docker style nested commands | `SUBCOMMAND("add", add_options, ACTION(add_command))` |
| **Collections** | Arrays and maps for multiple values | `OPTION_ARRAY_INT('n', "nums", FLAGS(FLAG_SORTED))` |
| **Environment Variables** | Auto integration with env vars | `OPTION_STRING('h', "host", ENV_VAR("HOST"))` |
| **Regex Validation** | Pattern-based validation | `OPTION_STRING('e', "email", REGEX(ARGUS_RE_EMAIL))` |
| **Command Abbreviations** | GitLab-style command shortening | `program ins` → `program install` |
| **Flexible Formats** | Support multiple CLI conventions | `--opt=val`, `-o val`, `-oval`, etc. |
| **Auto Documentation** | Generated help & usage text | `--help` generates formatted documentation |

## 📦 Installation Options

### Dependencies

- **PCRE2**: Required only for regex validation support
  - Can be disabled with `-Ddisable_regex=true` option

### Package Managers

#### Conan

```bash
# Basic installation
conan install libargus/1.0.1@

# Without regex support
conan install libargus/1.0.1@ -o libargus:disable_regex=true
```

#### vcpkg

```bash
# Full installation
vcpkg install libargus

# Core functionality only (no regex)
vcpkg install libargus[core]
```

### Build From Source

#### Meson (Recommended)

```bash
git clone https://github.com/lucocozz/argus.git
cd argus
meson setup builddir
meson compile -C builddir
sudo meson install -C builddir  # Optional
```

#### Using Just (Development)

```bash
git clone https://github.com/lucocozz/argus.git
cd argus
just build          # Build libraries
just test           # Run tests
just examples       # Build examples
just install        # Install system-wide
```

#### Installer Script

```bash
# Download and run installer
curl -LO https://github.com/lucocozz/argus/releases/download/v1.0.1/argus-1.0.1.tar.gz
tar -xzf argus-1.0.1.tar.gz
cd argus-1.0.1
./install.sh        # System-wide installation
# or
./install.sh --local # Local installation in ~/.local
```

## 🔥 Advanced Examples

### Environment Variables

```c
ARGUS_OPTIONS(
    options,
    // Auto-generated APP_HOST environment variable
    OPTION_STRING('H', "host", HELP("Server hostname"),
                 FLAGS(FLAG_AUTO_ENV), DEFAULT("localhost")),
    
    // Use specific environment variable with override capability
    OPTION_INT('p', "port", HELP("Server port"), 
               ENV_VAR("SERVICE_PORT"), FLAGS(FLAG_ENV_OVERRIDE))
)

// Set env variables: APP_HOST=example.com SERVICE_PORT=9000
```

### Subcommands with Actions

```c
int add_command(argus_t *argus, void *data) {
    const char* file = argus_get(*argus, "file").as_string;
    bool force = argus_get(*argus, "force").as_bool;
    printf("Adding %s (force: %s)\n", file, force ? "yes" : "no");
    return 0;
}

ARGUS_OPTIONS(
    add_options,
    OPTION_FLAG('f', "force", HELP("Force add operation")),
    POSITIONAL_STRING("file", HELP("File to add"))
)

ARGUS_OPTIONS(
    options,
    SUBCOMMAND("add", add_options, HELP("Add a file"), ACTION(add_command))
)

// Usage: program add --force file.txt
```

### Collection Types

```c
ARGUS_OPTIONS(
    options,
    // Array of integers with sorting and uniqueness
    OPTION_ARRAY_INT('n', "numbers", HELP("List of numbers"),
                   FLAGS(FLAG_SORTED | FLAG_UNIQUE)),
    
    // Map of environment variables
    OPTION_MAP_STRING('e', "env", HELP("Environment variables"),
                     FLAGS(FLAG_SORTED_KEY))
)

// Usage: 
// --numbers=1,2,3,1 -> [1,2,3]
// --env=USER=alice,HOME=/home
```

## 📚 Documentation

For detailed documentation, visit [argus.readthedocs.io](https://argus.readthedocs.io/).

The documentation covers:
- Complete API reference
- Detailed guides for all features
- Advanced usage examples
- Detailed validator documentation
- Best practices

## 🔍 Comparison

| Feature | argus | getopt | argp | argtable3 |
|---------|-------|--------|------|-----------|
| Concise macro API | ✅ | ❌ | ❌ | ❌ |
| Type Safety | ✅ | ❌ | ❌ | ✅ |
| Nested Subcommands | ✅ | ❌ | ❌ | ❌ |
| Environment Variables | ✅ | ❌ | ❌ | ❌ |
| Collections (Arrays/Maps) | ✅ | ❌ | ❌ | ❌ |
| Command Abbreviations | ✅ | ❌ | ❌ | ❌ |
| Regex Validation | ✅ | ❌ | ❌ | ❌ |
| Dependencies | PCRE2 (optional) | libc | GNU | None |
| Learning Curve | Low | Medium | High | Medium |

## 🚀 Roadmap

- 📄 **Config files**: JSON/YAML config loading
- 🪶 **Lightweight version**: Minimal footprint option
- 🎨 **Themed help**: Customizable colored help
- 📁 **Shell completion**: Auto-generated tab completion
- 🔗 **Alias support**: Command and option aliases
- 📦 **Plugin system**: Extensibility mechanisms

## 👥 Contributing

Contributions welcome! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## 📜 License

MIT License - See [LICENSE](LICENSE) for details.

---

<p align="center">
  <i>Built with ❤️ by <a href="https://github.com/lucocozz">lucocozz</a></i>
</p>
