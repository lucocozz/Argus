# Introduction to Argus

Welcome to **Argus**, a modern C library that transforms command-line argument parsing from a tedious chore into an elegant, type-safe experience.

## Why Argus?

Traditional C argument parsing is verbose, error-prone, and repetitive. Compare this typical approach:

```c
// Traditional getopt approach - verbose and fragile
int main(int argc, char **argv)
{
    int verbose = 0, port = 8080;
    char *output = "output.txt";
    char *input = NULL;
    
    int opt;
    while ((opt = getopt(argc, argv, "vo:p:")) != -1) {
        switch (opt) {
            case 'v': verbose = 1; break;
            case 'o': output = optarg; break;
            case 'p': port = atoi(optarg); break;
            case '?': /* error handling */ break;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected input file\n");
        exit(1);
    }
    input = argv[optind];
    
    // Manual validation needed...
    if (port < 1 || port > 65535) {
        fprintf(stderr, "Invalid port\n");
        exit(1);
    }
}
```

With **Argus**, the same functionality becomes:

```c
#include "argus.h"

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_INT('p', "port", HELP("Port number"), DEFAULT(8080),
               VALIDATOR(V_RANGE(1, 65535))),
    POSITIONAL_STRING("input", HELP("Input file")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "my_program", "1.0.0");

    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;

    // Type-safe value access
    int port = argus_get(argus, "port").as_int;
    bool verbose = argus_get(argus, "verbose").as_bool;
    const char *output = argus_get(argus, "output").as_string;
    const char *input = argus_get(argus, "input").as_string;
    
    argus_free(&argus);
    return 0;
}
```

## What Makes Argus Different

### 🛡️ Type Safety

Options are strongly typed with automatic validation. No more `atoi()` bugs or manual range checking.

### 🎯 Declarative Configuration

Define your CLI interface once using expressive macros. Argus handles parsing, validation, and help generation.

### 🚀 Modern Features

- **Auto-generated Help**: Beautiful, consistent help text
- **Subcommands**: Git/Docker-style nested commands
- **Collections**: Arrays and maps for complex data
- **Environment Variables**: Seamless integration
- **Validation**: Built-in validators for common types
- **Regex Validation**: Pattern-based input validation
- **Customizable**: Easily extend Argus with your own types and validators

### 📦 Zero Dependencies*

Pure C with optional PCRE2 for regex support. Compiles everywhere C does.

## Core Philosophy

Argus follows three principles:

1. **Simplicity**: Common tasks should be trivial
2. **Power**: Complex CLIs should be achievable  
3. **Safety**: Prevent bugs at compile-time when possible

## What You'll Build

By the end of this guide, you'll know how to create sophisticated command-line tools like:

- **Simple utilities** with flags and options
- **Git-like tools** with nested subcommands
- **Configuration managers** with validation and environment integration
- **Build systems** with complex option dependencies

## Ready to Start?

Let's begin with [installation](./installation) and then build your first CLI in the [quickstart guide](./quickstart).

---

:::tip
Argus generates automatic help for all your programs. Try `--help` on any example to see the formatted output!
:::
