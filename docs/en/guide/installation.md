# Installation Guide

This guide explains how to install the argus library on various operating systems.

!!! warning "Working in progress"
    - Only the manual installation method is available at the moment. The package manager methods are reserved for future releases.
    - Windows support is limited to gcc version 13.0.0 or newer.

## Quick Installation Methods

Choose the method that works best for your environment:

<div class="grid cards" markdown>

-   :material-package-down:{ .lg .middle } **Package Managers**

    ---
    
    === "Windows"
        ```bash
        # Using vcpkg
        vcpkg install argus
        ```
    
    === "Linux"
        ```bash
        # Using vcpkg
        vcpkg install argus

        # Using conan
        conan install argus/1.0.0
        ```
    
    === "macOS"
        ```bash
        # Using vcpkg
        vcpkg install argus
        ```

-   :material-code-tags:{ .lg .middle } **From Source**

    ---
    
    ```bash
    # Clone repository
    git clone https://github.com/lucocozz/argus.git
    cd argus
    
    # Build and install
    meson setup .build
    meson compile -C .build
    sudo meson install -C .build
    ```

-   :material-script:{ .lg .middle } **Installer Script**

    ---
    
    ```bash
    # Download and extract
    curl -LO https://github.com/lucocozz/argus/releases/download/v1.0.0/argus-1.0.0.tar.gz
    tar -xzf argus-1.0.0.tar.gz
    
    # Run installer
    cd argus-1.0.0
    ./install.sh
    ```

-   :material-download:{ .lg .middle } **Pre-built Binaries**

    ---
    
    1. Download from [GitHub Releases](https://github.com/lucocozz/argus/releases)
    2. Extract the package
    3. Use the libraries and headers directly

</div>

## Dependencies

argus has only one optional dependency:

**PCRE2**: Required only for regex validation support.

=== "Windows"
    ```bash
    # Using vcpkg
    vcpkg install pcre2:x64-windows
    ```

=== "Linux"
    ```bash
    # Ubuntu/Debian
    sudo apt install libpcre2-dev
    
    # Fedora/RHEL
    sudo dnf install pcre2-devel
    
    # Arch Linux
    sudo pacman -S pcre2
    ```

=== "macOS"
    ```bash
    # Using Homebrew
    brew install pcre2
    ```

## Detailed Installation Methods

### Package Managers

#### Conan

[Conan](https://conan.io/) provides cross-platform package management for C/C++ libraries.

```bash
# Install latest version
conan install argus/1.0.0@

# Disable regex support
conan install argus/1.0.0@ -o argus:regex=false
```

Add to your project's `conanfile.txt`:
```ini
[requires]
argus/1.0.0

[options]
argus:regex=True  # Set to False to disable regex
```

#### vcpkg

[vcpkg](https://github.com/microsoft/vcpkg) is Microsoft's C++ library manager.

```bash
# Install with regex support
vcpkg install argus

# Install without regex support
vcpkg install argus[core]
```

Add to your project's `vcpkg.json`:
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

<!--
#### APT (Debian/Ubuntu)
Reserved for future releases
-->

<!--
#### Homebrew (macOS)
Reserved for future releases
-->

<!--
#### Chocolatey (Windows)
Reserved for future releases
-->

### Building From Source

#### Step 1: Install Build Tools

=== "Windows"
    Install the following tools:
    
    - **Visual Studio** or **MinGW with GCC 13.0.0+**
    - **Python** 3.7+
    - **Meson** and **Ninja** build system
    
    ```bash
    # Install Python and Meson
    pip install meson ninja
    
    # For regex support (optional)
    vcpkg install pcre2:x64-windows
    ```
    
    !!! warning "Windows Compatibility"
        The argus library is only compatible with GCC version 13.0.0 or newer on Windows platforms.

=== "Linux"
    ```bash
    # Ubuntu/Debian
    sudo apt install build-essential meson ninja-build pkg-config
    
    # Fedora/RHEL
    sudo dnf install gcc meson ninja-build pkg-config
    
    # Arch Linux
    sudo pacman -S base-devel meson ninja
    ```

=== "macOS"
    ```bash
    # Install command line tools
    xcode-select --install
    
    # Install build tools
    brew install meson ninja
    ```

#### Step 2: Clone the Repository

```bash
git clone https://github.com/lucocozz/argus.git
cd argus
```

#### Step 3: Configure and Build

```bash
# Configure the build
meson setup .build

# Build the library
meson compile -C .build
```

Configuration options:

```bash
# Disable regex support
meson setup .build -Dregex=false

# Release build
meson setup .build --buildtype=release

# Enable examples
meson setup .build -Dexamples=true

# Enable tests
meson setup .build -Dtests=true
```

#### Step 4: Install

=== "Windows (Administrator Command Prompt)"
    ```bash
    meson install -C .build
    ```

=== "Linux/macOS"
    ```bash
    sudo meson install -C .build
    ```

### Using Just (Development)

If you have [Just](https://github.com/casey/just) installed, you can use these convenient commands:

```bash
# Build libraries
just build

# Run tests
just test

# Build and install
just build install

# Build with specific options
just regex=false build
```

### Pre-built Binaries

Download pre-built packages from [GitHub Releases](https://github.com/lucocozz/argus/releases).

=== "Windows"
    1. Download `argus-1.0.0-windows.zip`
    2. Extract to any location
    3. Add `bin` directory to PATH or use libraries directly
    
    **For MSVC projects:**
    - Add include path to your project
    - Link against argus.lib
    
    **For MinGW/GCC projects:**
    - Requires GCC 13.0.0 or newer
    - Add include path to your project
    - Link against libargus.a

=== "Linux/macOS"
    1. Download `argus-1.0.0.tar.gz`
    2. Extract: `tar -xzf argus-1.0.0.tar.gz`
    3. Run the installer: `cd argus-1.0.0 && ./install.sh`

### Installer Script

For Linux and macOS, the installer script provides a simple installation method:

```bash
# System-wide installation (default)
./install.sh

# Install to user directory (~/.local)
./install.sh --local

# Install without regex support
./install.sh --no-regex

# Specify custom installation directory
./install.sh --prefix=/opt/argus
```

Available options:

| Option | Description |
|--------|-------------|
| `--prefix=PATH` | Installation prefix (default: `/usr/local`) |
| `--libdir=PATH` | Library directory (default: `$prefix/lib`) |
| `--includedir=PATH` | Include directory (default: `$prefix/include`) |
| `--local` | Install to `~/.local` instead of system directories |
| `--no-regex` | Disable regex support |
| `--uninstall` | Uninstall the library |
| `--verbose` | Show detailed output |

## Project Integration

### Compiler Flags

```bash
# Direct compilation
gcc your_program.c -o your_program -largus

# With pkg-config
gcc your_program.c -o your_program $(pkg-config --cflags --libs argus)
```

### CMake Integration

```cmake
# Find installed argus
find_package(argus REQUIRED)
target_link_libraries(your_target PRIVATE argus::argus)
```

### Meson Integration

```meson
# In your meson.build
argus_dep = dependency('argus', version: '>=1.0.0', required: false)

# Fallback to subproject if not found
if not argus_dep.found()
  argus_proj = subproject('argus')
  argus_dep = argus_proj.get_variable('argus_dep')
endif

executable('myapp', 'main.c', dependencies: [argus_dep])
```

## Platform-Specific Notes

### Windows-Specific

=== "MSVC"
    ```powershell
    # Open Developer Command Prompt for VS
    
    # Configure and build
    meson setup .build
    meson compile -C .build
    
    # With vcpkg PCRE2
    meson setup .build -Dpcre2_include_dir=C:/vcpkg/installed/x64-windows/include -Dpcre2_lib_dir=C:/vcpkg/installed/x64-windows/lib
    ```

=== "MinGW"
    ```bash
    # Configure and build
    meson setup .build
    meson compile -C .build
    ```
    
    !!! important "GCC Version Requirement"
        For Windows with MinGW, you must use GCC version 13.0.0 or newer. Older versions are not compatible with the argus library.
        
        Check your GCC version with: `gcc --version`

### Linux-Specific

If you installed to a non-standard location or your user directory, you may need to update your environment:

```bash
# Add to your .bashrc or .zshrc
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.local/lib
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:~/.local/lib/pkgconfig
```

### macOS-Specific

If you installed to a non-standard location:

```bash
# Add to your .bashrc or .zshrc
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:~/.local/lib
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:~/.local/lib/pkgconfig
```

## Troubleshooting

### Common Issues

| Issue | Solution |
|-------|----------|
| **Library not found when linking** | Add library path: `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib` |
| **Headers not found** | Add include path: `-I/path/to/argus/include` |
| **PCRE2 not found** | Install PCRE2 or disable regex with `-Dregex=false` |
| **Windows DLL not found at runtime** | Add DLL directory to PATH or copy DLL to executable directory |
| **"undefined reference" errors on Windows** | Make sure you're using GCC 13.0.0 or newer |

### Windows GCC Version

On Windows, argus requires GCC 13.0.0 or newer. If you're using an older version, you'll need to upgrade:

```bash
# Check your GCC version
gcc --version

# If using MSYS2/MinGW, update to the latest version
pacman -Syu mingw-w64-x86_64-gcc
```

### PCRE2 on Windows

On Windows, PCRE2 detection can be tricky. Explicitly specify the directories:

```bash
meson setup .build -Dpcre2_include_dir=C:/path/to/pcre2/include -Dpcre2_lib_dir=C:/path/to/pcre2/lib
```

## Verifying Installation

Create a simple test program (`test.c`):

```c
#include <argus.h>
#include <stdio.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION()
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    if (status == ARGUS_SUCCESS)
        printf("argus is working correctly!\n");

    argus_free(&argus);
    return 0;
}
```

Compile and run:

```bash
gcc test.c -o test -largus
./test --help
```

## Next Steps

Once installed, proceed to the [Quick Start Guide](quickstart.md) to begin using argus in your projects.
