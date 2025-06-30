/**
 * Basic usage example for argus library
 * 
 * Demonstrates core features: flags, options with values, positional arguments,
 * and options with only short name or only long name
 */

#include "argus.h"
#include <stdio.h>
#include <stdlib.h>

// Define options
ARGUS_OPTIONS(
    options,
    // Standard options
    HELP_OPTION(),
    VERSION_OPTION(),

    // Flag option with both short and long names
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),

    // String option with only long name (no short name)
    OPTION_STRING(0, "output", HELP("Output file"), 
                 DEFAULT("output.txt"),
                 HINT("FILE")),

    // Integer option with only short name (no long name)
    OPTION_INT('p', NULL, HELP("Port number"), 
               DEFAULT(8080), 
               VALIDATOR(V_RANGE(1, 65535))),

    // Required positional argument
    POSITIONAL_STRING("input", HELP("Input file")),
    POSITIONAL_INT("value", HELP("Value to process"), FLAGS(FLAG_OPTIONAL)),
)

int main(int argc, char **argv)
{
    // Initialize argus
    argus_t argus = argus_init(options, "basic_example", "1.0.0");
    argus.description = "Basic example of argus library";

    // Parse command-line arguments
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;

    // Access parsed values
    bool verbose = argus_get(&argus, "verbose").as_bool;
    const char *output = argus_get(&argus, "output").as_string;
    int port = argus_get(&argus, "p").as_int;  // Using short name as ID when only short name exists
    const char *input = argus_get(&argus, "input").as_string;

    // Display configuration
    printf("Configuration:\n");
    printf("  Verbose: %s\n", verbose ? "enabled" : "disabled");
    printf("  Output: %s\n", output);
    printf("  Port: %d\n", port);
    printf("  Input: %s\n", input);
    if (argus_is_set(&argus, "value")) // Check if the optional positional argument was set
        printf("  Value: %d\n", argus_get(&argus, "value").as_int);

    // Free resources
    argus_free(&argus);
    return 0;
}
