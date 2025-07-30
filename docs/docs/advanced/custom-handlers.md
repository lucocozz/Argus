# > Custom Handlers_

Transform raw command-line input into structured data with custom parsing logic for specialized data types.

## // Handler Function Signature

All custom handlers must follow this signature:

```c
int handler_function(argus_t *argus, argus_option_t *option, char *arg);
```

**Parameters:**
- `argus` - Context for error reporting
- `option` - Where to store the parsed value
- `arg` - Raw string to parse (`NULL` for flags)

**Return values:**
- `ARGUS_SUCCESS` (0) - Parsing succeeded
- Error code - Parsing failed (use `ARGUS_PARSING_ERROR` and return error code)

## // Basic Custom Handler

Parse a "host:port" endpoint into a structured type:

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
<TabItem value="handler" label="Handler Implementation" default>

```c
typedef struct {
    char *host;
    int port;
} endpoint_t;

int endpoint_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    // Allocate structure
    endpoint_t *endpoint = calloc(1, sizeof(endpoint_t));
    if (!endpoint) {
        ARGUS_PARSING_ERROR(argus, "Memory allocation failed");
        return ARGUS_ERROR_MEMORY;
    }
    
    // Store immediately for automatic cleanup
    option->value.as_ptr = endpoint;
    option->is_allocated = true;
    
    // Find separator
    char *colon = strchr(arg, ':');
    if (!colon) {
        ARGUS_PARSING_ERROR(argus, "Invalid format '%s' (expected host:port)", arg);
        return ARGUS_ERROR_INVALID_FORMAT;
    }
    
    // Extract host
    size_t host_len = colon - arg;
    endpoint->host = strndup(arg, host_len);
    if (!endpoint->host) {
        ARGUS_PARSING_ERROR(argus, "Memory allocation failed");
        return ARGUS_ERROR_MEMORY;
    }
    
    // Extract and validate port
    long port = strtol(colon + 1, NULL, 10);
    if (port <= 0 || port > 65535) {
        ARGUS_PARSING_ERROR(argus, "Invalid port %ld (must be 1-65535)", port);
        return ARGUS_ERROR_INVALID_VALUE;
    }
    
    endpoint->port = (int)port;
    return ARGUS_SUCCESS;
}
```

</TabItem>
<TabItem value="free-handler" label="Free Handler">

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

**Required when:**
- Handler allocates memory
- Set `option->is_allocated = true`
- Argus calls free handler during cleanup

</TabItem>
<TabItem value="usage" label="Usage">

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Custom handler with OPTION_BASE
    OPTION_BASE('e', "endpoint", VALUE_TYPE_CUSTOM,
                HANDLER(endpoint_handler),
                FREE_HANDLER(endpoint_free_handler),
                HELP("Server endpoint (host:port)")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "client", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Access custom type
    endpoint_t *ep = (endpoint_t*)argus_get(&argus, "endpoint").as_ptr;
    if (ep)
        printf("Connecting to %s:%d\n", ep->host, ep->port);

    argus_free(&argus);
    return 0;
}
```

**Usage:**
```bash
./client --endpoint=api.example.com:443
./client -e localhost:8080
```

</TabItem>
</Tabs>

## // Helper Macros

Create reusable macros for common custom types:

<Tabs>
<TabItem value="macro-definition" label="Macro Definition" default>

```c
// Helper macro for endpoint options
#define OPTION_ENDPOINT(short_name, long_name, ...) \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_CUSTOM, \
                HANDLER(endpoint_handler), \
                FREE_HANDLER(endpoint_free_handler), \
                ##__VA_ARGS__)

// Positional version
#define POSITIONAL_ENDPOINT(name, ...) \
    POSITIONAL_BASE(name, VALUE_TYPE_CUSTOM, \
                    HANDLER(endpoint_handler), \
                    FREE_HANDLER(endpoint_free_handler), \
                    ##__VA_ARGS__)
```

</TabItem>
<TabItem value="macro-usage" label="Clean Usage">

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Much cleaner with dedicated macros
    OPTION_ENDPOINT('s', "server", HELP("Server endpoint")),
    OPTION_ENDPOINT('d', "database", HELP("Database endpoint"), 
                   DEFAULT("localhost:5432")),
    
    POSITIONAL_ENDPOINT("target", HELP("Target endpoint")),
)
```

</TabItem>
</Tabs>

## // Advanced Examples

<Tabs>
<TabItem value="url-parser" label="URL Parser" default>

```c
typedef struct {
    char *protocol;
    char *host;
    int port;
    char *path;
    char *query;
} url_t;

int url_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    url_t *url = calloc(1, sizeof(url_t));
    if (!url) {
        ARGUS_PARSING_ERROR(argus, "Memory allocation failed");
        return ARGUS_ERROR_MEMORY;
    }
    
    option->value.as_ptr = url;
    option->is_allocated = true;
    
    // Parse protocol
    char *protocol_end = strstr(arg, "://");
    if (protocol_end) {
        size_t protocol_len = protocol_end - arg;
        url->protocol = strndup(arg, protocol_len);
        arg = protocol_end + 3;
    } else {
        url->protocol = strdup("http");
    }
    
    // Parse host and port
    char *host_end = strchr(arg, '/');
    char *port_start = strchr(arg, ':');
    
    size_t host_len;
    if (port_start && (!host_end || port_start < host_end)) {
        host_len = port_start - arg;
        url->port = atoi(port_start + 1);
    } else {
        host_len = host_end ? (size_t)(host_end - arg) : strlen(arg);
        url->port = (strcmp(url->protocol, "https") == 0) ? 443 : 80;
    }
    
    url->host = strndup(arg, host_len);
    
    // Parse path and query
    if (host_end) {
        char *query_start = strchr(host_end, '?');
        if (query_start) {
            url->path = strndup(host_end, query_start - host_end);
            url->query = strdup(query_start + 1);
        } else {
            url->path = strdup(host_end);
        }
    } else {
        url->path = strdup("/");
    }
    
    return ARGUS_SUCCESS;
}

int url_free_handler(argus_option_t *option)
{
    url_t *url = (url_t*)option->value.as_ptr;
    if (url) {
        free(url->protocol);
        free(url->host);
        free(url->path);
        free(url->query);
        free(url);
    }
    return ARGUS_SUCCESS;
}
```

</TabItem>
<TabItem value="coordinate-parser" label="Coordinate Parser">

```c
typedef struct {
    double latitude;
    double longitude;
} coordinate_t;

int coordinate_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    coordinate_t *coord = calloc(1, sizeof(coordinate_t));
    if (!coord) {
        ARGUS_PARSING_ERROR(argus, "Memory allocation failed");
        return ARGUS_ERROR_MEMORY;
    }

    option->value.as_ptr = coord;
    option->is_allocated = true;

    // Parse "lat,lon" format
    char *comma = strchr(arg, ',');
    if (!comma) {
        free(coord);
        ARGUS_PARSING_ERROR(argus, "Invalid format '%s' (expected lat,lon)", arg);
        return ARGUS_ERROR_INVALID_FORMAT;
    }
    
    // Parse latitude
    char *endptr;
    coord->latitude = strtod(arg, &endptr);
    if (endptr != comma) {
        free(coord);
        ARGUS_PARSING_ERROR(argus, "Invalid latitude value");
        return ARGUS_ERROR_INVALID_VALUE;
    }
    
    // Parse longitude
    coord->longitude = strtod(comma + 1, &endptr);
    if (*endptr != '\0') {
        free(coord);
        ARGUS_PARSING_ERROR(argus, "Invalid longitude value");
        return ARGUS_ERROR_INVALID_VALUE;
    }
    
    // Validate ranges
    if (coord->latitude < -90.0 || coord->latitude > 90.0) {
        free(coord);
        ARGUS_PARSING_ERROR(argus, "Latitude must be between -90 and 90");
        return ARGUS_ERROR_INVALID_RANGE;
    }
    
    if (coord->longitude < -180.0 || coord->longitude > 180.0) {
        free(coord);
        ARGUS_PARSING_ERROR(argus, "Longitude must be between -180 and 180");
        return ARGUS_ERROR_INVALID_RANGE;
    }

    return ARGUS_SUCCESS;
}
```

</TabItem>
<TabItem value="color-parser" label="Color Parser">

```c
typedef struct {
    unsigned char r, g, b;
} rgb_color_t;

int color_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    rgb_color_t *color = calloc(1, sizeof(rgb_color_t));
    if (!color) {
        ARGUS_PARSING_ERROR(argus, "Memory allocation failed");
        return ARGUS_ERROR_MEMORY;
    }

    option->value.as_ptr = color;
    option->is_allocated = true;

    // Handle hex format: #RRGGBB or #RGB
    if (arg[0] == '#') {
        arg++; // Skip #
        
        if (strlen(arg) == 6) {
            // #RRGGBB format
            unsigned int r, g, b;
            if (sscanf(arg, "%2x%2x%2x", &r, &g, &b) != 3) {
                free(color);
                ARGUS_PARSING_ERROR(argus, "Invalid hex color format");
                return ARGUS_ERROR_INVALID_FORMAT;
            }
            color->r = r; color->g = g; color->b = b;
        } else if (strlen(arg) == 3) {
            // #RGB format (expand to #RRGGBB)
            unsigned int r, g, b;
            if (sscanf(arg, "%1x%1x%1x", &r, &g, &b) != 3) {
                free(color);
                ARGUS_PARSING_ERROR(argus, "Invalid hex color format");
                return ARGUS_ERROR_INVALID_FORMAT;
            }
            color->r = r * 17; color->g = g * 17; color->b = b * 17;
        } else {
            free(color);
            ARGUS_PARSING_ERROR(argus, "Hex color must be #RGB or #RRGGBB");
            return ARGUS_ERROR_INVALID_FORMAT;
        }
    }
    // Handle rgb(r,g,b) format
    else if (strncmp(arg, "rgb(", 4) == 0) {
        int r, g, b;
        if (sscanf(arg, "rgb(%d,%d,%d)", &r, &g, &b) != 3) {
            free(color);
            ARGUS_PARSING_ERROR(argus, "Invalid rgb() format (expected rgb(r,g,b))");
            return ARGUS_ERROR_INVALID_FORMAT;
        }
        
        if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
            free(color);
            ARGUS_PARSING_ERROR(argus, "RGB values must be 0-255");
            return ARGUS_ERROR_INVALID_RANGE;
        }
        
        color->r = r; color->g = g; color->b = b;
    }
    else {
        free(color);
        ARGUS_PARSING_ERROR(argus, "Unsupported color format (use #RRGGBB or rgb(r,g,b))");
        return ARGUS_ERROR_INVALID_FORMAT;
    }

    return ARGUS_SUCCESS;
}
```

</TabItem>
</Tabs>

## // Integration with Validation

Combine custom handlers with validators:

```c
// Custom pre-validator for URL format
int url_format_validator(argus_t *argus, void *value_ptr, validator_data_t data)
{
    const char *url = (const char *)value_ptr;
    
    // Basic format check before parsing
    if (!strstr(url, "://") && !strchr(url, '/')) {
        ARGUS_PARSING_ERROR(argus, "URL must contain protocol or path");
        return ARGUS_ERROR_INVALID_FORMAT;
    }
    
    return ARGUS_SUCCESS;
}

#define V_URL_FORMAT() \
    MAKE_VALIDATOR(url_format_validator, _V_DATA_CUSTOM_(NULL), ORDER_PRE)

ARGUS_OPTIONS(
    options,
    // Handler with pre-validation
    OPTION_BASE('u', "url", VALUE_TYPE_CUSTOM,
                HANDLER(url_handler),
                FREE_HANDLER(url_free_handler),
                VALIDATOR(V_URL_FORMAT()),
                HELP("Target URL")),
)
```

## // Error Handling Best Practices

<Tabs>
<TabItem value="error-patterns" label="Error Patterns" default>

```c
int safe_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    // 1. Allocate main structure
    my_type_t *obj = calloc(1, sizeof(my_type_t));
    if (!obj) {
        ARGUS_PARSING_ERROR(argus, "Memory allocation failed");
        return ARGUS_ERROR_MEMORY;
    }
    
    // 2. Store immediately for automatic cleanup
    option->value.as_ptr = obj;
    option->is_allocated = true;
    
    // 3. Parse and validate - can exit immediately on error
    if (parse_step_1(obj, arg) != 0) {
        ARGUS_PARSING_ERROR(argus, "Invalid format");
        return ARGUS_ERROR_INVALID_FORMAT;
    }
    
    if (parse_step_2(obj, arg) != 0) {
        ARGUS_PARSING_ERROR(argus, "Invalid value");
        return ARGUS_ERROR_INVALID_VALUE;
    }
    
    return ARGUS_SUCCESS;
}
```

**Key points:**
- Validate input early
- Set `option->value.as_ptr` and `option->is_allocated = true` early
- Use specific error codes
- `ARGUS_PARSING_ERROR` reports the error, then return the error code

</TabItem>
<TabItem value="error-messages" label="Clear Error Messages">

```c
// ✅ Good error messages
ARGUS_PARSING_ERROR(argus, "Invalid endpoint format '%s' (expected host:port)", arg);

ARGUS_PARSING_ERROR(argus, "Port %ld out of range (must be 1-65535)", port);

// ❌ Poor error messages  
ARGUS_PARSING_ERROR(argus, "Bad input");
ARGUS_PARSING_ERROR(argus, "Parse error");
```

**Guidelines:**
- Include the invalid value in the message
- Explain expected format
- Be specific about constraints
- Help users fix the problem

</TabItem>
</Tabs>

## // Memory Management

<Tabs>
<TabItem value="allocation-patterns" label="Allocation Patterns" default>

```c
// Pattern 1: Single allocation
int simple_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    my_type_t *obj = malloc(sizeof(my_type_t));
    if (!obj) {
        ARGUS_PARSING_ERROR(argus, "Allocation failed");
        return ARGUS_ERROR_MEMORY;
    }
    
    // Store immediately for automatic cleanup
    option->value.as_ptr = obj;
    option->is_allocated = true;
    
    // Parse into obj...
    
    return ARGUS_SUCCESS;
}

// Pattern 2: Multiple allocations
int complex_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    complex_type_t *obj = calloc(1, sizeof(complex_type_t));
    if (!obj) {
        ARGUS_PARSING_ERROR(argus, "Allocation failed");
        return ARGUS_ERROR_MEMORY;
    }
    
    // Store immediately for automatic cleanup
    option->value.as_ptr = obj;
    option->is_allocated = true;
    
    // Additional allocations
    obj->data = malloc(data_size);
    if (!obj->data) {
        ARGUS_PARSING_ERROR(argus, "Data allocation failed");
        return ARGUS_ERROR_MEMORY;
    }
    
    obj->name = strdup(parsed_name);
    if (!obj->name) {
        ARGUS_PARSING_ERROR(argus, "Name allocation failed");
        return ARGUS_ERROR_MEMORY;
    }
    
    return ARGUS_SUCCESS;
}
```

</TabItem>
<TabItem value="free-patterns" label="Free Patterns">

```c
// Corresponding free handlers
int simple_free_handler(argus_option_t *option)
{
    free(option->value.as_ptr);
    return ARGUS_SUCCESS;
}

int complex_free_handler(argus_option_t *option)
{
    complex_type_t *obj = (complex_type_t*)option->value.as_ptr;
    if (obj) {
        free(obj->data);
        free(obj->name);
        free(obj);
    }
    return ARGUS_SUCCESS;
}
```

**Rules:**
- Always provide a free handler when `is_allocated = true`
- Free in reverse allocation order
- Check for NULL pointers
- Match allocation method (malloc/free, calloc/free)

</TabItem>
</Tabs>

## // What's Next?

- **[Custom Validators](custom-validators)** - Advanced validation techniques
- **[Collections](../features/collections)** - Arrays and maps with custom types
- **[Validation](../features/validation)** - Combining handlers with validation

:::tip Handler Design
Design handlers around user-friendly input formats, not internal data structures. Transform complex formats into simple, usable data.
:::
