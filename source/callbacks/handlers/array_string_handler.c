#define _GNU_SOURCE  // NOLINT

#include <stdlib.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/cross_platform.h"
#include "argus/internal/utils.h"
#include "argus/options.h"
#include "argus/types.h"

static int set_value(argus_option_t *option, char *value)
{
    adjust_array_size(option);
    if (option->value_capacity == 0)  // adjust_array_size failed
        return ARGUS_ERROR_MEMORY;

    char *dup_value = safe_strdup(value);
    if (dup_value == NULL)
        return ARGUS_ERROR_MEMORY;

    option->value.as_array[option->value_count].as_string = dup_value;
    option->value_count++;
    return ARGUS_SUCCESS;
}

int array_string_handler(argus_t *argus, argus_option_t *option, char *value)
{
    UNUSED(argus);

    if (strchr(value, ',') != NULL) {
        char **splited_values = split(value, ",");
        if (splited_values == NULL) {
            ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_MEMORY, "Failed to split string '%s'", value);
            return ARGUS_ERROR_MEMORY;
        }
        for (size_t i = 0; splited_values[i] != NULL; ++i) {
            int status = set_value(option, splited_values[i]);
            if (status != ARGUS_SUCCESS) {
                free_split(splited_values);
                return status;
            }
        }
        free_split(splited_values);
    } else {
        int status = set_value(option, value);
        if (status != ARGUS_SUCCESS)
            return status;
    }

    apply_array_flags(option);
    option->is_allocated = true;
    return (ARGUS_SUCCESS);
}

int free_array_string_handler(argus_option_t *option)
{
    for (size_t i = 0; i < option->value_count; ++i)
        free(option->value.as_array[i].as_string);
    free(option->value.as_array);
    return (ARGUS_SUCCESS);
}
