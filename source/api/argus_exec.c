#include "argus/errors.h"
#include "argus/internal/context.h"
#include "argus/types.h"
#include <stddef.h>

bool argus_has_command(argus_t argus)
{
    return (argus.context.subcommand_depth > 0);
}

int argus_exec(argus_t *argus, void *data)
{
    const argus_option_t *command = context_get_subcommand(argus);
    if (command == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_NO_COMMAND, "Internal error: No command to execute");
    }
    if (command->action == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_HANDLER,
                           "Internal error: Command %s has no action handler", command->name);
    }
    return (command->action(argus, data));
}
