/**
 * help_display.c - Main help display coordination
 *
 * This file coordinates the help display functionality using modular components.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#define _GNU_SOURCE  // NOLINT

#include <stdbool.h>
#include <stdio.h>

#include "argus/internal/display.h"
#include "argus/internal/help_formatter.h"
#include "argus/internal/help_organizer.h"
#include "argus/internal/help_renderer.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

/*
 * Main help display function - now simplified and using modular components
 */

void display_help(argus_t *argus, const argus_option_t *command)
{
    if (command == NULL)
        command = get_active_options(argus);

    // Initialize help data
    help_data_t data = {NULL, NULL, NULL, NULL};

    // Organize options into appropriate categories using the organizer module
    organize_options(command, &data);

    // Print the help sections using the renderer module
    print_help_sections(argus, &data);

    // Clean up memory
    free_help_data(&data);

    printf("\n");
}
