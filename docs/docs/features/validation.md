# Validation

Ensure your users provide correct input with Argus's comprehensive validation system, from simple built-in validators to powerful regex patterns.

## Built-in Validators

Argus provides ready-to-use validators for common validation scenarios. Add them to your option definitions using the `VALIDATOR()` macro.

### Range Validation

Validate that numeric values fall within acceptable bounds:

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
<TabItem value="range-basic" label="Basic Range" default>

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Integer range validation
    OPTION_INT('p', "port", HELP("Port number"),
               VALIDATOR(V_RANGE(1, 65535)), DEFAULT(8080)),
    
    // Float range validation  
    OPTION_FLOAT('q', "quality", HELP("Compression quality"),
                 VALIDATOR(V_RANGE(0, 1)), DEFAULT(0.8)),
    
    // Negative ranges work too
    OPTION_INT('t', "temperature", HELP("Temperature in Celsius"),
               VALIDATOR(V_RANGE(-50, 100))),
)
```

**Usage:**
```bash
$ ./tool --port 8080        # ✅ Valid
$ ./tool --port 99999       # ❌ Error: Value 99999 is out of range [1, 65535]
$ ./tool --quality 0.9      # ✅ Valid  
$ ./tool --quality 1.5      # ❌ Error: Value 1.5 is out of range [0, 1]
```

</TabItem>
<TabItem value="range-help" label="Help Display">

Range validation automatically appears in help text:

```bash
$ ./tool --help
Options:
  -p, --port <NUM>       - Port number [1-65535] (default: 8080)
  -q, --quality <FLOAT>  - Compression quality [0-1] (default: 0.80)
  -t, --temperature <NUM> - Temperature in Celsius [-50-100]
```

The range `[min-max]` is automatically added to the help display.

</TabItem>
</Tabs>

### Length Validation

Validate string length for usernames, passwords, descriptions, etc:

<Tabs>
<TabItem value="length-basic" label="String Length" default>

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Username length validation
    OPTION_STRING('u', "username", HELP("Username"),
                  VALIDATOR(V_LENGTH(3, 20))),
    
    // Password minimum length
    OPTION_STRING('p', "password", HELP("Password"),  
                  VALIDATOR(V_LENGTH(8, 128))),
    
    // Description with reasonable limits
    OPTION_STRING('d', "description", HELP("Project description"),
                  VALIDATOR(V_LENGTH(10, 500))),
)
```

**Usage:**
```bash
$ ./tool --username "bob"          # ✅ Valid (3 chars)
$ ./tool --username "jo"           # ❌ Error: Value 2 is out of length [3, 20]
$ ./tool --password "secret123"    # ✅ Valid (9 chars)
$ ./tool --password "1234"         # ❌ Error: Value 4 is out of length [8, 128]
```

</TabItem>
<TabItem value="length-help" label="Help Display">

Length constraints appear in help:

```bash
$ ./tool --help
Options:
  -u, --username <STR>   - Username [3-20 chars]
  -p, --password <STR>   - Password [8-128 chars]
  -d, --description <STR> - Project description [10-500 chars]
```

</TabItem>
</Tabs>

### Count Validation

Validate the number of elements in arrays and maps:

<Tabs>
<TabItem value="count-arrays" label="Array Count" default>

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Require at least one tag, max 5
    OPTION_ARRAY_STRING('t', "tags", HELP("Resource tags"),
                        VALIDATOR(V_COUNT(1, 5))),
    
    // Allow 1-10 port numbers
    OPTION_ARRAY_INT('p', "ports", HELP("Port numbers"),
                     VALIDATOR(V_COUNT(1, 10))),
    
    // Optional tags with upper limit
    OPTION_ARRAY_STRING('l', "labels", HELP("Optional labels"),
                        VALIDATOR(V_COUNT(0, 3))),
)
```

**Usage:**
```bash
$ ./tool --tags=web,api,backend     # ✅ Valid (3 tags)
$ ./tool --tags=a,b,c,d,e,f        # ❌ Error: Values count 6 is out of [1, 5]
$ ./tool                           # ❌ Error: Values count 0 is out of [1, 5] (for tags)
```

</TabItem>
<TabItem value="count-maps" label="Map Count">

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Environment variables with limits
    OPTION_MAP_STRING('e', "env", HELP("Environment variables"),
                      VALIDATOR(V_COUNT(1, 10))),
    
    // Feature flags (optional)
    OPTION_MAP_BOOL('f', "features", HELP("Feature toggles"),
                    VALIDATOR(V_COUNT(0, 5))),
)
```

**Usage:**
```bash
$ ./tool --env=USER=alice,HOME=/home/alice    # ✅ Valid (2 pairs)
$ ./tool --features=debug=true               # ✅ Valid (1 pair)
```

</TabItem>
</Tabs>

### Choice Validation

Restrict values to a predefined set of valid options:

<Tabs>
<TabItem value="choices-string" label="String Choices" default>

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Log level with predefined choices
    OPTION_STRING('l', "level", HELP("Log level"),
                  VALIDATOR(V_CHOICES_STRING("debug", "info", "warn", "error")),
                  DEFAULT("info")),
    
    // Output format choices
    OPTION_STRING('f', "format", HELP("Output format"),
                  VALIDATOR(V_CHOICES_STRING("json", "xml", "yaml", "csv")),
                  DEFAULT("json")),
    
    // Compression algorithm
    OPTION_STRING('c', "compression", HELP("Compression type"),
                  VALIDATOR(V_CHOICES_STRING("gzip", "bzip2", "lzma"))),
)
```

**Usage:**
```bash
$ ./tool --level info             # ✅ Valid
$ ./tool --level invalid          # ❌ Error: Cannot be set to 'invalid'. Choose from ["debug", "info", "warn", "error"]
$ ./tool --format json            # ✅ Valid
$ ./tool --format txt             # ❌ Error: Invalid choice
```

</TabItem>
<TabItem value="choices-numbers" label="Numeric Choices">

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Predefined integer values
    OPTION_INT('m', "mode", HELP("Operating mode"),
               VALIDATOR(V_CHOICES_INT(1, 2, 3, 5, 8)),
               DEFAULT(1)),
    
    // Quality presets
    OPTION_FLOAT('q', "quality", HELP("Quality preset"),
                 VALIDATOR(V_CHOICES_FLOAT(0.1, 0.5, 0.8, 1.0)),
                 DEFAULT(0.8)),
)
```

**Usage:**
```bash
$ ./tool --mode 2                 # ✅ Valid
$ ./tool --mode 4                 # ❌ Error: Invalid choice (4 not in list)
$ ./tool --quality 0.5            # ✅ Valid
$ ./tool --quality 0.7            # ❌ Error: Invalid choice
```

</TabItem>
<TabItem value="choices-help" label="Help Display">

Choices automatically appear in help:

```bash
$ ./tool --help
Options:
  -l, --level <STR>      - Log level [debug|info|warn|error] (default: "info")
  -f, --format <STR>     - Output format [json|xml|yaml|csv] (default: "json")
  -m, --mode <NUM>       - Operating mode [1|2|3|5|8] (default: 1)
  -q, --quality <FLOAT>  - Quality preset [0.1|0.5|0.8|1.0] (default: 0.80)
```

</TabItem>
</Tabs>

## Regex Validation

Use powerful regular expressions for pattern-based validation:

### Predefined Patterns

Argus includes common regex patterns ready to use:

<Tabs>
<TabItem value="regex-predefined" label="Built-in Patterns" default>

```c
#include "argus/regex.h"

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Email validation
    OPTION_STRING('e', "email", HELP("Email address"),
                  VALIDATOR(V_REGEX(ARGUS_RE_EMAIL))),
    
    // IPv4 address validation
    OPTION_STRING('i', "ip", HELP("Server IP address"),
                  VALIDATOR(V_REGEX(ARGUS_RE_IPV4))),
    
    // URL validation
    OPTION_STRING('u', "url", HELP("Website URL"),
                  VALIDATOR(V_REGEX(ARGUS_RE_URL))),
    
    // ISO date format (YYYY-MM-DD)
    OPTION_STRING('d', "date", HELP("Date in ISO format"),
                  VALIDATOR(V_REGEX(ARGUS_RE_ISO_DATE))),
)
```

**Common predefined patterns:**
- `ARGUS_RE_EMAIL` - Email addresses
- `ARGUS_RE_IPV4` - IPv4 addresses  
- `ARGUS_RE_IPV6` - IPv6 addresses
- `ARGUS_RE_URL` - URLs (any protocol)
- `ARGUS_RE_ISO_DATE` - ISO dates (YYYY-MM-DD)
- `ARGUS_RE_PHONE_US` - US phone numbers
- `ARGUS_RE_UUID` - UUID v4 format

</TabItem>
<TabItem value="regex-custom" label="Custom Patterns">

Define custom regex patterns before using them:

```c
// Define patterns at top of file - avoid inline patterns
#define RE_PRODUCT_ID MAKE_REGEX("^[A-Z]{2}\\d{4}-[A-Z0-9]{6}$", "Format: XX0000-XXXXXX")
#define RE_USERNAME MAKE_REGEX("^[a-zA-Z][a-zA-Z0-9_-]{2,19}$", "3-20 chars, start with letter")
#define RE_SEMVER MAKE_REGEX("^\\d+\\.\\d+\\.\\d+$", "Format: X.Y.Z")

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Use predefined custom patterns
    OPTION_STRING('p', "product", HELP("Product ID"),
                  VALIDATOR(V_REGEX(RE_PRODUCT_ID))),
    
    OPTION_STRING('u', "username", HELP("Username"),
                  VALIDATOR(V_REGEX(RE_USERNAME))),
    
    OPTION_STRING('v', "version", HELP("Semantic version"),
                  VALIDATOR(V_REGEX(RE_SEMVER))),
)
```

**Usage:**
```bash
$ ./tool --product "AB1234-XYZ789"    # ✅ Valid
$ ./tool --product "invalid"          # ❌ Error: Invalid value 'invalid': Format: XX0000-XXXXXX
$ ./tool --version "1.2.3"            # ✅ Valid
$ ./tool --version "1.2"              # ❌ Error: Invalid value '1.2': Format: X.Y.Z
```

</TabItem>
</Tabs>

:::note Regex Dependency
Regex validation requires PCRE2. If compiled without regex support (`-Dregex=false`), regex validators will fail with an error message.
:::

## Combining Validators

Apply multiple validators to a single option. All validators are specified within a single `VALIDATOR()` call:

<Tabs>
<TabItem value="multiple-basic" label="Multiple Validators" default>

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Combine range and choices (value must be in range AND in choices)
    OPTION_INT('p', "port", HELP("Common HTTP ports"),
               VALIDATOR(V_RANGE(1, 65535), V_CHOICES_INT(80, 443, 8080, 8443))),

    // Combine length and regex validation
    OPTION_STRING('u', "username", HELP("Valid username"),
                  VALIDATOR(V_LENGTH(3, 20), V_REGEX(RE_USERNAME))),
    
    // Count validation for arrays
    OPTION_ARRAY_STRING('e', "emails", HELP("Email addresses"),
                        VALIDATOR(V_COUNT(1, 5))),
)
```

**Important:** Only **one** `VALIDATOR()` call per option. Include all validators as comma-separated arguments.

</TabItem>
<TabItem value="validation-order" label="Validation Order">

Validators execute in this order:

1. **Pre-validators** (`ORDER_PRE`) - Check raw string format
2. **Type conversion** - Parse string to target type
3. **Post-validators** (`ORDER_POST`) - Validate converted value
4. **Built-in validations** - Choices, etc.

```c
// Example showing validation execution order
#define RE_PASSWORD MAKE_REGEX("^[A-Za-z0-9@$!%*?&]{8,}$", "8+ chars with special chars")

OPTION_STRING('p', "password", HELP("Secure password"),
              // 1. Pre-validation: Check raw string format
              // 2. Type conversion: string -> string (no conversion)
              // 3. Post-validation: Check string length
              VALIDATOR(V_REGEX(RE_PASSWORD), V_LENGTH(8, 128))),
```

If any validator fails, parsing stops and an error is displayed.

</TabItem>
</Tabs>

## Validation Order and Custom Validators

Argus supports two types of validation timing:

- **`ORDER_PRE`**: Validates raw string before type conversion
- **`ORDER_POST`**: Validates converted value after type conversion

For detailed information on creating custom validators with specific ordering and complex validation logic, see the [Custom Validators](../advanced/custom-validators) guide.

## Best Practices

### ✅ Good Practices

```c
// 1. Define regex patterns before use
#define RE_EMAIL_DOMAIN MAKE_REGEX("^[\\w.-]+@company\\.com$", "Must use company.com domain")

// 2. Use appropriate validators for data types
OPTION_INT('p', "port", HELP("Port number"),
           VALIDATOR(V_RANGE(1, 65535)), DEFAULT(8080))

// 3. Combine validators logically
OPTION_STRING('u', "username", HELP("Username"),
              VALIDATOR(V_LENGTH(3, 20), V_REGEX(RE_USERNAME)))

// 4. Provide sensible defaults for choices
OPTION_STRING('l', "level", HELP("Log level"),
              VALIDATOR(V_CHOICES_STRING("debug", "info", "warn", "error")),
              DEFAULT("info"))
```

### ❌ Avoid These Patterns

```c
// ❌ Don't use inline regex patterns - hard to read and reuse
OPTION_STRING('e', "email", HELP("Email"),
              VALIDATOR(V_REGEX(MAKE_REGEX("^[\\w.-]+@[\\w.-]+$", "Email format"))))

// ❌ Don't over-validate simple inputs
OPTION_STRING('n', "name", HELP("Your name"),
              VALIDATOR(V_LENGTH(1, 50), V_REGEX(MAKE_REGEX("^[A-Za-z ]+$", "Letters only"))))

// ❌ Don't use multiple VALIDATOR() calls
OPTION_INT('p', "port", HELP("Port"),
           VALIDATOR(V_RANGE(1, 65535)),
           VALIDATOR(V_CHOICES(80, 443, 8080))) // ❌ Wrong - multiple calls
```

## Complete Example

```c
#include "argus.h"
#include "argus/regex.h"

// Define regex patterns
#define RE_USERNAME MAKE_REGEX("^[a-zA-Z][a-zA-Z0-9_]{2,19}$", "Start with letter, 3-20 chars")

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Range validation
    OPTION_INT('p', "port", HELP("Server port"),
               VALIDATOR(V_RANGE(1, 65535)), DEFAULT(8080)),
    
    // Length validation
    OPTION_STRING('u', "username", HELP("Username"),
                  VALIDATOR(V_LENGTH(3, 20), V_REGEX(RE_USERNAME))),
    
    // Choice validation
    OPTION_STRING('l', "level", HELP("Log level"),
                  VALIDATOR(V_CHOICES_STRING("debug", "info", "warn", "error"))),
                  DEFAULT("info")),
    
    // Regex validation with predefined pattern
    OPTION_STRING('e', "email", HELP("Email address"),
                  VALIDATOR(V_REGEX(ARGUS_RE_EMAIL))),
    
    // Count validation for arrays
    OPTION_ARRAY_STRING('t', "tags", HELP("Resource tags"),
                        VALIDATOR(V_COUNT(1, 5))),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "validation_example", "1.0.0");
    argus.description = "Validation example";
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    printf("✅ All validation passed!\n");
    
    argus_free(&argus);
    return 0;
}
```

## What's Next?

- **[Environment Variables](environment)** - Load options from environment
- **[Subcommands](subcommands)** - Build Git-like command hierarchies  
- **[Collections](collections)** - Work with arrays and maps
- **[Custom Validators](../advanced/custom-validators)** - Advanced custom validation techniques

:::tip Performance Note
Validation happens during parsing. For most applications, validation overhead is negligible, but complex regex patterns or custom validators can affect startup time.
:::
