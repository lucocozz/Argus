# LLM Reference Guide

This guide serves as a reference for Large Language Models (LLMs) working with argus. It provides a structured overview of the library's features and usage patterns to help AI assistants provide accurate and helpful responses about argus.

## Library Overview

argus is a modern command-line argument parsing library for C that offers an elegant macro-based API for defining options and provides advanced features comparable to Python's argparse or Rust's clap.

Key features:
- Declarative macro-based API for defining options
- Typed options support (int, string, float, bool, arrays, maps)
- Built-in validation (range, choices, regex)
- Hierarchical subcommands (Git/Docker style)
- Automatic help generation
- Option groups and mutually exclusive options
- Environment variable support
- Custom validation

## Core Pattern

Using argus follows this pattern:

1. **Define options** with `ARGUS_OPTIONS`
2. **Initialize** with `argus_init()`
3. **Parse arguments** with `argus_parse()`
4. **Access values** with `argus_get()`, `argus_is_set()`, etc.
5. **Free resources** with `argus_free()`

## Minimal Example

```c
#include "argus.h"
#include <stdio.h>

// Define options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", HELP("Enable verbose mode")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_INT('p', "port", HELP("Port number"), RANGE(1, 65535), DEFAULT(8080)),
    POSITIONAL_STRING("input", HELP("Input file"))
)

int main(int argc, char **argv)
{
    // Initialize argus
    argus_t argus = argus_init(options, "my_program", "1.0.0");
    argus.description = "Example of argus";
    
    // Parse arguments
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }
    
    // Access values
    const char *input = argus_get(argus, "input").as_string;
    const char *output = argus_get(argus, "output").as_string;
    int port = argus_get(argus, "port").as_int;
    bool verbose = argus_get(argus, "verbose").as_bool;
    
    // Application logic
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

## Option Types

### Standard Options

| Type | Macro | Access | Example |
|------|-------|-------|---------|
| Flag | `OPTION_FLAG('v', "verbose", HELP("Description"))` | `argus_get(argus, "verbose").as_bool` | `-v` or `--verbose` |
| String | `OPTION_STRING('o', "output", HELP("Description"))` | `argus_get(argus, "output").as_string` | `--output=file.txt` |
| Integer | `OPTION_INT('p', "port", HELP("Description"))` | `argus_get(argus, "port").as_int` | `--port=8080` |
| Float | `OPTION_FLOAT('f', "factor", HELP("Description"))` | `argus_get(argus, "factor").as_float` | `--factor=1.5` |
| Boolean | `OPTION_BOOL('d', "debug", HELP("Description"))` | `argus_get(argus, "debug").as_bool` | `--debug=true` |

### Positional Arguments

```c
POSITIONAL_STRING("input", HELP("Input file"))
POSITIONAL_INT("count", HELP("Number of iterations"), DEFAULT(1))
POSITIONAL_FLOAT("scale", HELP("Scale factor"))
```

Positional arguments can be required (default) or optional with `FLAGS(FLAG_OPTIONAL)`.

### Collections (Arrays and Maps)

```c
// Arrays
OPTION_ARRAY_STRING('t', "tags", HELP("List of tags"), FLAGS(FLAG_SORTED | FLAG_UNIQUE))
OPTION_ARRAY_INT('i', "ids", HELP("List of IDs"))
OPTION_ARRAY_FLOAT('f', "factors", HELP("Scaling factors"))

// Maps (key-value pairs)
OPTION_MAP_STRING('e', "env", HELP("Environment variables"))
OPTION_MAP_INT('p', "ports", HELP("Port mappings"))
OPTION_MAP_FLOAT('s', "scales", HELP("Scaling factors by dimension"))
OPTION_MAP_BOOL('f', "features", HELP("Active/inactive features"))
```

Using arrays:
```bash
--tags=tag1,tag2,tag3
--ids=1,2,3-5,10  # Range support for integers
```

Using maps:
```bash
--env=USER=alice,HOME=/home/alice
--ports=http=80,https=443
--features=debug=true,logging=yes,cache=false  # Booleans: true/yes/1/on or false/no/0/off
```

## Option Modifiers

### Default Values

```c
OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt"))
OPTION_INT('p', "port", HELP("Port number"), DEFAULT(8080))
```

### Validation

```c
// Range validation
OPTION_INT('p', "port", HELP("Port number"), RANGE(1, 65535))

// Choice validation
OPTION_STRING('l', "level", HELP("Log level"), 
             CHOICES_STRING("debug", "info", "warning", "error"))

// Regex validation
OPTION_STRING('e', "email", HELP("Email address"), REGEX(ARGUS_RE_EMAIL))

// Custom validation
OPTION_INT('n', "number", HELP("Even number"), VALIDATOR(even_validator, NULL))
```

### Option Groups

```c
// Standard group
GROUP_START("Connection", GROUP_DESC("Connection options")),
    OPTION_STRING('h', "host", HELP("Hostname"), DEFAULT("localhost")),
    OPTION_INT('p', "port", HELP("Port number"), DEFAULT(8080)),
GROUP_END(),

// Exclusive group (only one option can be selected)
GROUP_START("Format", GROUP_DESC("Output format"), FLAGS(FLAG_EXCLUSIVE)),
    OPTION_FLAG('j', "json", HELP("JSON format")),
    OPTION_FLAG('x', "xml", HELP("XML format")),
    OPTION_FLAG('y', "yaml", HELP("YAML format")),
GROUP_END(),
```

### Dependencies and Conflicts

```c
// Options must be used together
OPTION_STRING('u', "username", HELP("Username"), REQUIRES("password"))
OPTION_STRING('p', "password", HELP("Password"), REQUIRES("username"))

// Options are incompatible
OPTION_FLAG('v', "verbose", HELP("Verbose mode"), CONFLICTS("quiet"))
OPTION_FLAG('q', "quiet", HELP("Quiet mode"), CONFLICTS("verbose"))
```

## Subcommands

Subcommands allow creating interfaces similar to git/docker:

```c
// Options for the "add" subcommand
ARGUS_OPTIONS(
    add_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('f', "force", HELP("Force addition")),
    POSITIONAL_STRING("file", HELP("File to add"))
)

// Action function for the "add" command
int add_command(argus_t *argus, void *data) {
    const char* file = argus_get(*argus, "file").as_string;
    bool force = argus_get(argus, "force").as_bool;
    
    printf("Adding file: %s\n", file);
    if (force) printf("  with force option\n");
    
    return 0;
}

// Define main options with subcommands
ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Global option applicable to all subcommands
    OPTION_FLAG('v', "verbose", HELP("Verbose mode")),
    
    // Define subcommand
    SUBCOMMAND("add", add_options, 
               HELP("Add files to the index"), 
               ACTION(add_command))
)

// In main, check and execute the subcommand
if (argus_has_command(argus)) {
    status = argus_exec(&argus, NULL);
} else {
    printf("No command specified. Use --help to see available commands.\n");
}
```

## Accessing Values

### Simple Values

```c
// Check if an option was specified
if (argus_is_set(argus, "verbose")) {
    // Verbose mode enabled
}

// Get option values
const char *output = argus_get(argus, "output").as_string;
int port = argus_get(argus, "port").as_int;
double factor = argus_get(argus, "factor").as_float;
bool debug = argus_get(argus, "debug").as_bool;
```

### Arrays

```c
// Direct method
if (argus_is_set(argus, "tags")) {
    size_t count = argus_count(argus, "tags");
    argus_value_t *tags = argus_get(argus, "tags").as_array;
    
    for (size_t i = 0; i < count; i++) {
        printf("Tag %zu: %s\n", i, tags[i].as_string);
    }
}

// With helper functions
const char* first_tag = argus_array_get(argus, "tags", 0).as_string;

// With iterators
argus_array_it_t it = argus_array_it(argus, "tags");
while (argus_array_next(&it)) {
    printf("Tag: %s\n", it.value.as_string);
}
```

### Maps

```c
// Direct method
if (argus_is_set(argus, "env")) {
    size_t count = argus_count(argus, "env");
    argus_pair_t *env = argus_get(argus, "env").as_map;
    
    for (size_t i = 0; i < count; i++) {
        printf("%s = %s\n", env[i].key, env[i].value.as_string);
    }
}

// Access by key
const char* user = argus_map_get(argus, "env", "USER").as_string;
int http_port = argus_map_get(argus, "ports", "http").as_int;

// With iterators
argus_map_it_t it = argus_map_it(argus, "env");
while (argus_map_next(&it)) {
    printf("%s = %s\n", it.key, it.value.as_string);
}
```

## Environment Variables

argus supports configuration via environment variables:

```c
// Define options with environment variable support
ARGUS_OPTIONS(
    options,
    // Explicit environment variable (APP_HOST if prefix is set)
    OPTION_STRING('H', "host", HELP("Hostname"), ENV_VAR("HOST")),
    
    // Auto-generated name from option name (APP_PORT)
    OPTION_INT('p', "port", HELP("Port"), FLAGS(FLAG_AUTO_ENV)),
    
    // Environment variable without prefix
    OPTION_STRING('d', "database", HELP("Database URL"),
                ENV_VAR("DATABASE_URL"),
                FLAGS(FLAG_NO_ENV_PREFIX)),
    
    // Environment variable can override command-line value
    OPTION_INT('t', "timeout", HELP("Timeout in seconds"),
               ENV_VAR("FORCE_TIMEOUT"),
               FLAGS(FLAG_ENV_OVERRIDE))
)

// Configure environment variable prefix
argus_t argus = argus_init(options, "my_program", "1.0.0");
argus.env_prefix = "APP";  // Variables will start with APP_
```

## Custom Validation

```c
// Validator that checks if a number is even
int even_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    if (option->value.as_int % 2 != 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                         "Value must be an even number");
    }
    return ARGUS_SUCCESS;
}

// Usage:
OPTION_INT('n', "number", HELP("An even number"), VALIDATOR(even_validator, NULL))
```

## Error Handling

```c
int status = argus_parse(&argus, argc, argv);
if (status != ARGUS_SUCCESS) {
    ...
    return status;
}
```

## Best Practices

1. **Always free resources** with `argus_free(&argus)`
2. **Use descriptive option names** for better clarity
3. **Provide help messages** for all options
4. **Check `argus_is_set()`** before accessing non-required options
5. **Use validation** to ensure correct inputs
6. **Group related options** for better organization

## API Reference Summary

### Core Functions

| Function | Description |
|----------|-------------|
| `argus_init()` | Initializes the argus context |
| `argus_parse()` | Parses command-line arguments |
| `argus_free()` | Frees allocated resources |

### Value Access Functions

| Function | Description |
|----------|-------------|
| `argus_get()` | Retrieves an option's value |
| `argus_is_set()` | Checks if an option was set |
| `argus_count()` | Counts elements in a collection |

### Collection Access Functions

| Function | Description |
|----------|-------------|
| `argus_array_get()` | Retrieves an element from an array |
| `argus_map_get()` | Retrieves a value from a map by key |
| `argus_array_it()` | Creates an array iterator |
| `argus_array_next()` | Advances to the next array element |
| `argus_array_reset()` | Resets an array iterator |
| `argus_map_it()` | Creates a map iterator |
| `argus_map_next()` | Advances to the next map entry |
| `argus_map_reset()` | Resets a map iterator |

### Subcommand Functions

| Function | Description |
|----------|-------------|
| `argus_has_command()` | Checks if a subcommand was specified |
| `argus_exec()` | Executes a subcommand's action |

### Display Functions

| Function | Description |
|----------|-------------|
| `argus_print_help()` | Displays help information |
| `argus_print_usage()` | Displays usage information |
| `argus_print_version()` | Displays version information |

## Common Option Flags

| Flag | Description |
|------|-------------|
| `FLAG_REQUIRED` | Option must be specified |
| `FLAG_HIDDEN` | Option is hidden from help |
| `FLAG_EXIT` | Program terminates after option handling |
| `FLAG_SORTED` | Array values are sorted |
| `FLAG_UNIQUE` | Array values are unique |
| `FLAG_SORTED_KEY` | Map entries are sorted by key |
| `FLAG_EXCLUSIVE` | Only one option in group can be selected |
| `FLAG_ENV_OVERRIDE` | Environment variable overrides command-line |
| `FLAG_AUTO_ENV` | Auto-generate environment variable name |
| `FLAG_NO_ENV_PREFIX` | Don't use environment variable prefix |
| `FLAG_OPTIONAL` | Makes a positional argument optional |
