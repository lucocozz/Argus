#include "argus/errors.h"
#include "argus/types.h"

int range_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    if (data.range.min > data.range.max) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Range is invalid [%ld, %ld]",
                           data.range.min, data.range.max);
    }

    if (option->value.as_int < data.range.min || option->value.as_int > data.range.max) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Value %d is out of range [%ld, %ld]",
                           option->value.as_int, data.range.min, data.range.max);
    }
    return (ARGUS_SUCCESS);
}
