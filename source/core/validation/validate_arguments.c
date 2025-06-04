#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/types.h"
#include <stddef.h>
#include <string.h>

static int validate_basics(argus_t *argus, argus_option_t *option)
{
    int status = ARGUS_SUCCESS;

    if (option->sname == 0 && option->lname == NULL) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_MALFORMED_OPTION,
                            "Option must have a short name or a long name");
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }
    if ((option->value_type == VALUE_TYPE_PRIMITIVE && option->flags & ~OPTION_FLAG_MASK) ||
        (option->value_type == VALUE_TYPE_ARRAY && option->flags & ~OPTION_ARRAY_FLAG_MASK) ||
        (option->value_type == VALUE_TYPE_MAP && option->flags & ~OPTION_MAP_FLAG_MASK)) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_INVALID_FLAG, "Invalid flag for option: '%s'",
                            option->name);
        status = ARGUS_ERROR_INVALID_FLAG;
    }

    if (option->help == NULL) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_MALFORMED_OPTION,
                            "Option '%s' must have a help message", option->name);
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }

    if (option->handler == NULL) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_INVALID_HANDLER, "Option '%s' must have a handler",
                            option->name);
        status = ARGUS_ERROR_INVALID_HANDLER;
    }

    return (status);
}

static int validate_default_value(argus_t *argus, argus_option_t *option)
{
    int status = ARGUS_SUCCESS;

    if (option->have_default && option->value_type == VALUE_TYPE_FLAG) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_INVALID_DEFAULT,
                            "Option '%s' cannot have a default value for flag type", option->name);
        status = ARGUS_ERROR_INVALID_DEFAULT;
    }

    // if (option->choices_count > 0 && option->have_default) {
    //     bool valid_default = false;
    //     for (size_t i = 0; i < option->choices_count && !valid_default; ++i) {
    //         argus_value_t choice =
    //             choices_to_value(option->value_type, option->choices, option->choices_count, i);
    //         valid_default = (cmp_value(option->value_type, option->value, choice) == 0);
    //     }
    //     if (!valid_default) {
    //         ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_INVALID_DEFAULT,
    //                             "Default value of option '%s' must be one of the available choices",
    //                             option->name);
    //         status = ARGUS_ERROR_INVALID_DEFAULT;
    //     }
    // }

    return (status);
}

static int validate_dependencies(argus_t *argus, argus_option_t *options, argus_option_t *option)
{
    int status = ARGUS_SUCCESS;

    if (option->requires != NULL && option->conflicts != NULL) {
        for (int i = 0; option->requires[i] != NULL; ++i) {
            for (int j = 0; option->conflicts[j] != NULL; ++j) {
                if (strcmp(option->requires[i], option->conflicts[j]) == 0) {
                    ARGUS_COLLECT_ERROR(
                        argus, ARGUS_ERROR_INVALID_DEPENDENCY,
                        "Option '%s' cannot require and conflict with the same option: '%s'",
                        option->name, option->requires[i]);
                    status = ARGUS_ERROR_INVALID_DEPENDENCY;
                }
            }
        }
    }

    if (option->requires != NULL) {
        for (int i = 0; option->requires[i] != NULL; ++i) {
            argus_option_t *required = find_option_by_name(options, option->requires[i]);
            if (required == NULL) {
                ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_INVALID_DEPENDENCY,
                                    "Required option not found '%s' in option '%s'",
                                    option->requires[i], option->name);
                status = ARGUS_ERROR_INVALID_DEPENDENCY;
            }
        }
    }

    if (option->conflicts != NULL) {
        for (int i = 0; option->conflicts[i] != NULL; ++i) {
            argus_option_t *conflict = find_option_by_name(options, option->conflicts[i]);
            if (conflict == NULL) {
                ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_INVALID_DEPENDENCY,
                                    "Conflicting option not found '%s' in option '%s'",
                                    option->conflicts[i], option->name);
                status = ARGUS_ERROR_INVALID_DEPENDENCY;
            }
        }
    }

    return (status);
}

int validate_option(argus_t *argus, argus_option_t *options, argus_option_t *option)
{
    int status = ARGUS_SUCCESS;

    status = validate_basics(argus, option);
    if (status != ARGUS_SUCCESS)
        return (status);

    status = validate_default_value(argus, option);
    if (status != ARGUS_SUCCESS)
        return (status);

    status = validate_dependencies(argus, options, option);
    return (status);
}

int validate_positional(argus_t *argus, argus_option_t *option)
{
    int status = ARGUS_SUCCESS;

    if (option->name == NULL) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_MALFORMED_OPTION,
                            "Positional option must have a name");
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }

    if (option->help == NULL) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_MALFORMED_OPTION,
                            "Positional option '%s' must have a help message", option->name);
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }

    if (option->flags & ~POSITIONAL_FLAG_MASK) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_INVALID_FLAG,
                            "Invalid flags for positional option '%s'", option->name);
        status = ARGUS_ERROR_INVALID_FLAG;
    }

    if (option->handler == NULL) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_INVALID_HANDLER,
                            "Positional option '%s' must have a handler", option->name);
        status = ARGUS_ERROR_INVALID_HANDLER;
    }

    if ((option->flags & FLAG_REQUIRED) && option->have_default) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_INVALID_FLAG,
                            "Positional option '%s' cannot be required and have a default value",
                            option->name);
        status = ARGUS_ERROR_INVALID_FLAG;
    }

    // if (option->choices_count > 0 && option->have_default) {
    //     bool valid_default = false;
    //     for (size_t i = 0; i < option->choices_count && !valid_default; ++i) {
    //         argus_value_t choice =
    //             choices_to_value(option->value_type, option->choices, option->choices_count, i);
    //         valid_default = (cmp_value(option->value_type, option->value, choice) == 0);
    //     }
    //     if (!valid_default) {
    //         ARGUS_COLLECT_ERROR(
    //             argus, ARGUS_ERROR_INVALID_DEFAULT,
    //             "Default value of positional option '%s' must be one of the available choices",
    //             option->name);
    //         status = ARGUS_ERROR_INVALID_DEFAULT;
    //     }
    // }

    return (status);
}
