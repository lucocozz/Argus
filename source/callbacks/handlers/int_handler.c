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

#include <stdlib.h>

int int_handler(argus_t *argus, argus_option_t *option, char *value)
{
    UNUSED(argus);
    option->value = (argus_value_t){.as_int64 = strtoll(value, NULL, 10)};
    return (ARGUS_SUCCESS);
}
