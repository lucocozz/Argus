# Installation

Get Argus up and running on your system quickly with multiple installation options.

:::warning Work in Progress
Only manual installation is available at the moment. Package manager support is coming soon.
:::

## Quick Install

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
<TabItem value="linux" label="Linux" default>

### Package Managers

<Tabs>
<TabItem value="vcpkg" label="vcpkg">

```bash
vcpkg install argus
# Without Regex support (removes PCRE2 dependency)
vcpkg install argus[core]
```

</TabItem>
<TabItem value="conan" label="Conan">

```bash
conan install argus/1.0.0
# Without Regex support (removes PCRE2 dependency)
conan install argus/1.0.0 -o argus:regex=False
```

</TabItem>
<TabItem value="meson" label="Meson">

```bash
meson install argus
# Without Regex support (removes PCRE2 dependency)
meson install argus -Dregex=false
```

</TabItem>
<TabItem value="xrepo" label="XRepo">

```bash
xrepo install argus
# Without Regex support (removes PCRE2 dependency)
xrepo install "regex=false" argus
```

</TabItem>
</Tabs>

### System Packages

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install libargus-dev

# Fedora/CentOS/RHEL
sudo dnf install argus-devel

# Arch Linux
sudo pacman -S argus
```

### From Source

```bash
git clone https://github.com/lucocozz/argus.git
cd argus
meson setup builddir # -Dregex=false
meson compile -C builddir
sudo meson install -C builddir
```

</TabItem>
<TabItem value="macos" label="macOS">

### Package Managers

<Tabs>
<TabItem value="vcpkg" label="vcpkg">

```bash
vcpkg install argus
# Without Regex support (removes PCRE2 dependency)
vcpkg install argus[core]
```

</TabItem>
<TabItem value="conan" label="Conan">

```bash
conan install argus/1.0.0
# Without Regex support (removes PCRE2 dependency)
conan install argus/1.0.0 -o argus:regex=False
```

</TabItem>
<TabItem value="meson" label="Meson">

```bash
meson install argus
# Without Regex support (removes PCRE2 dependency)
meson install argus -Dregex=false
```

</TabItem>
<TabItem value="xrepo" label="XRepo">

```bash
xrepo install argus
# Without Regex support (removes PCRE2 dependency)
xrepo install "regex=false" argus
```

</TabItem>
</Tabs>

### From Source

```bash
# Install dependencies
brew install meson ninja pcre2

# Build and install
git clone https://github.com/lucocozz/argus.git
cd argus
meson setup builddir # -Dregex=false
meson compile -C builddir
sudo meson install -C builddir
```

</TabItem>
<TabItem value="windows" label="Windows">

### Package Managers

<Tabs>
<TabItem value="vcpkg" label="vcpkg">

```bash
vcpkg install argus
# Without Regex support (removes PCRE2 dependency)
vcpkg install argus[core]
```

</TabItem>
<TabItem value="conan" label="Conan">

```bash
conan install argus/1.0.0
# Without Regex support (removes PCRE2 dependency)
conan install argus/1.0.0 -o argus:regex=False
```

</TabItem>
<TabItem value="meson" label="Meson">

```bash
meson install argus
# Without Regex support (removes PCRE2 dependency)
meson install argus -Dregex=false
```

</TabItem>
<TabItem value="xrepo" label="XRepo">

```bash
xrepo install argus
# Without Regex support (removes PCRE2 dependency)
xrepo install "regex=false" argus
```

</TabItem>
</Tabs>

### From Source (MinGW/MSYS2)

```bash
# Install dependencies
pacman -S mingw-w64-x86_64-meson mingw-w64-x86_64-ninja

# Build
git clone https://github.com/lucocozz/argus.git
cd argus
meson setup builddir # -Dregex=false
meson compile -C builddir
meson install -C builddir
```

:::warning Windows Compatibility
Argus requires **GCC 13.0+** on Windows. MSVC support is planned for future releases.
:::

</TabItem>
</Tabs>

## Dependencies

Argus has minimal dependencies:

| Dependency | Required | Purpose | Notes |
|------------|----------|---------|-------|
| **PCRE2** ℹ️ | Optional | Regex validation | Can disable with `-Dregex=false` |
| **Meson** | Build only | Build system | Version 1.1.0+ required |
| **Ninja** | Build only | Backend | Recommended for faster builds |

:::info PCRE2 Auto-Installation
PCRE2 is automatically downloaded and built when installing Argus (via any method). No manual installation required!
:::

## Configuration Options

When building from source, customize your installation:

<Tabs>
<TabItem value="basic" label="Basic Options">

```bash
# Disable regex support (removes PCRE2 dependency)
meson setup builddir -Dregex=false

# Release build for production
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

## Verification

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
    argus_t argus = argus_init(options, "test", "1.0.0");
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

## Integration

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
add_requires("argus >=1.0.0")
target("myapp")
    set_kind("binary")
    add_files("main.c")
    add_packages("argus")
```

</TabItem>
<TabItem value="meson" label="Meson">

```meson title="meson.build"
argus_project = subproject('argus', version: '>=1.0.0')
# Without Regex support
argus_project = subproject('argus', version: '>=1.0.0', default_options: ['regex=false'])

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

## Troubleshooting

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
<summary><strong>Windows GCC version error</strong></summary>

```bash
# Check GCC version
gcc --version

# Update if < 13.0.0
# MSYS2: pacman -S mingw-w64-x86_64-gcc
# Or use MSVC build (coming soon)
```

</details>

### Platform-Specific Notes

**Linux**: Most distributions package Argus. Check your package manager first.

**macOS**: Homebrew is the recommended installation method.

**Windows**: vcpkg provides the smoothest experience. Native MSVC support coming soon.
