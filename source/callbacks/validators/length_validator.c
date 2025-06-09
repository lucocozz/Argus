#include "argus/errors.h"
#include "argus/types.h"

#include <string.h>

int length_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;

    if (option->value.as_string == NULL) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "Value is NULL");
        return ARGUS_ERROR_INVALID_VALUE;
    }
    if (data.range.min < 0 || data.range.max < 0) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Range is negative");
        return ARGUS_ERROR_INVALID_RANGE;
    }
    if (data.range.min > data.range.max) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Range is invalid [%ld, %ld]", data.range.min, data.range.max);
        return ARGUS_ERROR_INVALID_RANGE;
    }

    long long len = strlen(option->value.as_string);

    if (len < data.range.min || len > data.range.max) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Value %lld is out of length [%ld, %ld]",
                           len, data.range.min, data.range.max);
        return ARGUS_ERROR_INVALID_RANGE;
    }
    return (ARGUS_SUCCESS);
}
