# > Help and Errors_

Learn how Argus automatically generates beautiful help text and handles errors gracefully to create a polished user experience.

## // Automatic Help Generation

Argus automatically generates comprehensive help text from your option definitions - no manual formatting required.

### Basic Help Setup

Simply include `HELP_OPTION()` in your options to get automatic `--help` and `-h` support:

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),                    // Automatic --help/-h
    VERSION_OPTION(),                 // Automatic --version/-V
    
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("result.txt")),
    OPTION_INT('p', "port", HELP("Port number"), DEFAULT(8080),
               VALIDATOR(V_RANGE(1, 65535))),
    
    POSITIONAL_STRING("input", HELP("Input file to process")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "my_tool", "1.0.0");
    argus.description = "A simple file processing tool";
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    // Your app logic here
    
    argus_free(&argus);
    return 0;
}
```

**Generated help output:**
```bash
$ ./my_tool --help
my_tool v1.0.0

A simple file processing tool

Usage: my_tool [OPTIONS] <input>

Arguments:
  <input>                - Input file to process

Options:
  -h, --help             - Display this help message (exit)
  -V, --version          - Display version information (exit)
  -v, --verbose          - Enable verbose output
  -o, --output <STR>     - Output file (default: "result.txt")
  -p, --port <1-65535>   - Port number (default: 8080)
```

### Help Components

Argus automatically includes several components in the generated help:

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
<TabItem value="header" label="Header Section" default>

```bash
my_tool v1.0.0                    # Program name + version

A simple file processing tool     # Description (optional)
```

**Set with:**
```c
argus_t argus = argus_init(options, "my_tool", "1.0.0");
argus.description = "A simple file processing tool";
```

</TabItem>
<TabItem value="usage" label="Usage Line">

```bash
Usage: my_tool [OPTIONS] <input>
```

**Automatically generated from:**
- Program name
- Optional arguments (shown as `[OPTIONS]`)
- Required positionals (shown as `<name>`)
- Optional positionals (shown as `[name]`)
- Subcommands (shown as `COMMAND`)

</TabItem>
<TabItem value="sections" label="Help Sections">

```bash
Arguments:                        # Positional arguments
  <input>                - Input file to process
  [output]               - Output file (optional)

Options:                          # Command-line options
  -h, --help             - Display this help message (exit)
  -v, --verbose          - Enable verbose output

Commands:                         # Subcommands (if any)
  add                    - Add files to the index
  remove                 - Remove files from the index
```

**Sections appear automatically** based on what you define.

</TabItem>
</Tabs>

### Option Display Format

Argus formats each option with detailed information:

```bash
  -o, --output <STR>     - Output file (default: "result.txt")
  ^^^^  ^^^^^^ ^^^^^       ^^^^^^^^^^^^ ^^^^^^^^^^^^^^^^^^^^
   |      |      |             |              |
  short  long   hint        help text      default value
```

**Components:**
- **Short name**: `-o` (if provided)
- **Long name**: `--output` (if provided) 
- **Value hint**: `<STR>`, `<NUM>`, custom hints
- **Help text**: From `HELP()` macro
- **Default value**: Shown automatically if `DEFAULT()` provided
- **Validation info**: Ranges, choices displayed automatically

### Advanced Help Features

<Tabs>
<TabItem value="validation" label="Validation Display" default>

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Range validation shown automatically
    OPTION_INT('p', "port", HELP("Port number"), 
               VALIDATOR(V_RANGE(1, 65535)), DEFAULT(8080)),
    
    // Choices shown automatically  
    OPTION_STRING('l', "level", HELP("Log level"),
                  VALIDATOR(V_CHOICE_STR("debug", "info", "warn", "error")),
                  DEFAULT("info")),
    
    // Length validation shown
    OPTION_STRING('u', "username", HELP("Username"),
                  VALIDATOR(V_LENGTH(3, 20))),
)
```

**Generated help:**
```bash
  -p, --port <1-65535>          - Port number (default: 8080)
  -l, --log-level <STR>         - Log level [debug|info|warn|error] (default:
                                  "info")
  -u, --username <STR>         - Username (3-20 chars)
```

</TabItem>
<TabItem value="hints" label="Custom Hints">

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Default hints based on type
    OPTION_STRING('f', "file", HELP("Input file")),        // <STR>
    OPTION_INT('c', "count", HELP("Item count")),          // <NUM>
    OPTION_FLOAT('r', "rate", HELP("Processing rate")),    // <FLOAT>
    
    // Custom hints for clarity
    OPTION_STRING('c', "config", HELP("Config file"), HINT("FILE")),
    OPTION_INT('s', "size", HELP("Buffer size"), HINT("BYTES")),
    OPTION_FLOAT('q', "quality", HELP("Quality"), HINT("0.0-1.0")),
)
```

**Generated help:**
```bash
  -f, --file <STR>             - Input file
  -c, --count <NUM>            - Item count
  -r, --rate <FLOAT>           - Processing rate
  -c, --config <FILE>          - Config file
  -s, --size <BYTES>           - Buffer size
  -q, --quality <0.0-1.0>      - Quality
```

</TabItem>
<TabItem value="groups" label="Option Groups">

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    GROUP_START("Connection"),
        OPTION_STRING('H', "host", HELP("Server hostname"), DEFAULT("localhost")),
        OPTION_INT('p', "port", HELP("Server port"), DEFAULT(8080)),
        OPTION_FLAG('s', "secure", HELP("Use HTTPS")),
    GROUP_END(),
    
    GROUP_START("Output"),
        OPTION_STRING('f', "format", HELP("Output format"), DEFAULT("json")),
        OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    GROUP_END(),
)
```

**Generated help:**
```bash
Network connection options:
  -H, --host <STR>       - Server hostname (default: "localhost")
  -p, --port <NUM>       - Server port (default: 8080)
  -s, --secure           - Use HTTPS

Output formatting options:
  -f, --format <STR>     - Output format (default: "json")
  -v, --verbose          - Verbose output
```

</TabItem>
</Tabs>

## // Subcommand Help

Argus automatically provides help for subcommands and nested command structures:

### Basic Subcommand Help

```c
int add_command(argus_t *argus, void *data)
{
    (void)data;
    const char *file = argus_get(argus, "file").as_string;
    bool force = argus_get(argus, "force").as_bool;
    
    printf("Adding file: %s\n", file);
    return 0;
}

ARGUS_OPTIONS(add_options,
    HELP_OPTION(),
    OPTION_FLAG('f', "force", HELP("Force add operation")),
    POSITIONAL_STRING("file", HELP("File to add")),
)

ARGUS_OPTIONS(options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    
    SUBCOMMAND("add", add_options, HELP("Add files to the index"), 
               ACTION(add_command)),
    SUBCOMMAND("remove", remove_options, HELP("Remove files from the index"),
               ACTION(remove_command)),
)
```

**Main help:**
```bash
$ ./tool --help
tool v1.0.0

Usage: tool [OPTIONS] COMMAND

Options:
  -h, --help             - Display this help message (exit)
  -V, --version          - Display version information (exit)  
  -v, --verbose          - Enable verbose output

Commands:
  add                    - Add files to the index
  remove                 - Remove files from the index

Run 'tool COMMAND --help' for more information on a command.
```

**Subcommand help:**
```bash
$ ./tool add --help
tool v1.0.0

Usage: tool add [OPTIONS] <file>

Add files to the index

Arguments:
  <file>                 - File to add

Options:
  -h, --help             - Display this help message (exit)
  -f, --force            - Force add operation
```

### Nested Subcommand Help

```c
ARGUS_OPTIONS(service_create_options,
    HELP_OPTION(),
    OPTION_STRING('n', "name", HELP("Service name")),
    OPTION_STRING('i', "image", HELP("Container image")),
)

ARGUS_OPTIONS(service_options,
    HELP_OPTION(),
    SUBCOMMAND("create", service_create_options, HELP("Create a service"),
               ACTION(service_create_action)),
    SUBCOMMAND("list", service_list_options, HELP("List services"),
               ACTION(service_list_action)),
)

ARGUS_OPTIONS(options,
    HELP_OPTION(),
    OPTION_FLAG('d', "debug", HELP("Debug mode")),
    SUBCOMMAND("service", service_options, HELP("Service management")),
)
```

**Nested help navigation:**
```bash
$ ./tool --help                    # Main help
$ ./tool service --help            # Service subcommand help  
$ ./tool service create --help     # Nested create help
```

## // Error Handling

Argus provides comprehensive error handling with helpful messages:

### Automatic Error Display

```c
int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "my_tool", "1.0.0");
    
    // Parse automatically handles and displays errors
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        // Error already printed, argus_free() is not needed, just exit
        return status;
    }
    
    // Continue with successful parsing
    argus_free(&argus);
    return 0;
}
```

### Common Error Types

<Tabs>
<TabItem value="missing-value" label="Missing Values" default>

```bash
$ ./tool --output
tool: Missing value for option: '--output'

$ ./tool -o
tool: Missing value for option: '-o'
```

**Triggered when:** Required option value not provided.

</TabItem>
<TabItem value="invalid-option" label="Unknown Options">

```bash
$ ./tool --unknown-option
tool: Unknown option: '--unknown-option'

$ ./tool -x
tool: Unknown option: '-x'
```

**Triggered when:** Option not defined in `ARGUS_OPTIONS`.

</TabItem>
<TabItem value="validation-errors" label="Validation Errors">

```bash
$ ./tool --port 99999
tool: Value 99999 is out of range [1, 65535]

$ ./tool --level invalid
tool: The 'level' option cannot be set to 'invalid'. Please choose from ["debug", "info", "warn", "error"]

$ ./tool --username ab
tool: Value 2 is out of length [3, 20]
```

**Triggered when:** Built-in validators fail.

</TabItem>
<TabItem value="missing-required" label="Missing Required">

```bash
$ ./tool
tool: Required positional argument missing: 'input'

$ ./tool --username alice
tool: Required option is missing: 'password' with option 'username'
```

**Triggered when:** Required arguments or dependencies not provided.

</TabItem>
<TabItem value="conflicts" label="Conflicting Options">

```bash
$ ./tool --verbose --quiet
tool: Conflict between 'verbose' and 'quiet'

$ ./tool --gzip --bzip2
tool: Exclusive options group 'Compression' conflict: 'gzip' and 'bzip2'
```

**Triggered when:** Conflicting or exclusive options used together.

</TabItem>
</Tabs>

### Error Suggestions

When parsing fails, Argus automatically suggests the help option:

```bash
$ ./tool --invalid-option value
tool: Unknown option: '--invalid-option'

Try 'tool --help' for more information.

$ ./tool service --bad-option  
tool: Unknown option: '--bad-option'

Try 'tool service --help' for more information.
```

## // Manual Help and Version Display

You can also display help programmatically:

```c
int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "my_tool", "1.0.0");
    
    // Display help manually
    if (argc == 1) {
        argus_print_help(&argus);
        argus_free(&argus);
        return 0;
    }
    
    // Display version manually
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        argus_print_version(&argus);
        argus_free(&argus);
        return 0;
    }
    
    // Normal parsing
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS) {
        argus_free(&argus);
        return 1;
    }
    
    argus_free(&argus);
    return 0;
}
```

### Manual Display Functions

| Function | Purpose | Output |
|----------|---------|--------|
| `argus_print_help()` | Show full help | Complete help text |
| `argus_print_usage()` | Show usage only | Just the usage line |
| `argus_print_version()` | Show version | Program name and version |

## // Help Customization

### Program Information

```c
int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "my_awesome_tool", "2.1.0");
    
    // Optional description (appears after version)
    argus.description = "Process files with advanced algorithms and modern techniques";
    
    // Parse and continue...
}
```

### Option Descriptions

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Clear, actionable help text
    OPTION_FLAG('v', "verbose", 
        HELP("Enable detailed output including processing steps and timing information")),
    
    // Mention important defaults or behaviors
    OPTION_STRING('c', "config", 
        HELP("Configuration file path (searches current directory if not absolute)")),
    
    // Explain validation constraints
    OPTION_INT('t', "threads", 
        HELP("Number of worker threads (limited by available CPU cores)"),
        VALIDATOR(V_RANGE(1, 16))),
)
```

### Group Organization

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    
    // Input/Output options
    GROUP_START("Input/Output"),
        OPTION_STRING('i', "input", HELP("Input file or directory")),
        OPTION_STRING('o', "output", HELP("Output destination")),
        OPTION_STRING('f', "format", HELP("Output format")),
    GROUP_END(),
    
    // Processing options  
    GROUP_START("Processing"),
        OPTION_INT('t', "threads", HELP("Worker threads")),
        OPTION_FLOAT('q', "quality", HELP("Processing quality")),
        OPTION_FLAG('F', "fast", HELP("Use fast mode (lower quality)")),
    GROUP_END(),
)
```

## // Best Practices

### ✅ Good Help Text

```c
// Clear, specific descriptions
OPTION_STRING('c', "config", HELP("Configuration file path"))

// Mention important behaviors
OPTION_FLAG('f', "force", HELP("Overwrite existing files without prompting"))

// Explain what the option does
OPTION_INT('t', "timeout", HELP("Connection timeout in seconds"))

// Use consistent terminology
OPTION_FLAG('v', "verbose", HELP("Enable verbose output"))
OPTION_FLAG('q', "quiet", HELP("Suppress all output"))
```

### ❌ Avoid These Patterns

```c
// ❌ Too vague
OPTION_STRING('f', "file", HELP("File"))

// ❌ Just repeating the option name
OPTION_FLAG('v', "verbose", HELP("Verbose"))

// ❌ No context about what it affects
OPTION_INT('n', "number", HELP("A number"))

// ❌ Missing important details
OPTION_FLAG('d', "delete", HELP("Delete")) // Delete what? Permanent?
```

### Help Text Guidelines

1. **Be specific**: Explain what the option does, not just what it is
2. **Mention side effects**: If an option changes default behavior significantly
3. **Include constraints**: Mention file format requirements, valid ranges, etc.
4. **Use consistent style**: Same terminology and formatting across all options
5. **Keep it concise**: One line that clearly explains the purpose

## // Integration with Error Handling

Argus integrates error handling with help display for a smooth user experience:

```c
// This pattern is handled automatically by argus_parse()
if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS) {
    // Error already displayed with suggestion to use --help
    // No need for additional error handling
    return 1;
}

// For custom validation in your app:
if (some_custom_validation_fails) {
    fprintf(stderr, "%s: Custom validation failed\n", argus.program_name);
    fprintf(stderr, "Try '%s --help' for more information.\n", argus.program_name);
    return 1;
}
```

## // What's Next?

Now that you understand help and error handling, dive into advanced features:

- **[Validation](../features/validation)** - Built-in and custom validation
- **[Subcommands](../features/subcommands)** - Multi-command applications
- **[Environment Variables](../features/environment)** - Loading options from env vars
- **[Collections](../features/collections)** - Working with arrays and maps

:::tip Pro Tip
Good help text is just as important as good code. Users will see your help text before they see your application, so make it clear, helpful, and professional!
:::
