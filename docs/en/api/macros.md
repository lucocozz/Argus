# Macros Reference

This reference provides a comprehensive guide to all macros available in argus, organized by function and purpose.

## Option Definition Macros

These macros are the building blocks for defining command-line options and their behavior.

### Core Definitions

| Macro | Purpose | Example |
|-------|---------|---------|
| `ARGUS_OPTIONS(name, ...)` | Define a set of command-line options | `ARGUS_OPTIONS(options, HELP_OPTION(), ...)` |
| `OPTION_END()` | Terminate an options array | Usually added automatically |

### Standard Options

These macros define options that accept different types of values:

| Option Type | Macro | Description | Example |
|-------------|-------|-------------|---------|
| **String** | `OPTION_STRING(short, long, help, ...)` | Option with string value | `OPTION_STRING('o', "output", HELP("Output file"))` |
| **Integer** | `OPTION_INT(short, long, help, ...)` | Option with integer value | `OPTION_INT('p', "port", HELP("Port"))` |
| **Float** | `OPTION_FLOAT(short, long, help, ...)` | Option with float value | `OPTION_FLOAT('s', "scale", HELP("Scale"))` |
| **Boolean** | `OPTION_BOOL(short, long, help, ...)` | Option with true/false value | `OPTION_BOOL('d', "debug", HELP("Debug mode"))` |
| **Flag** | `OPTION_FLAG(short, long, help, ...)` | Boolean flag (no value needed) | `OPTION_FLAG('v', "verbose", HELP("Verbose"))` |

### Array Options

These macros define options that can accept multiple values:

| Array Type | Macro | Description | Example |
|------------|-------|-------------|---------|
| **String Array** | `OPTION_ARRAY_STRING(short, long, help, ...)` | Multiple string values | `OPTION_ARRAY_STRING('t', "tags", HELP("Tags"))` |
| **Integer Array** | `OPTION_ARRAY_INT(short, long, help, ...)` | Multiple integer values | `OPTION_ARRAY_INT('p', "ports", HELP("Ports"))` |
| **Float Array** | `OPTION_ARRAY_FLOAT(short, long, help, ...)` | Multiple float values | `OPTION_ARRAY_FLOAT('f', "factors", HELP("Factors"))` |

### Map Options

These macros define options that accept key-value pairs:

| Map Type | Macro | Description | Example |
|----------|-------|-------------|---------|
| **String Map** | `OPTION_MAP_STRING(short, long, help, ...)` | Key-value with string values | `OPTION_MAP_STRING('e', "env", HELP("Environment"))` |
| **Integer Map** | `OPTION_MAP_INT(short, long, help, ...)` | Key-value with integer values | `OPTION_MAP_INT('p', "port", HELP("Port mapping"))` |
| **Float Map** | `OPTION_MAP_FLOAT(short, long, help, ...)` | Key-value with float values | `OPTION_MAP_FLOAT('s', "scale", HELP("Scaling"))` |
| **Boolean Map** | `OPTION_MAP_BOOL(short, long, help, ...)` | Key-value with boolean values | `OPTION_MAP_BOOL('f', "feature", HELP("Features"))` |

### Positional Arguments

These macros define positional arguments (without dashes):

| Positional Type | Macro | Description | Example |
|-----------------|-------|-------------|---------|
| **String** | `POSITIONAL_STRING(name, help, ...)` | Positional with string value | `POSITIONAL_STRING("input", HELP("Input file"))` |
| **Integer** | `POSITIONAL_INT(name, help, ...)` | Positional with integer value | `POSITIONAL_INT("count", HELP("Count"))` |
| **Float** | `POSITIONAL_FLOAT(name, help, ...)` | Positional with float value | `POSITIONAL_FLOAT("factor", HELP("Factor"))` |
| **Boolean** | `POSITIONAL_BOOL(name, help, ...)` | Positional with boolean value | `POSITIONAL_BOOL("enabled", HELP("Enable"))` |

### Common Options

Pre-defined options for standard functionality:

| Option | Macro | Description | Example |
|--------|-------|-------------|---------|
| **Help** | `HELP_OPTION(...)` | Adds `-h/--help` option | `HELP_OPTION(FLAGS(FLAG_EXIT))` |
| **Version** | `VERSION_OPTION(...)` | Adds `-V/--version` option | `VERSION_OPTION(FLAGS(FLAG_EXIT))` |

### Base Options

Advanced macros for custom option types:

| Base Type | Macro | Purpose | Example |
|-----------|-------|---------|---------|
| **Option Base** | `OPTION_BASE(short, long, type, ...)` | Custom option type | `OPTION_BASE('i', "ip", VALUE_TYPE_CUSTOM, ...)` |
| **Positional Base** | `POSITIONAL_BASE(name, type, ...)` | Custom positional type | `POSITIONAL_BASE("coord", VALUE_TYPE_CUSTOM, ...)` |

## Validation Macros

These macros add validation constraints to options:

| Validator | Macro | Purpose | Example |
|-----------|-------|---------|---------|
| **Range** | `RANGE(min, max)` | Validates numeric values within range | `OPTION_INT('p', "port", HELP("Port"), RANGE(1, 65535))` |
| **Length** | `LENGTH(min, max)` | Validates string length within range | `OPTION_STRING('u', "user", HELP("Username"), LENGTH(3, 20))` |
| **Count** | `COUNT(min, max)` | Validates collection size within range | `OPTION_ARRAY_INT('n', "nums", HELP("Numbers"), COUNT(1, 5))` |
| **Regex** | `REGEX(pattern)` | Validates text against a pattern | `OPTION_STRING('e', "email", HELP("Email"), REGEX(ARGUS_RE_EMAIL))` |
| **Custom Pattern** | `MAKE_REGEX(pattern, hint)` | Creates a regex pattern with explanation | `REGEX(MAKE_REGEX("^[A-Z]{2}\\d{4}$", "Format: XX0000"))` |
| **Custom Validator** | `VALIDATOR(function, data)` | Custom validation logic | `VALIDATOR(even_validator, NULL)` |
| **Pre-validator** | `PRE_VALIDATOR(function, data)` | Validates raw string before processing | `PRE_VALIDATOR(length_validator, &min_length)` |

### Choices Validators

These macros validate against a set of allowed values:

| Choices Type | Macro | Purpose | Example |
|--------------|-------|---------|---------|
| **String Choices** | `CHOICES_STRING(...)` | Allows specific string values | `CHOICES_STRING("debug", "info", "warning", "error")` |
| **Integer Choices** | `CHOICES_INT(...)` | Allows specific integer values | `CHOICES_INT(1, 2, 3, 4)` |
| **Float Choices** | `CHOICES_FLOAT(...)` | Allows specific float values | `CHOICES_FLOAT(0.5, 1.0, 2.0)` |

## Option Property Macros

These macros modify option properties:

| Property | Macro | Purpose | Example |
|----------|-------|---------|---------|
| **Default Value** | `DEFAULT(value)` | Sets default value | `DEFAULT("output.txt")` |
| **Help Text** | `HELP(text)` | Sets help description | `HELP("Output file path")` |
| **Value Hint** | `HINT(text)` | Sets hint for help display | `HINT("FILE")` |
| **Flags** | `FLAGS(flags)` | Sets behavior flags | `FLAGS(FLAG_REQUIRED)` |
| **Requirements** | `REQUIRES(...)` | Defines dependent options | `REQUIRES("username", "password")` |
| **Conflicts** | `CONFLICTS(...)` | Defines incompatible options | `CONFLICTS("quiet")` |
| **Environment Variable** | `ENV_VAR(name)` | Sets environment variable | `ENV_VAR("OUTPUT")` |

## Group and Subcommand Macros

These macros organize options and define command hierarchies:

| Macro | Purpose | Example |
|-------|---------|---------|
| `GROUP_START(name, ...)` | Starts an option group | `GROUP_START("Connection", GROUP_DESC("Connection options"))` |
| `GROUP_END()` | Ends an option group | `GROUP_END()` |
| `GROUP_DESC(text)` | Sets group description | `GROUP_DESC("Advanced settings")` |
| `SUBCOMMAND(name, options, ...)` | Defines a subcommand | `SUBCOMMAND("add", add_options, HELP("Add files"))` |
| `ACTION(function)` | Sets subcommand action | `ACTION(add_command)` |

## Handler Macros

These macros define how options process input:

| Macro | Purpose | Example |
|-------|---------|---------|
| `HANDLER(function)` | Custom processing logic | `HANDLER(ip_handler)` |
| `FREE_HANDLER(function)` | Resource cleanup | `FREE_HANDLER(ip_free_handler)` |

## Complete Example

Here's a comprehensive example showing various macros in use:

```c
// Define options for the "add" subcommand
ARGUS_OPTIONS(
    add_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('f', "force", HELP("Force add operation"), 
               CONFLICTS("dry-run")),
    POSITIONAL_STRING("file", HELP("File to add"))
)

// Define main options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Standard options with validation
    OPTION_STRING('o', "output", HELP("Output file"), 
                 DEFAULT("output.txt"), 
                 HINT("FILE"),
                 LENGTH(1, 100)),  // Validate string length
    
    OPTION_INT('p', "port", HELP("Port number"), 
              RANGE(1, 65535),     // Validate numeric range
              DEFAULT(8080)),
    
    // Array with count validation
    OPTION_ARRAY_INT('n', "numbers", HELP("List of numbers"),
                    COUNT(1, 10),   // Validate collection size
                    FLAGS(FLAG_SORTED | FLAG_UNIQUE)),
    
    // Option groups
    GROUP_START("Display", GROUP_DESC("Display options")),
        OPTION_FLAG('q', "quiet", HELP("Suppress output"), 
                   CONFLICTS("verbose")),
        OPTION_FLAG('c', "color", HELP("Colorize output")),
    GROUP_END(),
    
    // Exclusive group
    GROUP_START("Format", GROUP_DESC("Output format"), 
                FLAGS(FLAG_EXCLUSIVE)),
        OPTION_FLAG('j', "json", HELP("JSON output")),
        OPTION_FLAG('x', "xml", HELP("XML output")),
        OPTION_FLAG('y', "yaml", HELP("YAML output")),
    GROUP_END(),
    
    // Subcommands
    SUBCOMMAND("add", add_options, 
               HELP("Add files to the index"), 
               ACTION(add_command))
)
```

For more detailed information about specific macros, see the relevant sections of this guide.
