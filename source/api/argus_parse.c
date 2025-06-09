#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/display.h"
#include "argus/internal/parsing.h"
#include "argus/types.h"

void argus_free(argus_t *argus);

int argus_parse(argus_t *argus, int argc, char **argv)
{
    int status = parse_args(argus, argus->options, argc - 1, &argv[1]);
    argus->errno = status;
    if (status == ARGUS_SHOULD_EXIT) {
        argus_free(argus);
        exit(ARGUS_SUCCESS);
    }
    if (status != ARGUS_SUCCESS) {
        printf("\n");
        display_usage(argus, NULL);
        printf("\nTry '%s", argus->program_name);
        for (size_t i = 0; i < argus->subcommand_depth; ++i)
            printf(" %s", argus->subcommand_stack[i]->name);
        printf(" --help' for more information.\n");
        argus_free(argus);
        return (status);
    }

    status = load_env_vars(argus);
    if (status != ARGUS_SUCCESS) {
        argus_free(argus);
        return (status);
    }

    status = post_parse_validation(argus);
    if (status != ARGUS_SUCCESS)
        argus_free(argus);
    return (status);
}
