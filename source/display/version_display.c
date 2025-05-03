/**
 * version_display.c - Functions for displaying version information
 *
 * This file implements the version display functionality for the argus library.
 *
 * MIT License - Copyright (c) 2024 lucocozz
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
