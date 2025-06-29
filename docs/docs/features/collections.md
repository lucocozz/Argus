# Collections

Handle multiple values and key-value pairs with arrays and maps for complex command-line scenarios.

## Array Options

Collect multiple values for a single option through various input methods:

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
<TabItem value="array-types" label="Array Types" default>

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // String arrays
    OPTION_ARRAY_STRING('n', "names", HELP("List of names")),
    
    // Integer arrays with range expansion
    OPTION_ARRAY_INT('p', "ports", HELP("Port numbers")),
    
    // Float arrays
    OPTION_ARRAY_FLOAT('r', "rates", HELP("Processing rates")),
)
```

**Supported types:**
- `OPTION_ARRAY_STRING` - Text values
- `OPTION_ARRAY_INT` - Integer values with range support
- `OPTION_ARRAY_FLOAT` - Decimal values

</TabItem>
<TabItem value="array-input" label="Input Formats">

Users can provide array values in multiple ways:

```bash
# Multiple option occurrences
./program --names=john --names=alice --names=bob

# Comma-separated values
./program --names=john,alice,bob

# Combined approaches
./program --names=john,alice --names=bob

# Short option format
./program -n john -n alice -n bob
./program -njohn,alice,bob
```

**Integer ranges (special syntax):**
```bash
# Range expansion with dashes or colons
./program --ports=8000-8005,9000
./program --ports=8000:8005,9000
# Expands to: 8000,8001,8002,8003,8004,8005,9000

./program --ids=1-5,10,15-20
# Expands to: 1,2,3,4,5,10,15,16,17,18,19,20
```

</TabItem>
<TabItem value="array-flags" label="Array Flags">

Control array processing with flags:

```c
ARGUS_OPTIONS(
    options,
    // Sorted arrays
    OPTION_ARRAY_STRING('t', "tags", HELP("Resource tags"),
                        FLAGS(FLAG_SORTED)),
    
    // Unique values only
    OPTION_ARRAY_INT('p', "ports", HELP("Port numbers"),
                     FLAGS(FLAG_UNIQUE)),
    
    // Both sorted and unique
    OPTION_ARRAY_STRING('l', "labels", HELP("Labels"),
                        FLAGS(FLAG_SORTED | FLAG_UNIQUE)),
)
```

**Flag behavior:**
- `FLAG_SORTED` - Sort values after collection
- `FLAG_UNIQUE` - Remove duplicate values
- Combined flags - Remove duplicates, then sort

</TabItem>
</Tabs>

## Map Options

Collect key-value pairs for configuration-style options:

<Tabs>
<TabItem value="map-types" label="Map Types" default>

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // String maps
    OPTION_MAP_STRING('e', "env", HELP("Environment variables")),
    
    // Integer maps
    OPTION_MAP_INT('p', "ports", HELP("Service port mappings")),
    
    // Float maps
    OPTION_MAP_FLOAT('s', "scales", HELP("Scaling factors")),
    
    // Boolean maps
    OPTION_MAP_BOOL('f', "features", HELP("Feature toggles")),
)
```

**Supported value types:**
- `OPTION_MAP_STRING` - Text values
- `OPTION_MAP_INT` - Integer values
- `OPTION_MAP_FLOAT` - Decimal values
- `OPTION_MAP_BOOL` - Boolean values

</TabItem>
<TabItem value="map-input" label="Input Formats">

```bash
# Multiple key-value pairs
./program --env=USER=alice --env=HOME=/home/alice

# Comma-separated pairs
./program --env=USER=alice,HOME=/home/alice,TERM=xterm

# Combined approaches
./program --env=USER=alice,HOME=/home/alice --env=TERM=xterm

# Different value types
./program --ports=http=80,https=443,ftp=21
./program --features=debug=true,cache=false,logging=on
```

**Boolean value formats (case-insensitive):**
- **True**: `true`, `yes`, `1`, `on`, `y`
- **False**: `false`, `no`, `0`, `off`, `n`

</TabItem>
<TabItem value="map-flags" label="Map Flags">

Control map organization:

```c
ARGUS_OPTIONS(
    options,
    // Sort by keys
    OPTION_MAP_STRING('e', "env", HELP("Environment variables"),
                      FLAGS(FLAG_SORTED_KEY)),
    
    // Sort by values
    OPTION_MAP_INT('p', "ports", HELP("Port mappings"),
                   FLAGS(FLAG_SORTED_VALUE)),
    
    // Unique values only
    OPTION_MAP_STRING('c', "config", HELP("Configuration"),
                      FLAGS(FLAG_UNIQUE_VALUE)),
)
```

**Flag behavior:**
- `FLAG_SORTED_KEY` - Sort entries by key
- `FLAG_SORTED_VALUE` - Sort entries by value
- `FLAG_UNIQUE_VALUE` - Remove duplicate values (keep first key)

</TabItem>
</Tabs>

## Accessing Collection Values

<Tabs>
<TabItem value="direct-access" label="Direct Access" default>

Access the raw array or map data:

```c
ARGUS_OPTIONS(
    options,
    OPTION_ARRAY_STRING('n', "names", HELP("Names")),
    OPTION_MAP_STRING('e', "env", HELP("Environment variables")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Access arrays
    if (argus_is_set(&argus, "names")) {
        size_t count = argus_count(&argus, "names");
        argus_value_t *names = argus_get(&argus, "names").as_array;
        
        printf("Names (%zu):\n", count);
        for (size_t i = 0; i < count; i++)
            printf("  %zu: %s\n", i + 1, names[i].as_string);
    }
    
    // Access maps
    if (argus_is_set(&argus, "env")) {
        size_t count = argus_count(&argus, "env");
        argus_pair_t *env = argus_get(&argus, "env").as_map;
        
        printf("Environment (%zu):\n", count);
        for (size_t i = 0; i < count; i++)
            printf("  %s = %s\n", env[i].key, env[i].value.as_string);
    }
    
    argus_free(&argus);
    return 0;
}
```

</TabItem>
<TabItem value="helper-access" label="Helper Functions">

Use convenience functions for specific elements:

```c
int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Array element access
    const char *first_name = argus_array_get(&argus, "names", 0).as_string;
    const char *second_name = argus_array_get(&argus, "names", 1).as_string;
    
    if (first_name) 
        printf("First name: %s\n", first_name);
    
    // Map key lookup
    const char *user = argus_map_get(&argus, "env", "USER").as_string;
    const char *home = argus_map_get(&argus, "env", "HOME").as_string;
    int http_port = argus_map_get(&argus, "ports", "http").as_int;
    bool debug = argus_map_get(&argus, "features", "debug").as_bool;
    
    if (user) printf("User: %s\n", user);
    if (http_port) printf("HTTP port: %d\n", http_port);
    
    argus_free(&argus);
    return 0;
}
```

**Helper functions:**
- `argus_array_get(&argus, "option", index)` - Get array element by index
- `argus_map_get(&argus, "option", "key")` - Get map value by key
- Return empty value (`{.raw = 0}`) for invalid indices/keys

:::warning Safe Collection Access
Always check collection size before accessing elements:

```c
// ✅ Safe array access
size_t count = argus_count(&argus, "names");
if (count > 0) {
    const char *first = argus_array_get(&argus, "names", 0).as_string;
}

// ✅ Safe map access  
if (argus_is_set(&argus, "env")) {
    const char *user = argus_map_get(&argus, "env", "USER").as_string;
    if (user) {
        // Use user value
    }
}

// ❌ Unsafe - may access invalid indices
const char *first = argus_array_get(&argus, "names", 0).as_string; // Could be NULL
```
:::

</TabItem>
<TabItem value="iterators" label="Iterator Pattern">

Use iterators for clean traversal:

```c
int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Array iteration
    argus_array_it_t names_it = argus_array_it(&argus, "names");
    printf("Names:\n");
    while (argus_array_next(&names_it))
        printf("  - %s\n", names_it.value.as_string);
    
    // Map iteration
    argus_map_it_t env_it = argus_map_it(&argus, "env");
    printf("Environment:\n");
    while (argus_map_next(&env_it))
        printf("  %s = %s\n", env_it.key, env_it.value.as_string);
    
    // Reset and iterate again
    argus_array_reset(&names_it);
    printf("Second pass:\n");
    while (argus_array_next(&names_it))
        printf("  * %s\n", names_it.value.as_string);
    
    argus_free(&argus);
    return 0;
}
```

**Iterator functions:**
- `argus_array_it()` / `argus_map_it()` - Create iterator
- `argus_array_next()` / `argus_map_next()` - Get next element
- `argus_array_reset()` / `argus_map_reset()` - Reset to beginning

</TabItem>
</Tabs>

## Validation with Collections

Apply validation to collection size and content:

<Tabs>
<TabItem value="count-validation" label="Count Validation" default>

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Require 1-5 tags
    OPTION_ARRAY_STRING('t', "tags", HELP("Resource tags"),
                        VALIDATOR(V_COUNT(1, 5))),
    
    // Optional labels (0-3)
    OPTION_ARRAY_STRING('l', "labels", HELP("Optional labels"),
                        VALIDATOR(V_COUNT(0, 3))),
    
    // Environment variables (1-10 pairs)
    OPTION_MAP_STRING('e', "env", HELP("Environment variables"),
                      VALIDATOR(V_COUNT(1, 10))),
)
```

**Usage:**
```bash
./program --tags=web,api,backend        # ✅ Valid (3 tags)
./program --tags=a,b,c,d,e,f           # ❌ Error: Count 6 is out of [1, 5]
./program                              # ❌ Error: Count 0 is out of [1, 5] (for tags)
```

</TabItem>
<TabItem value="combined-validation" label="Combined Validation">

```c
ARGUS_OPTIONS(
    options,
    // Multiple validators on collections
    OPTION_ARRAY_STRING('e', "emails", HELP("Email addresses"),
                        VALIDATOR(V_COUNT(1, 5))),
    
    // Flags with validation
    OPTION_ARRAY_INT('p', "ports", HELP("Port numbers"),
                     FLAGS(FLAG_SORTED | FLAG_UNIQUE),
                     VALIDATOR(V_COUNT(1, 10))),
)
```

Processing order:
1. Collect values from command line
2. Apply flags (sorting, uniqueness)
3. Run count validation
4. Additional custom validation

</TabItem>
</Tabs>

## Real-World Examples

<Tabs>
<TabItem value="build-tool" label="Build Tool" default>

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Build targets
    OPTION_ARRAY_STRING('t', "targets", HELP("Build targets"),
                        FLAGS(FLAG_UNIQUE),
                        VALIDATOR(V_COUNT(1, 10))),
    
    // Compiler flags
    OPTION_ARRAY_STRING('f', "flags", HELP("Compiler flags")),
    
    // Environment overrides
    OPTION_MAP_STRING('D', "define", HELP("Preprocessor definitions")),
    
    // Feature toggles
    OPTION_MAP_BOOL('\0', "features", HELP("Feature flags"),
                    FLAGS(FLAG_SORTED_KEY)),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "builder", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Process build targets
    argus_array_it_t targets_it = argus_array_it(&argus, "targets");
    while (argus_array_next(&targets_it)) {
        printf("Building target: %s\n", targets_it.value.as_string);
        build_target(targets_it.value.as_string);
    }
    
    // Apply feature flags
    argus_map_it_t features_it = argus_map_it(&argus, "features");
    while (argus_map_next(&features_it)) {
        set_feature(features_it.key, features_it.value.as_bool);
    }
    
    argus_free(&argus);
    return 0;
}
```

**Usage:**
```bash
./builder --targets=lib,bin,tests \
          --define=DEBUG=1,VERSION=2.0 \
          --features=optimization=true,debug=false
```

</TabItem>
<TabItem value="deployment" label="Deployment Tool">

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Deployment environments
    OPTION_ARRAY_STRING('e', "environments", HELP("Target environments"),
                        FLAGS(FLAG_UNIQUE | FLAG_SORTED),
                        VALIDATOR(V_COUNT(1, 5))),
    
    // Service configuration
    OPTION_MAP_STRING('s', "services", HELP("Service configurations")),
    
    // Resource limits
    OPTION_MAP_INT('r', "resources", HELP("Resource limits")),
    
    // Feature rollout percentages
    OPTION_MAP_FLOAT('f', "rollout", HELP("Feature rollout percentages")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "deploy", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Deploy to each environment
    argus_array_it_t env_it = argus_array_it(&argus, "environments");
    while (argus_array_next(&env_it)) {
        const char *env = env_it.value.as_string;
        printf("Deploying to %s...\n", env);
        
        // Apply resource limits for this environment
        char key_buffer[64];
        snprintf(key_buffer, sizeof(key_buffer), "%s_cpu", env);
        int cpu_limit = argus_map_get(&argus, "resources", key_buffer).as_int;
        if (cpu_limit) {
            printf("  CPU limit: %d\n", cpu_limit);
        }
        
        deploy_to_environment(env);
    }
    
    argus_free(&argus);
    return 0;
}
```

**Usage:**
```bash
./deploy --environments=staging,production \
         --services=web=nginx:1.20,api=myapp:v2.1 \
         --resources=staging_cpu=2,production_cpu=8 \
         --rollout=feature_x=50.0,feature_y=100.0
```

</TabItem>
</Tabs>

## Performance Considerations

Choose the right access method based on usage patterns:

| Access Method | Best For | Performance |
|---------------|----------|-------------|
| **Direct Access** | Random access, full processing | O(1) access |
| **Helper Functions** | Specific element lookup | O(1) arrays, O(n) maps |
| **Iterators** | Sequential processing | Most efficient for full traversal |

**Memory usage:**
- Initial capacity: 8 elements
- Growth: Doubles when needed
- Use `V_COUNT()` validation for large collections

## Collection Flags Summary

| Flag | Arrays | Maps | Effect |
|------|--------|------|--------|
| `FLAG_SORTED` | ✅ | ❌ | Sort array values |
| `FLAG_UNIQUE` | ✅ | ❌ | Remove duplicate values |
| `FLAG_SORTED_KEY` | ❌ | ✅ | Sort map by keys |
| `FLAG_SORTED_VALUE` | ❌ | ✅ | Sort map by values |
| `FLAG_UNIQUE_VALUE` | ❌ | ✅ | Remove duplicate values |

**Flag precedence:** `FLAG_SORTED_KEY` overrides `FLAG_SORTED_VALUE`

## What's Next?

- **[Validation](validation)** - Validate collection content and constraints
- **[Environment Variables](environment)** - Load collections from environment
- **[Custom Handlers](../advanced/custom-handlers)** - Process complex collection data
- **[Subcommands](subcommands)** - Use collections in subcommand options

:::tip Collection Design
Design collections around user workflows. Arrays work well for lists of similar items, while maps excel at configuration-style key-value pairs.
:::
