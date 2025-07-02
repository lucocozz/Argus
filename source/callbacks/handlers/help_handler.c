/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#include "argus/errors.h"
#include "argus/internal/display.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

int help_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    UNUSED(arg);
    UNUSED(option);

    const argus_option_t *command = get_active_options(argus);
    display_usage(argus, command);
    display_help(argus, command);

    return (ARGUS_SUCCESS);
}
