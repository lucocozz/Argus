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
int remove_all_command(argus_t *argus, void *data);

// Define options for "add" subcommand
ARGUS_OPTIONS(
    add_options,
    HELP_OPTION(),
    OPTION_FLAG('f', "force", HELP("Force add operation")),
    POSITIONAL_STRING("file", HELP("File to add")),
)

// Define options for "remove" subcommand
ARGUS_OPTIONS(
    remove_options,
    HELP_OPTION(),
    OPTION_FLAG('r', "recursive", HELP("Recursively remove directories")),
    POSITIONAL_STRING("file", HELP("File to remove")),
)

// Define options for "remove-all" subcommand
ARGUS_OPTIONS(
    remove_all_options,
    HELP_OPTION(),
    OPTION_FLAG('f', "force", HELP("Force remove all operation")),
    OPTION_FLAG('n', "dry-run", HELP("Show what would be removed without actually removing")),
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
    
    SUBCOMMAND("remove", remove_options, 
               HELP("Remove files from the index"), 
               ACTION(remove_command)),
    
    SUBCOMMAND("remove-all", remove_all_options, 
               HELP("Remove all files from the index"), 
               ACTION(remove_all_command)),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "subcommands_example", "1.0.0");
    argus.description = "Example of subcommands";

    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;

    if (argus_has_command(&argus))
        // Execute the subcommand handler
        status = argus_exec(&argus, NULL);
    else
        printf("No command specified. Use --help to see available commands.\n");

    argus_free(&argus);
    return 0;
}

// Implementation of the "add" command
int add_command(argus_t *argus, void *data)
{
    (void)data; // Unused parameter
    
    // Get the global option
    bool verbose = argus_get(argus, "verbose").as_bool;
    
    // Get command-specific options
    const char* file = argus_get(argus, "add.file").as_string;
    bool force = argus_get(argus, "add.force").as_bool;

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
    bool verbose = argus_get(argus, ".verbose").as_bool;
    
    // Get command-specific options
    const char* file = argus_get(argus, "file").as_string;
    bool recursive = argus_get(argus, "remove.recursive").as_bool;

    printf("Removing file: %s\n", file);
    if (verbose) printf("  verbose mode enabled\n");
    if (recursive) printf("  recursively\n");

    return 0;
}

// Implementation of the "remove-all" command
int remove_all_command(argus_t *argus, void *data)
{
    (void)data; // Unused parameter
    
    // Get the global option
    bool verbose = argus_get(argus, ".verbose").as_bool;
    
    // Get command-specific options
    bool force = argus_get(argus, "remove-all.force").as_bool;
    bool dry_run = argus_get(argus, "remove-all.dry-run").as_bool;

    if (dry_run) {
        printf("Would remove all files from the index\n");
    } else {
        printf("Removing all files from the index\n");
    }
    
    if (verbose) printf("  verbose mode enabled\n");
    if (force) printf("  with force option\n");
    if (dry_run) printf("  dry-run mode - no actual changes made\n");

    return 0;
}
