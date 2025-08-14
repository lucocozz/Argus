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
#include "argus/types.h"
#include <stddef.h>

bool argus_has_command(argus_t *argus)
{
    return (argus->subcommand_depth > 0);
}

int argus_exec(argus_t *argus, void *data)
{
    const argus_option_t *command = NULL;
    if (argus->subcommand_depth > 0)
        command = argus->subcommand_stack[argus->subcommand_depth - 1];

    if (command == NULL) {
        display_all_commands(argus);
        return ARGUS_ERROR_NO_COMMAND;
    }
    if (command->action == NULL) {
        display_available_subcommands(argus, command);
        return ARGUS_ERROR_INVALID_HANDLER;
    }
    return (command->action(argus, data));
}
