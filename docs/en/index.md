# Welcome to argus

<div align="center">
  <img src="assets/argus-logo.webp" alt="argus" width="180px">
  <br><br>
  <em>Modern command-line argument parsing for C</em>
  <br><br>
</div>

## Explore argus

<div class="grid cards" markdown>

-   :material-rocket-launch:{ .lg .middle } **Start Using argus**

    ---

    Get up and running with argus in minutes:

    [:octicons-arrow-right-24: Installation](guide/installation.md)
    [:octicons-arrow-right-24: Quick Start](guide/quickstart.md)
    [:octicons-arrow-right-24: Examples](examples/basic.md)

-   :material-book-open-variant:{ .lg .middle } **Learn the Basics**

    ---

    Master the core concepts:

    [:octicons-arrow-right-24: Basic Options](guide/basic-options.md)
    [:octicons-arrow-right-24: Option Formats](guide/option-formats.md)
    [:octicons-arrow-right-24: Validation](guide/validation.md)

-   :material-tools:{ .lg .middle } **Advanced Features**

    ---

    Unlock the full power of argus:

    [:octicons-arrow-right-24: Subcommands](guide/subcommands.md)
    [:octicons-arrow-right-24: Multi-Values](guide/multi-values.md)
    [:octicons-arrow-right-24: Environment Variables](guide/environment.md)

-   :material-api:{ .lg .middle } **API Reference**

    ---

    Complete documentation for developers:

    [:octicons-arrow-right-24: Functions](api/functions.md)
    [:octicons-arrow-right-24: Macros](api/macros.md)
    [:octicons-arrow-right-24: Types](api/types.md)

</div>

## Features at a glance

argus provides a unique combination of power and simplicity for C command-line argument parsing:

- **Elegant API** - Concise, expressive macros for defining options
- **Type Safety** - Built-in support for strings, integers, floats, booleans
- **Multi-Value Collections** - Arrays and maps for complex data
- **Nested Subcommands** - Git/Docker-style command hierarchies
- **Input Validation** - Range, regex, and custom validators
- **Error Handling** - Clear, informative error messages
- **Environment Variables** - Auto-load options from environment
- **Help Generation** - Beautiful, formatted help output

## Quick Example

```c
#include "argus.h"
#include <stdio.h>

// Define options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_INT('p', "port", HELP("Port number"), RANGE(1, 65535), DEFAULT(8080)),
    POSITIONAL_STRING("input", HELP("Input file"))
)

int main(int argc, char **argv)
{
    // Initialize argus
    argus_t argus = argus_init(options, "my_program", "1.0.0");
    argus.description = "argus demonstrator";

    // Parse arguments
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS) {
        return 1;
    }

    // Access parsed values
    const char *input = argus_get(argus, "input").as_string;
    const char *output = argus_get(argus, "output").as_string;
    int port = argus_get(argus, "port").as_int;
    bool verbose = argus_get(argus, "verbose").as_bool;

    printf("Configuration:\n");
    printf("  Input: %s\n", input);
    printf("  Output: %s\n", output);
    printf("  Port: %d\n", port);
    printf("  Verbose: %s\n", verbose ? "yes" : "no");

    // Free resources
    argus_free(&argus);
    return 0;
}
```

## Automatic Help Display

When users invoke your program with `--help`, argus automatically generates a formatted help message like this:

```
my_program v1.0.0

argus demonstrator

Usage: my_program [OPTIONS] <input>

Arguments:
  <input>                - Input file

Options:
  -h, --help             - Display this help message (exit)
  -V, --version          - Display version information (exit)
  -v, --verbose          - Enable verbose output
  -o, --output <STR>     - Output file (default: "output.txt")
  -p, --port <NUM>       - Port number [1-65535] (default: 8080)
```

## Getting Help

- **[:material-help-circle: User Guide](guide/installation.md)** - Learn how to use argus step by step
- **[:material-code-tags: API Reference](api/overview.md)** - Explore the complete API documentation
- **[:material-github: GitHub](https://github.com/lucocozz/argus)** - Report issues or contribute to development

## Community

We welcome contributions and feedback from the community:

- **[:octicons-git-pull-request-24: Contribute](contributing.md)** - Help improve argus
- **[:octicons-law-24: License](license.md)** - MIT License