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

/**
 * argus_print_help - Print help message for command-line options
 *
 * param argus  Argus context
 */
void argus_print_help(argus_t *argus)
{
    display_help(argus, NULL);
}

/**
 * argus_print_usage - Print short usage information
 *
 * param argus  Argus context
 */
void argus_print_usage(argus_t *argus)
{
    display_usage(argus, NULL);
}

/**
 * argus_print_version - Print version information
 *
 * param argus  Argus context
 */
void argus_print_version(argus_t *argus)
{
    display_version(argus);
}
