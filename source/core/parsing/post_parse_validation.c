#include <stdio.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/context.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

// static int validate_choices(argus_t *argus, argus_option_t *option)
// {
//     if (option->choices_count > 0) {
//         bool valid_choices = false;
//         for (size_t i = 0; i < option->choices_count && !valid_choices; ++i) {
//             argus_value_t choice =
//                 choices_to_value(option->value_type, option->choices, option->choices_count, i);
//             valid_choices = (cmp_value(option->value_type, option->value, choice) == 0);
//         }
//         if (!valid_choices) {
//             fprintf(stderr, "%s: The '%s' option cannot be set to '", argus->program_name,
//                     option->name);
//             print_value(stderr, option->value_type, option->value);
//             fprintf(stderr, "'. Please choose from ");
//             print_value_array(stderr, option->value_type, option->choices.as_ptr,
//                               option->choices_count);
//             fprintf(stderr, "\n");
//             return (ARGUS_ERROR_INVALID_CHOICE);
//         }
//     }
//     return (ARGUS_SUCCESS);
// }

static int validate_required(argus_t *argus, argus_option_t *options, argus_option_t *option)
{
    if (option->require) {
        for (int j = 0; option->require[j] != NULL; ++j) {
            argus_option_t *required = find_option_by_name(options, option->require[j]);
            if (required && !required->is_set) {
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MISSING_REQUIRED,
                                   "Required option is missing: '%s' with option '%s'",
                                   option->require[j], option->name);
            }
        }
    }
    return (ARGUS_SUCCESS);
}

static int validate_conflicts(argus_t *argus, argus_option_t *options, argus_option_t *option)
{
    if (option->conflict) {
        for (int j = 0; option->conflict[j] != NULL; ++j) {
            argus_option_t *conflict = find_option_by_name(options, option->conflict[j]);
            if (conflict && conflict->is_set) {
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_CONFLICTING_OPTIONS,
                                   "Conflict between '%s' and '%s'", option->name, conflict->name);
            }
        }
    }
    return (ARGUS_SUCCESS);
}

static int call_validators(argus_t *argus, argus_option_t *option)
{
    if (!option->validators) {
        return ARGUS_SUCCESS;
    }

    for (size_t i = 0; option->validators[i] != NULL; ++i) {
        validator_entry_t *validator = option->validators[i];
        if (validator->order == ORDER_POST) {
            int status = validator->func(argus, (void *)option, validator->data);
            if (status != ARGUS_SUCCESS)
                return (status);
        }
    }
    return (ARGUS_SUCCESS);
}

static int validate_options_set(argus_t *argus, argus_option_t *options)
{
    bool        current_group_is_exclusive = false;
    const char *first_set_option_name      = NULL;

    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        argus_option_t *option = &options[i];

        if (option->type == TYPE_GROUP) {
            context_set_group(argus, option);
            current_group_is_exclusive = option->flags & FLAG_EXCLUSIVE;
            first_set_option_name      = NULL;
            continue;
        }

        if (option->type == TYPE_POSITIONAL && (option->flags & FLAG_REQUIRED) && !option->is_set) {
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MISSING_REQUIRED,
                               "Required positional argument missing: '%s'", option->name);
        }

        if (option->is_set) {
            int status;

            if (current_group_is_exclusive) {
                if (first_set_option_name == NULL) {
                    first_set_option_name = option->name;
                } else {
                    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_EXCLUSIVE_GROUP,
                                       "Exclusive options group '%s' conflict: '%s' and '%s'",
                                       argus->context.group, first_set_option_name, option->name);
                }
            }

            status = call_validators(argus, option);
            if (status != ARGUS_SUCCESS)
                return (status);

            // status = validate_choices(argus, option);
            // if (status != ARGUS_SUCCESS)
            //     return (status);

            status = validate_required(argus, options, option);
            if (status != ARGUS_SUCCESS)
                return (status);

            status = validate_conflicts(argus, options, option);
            if (status != ARGUS_SUCCESS)
                return (status);
        }
    }
    return (ARGUS_SUCCESS);
}

int post_parse_validation(argus_t *argus)
{
    int status;

    status = validate_options_set(argus, argus->options);
    if (status != ARGUS_SUCCESS)
        return status;

    for (size_t i = 0; i < argus->context.subcommand_depth; ++i) {
        const argus_option_t *subcommand = argus->context.subcommand_stack[i];
        if (!subcommand || !subcommand->sub_options)
            continue;

        int subcommand_status = validate_options_set(argus, subcommand->sub_options);
        if (subcommand_status != ARGUS_SUCCESS)
            return subcommand_status;
    }

    return status;
}
