/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

int flag_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    UNUSED(argus);
    UNUSED(arg);
    option->value.as_bool = true;
    return (ARGUS_SUCCESS);
}
