<p align="center">
  <img src="docs/static/img/argus-logo.webp" alt="argus logo" width="200">
</p>

<h1 align="center">Argus</h1>

<p align="center">
  <strong>Modern C library for command-line argument parsing with a powerful, declarative API</strong>
</p>

<p align="center">
  <!-- <a href="https://github.com/lucocozz/argus/actions/workflows/ci-complete.yml"><img src="https://github.com/lucocozz/argus/actions/workflows/ci-complete.yml/badge.svg" alt="CI/CD Pipeline"></a>
  <a href="https://github.com/lucocozz/argus/actions/workflows/codeql.yml"><img src="https://github.com/lucocozz/argus/actions/workflows/codeql.yml/badge.svg" alt="CodeQL Analysis"></a> -->
  <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-blue.svg" alt="License: MIT"></a>
  <a href="https://argus-lib.com"><img src="https://img.shields.io/badge/docs-latest-blue.svg" alt="Documentation"></a>
</p>

---

## The Problem

Replace tedious argument parsing with declarative definitions:

```c
// Old way - verbose and error-prone
int opt;
while ((opt = getopt(argc, argv, "vo:p:")) != -1) {
    switch (opt) {
        case 'v': verbose = 1; break;
        case 'o': output = optarg; break;
        case 'p': port = atoi(optarg); break;
    }
}
// + manual validation, help generation, error handling...

// Argus way - declarative and complete
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("result.txt")),
    OPTION_INT('p', "port", HELP("Port number"), VALIDATOR(V_RANGE(1, 65535)))
)
```

## Why Choose Argus?

Instead of feature matrices, let's be honest about what each library excels at:

### 🏃‍♂️ **getopt** - The Speed Demon
**Best for**: Performance-critical applications, embedded systems, legacy codebases
```c
// Minimal overhead, maximum control
while ((opt = getopt(argc, argv, "vo:")) != -1) { ... }
```
**Trade-offs**: You write everything from scratch - validation, help, error handling.

### 🔧 **argp** - The GNU Workhorse  
**Best for**: GNU/Linux applications, when you need proven stability
```c
// Structured approach with auto-help
static struct argp_option options[] = { ... };
```
**Trade-offs**: GNU-only, callback complexity grows with features.

### 📊 **argtable3** - The Structured Approach
**Best for**: Type safety without modern C requirements, cross-platform needs
```c
// Table-driven with strong typing
struct arg_lit *verbose = arg_lit0("v", "verbose", "enable verbose");
```
**Trade-offs**: Verbose setup, no built-in subcommands or environment integration.

### 🚀 **Argus** - The Modern Choice
**Best for**: New projects, complex CLIs, developer experience
```c
// Declarative with everything built-in
ARGUS_OPTIONS(opts,
    OPTION_FLAG('v', "verbose", HELP("Enable verbose")),
    SUBCOMMAND("deploy", deploy_options, HELP("Deploy application"))
)
```
**Trade-offs**: Requires modern compiler (C11+), younger ecosystem.

---

## Quick Decision Guide

**Choose getopt if**: Raw performance matters most, working with legacy systems  
**Choose argp if**: Building GNU/Linux tools, want battle-tested reliability  
**Choose argtable3 if**: Need type safety but stuck with older compilers  
**Choose Argus if**: Want modern C development experience with full feature set

---

## What Makes Argus Different

The real question isn't "what features does each library have?" but "how much code do you want to write?"

**Traditional approach** (getopt/argp):
- ✅ You control the parsing loop and logic
- ❌ You implement validation, help, error handling from scratch

**Argus approach**:
- ✅ Declare your interface once, extend via handlers/validators as needed
- ✅ Get parsing, help, subcommands built-in with full customization hooks
- ❌ Less control over the core parsing algorithm (but who wants to rewrite that?)

**Bottom line**: Argus trades some control for significantly less boilerplate and better maintainability.

## Core Features

- **🎯 Type Safety** - Strong typing with automatic validation
- **📖 Auto Help** - Beautiful help generation from definitions  
- **🔧 Subcommands** - Git/Docker style nested commands
- **📦 Collections** - Arrays and maps for multiple values
- **🌍 Environment** - Seamless env var integration
- **✅ Validation** - Built-in validators + regex patterns

## Quick Start

**Install:**
```bash
# Package managers
vcpkg install argus
conan install argus/0.1.0

# From source
git clone https://github.com/lucocozz/argus.git && cd argus
meson setup build && meson compile -C build && sudo meson install -C build
```

**Create your first CLI:**
```c
#include <argus.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("result.txt")),
    POSITIONAL_STRING("input", HELP("Input file"))
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "my_tool", "1.0.0");
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    // Type-safe access
    bool verbose = argus_get(&argus, "verbose").as_bool;
    const char *output = argus_get(&argus, "output").as_string;
    const char *input = argus_get(&argus, "input").as_string;
    
    argus_free(&argus);
    return 0;
}
```

<details>
<summary><strong>🌟 Advanced Example</strong></summary>

```c
ARGUS_OPTIONS(
    server_options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_STRING('H', "host", HELP("Server hostname"), DEFAULT("0.0.0.0")),
    OPTION_INT('p', "port", HELP("Port number"),
              ENV_VAR("PORT"), VALIDATOR(V_RANGE(1, 65535)), DEFAULT(8080)),
    // Choice validation
    OPTION_STRING('l', "level", HELP("Log level"), DEFAULT("info"),
                 VALIDATOR(V_CHOICE_STR("debug", "info", "warn", "error")))
)

// Usage: ./server --host 0.0.0.0 --port 8080 --level debug
// Or:    HOST=api.example.com PORT=9000 ./server
```
</details>

## Documentation

📚 **[Full Documentation](https://argus-lib.com)** - Complete guides and API reference  
🚀 **[Quick Start Guide](https://argus-lib.com/getting-started/quickstart)** - Get running in 5 minutes  
💡 **[Examples](https://argus-lib.com/examples/simple-cli)** - Real-world usage patterns  
🔧 **[API Reference](https://argus-lib.com/api-reference/overview)** - Complete function and macro documentation  

## Requirements

- **C11 compatible compiler** (GCC 13+, Clang 14+)
- **Optional:** PCRE2 for regex validation (disable with `-Dregex=false`)

## Roadmap

- 📄 Config files - JSON/YAML configuration loading
- 🪶 Lightweight version - Minimal footprint option for embedded systems
- 🎨 Themed help - Customizable colored help output
- 📁 Shell completion - Auto-generated tab completion for bash/zsh/fish
- 🔗 Universal built-ins - Common CLI patterns (version formats, debug levels, etc.)
- 📦 Plugin system - Extensibility mechanisms for custom handlers

## Contributing

Contributions welcome! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

MIT License - See [LICENSE](LICENSE) for details.
