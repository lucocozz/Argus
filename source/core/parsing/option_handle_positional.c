#include "argus/errors.h"
#include "argus/internal/parsing.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

#include <stdio.h>

int handle_positional(argus_t *argus, argus_option_t *options, char *value, int position)
{
    argus_option_t *option = find_positional(options, position);
    if (option == NULL) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_ARGUMENT, "Unknown positional: '%s'", value);
        return ARGUS_ERROR_INVALID_ARGUMENT;
    }

    return (execute_callbacks(argus, option, value));
}
