/**
 * argus/internal/callbacks/handlers.h - Internal handlers implementation
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines standard handler callbacks for different option types.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_INTERNAL_CALLBACKS_HANDLERS_H
#define ARGUS_INTERNAL_CALLBACKS_HANDLERS_H

#include "argus/types.h"

/**
 * Type-specific option handlers
 */
int string_handler(argus_t *argus, argus_option_t *options, char *arg);
int int_handler(argus_t *argus, argus_option_t *options, char *arg);
int bool_handler(argus_t *argus, argus_option_t *options, char *arg);
int flag_handler(argus_t *argus, argus_option_t *options, char *arg);
int float_handler(argus_t *argus, argus_option_t *options, char *arg);

int array_string_handler(argus_t *argus, argus_option_t *options, char *arg);
int array_int_handler(argus_t *argus, argus_option_t *options, char *arg);
int array_float_handler(argus_t *argus, argus_option_t *options, char *arg);

int map_string_handler(argus_t *argus, argus_option_t *options, char *arg);
int map_int_handler(argus_t *argus, argus_option_t *options, char *arg);
int map_float_handler(argus_t *argus, argus_option_t *options, char *arg);
int map_bool_handler(argus_t *argus, argus_option_t *options, char *arg);

/**
 * Type-specific option free handlers
 */
int default_free(argus_option_t *option);
int free_array_string_handler(argus_option_t *option);
// int free_array_int_handler(argus_option_t *option);
// int free_array_float_handler(argus_option_t *option);

int free_map_string_handler(argus_option_t *option);
int free_map_int_handler(argus_option_t *option);
int free_map_float_handler(argus_option_t *option);
int free_map_bool_handler(argus_option_t *option);

/**
 * Special option handlers
 */
int help_handler(argus_t *argus, argus_option_t *options, char *arg);
int version_handler(argus_t *argus, argus_option_t *options, char *arg);

#endif /* ARGUS_INTERNAL_CALLBACKS_HANDLERS_H */
