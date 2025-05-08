#include "argus/errors.h"
#include "argus/types.h"
#include <stddef.h>
#include <stdio.h>

int execute_callbacks(argus_t *argus, argus_option_t *option, char *value)
{
    int status;

    if (option->handler == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_HANDLER, "Option %s has no handler",
                           option->name);
    }

    if (option->pre_validator != NULL) {
        status = option->pre_validator(argus, value, option->pre_validator_data);
        if (status != ARGUS_SUCCESS)
            return status;
    }

    status = option->handler(argus, option, value);
    if (status != ARGUS_SUCCESS)
        return (status);

    option->is_set = true;
    if (option->value_count == 0)
        option->value_count = 1;

    if (option->flags & FLAG_EXIT)
        return (ARGUS_SOULD_EXIT);
    return (ARGUS_SUCCESS);
}
