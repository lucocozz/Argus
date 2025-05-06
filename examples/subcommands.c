/**
 * Subcommands example for argus
 * 
 * Demonstrates how to implement command structures similar to git/docker
 */

#include "argus.h"
#include <stdio.h>
#include <stdlib.h>

// Subcommand action handlers
int add_command(argus_t *argus, void *data);
int remove_command(argus_t *argus, void *data);

// Define options for "add" subcommand
ARGUS_OPTIONS(
    add_options,
    HELP_OPTION(),
    OPTION_FLAG('f', "force", HELP("Force add operation")),
    POSITIONAL_STRING("file", HELP("File to add"))
)

// Define options for "remove" subcommand
ARGUS_OPTIONS(
    remove_options,
    HELP_OPTION(),
    OPTION_FLAG('r', "recursive", HELP("Recursively remove directories")),
    POSITIONAL_STRING("file", HELP("File to remove"))
)

// Define main options with subcommands
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
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
    bool verbose = argus_get(*argus, "verbose").as_bool;
    
    // Get command-specific options
    const char* file = argus_get(*argus, "add.file").as_string;
    bool force = argus_get(*argus, "add.force").as_bool;

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
    bool verbose = argus_get(*argus, "verbose").as_bool;
    
    // Get command-specific options
    const char* file = argus_get(*argus, "rm.file").as_string;
    bool recursive = argus_get(*argus, "rm.recursive").as_bool;

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
