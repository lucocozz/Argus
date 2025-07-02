/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#include "argus/internal/display.h"
#include "argus/internal/utils.h"
#include "argus/types.h"
#include <stdio.h>

static bool have_subcommand(const argus_option_t *options)
{
    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        if (options[i].type == TYPE_SUBCOMMAND)
            return (true);
    }
    return (false);
}

void display_usage(argus_t *argus, const argus_option_t *command)
{
    UNUSED(command);
    const argus_option_t *options = get_active_options(argus);

    printf("Usage: %s", argus->program_name);

    // Add subcommand chain if any
    for (size_t i = 0; i < argus->subcommand_depth; ++i)
        printf(" %s", argus->subcommand_stack[i]->name);

    printf(" [OPTIONS]");

    // Add positional arguments to usage line
    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        if (options[i].type == TYPE_POSITIONAL) {
            if (options[i].flags & FLAG_REQUIRED) {
                printf(" <%s>", options[i].name);
            } else {
                printf(" [%s]", options[i].name);
            }
        }
    }

    // Add COMMAND placeholder if we have subcommands
    if (have_subcommand(options))
        printf(" COMMAND");

    printf("\n");
}
