/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#include "argus/internal/display.h"
#include "argus/types.h"
#include <stdio.h>

void display_version(argus_t *argus)
{
    printf("%s", argus->program_name);

    if (argus->version)
        printf(" v%s", argus->version);

    printf("\n");

    if (argus->description)
        printf("\n%s\n", argus->description);
}
