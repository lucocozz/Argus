/**
 * Positional arguments example for argus
 * 
 * Demonstrates how to use required and optional positional arguments
 */

#include "argus.h"
#include <stdio.h>
#include <stdlib.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),

    // Optional flag
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),

    // Required positional arguments must come before optional ones
    POSITIONAL_STRING("source", HELP("Source file"), 
                    HINT("SRC")),
    
    POSITIONAL_STRING("destination", HELP("Destination file"), 
                    HINT("DEST")),
    
    // Optional positional argument with default value
    POSITIONAL_INT("buffer_size", HELP("Buffer size in KB"),
                    FLAGS(FLAG_OPTIONAL),
                    DEFAULT(64),
                    RANGE(1, 8192)),
    
    // Optional positional argument without default value
    POSITIONAL_STRING("log_file", HELP("Log file"),
                    FLAGS(FLAG_OPTIONAL),
                    HINT("LOG")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "positional_example", "1.0.0");
    argus.description = "Example of positional arguments";
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;

    // Access required positional arguments
    const char *source = argus_get(argus, "source").as_string;
    const char *destination = argus_get(argus, "destination").as_string;
    
    // Access optional positional arguments
    int buffer_size = argus_get(argus, "buffer_size").as_int;
    
    // Optional argument may not be set
    const char *log_file = argus_is_set(argus, "log_file") ? 
                          argus_get(argus, "log_file").as_string : 
                          "(none)";
    
    printf("Configuration:\n");
    printf("  Source: %s\n", source);
    printf("  Destination: %s\n", destination);
    printf("  Buffer size: %d KB\n", buffer_size);
    printf("  Log file: %s\n", log_file);
    
    argus_free(&argus);
    return 0;
}
