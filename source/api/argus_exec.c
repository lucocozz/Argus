#include "argus/errors.h"
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
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_NO_COMMAND, "Internal error: No command to execute");
        return ARGUS_ERROR_NO_COMMAND;
    }
    if (command->action == NULL) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_HANDLER,
                            "The %s command cannot be executed by itself", command->name);
        return ARGUS_ERROR_INVALID_HANDLER;
    }
    return (command->action(argus, data));
}
