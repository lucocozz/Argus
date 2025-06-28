#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/cross_platform.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

// Forward declaration
char *format_choices_validator(validator_data_t data);

int choices_string_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option  = (argus_option_t *)option_ptr;
    choices_data_t *choices = &data.choices;

    for (size_t i = 0; i < choices->count; i++) {
        if (strcmp(option->value.as_string, choices->as_strings[i]) == 0)
            return ARGUS_SUCCESS;
    }
    char *choices_formatted = format_choices_validator(data);
    if (choices_formatted) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_CHOICE, "Value '%s' is not one of [%s]",
                            option->value.as_string, choices_formatted);
        free(choices_formatted);
    } else {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_CHOICE,
                            "Value '%s' is not one of the choices", option->value.as_string);
    }
    return ARGUS_ERROR_INVALID_CHOICE;
}

int choices_int_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option  = (argus_option_t *)option_ptr;
    choices_data_t *choices = &data.choices;

    for (size_t i = 0; i < choices->count; i++) {
        if (option->value.as_int == choices->as_ints[i])
            return ARGUS_SUCCESS;
    }

    char *choices_formatted = format_choices_validator(data);
    if (choices_formatted) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_CHOICE, "Value '%d' is not one of [%s]",
                            option->value.as_int, choices_formatted);
        free(choices_formatted);
    } else {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_CHOICE,
                            "Value '%d' is not one of the choices", option->value.as_int);
    }
    return ARGUS_ERROR_INVALID_CHOICE;
}

int choices_float_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option  = (argus_option_t *)option_ptr;
    choices_data_t *choices = &data.choices;

    for (size_t i = 0; i < choices->count; i++) {
        if (option->value.as_float == choices->as_floats[i])
            return ARGUS_SUCCESS;
    }

    char *choices_formatted = format_choices_validator(data);
    if (choices_formatted) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_CHOICE, "Value '%f' is not one of [%s]",
                            option->value.as_float, choices_formatted);
        free(choices_formatted);
    } else {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_CHOICE,
                            "Value '%f' is not one of the choices", option->value.as_float);
    }
    return ARGUS_ERROR_INVALID_CHOICE;
}

char *format_choices_validator(validator_data_t data)
{
    choices_data_t *choices      = &data.choices;
    size_t          total_length = 0;

    // Calculate total length needed
    for (size_t i = 0; i < choices->count; i++) {
        switch (choices->type) {
            case VALUE_TYPE_STRING:
                total_length += strlen(choices->as_strings[i]);
                break;
            case VALUE_TYPE_INT:
                total_length += 20;  // Max digits for int64_t
                break;
            case VALUE_TYPE_FLOAT:
                total_length += 20;  // Max digits for double
                break;
            default:
                break;
        }
        if (i < choices->count - 1) {
            total_length += 1;  // For '|' separator
        }
    }

    char *result = malloc(total_length + 1);
    if (!result)
        return NULL;

    result[0] = '\0';

    for (size_t i = 0; i < choices->count; i++) {
        char item[32];

        switch (choices->type) {
            case VALUE_TYPE_STRING:
                safe_strcat(result, total_length + 1, choices->as_strings[i]);
                break;
            case VALUE_TYPE_INT:
                snprintf(item, sizeof(item), "%" PRId64, choices->as_ints[i]);
                safe_strcat(result, total_length + 1, item);
                break;
            case VALUE_TYPE_FLOAT:
                snprintf(item, sizeof(item), "%.2g", choices->as_floats[i]);
                safe_strcat(result, total_length + 1, item);
                break;
            default:
                break;
        }

        if (i < choices->count - 1)
            safe_strcat(result, total_length + 1, "|");
    }

    return result;
}
