#include "argus/errors.h"
#include "argus/internal/context.h"
#include "argus/types.h"
#include <stddef.h>
#include <string.h>

int validate_subcommand(argus_t *argus, argus_option_t *option);
int validate_group(argus_t *argus, argus_option_t *option);
int validate_option(argus_t *argus, argus_option_t *options, argus_option_t *option);
int validate_positional(argus_t *argus, argus_option_t *option);

static int ensure_validity(argus_t *argus, argus_option_t *options, argus_option_t *option)
{
    switch (option->type) {
        case TYPE_OPTION:
            return validate_option(argus, options, option);
        case TYPE_POSITIONAL:
            return validate_positional(argus, option);
        case TYPE_GROUP:
            return validate_group(argus, option);
        case TYPE_SUBCOMMAND:
            return validate_subcommand(argus, option);
        default:
            ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_MALFORMED_OPTION, "Invalid option type");
            return ARGUS_ERROR_MALFORMED_OPTION;
    }
}

static int is_unique(argus_t *argus, argus_option_t *option, argus_option_t *other_option)
{
    int status = ARGUS_SUCCESS;

    if (option->name && other_option->name && strcmp(option->name, other_option->name) == 0) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_DUPLICATE_OPTION, "%s: Name must be unique",
                            option->name);
        status = ARGUS_ERROR_DUPLICATE_OPTION;
    }

    if (option->sname && other_option->sname && option->sname == other_option->sname) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_DUPLICATE_OPTION, "%c: Short name must be unique",
                            option->sname);
        status = ARGUS_ERROR_DUPLICATE_OPTION;
    }

    if (option->lname && other_option->lname && strcmp(option->lname, other_option->lname) == 0) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_DUPLICATE_OPTION, "%s: Long name must be unique",
                            option->lname);
        status = ARGUS_ERROR_DUPLICATE_OPTION;
    }

    return (status);
}

int validate_structure(argus_t *argus, argus_option_t *options)
{
    bool has_helper              = false;
    bool has_optional_positional = false;
    bool has_subcommands         = false;
    bool has_positionals         = false;
    int  status                  = ARGUS_SUCCESS;

    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        argus_option_t *option = &options[i];
        context_set_option(argus, option);

        if (options[i].type == TYPE_SUBCOMMAND)
            has_subcommands = true;
        if (options[i].type == TYPE_POSITIONAL)
            has_positionals = true;
        if (has_subcommands && has_positionals) {
            ARGUS_COLLECT_ERROR(
                argus, ARGUS_ERROR_MALFORMED_OPTION,
                "Cannot mix subcommands and positional arguments at the same level");
            status = ARGUS_ERROR_MALFORMED_OPTION;
        }

        int result = ensure_validity(argus, options, option);
        if (result != ARGUS_SUCCESS)
            status = result;

        for (int j = i + 1; options[j].type != TYPE_NONE; ++j) {
            argus_option_t *other_option = &options[j];

            if (option->type != other_option->type)
                continue;

            result = is_unique(argus, option, other_option);
            if (result != ARGUS_SUCCESS)
                status = result;
        }

        if (option->type == TYPE_OPTION && strcmp(option->name, "help") == 0)
            has_helper = true;

        // Checking bad order of positional arguments. Required positional arguments must be before
        // optional positional arguments
        if (option->type == TYPE_POSITIONAL && (option->flags & FLAG_REQUIRED) &&
            has_optional_positional) {
            ARGUS_COLLECT_ERROR(
                argus, ARGUS_ERROR_INVALID_POSITION,
                "Required positional must be before all optional positional arguments");
            status = ARGUS_ERROR_INVALID_POSITION;
        }
        if (option->type == TYPE_POSITIONAL && (option->flags & FLAG_REQUIRED) == 0)
            has_optional_positional = true;

        // Validate sub_options recursively
        if (option->type == TYPE_SUBCOMMAND && option->sub_options != NULL) {
            context_push_subcommand(argus, option);
            validate_structure(argus, option->sub_options);
            context_pop_subcommand(argus);
        }
    }
    if (!has_helper) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_MISSING_HELP, "Missing 'help' option");
        status = ARGUS_ERROR_MISSING_HELP;
    }
    return (status);
}
