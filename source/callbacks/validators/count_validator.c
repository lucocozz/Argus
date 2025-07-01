/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

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
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_RANGE,
                            "Range is invalid %" PRId64 "-%" PRId64, data.range.min,
                            data.range.max);
        return ARGUS_ERROR_INVALID_RANGE;
    }

    int64_t count = option->value_count;

    if (count < data.range.min || count > data.range.max) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_RANGE,
                            "Values count %" PRId64 " is out of range %" PRId64 "-%" PRId64, count,
                            data.range.min, data.range.max);
        return ARGUS_ERROR_INVALID_RANGE;
    }
    return (ARGUS_SUCCESS);
}

char *format_count_validator(validator_data_t data)
{
    char *result = malloc(32);
    if (!result)
        return NULL;

    snprintf(result, 32, "%" PRId64 "-%" PRId64, data.range.min, data.range.max);
    return result;
}
