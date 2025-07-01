/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#include <stdio.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

static int validate_required(argus_t *argus, argus_option_t *options, argus_option_t *option)
{
    if (option->require) {
        for (int j = 0; option->require[j] != NULL; ++j) {
            argus_option_t *required = find_option_by_name(options, option->require[j]);
            if (required && !required->is_set) {
                ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_MISSING_REQUIRED,
                                    "Required option is missing: '%s' with option '%s'",
                                    option->require[j], option->name);
                return (ARGUS_ERROR_MISSING_REQUIRED);
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
                ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_CONFLICTING_OPTIONS,
                                    "Conflict between '%s' and '%s'", option->name, conflict->name);
                return (ARGUS_ERROR_CONFLICTING_OPTIONS);
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
    const char *group_name                 = NULL;
    bool        current_group_is_exclusive = false;
    const char *first_set_option_name      = NULL;

    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        argus_option_t *option = &options[i];

        if (option->type == TYPE_GROUP) {
            group_name                 = option->name;
            current_group_is_exclusive = option->flags & FLAG_EXCLUSIVE;
            first_set_option_name      = NULL;
            continue;
        }

        if (option->type == TYPE_POSITIONAL && (option->flags & FLAG_REQUIRED) && !option->is_set) {
            ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_MISSING_REQUIRED,
                                "Required positional argument missing: '%s'", option->name);
            return (ARGUS_ERROR_MISSING_REQUIRED);
        }

        if (option->is_set) {
            int status;

            if (current_group_is_exclusive) {
                if (first_set_option_name == NULL) {
                    first_set_option_name = option->name;
                } else {
                    ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_EXCLUSIVE_GROUP,
                                        "Exclusive options group '%s' conflict: '%s' and '%s'",
                                        group_name, first_set_option_name, option->name);
                    return (ARGUS_ERROR_EXCLUSIVE_GROUP);
                }
            }

            status = call_validators(argus, option);
            if (status != ARGUS_SUCCESS)
                return (status);

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

    return status;
}
