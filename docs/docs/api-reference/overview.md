# API Reference Overview

Complete reference for all Argus functions, macros, and types.

## Core Functions

### Initialization and Parsing

| Function | Purpose | Returns |
|----------|---------|---------|
| `argus_init()` | Initialize argus context | `argus_t` |
| `argus_parse()` | Parse command-line arguments | `int` status |
| `argus_free()` | Clean up and free resources | `void` |

### Value Access

| Function | Purpose | Returns |
|----------|---------|---------|
| `argus_get()` | Get option value | `argus_value_t` |
| `argus_is_set()` | Check if option was provided | `bool` |
| `argus_count()` | Get number of values (collections) | `size_t` |

### Collection Access

| Function | Purpose | Returns |
|----------|---------|---------|
| `argus_array_get()` | Get array element by index | `argus_value_t` |
| `argus_map_get()` | Get map value by key | `argus_value_t` |
| `argus_array_it()` | Create array iterator | `argus_array_it_t` |
| `argus_map_it()` | Create map iterator | `argus_map_it_t` |

### Display Functions

| Function | Purpose | Returns |
|----------|---------|---------|
| `argus_print_help()` | Display help text | `void` |
| `argus_print_usage()` | Display usage line | `void` |
| `argus_print_version()` | Display version info | `void` |

### Subcommand Functions

| Function | Purpose | Returns |
|----------|---------|---------|
| `argus_has_command()` | Check if subcommand parsed | `bool` |
| `argus_exec()` | Execute parsed subcommand | `int` |

## Option Definition Macros

### Basic Options

```c
OPTION_FLAG(short, long, ...)        // Boolean flag
OPTION_BOOL(short, long, ...)        // Explicit boolean
OPTION_STRING(short, long, ...)      // String value
OPTION_INT(short, long, ...)         // Integer value
OPTION_FLOAT(short, long, ...)       // Float value
```

### Collections

```c
OPTION_ARRAY_STRING(short, long, ...)  // String array
OPTION_ARRAY_INT(short, long, ...)     // Integer array
OPTION_ARRAY_FLOAT(short, long, ...)   // Float array

OPTION_MAP_STRING(short, long, ...)    // String map
OPTION_MAP_INT(short, long, ...)       // Integer map
OPTION_MAP_FLOAT(short, long, ...)     // Float map
OPTION_MAP_BOOL(short, long, ...)      // Boolean map
```

### Positional Arguments

```c
POSITIONAL_STRING(name, ...)         // String positional
POSITIONAL_INT(name, ...)            // Integer positional
POSITIONAL_BOOL(name, ...)           // Boolean positional
POSITIONAL_FLOAT(name, ...)          // Float positional
```

### Structure Macros

```c
ARGUS_OPTIONS(name, ...)             // Define options array
HELP_OPTION()                        // Standard help option
VERSION_OPTION()                     // Standard version option
SUBCOMMAND(name, options, ...)       // Define subcommand
GROUP_START(name, ...)               // Start option group
GROUP_END()                          // End option group
```

## Option Modifiers

### Core Modifiers

```c
HELP("description")                  // Help text
DEFAULT(value)                       // Default value
HINT("hint")                         // Value hint for help
FLAGS(flags)                         // Option flags
```

### Dependencies

```c
REQUIRES("opt1", "opt2", ...)        // Required dependencies
CONFLICTS("opt1", "opt2", ...)       // Conflicting options
```

### Environment Variables

```c
ENV_VAR("VAR_NAME")                  // Environment variable name
```

### Validation

```c
VALIDATOR(V_RANGE(min, max))         // Range validation
VALIDATOR(V_LENGTH(min, max))        // String length
VALIDATOR(V_COUNT(min, max))         // Collection count
VALIDATOR(V_REGEX(pattern))          // Regex validation
```

### Choice Validation

```c
CHOICES_STRING("opt1", "opt2", ...)  // String choices
CHOICES_INT(1, 2, 3, ...)            // Integer choices
CHOICES_FLOAT(0.1, 0.5, 1.0, ...)    // Float choices
```

## Core Types

### Main Context

```c
typedef struct argus_s argus_t;      // Main library context
```

### Option Structure

```c
typedef struct argus_option_s argus_option_t;  // Option definition
```

### Value Types

```c
typedef union argus_value_u argus_value_t;     // Value container
typedef struct argus_pair_s argus_pair_t;      // Key-value pair
```

### Iterators

```c
typedef struct argus_array_iterator_s argus_array_it_t;  // Array iterator
typedef struct argus_map_iterator_s argus_map_it_t;      // Map iterator
```

## Value Type Enumeration

```c
typedef enum argus_valtype_e {
    VALUE_TYPE_NONE,
    VALUE_TYPE_INT,
    VALUE_TYPE_STRING,
    VALUE_TYPE_FLOAT,
    VALUE_TYPE_BOOL,
    VALUE_TYPE_FLAG,
    
    VALUE_TYPE_ARRAY_STRING,
    VALUE_TYPE_ARRAY_INT,
    VALUE_TYPE_ARRAY_FLOAT,
    
    VALUE_TYPE_MAP_STRING,
    VALUE_TYPE_MAP_INT,
    VALUE_TYPE_MAP_FLOAT,
    VALUE_TYPE_MAP_BOOL,
    
    VALUE_TYPE_CUSTOM
} argus_valtype_t;
```

## Option Flags

```c
typedef enum argus_optflags_e {
    FLAG_NONE = 0,
    FLAG_REQUIRED,           // Option must be specified
    FLAG_HIDDEN,             // Hidden from help
    FLAG_ADVANCED,           // Advanced option
    FLAG_DEPRECATED,         // Deprecated option
    FLAG_EXPERIMENTAL,       // Experimental option
    FLAG_EXIT,               // Exit after handler
    FLAG_ENV_OVERRIDE,       // Env overrides CLI
    FLAG_AUTO_ENV,           // Auto-generate env name
    FLAG_NO_ENV_PREFIX,      // Skip env prefix
    
    // Collection flags
    FLAG_SORTED,             // Sort array values
    FLAG_UNIQUE,             // Unique array values
    FLAG_SORTED_VALUE,       // Sort map by values
    FLAG_SORTED_KEY,         // Sort map by keys
    FLAG_UNIQUE_VALUE,       // Unique map values
    
    // Group flags
    FLAG_EXCLUSIVE           // Exclusive group
} argus_optflags_t;
```

## Error Codes

```c
typedef enum argus_error_type_e {
    ARGUS_SUCCESS = 0,
    ARGUS_SOULD_EXIT,
    
    // Structure errors
    ARGUS_ERROR_DUPLICATE_OPTION,
    ARGUS_ERROR_INVALID_HANDLER,
    ARGUS_ERROR_INVALID_DEFAULT,
    ARGUS_ERROR_INVALID_GROUP,
    ARGUS_ERROR_INVALID_DEPENDENCY,
    ARGUS_ERROR_INVALID_FLAG,
    ARGUS_ERROR_INVALID_POSITION,
    ARGUS_ERROR_MALFORMED_OPTION,
    ARGUS_ERROR_MISSING_HELP,
    
    // Parsing errors
    ARGUS_ERROR_INVALID_ARGUMENT,
    ARGUS_ERROR_MISSING_VALUE,
    ARGUS_ERROR_MISSING_REQUIRED,
    ARGUS_ERROR_CONFLICTING_OPTIONS,
    ARGUS_ERROR_INVALID_FORMAT,
    ARGUS_ERROR_EXCLUSIVE_GROUP,
    ARGUS_ERROR_INVALID_CHOICE,
    ARGUS_ERROR_INVALID_RANGE,
    
    // Execution errors
    ARGUS_ERROR_NO_COMMAND,
    
    // Internal errors
    ARGUS_ERROR_MEMORY,
    ARGUS_ERROR_INTERNAL,
    ARGUS_ERROR_UNSUPPORTED,
    ARGUS_ERROR_INVALID_VALUE,
    ARGUS_ERROR_STACK_OVERFLOW
} argus_error_type_t;
```

## Quick Reference

### Basic Program Structure

```c
#include <argus.h>

ARGUS_OPTIONS(options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file")),
    POSITIONAL_STRING("input", HELP("Input file"))
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "program", "1.0.0");
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    bool verbose = argus_get(argus, "verbose").as_bool;
    const char *output = argus_get(argus, "output").as_string;
    const char *input = argus_get(argus, "input").as_string;
    
    // Application logic here
    
    argus_free(&argus);
    return 0;
}
```

### Subcommand Program Structure

```c
int add_command(argus_t *argus, void *data)
{
    const char *file = argus_get(*argus, "file").as_string;
    bool force = argus_get(*argus, "force").as_bool;
    // Command logic here
    return 0;
}

ARGUS_OPTIONS(add_options,
    HELP_OPTION(),
    OPTION_FLAG('f', "force", HELP("Force operation")),
    POSITIONAL_STRING("file", HELP("File to add"))
)

ARGUS_OPTIONS(options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    SUBCOMMAND("add", add_options, HELP("Add files"), ACTION(add_command))
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "tool", "1.0.0");
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    if (argus_has_command(argus)) {
        return argus_exec(&argus, NULL);
    }
    
    argus_free(&argus);
    return 0;
}
```

## See Also

- **[Regex Patterns](./regex-patterns)** - Built-in validation patterns
- **[Error Codes](./error-codes)** - Complete error code reference
