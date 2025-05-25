#include "argus/errors.h"
#include "argus/types.h"

#include <string.h>

int length_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;

    if (option->value.as_string == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "Value is NULL");
    }
    if (data.range.min < 0 || data.range.max < 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Range is negative");
    }
    if (data.range.min > data.range.max) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Range is invalid [%ld, %ld]",
                           data.range.min, data.range.max);
    }

    long long len = strlen(option->value.as_string);

    if (len < data.range.min || len > data.range.max) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Value %d is out of length [%ld, %ld]",
                           len, data.range.min, data.range.max);
    }
    return (ARGUS_SUCCESS);
}
