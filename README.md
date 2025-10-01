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

## The Traditional Approach

Building command-line tools in C typically involves extensive boilerplate:

```c
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int opt, verbose = 0, port = 8080;
    char *output = "output.txt";
    
    struct option long_options[] = {
        {"verbose", no_argument, 0, 'v'},
        {"output", required_argument, 0, 'o'},
        {"port", required_argument, 0, 'p'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    while ((opt = getopt_long(argc, argv, "vo:p:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'v': verbose = 1; break;
            case 'o': output = optarg; break;
            case 'p': 
                port = atoi(optarg);
                if (port < 1 || port > 65535) {
                    fprintf(stderr, "Error: Port must be 1-65535\n");
                    return 1;
                }
                break;
            case 'h':
                printf("Usage: %s [OPTIONS]\n", argv[0]);
                printf("  -v, --verbose    Enable verbose output\n");
                printf("  -o, --output     Output file\n");
                printf("  -p, --port       Port number\n");
                return 0;
            case '?': return 1;
        }
    }
    
    // Application logic starts here...
}
```

This approach requires manual validation, error handling, and help text generation for every application.

## The Argus Approach

Argus eliminates this boilerplate with a declarative interface:

```c
#include <argus.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_INT('p', "port", HELP("Port number"), 
               VALIDATOR(V_RANGE(1, 65535)), DEFAULT(8080)),
)

int main(int argc, char **argv) {
    argus_t argus = argus_init(options, "my_tool", "1.0.0");
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    // Type-safe access to parsed values
    bool verbose = argus_get(&argus, "verbose").as_bool;
    const char *output = argus_get(&argus, "output").as_string;
    int port = argus_get(&argus, "port").as_int;
    
    // Application logic starts here...
    argus_free(&argus);
    return 0;
}
```

The result is the same functionality with significantly less code and automatic validation, help generation, and type safety.

## Core Features

- **Type Safety** - Strong typing with automatic validation and conversion
- **Declarative API** - Define your interface once, get everything else automatically
- **Auto-generated Help** - Consistent, professional help output based on your definitions
- **Built-in Validation** - Range checking, pattern matching, and custom validators
- **Subcommands** - Git-style nested commands with inheritance
- **Environment Variables** - Seamless integration with system environment
- **Collections** - Arrays and key-value maps for complex data structures
- **Cross-platform** - Works on Linux, macOS, and Windows

## Quick Start

**Installation:**
```bash
# Package managers
vcpkg install argus
conan install argus/0.1.0

# From source
git clone https://github.com/lucocozz/argus.git
cd argus && meson setup build && meson compile -C build
sudo meson install -C build
```

**Basic Example:**
```c
#include <argus.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    OPTION_STRING('f', "file", HELP("Input file")),
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    POSITIONAL_STRING("output", HELP("Output destination")),
)

int main(int argc, char **argv) {
    argus_t argus = argus_init(options, "example", "1.0.0");
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    // Access parsed values
    const char *file = argus_get(&argus, "file").as_string;
    bool verbose = argus_get(&argus, "verbose").as_bool;
    const char *output = argus_get(&argus, "output").as_string;
    
    argus_free(&argus);
    return 0;
}
```

## Advanced Features

<details>
<summary><strong>Complex Validation and Types</strong></summary>

```c
#include <argus.h>
#include <argus/regex.h>

ARGUS_OPTIONS(
    server_options,
    OPTION_STRING(
        'H', "host",
        HELP("Server hostname"),
        DEFAULT("localhost"),
        VALIDATOR(V_REGEX(ARGUS_RE_DOMAIN))
    ),
    OPTION_INT(
        'p', "port",
        HELP("Port number"),
        VALIDATOR(V_RANGE(1024, 65535)),
        ENV_VAR("PORT")
    ),
    OPTION_ARRAY_STRING(
        'w', "worker",
        HELP("Worker processes"),
        VALIDATOR(V_COUNT(1, 8))
    ),
    POSITIONAL_MAP_STRING(
        "config",
        HELP("Key=value configuration pairs")
    )
)
```
</details>

<details>
<summary><strong>Subcommands</strong></summary>

```c
// Define subcommand options
ARGUS_OPTIONS(
    deploy_options,
    OPTION_FLAG('f', "force", HELP("Force deployment")),
    OPTION_STRING('e', "environment", HELP("Target environment")),
)

ARGUS_OPTIONS(
    status_options,
    OPTION_FLAG('v', "verbose", HELP("Verbose status output")),
    OPTION_STRING('s', "service", HELP("Service name to check")),
)

// Main command with subcommands
ARGUS_OPTIONS(
    main_options,
    HELP_OPTION(),
    VERSION_OPTION(),
    SUBCOMMAND("deploy", deploy_options, HELP("Deploy application")),
    SUBCOMMAND("status", status_options, HELP("Check deployment status")),
)

// Usage: ./app deploy --force --environment production
```
</details>

## Use Cases

Argus is well-suited for:

- **Command-line utilities** - Tools that need robust argument handling
- **Developer tools** - Build systems, code generators, deployment scripts
- **System utilities** - Network tools, file processors, automation scripts
- **Modern C applications** - Projects prioritizing maintainability and developer experience

Consider traditional approaches for embedded systems with strict constraints, legacy codebases using getopt, or environments requiring pre-C11 compatibility.

## Design Philosophy

**Declarative over imperative** - Describe what you need rather than how to parse it. Argus handles the implementation details.

**Type safety by default** - Eliminate common parsing errors through strong typing and automatic validation.

**Consistency and reliability** - Generated help text is always current, validation is comprehensive, and error handling is uniform.

**Developer experience** - Reduce boilerplate, provide clear APIs, and make common tasks simple while keeping complex scenarios achievable.

## Documentation

📚 **[Complete Documentation](https://argus-lib.com)** - Comprehensive guides and tutorials  
🚀 **[Quick Start Guide](https://argus-lib.com/getting-started/quickstart)** - Get running in minutes  
💡 **[Example](https://github.com/lucocozz/argus-git-demo)** - Git clone demonstration  
🔧 **[API Reference](https://argus-lib.com/api-reference/overview)** - Complete function documentation

## Requirements

- **C11 compatible compiler** (GCC 13+, Clang 14+)
- **Optional:** PCRE2 for regex validation (disable with `-Dregex=false`)

## Roadmap

- Configuration file support (JSON/YAML)
- Shell completion generation (bash/zsh/fish)
- Lightweight build option for constrained environments
- Additional built-in validators and types

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on reporting issues, submitting features, and development setup.

## License

MIT License - see [LICENSE](LICENSE) for details.
