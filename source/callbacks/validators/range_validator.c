#include <stdio.h>
#include <stdlib.h>

#include "argus/errors.h"
#include "argus/types.h"

int range_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;

    if (data.range.min > data.range.max) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Range is invalid %lld-%lld",
                            data.range.min, data.range.max);
        return ARGUS_ERROR_INVALID_RANGE;
    }

    if (option->value.as_int < data.range.min || option->value.as_int > data.range.max) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Value %d is out of range %lld-%lld",
                            option->value.as_int, data.range.min, data.range.max);
        return ARGUS_ERROR_INVALID_RANGE;
    }
    return (ARGUS_SUCCESS);
}

char *format_range_validator(validator_data_t data)
{
    char *result = malloc(32);
    if (!result)
        return NULL;

    snprintf(result, 32, "%lld-%lld", data.range.min, data.range.max);
    return result;
}
