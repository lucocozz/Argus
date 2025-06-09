#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/types.h"
#include <stddef.h>
#include <string.h>


static int validate_default_value(argus_t *argus, argus_option_t *option)
{
    int status = ARGUS_SUCCESS;

    if (option->have_default && option->value_type == VALUE_TYPE_FLAG) {
        ARGUS_STRUCT_ERROR(option, "Option type flag cannot have a default value");
        status = ARGUS_ERROR_INVALID_DEFAULT;
    }
    else if (option->have_default && option->validators != NULL)
    {
        for (size_t i = 0; option->validators[i] != NULL; ++i)
        {
            validator_entry_t *validator = option->validators[i];
            if (validator->order == ORDER_PRE && option->value_type == VALUE_TYPE_STRING) {
                if (validator->func(argus, option->default_value.as_string, validator->data) != ARGUS_SUCCESS)
                    status = ARGUS_ERROR_INVALID_DEFAULT;
            }
            else if (validator->order == ORDER_POST) {
                if (validator->func(argus, (void*)option, validator->data) != ARGUS_SUCCESS)
                    status = ARGUS_ERROR_INVALID_DEFAULT;
            }
            if (status != ARGUS_SUCCESS) {
                ARGUS_STRUCT_ERROR(option, "Default value does not pass validations");
            }
        }
    }

    return (status);
}

static int validate_dependencies(argus_option_t *options, argus_option_t *option)
{
    int status = ARGUS_SUCCESS;

    if (option->require != NULL && option->conflict != NULL) {
        for (int i = 0; option->require[i] != NULL; ++i) {
            for (int j = 0; option->conflict[j] != NULL; ++j) {
                if (strcmp(option->require[i], option->conflict[j]) == 0) {
                    ARGUS_STRUCT_ERROR(option,
                        "Option cannot require and conflict with the same option: '%s'", option->require[i]);
                    status = ARGUS_ERROR_INVALID_DEPENDENCY;
                }
            }
        }
    }

    if (option->require != NULL) {
        for (int i = 0; option->require[i] != NULL; ++i) {
            argus_option_t *required = find_option_by_name(options, option->require[i]);
            if (required == NULL) {
                ARGUS_STRUCT_ERROR(option,
                    "Required option not found '%s' in options", option->require[i]);
                status = ARGUS_ERROR_INVALID_DEPENDENCY;
            }
        }
    }

    if (option->conflict != NULL) {
        for (int i = 0; option->conflict[i] != NULL; ++i) {
            argus_option_t *conflict = find_option_by_name(options, option->conflict[i]);
            if (conflict == NULL) {
                ARGUS_STRUCT_ERROR(option,
                    "Conflicting option not found '%s' in options", option->conflict[i]);
                status = ARGUS_ERROR_INVALID_DEPENDENCY;
            }
        }
    }

    return (status);
}

int validate_option(argus_t *argus, argus_option_t *options, argus_option_t *option)
{
    int status = ARGUS_SUCCESS;

    if (option->sname == 0 && option->lname == NULL) {
        ARGUS_STRUCT_ERROR(option, "Option must have a short name or a long name");
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }
    if ((option->value_type == VALUE_TYPE_PRIMITIVE && option->flags & ~OPTION_FLAG_MASK) ||
        (option->value_type == VALUE_TYPE_ARRAY && option->flags & ~OPTION_ARRAY_FLAG_MASK) ||
        (option->value_type == VALUE_TYPE_MAP && option->flags & ~OPTION_MAP_FLAG_MASK)) {
        ARGUS_STRUCT_ERROR(option, "Option has invalid flags for its value type");
        status = ARGUS_ERROR_INVALID_FLAG;
    }

    if (option->help == NULL) {
        ARGUS_STRUCT_ERROR(option, "Option must have a help message");
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }

    if (option->handler == NULL) {
        ARGUS_STRUCT_ERROR(option, "Option must have a handler");
        status = ARGUS_ERROR_INVALID_HANDLER;
    }

    if (validate_default_value(argus, option) != ARGUS_SUCCESS)
        status = ARGUS_ERROR_MALFORMED_OPTION;

    if (validate_dependencies(options, option) != ARGUS_SUCCESS)
        status = ARGUS_ERROR_MALFORMED_OPTION;

    return (status);
}

int validate_positional(argus_t *argus, argus_option_t *option)
{
    int status = ARGUS_SUCCESS;

    if (option->name == NULL) {
        ARGUS_STRUCT_ERROR(option, "Positional must have a name");
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }

    if (option->help == NULL) {
        ARGUS_STRUCT_ERROR(option, "Positional must have a help message");
        status = ARGUS_ERROR_MALFORMED_OPTION;
    }

    if (option->flags & ~POSITIONAL_FLAG_MASK) {
        ARGUS_STRUCT_ERROR(option, "Invalid flags for positional");
        status = ARGUS_ERROR_INVALID_FLAG;
    }

    if (option->handler == NULL) {
        ARGUS_STRUCT_ERROR(option, "Positional must have a handler");
        status = ARGUS_ERROR_INVALID_HANDLER;
    }

    if ((option->flags & FLAG_REQUIRED) && option->have_default) {
        ARGUS_STRUCT_ERROR(option, "Positional cannot be required and have a default value");
        status = ARGUS_ERROR_INVALID_FLAG;
    }

    if (validate_default_value(argus, option) != ARGUS_SUCCESS)
        status = ARGUS_ERROR_MALFORMED_OPTION;

    if (option->require != NULL || option->conflict != NULL) {
        ARGUS_STRUCT_ERROR(option, "Positional cannot have dependencies");
        status = ARGUS_ERROR_INVALID_DEPENDENCY;
    }

    return (status);
}
