/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#define _GNU_SOURCE  // NOLINT

#include <stdbool.h>
#include <stdio.h>

#include "argus/internal/display.h"
#include "argus/internal/help.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

/*
 * Main help display function - now simplified and using modular components
 */

void display_help(argus_t *argus, const argus_option_t *command)
{
    help_data_t data = {0};

    if (command == NULL)
        command = get_active_options(argus);

    organize_options(command, &data);
    print_help_sections(argus, &data);
    free_help_data(&data);
    printf("\n");
}
