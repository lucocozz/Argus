# Basic Usage Example

This example shows how to use argus to create a simple application with different types of options.

## Complete Code

```c
/**
 * Basic usage example of the argus library
 * 
 * Demonstrates the main features: flags, options with values, positional arguments,
 * and options with only a short name or a long name
 */

#include "argus.h"
#include <stdio.h>
#include <stdlib.h>

// Option definition
ARGUS_OPTIONS(
    options,
    // Standard options
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Flag type option with short and long name
    OPTION_FLAG('v', "verbose", "Enable verbose output"),
    
    // String type option with only long name (no short name)
    OPTION_STRING(0, "output", "Output file", 
                 DEFAULT("output.txt"), 
                 HINT("FILE")),
    
    // Integer type option with only short name (no long name)
    OPTION_INT('p', NULL, "Port number", 
               DEFAULT(8080), 
               RANGE(1, 65535)),
               
    // Another option with only long name (using '\0' instead of 0)
    OPTION_FLAG('\0', "dry-run", "Run without applying changes"),
    
    // Required positional argument
    POSITIONAL_STRING("input", "Input file")
)

int main(int argc, char **argv)
{
    // Initialize argus
    argus_t argus = argus_init(options, "basic_example", "1.0.0");
    argus.description = "Basic example of the argus library";

    // Parse command line arguments
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }

    // Access parsed values
    bool verbose = argus_get(argus, "verbose").as_bool;
    const char* output = argus_get(argus, "output").as_string;
    int port = argus_get(argus, "p").as_int;  // Using short name as ID when only short name exists
    bool dry_run = argus_get(argus, "dry-run").as_bool;
    const char* input = argus_get(argus, "input").as_string;

    // Display the configuration
    printf("Configuration:\n");
    printf("  Verbose (-v, --verbose): %s\n", verbose ? "enabled" : "disabled");
    printf("  Output (--output only): %s\n", output);
    printf("  Port (-p only): %d\n", port);
    printf("  Dry run (--dry-run only): %s\n", dry_run ? "enabled" : "disabled");
    printf("  Input: %s\n", input);

    // Free resources
    argus_free(&argus);
    return 0;
}
```

## Compilation and Execution

```bash
gcc basic_usage.c -o basic_usage -largus
```

## Execution Examples

### Display Help

```
$ ./basic_usage --help
basic_example v1.0.0

Basic example of the argus library

Usage: basic_example [OPTIONS] <input>

Arguments:
  <input>                - Input file

Options:
  -h, --help             - Display this help message (exit)
  -V, --version          - Display version information (exit)
  -v, --verbose          - Enable verbose output
  --output <FILE>        - Output file (default: "output.txt")
  -p <NUM>               - Port number [1-65535] (default: 8080)
  --dry-run              - Run without applying changes
```

### Basic Usage

```
$ ./basic_usage input.txt
Configuration:
  Verbose (-v, --verbose): disabled
  Output (--output only): output.txt
  Port (-p only): 8080
  Dry run (--dry-run only): disabled
  Input: input.txt
```

### With Options

```
$ ./basic_usage -v --output=custom.txt -p 9000 --dry-run input.txt
Configuration:
  Verbose (-v, --verbose): enabled
  Output (--output only): custom.txt
  Port (-p only): 9000
  Dry run (--dry-run only): enabled
  Input: input.txt
```

## Important Points

### Option Types

This example illustrates several types of options:

- Boolean options (flags): `-v, --verbose` and `--dry-run`
- Options with values: `--output` (string) and `-p` (integer)
- Positional arguments: `input`

### Options with Short or Long Name Only

You can define options with:

- Only a short name (`-p`)
- Only a long name (`--output`, `--dry-run`)
- Both (`-v, --verbose`)

To define an option with only a long name, use `0` or `'\0'` as the short name.

### Default Values

Options can have default values specified with `DEFAULT()`:

```c
OPTION_STRING(0, "output", "Output file",
             DEFAULT("output.txt"),
             HINT("FILE"))
```

### Range Validation

Numeric options can have range validation:

```c
OPTION_INT('p', NULL, "Port number",
           DEFAULT(8080),
           RANGE(1, 65535))
```

### Usage Hint

The `HINT()` argument can be used to define a hint displayed in the help:

```c
OPTION_STRING(0, "output", "Output file",
             DEFAULT("output.txt"),
             HINT("FILE"))
```

### Accessing Values

Accessing option values depends on their name:

- For options with a long name, use the long name: `argus_get(argus, "verbose")`
- For options with only a short name, use the short name: `argus_get(argus, "p")`
- For positional arguments, use the defined name: `argus_get(argus, "input")`

### Freeing Resources

Don't forget to call `argus_free(&argus)` to free allocated resources.
