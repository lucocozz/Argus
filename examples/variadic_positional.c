/**
 * Multi-value positional arguments example for argus
 * 
 * Demonstrates how to use positional arguments that can accept
 * multiple values, similar to how many command-line tools work (e.g., cp file1 file2 file3 dest)
 */

#include "argus.h"
#include <stdio.h>
#include <stdlib.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),

    POSITIONAL_STRING(
        "command", HELP("Operation to perform"), HINT("CMD"),
        VALIDATOR(V_CHOICE_STR("build", "copy", "delete"))),

    POSITIONAL_MANY_STRING(
        "files", HELP("Input files to process"), HINT("FILE..."),
        FLAGS(FLAG_UNIQUE | FLAG_SORTED)),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "many_args_example", "1.0.0");
    argus.description = "Example of multi-value positional arguments";
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;

    const char *command = argus_get(&argus, "command").as_string;
    
    printf("Configuration:\n");
    printf("  Command: %s\n", command);

    if (argus_is_set(&argus, "files"))
    {
        printf("  Files:\n");

        argus_array_it_t it = argus_array_it(&argus, "files");   
        while (argus_array_next(&it))
            printf("    - %s\n", it.value.as_string);
    }
    else
        printf("  Files: (none)\n");
    
    argus_free(&argus);
    return 0;
}
