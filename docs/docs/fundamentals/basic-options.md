# Basic Options

Learn how to define different types of command-line options in Argus using declarative macros.

## Option Types Overview

Argus supports several fundamental types of command-line elements:

| Type | Purpose | Definition Macro | Example Usage |
|------|---------|------------------|---------------|
| **Flag** | Boolean toggle | `OPTION_FLAG()` | `--verbose`, `-v` |
| **String** | Text values | `OPTION_STRING()` | `--output file.txt` |
| **Integer** | Numeric values | `OPTION_INT()` | `--port 8080` |
| **Float** | Decimal values | `OPTION_FLOAT()` | `--rate 0.5` |
| **Boolean** | Explicit true/false | `OPTION_BOOL()` | `--debug=true` |
| **Positional** | Ordered arguments | `POSITIONAL_*()` | `input.txt output.txt` |

## Flag Options

**Flags** are boolean options activated by their presence - no value required.

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),                    // Standard help option
    
    // Simple flag - presence = true, absence = false
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    
    // Flag with only short name
    OPTION_FLAG('q', NULL, HELP("Quiet mode")),
    
    // Flag with only long name  
    OPTION_FLAG('\0', "dry-run", HELP("Show what would be done")),
    
    // For "disable" semantics, use negative naming
    OPTION_FLAG('s', "silent", HELP("Disable all output")),
)
```

:::info Flag Behavior
- **Present**: `./program --verbose` → `true`
- **Absent**: `./program` → `false` 
- **No value needed**: Flags never take arguments
- **Always false by default**: Use negative naming for "disable" semantics
:::

:::warning Flag Defaults
Flags cannot have `DEFAULT(true)` - this would make the flag useless since presence would always result in `true`. For "disable" behavior, use negative naming:

```c
// ❌ Don't do this
OPTION_FLAG('v', "verbose", DEFAULT(true))  // Validation error

// ✅ Instead, use negative naming  
OPTION_FLAG('q', "quiet", HELP("Disable verbose output"))

// ✅ Or use OPTION_BOOL for explicit control
OPTION_BOOL('v', "verbose", DEFAULT(true))  // Allows --verbose=false
```
:::

## String Options

**String options** accept text values for file paths, names, formats, etc.

```c
ARGUS_OPTIONS(
    options,
    // Basic string option
    OPTION_STRING('o', "output", HELP("Output file name")),
    
    // String with default value
    OPTION_STRING('f', "format", HELP("Output format"), DEFAULT("json")),
    
    // String with hint for help display
    OPTION_STRING('c', "config", HELP("Configuration file"), 
                 HINT("FILE")),
    
    // Long name only
    OPTION_STRING('\0', "template", HELP("Template file"), 
                 DEFAULT("default.tmpl"), HINT("TEMPLATE")),
)
```

## Integer Options

**Integer options** handle numeric values with automatic validation.

```c
ARGUS_OPTIONS(
    options,
    // Basic integer
    OPTION_INT('p', "port", HELP("Port number")),
    
    // Integer with default
    OPTION_INT('c', "count", HELP("Number of items"), DEFAULT(10)),
    
    // Integer with validation range  
    OPTION_INT('t', "threads", HELP("Worker threads"), 
              DEFAULT(1), VALIDATOR(V_RANGE(1, 16))),
    
    // Integer with custom hint
    OPTION_INT('s', "size", HELP("Buffer size"), 
              HINT("BYTES"), DEFAULT(1024)),
)
```

## Float Options

**Float options** handle decimal numbers for rates, percentages, etc.

```c
ARGUS_OPTIONS(
    options,
    // Basic float
    OPTION_FLOAT('r', "rate", HELP("Processing rate")),
    
    // Float with default
    OPTION_FLOAT('s', "scale", HELP("Scale factor"), DEFAULT(1.0)),
    
    // Float with validation and hint
    OPTION_FLOAT('q', "quality", HELP("Compression quality"), 
                DEFAULT(0.8), VALIDATOR(V_RANGE(0, 1)), HINT("0.0-1.0")),
)
```

## Boolean Options

**Boolean options** require explicit true/false values (unlike flags).

```c
ARGUS_OPTIONS(
    options,
    // Basic boolean (explicit value required)
    OPTION_BOOL('d', "debug", HELP("Enable debug mode")),
    
    // Boolean with default
    OPTION_BOOL('a', "auto-save", HELP("Enable auto-save"), DEFAULT(true)),
    
    // Boolean that accepts various formats
    OPTION_BOOL('e', "encryption", HELP("Enable encryption"), 
               DEFAULT(false)),
)
```

**Accepted boolean values:**
- **True**: `true`, `yes`, `1`, `on`, `y` (case-insensitive)
- **False**: `false`, `no`, `0`, `off`, `n` (case-insensitive)

:::info Boolean vs Flag
- **Flag**: `--verbose` (presence = true, absence = false, no value)
- **Boolean**: `--debug=true` (explicit value always required)

**When to use each:**
- **Flag**: Simple on/off toggles (`--verbose`, `--force`, `--quiet`)
- **Boolean**: When you need explicit control or defaults of true (`--auto-save=false`)
:::

## Positional Arguments

**Positional arguments** are provided without option names, processed in order.

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
<TabItem value="required" label="Required" default>

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Required positionals (must be provided)
    POSITIONAL_STRING("input", HELP("Input file to process")),
    POSITIONAL_STRING("output", HELP("Output file destination")),
    POSITIONAL_INT("mode", HELP("Processing mode (1-3)")),
)
```

**Usage:** `./program input.txt output.txt 2`

</TabItem>
<TabItem value="optional" label="Optional">

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Required first
    POSITIONAL_STRING("input", HELP("Input file")),
    
    // Optional positionals with defaults
    POSITIONAL_STRING("output", HELP("Output file"), 
                     FLAGS(FLAG_OPTIONAL), DEFAULT("output.txt")),
    POSITIONAL_INT("buffer_size", HELP("Buffer size in KB"), 
                  FLAGS(FLAG_OPTIONAL), DEFAULT(64)),
)
```

**Usage:** `./program input.txt [output.txt] [1024]`

</TabItem>
<TabItem value="types" label="All Types">

```c
// Positional arguments support all data types
POSITIONAL_STRING("file", HELP("File path"))
POSITIONAL_INT("count", HELP("Item count"))  
POSITIONAL_FLOAT("rate", HELP("Processing rate"))
POSITIONAL_BOOL("enabled", HELP("Feature enabled"))
```

</TabItem>
</Tabs>

:::warning Ordering Rule
Required positional arguments must be defined **before** optional ones:

```c
// ✅ Correct order
POSITIONAL_STRING("input", HELP("Required input")),           // Required
POSITIONAL_STRING("output", HELP("Required output")),         // Required  
POSITIONAL_INT("size", HELP("Optional size"), FLAGS(FLAG_OPTIONAL)), // Optional

// ❌ Wrong order - validation error at startup
POSITIONAL_INT("size", HELP("Optional size"), FLAGS(FLAG_OPTIONAL)), // Optional first
POSITIONAL_STRING("input", HELP("Required input")),           // Required after
```
:::

## Option Modifiers

Enhance your options with these common modifiers:

### Default Values

```c
ARGUS_OPTIONS(
    options,
    // Provide sensible defaults
    OPTION_STRING('f', "format", HELP("Output format"), DEFAULT("json")),
    OPTION_INT('p', "port", HELP("Port number"), DEFAULT(8080)),
    OPTION_FLOAT('r', "ratio", HELP("Compression ratio"), DEFAULT(0.8)),
    OPTION_BOOL('a', "auto", HELP("Auto mode"), DEFAULT(true)),
)
```

### Help Text and Hints

```c
ARGUS_OPTIONS(
    options,
    // Clear, descriptive help text
    OPTION_STRING('o', "output", HELP("Output directory for processed files")),
    
    // Custom hints for value display in help
    OPTION_STRING('c', "config", HELP("Configuration file"), HINT("FILE")),
    OPTION_INT('s', "size", HELP("Buffer size"), HINT("BYTES")),
    OPTION_FLOAT('r', "rate", HELP("Processing rate"), HINT("0.0-1.0")),
    
    // Hints appear in help: -c, --config <FILE>
)
```

### Basic Validation

```c
ARGUS_OPTIONS(
    options,
    // Range validation for numbers
    OPTION_INT('p', "port", HELP("Port number"), 
              VALIDATOR(V_RANGE(1, 65535))),
    
    // String length validation
    OPTION_STRING('u', "username", HELP("Username"), 
                 VALIDATOR(V_LENGTH(3, 20))),
    
    // Multiple choice validation
    OPTION_STRING('l', "level", HELP("Log level"), 
                 VALIDATOR(V_CHOICE_STR("debug", "info", "warn", "error"))),
)
```

## Option Naming Strategies

### Both Short and Long Names

```c
OPTION_STRING('o', "output", HELP("Output file"))
// Access via: argus_get(argus, "output")
// Usage: -o file.txt OR --output=file.txt
```

### Long Name Only

```c
OPTION_FLAG('\0', "dry-run", HELP("Show what would be done"))
// Access via: argus_get(argus, "dry-run")  
// Usage: --dry-run (no short form)
```

### Short Name Only

```c
OPTION_INT('p', NULL, HELP("Port number"))
// Access via: argus_get(argus, "p")  -- uses short name as identifier
// Usage: -p 8080 (no long form)
```

:::tip Naming Conventions
- **Short names**: Single character, common options (`-v`, `-o`, `-p`, `-h`)
- **Long names**: Descriptive, kebab-case for multi-word (`--output-dir`, `--max-count`)
- **Avoid conflicts**: Don't reuse common short names (`-h` = help, `-v` = verbose/version)
:::

## Complete Definition Example

Here's a comprehensive example showing all option types together:

```c title="comprehensive_tool.c"
#include <argus.h>

ARGUS_OPTIONS(
    options,
    // Standard options (auto-generated help/version)
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Flags (boolean toggles - always false by default)
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    OPTION_FLAG('f', "force", HELP("Force overwrite existing files")),
    OPTION_FLAG('\0', "dry-run", HELP("Show what would be done")),
    
    // String options
    OPTION_STRING('o', "output", HELP("Output directory"), 
                 DEFAULT("./output"), HINT("DIR")),
    OPTION_STRING('\0', "format", HELP("Output format"), 
                 DEFAULT("json"), VALIDATOR(V_CHOICE_STR("json", "xml", "csv"))),

    // Numeric options  
    OPTION_INT('t', "threads", HELP("Number of worker threads"), 
              DEFAULT(1), VALIDATOR(V_RANGE(1, 16))),
    OPTION_FLOAT('q', "quality", HELP("Compression quality"), 
                DEFAULT(0.8), VALIDATOR(V_RANGE(0.0, 1.0)), HINT("0.0-1.0")),
    
    // Boolean option (explicit value)
    OPTION_BOOL('b', "backup", HELP("Create backup before processing"), 
               DEFAULT(false)),
    
    // Positional arguments (required first, then optional)
    POSITIONAL_STRING("input", HELP("Input file to process")),
    POSITIONAL_INT("iterations", HELP("Number of processing iterations"), 
                  FLAGS(FLAG_OPTIONAL), DEFAULT(1)),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "comprehensive_tool", "1.0.0");
    argus.description = "A comprehensive example showing all option types";
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    // Options are now parsed and ready to use
    // See "Accessing Values" guide for retrieval details
    
    printf("Tool configured successfully!\n");
    
    argus_free(&argus);
    return 0;
}
```

This example generates help like:
```bash
$ ./comprehensive_tool --help
comprehensive_tool v1.0.0

A comprehensive example showing all option types

Usage: comprehensive_tool [OPTIONS] <input> [iterations]

Arguments:
  <input>                     - Input file to process
  [iterations]                - Number of processing iterations (default: 1)

Options:
  -h, --help                  - Display this help message (exit)
  -V, --version               - Display version information (exit)
  -v, --verbose               - Enable verbose output
  -f, --force                 - Force overwrite existing files
      --dry-run               - Show what would be done
  -o, --output <DIR>          - Output directory (default: "./output")
      --format <json|xml|csv> - Output format (default: "json")
  -t, --threads <1-16>        - Number of worker threads (default: 1)
  -q, --quality <0.0-1.0>     - Compression quality (default: 0.80)
  -b, --backup <BOOL>         - Create backup before processing (default: false)
```

## What's Next?

Now that you know how to define options, learn about:

- **[Option Formats](option-formats)** - All the ways users can specify your options
- **[Accessing Values](accessing-values)** - How to retrieve option values in your code  
- **[Validation](../features/validation)** - Ensure users provide correct input
- **[Environment Variables](../features/environment)** - Load options from environment

:::tip Pro Tip
Always provide `HELP_OPTION()` in your definitions. Argus auto-generates beautiful help text from your option definitions, saving you from manual help formatting!
:::
