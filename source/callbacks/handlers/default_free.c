/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#include <stdlib.h>

#include "argus/errors.h"
#include "argus/types.h"

int default_free(argus_option_t *option)
{
    free(option->value.as_ptr);
    return (ARGUS_SUCCESS);
}
