#include "argus/errors.h"
#include "argus/types.h"

int count_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;

    if (data.range.min < 0 || data.range.max < 0) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Range is negative");
        return ARGUS_ERROR_INVALID_RANGE;
    }
    if (data.range.min > data.range.max) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_RANGE, "Range is invalid [%ld, %ld]",
                            data.range.min, data.range.max);
        return ARGUS_ERROR_INVALID_RANGE;
    }

    long long count = option->value_count;

    if (count < data.range.min || count > data.range.max) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_RANGE,
                            "Values count %lld is out of [%ld, %ld]", count, data.range.min,
                            data.range.max);
        return ARGUS_ERROR_INVALID_RANGE;
    }
    return (ARGUS_SUCCESS);
}
