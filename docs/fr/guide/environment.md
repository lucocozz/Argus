# Environment Variables

argus can configure options through environment variables, providing flexibility for containerized applications, CI/CD pipelines, and system-wide configurations.

## Basic Concepts

!!! abstract "Overview"
    Environment variables support in argus allows you to:
    
    1. Associate options with environment variables
    2. Optionally use a prefix for variable namespacing
    3. Configure whether variables provide defaults or override command-line arguments
    4. Use either explicit variable names or auto-generated ones

## Setting Up Environment Variables Support

### Setting the Application Prefix (Optional)

You can configure an environment variable prefix in your argus context:

```c
argus_t argus = argus_init(options, "my_program", "1.0.0");
argus.env_prefix = "APP";  // Optional: prefix env vars with APP_
```

This prefix helps namespace your environment variables to avoid conflicts with other applications. If you don't set a prefix, argus will use the environment variable names exactly as specified.

!!! note "Prefix is Optional"
    The environment prefix is completely optional. If you don't set `argus.env_prefix`, argus will still work with environment variables but won't add any prefix.

## Associating Options with Environment Variables

### 1. Explicit Environment Variable Name

=== "Definition"
    ```c
    // With prefix (APP_HOST if prefix is set)
    OPTION_STRING('H', "host", HELP("Server hostname"),
                  ENV_VAR("HOST"))
    ```

=== "Environment Variable"
    ```bash
    # If prefix is "APP"
    export APP_HOST=localhost
    
    # If no prefix is set
    export HOST=localhost
    
    # Run without command-line argument
    ./my_program  # Will use HOST value from environment
    ```

### 2. Auto-generated Environment Variable Name

=== "Definition"
    ```c
    // Will use APP_PORT if prefix is "APP", or PORT if no prefix
    OPTION_INT('p', "port", HELP("Server port"), 
               FLAGS(FLAG_AUTO_ENV))
    ```

=== "Environment Variable"
    ```bash
    # If prefix is "APP"
    export APP_PORT=9000
    
    # If no prefix is set
    export PORT=9000
    
    # Run without command-line argument
    ./my_program  # Will use PORT=9000 from environment
    ```

When using `FLAG_AUTO_ENV`, argus automatically generates a variable name:
- Converts the option name to uppercase
- Replaces dashes with underscores
- Adds the environment prefix (if set)

### 3. Non-prefixed Environment Variable

=== "Definition"
    ```c
    // Always uses DATABASE_URL exactly as specified, ignoring any prefix
    OPTION_STRING('d', "database", HELP("Database URL"),
                  ENV_VAR("DATABASE_URL"),
                  FLAGS(FLAG_NO_ENV_PREFIX))
    ```

=== "Environment Variable"
    ```bash
    # System variable without prefix, regardless of argus.env_prefix setting
    export DATABASE_URL=postgres://user:pass@localhost/db
    
    # Run without command-line argument
    ./my_program  # Will use DATABASE_URL from environment
    ```

Use `FLAG_NO_ENV_PREFIX` for standard system variables that shouldn't have your application's prefix.

### 4. Auto-generated Name Without Prefix

=== "Definition"
    ```c
    // Uses VERBOSE directly, ignoring any prefix
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output"),
                FLAGS(FLAG_AUTO_ENV | FLAG_NO_ENV_PREFIX))
    ```

=== "Environment Variable"
    ```bash
    # Environment variable without prefix
    export VERBOSE=true
    
    # Run without command-line flag
    ./my_program  # Will enable verbose mode from environment
    ```

Combine `FLAG_AUTO_ENV` with `FLAG_NO_ENV_PREFIX` to auto-generate a name without the application prefix.

## Controlling Environment Variables Precedence

### Default Behavior: Environment as Fallback

By default, environment variables provide fallback values when command-line options aren't specified:

=== "Definition"
    ```c
    OPTION_INT('t', "timeout", HELP("Timeout in seconds"),
               ENV_VAR("TIMEOUT"))
    ```

=== "Precedence"
    1. If `--timeout=30` is provided on command line: value is 30
    2. If no command-line option but environment variable is set: value from environment
    3. If neither is provided but a default value is set: use the default

### Override Behavior: Environment Takes Priority

Use `FLAG_ENV_OVERRIDE` to make environment variables override command-line options:

=== "Definition"
    ```c
    OPTION_INT('t', "timeout", HELP("Timeout in seconds"),
               ENV_VAR("FORCE_TIMEOUT"), 
               FLAGS(FLAG_ENV_OVERRIDE))
    ```

=== "Precedence"
    1. If environment variable is set: use that value (even if command-line specifies differently)
    2. If no environment variable but `--timeout=30` is provided: value is 30
    3. If neither is provided but a default value is set: use the default

This is useful for system-wide configurations that should not be overridden by individual users.

## Type Conversion

Environment variables are always strings, but argus automatically converts them to the appropriate type:

| Option Type | Environment Value | Conversion |
|-------------|------------------|------------|
| `OPTION_STRING` | Any text | Used as-is |
| `OPTION_INT` | "123" | Parsed as integer 123 |
| `OPTION_FLOAT` | "3.14" | Parsed as float 3.14 |
| `OPTION_BOOL` | "1", "true", "yes", "on", "y" | Converted to `true` |
| | "0", "false", "no", "off", "n" | Converted to `false` |
| Collections | "value1,value2,value3" | Parsed as comma-separated values |

!!! note "Boolean Value Support"
    For boolean options (`OPTION_BOOL`), the environment variables support true/false values in various formats (case-insensitive):
    
    - **True values**: "1", "true", "yes", "on", "y" 
    - **False values**: "0", "false", "no", "off", "n"

## Best Practices for Environment Variables

### Naming Conventions

Follow these naming conventions for clarity and consistency:

1. **Use UPPER_SNAKE_CASE** for environment variable names
2. **Choose a short, unique prefix** related to your application name (if using a prefix)
3. **Use consistent naming patterns** throughout your application
4. **Document environment variables** in your application's help and docs

### Document Environment Variables

Clearly document the environment variables your application supports:

```c
// Help text explicitly mentions environment variable
OPTION_STRING('H', "host", HELP("Server hostname (env: APP_HOST)"),
              ENV_VAR("HOST"))
```

## Complete Example

Here's a complete example showing various environment variable configurations:

```c
#include "argus.h"
#include <stdio.h>
#include <stdlib.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),

    // Option 1: Explicit environment variable with optional prefix
    // Will look for APP_HOST if prefix is set, or HOST if no prefix
    OPTION_STRING('H', "host", HELP("Server hostname"),
                DEFAULT("localhost"),
                ENV_VAR("HOST")),
    
    // Option 2: Auto-generated environment variable name
    // Will generate APP_PORT or PORT from the option name
    OPTION_INT('p', "port", HELP("Server port"), 
               DEFAULT(8080),
               FLAGS(FLAG_AUTO_ENV)),
    
    // Option 3: Explicit environment variable without prefix
    // Will look for DATABASE_URL exactly as specified
    OPTION_STRING('d', "database", HELP("Database connection string"),
                ENV_VAR("DATABASE_URL"),
                FLAGS(FLAG_NO_ENV_PREFIX)),
    
    // Option 4: Auto-generated name without prefix
    // Will generate VERBOSE from the option name
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output"),
                FLAGS(FLAG_AUTO_ENV | FLAG_NO_ENV_PREFIX)),
    
    // Option 5: Environment variable that can override command line
    // Even if --timeout is specified, FORCE_TIMEOUT will take precedence
    OPTION_INT('t', "timeout", HELP("Connection timeout in seconds"),
               DEFAULT(30),
               ENV_VAR("FORCE_TIMEOUT"),
               FLAGS(FLAG_ENV_OVERRIDE)),
              
    // Option 6: Debug flag with prefix (if set)
    OPTION_FLAG('\0', "debug", HELP("Enable debug mode"),
               ENV_VAR("DEBUG"))
)

int main(int argc, char **argv)
{
    // Initialize argus and optionally set environment prefix
    argus_t argus = argus_init(options, "env_variables", "1.0.0");
    argus.description = "Example of environment variables usage";
    argus.env_prefix = "APP";  // Optional: set a prefix for env vars
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;

    // Access option values as usual
    const char* host = argus_get(argus, "host").as_string;
    int port = argus_get(argus, "port").as_int;
    const char* database = argus_get(argus, "database").as_string;
    bool verbose = argus_get(argus, "verbose").as_bool;
    int timeout = argus_get(argus, "timeout").as_int;
    bool debug = argus_get(argus, "debug").as_bool;
    
    // Display configuration
    printf("Server Configuration:\n");
    printf("  Host:      %s\n", host);
    printf("  Port:      %d\n", port);
    printf("  Database:  %s\n", database ? database : "(not set)");
    printf("  Timeout:   %d seconds\n", timeout);
    
    printf("\nDebug Settings:\n");
    printf("  Verbose:   %s\n", verbose ? "enabled" : "disabled");
    printf("  Debug:     %s\n", debug ? "enabled" : "disabled");
    
    // Show environment variable mappings
    printf("\nEnvironment Variables:\n");
    printf("  APP_HOST          - Maps to --host\n");
    printf("  APP_PORT          - Maps to --port\n");
    printf("  DATABASE_URL      - Maps to --database\n");
    printf("  VERBOSE           - Maps to --verbose\n");
    printf("  APP_FORCE_TIMEOUT - Maps to --timeout (with override)\n");
    printf("  APP_DEBUG         - Maps to --debug\n");
    
    argus_free(&argus);
    return 0;
}
```

## Implementation Details

Behind the scenes, argus follows this process for environment variables:

1. Parse command-line arguments first with `parse_args()`
2. Load environment variables with `load_env_vars()`
3. For options with `FLAG_ENV_OVERRIDE`, environment values replace command-line values
4. For options without override flag, environment values are used only if the option wasn't set from command line
5. Run validation on all values with `post_parse_validation()`

## Testing Environment Variables

### Shell Environment

```bash
# Set environment variables
export APP_HOST=env-server.example.com
export APP_PORT=9000
export DATABASE_URL=postgres://user:pass@localhost/db
export VERBOSE=1
export APP_FORCE_TIMEOUT=60
export APP_DEBUG=true

# Run your program
./my_program
```

### Docker Container

```dockerfile
FROM alpine:latest
COPY ./my_program /app/my_program
ENV APP_HOST=container-host
ENV APP_PORT=9000
ENV DATABASE_URL=postgres://user:pass@db/app
ENV APP_FORCE_TIMEOUT=60
WORKDIR /app
CMD ["./my_program"]
```

For a complete example, see `examples/env_vars.c`.
