#include "argus/errors.h"
#include "argus/types.h"
#include <stddef.h>
#include <string.h>

int validate_subcommand(argus_option_t *option);
int validate_group(argus_option_t *option);
int validate_option(argus_t *argus, argus_option_t *options, argus_option_t *option);
int validate_positional(argus_t *argus, argus_option_t *option);

int validate_placement(argus_option_t *options)
{
    bool has_positional          = false;
    bool has_optional_positional = false;
    bool has_subcommands         = false;
    bool has_helper              = false;
    int  status                  = ARGUS_SUCCESS;

    for (int i = 0; options[i].type != TYPE_NONE; ++i)
    {
        argus_option_t *option = &options[i];

        if (option->type == TYPE_SUBCOMMAND)
            has_subcommands = true;
        if (option->type == TYPE_POSITIONAL && (option->flags & FLAG_OPTIONAL))
            has_optional_positional = true;
        else if (option->type == TYPE_POSITIONAL)
            has_positional = true;
        if (option->type == TYPE_OPTION && option->name && strcmp(option->name, "help") == 0)
            has_helper = true;

        if (option->type == TYPE_OPTION && has_subcommands) {
            ARGUS_STRUCT_ERROR(option,
                "Options should be placed at the top level, before any subcommands");
            status = ARGUS_STRUCT_ERROR;
        }
        if (has_subcommands && has_positional) {
            ARGUS_STRUCT_ERROR(option,
                "Cannot mix subcommands and positional arguments at the same level");
            status = ARGUS_STRUCT_ERROR;
        }
        if (option->type == TYPE_POSITIONAL && (option->flags & FLAG_REQUIRED) &&
            has_optional_positional) {
            ARGUS_STRUCT_ERROR(option,
                "Required positional must be before all optional positional arguments");
            status = ARGUS_STRUCT_ERROR;
        }
    }
    if (!has_helper) {
        ARGUS_STRUCT_ERROR(&options[0], "Help option is missing");
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }
    return (status);
}

static int lname_is_unique(argus_option_t *options, int i)
{
    int status = ARGUS_SUCCESS;

    if (options[i].lname == NULL)
        return status;

    for (int j = i + 1; options[j].type != TYPE_NONE; ++j)
    {
        if (options[i].lname && options[j].name && strcmp(options[i].lname, options[j].name) == 0) {
            ARGUS_STRUCT_ERROR(&options[i],
                "Name conflict with option line: %d", options[j].line);
            status = ARGUS_ERROR_DUPLICATE_OPTION;
        }
        if (options[i].lname && options[j].lname && strcmp(options[i].lname, options[j].lname) == 0) {
            ARGUS_STRUCT_ERROR(&options[i],
                "Long name conflict with option line: %d", options[j].line);
            status = ARGUS_ERROR_DUPLICATE_OPTION;
        }
    }
    return status;
}

int validate_naming(argus_option_t *options)
{
    bool used_short[256] = {0};
    int  status = ARGUS_SUCCESS;

    for (int i = 0; options[i].type != TYPE_NONE; ++i)
    {
        argus_option_t *option = &options[i];

        // key name
        if ((option->name == NULL || option->name[0] == '\0') && option->type != TYPE_GROUP) {
            ARGUS_STRUCT_ERROR(option, "Option name cannot be empty");
            status = ARGUS_ERROR_MALFORMED_OPTION;
        }

        // short name
        if (option->sname != '\0' && option->sname == ' ') {
            ARGUS_STRUCT_ERROR(option, "Short name cannot be a space character");
            status = ARGUS_ERROR_MALFORMED_OPTION;
        }
        if (option->sname != '\0' && option->sname == '-') {
            ARGUS_STRUCT_ERROR(option, "Short name cannot be a hyphen");
            status = ARGUS_ERROR_MALFORMED_OPTION;
        }
        if (option->sname != '\0' && used_short[(unsigned char)option->sname]) {
            ARGUS_STRUCT_ERROR(option, "Short name '%c' is already used", option->sname);
            status = ARGUS_ERROR_MALFORMED_OPTION;
        }
        if (option->sname != '\0')
            used_short[(unsigned char)option->sname] = true;

        // long name
        if (option->lname != NULL && option->lname[0] == '-') {
            ARGUS_STRUCT_ERROR(option, "Long name cannot start with a hyphen");
            status = ARGUS_ERROR_MALFORMED_OPTION;
        }
        if (option->lname != NULL && strchr(option->lname, ' ') != NULL) {
            ARGUS_STRUCT_ERROR(option, "Long name cannot contain spaces");
            status = ARGUS_ERROR_MALFORMED_OPTION;
        }
        if (option->lname != NULL && strchr(option->lname, '.') != NULL) {
            ARGUS_STRUCT_ERROR(option, "Long name cannot contain dots");
            status = ARGUS_ERROR_MALFORMED_OPTION;
        }
        if (option->lname != NULL && option->lname[0] == '\0') {
            ARGUS_STRUCT_ERROR(option, "Long name cannot be empty");
            status = ARGUS_ERROR_MALFORMED_OPTION;
        }
        if (lname_is_unique(options, i) != ARGUS_SUCCESS)
            status = ARGUS_ERROR_MALFORMED_OPTION;
    }
    return (status);
}

static int validate_type_format(argus_t *argus, argus_option_t *options)
{
    int status = ARGUS_SUCCESS;

    for (int i = 0; options[i].type != TYPE_NONE; ++i)
    {
        argus_option_t *option = &options[i];

        switch (option->type) {
            case TYPE_OPTION:
                if (validate_option(argus, options, option) != ARGUS_SUCCESS)
                    status = ARGUS_ERROR_MALFORMED_OPTION;
                continue;
            case TYPE_POSITIONAL:
                if (validate_positional(argus, option) != ARGUS_SUCCESS)
                    status = ARGUS_ERROR_MALFORMED_OPTION;
                continue;
            case TYPE_GROUP:
                if (validate_group(option) != ARGUS_SUCCESS)
                    status = ARGUS_ERROR_MALFORMED_OPTION;
                continue;
            case TYPE_SUBCOMMAND:
                if (validate_subcommand(option) != ARGUS_SUCCESS)
                    status = ARGUS_ERROR_MALFORMED_OPTION;
                continue;
            default:
                ARGUS_STRUCT_ERROR(option, "Invalid option type");
                status = ARGUS_ERROR_MALFORMED_OPTION;
        }
    }
    return (status);
}

int validate_structure(argus_t *argus, argus_option_t *options)
{
    int status = ARGUS_SUCCESS;
    static int subcommand_depth = 0;

    if (validate_placement(options) != ARGUS_SUCCESS)
        status = ARGUS_ERROR_MALFORMED_OPTION;
    if (validate_naming(options) != ARGUS_SUCCESS)
        status = ARGUS_ERROR_MALFORMED_OPTION;
    if (validate_type_format(argus, options) != ARGUS_SUCCESS)
        status = ARGUS_ERROR_MALFORMED_OPTION;

    for (int i = 0; options[i].type != TYPE_NONE; ++i)
    {
        argus_option_t *option = &options[i];
        if (option->type == TYPE_SUBCOMMAND && option->sub_options != NULL)
        {
            subcommand_depth++;
            if (subcommand_depth > MAX_SUBCOMMAND_DEPTH) {
                ARGUS_STRUCT_ERROR(option,
                    "Subcommand depth exceeds maximum allowed (%d)", MAX_SUBCOMMAND_DEPTH);
                status = ARGUS_ERROR_MALFORMED_OPTION;
            }
            if (validate_structure(argus, option->sub_options) != ARGUS_SUCCESS)
                status = ARGUS_ERROR_MALFORMED_OPTION;
            subcommand_depth--;
        }
    }
    return (status);
}
