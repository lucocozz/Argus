# Subcommands

argus supports Git/Docker-style subcommands, allowing you to create command-line interfaces with distinct commands.

!!! abstract "Overview"
    Subcommands let you organize your program's functionality into separate commands, each with its own options and behavior. Examples include:
    
    - `git commit`, `git push`, `git pull`
    - `docker run`, `docker build`, `docker pull`
    
    This guide covers the basics of implementing subcommands in argus. For advanced usage including nested subcommands, see the [Nested Commands](../advanced/nested-commands.md) guide.

## Basic Concept

Subcommands transform your program from a simple utility into a multi-purpose tool with specialized commands:

```
my_program [global options] command [command options] [arguments]
```

Each command can have its own set of options and behavior while sharing common global options.

## Implementing Subcommands

To implement subcommands in argus, you need to:

1. Define options for each subcommand
2. Define actions for each subcommand
3. Reference the subcommands in your main options

### 1. Define Subcommand Options

First, define the options for each of your subcommands:

```c
// Options for the "add" subcommand
ARGUS_OPTIONS(
    add_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('f', "force", HELP("Force add operation")),
    POSITIONAL_STRING("file", HELP("File to add"))
)

// Options for the "remove" subcommand
ARGUS_OPTIONS(
    remove_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('r', "recursive", HELP("Recursively remove directories")),
    POSITIONAL_STRING("file", HELP("File to remove"))
)
```

### 2. Define Action Functions

Next, define action functions that will be called when each subcommand is executed:

```c
// Action for the "add" subcommand
int add_command(argus_t *argus, void *data)
{
    // Access subcommand options
    const char* file = argus_get(*argus, "add.file").as_string;
    bool force = argus_get(*argus, "add.force").as_bool;
    
    printf("Adding file: %s\n", file);
    if (force) printf("  with force option\n");
    
    return 0;
}

// Action for the "remove" subcommand
int remove_command(argus_t *argus, void *data)
{
    // Access subcommand options
    const char* file = argus_get(*argus, "remove.file").as_string;
    bool recursive = argus_get(*argus, "remove.recursive").as_bool;
    
    printf("Removing file: %s\n", file);
    if (recursive) printf("  recursively\n");
    
    return 0;
}
```

### 3. Define Main Options with Subcommands

Finally, define your main options and include the subcommands:

```c
// Main options with subcommands
ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Global option applicable to all subcommands
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    
    // Define subcommands
    SUBCOMMAND("add", add_options, 
               HELP("Add files to the index"), 
               ACTION(add_command)),
    
    SUBCOMMAND("rm", remove_options, 
               HELP("Remove files from the index"), 
               ACTION(remove_command))
)
```

## Processing Subcommands

In your main function, you need to check if a subcommand was specified and execute its action:

```c
int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "subcommands_example", "1.0.0");
    argus.description = "Example of subcommands";

    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }

    // Check if a subcommand was specified
    if (argus_has_command(argus)) {
        // Execute the subcommand's action
        status = argus_exec(&argus, NULL);
    } else {
        printf("No command specified. Use --help to see available commands.\n");
    }

    argus_free(&argus);
    return status;
}
```

## Accessing Options

### Accessing Subcommand Options

Within a subcommand action function, you can access options in two ways:

=== "Using Absolute Path"
    ```c
    // Access with full path 
    const char* file = argus_get(*argus, "add.file").as_string;
    bool force = argus_get(*argus, "add.force").as_bool;
    ```

=== "Using Relative Path"
    ```c
    // Within the add_command function, you can use relative paths
    const char* file = argus_get(*argus, "file").as_string;
    bool force = argus_get(*argus, "force").as_bool;
    ```

### Accessing Global Options

Global options are accessible from within subcommand actions:

```c
// Access global options
bool verbose = argus_get(*argus, ".verbose").as_bool;
```

The leading dot (`.`) indicates the option is defined at the root level.

## Help Integration

argus automatically integrates subcommands into the help display:

```
subcommands_example v1.0.0

Example of subcommands

Usage: subcommands_example [OPTIONS] COMMAND

Options:
  -h, --help             - Display this help message (exit)
  -V, --version          - Display version information (exit)
  -v, --verbose          - Enable verbose output

Commands:
  add                    - Add files to the index
  rm                     - Remove files from the index

Run 'subcommands_example COMMAND --help' for more information on a command.
```

Each subcommand also has its own help:

```
subcommands_example v1.0.0

Usage: subcommands_example add [OPTIONS] <file>

Add files to the index

Arguments:
  <file>                 - File to add

Options:
  -h, --help             - Display this help message (exit)
  -f, --force            - Force add operation
```

## Complete Example

Here's a complete example of implementing basic subcommands:

```c
#include "argus.h"
#include <stdio.h>
#include <stdlib.h>

// Subcommand action handlers
int add_command(argus_t *argus, void *data);
int remove_command(argus_t *argus, void *data);

// Define options for "add" subcommand
ARGUS_OPTIONS(
    add_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('f', "force", HELP("Force add operation")),
    POSITIONAL_STRING("file", HELP("File to add"))
)

// Define options for "remove" subcommand
ARGUS_OPTIONS(
    remove_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('r', "recursive", HELP("Recursively remove directories")),
    POSITIONAL_STRING("file", HELP("File to remove"))
)

// Define main options with subcommands
ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Global option applicable to all subcommands
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    
    // Define subcommands
    SUBCOMMAND("add", add_options, 
               HELP("Add files to the index"), 
               ACTION(add_command)),
    
    SUBCOMMAND("rm", remove_options, 
               HELP("Remove files from the index"), 
               ACTION(remove_command))
)

// Implementation of the "add" command
int add_command(argus_t *argus, void *data)
{
    (void)data; // Unused parameter
    
    // Get the global option
    bool verbose = argus_get(*argus, ".verbose").as_bool;
    
    // Get command-specific options
    const char* file = argus_get(*argus, "file").as_string;
    bool force = argus_get(*argus, "force").as_bool;

    printf("Adding file: %s\n", file);
    if (verbose) printf("  verbose mode enabled\n");
    if (force) printf("  with force option\n");

    return 0;
}

// Implementation of the "remove" command
int remove_command(argus_t *argus, void *data)
{
    (void)data; // Unused parameter
    
    // Get the global option
    bool verbose = argus_get(*argus, ".verbose").as_bool;
    
    // Get command-specific options
    const char* file = argus_get(*argus, "file").as_string;
    bool recursive = argus_get(*argus, "recursive").as_bool;

    printf("Removing file: %s\n", file);
    if (verbose) printf("  verbose mode enabled\n");
    if (recursive) printf("  recursively\n");

    return 0;
}

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "subcommands_example", "1.0.0");
    argus.description = "Example of subcommands";

    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }

    if (argus_has_command(argus)) {
        // Execute the subcommand handler
        status = argus_exec(&argus, NULL);
    } else {
        printf("No command specified. Use --help to see available commands.\n");
    }

    argus_free(&argus);
    return 0;
}
```

## Next Steps

For more advanced subcommand features, such as:

- Nested subcommands (commands within commands)
- Command name abbreviations
- Alternative positional argument placements
- Custom command handling

See the [Nested Commands](../advanced/nested-commands.md) guide.
