# Quickstart

Get up and running with Argus in minutes. This guide walks you through creating your first CLI tool.

## Your First CLI

Let's build a simple file processor that demonstrates Argus's core features:

```c title="my_tool.c"
#include <argus.h>
#include <stdio.h>

// Define your CLI interface declaratively
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),                    // Automatic --help/-h
    VERSION_OPTION(),                 // Automatic --version/-V
    
    OPTION_FLAG('v', "verbose", 
        HELP("Enable verbose output")),
    
    OPTION_STRING('o', "output", 
        HELP("Output file"), 
        DEFAULT("result.txt")),
    
    OPTION_INT('c', "count", 
        HELP("Number of iterations"), 
        DEFAULT(1),
        VALIDATOR(V_RANGE(1, 100))),   // Built-in validation
    
    POSITIONAL_STRING("input", 
        HELP("Input file to process")),
)

int main(int argc, char **argv)
{
    // Initialize with program info
    argus_t argus = argus_init(options, "my_tool", "1.0.0");
    argus.description = "A simple file processing tool";
    
    // Parse arguments - handles errors automatically
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS) {
        return 1;
    }
    
    // Access parsed values with type safety
    const char *input = argus_get(argus, "input").as_string;
    const char *output = argus_get(argus, "output").as_string;
    int count = argus_get(argus, "count").as_int;
    bool verbose = argus_get(argus, "verbose").as_bool;
    
    // Your application logic
    if (verbose) {
        printf("Processing %s -> %s (%d times)\n", input, output, count);
    }
    
    printf("File processed successfully!\n");
    
    // Cleanup
    argus_free(&argus);
    return 0;
}
```

## Build and Test

```bash
# Development build (recommended during development)
gcc my_tool.c -o my_tool -largus
./my_tool --help    # See auto-generated help
./my_tool input.txt # Run your tool

# Production build (skip validation for better performance)
gcc -DARGUS_RELEASE my_tool.c -o my_tool-prod -largus
```

:::tip Production Note
Use `ARGUS_RELEASE` only in production after thorough testing. It disables option structure validation for faster startup time.
:::

## What You Get For Free

### Automatic Help Generation

```bash
$ ./my_tool --help
my_tool v1.0.0

A simple file processing tool

Usage: my_tool [OPTIONS] <input>

Arguments:
  <input>                - Input file to process

Options:
  -h, --help             - Display this help message (exit)
  -V, --version          - Display version information (exit)
  -v, --verbose          - Enable verbose output
  -o, --output <STR>     - Output file (default: "result.txt")
  -c, --count <NUM>      - Number of iterations [1-100] (default: 1)
```

### Input Validation

```bash
$ ./my_tool --count 150 input.txt
my_tool: Value 150 is out of range [1, 100]
```

### Flexible Input Formats

All equivalent:
```bash
./my_tool --output=file.txt --count=5 input.txt
./my_tool -o file.txt -c 5 input.txt  
./my_tool -ofile.txt -c5 input.txt
```

## Key Concepts

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
<TabItem value="options" label="Options" default>

**Options** are named arguments with `--` or `-` prefix:

```c
// Flag (boolean, no value needed)
OPTION_FLAG('v', "verbose", HELP("Enable verbose mode"))

// String option with default value
OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("out.txt"))

// Integer with validation
OPTION_INT('p', "port", HELP("Port number"), 
           VALIDATOR(V_RANGE(1, 65535)))
```

</TabItem>
<TabItem value="positionals" label="Positionals">

**Positional arguments** don't need prefixes:

```c
// Required positional (must be provided)
POSITIONAL_STRING("input", HELP("Input file"))

// Optional positional with default
POSITIONAL_INT("buffer_size", HELP("Buffer size"), 
               FLAGS(FLAG_OPTIONAL), DEFAULT(1024))
```

Usage:
```bash
./tool input.txt          # Required only
./tool input.txt 2048     # Both provided
```

</TabItem>
<TabItem value="validation" label="Validation">

**Built-in validators** ensure correct input:

```c
// Range validation
OPTION_INT('p', "port", HELP("Port"), VALIDATOR(V_RANGE(1, 65535)))

// String length
OPTION_STRING('u', "user", HELP("Username"), VALIDATOR(V_LENGTH(3, 20)))

// Regex patterns (requires PCRE2)
OPTION_STRING('e', "email", HELP("Email"), VALIDATOR(V_REGEX(ARGUS_RE_EMAIL)))

// Multiple choices
OPTION_STRING('l', "level", HELP("Log level"), 
              VALIDATOR(V_CHOICE_STR("debug", "info", "warn", "error")))
```

</TabItem>
</Tabs>

## Next Steps

### Add Subcommands

Transform your tool into a multi-command interface like `git` or `docker`:

```c
// Define subcommand options
ARGUS_OPTIONS(process_options,
    HELP_OPTION(),
    OPTION_FLAG('f', "force", HELP("Force processing")),
    POSITIONAL_STRING("file", HELP("File to process")),
)

// Add to main options
ARGUS_OPTIONS(options,
    HELP_OPTION(),
    VERSION_OPTION(),
    SUBCOMMAND("process", process_options, 
               HELP("Process files"), 
               ACTION(process_command)),
)
```

### Handle Collections

Work with multiple values and key-value pairs:

```c
// Array of strings: --tags=web,api,backend
OPTION_ARRAY_STRING('t', "tags", HELP("Tags"))

// Key-value map: --env=DEBUG=1,LOG_LEVEL=info  
OPTION_MAP_STRING('e', "env", HELP("Environment variables"))
```

### 🔧 Advanced Features

- **Environment Variables**: Auto-load from env vars
- **Custom Handlers**: Parse complex data types
- **Nested Commands**: Multi-level command hierarchies
- **Dependency Management**: Require/conflict between options

## Common Patterns

<Tabs>
<TabItem value="simple" label="Simple Utility" default>

```c
ARGUS_OPTIONS(options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file")),
    POSITIONAL_STRING("input", HELP("Input file")),
)
```

Perfect for file processors, converters, simple tools.

</TabItem>
<TabItem value="server" label="Server Tool">

```c
ARGUS_OPTIONS(options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_STRING('H', "host", HELP("Bind address"), DEFAULT("0.0.0.0")),
    OPTION_INT('p', "port", HELP("Port"), DEFAULT(8080), 
               VALIDATOR(V_RANGE(1, 65535))),
    OPTION_STRING('c', "config", HELP("Config file")),
    OPTION_FLAG('d', "daemon", HELP("Run as daemon")),
)
```

Network services, web servers, daemons.

</TabItem>
<TabItem value="build" label="Build Tool">

```c
ARGUS_OPTIONS(options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('t', "target", HELP("Build target"), DEFAULT("all")),
    OPTION_INT('j', "jobs", HELP("Parallel jobs"), DEFAULT(1)),
    OPTION_FLAG('f', "force", HELP("Force rebuild")),
)
```

Build systems, compilers, deployment tools.

</TabItem>
</Tabs>

:::tip
Every Argus program automatically gets `--help` and proper error messages. Focus on your application logic, not argument parsing!
:::
