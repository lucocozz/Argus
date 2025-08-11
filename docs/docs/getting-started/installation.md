# > Installation_

Get Argus up and running on your system quickly with multiple installation options.

## // Quick Install

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
<TabItem value="linux" label="Linux" default>

:::info System Requirements
**Compiler**: GCC 13.0+ or Clang 14+
:::

### Package Managers

<Tabs>
<TabItem value="vcpkg" label="vcpkg">

```bash
# With Regex support (requires PCRE2)
vcpkg install argus[regex]
# Without Regex support (default)
vcpkg install argus
```

</TabItem>
<!-- <TabItem value="conan" label="Conan">

```bash
# With Regex support (requires PCRE2)
conan install argus/0.1.0 -o argus:regex=True
# Without Regex support (default)
conan install argus/0.1.0
```

</TabItem> -->
<TabItem value="meson" label="Meson">

```bash
# Install as wrap dependency
meson wrap install argus
# With Regex support (requires PCRE2)
meson wrap install argus -Dregex=true
```

</TabItem>
<TabItem value="xmake" label="XMake">

```bash
# Basic installation
xmake require argus
# With Regex support (requires PCRE2)
xmake require --extra="{configs={regex=true}}" argus

# XRepo alternative
xrepo install argus
xrepo install --configs="regex=true" argus
```

</TabItem>
</Tabs>

### From Source

```bash
git clone https://github.com/lucocozz/argus.git
cd argus
meson setup builddir # -Dregex=true for regex support
meson compile -C builddir
sudo meson install -C builddir
```

</TabItem>
<TabItem value="macos" label="macOS">

:::info System Requirements
**Compiler**: GCC 13.0+ or Clang 14+
:::

### Package Managers

<Tabs>
<TabItem value="vcpkg" label="vcpkg">

```bash
# With Regex support (requires PCRE2)
vcpkg install argus[regex]
# Without Regex support (default)
vcpkg install argus
```

</TabItem>
<!-- <TabItem value="conan" label="Conan">

```bash
# With Regex support (requires PCRE2)
conan install argus/0.1.0 -o argus:regex=True
# Without Regex support (default)
conan install argus/0.1.0
```

</TabItem> -->
<TabItem value="meson" label="Meson">

```bash
# Install as wrap dependency
meson wrap install argus
# With Regex support (requires PCRE2)
meson wrap install argus -Dregex=true
```

</TabItem>
<TabItem value="xmake" label="XMake">

```bash
# Basic installation
xmake require argus
# With Regex support (requires PCRE2)
xmake require --extra="{configs={regex=true}}" argus

# XRepo alternative
xrepo install argus
xrepo install --configs="regex=true" argus
```

</TabItem>
</Tabs>

### From Source


```bash
# Install dependencies (if needed)
# Homebrew: brew install meson ninja
# MacPorts: sudo port install meson ninja

# Build and install
git clone https://github.com/lucocozz/argus.git
cd argus
meson setup builddir # -Dregex=true for regex support
meson compile -C builddir
sudo meson install -C builddir
```

</TabItem>
<TabItem value="windows" label="Windows">

:::info System Requirements
**Compiler**: GCC 13.0+ (MSYS2/MinGW) or Clang 14+  
**Note**: MSVC is not compatible due to non-standard extensions used
:::

### Package Managers

<Tabs>
<TabItem value="vcpkg" label="vcpkg">

```bash
# With Regex support (requires PCRE2)
vcpkg install argus[regex]
# Without Regex support (default)
vcpkg install argus
```

</TabItem>
<!-- <TabItem value="conan" label="Conan">

```bash
# With Regex support (requires PCRE2)
conan install argus/0.1.0 -o argus:regex=True
# Without Regex support (default)
conan install argus/0.1.0
```

</TabItem> -->
<TabItem value="meson" label="Meson">

```bash
# Install as wrap dependency
meson wrap install argus
# With Regex support (requires PCRE2)
meson wrap install argus -Dregex=true
```

</TabItem>
<TabItem value="xmake" label="XMake">

```bash
# Basic installation
xmake require argus
# With Regex support (requires PCRE2)
xmake require --extra="{configs={regex=true}}" argus

# XRepo alternative
xrepo install argus
xrepo install --configs="regex=true" argus
```

</TabItem>
</Tabs>

### From Source (MinGW/MSYS2)


```bash
# Install dependencies
pacman -S mingw-w64-x86_64-meson mingw-w64-x86_64-ninja mingw-w64-x86_64-gcc

# Build
git clone https://github.com/lucocozz/argus.git
cd argus
meson setup builddir # -Dregex=true for regex support
meson compile -C builddir
meson install -C builddir
```

:::warning MSVC Compatibility
Argus is **not compatible with MSVC** due to non-standard language extensions. Use **GCC 13.0+** or **Clang 14+** with MSYS2/MinGW.
:::

</TabItem>
</Tabs>

:::info System Packages
System packages are not currently available. Use package managers or build from source.
:::

## // Dependencies

Argus has minimal dependencies:

| Dependency | Required | Purpose | Notes |
|------------|----------|---------|-------|
| **PCRE2** ℹ️ | Optional | Regex validation | Enable with `-Dregex=true` |
| **Meson** | Build only | Build system | Version 1.1.0+ required |
| **Ninja** | Build only | Backend | Recommended for faster builds |

:::info PCRE2 Auto-Installation
PCRE2 is automatically downloaded and built when regex support is enabled. No manual installation required!
:::

## // Configuration Options

When building from source, customize your installation:

<Tabs>
<TabItem value="basic" label="Basic Options">

```bash
# Enable regex support (adds PCRE2 dependency)
meson setup builddir -Dregex=true

# Release build for production (default)
meson setup builddir

# Install to custom location
meson setup builddir --prefix=/opt/argus
```

</TabItem>
<TabItem value="development" label="Development">

```bash
# Enable tests and examples
meson setup builddir -Dtests=true -Dexamples=true

# Debug build with coverage
meson setup builddir --buildtype=debug -Db_coverage=true
```

</TabItem>
</Tabs>

## // Verification

Test your installation:

```c title="test.c"
#include <argus.h>
#include <stdio.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "test", "0.1.0");
    int status = argus_parse(&argus, argc, argv);
    
    if (status == ARGUS_SUCCESS)
        printf("✅ Argus is working!\n");
    
    return 0;
}
```

```bash
# Compile and test
gcc test.c -o test -largus
./test --help
```

## // Integration

<Tabs>
<TabItem value="cmake" label="CMake" default>

```cmake title="CMakeLists.txt"
find_package(PkgConfig REQUIRED)
pkg_check_modules(ARGUS REQUIRED argus)

add_executable(myapp main.c)
target_link_libraries(myapp ${ARGUS_LIBRARIES})
target_include_directories(myapp PRIVATE ${ARGUS_INCLUDE_DIRS})
```

</TabItem>
<TabItem value="xmake" label="XMake">

```lua title="xmake.lua"
add_requires("argus >=0.1.0")
-- With regex support
add_requires("argus >=0.1.0", {configs = {regex = true}})

target("myapp")
    set_kind("binary")
    add_files("main.c")
    add_packages("argus")
```

</TabItem>
<TabItem value="meson" label="Meson">

```meson title="meson.build"
argus_project = subproject('argus', version: '>=0.1.0')
# With regex support
argus_project = subproject('argus', version: '>=0.1.0', default_options: ['regex=true'])

argus_dep = argus_project.get_variable('argus_dep')

executable('myapp', 'main.c', 
          dependencies: [argus_dep])
```

</TabItem>
<TabItem value="pkg-config" label="pkg-config">

```bash
# Get compiler flags
pkg-config --cflags argus

# Get linker flags  
pkg-config --libs argus

# Compile directly
gcc main.c $(pkg-config --cflags --libs argus) -o myapp
```

</TabItem>
</Tabs>

## // Troubleshooting

### Common Issues

<details>
<summary><strong>Library not found during linking</strong></summary>

```bash
# Add library path
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
sudo ldconfig

# Or specify path explicitly
gcc main.c -L/usr/local/lib -largus -o myapp
```

</details>

<details>
<summary><strong>Headers not found</strong></summary>

```bash
# Add include path
gcc main.c -I/usr/local/include -largus -o myapp

# Check installation
find /usr -name "argus.h" 2>/dev/null
```

</details>

<details>
<summary><strong>Compiler version error</strong></summary>

```bash
# Check compiler version
gcc --version
clang --version

# Required: GCC 13.0+ or Clang 14+
# Update if needed or use MSYS2 on Windows
```

</details>

### Platform-Specific Notes

**Linux**: Use package managers or build from source. Ensure GCC 13+ or Clang 14+.

**macOS**: Package managers provide the best experience. Ensure GCC 13+ or Clang 14+.

**Windows**: vcpkg recommended. **MSVC not supported** - use GCC 13+ via MSYS2/MinGW or Clang 14+.
