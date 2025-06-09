#include "argus/errors.h"
#include "argus/types.h"
#include <stddef.h>

int validate_subcommand(argus_option_t *option)
{
    int status = ARGUS_SUCCESS;

    if (option->name == NULL) {
        ARGUS_STRUCT_ERROR(option, "Subcommand must have a name");
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }

    if (option->help == NULL) {
        ARGUS_STRUCT_ERROR(option, "Subcommand must have a help message");
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }

    if (option->sub_options == NULL) {
        ARGUS_STRUCT_ERROR(option, "Subcommand must have options");
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }

    if (option->flags & ~SUBCOMMAND_FLAG_MASK) {
        ARGUS_STRUCT_ERROR(option, "Invalid flags for subcommand");
        status = ARGUS_ERROR_INVALID_FLAG;
    }

    if (option->handler != NULL) {
        ARGUS_STRUCT_ERROR(option, "Subcommand cannot have a handler");
        status = ARGUS_ERROR_INVALID_HANDLER;
    }

    if (option->require != NULL || option->conflict != NULL) {
        ARGUS_STRUCT_ERROR(option, "Subcommand cannot have dependencies");
        status = ARGUS_ERROR_INVALID_DEPENDENCY;
    }

    if (option->have_default) {
        ARGUS_STRUCT_ERROR(option, "Subcommand cannot have a default value");
        status = ARGUS_ERROR_INVALID_DEFAULT;
    }

    if (option->validators != NULL) {
        ARGUS_STRUCT_ERROR(option, "Subcommand cannot have validators");
        status = ARGUS_ERROR_INVALID_FLAG;
    }

    return (status);
}

int validate_group(argus_option_t *option)
{
    int status = ARGUS_SUCCESS;

    if (option->flags & ~GROUP_FLAG_MASK) {
        ARGUS_STRUCT_ERROR(option, "Group have Invalid flags");
        status = ARGUS_ERROR_INVALID_GROUP;
    }

    return (status);
}
