/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#include <stdint.h>
#define _GNU_SOURCE  // NOLINT

#include <stdlib.h>

#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/options.h"
#include "argus/types.h"

static int set_int_value(argus_option_t *option, char *value)
{
    adjust_array_size(option);
    if (option->value_capacity == 0)
        return ARGUS_ERROR_MEMORY;

    char     *endptr;
    long long parsed_value = strtoll(value, &endptr, 10);

    if (*endptr != '\0' || endptr == value)
        return ARGUS_ERROR_INVALID_VALUE;

    option->value.as_array[option->value_count].as_int64 = parsed_value;
    option->value_count++;
    return ARGUS_SUCCESS;
}

int variadic_int_handler(argus_t *argus, argus_option_t *option, char *value)
{
    int status = set_int_value(option, value);
    if (status != ARGUS_SUCCESS) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                            "Invalid integer value '%s' for positional argument '%s'", value,
                            option->name);
        return status;
    }

    apply_array_flags(option);
    option->is_allocated = true;
    return ARGUS_SUCCESS;
}
