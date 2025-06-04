#include "argus/errors.h"
#include "argus/internal/context.h"
#include "argus/types.h"
#include <stddef.h>

int validate_subcommand(argus_t *argus, argus_option_t *option)
{
    int status = ARGUS_SUCCESS;

    if (option->name == NULL) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_MALFORMED_OPTION, "Subcommand must have a name");
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }

    if (option->help == NULL) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_MALFORMED_OPTION,
                            "Subcommand '%s' must have a help message", option->name);
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }

    if (option->sub_options == NULL) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_MALFORMED_OPTION,
                            "Subcommand '%s' must have options", option->name);
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }

    if (option->flags & ~SUBCOMMAND_FLAG_MASK) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_INVALID_FLAG, "Invalid flags for subcommand '%s'",
                            option->name);
        status = ARGUS_ERROR_INVALID_FLAG;
    }

    // if (option->choices_count > 0) {
    //     ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_INVALID_CHOICE,
    //                         "Subcommand '%s' cannot have choices", option->name);
    //     status = ARGUS_ERROR_INVALID_CHOICE;
    // }

    return (status);
}

int validate_group(argus_t *argus, argus_option_t *option)
{
    int status = ARGUS_SUCCESS;

    context_set_group(argus, option);
    if (option->flags & ~GROUP_FLAG_MASK) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_INVALID_GROUP, "Invalid flags for group '%s'",
                            option->name);
        status = ARGUS_ERROR_INVALID_GROUP;
    }

    return (status);
}
