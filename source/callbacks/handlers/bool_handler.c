/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#include <ctype.h>
#include <stddef.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

static int string_to_bool(const char *arg)
{
    char   value[6]       = {0};
    char  *false_values[] = {"0", "false", "no", "n", "off", "0x0", "0b0"};
    char  *true_values[]  = {"1", "true", "yes", "y", "on", "0x1", "0b1"};
    size_t arg_len        = strlen(arg);

    if (arg_len > sizeof(value) - 1)
        return -1;
    for (size_t i = 0; i < arg_len; ++i)
        value[i] = (char)tolower(arg[i]);
    value[arg_len] = '\0';

    for (size_t i = 0; i < ARRAY_SIZE(true_values); ++i) {
        if (strcmp(value, true_values[i]) == 0)
            return 1;
    }

    for (size_t i = 0; i < ARRAY_SIZE(false_values); ++i) {
        if (strcmp(value, false_values[i]) == 0)
            return 0;
    }

    return -1;  // Invalid boolean value
}

int bool_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    UNUSED(argus);
    if (arg == NULL) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for option: '%s'", option->name);
        return ARGUS_ERROR_INVALID_ARGUMENT;
    }

    int is_bool = string_to_bool(arg);
    if (is_bool == -1) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_ARGUMENT,
                            "Invalid boolean value: '%s'. Expected 'true' or 'false'", arg);
        return ARGUS_ERROR_INVALID_ARGUMENT;
    }
    option->value.as_bool = is_bool;
    return (ARGUS_SUCCESS);
}
