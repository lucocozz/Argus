/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#define _GNU_SOURCE  // NOLINT

#include <stdlib.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/cross_platform.h"
#include "argus/internal/utils.h"
#include "argus/options.h"
#include "argus/types.h"

static int set_string_value(argus_option_t *option, char *value)
{
    adjust_array_size(option);
    if (option->value_capacity == 0)
        return ARGUS_ERROR_MEMORY;

    char *dup_value = safe_strdup(value);
    if (dup_value == NULL)
        return ARGUS_ERROR_MEMORY;

    option->value.as_array[option->value_count].as_string = dup_value;
    option->value_count++;
    return ARGUS_SUCCESS;
}

int variadic_string_handler(argus_t *argus, argus_option_t *option, char *value)
{
    UNUSED(argus);

    int status = set_string_value(option, value);
    if (status != ARGUS_SUCCESS)
        return status;

    apply_array_flags(option);
    option->is_allocated = true;
    return ARGUS_SUCCESS;
}

int free_variadic_string_handler(argus_option_t *option)
{
    for (size_t i = 0; i < option->value_count; ++i)
        free(option->value.as_array[i].as_string);
    free(option->value.as_array);
    return ARGUS_SUCCESS;
}
