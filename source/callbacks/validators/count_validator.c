#include "argus/errors.h"
#include "argus/types.h"

int count_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    if (data.range.min < 0 || data.range.max < 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Range is negative");
    }
    if (data.range.min > data.range.max) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Range is invalid [%ld, %ld]",
                           data.range.min, data.range.max);
    }

    long long count = option->value_count;

    if (count < data.range.min || count > data.range.max) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Values count %d is out of [%ld, %ld]",
                           count, data.range.min, data.range.max);
    }
    return (ARGUS_SUCCESS);
}
