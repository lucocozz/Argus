# API Overview

!!! abstract "Overview"
    This page provides a comprehensive overview of the argus API architecture and its key components. argus is designed to be both simple for basic usage and powerful enough for complex command-line interfaces.

## Architecture

argus is organized around several key components:

```mermaid
graph TD
    A[Option Definition] --> B[Argument Parsing]
    B --> C[Value Access]
    B --> D[Validation]
    B --> E[Error Handling]
    F[Environment Variables] --> B
    G[Subcommands] --> B
```

1. **Option Definition** - Macros for defining options and their properties
2. **Argument Parsing** - Functions for processing command-line arguments
3. **Value Access** - Functions for retrieving parsed values
4. **Validation** - Mechanisms for validating inputs
5. **Error Handling** - System for managing and reporting errors
6. **Environment Variables** - Support for configuring via environment
7. **Subcommands** - Support for command hierarchies

## Key Components

### Main Structure

The `argus_t` structure is the core of argus, containing all necessary data:

```c
typedef struct argus_s {
    /* Public fields */
    const char *program_name;    // Program name
    const char *version;         // Program version
    const char *description;     // Program description
    const char *env_prefix;      // Prefix for environment variables
    
    /* Internal fields - do not access directly */
    argus_option_t     *options;      // Defined options
    argus_error_stack_t error_stack;  // Error stack
    // Other internal fields...
} argus_t;
```

!!! warning "Internal Fields"
    Only access the public fields directly. The internal fields should be accessed through the provided API functions.

### Option Definition

Options are defined using the `ARGUS_OPTIONS` macro along with option-specific macros:

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_INT('p', "port", HELP("Port number"), RANGE(1, 65535), DEFAULT(8080)),
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    POSITIONAL_STRING("input", HELP("Input file"))
)
```

The `argus_option_t` structure represents an individual option with all its properties.

### Value Types

argus supports various value types for options:

| Category | Types | Description |
|----------|-------|-------------|
| **Primitive** | `VALUE_TYPE_STRING`<br>`VALUE_TYPE_INT`<br>`VALUE_TYPE_FLOAT`<br>`VALUE_TYPE_BOOL` | Basic value types |
| **Array** | `VALUE_TYPE_ARRAY_STRING`<br>`VALUE_TYPE_ARRAY_INT`<br>`VALUE_TYPE_ARRAY_FLOAT` | Arrays of multiple values |
| **Map** | `VALUE_TYPE_MAP_STRING`<br>`VALUE_TYPE_MAP_INT`<br>`VALUE_TYPE_MAP_FLOAT`<br>`VALUE_TYPE_MAP_BOOL` | Key-value pairs |
| **Custom** | `VALUE_TYPE_CUSTOM` | User-defined types |

Values are stored in a `argus_value_t` union that can contain different types:

```c
union argus_value_u {
    uintptr_t raw;
    void     *as_ptr;
    
    char     *as_string;
    int       as_int;
    double    as_float;
    bool      as_bool;
    
    // Collection types
    argus_value_t      *as_array;
    argus_pair_t *as_map;
    // ...
};
```

### Option Types

Options are categorized into several types:

| Type | Macro | Description | Example |
|------|-------|-------------|---------|
| **Regular options** | `OPTION_*` | Standard options with - or -- prefix | `OPTION_INT('p', "port", HELP("Port number"))` |
| **Flag options** | `OPTION_FLAG` | Boolean options | `OPTION_FLAG('v', "verbose", HELP("Enable verbose mode"))` |
| **Positional arguments** | `POSITIONAL_*` | Arguments without dash prefix | `POSITIONAL_STRING("input", HELP("Input file"))` |
| **Subcommands** | `SUBCOMMAND` | Command hierarchies | `SUBCOMMAND("add", add_options, ...)` |
| **Option groups** | `GROUP_START` ... `GROUP_END` | Visual grouping | `GROUP_START("Network", ...)` |
| **Collection options** | `OPTION_ARRAY_*`<br>`OPTION_MAP_*` | Multiple values | `OPTION_ARRAY_STRING('t', "tags", HELP("Tags"))` |

## API Lifecycle

Typical argus usage follows this lifecycle:

```mermaid
graph TD
    A[Define options] --> B[Initialize argus]
    B --> C[Parse arguments]
    C --> D{Success?}
    D -- No --> E[Handle error]
    D -- Yes --> F[Access values]
    F --> G[Application logic]
    G --> H[Free resources]
```

1. **Define options** with `ARGUS_OPTIONS`
2. **Initialize** with `argus_init()`
3. **Parse arguments** with `argus_parse()`
4. **Access values** with `argus_get()`, `argus_is_set()`, etc.
5. **Free resources** with `argus_free()`

```c
// 1. Define options
ARGUS_OPTIONS(options, /* ... */)

int main(int argc, char **argv)
{
    // 2. Initialize
    argus_t argus = argus_init(options, "my_program", "1.0.0");
    argus.description = "My program description";
    
    // 3. Parse arguments
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }
    
    // 4. Access values
    const char *output = argus_get(argus, "output").as_string;
    bool verbose = argus_get(argus, "verbose").as_bool;
    
    // Application logic...
    
    // 5. Free resources
    argus_free(&argus);
    return 0;
}
```

## Function Families

The argus API is organized into several function families:

### Initialization and Parsing Functions

| Function | Description | Example |
|----------|-------------|---------|
| `argus_init()` | Initializes the argus context | `argus_t argus = argus_init(options, "my_program", "1.0.0");` |
| `argus_parse()` | Parses command-line arguments | `int status = argus_parse(&argus, argc, argv);` |
| `argus_free()` | Frees resources | `argus_free(&argus);` |

### Value Access Functions

| Function | Description | Example |
|----------|-------------|---------|
| `argus_get()` | Retrieves an option's value | `int port = argus_get(argus, "port").as_int;` |
| `argus_is_set()` | Checks if an option was set | `if (argus_is_set(argus, "verbose")) { ... }` |
| `argus_count()` | Gets the number of values for an option | `size_t count = argus_count(argus, "names");` |
| `argus_array_get()` | Retrieves an element from an array | `const char* name = argus_array_get(argus, "names", 0).as_string;` |
| `argus_map_get()` | Retrieves a value from a map | `int port = argus_map_get(argus, "ports", "http").as_int;` |

### Iteration Functions

| Function | Description | Example |
|----------|-------------|---------|
| `argus_array_it()` | Creates an array iterator | `argus_array_it_t it = argus_array_it(argus, "names");` |
| `argus_array_next()` | Advances to the next array element | `while (argus_array_next(&it)) { ... }` |
| `argus_array_reset()` | Resets an array iterator | `argus_array_reset(&it);` |
| `argus_map_it()` | Creates a map iterator | `argus_map_it_t it = argus_map_it(argus, "env");` |
| `argus_map_next()` | Advances to the next map entry | `while (argus_map_next(&it)) { ... }` |
| `argus_map_reset()` | Resets a map iterator | `argus_map_reset(&it);` |

### Subcommand Functions

| Function | Description | Example |
|----------|-------------|---------|
| `argus_has_command()` | Checks if a subcommand was specified | `if (argus_has_command(argus)) { ... }` |
| `argus_exec()` | Executes the action associated with a subcommand | `status = argus_exec(&argus, data);` |

### Display Functions

| Function | Description | Example |
|----------|-------------|---------|
| `argus_print_help()` | Displays a formatted help message | `argus_print_help(argus);` |
| `argus_print_usage()` | Displays a short usage message | `argus_print_usage(argus);` |
| `argus_print_version()` | Displays version information | `argus_print_version(argus);` |

## Error Handling

argus uses an error code system and an error stack for comprehensive error handling:

```c
// Parse arguments with error handling
int status = argus_parse(&argus, argc, argv);
if (status != ARGUS_SUCCESS) {
    // An error occurred during parsing
    argus_print_error_stack(&argus);  // Display detailed errors
    return status;
}
```

Common error codes:

| Error Code | Description |
|------------|-------------|
| `ARGUS_SUCCESS` | Operation successful |
| `ARGUS_ERROR_INVALID_ARGUMENT` | Invalid argument |
| `ARGUS_ERROR_MISSING_VALUE` | Value required but not provided |
| `ARGUS_ERROR_MISSING_REQUIRED` | Required option not provided |
| `ARGUS_ERROR_INVALID_FORMAT` | Value format is incorrect |
| `ARGUS_ERROR_INVALID_RANGE` | Value outside allowed range |
| `ARGUS_ERROR_INVALID_CHOICE` | Value not in allowed choices |
| `ARGUS_ERROR_CONFLICTING_OPTIONS` | Mutually exclusive options specified |

## Advanced Components

### Validators

Validators check that input values meet certain criteria:

=== "Range Validator"
    ```c
    // Ensure port is between 1 and 65535
    OPTION_INT('p', "port", HELP("Port number"), RANGE(1, 65535))
    ```

=== "Regular Expression Validator"
    ```c
    // Ensure email is valid
    OPTION_STRING('e', "email", HELP("Email address"), REGEX(ARGUS_RE_EMAIL))
    ```

=== "Custom Validator"
    ```c
    // Custom validation logic
    OPTION_INT('n', "number", HELP("Even number"), 
               VALIDATOR(even_validator, NULL))
    ```

### Handlers

Handlers process input values and convert them to internal representations:

```c
// Custom handler for IPv4 addresses
int ipv4_handler(argus_t *argus, argus_option_t *option, char *arg);

// Usage
OPTION_BASE('i', "ip", HELP("IP address"), VALUE_TYPE_CUSTOM,
            HANDLER(ipv4_handler),
            FREE_HANDLER(ipv4_free_handler))
```

### Environment Variables

Options can be configured to read from environment variables:

```c
// Define options with environment variable support
ARGUS_OPTIONS(
    options,
    OPTION_STRING('H', "host", HELP("Hostname"), 
                  ENV_VAR("HOST")),  // Will use APP_HOST
    OPTION_INT('p', "port", HELP("Port number"), 
               FLAGS(FLAG_AUTO_ENV))  // Will use APP_PORT
)

// Set the environment prefix
argus_t argus = argus_init(options, "my_program", "1.0.0");
argus.env_prefix = "APP";
```

## Header Organization

The argus public API is distributed across several header files:

| Header | Content | Include When You Need |
|--------|---------|----------------------|
| `argus.h` | Main entry point, includes all other headers | Always |
| `argus/types.h` | Type and data structure definitions | Rarely needed directly |
| `argus/options.h` | Macros for defining options | Rarely needed directly |
| `argus/api.h` | Public API functions | Rarely needed directly |
| `argus/errors.h` | Error codes and error handling functions | When handling specific errors |
| `argus/regex.h` | Predefined regular expression patterns | When using regex validation |

In most cases, you'll only need to include the main header:

```c
#include "argus.h"
```

## Performance Considerations

### Development vs. Release Mode

argus offers a release mode for optimal performance in production. This mode skips the comprehensive validation of option structures during initialization.

```c
// In development mode (default): full validation
argus_t argus = argus_init(options, "my_program", "1.0.0");

// In release mode: faster initialization
// (enabled by compiling with -DARGUS_RELEASE)
argus_t argus = argus_init(options, "my_program", "1.0.0");
```
To enable release mode, compile with the `-DARGUS_RELEASE` flag.

## Example Code

Here's a complete example demonstrating key features of argus:

```c
#include "argus.h"
#include <stdio.h>
#include <stdlib.h>

// Define options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Regular options
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_INT('p', "port", HELP("Port number"), RANGE(1, 65535), DEFAULT(8080)),
    
    // Option group
    GROUP_START("Advanced", GROUP_DESC("Advanced options")),
        OPTION_FLAG('f', "force", HELP("Force operation")),
        OPTION_FLAG('r', "recursive", HELP("Recursive mode")),
    GROUP_END(),
    
    // Array options
    OPTION_ARRAY_STRING('t', "tag", HELP("Tags"), FLAGS(FLAG_SORTED | FLAG_UNIQUE)),
    
    // Positional arguments
    POSITIONAL_STRING("input", HELP("Input file"))
)

int main(int argc, char **argv)
{
    // Initialize argus
    argus_t argus = argus_init(options, "example", "1.0.0");
    argus.description = "Example program demonstrating argus";
    
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
    bool force = argus_get(argus, "force").as_bool;
    bool recursive = argus_get(argus, "recursive").as_bool;
    
    // Process array values if set
    if (argus_is_set(argus, "tag")) {
        size_t count = argus_count(argus, "tag");
        printf("Tags (%zu):\n", count);
        
        argus_array_it_t it = argus_array_it(argus, "tag");
        while (argus_array_next(&it)) {
            printf("  - %s\n", it.value.as_string);
        }
    }
    
    // Application logic...
    
    // Free resources
    argus_free(&argus);
    return 0;
}
```

## Related Documentation

For more detailed information on specific aspects of the API:

- [Types Reference](types.md) - Detailed information about data types
- [Macros Reference](macros.md) - Complete list of option definition macros
- [Functions Reference](functions.md) - Comprehensive API function reference
- [Regex Patterns](regex_patterns.md) - Predefined regular expression patterns
