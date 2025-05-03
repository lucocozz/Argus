# Types Reference

This page documents the core data types and structures provided by the argus library. Understanding these types is essential for advanced usage and custom extensions.

!!! abstract "Overview"
    argus defines several key data types and structures that form the foundation of the library:
    
    - **Core Structures** - Main data structures like `argus_t` and `argus_option_t`
    - **Value Types** - Types for storing and accessing different kinds of values
    - **Enumerations** - Type classifiers and flag definitions
    - **Collections** - Array and map data structures and iterators
    - **Callback Types** - Function pointer types for handlers and validators

## Core Structures

### argus_t

The main context structure that holds all parsing state and configuration.

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
    struct { /* ... */ } context;     // Internal context
} argus_t;
```

The public fields can be accessed and modified directly after initializing with `argus_init()`:

```c
argus_t argus = argus_init(options, "my_program", "1.0.0");
argus.description = "My awesome program";
argus.env_prefix = "MYAPP";  // Optional: prefix for environment variables
```

!!! warning "Internal Fields"
    The internal fields should not be accessed directly. Use the provided API functions to interact with them.

### argus_option_t

Defines a command-line option with all its properties and behavior.

```c
typedef struct argus_option_s {
    /* Base metadata */
    argus_optype_t type;        // Type of option (flag, positional, etc.)
    const char *name;          // Internal name for references
    char sname;                // Short name (e.g., 'v' for -v)
    const char *lname;         // Long name (e.g., "verbose" for --verbose)
    const char *help;          // Help text
    const char *hint;          // Value hint for help display
    
    /* Value metadata */
    argus_valtype_t value_type;   // Type of value
    argus_value_t value;             // Current value
    argus_value_t default_value;     // Default value
    bool have_default;         // Whether a default is set
    /* Additional value fields... */
    
    /* Callbacks */
    argus_handler_t handler;       // Value handler function
    argus_free_handler_t free_handler;  // Resource cleanup function
    argus_validator_t validator;    // Value validator
    /* Additional callback fields... */
    
    /* Dependencies */
    const char **requires;     // Options that must be set with this one
    const char **conflicts;    // Options that cannot be used with this one
    
    /* State */
    argus_optflags_t flags;      // Option behavior flags
    bool is_set;               // Whether the option was set on command line
    
    /* Subcommand fields */
    argus_action_t action;     // Action for subcommands
    struct argus_option_s *sub_options;  // Options for subcommands
} argus_option_t;
```

!!! note
    You typically don't create `argus_option_t` structures directly. Instead, use the provided macros like `OPTION_STRING`, `OPTION_INT`, etc.

## Value Types

### argus_value_t

A union type that can hold values of different types:

```c
typedef union argus_value_u {
    uintptr_t raw;          // Raw value as integer
    void     *as_ptr;       // Generic pointer
    
    // Basic types
    char  as_char;          // Character
    char *as_string;        // String
    int   as_int;           // Integer
    long long as_int64;     // 64-bit integer
    double as_float;        // Floating-point
    bool   as_bool;         // Boolean
    
    // Collection types
    argus_value_t      *as_array;  // Array of values
    argus_pair_t *as_map;    // Key-value map
} argus_value_t;
```

### argus_pair_t

A key-value pair used in map collections:

```c
typedef struct argus_pair_s {
    const char *key;    // String key
    argus_value_t     value;  // Value of any supported type
} argus_pair_t;
```

## Enumerations

### argus_optype_t

Defines the different types of command-line elements:

```c
typedef enum argus_optype_e {
    TYPE_NONE = 0,        // Terminator for option arrays
    TYPE_OPTION,          // Standard option with - or -- prefix
    TYPE_GROUP,           // Logical grouping of options
    TYPE_POSITIONAL,      // Positional argument
    TYPE_SUBCOMMAND,      // Subcommand with its own options
} argus_optype_t;
```

### argus_valtype_t

Defines the types of values an option can hold:

```c
typedef enum argus_valtype_e {
    VALUE_TYPE_NONE = 0,        // No value
    
    // Primitive types
    VALUE_TYPE_INT    = 1 << 0,  // Integer
    VALUE_TYPE_STRING = 1 << 1,  // String
    VALUE_TYPE_FLOAT  = 1 << 2,  // Floating-point
    VALUE_TYPE_BOOL   = 1 << 3,  // Boolean
    VALUE_TYPE_FLAG   = 1 << 4,  // Flag (boolean with no value)
    
    // Array types
    VALUE_TYPE_ARRAY_STRING = 1 << 5,  // String array
    VALUE_TYPE_ARRAY_INT    = 1 << 6,  // Integer array
    VALUE_TYPE_ARRAY_FLOAT  = 1 << 7,  // Float array
    
    // Map types
    VALUE_TYPE_MAP_STRING = 1 << 8,    // String map
    VALUE_TYPE_MAP_INT    = 1 << 9,    // Integer map
    VALUE_TYPE_MAP_FLOAT  = 1 << 10,    // Float map
    VALUE_TYPE_MAP_BOOL   = 1 << 11,   // Boolean map
    
    VALUE_TYPE_CUSTOM = 1 << 12,       // Custom type
} argus_valtype_t;
```

### argus_optflags_t

Defines flags that modify option behavior:

```c
typedef enum argus_optflags_e {
    FLAG_NONE = 0,
    /* Option flags */
    FLAG_REQUIRED      = 1 << 0,  // Option must be specified
    FLAG_HIDDEN        = 1 << 1,  // Option is hidden from help
    FLAG_ADVANCED      = 1 << 2,  // Option is marked as advanced
    FLAG_DEPRECATED    = 1 << 3,  // Option is marked as deprecated
    FLAG_EXPERIMENTAL  = 1 << 4,  // Option is marked as experimental
    FLAG_EXIT          = 1 << 5,  // Program exits after option handler
    FLAG_ENV_OVERRIDE  = 1 << 6,  // Option value can be overridden by environment variable
    FLAG_AUTO_ENV      = 1 << 7,  // Automatically generate environment variable name
    FLAG_NO_ENV_PREFIX = 1 << 8,  // Do not use environment variable prefix
    
    /* Array and Map type flags */
    FLAG_SORTED       = 1 << 9,   // Array values are sorted
    FLAG_UNIQUE       = 1 << 10,  // Array values are unique
    FLAG_SORTED_VALUE = 1 << 11,  // Map values are sorted
    FLAG_SORTED_KEY   = 1 << 12,  // Map keys are sorted
    FLAG_UNIQUE_VALUE = 1 << 13,  // Map values are unique
    
    /* Group flags */
    FLAG_EXCLUSIVE = 1 << 14,     // Only one option in group can be set
} argus_optflags_t;
```

## Collection Iterators

### argus_array_it_t

Iterator for array collections:

```c
typedef struct argus_array_iterator_s {
    argus_value_t *_array;      // Internal array pointer
    size_t   _count;      // Number of elements
    size_t   _position;   // Current position
    argus_value_t  value;       // Current value
} argus_array_it_t;
```

Example usage:
```c
argus_array_it_t it = argus_array_it(argus, "names");
while (argus_array_next(&it)) {
    printf("Name: %s\n", it.value.as_string);
}
```

### argus_map_it_t

Iterator for map collections:

```c
typedef struct argus_map_iterator_s {
    argus_pair_t *_map;    // Internal map pointer
    size_t        _count;  // Number of elements
    size_t        _position; // Current position
    const char   *key;     // Current key
    argus_value_t       value;   // Current value
} argus_map_it_t;
```

Example usage:
```c
argus_map_it_t it = argus_map_it(argus, "env");
while (argus_map_next(&it)) {
    printf("%s = %s\n", it.key, it.value.as_string);
}
```

## Callback Types

### argus_handler_t

Handler function for processing option values:

```c
typedef int (*argus_handler_t)(argus_t *, argus_option_t *, char *);
```

Custom handlers process the raw string value and store it in the option structure.

### argus_free_handler_t

Handler function for freeing option resources:

```c
typedef int (*argus_free_handler_t)(argus_option_t *);
```

Custom free handlers clean up resources allocated by a handler function.

### argus_validator_t

Validator function for checking option values:

```c
typedef int (*argus_validator_t)(argus_t *, argus_value_t, validator_data_t);
```

Validators ensure values meet certain criteria after processing.

### argus_pre_validator_t

Pre-validator function for checking raw string values:

```c
typedef int (*argus_pre_validator_t)(argus_t *, const char *, validator_data_t);
```

Pre-validators check input strings before they're processed.

### argus_action_t

Action function for subcommands:

```c
typedef int (*argus_action_t)(argus_t *, void *);
```

Actions are executed when a subcommand is invoked.

## Validator Data Types

### validator_data_t

Data passed to validators:

```c
typedef union validator_data_u {
    void        *custom;  // Custom validator data
    range_t      range;   // Range limits for numeric validation
    regex_data_t regex;   // Regex pattern and info
} validator_data_t;
```

### range_t

Range limits for numeric validation:

```c
typedef struct range_s {
    long long min;  // Minimum value
    long long max;  // Maximum value
} range_t;
```

### regex_data_t

Regular expression validation data:

```c
typedef struct regex_data_s {
    const char *pattern;  // Regex pattern string
    const char *hint;     // Error message hint
} regex_data_t;
```

## Error Handling Types

### argus_error_type_t

Error codes returned by argus functions:

```c
typedef enum argus_error_type_e {
    ARGUS_SUCCESS = 0,             // No error
    ARGUS_SOULD_EXIT,              // Normal exit after option handling
    
    /* Structure errors */
    ARGUS_ERROR_DUPLICATE_OPTION,  // Option name collision
    ARGUS_ERROR_INVALID_HANDLER,   // Invalid handler function
    /* More error codes... */
    
    /* Parsing errors */
    ARGUS_ERROR_INVALID_ARGUMENT,  // Invalid argument
    ARGUS_ERROR_MISSING_VALUE,     // Required value not provided
    /* More error codes... */
} argus_error_type_t;
```

### argus_error_t

Structure for storing detailed error information:

```c
typedef struct argus_error_s {
    argus_error_context_t context;  // Error context
    int code;                       // Error code
    char message[ARGUS_MAX_ERROR_MESSAGE_SIZE];  // Error message
} argus_error_t;
```

## Constants and Limits

Argus defines several constants that control its behavior:

```c
#define MAX_SUBCOMMAND_DEPTH 8  // Maximum depth of nested subcommands
#define ARGUS_MAX_ERROR_MESSAGE_SIZE 256  // Maximum error message length
#define ARGUS_MAX_ERRORS_STACK 16  // Maximum number of errors in stack
#define MULTI_VALUE_INITIAL_CAPACITY 8  // Initial capacity for collections
```

## Type Macros and Categories

Argus provides several helper macros for working with value types:

```c
// Type category macros
#define VALUE_TYPE_PRIMITIVE \
    (VALUE_TYPE_INT | VALUE_TYPE_STRING | VALUE_TYPE_FLOAT | VALUE_TYPE_BOOL)
    
#define VALUE_TYPE_ARRAY \
    (VALUE_TYPE_ARRAY_STRING | VALUE_TYPE_ARRAY_INT | VALUE_TYPE_ARRAY_FLOAT)
    
#define VALUE_TYPE_MAP \
    (VALUE_TYPE_MAP_STRING | VALUE_TYPE_MAP_INT | VALUE_TYPE_MAP_FLOAT | VALUE_TYPE_MAP_BOOL)

// Flag category macros
#define OPTION_FLAG_MASK \
    (FLAG_REQUIRED | FLAG_HIDDEN | FLAG_ADVANCED | FLAG_EXIT | VERSIONING_FLAG_MASK)
    
#define OPTION_ARRAY_FLAG_MASK \
    (FLAG_SORTED | FLAG_UNIQUE | VERSIONING_FLAG_MASK)
    
// More flag masks...
```

## Utility Types

The library also includes several utility types for error tracking and context management:

```c
typedef struct argus_error_context_s {
    const char *option_name;     // Current option name
    const char *group_name;      // Current group name 
    const char *subcommand_name; // Current subcommand name
} argus_error_context_t;

typedef struct argus_error_stack_s {
    argus_error_t errors[ARGUS_MAX_ERRORS_STACK];  // Error array
    size_t count;                                  // Number of errors
} argus_error_stack_t;
```

## Related Documentation

- [Macros Reference](macros.md) - Complete list of option definition macros
- [Functions Reference](functions.md) - Comprehensive API function reference
- [Overview](overview.md) - High-level overview of the argus API
