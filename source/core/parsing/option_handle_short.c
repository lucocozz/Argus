#include <stdio.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/parsing.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

int handle_short_option(argus_t *argus, argus_option_t *options, char *arg, char **argv, int argc,
                        int *current_index)
{
    size_t len = strlen(arg);

    // Format "-abc"
    for (size_t i = 0; i < len; ++i) {
        char            option_char = arg[i];
        argus_option_t *option      = find_option_by_sname(options, option_char);
        if (option == NULL) {
            ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_ARGUMENT, "Unknown option: '-%c'",
                                option_char);
            return ARGUS_ERROR_INVALID_ARGUMENT;
        }

        char *value = NULL;
        if (option->value_type != VALUE_TYPE_FLAG) {
            // Format "-ovalue"
            if (i < len - 1) {
                value = arg + i + 1;
                i     = len;
            } else if (*current_index + 1 < argc) {
                // Format ["-o", "value"]
                *current_index += 1;
                value = argv[*current_index];
            } else {
                ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_MISSING_VALUE,
                                    "Missing value for option: '-%c'", option_char);
                return ARGUS_ERROR_MISSING_VALUE;
            }
        }
        int status = execute_callbacks(argus, option, value);
        if (status != ARGUS_SUCCESS)
            return (status);
    }
    return (ARGUS_SUCCESS);
}
