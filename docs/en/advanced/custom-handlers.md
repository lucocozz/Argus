# Custom Handlers

Custom handlers extend argus' capabilities by allowing you to transform and validate input data according to your specific needs.

!!! abstract "Overview"
    This guide covers advanced input processing with custom handlers:
    
    - **Handler Function Signature** - Creating custom processing functions
    - **Memory Management** - Allocating and freeing custom data structures
    - **Data Transformation** - Converting raw input to structured data
    - **Custom Type Creation** - Defining specialized data types
    - **Integration with Validators** - Combining handlers with validation
    
    For basic option handling, see the [Basic Options guide](../guide/basic-options.md).

## Understanding Handlers

While argus provides standard handlers for common types (string, int, float, boolean), custom handlers enable more advanced processing:

- Transforming inputs into complex data structures
- Parsing specialized formats (host:port, coordinates, colors)
- Converting between different representations
- Validating with business rules during processing

Unlike validators that simply check values, custom handlers can transform input strings into sophisticated data structures.

## Handler Function Signature

All handler functions must follow this signature:

```c
int handler_function(argus_t *argus, argus_option_t *option, char *arg);
```

Parameters:
- `argus`: The argus context, used for error reporting
- `option`: The option being processed, where to store the value
- `arg`: The raw string value to process (NULL for boolean flags)

Return value:
- `ARGUS_SUCCESS` (0) on success
- Any error code (non-zero) on failure

## Creating a Custom Handler

Let's look at a complete example of a custom handler that parses an endpoint in the format "host:port":

```c
typedef struct {
    char* host;
    int port;
} endpoint_t;

int endpoint_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    if (arg == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MISSING_VALUE, "Endpoint is required");
    }
    
    // Allocate the endpoint structure
    endpoint_t *endpoint = calloc(1, sizeof(endpoint_t));
    if (!endpoint) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
    }
    
    // Find the separator
    char *colon = strchr(arg, ':');
    if (!colon) {
        free(endpoint);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT, 
                          "Invalid endpoint format: %s (expected host:port)", arg);
    }
    
    // Extract host
    size_t host_len = colon - arg;
    endpoint->host = strndup(arg, host_len);
    if (!endpoint->host) {
        free(endpoint);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
    }
    
    // Extract port
    long port = strtol(colon + 1, NULL, 10);
    if (port <= 0 || port > 65535) {
        free(endpoint->host);
        free(endpoint);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, 
                          "Invalid port: %ld (must be between 1 and 65535)", port);
    }
    
    endpoint->port = (int)port;
    
    // Store the endpoint structure
    option->value.as_ptr = endpoint;
    option->is_allocated = true;  // Important: mark as allocated for proper cleanup
    return ARGUS_SUCCESS;
}
```

## Free Handlers

When your handler allocates memory, you must provide a custom free handler:

```c
int endpoint_free_handler(argus_option_t *option)
{
    endpoint_t *endpoint = (endpoint_t*)option->value.as_ptr;
    if (endpoint) {
        free(endpoint->host);
        free(endpoint);
    }
    return ARGUS_SUCCESS;
}
```

## Using Custom Handlers

To use a custom handler, define an option with the `OPTION_BASE` macro:

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    // Option with custom handler
    OPTION_BASE('e', "endpoint", HELP("Server endpoint (host:port)"), 
                VALUE_TYPE_CUSTOM,
                HANDLER(endpoint_handler),
                FREE_HANDLER(endpoint_free_handler))
)
```

## Creating Helper Macros

For reusability and cleaner code, you can create your own macro:

```c
// Helper macro for endpoint options
#define OPTION_ENDPOINT(short_name, long_name, ...) \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_CUSTOM, \
                HANDLER(endpoint_handler), \
                FREE_HANDLER(endpoint_free_handler), \
                ##__VA_ARGS__)

// Usage in option definitions
ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    // Much cleaner with a dedicated macro
    OPTION_ENDPOINT('e', "endpoint", HELP("Server endpoint"))
)
```

## Accessing Custom Values

To access values processed by custom handlers:

```c
if (argus_is_set(argus, "endpoint")) {
    // Get and cast the value to the correct type
    endpoint_t *endpoint = (endpoint_t*)argus_get(argus, "endpoint").as_ptr;
    
    // Use the structured data
    printf("Host: %s\n", endpoint->host);
    printf("Port: %d\n", endpoint->port);
}
```

## Advanced Techniques

### Combining Handlers and Validators

You can use custom handlers with validators for complex processing:

```c
// Custom handler for endpoint "host:port"
OPTION_BASE('e', "endpoint", HELP("Server endpoint (host:port)"), 
            VALUE_TYPE_CUSTOM,
            HANDLER(endpoint_handler),
            FREE_HANDLER(endpoint_free_handler),
            VALIDATOR(endpoint_validator, NULL))  // Add extra validation
```

### Custom Types with Specialized Processing

For more complex types, you can implement specialized processing:

=== "Color Processing"
    ```c
    typedef struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    } rgb_color_t;
    
    int color_handler(argus_t *argus, argus_option_t *option, char *arg)
    {
        // Allocate and initialize color structure
        rgb_color_t *color = calloc(1, sizeof(rgb_color_t));
        
        // Handle different formats:
        // - Hex: #RRGGBB or #RGB
        // - RGB: rgb(r,g,b)
        // - Named colors: red, green, blue, etc.
        
        // Store the color structure
        option->value.as_ptr = color;
        option->is_allocated = true;
        return ARGUS_SUCCESS;
    }
    ```

=== "Coordinate Processing"
    ```c
    typedef struct {
        double lat;
        double lon;
    } geo_coord_t;
    
    int coordinate_handler(argus_t *argus, argus_option_t *option, char *arg)
    {
        // Allocate coordinate structure
        geo_coord_t *coord = calloc(1, sizeof(geo_coord_t));
        
        // Parse formats:
        // - Decimal: "lat,lon" 
        // - DMS: "40°45'30"N,74°0'23"W"
        
        // Store the coordinate structure
        option->value.as_ptr = coord;
        option->is_allocated = true;
        return ARGUS_SUCCESS;
    }
    ```

## Error Reporting

Custom handlers should use the `ARGUS_REPORT_ERROR` macro to report errors:

```c
ARGUS_REPORT_ERROR(argus, error_code, format_string, ...);
```

This macro:
1. Formats an error message
2. Adds the error to the error stack
3. Returns the specified error code from your function

Common error codes:
- `ARGUS_ERROR_MISSING_VALUE`: Value was required but not provided
- `ARGUS_ERROR_INVALID_FORMAT`: Value has incorrect format
- `ARGUS_ERROR_INVALID_VALUE`: Value is semantically invalid
- `ARGUS_ERROR_MEMORY`: Memory allocation failed

## Memory Management Key Points

When your handler allocates memory:

1. Store the value in `option->value` (usually `option->value.as_ptr` for custom structures)
2. Set `option->is_allocated = true` to indicate that memory needs to be freed
3. Provide a custom free handler with `FREE_HANDLER()`
4. Ensure proper cleanup in error cases

## Best Practices

### 1. Robust Error Handling

Always implement comprehensive error handling:

```c
// Allocate memory
endpoint_t *endpoint = calloc(1, sizeof(endpoint_t));
if (!endpoint) {
    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
}

// Check for format errors
char *colon = strchr(arg, ':');
if (!colon) {
    free(endpoint);  // Clean up on error
    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT, 
                       "Invalid format: expected host:port");
}
```

### 2. Clear Error Messages

Provide informative error messages that help users:

=== "Good Message"
    ```c
    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT,
                     "Invalid endpoint format: %s (expected host:port)", arg);
    ```

=== "Bad Message"
    ```c
    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT, "Bad input");
    ```

### 3. Consistent Memory Management

Follow consistent memory management patterns:

```c
int my_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    // 1. Validate input
    if (arg == NULL) { ... }
    
    // 2. Allocate main structure
    my_type_t *obj = calloc(1, sizeof(my_type_t));
    if (!obj) { ... }
    
    // 3. Process input and allocate additional memory as needed
    // ...
    
    // 4. On error, clean up all allocations and report error
    // ...
    
    // 5. On success, store result and mark as allocated
    option->value.as_ptr = obj;
    option->is_allocated = true;
    return ARGUS_SUCCESS;
}
```

### 4. Type-Specific Helper Macros

Create helper macros for custom types:

```c
#define OPTION_ENDPOINT(short_name, long_name, ...) \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_CUSTOM, \
                HANDLER(endpoint_handler), \
                FREE_HANDLER(endpoint_free_handler), \
                ##__VA_ARGS__)

#define OPTION_COLOR(short_name, long_name, ...) \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_CUSTOM, \
                HANDLER(color_handler), \
                FREE_HANDLER(color_free_handler), \
                ##__VA_ARGS__)
```

## Complete Example

Here's a complete example implementing a custom endpoint handler:

```c
#include "argus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for the endpoint
typedef struct {
    char* host;
    int port;
} endpoint_t;

// Custom handler for endpoint "host:port"
int endpoint_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    if (arg == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MISSING_VALUE, "Endpoint is required");
    }
    
    // Allocate the endpoint structure
    endpoint_t *endpoint = calloc(1, sizeof(endpoint_t));
    if (!endpoint) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
    }
    
    // Find the separator
    char *colon = strchr(arg, ':');
    if (!colon) {
        free(endpoint);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT, 
                          "Invalid endpoint format: %s (expected host:port)", arg);
    }
    
    // Extract host
    size_t host_len = colon - arg;
    endpoint->host = strndup(arg, host_len);
    if (!endpoint->host) {
        free(endpoint);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
    }
    
    // Extract port
    long port = strtol(colon + 1, NULL, 10);
    if (port <= 0 || port > 65535) {
        free(endpoint->host);
        free(endpoint);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, 
                          "Invalid port: %ld (must be between 1 and 65535)", port);
    }
    
    endpoint->port = (int)port;
    
    // Store the endpoint structure
    option->value.as_ptr = endpoint;
    option->is_allocated = true;
    return ARGUS_SUCCESS;
}

// Free handler for endpoint structure
int endpoint_free_handler(argus_option_t *option)
{
    endpoint_t *endpoint = (endpoint_t*)option->value.as_ptr;
    if (endpoint) {
        free(endpoint->host);
        free(endpoint);
    }
    return ARGUS_SUCCESS;
}

// Helper macro for endpoint options
#define OPTION_ENDPOINT(short_name, long_name, ...) \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_CUSTOM, \
                HANDLER(endpoint_handler), \
                FREE_HANDLER(endpoint_free_handler), \
                ##__VA_ARGS__)

// Option definitions
ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Server endpoint using the custom handler
    OPTION_ENDPOINT('s', "server", HELP("Server endpoint (host:port)")),
    
    // Database endpoint with default value
    OPTION_ENDPOINT('d', "database", HELP("Database endpoint (host:port)"), 
                   DEFAULT("localhost:5432"))
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "custom_handlers_example", "1.0.0");
    argus.description = "Example of custom handlers";
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }
    
    // Process server endpoint if provided
    if (argus_is_set(argus, "server")) {
        endpoint_t *server = argus_get(argus, "server").as_ptr;
        
        printf("Server information:\n");
        printf("  Host: %s\n", server->host);
        printf("  Port: %d\n", server->port);
        printf("\n");
    }
    
    // Process database endpoint if provided
    if (argus_is_set(argus, "database")) {
        endpoint_t *db = argus_get(argus, "database").as_ptr;
        
        printf("Database information:\n");
        printf("  Host: %s\n", db->host);
        printf("  Port: %d\n", db->port);
        printf("\n");
    }
    
    argus_free(&argus);
    return 0;
}
```

## Related Documentation

- [Basic Options Guide](../guide/basic-options.md) - Understanding standard option types
- [Custom Validators Guide](custom-validators.md) - Custom validation techniques
