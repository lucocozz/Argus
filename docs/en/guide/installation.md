# Installation Guide

This guide explains how to install the argus library in different environments.

## Quick Reference

| Method | Command | Best For |
|--------|---------|----------|
| **Package Managers** | `conan install argus/1.0.0@` | Production use |
| **From Source** | `meson setup .build && meson compile -C .build` | Development |
| **With Just** | `just build` | Development workflow |

## Prerequisites

argus has only one optional dependency:

!!! info "PCRE2 Dependency"
    **PCRE2**: Required only for regex validation support.
    
    === "Ubuntu/Debian"
        ```bash
        apt install libpcre2-dev
        ```
    
    === "Fedora/CentOS"
        ```bash
        dnf install pcre2-devel
        ```
    
    === "macOS"
        ```bash
        brew install pcre2
        ```

## Installation Methods

### Package Managers (Recommended)

#### Conan

```bash
# Install from Conan Center
conan install argus/1.0.0@

# With specific options
conan install argus/1.0.0@ -o argus:regex=false
```

In your project's `conanfile.txt`:
```
[requires]
argus/1.0.0

[options]
argus:regex=False
```

#### vcpkg

```bash
# Install from vcpkg registry
vcpkg install argus

# Without regex support
vcpkg install argus[core]
```

In your project's `vcpkg.json`:
```json
{
  "dependencies": [
    {
      "name": "argus",
      "features": ["regex"]
    }
  ]
}
```

### Building From Source

#### Using Meson

```bash
# Clone the repository
git clone https://github.com/lucocozz/argus.git
cd argus

# Build
meson setup .build
meson compile -C .build

# Install (requires permissions)
meson install -C .build
```

#### Using Just (Development Workflow)

!!! tip "Just"
    [Just](https://github.com/casey/just) is a command runner that simplifies common tasks.

```bash
# Clone the repository
git clone https://github.com/lucocozz/argus.git
cd argus

# Build static and shared libraries
just build

# Install
just install
```

### Using as a Static Library

If you prefer not to install system-wide:

1. Build the project using any method above
2. Copy `argus.a` to your project
3. Copy the `includes/` directory to your project
4. Link with the static library:

```bash
gcc your_program.c -o your_program -L/path/to/argus.a -largus
```

### As a Meson Dependency

```meson
# In your meson.build
argus_dep = dependency('argus', version: '>=1.0.0', required: false)

# Fallback to subproject if not found system-wide
if not argus_dep.found()
  argus_proj = subproject('argus')
  argus_dep = argus_proj.get_variable('argus_dep')
endif
```

## Configuration Options

### Disabling Regex Support

If you don't need regex validation, you can build without the PCRE2 dependency:

=== "Meson"
    ```bash
    meson setup -Dregex=false .build
    ```

=== "Just"
    ```bash
    just regex=false build
    ```

=== "Conan"
    ```bash
    conan install . -o argus:regex=false
    ```

=== "vcpkg"
    ```bash
    vcpkg install argus --features=""
    ```

When regex support is disabled:
- No PCRE2 dependency is required
- The `REGEX()` validator becomes a non-functional stub
- All predefined patterns in `argus/regex.h` are defined but won't work
- The `ARGUS_REGEX` macro isn't defined for conditional compilation

### Performance Optimization

For production deployments, enable release mode to skip validation during initialization:

=== "Manual Compilation"
    ```bash
    gcc your_program.c -o your_program -DARGUS_RELEASE -largus
    ```

=== "Meson"
    ```meson
    add_project_arguments('-DARGUS_RELEASE', language: 'c')
    ```

## Verifying Installation

=== "Check Files"
    ```bash
    # Check shared library
    ls -la /usr/local/lib/argus.so*
    
    # Check headers
    ls -la /usr/local/include/argus*
    ```

=== "Test Compilation"
    ```bash
    # Compile an example program
    gcc -o test_argus test.c -largus
    
    # Run the program
    ./test_argus --help
    ```
