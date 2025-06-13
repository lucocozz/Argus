#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/display.h"
#include "argus/types.h"

int validate_structure(argus_t *argus, argus_option_t *options);

argus_t _argus_init_validate(argus_option_t *options, const char *program_name, const char *version,
                             bool validate)
{
    argus_t argus = {
        .program_name = program_name,
        .version      = version,
        .description  = NULL,
        .env_prefix   = NULL,
        .helper       = {.config = get_default_helper_config()},
        .options      = options,
    };

    if (validate) {
        if (validate_structure(&argus, options) != ARGUS_SUCCESS)
            exit(EXIT_FAILURE);
    }

    return (argus);
}
