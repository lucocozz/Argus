#include "argus/errors.h"
#include "argus/internal/context.h"
#include "argus/internal/parsing.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

#include <stdio.h>

int handle_positional(argus_t *argus, argus_option_t *options, char *value, int position)
{
    argus_option_t *option = find_positional(options, position);
    if (option == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_ARGUMENT, "Unknown positional: '%s'", value);
    }
    context_set_option(argus, option);

    return (execute_callbacks(argus, option, value));
}
