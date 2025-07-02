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

int version_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    UNUSED(arg);
    UNUSED(option);
    display_version(argus);

    return (ARGUS_SUCCESS);
}
