<p align="center">
  <img src="docs/static/img/argus-logo.webp" alt="argus logo" width="200">
</p>

<h1 align="center">Argus</h1>

<p align="center">
  <strong>Modern C library for command-line argument parsing with an elegant, declarative API</strong>
</p>

<p align="center">
  <a href="https://github.com/lucocozz/argus/actions/workflows/ci-complete.yml"><img src="https://github.com/lucocozz/argus/actions/workflows/ci-complete.yml/badge.svg" alt="CI/CD Pipeline"></a>
  <a href="https://github.com/lucocozz/argus/actions/workflows/codeql.yml"><img src="https://github.com/lucocozz/argus/actions/workflows/codeql.yml/badge.svg" alt="CodeQL Analysis"></a>
  <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-blue.svg" alt="License: MIT"></a>
  <a href="https://argus.readthedocs.io"><img src="https://img.shields.io/badge/docs-latest-blue.svg" alt="Documentation"></a>
</p>

---

## Why Argus?

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
    bool verbose = argus_get(argus, "verbose").as_bool;
    const char *input = argus_get(argus, "input").as_string;
    
    printf("Processing %s\n", input);
    
    argus_free(&argus);
    return 0;
}
```

**Build and test:**
```bash
gcc my_tool.c -o my_tool -largus
./my_tool --help    # See auto-generated help
./my_tool input.txt # Run your tool
```

## Real-World Examples

<details>
<summary><strong>Git-like Tool</strong></summary>

```c
// Subcommands with their own options
ARGUS_OPTIONS(add_options,
    HELP_OPTION(),
    OPTION_FLAG('f', "force", HELP("Force add")),
    POSITIONAL_STRING("files", HELP("Files to add"))
)

ARGUS_OPTIONS(options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    SUBCOMMAND("add", add_options, HELP("Add files"), ACTION(add_command)),
    SUBCOMMAND("status", status_options, HELP("Show status"), ACTION(status_command))
)

// Usage: ./vcs add --force file.txt
//        ./vcs status --verbose
```
</details>

<details>
<summary><strong>Configuration Tool</strong></summary>

```c
ARGUS_OPTIONS(options,
    HELP_OPTION(),
    // Array of tags
    OPTION_ARRAY_STRING('t', "tags", HELP("Resource tags")),
    // Key-value environment variables  
    OPTION_MAP_STRING('e', "env", HELP("Environment variables")),
    // Email validation with regex
    OPTION_STRING('n', "notify", HELP("Notification email"),
                 VALIDATOR(V_REGEX(ARGUS_RE_EMAIL)))
)

// Usage: ./config --tags=web,api --env=DEBUG=1,PORT=8080 --notify=admin@company.com
```
</details>

<details>
<summary><strong>Server Application</strong></summary>

```c
ARGUS_OPTIONS(options,
    HELP_OPTION(),
    // Load from environment with fallback
    OPTION_STRING('H', "host", HELP("Bind address"), 
                 ENV_VAR("HOST"), DEFAULT("0.0.0.0")),
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

📚 **[Full Documentation](https://argus.readthedocs.io)** - Complete guides and API reference  
🚀 **[Quick Start Guide](https://argus.readthedocs.io/getting-started/quickstart)** - Get running in 5 minutes  
💡 **[Examples](https://argus.readthedocs.io/examples)** - Real-world usage patterns  
🔧 **[API Reference](https://argus.readthedocs.io/api-reference)** - Complete function and macro documentation  

## Comparison

| Feature | Argus | getopt | argp | argtable3 |
|---------|-------|--------|------|-----------|
| Declarative API | ✅ | ❌ | ❌ | ❌ |
| Type Safety | ✅ | ❌ | ❌ | ✅ |
| Auto Help | ✅ | ❌ | ✅ | ✅ |
| Subcommands | ✅ | ❌ | ❌ | ❌ |
| Environment Vars | ✅ | ❌ | ❌ | ❌ |
| Collections | ✅ | ❌ | ❌ | ❌ |
| Validation | ✅ | ❌ | ❌ | ❌ |
| Learning Curve | Low | High | High | Medium |

## Requirements

- **C11 compatible compiler** (GCC 13+, Clang 14+)
- **Optional:** PCRE2 for regex validation (disable with `-Dregex=false`)

## Roadmap

- 📄 Config files - JSON/YAML configuration loading
- 🪶 Lightweight version - Minimal footprint option for embedded systems
- 🎨 Themed help - Customizable colored help output
- 📁 Shell completion - Auto-generated tab completion for bash/zsh/fish
- 🔗 Alias support - Command and option aliases for better UX
- 📦 Plugin system - Extensibility mechanisms for custom handlers

## Contributing

Contributions welcome! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

MIT License - See [LICENSE](LICENSE) for details.

---

<p align="center">
  <sub>Built with ❤️ by <a href="https://github.com/lucocozz">lucocozz</a></sub>
</p>
