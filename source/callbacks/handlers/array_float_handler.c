/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#include <stdlib.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/options.h"
#include "argus/types.h"

static void set_value(argus_option_t *option, char *value)
{
    adjust_array_size(option);
    option->value.as_array[option->value_count].as_float = strtof(value, NULL);
    option->value_count++;
}

int array_float_handler(argus_t *argus, argus_option_t *option, char *value)
{
    UNUSED(argus);

    if (strchr(value, ',') != NULL) {
        char **splited_values = split(value, ",");
        if (splited_values == NULL) {
            ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_MEMORY, "Failed to split string '%s'", value);
            return ARGUS_ERROR_MEMORY;
        }
        for (size_t i = 0; splited_values[i] != NULL; ++i)
            set_value(option, splited_values[i]);
        free_split(splited_values);
    } else
        set_value(option, value);

    apply_array_flags(option);
    option->is_allocated = true;
    return (ARGUS_SUCCESS);
}

int free_array_float_handler(argus_option_t *option)
{
    free(option->value.as_array);
    return (ARGUS_SUCCESS);
}
