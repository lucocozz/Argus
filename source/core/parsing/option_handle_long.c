/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#include <stdio.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/cross_platform.h"
#include "argus/internal/parsing.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

int handle_long_option(argus_t *argus, argus_option_t *options, char *arg, char **argv, int argc,
                       int *current_index)
{
    char  option_name[64] = {0};
    char *equal_pos       = strchr(arg, '=');
    if (equal_pos != NULL) {
        safe_strncpy(option_name, sizeof(option_name), arg, equal_pos - arg);
    } else
        safe_strncpy(option_name, sizeof(option_name), arg, sizeof(option_name) - 1);

    argus_option_t *option = find_option_by_lname(options, option_name);
    if (option == NULL) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_ARGUMENT, "Unknown option: '--%s'",
                            option_name);
        return ARGUS_ERROR_INVALID_ARGUMENT;
    }

    char *value = NULL;
    if (option->value_type != VALUE_TYPE_FLAG) {
        if (equal_pos != NULL) {  // Format "--option=value"
            value = equal_pos + 1;
        } else if (*current_index + 1 < argc) {  // Format ["--option", "value"]
            *current_index += 1;
            value = argv[*current_index];
        } else {
            ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_MISSING_VALUE,
                                "Missing value for option: '--%s'", option_name);
            return ARGUS_ERROR_MISSING_VALUE;
        }
    }

    return (execute_callbacks(argus, option, value));
}
