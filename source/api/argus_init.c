#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/context.h"
#include "argus/types.h"

int validate_structure(argus_t *argus, argus_option_t *options);

argus_t argus_init_mode(argus_option_t *options, const char *program_name, const char *version,
                        bool release_mode)
{
    argus_t argus = {
        .program_name      = program_name,
        .version           = version,
        .description       = NULL,
        .env_prefix        = NULL,
        .options           = options,
        .error_stack.count = 0,
    };
    context_init(&argus);

    if (release_mode == false) {
        if (validate_structure(&argus, options) != ARGUS_SUCCESS) {
            fprintf(stderr, "Error while initializing argus:\n\n");
            argus_print_error_stack(&argus);
            exit(EXIT_FAILURE);
        }
        context_init(&argus);
    }

    return (argus);
}
