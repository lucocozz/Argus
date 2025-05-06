# Functions Reference

This page provides a comprehensive reference for all the functions in the argus API. These functions allow you to initialize the library, parse arguments, access values, and manage resources.

!!! abstract "Overview"
    The argus API is organized into several function groups:
    
    - **Initialization and Parsing** - Core functions for setup and parsing
    - **Value Access** - Functions for retrieving option values
    - **Collection Access** - Functions for working with arrays and maps
    - **Subcommand Management** - Functions for handling subcommands
    - **Display Functions** - Functions for generating help and usage text
    - **Error Handling** - Functions for managing and reporting errors

## Initialization and Parsing

### argus_init

Initializes a argus context with program information and options.

```c
argus_t argus_init(argus_option_t *options, const char *program_name, const char *version);
```

**Parameters:**
- `options`: Array of command-line options defined with `ARGUS_OPTIONS`
- `program_name`: Name of the program (used in help/error messages)
- `version`: Version string

**Returns:**
- An initialized `argus_t` structure

**Example:**
```c
argus_t argus = argus_init(options, "my_program", "1.0.0");
argus.description = "My awesome program description";
```

!!! tip
    After initialization, you can set additional fields like `description` and `env_prefix` before parsing.

### argus_parse

Parses command-line arguments according to the defined options.

```c
int argus_parse(argus_t *argus, int argc, char **argv);
```

**Parameters:**
- `argus`: Pointer to the initialized argus context
- `argc`: Argument count (from `main`)
- `argv`: Argument values (from `main`)

**Returns:**
- `ARGUS_SUCCESS` (0) on success
- A non-zero error code on failure

**Example:**
```c
int status = argus_parse(&argus, argc, argv);
if (status != ARGUS_SUCCESS) {
    return status;
}
```

### argus_free

Frees resources allocated during parsing.

```c
void argus_free(argus_t *argus);
```

**Parameters:**
- `argus`: Pointer to the argus context to free

**Example:**
```c
argus_free(&argus);
```

!!! warning
    Always call `argus_free()` when you're done with a argus context to avoid memory leaks.

## Value Access

### argus_get

Retrieves the value of an option.

```c
argus_value_t argus_get(argus_t argus, const char *option_path);
```

**Parameters:**
- `argus`: The argus context
- `option_path`: Path to the option (name or `subcommand.name` format)

**Returns:**
- The option's value as a `argus_value_t` union, or `{.raw = 0}` if not found

**Example:**
```c
const char *output = argus_get(argus, "output").as_string;
int port = argus_get(argus, "port").as_int;
bool verbose = argus_get(argus, "verbose").as_bool;
```

### argus_is_set

Checks if an option was explicitly set on the command line.

```c
bool argus_is_set(argus_t argus, const char *option_path);
```

**Parameters:**
- `argus`: The argus context
- `option_path`: Path to the option (name or `subcommand.name` format)

**Returns:**
- `true` if the option was set, `false` otherwise

**Example:**
```c
if (argus_is_set(argus, "verbose")) {
    // Verbose mode enabled
}
```

### argus_count

Gets the number of values for an option (for collections).

```c
size_t argus_count(argus_t argus, const char *option_path);
```

**Parameters:**
- `argus`: The argus context
- `option_path`: Path to the option (name or `subcommand.name` format)

**Returns:**
- Number of values for the option, or 0 if not found or not a collection

**Example:**
```c
size_t tags_count = argus_count(argus, "tags");
printf("Tags: %zu\n", tags_count);
```

## Collection Access

### argus_array_get

Retrieves an element from an array option at the specified index.

```c
argus_value_t argus_array_get(argus_t argus, const char *option_path, size_t index);
```

**Parameters:**
- `argus`: The argus context
- `option_path`: Path to the array option
- `index`: Index of the element to retrieve

**Returns:**
- The value at the specified index, or `{.raw = 0}` if not found or index out of bounds

**Example:**
```c
const char *first_tag = argus_array_get(argus, "tags", 0).as_string;
```

### argus_map_get

Retrieves a value from a map option with the specified key.

```c
argus_value_t argus_map_get(argus_t argus, const char *option_path, const char *key);
```

**Parameters:**
- `argus`: The argus context
- `option_path`: Path to the map option
- `key`: Key to look up in the map

**Returns:**
- The value associated with the key, or `{.raw = 0}` if not found

**Example:**
```c
const char *user = argus_map_get(argus, "env", "USER").as_string;
int http_port = argus_map_get(argus, "ports", "http").as_int;
```

### argus_array_it

Creates an iterator for efficiently traversing an array option.

```c
argus_array_it_t argus_array_it(argus_t argus, const char *option_path);
```

**Parameters:**
- `argus`: The argus context
- `option_path`: Path to the array option

**Returns:**
- Iterator structure for the array, with `count=0` if option not found

**Example:**
```c
argus_array_it_t it = argus_array_it(argus, "tags");
```

### argus_array_next

Gets the next element from an array iterator.

```c
bool argus_array_next(argus_array_it_t *it);
```

**Parameters:**
- `it`: Array iterator

**Returns:**
- `true` if a value was retrieved, `false` if end of array

**Example:**
```c
argus_array_it_t it = argus_array_it(argus, "tags");
while (argus_array_next(&it)) {
    printf("Tag: %s\n", it.value.as_string);
}
```

### argus_array_reset

Resets an array iterator to the beginning.

```c
void argus_array_reset(argus_array_it_t *it);
```

**Parameters:**
- `it`: Array iterator to reset

**Example:**
```c
argus_array_reset(&it);  // Reset to start a new iteration
```

### argus_map_it

Creates an iterator for efficiently traversing a map option.

```c
argus_map_it_t argus_map_it(argus_t argus, const char *option_path);
```

**Parameters:**
- `argus`: The argus context
- `option_path`: Path to the map option

**Returns:**
- Iterator structure for the map, with `count=0` if option not found

**Example:**
```c
argus_map_it_t it = argus_map_it(argus, "env");
```

### argus_map_next

Gets the next key-value pair from a map iterator.

```c
bool argus_map_next(argus_map_it_t *it);
```

**Parameters:**
- `it`: Map iterator

**Returns:**
- `true` if a pair was retrieved, `false` if end of map

**Example:**
```c
argus_map_it_t it = argus_map_it(argus, "env");
while (argus_map_next(&it)) {
    printf("%s = %s\n", it.key, it.value.as_string);
}
```

### argus_map_reset

Resets a map iterator to the beginning.

```c
void argus_map_reset(argus_map_it_t *it);
```

**Parameters:**
- `it`: Map iterator to reset

**Example:**
```c
argus_map_reset(&it);  // Reset to start a new iteration
```

## Subcommand Management

### argus_has_command

Checks if a subcommand was specified on the command line.

```c
bool argus_has_command(argus_t argus);
```

**Parameters:**
- `argus`: The argus context

**Returns:**
- `true` if a subcommand was specified, `false` otherwise

**Example:**
```c
if (argus_has_command(argus)) {
    // A subcommand was specified
} else {
    printf("No command specified. Use --help to see available commands.\n");
}
```

### argus_exec

Executes the action associated with the specified subcommand.

```c
int argus_exec(argus_t *argus, void *data);
```

**Parameters:**
- `argus`: Pointer to the argus context
- `data`: Optional data to pass to the subcommand action

**Returns:**
- Status code returned by the subcommand action
- `ARGUS_ERROR_NO_COMMAND` if no command was specified
- `ARGUS_ERROR_INVALID_HANDLER` if the command has no action

**Example:**
```c
if (argus_has_command(argus)) {
    status = argus_exec(&argus, NULL);
}
```

## Display Functions

### argus_print_help

Prints a formatted help message based on the defined options.

```c
void argus_print_help(argus_t argus);
```

**Parameters:**
- `argus`: The argus context

**Example:**
```c
argus_print_help(argus);
```

### argus_print_usage

Prints a short usage summary.

```c
void argus_print_usage(argus_t argus);
```

**Parameters:**
- `argus`: The argus context

**Example:**
```c
argus_print_usage(argus);
```

### argus_print_version

Prints version information.

```c
void argus_print_version(argus_t argus);
```

**Parameters:**
- `argus`: The argus context

**Example:**
```c
argus_print_version(argus);
```

## Error Handling

### argus_print_error_stack

Prints all errors in the error stack.

```c
void argus_print_error_stack(const argus_t *argus);
```

**Parameters:**
- `argus`: The argus context

**Example:**
```c
if (status != ARGUS_SUCCESS) {
    argus_print_error_stack(&argus);
    return status;
}
```

### argus_strerror

Gets a string description of an error code.

```c
const char *argus_strerror(argus_error_type_t error);
```

**Parameters:**
- `error`: Error code

**Returns:**
- String description of the error

**Example:**
```c
printf("Error: %s\n", argus_strerror(status));
```

## Complete Example

Here's a complete example showing the main function usage pattern:

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
    POSITIONAL_STRING("input", HELP("Input file"))
)

int main(int argc, char **argv)
{
    // Initialize argus
    argus_t argus = argus_init(options, "example", "1.0.0");
    argus.description = "Example program using argus";
    
    // Parse arguments
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        argus_print_error_stack(&argus);
        return status;
    }
    
    // Access values
    const char *input = argus_get(argus, "input").as_string;
    const char *output = argus_get(argus, "output").as_string;
    bool verbose = argus_get(argus, "verbose").as_bool;
    
    // Application logic
    if (verbose) {
        printf("Input: %s\n", input);
        printf("Output: %s\n", output);
    }
    
    // Free resources
    argus_free(&argus);
    return 0;
}
```

## Function Categories

For ease of reference, here's a summary of the function categories:

| Category | Functions |
|----------|-----------|
| **Initialization** | `argus_init`, `argus_parse`, `argus_free` |
| **Value Access** | `argus_get`, `argus_is_set`, `argus_count` |
| **Array Functions** | `argus_array_get`, `argus_array_it`, `argus_array_next`, `argus_array_reset` |
| **Map Functions** | `argus_map_get`, `argus_map_it`, `argus_map_next`, `argus_map_reset` |
| **Subcommand Functions** | `argus_has_command`, `argus_exec` |
| **Display Functions** | `argus_print_help`, `argus_print_usage`, `argus_print_version` |
| **Error Functions** | `argus_print_error_stack`, `argus_strerror` |

## Related Documentation

- [Types Reference](types.md) - Detailed information about data types
- [Macros Reference](macros.md) - Complete list of option definition macros
- [API Overview](overview.md) - High-level overview of the argus API
