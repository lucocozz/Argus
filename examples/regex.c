/**
 * Example demonstrating regex validation for argus
 *
 * Shows a mix of predefined patterns and custom patterns
 */

#include "argus.h"
#include "argus/regex.h"  /* Import predefined patterns */
#include <stdio.h>
#include <stdlib.h>


/* Custom regex patterns */
#define RE_SIMPLE_NAME MAKE_REGEX("^[a-zA-Z][a-zA-Z0-9_-]{2,29}$", "Letters, numbers, underscore, dash")

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),

    // Custom patterns defined in this file
    OPTION_STRING('u', "username", HELP("Username (3-20 chars)"),
        REGEX(RE_SIMPLE_NAME)),
    
    // Using predefined patterns from header
    POSITIONAL_STRING("ip", HELP("IPv4 address"),
        REGEX(ARGUS_RE_IPV4)),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "regex_example", "1.0.0");
    argus.description = "Example of using regex validation";

    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;

    if (argus_is_set(argus, "username"))
        printf("Username: %s\n", argus_get(argus, "username").as_string);
    printf("  IP Address: %s\n", argus_get(argus, "ip").as_string);

    argus_free(&argus);
    return 0;
}
