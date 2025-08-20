# > Argus Cheat Sheet_

Quick reference for common patterns and syntax.

## // Basic Setup

```c
#include <argus.h>

ARGUS_OPTIONS(options,
    HELP_OPTION(),
    VERSION_OPTION(),
    // Your options here
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "program", "1.0.0");
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    // Access values
    bool flag = argus_get(&argus, "flag").as_bool;
    
    argus_free(&argus);
    return 0;
}
```

## // Option Types

| Type | Macro | Access | Example |
|------|-------|--------|---------|
| Flag | `OPTION_FLAG('v', "verbose", ...)` | `.as_bool` | `--verbose` |
| String | `OPTION_STRING('o', "output", ...)` | `.as_string` | `--output file.txt` |
| Integer | `OPTION_INT('p', "port", ...)` | `.as_int` | `--port 8080` |
| Float | `OPTION_FLOAT('r', "rate", ...)` | `.as_float` | `--rate 0.5` |
| Boolean | `OPTION_BOOL('d', "debug", ...)` | `.as_bool` | `--debug=true` |

## // Collections

```c
// Arrays
OPTION_ARRAY_STRING('t', "tags", HELP("Tags"))
OPTION_ARRAY_INT('p', "ports", HELP("Ports"))
OPTION_ARRAY_FLOAT('r', "rates", HELP("Rates"))

// Maps
OPTION_MAP_STRING('e', "env", HELP("Environment vars"))
OPTION_MAP_INT('c', "config", HELP("Config values"))
OPTION_MAP_BOOL('f', "features", HELP("Feature flags"))
```

## // Positional Arguments

```c
// Single positionals
POSITIONAL_STRING("input", HELP("Input file"))
POSITIONAL_INT("count", HELP("Count"), FLAGS(FLAG_OPTIONAL))

// Variadic positionals (accept multiple values)
POSITIONAL_MANY_STRING("files", HELP("Files to process"))
POSITIONAL_MANY_INT("numbers", HELP("Numbers to calculate"))
POSITIONAL_MANY_FLOAT("values", HELP("Decimal values"))
```

## // Option Modifiers

```c
OPTION_STRING('o', "output", 
    HELP("Output file"),                    // Description
    DEFAULT("result.txt"),                  // Default value
    HINT("FILE"),                          // Value hint
    FLAGS(FLAG_REQUIRED),                  // Required option
    ENV_VAR("OUTPUT_FILE"),               // Environment variable
    VALIDATOR(V_LENGTH(1, 100))           // Validation
)
```

## // Validation

```c
VALIDATOR(V_RANGE(1, 100))                 // Numeric range
VALIDATOR(V_LENGTH(3, 20))                 // String length
VALIDATOR(V_COUNT(1, 5))                   // Collection count
VALIDATOR(V_REGEX(ARGUS_RE_EMAIL))         // Regex pattern

VALIDATOR(V_CHOICE_STR("json", "xml", "yaml"))      // String choices
VALIDATOR(V_CHOICE_INT(80, 443, 8080))                // Integer choices
```

## // Common Regex Patterns

```c
ARGUS_RE_EMAIL          // user@example.com
ARGUS_RE_IPV4           // 192.168.1.1
ARGUS_RE_URL            // https://example.com
ARGUS_RE_ISO_DATE       // 2024-03-15
ARGUS_RE_PHONE_US       // 123-456-7890
ARGUS_RE_UUID           // 550e8400-e29b-41d4-a716-446655440000
```

## // Environment Variables

```c
int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "app", "1.0.0");
    argus.env_prefix = "APP";  // APP_PORT, APP_HOST, etc.
    // ...
}

// Option definitions
OPTION_STRING('h', "host", ENV_VAR("HOST"))           // Uses APP_HOST
OPTION_INT('p', "port", FLAGS(FLAG_AUTO_ENV))         // Auto-generates APP_PORT
OPTION_STRING('d', "db", ENV_VAR("DATABASE_URL"),     // Uses DATABASE_URL exactly
              FLAGS(FLAG_NO_ENV_PREFIX))
```

## // Flags

```c
FLAGS(FLAG_REQUIRED)        // Must be provided
FLAGS(FLAG_OPTIONAL)        // Optional (positionals)
FLAGS(FLAG_HIDDEN)          // Hidden from help
FLAGS(FLAG_ENV_OVERRIDE)    // Env overrides CLI
FLAGS(FLAG_AUTO_ENV)        // Auto-generate env name
FLAGS(FLAG_NO_ENV_PREFIX)   // Skip env prefix

// Collection flags
FLAGS(FLAG_SORTED)          // Sort array values
FLAGS(FLAG_UNIQUE)          // Remove duplicates
FLAGS(FLAG_SORTED_KEY)      // Sort map by keys
FLAGS(FLAG_SORTED_VALUE)    // Sort map by values
```

## // Value Access

```c
// Basic access
bool verbose = argus_get(&argus, "verbose").as_bool;
const char *output = argus_get(&argus, "output").as_string;

// Check if set
if (argus_is_set(&argus, "output")) {
    // User provided this option
}

// Array access
size_t count = argus_count(&argus, "tags");
const char *first = argus_array_get(&argus, "tags", 0).as_string;

// Map access
const char *user = argus_map_get(&argus, "env", "USER").as_string;

// Iterators
argus_array_it_t it = argus_array_it(&argus, "tags");
while (argus_array_next(&it)) {
    printf("%s\n", it.value.as_string);
}
```

## // Subcommands

```c
int add_action(argus_t *argus, void *data)
{
    const char *file = argus_get(argus, "file").as_string;     // Local option
    bool verbose = argus_get(argus, ".verbose").as_bool;       // Global option
    return 0;
}

ARGUS_OPTIONS(add_options,
    HELP_OPTION(),
    POSITIONAL_STRING("file", HELP("File to add"))
)

ARGUS_OPTIONS(options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    SUBCOMMAND("add", add_options, HELP("Add files"), ACTION(add_action))
)

int main(int argc, char **argv)
{
    // ... parse ...
    if (argus_has_command(&argus)) {
        return argus_exec(&argus, NULL);
    }
}
```

## // Input Formats

```bash
# Flags
--verbose, -v

# Options
--output=file.txt, --output file.txt, -o file.txt, -ofile.txt

# Combined short options
-vf, -vo file.txt, -vofile.txt

# Arrays
--tags=web,api,backend
--tags=web --tags=api --tags=backend
--ports=8000-8005,9000  # Range expansion for integers

# Maps
--env=USER=alice,HOME=/home/alice
--env=USER=alice --env=HOME=/home/alice

# Positional
program input.txt output.txt

# Variadic positionals
program file1.c file2.c file3.c ...

# Stop parsing
program --option -- --not-an-option
```

## // Error Handling

```c
// Parse errors are handled automatically
if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS) {
    return 1;  // Error already printed
}

// Custom validation errors
if (custom_check_failed) {
    fprintf(stderr, "%s: Custom error message\n", argus.program_name);
    return 1;
}
```

## // Custom Types

```c
typedef struct { char *host; int port; } endpoint_t;

int endpoint_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    endpoint_t *ep = malloc(sizeof(endpoint_t));
    // Parse "host:port" into ep
    option->value.as_ptr = ep;
    option->is_allocated = true;
    return ARGUS_SUCCESS;
}

OPTION_BASE('e', "endpoint", VALUE_TYPE_CUSTOM,
            HANDLER(endpoint_handler), FREE_HANDLER(endpoint_free))

// Access
endpoint_t *ep = (endpoint_t*)argus_get(&argus, "endpoint").as_ptr;
```

## // Groups

```c
GROUP_START("Connection", GROUP_DESC("Network options")),
    OPTION_STRING('h', "host", HELP("Hostname")),
    OPTION_INT('p', "port", HELP("Port")),
GROUP_END(),

GROUP_START("Output", GROUP_DESC("Output options")),
    OPTION_STRING('o', "output", HELP("Output file")),
    OPTION_STRING('f', "format", HELP("Format")),
GROUP_END(),
```

## // Complete Example

```c
ARGUS_OPTIONS(options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('c', "config", HELP("Config file"), DEFAULT("app.conf")),
    OPTION_INT('p', "port", HELP("Port"), VALIDATOR(V_RANGE(1, 65535)), 
               ENV_VAR("PORT"), DEFAULT(8080)),
    OPTION_ARRAY_STRING('t', "tags", HELP("Tags"), FLAGS(FLAG_UNIQUE)),
    OPTION_MAP_STRING('e', "env", HELP("Environment"), FLAGS(FLAG_SORTED_KEY)),
    
    POSITIONAL_STRING("input", HELP("Input file")),
    POSITIONAL_STRING("output", HELP("Output file"), FLAGS(FLAG_OPTIONAL))
)
```
