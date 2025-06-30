# Configuration Tool

Advanced example demonstrating collections, environment variables, and complex validation.

## Complete Example

```c title="config_tool.c"
#include <argus.h>
#include <stdio.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Arrays
    OPTION_ARRAY_STRING('t', "tags", HELP("Resource tags"),
                        FLAGS(FLAG_UNIQUE | FLAG_SORTED),
                        VALIDATOR(V_COUNT(1, 10))),
    
    OPTION_ARRAY_INT('p', "ports", HELP("Port numbers (supports ranges)"),
                     VALIDATOR(V_COUNT(1, 5))),
    
    // Maps  
    OPTION_MAP_STRING('e', "env", HELP("Environment variables"),
                      ENV_VAR("CONFIG_ENV"), FLAGS(FLAG_SORTED_KEY)),
    
    OPTION_MAP_BOOL('f', "features", HELP("Feature toggles"),
                    FLAGS(FLAG_SORTED_KEY)),
    
    // Environment integration
    OPTION_STRING('n', "name", HELP("Service name"),
                  ENV_VAR("SERVICE_NAME"), FLAGS(FLAG_REQUIRED)),
    
    OPTION_STRING('h', "host", HELP("Host address"),
                  ENV_VAR("HOST"), DEFAULT("localhost")),
    
    OPTION_INT('P', "port", HELP("Main port"),
               ENV_VAR("PORT"), VALIDATOR(V_RANGE(1, 65535)), DEFAULT(8080)),
    
    // Email validation
    OPTION_STRING('c', "contact", HELP("Contact email"),
                  VALIDATOR(V_REGEX(ARGUS_RE_EMAIL))),
    
    // Optional output
    POSITIONAL_STRING("output", HELP("Output file"), FLAGS(FLAG_OPTIONAL)),
)

void print_array_strings(argus_t argus, const char *name)
{
    if (!argus_is_set(&argus, name)) return;
    
    printf("%s:\n", name);
    argus_array_it_t it = argus_array_it(&argus, name);
    while (argus_array_next(&it))
        printf("  - %s\n", it.value.as_string);
}

void print_array_ints(argus_t argus, const char *name)
{
    if (!argus_is_set(&argus, name)) return;
    
    printf("%s:\n", name);
    argus_array_it_t it = argus_array_it(&argus, name);
    while (argus_array_next(&it))
        printf("  - %d\n", it.value.as_int);
}

void print_map_strings(argus_t argus, const char *name)
{
    if (!argus_is_set(&argus, name)) return;
    
    printf("%s:\n", name);
    argus_map_it_t it = argus_map_it(&argus, name);
    while (argus_map_next(&it))
        printf("  %s = %s\n", it.key, it.value.as_string);
}

void print_map_bools(argus_t argus, const char *name)
{
    if (!argus_is_set(&argus, name)) return;
    
    printf("%s:\n", name);
    argus_map_it_t it = argus_map_it(&argus, name);
    while (argus_map_next(&it))
        printf("  %s = %s\n", it.key, it.value.as_bool ? "true" : "false");
}

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "config_tool", "2.0.0");
    argus.description = "Configuration management with collections and environment support";
    argus.env_prefix = "CONFIG";
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    // Basic values
    const char *name = argus_get(&argus, "name").as_string;
    const char *host = argus_get(&argus, "host").as_string;
    int port = argus_get(&argus, "port").as_int;
    
    printf("=== Service Configuration ===\n");
    printf("Name: %s\n", name);
    printf("Host: %s:%d\n", host, port);
    
    if (argus_is_set(&argus, "contact"))
        printf("Contact: %s\n", argus_get(&argus, "contact").as_string);
    printf("\n");
    
    // Collections
    print_array_strings(argus, "tags");
    print_array_ints(argus, "ports");
    print_map_strings(argus, "env");
    print_map_bools(argus, "features");
    
    // Generate config file
    const char *output = "config.json";
    if (argus_is_set(&argus, "output")) {
        output = argus_get(&argus, "output").as_string;
    }
    
    FILE *config = fopen(output, "w");
    if (!config) {
        fprintf(stderr, "Error: Cannot create config file '%s'\n", output);
        return 1;
    }
    
    fprintf(config, "{\n");
    fprintf(config, "  \"name\": \"%s\",\n", name);
    fprintf(config, "  \"host\": \"%s\",\n", host);
    fprintf(config, "  \"port\": %d", port);
    
    if (argus_is_set(&argus, "contact")) {
        fprintf(config, ",\n  \"contact\": \"%s\"", argus_get(&argus, "contact").as_string);
    }
    
    // Add arrays
    if (argus_is_set(&argus, "tags")) {
        fprintf(config, ",\n  \"tags\": [");
        argus_array_it_t it = argus_array_it(&argus, "tags");
        bool first = true;
        while (argus_array_next(&it)) {
            if (!first) fprintf(config, ", ");
            fprintf(config, "\"%s\"", it.value.as_string);
            first = false;
        }
        fprintf(config, "]");
    }
    
    if (argus_is_set(&argus, "ports")) {
        fprintf(config, ",\n  \"ports\": [");
        argus_array_it_t it = argus_array_it(&argus, "ports");
        bool first = true;
        while (argus_array_next(&it)) {
            if (!first) fprintf(config, ", ");
            fprintf(config, "%d", it.value.as_int);
            first = false;
        }
        fprintf(config, "]");
    }
    
    // Add maps
    if (argus_is_set(&argus, "env")) {
        fprintf(config, ",\n  \"environment\": {");
        argus_map_it_t it = argus_map_it(&argus, "env");
        bool first = true;
        while (argus_map_next(&it)) {
            if (!first) fprintf(config, ", ");
            fprintf(config, "\n    \"%s\": \"%s\"", it.key, it.value.as_string);
            first = false;
        }
        if (!first) fprintf(config, "\n  ");
        fprintf(config, "}");
    }
    
    if (argus_is_set(&argus, "features")) {
        fprintf(config, ",\n  \"features\": {");
        argus_map_it_t it = argus_map_it(&argus, "features");
        bool first = true;
        while (argus_map_next(&it)) {
            if (!first) fprintf(config, ", ");
            fprintf(config, "\n    \"%s\": %s", it.key, it.value.as_bool ? "true" : "false");
            first = false;
        }
        if (!first) fprintf(config, "\n  ");
        fprintf(config, "}");
    }
    
    fprintf(config, "\n}\n");
    fclose(config);
    
    printf("Configuration written to: %s\n", output);
    
    argus_free(&argus);
    return 0;
}
```

## Usage Examples

### Command Line

```bash
# Build
gcc config_tool.c -o config_tool -largus

# Basic usage
./config_tool --name myservice

# With collections
./config_tool --name api \
  --tags=web,backend,api \
  --ports=8080,8443,9000 \
  --env=DEBUG=true,LOG_LEVEL=info \
  --features=cache=true,metrics=false

# Range expansion for ports
./config_tool --name worker --ports=8000-8005,9000

# Custom output
./config_tool --name db --output database.json
```

### Environment Variables

```bash
# Set environment variables
export CONFIG_SERVICE_NAME=production-api
export CONFIG_HOST=api.company.com
export CONFIG_PORT=443
export CONFIG_CONFIG_ENV=DEPLOY_ENV=prod,VERSION=2.1.0

# Run with environment
./config_tool --tags=production,api

# Environment overrides CLI (for some options)
./config_tool --name dev-service  # Uses CONFIG_SERVICE_NAME instead
```

## Generated Help

```
config_tool v2.0.0

Configuration management with collections and environment support

Usage: config_tool [OPTIONS] [output]

Arguments:
  [output]               - Output file (optional)

Options:
  -h, --help             - Display this help message (exit)
  -V, --version          - Display version information (exit)
  -t, --tags <STR,...>   - Resource tags (count: 1-10) (sorted, unique)
  -p, --ports <NUM,...>  - Port numbers (supports ranges) (count: 1-5)
  -e, --env <KEY=VAL,...>
                         - Environment variables (env: CONFIG_CONFIG_ENV)
                           (sorted by key)
  -f, --features <KEY=BOOL,...>
                         - Feature toggles (sorted by key)
  -n, --name <STR>       - Service name (env: CONFIG_SERVICE_NAME) (required)
  -h, --host <STR>       - Host address (env: CONFIG_HOST) (default:
                           "localhost")
  -P, --port <1-65535>   - Main port (env: CONFIG_PORT) (default: 8080)
  -c, --contact <STR>    - Contact email
```

## Advanced Features

### Collection Processing

```bash
# Arrays with flags
--tags=web,api,web,backend  # Becomes: api,backend,web (unique + sorted)

# Integer ranges
--ports=8000-8003,9000      # Becomes: 8000,8001,8002,8003,9000

# Maps with sorting
--env=ZEBRA=last,ALPHA=first  # Sorted by key: ALPHA=first,ZEBRA=last
```

### Environment Integration

```bash
# Multiple approaches
CONFIG_SERVICE_NAME=myapp ./config_tool --tags=web
./config_tool --name myapp --tags=web  # Same result

# Complex environment
export CONFIG_CONFIG_ENV="DB_HOST=localhost,DB_PORT=5432,DEBUG=true"
./config_tool --name myapp  # Automatically loads env map
```

### Validation Examples

```bash
# Array count validation
./config_tool --name test --tags=a,b,c,d,e,f,g,h,i,j,k
# Error: Values count 11 is out of [1, 10]

# Email validation
./config_tool --name test --contact=invalid-email
# Error: Invalid value 'invalid-email': Enter email: user@example.com

# Port range validation
./config_tool --name test --port=99999
# Error: Value 99999 is out of range [1, 65535]
```

## Generated Config

```json
{
  "name": "api",
  "host": "localhost",
  "port": 8080,
  "contact": "admin@company.com",
  "tags": ["api", "backend", "web"],
  "ports": [8080, 8443, 9000],
  "environment": {
    "DEBUG": "true",
    "LOG_LEVEL": "info"
  },
  "features": {
    "cache": true,
    "metrics": false
  }
}
```

## Key Features Demonstrated

- **String arrays** with unique/sorted flags
- **Integer arrays** with range expansion (8000-8003)
- **String maps** for key-value configuration
- **Boolean maps** for feature toggles
- **Environment variables** with prefix support
- **Email validation** using regex patterns
- **Count validation** for collections
- **Optional positional** arguments
- **JSON output** generation from parsed data
