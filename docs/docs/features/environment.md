# Environment Variables

Load option values from environment variables to support containerized deployments, CI/CD pipelines, and system-wide configurations.

## Basic Setup

Configure environment variable support by setting an optional prefix:

```c
int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "my_app", "1.0.0");
    
    // Optional: set prefix for all environment variables
    argus.env_prefix = "APP";
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    argus_free(&argus);
    return 0;
}
```

## Environment Variable Types

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
<TabItem value="explicit" label="Explicit Names" default>

Specify exact environment variable names:

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Will look for APP_HOST (with prefix) or HOST (without prefix)
    OPTION_STRING('H', "host", HELP("Server hostname"),
                  ENV_VAR("HOST"), DEFAULT("localhost")),
    
    // Will look for APP_PORT (with prefix) or PORT (without prefix)  
    OPTION_INT('p', "port", HELP("Server port"),
               ENV_VAR("PORT"), DEFAULT(8080)),
    
    // Will look for APP_DATABASE_URL regardless of prefix setting
    OPTION_STRING('d', "database", HELP("Database URL"),
                  ENV_VAR("DATABASE_URL")),
)
```

**Environment usage:**
```bash
# With prefix "APP"
export APP_HOST=production.example.com
export APP_PORT=9000
export APP_DATABASE_URL=postgres://user:pass@db/app

# Without prefix
export HOST=localhost
export PORT=8080
export DATABASE_URL=sqlite:///local.db

./my_app
```

</TabItem>
<TabItem value="auto" label="Auto-Generated">

Automatically generate environment variable names from option names:

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Auto-generates APP_PORT (with prefix) or PORT (without prefix)
    OPTION_INT('p', "port", HELP("Server port"),
               FLAGS(FLAG_AUTO_ENV), DEFAULT(8080)),
    
    // Auto-generates APP_LOG_LEVEL (with prefix) or LOG_LEVEL (without prefix)
    OPTION_STRING('\0', "log-level", HELP("Log level"),
                  FLAGS(FLAG_AUTO_ENV), DEFAULT("info")),
    
    // Auto-generates APP_MAX_CONNECTIONS (dash -> underscore, uppercase)
    OPTION_INT('\0', "max-connections", HELP("Max connections"),
               FLAGS(FLAG_AUTO_ENV), DEFAULT(100)),
)
```

**Name transformation rules:**
- Convert to uppercase: `port` → `PORT`
- Replace dashes with underscores: `log-level` → `LOG_LEVEL`
- Add prefix if set: `LOG_LEVEL` → `APP_LOG_LEVEL`

</TabItem>
<TabItem value="no-prefix" label="Without Prefix">

Force environment variables to ignore the global prefix:

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Always uses DATABASE_URL exactly (ignores prefix)
    OPTION_STRING('d', "database", HELP("Database URL"),
                  ENV_VAR("DATABASE_URL"), FLAGS(FLAG_NO_ENV_PREFIX)),
    
    // Auto-generates VERBOSE (ignores prefix)
    OPTION_FLAG('v', "verbose", HELP("Verbose output"),
                FLAGS(FLAG_AUTO_ENV | FLAG_NO_ENV_PREFIX)),
    
    // Useful for standard system variables
    OPTION_STRING('\0', "home-dir", HELP("Home directory"),
                  ENV_VAR("HOME"), FLAGS(FLAG_NO_ENV_PREFIX)),
)
```

**Usage:**
```bash
# These variables used exactly as specified, regardless of prefix
export DATABASE_URL=postgres://localhost/db
export VERBOSE=true
export HOME=/home/user

./my_app  # Uses these values directly
```

</TabItem>
</Tabs>

## Precedence Control

Control whether environment variables provide defaults or override command-line options:

<Tabs>
<TabItem value="fallback" label="Fallback (Default)" default>

Environment variables provide fallback values when command-line options aren't specified:

```c
ARGUS_OPTIONS(
    options,
    OPTION_STRING('h', "host", HELP("Server host"),
                  ENV_VAR("HOST"), DEFAULT("localhost")),
    
    OPTION_INT('p', "port", HELP("Server port"),
               ENV_VAR("PORT"), DEFAULT(8080)),
)
```

**Precedence order:**
1. Command-line option (highest priority)
2. Environment variable
3. Default value (lowest priority)

**Example:**
```bash
export APP_HOST=env-server.com
export APP_PORT=9000

./my_app                           # Uses env values: env-server.com:9000
./my_app --host cli-server.com     # Uses: cli-server.com:9000 (CLI overrides env)
./my_app --port 8080               # Uses: env-server.com:8080 (CLI overrides env)
```

</TabItem>
<TabItem value="override" label="Override Mode">

Environment variables override command-line options:

```c
ARGUS_OPTIONS(
    options,
    // Environment variable takes precedence over CLI
    OPTION_INT('t', "timeout", HELP("Timeout in seconds"),
               ENV_VAR("FORCE_TIMEOUT"), FLAGS(FLAG_ENV_OVERRIDE),
               DEFAULT(30)),
    
    // Useful for system-wide policies
    OPTION_STRING('l', "log-level", HELP("Log level"),
                  ENV_VAR("SYSTEM_LOG_LEVEL"), FLAGS(FLAG_ENV_OVERRIDE),
                  DEFAULT("info")),
)
```

**Precedence order:**
1. Environment variable (highest priority)
2. Command-line option
3. Default value (lowest priority)

**Example:**
```bash
export APP_FORCE_TIMEOUT=60
export APP_SYSTEM_LOG_LEVEL=debug

./my_app --timeout 30              # Uses: 60 (env overrides CLI)
./my_app --log-level info          # Uses: debug (env overrides CLI)
```

**Use cases:** Security policies, container orchestration, system-wide settings.

</TabItem>
</Tabs>

## Type Conversion

Environment variables (always strings) are automatically converted to option types:

<Tabs>
<TabItem value="basic-types" label="Basic Types" default>

```c
ARGUS_OPTIONS(
    options,
    OPTION_STRING('h', "host", ENV_VAR("HOST")),
    OPTION_INT('p', "port", ENV_VAR("PORT")),
    OPTION_FLOAT('r', "rate", ENV_VAR("RATE")),
    OPTION_BOOL('d', "debug", ENV_VAR("DEBUG")),
    OPTION_FLAG('v', "verbose", ENV_VAR("VERBOSE")),
)
```

**Environment values:**
```bash
export APP_HOST=example.com        # String: used as-is
export APP_PORT=8080               # Integer: parsed as 8080
export APP_RATE=0.75               # Float: parsed as 0.75
export APP_DEBUG=true              # Boolean: parsed as true
export APP_VERBOSE=1               # Flag: parsed as true
```

</TabItem>
<TabItem value="boolean-values" label="Boolean Formats">

Boolean and flag options accept various string formats (case-insensitive):

| True Values | False Values |
|-------------|--------------|
| `"true"`, `"yes"`, `"1"`, `"on"`, `"y"` | `"false"`, `"no"`, `"0"`, `"off"`, `"n"` |

```bash
# All of these enable the debug flag
export APP_DEBUG=true
export APP_DEBUG=True
export APP_DEBUG=YES
export APP_DEBUG=1
export APP_DEBUG=on

# All of these disable the debug flag
export APP_DEBUG=false
export APP_DEBUG=False
export APP_DEBUG=NO
export APP_DEBUG=0
export APP_DEBUG=off
```

</TabItem>
<TabItem value="collections" label="Collections">

Arrays and maps use comma-separated values:

```c
ARGUS_OPTIONS(
    options,
    OPTION_ARRAY_STRING('t', "tags", ENV_VAR("TAGS")),
    OPTION_ARRAY_INT('p', "ports", ENV_VAR("PORTS")),
    OPTION_MAP_STRING('e', "env", ENV_VAR("ENV_VARS")),
)
```

**Environment values:**
```bash
# Array values
export APP_TAGS=web,api,backend
export APP_PORTS=8080,8443,9000

# Map values (key=value pairs)
export APP_ENV_VARS=USER=alice,HOME=/home/alice,TERM=xterm
```

</TabItem>
</Tabs>

## Complete Examples

<Tabs>
<TabItem value="web-app" label="Web Application" default>

```c
#include "argus.h"

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Connection settings
    OPTION_STRING('H', "host", HELP("Server hostname"),
                  ENV_VAR("HOST"), DEFAULT("localhost")),
    
    OPTION_INT('p', "port", HELP("Server port"),
               ENV_VAR("PORT"), DEFAULT(8080)),
    
    // Database configuration
    OPTION_STRING('d', "database", HELP("Database URL"),
                  ENV_VAR("DATABASE_URL"), FLAGS(FLAG_NO_ENV_PREFIX)),
    
    // System settings (auto-generated names)
    OPTION_STRING('\0', "log-level", HELP("Log level"),
                  FLAGS(FLAG_AUTO_ENV), DEFAULT("info")),
    
    OPTION_INT('\0', "worker-threads", HELP("Number of worker threads"),
               FLAGS(FLAG_AUTO_ENV), DEFAULT(4)),
    
    // Security settings (override mode)
    OPTION_BOOL('\0', "ssl-verify", HELP("Verify SSL certificates"),
                ENV_VAR("SSL_VERIFY"), FLAGS(FLAG_ENV_OVERRIDE),
                DEFAULT(true)),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "webapp", "1.0.0");
    argus.env_prefix = "WEBAPP";
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    // Access configuration
    const char *host = argus_get(argus, "host").as_string;
    int port = argus_get(argus, "port").as_int;
    const char *database = argus_get(argus, "database").as_string;
    const char *log_level = argus_get(argus, "log-level").as_string;
    
    printf("Starting webapp on %s:%d\n", host, port);
    printf("Database: %s\n", database ? database : "not configured");
    printf("Log level: %s\n", log_level);
    
    argus_free(&argus);
    return 0;
}
```

**Environment setup:**
```bash
# Production environment
export WEBAPP_HOST=0.0.0.0
export WEBAPP_PORT=443
export DATABASE_URL=postgres://prod-db/webapp
export WEBAPP_LOG_LEVEL=warn
export WEBAPP_WORKER_THREADS=8
export WEBAPP_SSL_VERIFY=true

./webapp
```

</TabItem>
<TabItem value="docker" label="Docker Integration">

```dockerfile
FROM alpine:latest

# Set environment variables for the application
ENV APP_HOST=0.0.0.0
ENV APP_PORT=8080
ENV APP_LOG_LEVEL=info
ENV APP_WORKER_THREADS=2
ENV DATABASE_URL=postgres://db:5432/myapp

# Copy application
COPY ./myapp /usr/local/bin/myapp

# Application runs with environment configuration
CMD ["myapp"]
```

**Docker Compose:**
```yaml
version: '3.8'
services:
  app:
    image: myapp:latest
    environment:
      - APP_HOST=0.0.0.0
      - APP_PORT=8080
      - APP_LOG_LEVEL=debug
      - DATABASE_URL=postgres://db:5432/myapp
    ports:
      - "8080:8080"
  
  db:
    image: postgres:13
    environment:
      - POSTGRES_DB=myapp
      - POSTGRES_USER=user
      - POSTGRES_PASSWORD=password
```

</TabItem>
<TabItem value="ci-cd" label="CI/CD Pipeline">

```yaml
# GitHub Actions example
name: Build and Test
on: [push]

jobs:
  test:
    runs-on: ubuntu-latest
    env:
      # Test environment configuration
      APP_LOG_LEVEL: debug
      APP_DATABASE_URL: sqlite:///test.db
      APP_MAX_CONNECTIONS: 10
      APP_TIMEOUT: 5
    
    steps:
      - uses: actions/checkout@v2
      - name: Run tests
        run: |
          # Application automatically picks up environment variables
          ./myapp test --coverage
  
  deploy:
    runs-on: ubuntu-latest
    needs: test
    env:
      # Production environment configuration
      APP_HOST: ${{ secrets.PROD_HOST }}
      APP_PORT: 443
      APP_LOG_LEVEL: warn
      DATABASE_URL: ${{ secrets.DATABASE_URL }}
    
    steps:
      - name: Deploy
        run: |
          # Deploy with production environment
          ./deploy.sh
```

</TabItem>
</Tabs>

## Best Practices

### ✅ Good Practices

```c
// 1. Use consistent naming with prefixes
argus.env_prefix = "MYAPP";

// 2. Provide sensible defaults
OPTION_STRING('h', "host", ENV_VAR("HOST"), DEFAULT("localhost"))

// 3. Use FLAG_NO_ENV_PREFIX for standard system variables
OPTION_STRING('\0', "home", ENV_VAR("HOME"), FLAGS(FLAG_NO_ENV_PREFIX))

// 4. Use override mode for security-critical settings
OPTION_BOOL('\0', "ssl-verify", ENV_VAR("SSL_VERIFY"), 
            FLAGS(FLAG_ENV_OVERRIDE), DEFAULT(true))

// 5. Document environment variables in help text
OPTION_STRING('d', "database", HELP("Database URL (env: DATABASE_URL)"),
              ENV_VAR("DATABASE_URL"))
```

### ❌ Avoid These Patterns

```c
// ❌ Don't use confusing environment variable names
OPTION_STRING('p', "port", ENV_VAR("SERVER_PORT_NUMBER_CONFIG"))

// ❌ Don't override critical security settings by default
OPTION_BOOL('\0', "skip-auth", ENV_VAR("SKIP_AUTH"), 
            FLAGS(FLAG_ENV_OVERRIDE))  // Dangerous!

// ❌ Don't forget to set prefix consistently
// Set argus.env_prefix once, not per option
```

### Documentation

Argus automatically displays environment variables in the help output.
Simply define your options and the help system will show the corresponding environment variables:

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    OPTION_STRING('h', "host", HELP("Server hostname"),
                  ENV_VAR("HOST"), DEFAULT("localhost")),
    
    OPTION_INT('t', "timeout", HELP("Timeout in seconds"),
               ENV_VAR("FORCE_TIMEOUT"), FLAGS(FLAG_ENV_OVERRIDE)),
    
    OPTION_INT('p', "port", HELP("Server port"),
               FLAGS(FLAG_AUTO_ENV), DEFAULT(8080)),
)
```

**Automatic help output:**
```
Options:
  -h, --host <STR>       - Server hostname (default: "localhost", env:
                           APP_HOST)
  -t, --timeout <NUM>    - Timeout in seconds (env: APP_FORCE_TIMEOUT)  
  -p, --port <NUM>       - Server port (default: 8080, env: APP_PORT)
```

## Environment Variable Loading Process

Argus loads environment variables in this order:

1. **Parse command-line arguments** - Process all CLI options first
2. **Load environment variables** - Check for environment values
3. **Apply precedence rules** - Respect `FLAG_ENV_OVERRIDE` settings
4. **Apply defaults** - Use default values for unset options
5. **Run validation** - Validate all final values

## Integration with Other Features

Environment variables work seamlessly with other Argus features:

```c
ARGUS_OPTIONS(
    options,
    // Environment + validation
    OPTION_INT('p', "port", ENV_VAR("PORT"),
               VALIDATOR(V_RANGE(1, 65535)), DEFAULT(8080)),
    
    // Environment + choices
    OPTION_STRING('l', "level", ENV_VAR("LOG_LEVEL"),
                  VALIDATOR(V_CHOICE_STR("debug", "info", "warn", "error")),
                  DEFAULT("info")),
    
    // Environment + collections
    OPTION_ARRAY_STRING('t', "tags", ENV_VAR("TAGS"),
                        VALIDATOR(V_COUNT(1, 10))),
)
```

## What's Next?

- **[Subcommands](subcommands)** - Build Git-like command hierarchies
- **[Collections](collections)** - Work with arrays and maps
- **[Validation](validation)** - Ensure environment values are valid
- **[Custom Handlers](../advanced/custom-handlers)** - Process complex environment data

:::tip Debugging Environment Variables
Use `printenv | grep APP_` to see what environment variables are set with your prefix, and add verbose logging to verify which values are being loaded.
:::
