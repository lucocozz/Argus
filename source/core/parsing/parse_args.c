#include <ctype.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/parsing.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

int find_subcommand(argus_t *argus, argus_option_t *options, const char *name,
                    argus_option_t **result)
{
    argus_option_t *subcommand = NULL;

    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        if (options[i].type == TYPE_SUBCOMMAND && starts_with(name, options[i].name) != NULL) {
            if (subcommand != NULL) {
                ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_AMBIGUOUS_SUBCOMMAND,
                                    "'%s' is ambigous and could match:\n"
                                    "  '%s' - %s\n"
                                    "  '%s' - %s",
                                    name, subcommand->name, subcommand->help, options[i].name,
                                    options[i].help);
                return ARGUS_ERROR_AMBIGUOUS_SUBCOMMAND;
            }
            if (subcommand == NULL && strcmp(name, options[i].name) == 0) {
                *result = &options[i];
                return ARGUS_SUCCESS;
            }
            subcommand = &options[i];
        }
    }
    *result = subcommand;
    return ARGUS_SUCCESS;
}

int parse_args(argus_t *argus, argus_option_t *options, int argc, char **argv)
{
    int  positional_index = 0;
    bool only_positional  = false;
    int  status           = ARGUS_SUCCESS;

    for (int i = 0; i < argc; ++i) {
        char *arg = argv[i];

        if (strcmp(arg, "--") == 0) {
            only_positional = true;
            continue;
        }

        if (only_positional) {
            status = handle_positional(argus, options, arg, positional_index++);
            if (status != ARGUS_SUCCESS)
                return (status);
            continue;
        }

        char *long_arg = starts_with("--", arg);
        if (long_arg != NULL) {
            status = handle_long_option(argus, options, long_arg, argv, argc, &i);
            if (status != ARGUS_SUCCESS)
                return (status);
            continue;
        }

        char *short_arg = starts_with("-", arg);
        if (short_arg != NULL) {
            // Checking if this is a negative number or an option
            if (isdigit(short_arg[0]) || (short_arg[0] == '.' && isdigit(short_arg[1]))) {
                argus_option_t *pos_opt = find_positional(options, positional_index);

                if (pos_opt && (pos_opt->value_type & VALUE_TYPE_ANY_NUMERIC)) {
                    status = handle_positional(argus, options, arg, positional_index++);
                    if (status != ARGUS_SUCCESS)
                        return (status);
                    continue;
                }
            }

            // Otherwise, handle as a regular short option
            status = handle_short_option(argus, options, short_arg, argv, argc, &i);
            if (status != ARGUS_SUCCESS)
                return (status);
            continue;
        }

        argus_option_t *subcommand = NULL;
        status                     = find_subcommand(argus, options, arg, &subcommand);
        if (status != ARGUS_SUCCESS)
            return status;
        if (subcommand != NULL) {
            status = handle_subcommand(argus, subcommand, argc - i - 1, &argv[i + 1]);
            return (status);
        }

        status = handle_positional(argus, options, arg, positional_index++);
        if (status != ARGUS_SUCCESS)
            return (status);
    }
    return (status);
}
