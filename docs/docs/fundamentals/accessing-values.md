# > Accessing Values_

Learn how to retrieve and use parsed option values in your Argus applications with type-safe access methods.

## // Core Access Functions

After `argus_parse()` succeeds, use these functions to access your parsed values:

| Function | Purpose | Returns |
|----------|---------|---------|
| `argus_get()` | Get option value | `argus_value_t` union |
| `argus_is_set()` | Check if option was provided | `bool` |
| `argus_count()` | Get number of values (collections) | `size_t` |

## // Basic Value Access

Access values using the option's identifier (long name, or short name if no long name):

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
<TabItem value="simple" label="Simple Types" default>

```c
ARGUS_OPTIONS(
    options,
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file")),  
    OPTION_INT('p', "port", HELP("Port number")),
    OPTION_FLOAT('r', "rate", HELP("Processing rate")),
    OPTION_BOOL('d', "debug", HELP("Debug mode")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Access by long name (preferred)
    bool verbose = argus_get(&argus, "verbose").as_bool;
    const char *output = argus_get(&argus, "output").as_string;
    int port = argus_get(&argus, "port").as_int;
    float rate = argus_get(&argus, "rate").as_float;
    bool debug = argus_get(&argus, "debug").as_bool;
    
    printf("Verbose: %s\n", verbose ? "yes" : "no");
    printf("Output: %s\n", output);
    printf("Port: %d\n", port);
    printf("Rate: %.2f\n", rate);
    
    argus_free(&argus);
    return 0;
}
```

</TabItem>
<TabItem value="positional" label="Positional Arguments">

```c
ARGUS_OPTIONS(
    options,
    POSITIONAL_STRING("input", HELP("Input file")),
    POSITIONAL_STRING("output", HELP("Output file")),
    POSITIONAL_INT("count", HELP("Count"), FLAGS(FLAG_OPTIONAL)),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Access positional arguments by name
    const char *input = argus_get(&argus, "input").as_string;
    const char *output = argus_get(&argus, "output").as_string;
    
    // Check optional positionals before accessing
    if (argus_is_set(&argus, "count")) {
        int count = argus_get(&argus, "count").as_int;
        printf("Count: %d\n", count);
    }
    
    argus_free(&argus);
    return 0;
}
```

</TabItem>
<TabItem value="short-only" label="Short Name Only">

```c
ARGUS_OPTIONS(
    options,
    OPTION_INT('p', NULL, HELP("Port number")),      // No long name
    OPTION_FLAG('\0', "debug", HELP("Debug mode")),  // No short name
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Use short name as identifier when no long name
    int port = argus_get(&argus, "p").as_int;
    
    // Use long name as identifier when available
    bool debug = argus_get(&argus, "debug").as_bool;
    
    argus_free(&argus);
    return 0;
}
```

</TabItem>
</Tabs>

## // Checking If Options Are Set

Use `argus_is_set()` to distinguish between default values and user-provided values:

```c
ARGUS_OPTIONS(
    options,
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("default.txt")),
    OPTION_INT('p', "port", HELP("Port"), DEFAULT(8080)),
    POSITIONAL_STRING("input", HELP("Input file")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Always check if user explicitly set the option
    if (argus_is_set(&argus, "output"))
        printf("User specified output: %s\n", argus_get(&argus, "output").as_string);
    else
        printf("Using default output: %s\n", argus_get(&argus, "output").as_string);
    
    // Required positionals are always set (or parsing fails)
    const char *input = argus_get(&argus, "input").as_string;
    printf("Input: %s\n", input);
    
    argus_free(&argus);
    return 0;
}
```

:::tip When to use argus_is_set()
- **Optional arguments**: Check before accessing to avoid using defaults
- **Conditional logic**: Different behavior based on whether user provided the option
- **Validation**: Ensure required logic only runs when user provided values
- **Debugging**: Log what the user actually specified vs defaults
:::

## // Array Collections

Access array values using direct access or helper functions:

<Tabs>
<TabItem value="direct" label="Direct Access" default>

```c
ARGUS_OPTIONS(
    options,
    OPTION_ARRAY_STRING('t', "tags", HELP("Tags")),
    OPTION_ARRAY_INT('n', "numbers", HELP("Numbers")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Get array count and data
    size_t tag_count = argus_count(&argus, "tags");
    argus_value_t *tags = argus_get(&argus, "tags").as_array;
    
    printf("Tags (%zu):\n", tag_count);
    for (size_t i = 0; i < tag_count; i++)
        printf("  %zu: %s\n", i + 1, tags[i].as_string);
    
    // Same for integers
    size_t num_count = argus_count(&argus, "numbers");
    argus_value_t *numbers = argus_get(&argus, "numbers").as_array;
    
    for (size_t i = 0; i < num_count; i++)
        printf("Number %zu: %d\n", i + 1, numbers[i].as_int);
    
    argus_free(&argus);
    return 0;
}
```

</TabItem>
<TabItem value="helpers" label="Helper Functions">

```c
ARGUS_OPTIONS(
    options,
    OPTION_ARRAY_STRING('t', "tags", HELP("Tags")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Get specific array elements by index
    const char *first_tag = argus_array_get(&argus, "tags", 0).as_string;
    const char *second_tag = argus_array_get(&argus, "tags", 1).as_string;
    
    if (first_tag) 
        printf("First tag: %s\n", first_tag);
    if (second_tag) 
        printf("Second tag: %s\n", second_tag);
    
    // Helper returns empty value for invalid indices
    argus_value_t invalid = argus_array_get(&argus, "tags", 999);
    // invalid.as_string will be NULL
    
    argus_free(&argus);
    return 0;
}
```

</TabItem>
<TabItem value="iterators" label="Iterator Pattern">

```c
ARGUS_OPTIONS(
    options,
    OPTION_ARRAY_STRING('t', "tags", HELP("Tags")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Create iterator for clean traversal
    argus_array_it_t it = argus_array_it(&argus, "tags");
    
    printf("Tags:\n");
    while (argus_array_next(&it))
        printf("  - %s\n", it.value.as_string);
    
    // Can reset and iterate again
    argus_array_reset(&it);
    printf("\nSecond pass:\n");
    while (argus_array_next(&it))
        printf("  * %s\n", it.value.as_string);
    
    argus_free(&argus);
    return 0;
}
```

</TabItem>
</Tabs>

## // Map Collections

Access key-value pairs using direct access or lookup helpers:

<Tabs>
<TabItem value="map-direct" label="Direct Access" default>

```c
ARGUS_OPTIONS(
    options,
    OPTION_MAP_STRING('e', "env", HELP("Environment variables")),
    OPTION_MAP_INT('p', "ports", HELP("Port mappings")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Get map count and data
    size_t env_count = argus_count(&argus, "env");
    argus_pair_t *env_map = argus_get(&argus, "env").as_map;
    
    printf("Environment variables (%zu):\n", env_count);
    for (size_t i = 0; i < env_count; i++)
        printf("  %s = %s\n", env_map[i].key, env_map[i].value.as_string);
    
    argus_free(&argus);
    return 0;
}
```

</TabItem>
<TabItem value="map-lookup" label="Key Lookup">

```c
ARGUS_OPTIONS(
    options,
    OPTION_MAP_STRING('e', "env", HELP("Environment variables")),
    OPTION_MAP_INT('p', "ports", HELP("Port mappings")),
    OPTION_MAP_BOOL('f', "features", HELP("Feature flags")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Look up specific keys
    const char *user = argus_map_get(&argus, "env", "USER").as_string;
    const char *home = argus_map_get(&argus, "env", "HOME").as_string;
    
    int http_port = argus_map_get(&argus, "ports", "http").as_int;
    int https_port = argus_map_get(&argus, "ports", "https").as_int;
    
    bool debug = argus_map_get(&argus, "features", "debug").as_bool;
    bool cache = argus_map_get(&argus, "features", "cache").as_bool;
    
    // Check if keys were actually provided
    if (user) 
        printf("User: %s\n", user);
    if (home) 
        printf("Home: %s\n", home);
    if (http_port) 
        printf("HTTP port: %d\n", http_port);
    
    printf("Debug: %s\n", debug ? "enabled" : "disabled");
    
    argus_free(&argus);
    return 0;
}
```

</TabItem>
<TabItem value="map-iterator" label="Iterator Pattern">

```c
ARGUS_OPTIONS(
    options,
    OPTION_MAP_STRING('e', "env", HELP("Environment variables")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Create iterator for clean traversal
    argus_map_it_t it = argus_map_it(&argus, "env");
    
    printf("Environment variables:\n");
    while (argus_map_next(&it))
        printf("  %s = %s\n", it.key, it.value.as_string);
    
    argus_free(&argus);
    return 0;
}
```

</TabItem>
</Tabs>

## // Variadic Positional

Access variadic positional arguments the same way as array collections:

<Tabs>
<TabItem value="variadic-direct" label="Direct Access" default>

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    POSITIONAL_MANY_STRING("files", HELP("Input files to process"), 
                           HINT("FILE..."),
                           FLAGS(FLAG_UNIQUE | FLAG_SORTED)),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Check if files were provided
    if (argus_is_set(&argus, "files")) {
        size_t count = argus_count(&argus, "files");
        argus_value_t *files = argus_get(&argus, "files").as_array;
        
        printf("Processing %zu files:\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  %zu: %s\n", i + 1, files[i].as_string);
        }
    } else {
        printf("No files specified\n");
    }
    
    argus_free(&argus);
    return 0;
}
```

</TabItem>
<TabItem value="variadic-iterator" label="Iterator Access">

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    POSITIONAL_MANY_STRING("files", HELP("Input files"), HINT("FILE...")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Use iterator for clean traversal
    argus_array_it_t it = argus_array_it(&argus, "files");
    
    printf("Files to process:\n");
    while (argus_array_next(&it)) {
        printf("  - %s\n", it.value.as_string);
        process_file(it.value.as_string);
    }
    
    argus_free(&argus);
    return 0;
}
```

</TabItem>
<TabItem value="variadic-helper" label="Helper Functions">

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    POSITIONAL_MANY_INT("numbers", HELP("Numbers to process"), HINT("NUM...")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Access specific elements by index
    int first = argus_array_get(&argus, "numbers", 0).as_int;
    int second = argus_array_get(&argus, "numbers", 1).as_int;
    
    if (first) printf("First number: %d\n", first);
    if (second) printf("Second number: %d\n", second);
    
    // Get total count
    size_t total = argus_count(&argus, "numbers");
    printf("Total numbers: %zu\n", total);
    
    argus_free(&argus);
    return 0;
}
```

</TabItem>
</Tabs>

**Important notes:**
- Variadic positionals use the same access functions as array options
- Always check `argus_is_set()` before accessing since they accept zero values
- Use `argus_count()` to get the number of values provided
- Iterator pattern is most efficient for processing all values

## // Subcommand Access

In subcommand context, access values using path notation or relative names:

<Tabs>
<TabItem value="subcommand-basic" label="Basic Subcommands" default>

```c
// Subcommand action handler
int add_command(argus_t *argus, void *data)
{
    (void)data; // Unused
    
    // Access subcommand options using relative names
    const char *file = argus_get(argus, "file").as_string;
    bool force = argus_get(argus, "force").as_bool;
    
    // Access global options using absolute path
    bool verbose = argus_get(argus, ".verbose").as_bool;
    
    printf("Adding file: %s\n", file);
    if (force) 
        printf("  (forced)\n");
    if (verbose) 
        printf("  (verbose mode)\n");
    
    return 0;
}

ARGUS_OPTIONS(add_options,
    HELP_OPTION(),
    OPTION_FLAG('f', "force", HELP("Force operation")),
    POSITIONAL_STRING("file", HELP("File to add")),
)

ARGUS_OPTIONS(options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    SUBCOMMAND("add", add_options, HELP("Add a file"), ACTION(add_command)),
)
```

</TabItem>
<TabItem value="subcommand-paths" label="Path Access">

```c
int service_create_action(argus_t *argus, void *data)
{
    (void)data;
    
    // Different ways to access the same value
    
    // 1. Relative path (current subcommand context)
    const char *name = argus_get(argus, "name").as_string;
    
    // 2. Absolute path (full path from root)
    const char *name_abs = argus_get(argus, "service.create.name").as_string;
    
    // 3. Root-level option (dot prefix)
    bool debug = argus_get(argus, ".debug").as_bool;
    
    printf("Creating service: %s\n", name);
    if (debug) 
        printf("Debug mode enabled\n");
    
    return 0;
}

ARGUS_OPTIONS(create_options,
    HELP_OPTION(),
    OPTION_STRING('n', "name", HELP("Service name")),
)

ARGUS_OPTIONS(service_options,
    HELP_OPTION(),
    SUBCOMMAND("create", create_options, ACTION(service_create_action)),
)

ARGUS_OPTIONS(options,
    HELP_OPTION(),
    OPTION_FLAG('d', "debug", HELP("Debug mode")),
    SUBCOMMAND("service", service_options, HELP("Service commands")),
)
```

</TabItem>
</Tabs>

## // Custom Types

Access custom handler values through the generic pointer:

```c
typedef struct {
    char *host;
    int port;
} endpoint_t;

// Custom handler creates endpoint_t structure
int endpoint_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    // Parse "host:port" format into endpoint_t
    // (implementation details omitted)
    
    endpoint_t *endpoint = malloc(sizeof(endpoint_t));
    // ... parse and populate endpoint ...
    
    option->value.as_ptr = endpoint;
    option->is_allocated = true;
    return ARGUS_SUCCESS;
}

ARGUS_OPTIONS(options,
    OPTION_BASE('e', "endpoint", VALUE_TYPE_CUSTOM,
                HANDLER(endpoint_handler),
                HELP("Server endpoint (host:port)")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Cast generic pointer to your custom type
    endpoint_t *endpoint = (endpoint_t*)argus_get(&argus, "endpoint").as_ptr;
    
    if (endpoint) {
        printf("Host: %s\n", endpoint->host);
        printf("Port: %d\n", endpoint->port);
    }
    
    argus_free(&argus);
    return 0;
}
```

## // Error Handling

Always check parse results before accessing values:

```c
int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    
    // Parse can fail - always check the result
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        // Error already printed by argus_parse(), argus_free() is not needed
        return status;
    }
    
    // Safe to access values after successful parse
    bool verbose = argus_get(&argus, "verbose").as_bool;
    const char *input = argus_get(&argus, "input").as_string;
    
    // Your application logic here
    
    argus_free(&argus);
    return 0;
}
```

## // Best Practices

### ✅ Good Practices

```c
// 1. Always check parse result
if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS) {
    return 1;
}

// 2. Use descriptive variable names
const char *config_file = argus_get(&argus, "config").as_string;
bool enable_debug = argus_get(&argus, "debug").as_bool;

// 3. Check optional values before using
if (argus_is_set(&argus, "output")) {
    const char *output = argus_get(&argus, "output").as_string;
    // Use output...
}

// 4. Access arrays safely
size_t count = argus_count(&argus, "files");
if (count > 0) {
    argus_value_t *files = argus_get(&argus, "files").as_array;
    // Process files...
}
```

### ❌ Avoid These Patterns

```c
// ❌ Don't ignore parse errors
argus_parse(&argus, argc, argv); // Missing error check
bool verbose = argus_get(&argus, "verbose").as_bool;

// ❌ Don't access without checking count for arrays
argus_value_t *files = argus_get(&argus, "files").as_array;
printf("First file: %s\n", files[0].as_string); // May crash!

// ❌ Don't assume optional values are set
const char *output = argus_get(&argus, "output").as_string;
FILE *f = fopen(output, "w"); // output might be NULL!
```

## // Access Patterns Summary

| Scenario | Method | Example |
|----------|---------|---------|
| **Basic types** | `argus_get().as_type` | `argus_get(&argus, "port").as_int` |
| **Check if set** | `argus_is_set()` | `if (argus_is_set(&argus, "output"))` |
| **Array elements** | `argus_array_get()` | `argus_array_get(&argus, "tags", 0)` |
| **Array iteration** | `argus_array_it()` | `while (argus_array_next(&it))` |
| **Variadic positional** | `argus_get().as_array` | `argus_get(&argus, "files").as_array` |
| **Variadic iteration** | `argus_array_it()` | `while (argus_array_next(&it))` |
| **Map lookup** | `argus_map_get()` | `argus_map_get(&argus, "env", "USER")` |
| **Map iteration** | `argus_map_it()` | `while (argus_map_next(&it))` |
| **Subcommand relative** | `argus_get()` | `argus_get(argus, "file").as_string` |
| **Root from subcommand** | `argus_get()` with `.` | `argus_get(argus, ".debug").as_bool` |
| **Custom types** | Cast `as_ptr` | `(my_type_t*)argus_get().as_ptr` |

## // What's Next?

Now that you know how to access values, learn about:

- **[Help and Errors](help-and-errors)** - Understanding help generation and error handling
- **[Validation](../features/validation)** - Ensuring users provide correct input
- **[Subcommands](../features/subcommands)** - Building Git-like command hierarchies
- **[Collections](../features/collections)** - Working with arrays and maps in detail

:::tip Memory Management
Argus automatically manages memory for all parsed values. Just remember to call `argus_free()` before your program exits to clean up resources.
:::
