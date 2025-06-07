/**
 * argus/internal/parsing.h - Internal parsing functions
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines internal functions for parsing command-line arguments.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_INTERNAL_PARSING_H
#define ARGUS_INTERNAL_PARSING_H

#include "argus/types.h"

/**
 * parse_args - Parse an array of command-line arguments
 *
 * @param argus    Argus context
 * @param options  Options array
 * @param argc     Argument count
 * @param argv     Argument values
 *
 * @return Status code
 */
int             parse_args(argus_t *argus, argus_option_t *options, int argc, char **argv);
int find_subcommand(argus_t *argus, argus_option_t *options, const char *name, argus_option_t **result);

/**
 * Handle different types of arguments
 */
int handle_subcommand(argus_t *argus, argus_option_t *option, int argc, char **argv);
int handle_positional(argus_t *argus, argus_option_t *options, char *value, int position);
int handle_long_option(argus_t *argus, argus_option_t *options, char *arg, char **argv, int argc,
                       int *current_index);
int handle_short_option(argus_t *argus, argus_option_t *options, char *arg, char **argv, int argc,
                        int *current_index);

/**
 * Validation and callback execution
 */
int post_parse_validation(argus_t *argus);
int execute_callbacks(argus_t *argus, argus_option_t *option, char *value);

/**
 * Load option values from environment variables
 */
int load_env_vars(argus_t *argus);

#endif /* ARGUS_INTERNAL_PARSING_H */
