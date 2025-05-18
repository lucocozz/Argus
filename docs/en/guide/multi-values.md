# Multi-Value Collections

argus supports multi-value collections through arrays and maps, allowing you to handle multiple values for a single option or key-value pairs.

!!! abstract "Overview"
    This guide covers the basics of multi-value collections in argus:
    
    - **Array options** - Collecting multiple values
    - **Map options** - Collecting key-value pairs
    - **Input formats** - Different ways users can provide collection data
    - **Accessing values** - How to retrieve and use collection data
    
    For advanced usage, see the [Advanced Multi-Values](../advanced/multi-values.md) guide.

## Array Options

Array options allow users to provide multiple values for a single option, either through multiple occurrences or comma-separated lists.

### Array Types

argus supports these array types:

- `OPTION_ARRAY_STRING`: Array of strings
- `OPTION_ARRAY_INT`: Array of integers
- `OPTION_ARRAY_FLOAT`: Array of floating-point values

### Defining Array Options

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Simple string array
    OPTION_ARRAY_STRING('n', "names", HELP("List of names")),
    
    // Integer array
    OPTION_ARRAY_INT('i', "ids", HELP("List of IDs")),
    
    // Float array
    OPTION_ARRAY_FLOAT('f', "factors", HELP("Scaling factors")),
)
```

### Input Formats for Arrays

Users can provide array values in several ways:

=== "Multiple Option Occurrences"
    ```bash
    ./program --names=john --names=alice --names=bob
    ```

=== "Comma-Separated Values"
    ```bash
    ./program --names=john,alice,bob
    ```

=== "Combined Approaches"
    ```bash
    ./program --names=john,alice --names=bob
    ```

=== "Short Option Format"
    ```bash
    ./program -n john -n alice -n bob
    ./program -njohn,alice,bob
    ```

### Integer Range Syntax

For integer arrays, argus supports a convenient range syntax:

```bash
./program --ids=1-5,10,15-20
# Equivalent to: --ids=1,2,3,4,5,10,15,16,17,18,19,20
```

## Map Options

Map options allow users to provide key-value pairs, enabling structured configuration through command-line arguments.

### Map Types

argus supports these map types:

- `OPTION_MAP_STRING`: Map with string values
- `OPTION_MAP_INT`: Map with integer values
- `OPTION_MAP_FLOAT`: Map with floating-point values
- `OPTION_MAP_BOOL`: Map with boolean values

### Defining Map Options

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // String map
    OPTION_MAP_STRING('e', "env", HELP("Environment variables")),
    
    // Integer map
    OPTION_MAP_INT('p', "ports", HELP("Service port numbers")),
    
    // Float map
    OPTION_MAP_FLOAT('s', "scales", HELP("Scaling factors")),
    
    // Boolean map
    OPTION_MAP_BOOL('f', "features", HELP("Feature flags")),
)
```

### Input Formats for Maps

Users can provide map values in several ways:

=== "Multiple Option Occurrences"
    ```bash
    ./program --env=USER=alice --env=HOME=/home/alice
    ```

=== "Comma-Separated Key-Value Pairs"
    ```bash
    ./program --env=USER=alice,HOME=/home/alice,TERM=xterm
    ```

=== "Combined Approaches"
    ```bash
    ./program --env=USER=alice,HOME=/home/alice --env=TERM=xterm
    ```

=== "Short Option Format"
    ```bash
    ./program -e USER=alice -e HOME=/home/alice
    ./program -eUSER=alice,HOME=/home/alice
    ```

### Special Handling for Boolean Maps

For boolean maps (`OPTION_MAP_BOOL`), values are parsed as booleans:

- **True values**: "true", "yes", "1", "on", "y" (case-insensitive)
- **False values**: "false", "no", "0", "off", "n" (case-insensitive)

```bash
./program --features=debug=true,logging=yes,metrics=1,cache=false
```

## Accessing Multi-Value Collections

argus provides multiple ways to access collection data.

### Basic Array Access

To access array elements:

```c
// Check if array option was set
if (argus_is_set(argus, "names")) {
    // Get the array count
    size_t count = argus_count(argus, "names");
    
    // Get the array pointer
    argus_value_t *names_array = argus_get(argus, "names").as_array;
    
    // Print all names
    printf("Names (%zu):\n", count);
    for (size_t i = 0; i < count; i++) {
        printf("  %zu: %s\n", i+1, names_array[i].as_string);
    }
}
```

### Basic Map Access

To access map entries:

```c
// Check if map option was set
if (argus_is_set(argus, "env")) {
    // Get the map count
    size_t count = argus_count(argus, "env");
    
    // Get the map pointer
    argus_pair_t *env_map = argus_get(argus, "env").as_map;
    
    // Print all environment variables
    printf("Environment variables (%zu):\n", count);
    for (size_t i = 0; i < count; i++) {
        const char* key = env_map[i].key;
        const char* value = env_map[i].value.as_string;
        printf("  %s = %s\n", key, value);
    }
}
```

### Element Access Helpers

For more convenient access to specific elements:

```c
// Get a specific array element by index
const char* first_name = argus_array_get(argus, "names", 0).as_string;
int second_id = argus_array_get(argus, "ids", 1).as_int;

// Look up a specific map value by key
const char* user = argus_map_get(argus, "env", "USER").as_string;
int http_port = argus_map_get(argus, "ports", "http").as_int;
bool debug_enabled = argus_map_get(argus, "features", "debug").as_bool;
```

These helpers handle invalid indices or missing keys gracefully, returning an empty value when the requested element doesn't exist.

## Common Use Cases

### Command-Line Tags or Labels

```c
OPTION_ARRAY_STRING('t', "tag", "Tags for the resource")
```

Usage:
```bash
./program --tag=important,urgent --tag=priority
```

### Service Configuration

```c
OPTION_MAP_INT('p', "port", "Port mappings for services")
```

Usage:
```bash
./program --port=http=80,https=443,ftp=21
```

### Feature Toggles

```c
OPTION_MAP_BOOL('f', "feature", "Feature toggles")
```

Usage:
```bash
./program --feature=dark-mode=on,animations=off,beta=true
```

## Complete Example

Here's a complete example demonstrating both array and map options:

```c
#include "argus.h"
#include <stdio.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Array options
    OPTION_ARRAY_STRING('n', "name", HELP("Names of users")),
    OPTION_ARRAY_INT('i', "id", HELP("User IDs")),
    
    // Map options
    OPTION_MAP_STRING('e', "env", HELP("Environment variables")),
    OPTION_MAP_INT('p', "port", HELP("Port mappings")),
    OPTION_MAP_BOOL('f', "feature", HELP("Feature flags")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "multi_values", "1.0.0");
    argus.description = "Example of multi-value collections";
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }
    
    // Process string array
    if (argus_is_set(argus, "name")) {
        size_t count = argus_count(argus, "name");
        argus_value_t *names = argus_get(argus, "name").as_array;
        
        printf("Names (%zu):\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  %zu: %s\n", i+1, names[i].as_string);
        }
        printf("\n");
    }
    
    // Process integer array
    if (argus_is_set(argus, "id")) {
        size_t count = argus_count(argus, "id");
        argus_value_t *ids = argus_get(argus, "id").as_array;
        
        printf("IDs (%zu):\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  %zu: %d\n", i+1, ids[i].as_int);
        }
        printf("\n");
    }
    
    // Process string map
    if (argus_is_set(argus, "env")) {
        size_t count = argus_count(argus, "env");
        argus_pair_t *env = argus_get(argus, "env").as_map;
        
        printf("Environment Variables (%zu):\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  %s = %s\n", env[i].key, env[i].value.as_string);
        }
        printf("\n");
    }
    
    // Process integer map
    if (argus_is_set(argus, "port")) {
        size_t count = argus_count(argus, "port");
        argus_pair_t *ports = argus_get(argus, "port").as_map;
        
        printf("Port Mappings (%zu):\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  %s: %d\n", ports[i].key, ports[i].value.as_int);
        }
        printf("\n");
    }
    
    // Process boolean map
    if (argus_is_set(argus, "feature")) {
        size_t count = argus_count(argus, "feature");
        argus_pair_t *features = argus_get(argus, "feature").as_map;
        
        printf("Feature Flags (%zu):\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  %s: %s\n", features[i].key, 
                   features[i].value.as_bool ? "enabled" : "disabled");
        }
        printf("\n");
    }
    
    argus_free(&argus);
    return 0;
}
```

## Next Steps

For more advanced collection handling including:

- Iterators for efficient traversal
- Collection flags (sorting, uniqueness)
- Performance considerations
- Advanced use cases

See the [Advanced Multi-Values](../advanced/multi-values.md) guide.
