/**
 * argus/internal/utils.h - Internal utility functions
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines utility functions used internally by the library.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_INTERNAL_UTILS_H
#define ARGUS_INTERNAL_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argus/types.h" /* Inclure les types publics */

/**
 * ANSI color codes for terminal output
 */
#define ANSI_RESET     "\x1b[0m"
#define ANSI_BOLD      "\x1b[1m"
#define ANSI_ITALIC    "\x1b[3m"
#define ANSI_UNDERLINE "\x1b[4m"
#define ANSI_RED       "\x1b[31m"
#define ANSI_YELLOW    "\x1b[33m"
#define ANSI_BLUE      "\x1b[34m"
#define ANSI_GREEN     "\x1b[32m"
#define ANSI_MAGENTA   "\x1b[35m"
#define ANSI_CYAN      "\x1b[36m"
#define ANSI_WHITE     "\x1b[37m"
#define ANSI_GRAY      "\x1b[90m"

/**
 * Conditional color output macros
 */
#ifdef NO_COLOR
    #define COLOR(code, text) text
#else
    #define COLOR(code, text) code text ANSI_RESET
#endif

/**
 * Utility macros
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define EXPAND(x)       x
#define UNUSED(x)       (void)(x)

/**
 * String utility functions
 */
char  *starts_with(const char *prefix, const char *str);
char **split(const char *str, const char *charset);
void   free_split(char **split);

/**
 * Multi_value utility functions
 */
#define MULTI_VALUE_INITIAL_CAPACITY 8
void adjust_array_size(argus_option_t *option);
void adjust_map_size(argus_option_t *option);
int  map_find_key(argus_option_t *option, const char *key);
void apply_array_flags(argus_option_t *option);
void apply_map_flags(argus_option_t *option);

/**
 * Value manipulation functions
 */
int           cmp_value(argus_valtype_t type, argus_value_t a, argus_value_t b);
argus_value_t choices_to_value(argus_valtype_t type, argus_value_t choices, size_t choices_count,
                               int index);
void          free_option_value(argus_option_t *option);
void          print_value(FILE *stream, argus_valtype_t type, argus_value_t value);
void print_value_array(FILE *stream, argus_valtype_t type, argus_value_t *values, size_t count);

/**
 * Option lookup functions
 */
argus_option_t       *find_option_by_lname(argus_option_t *options, const char *lname);
argus_option_t       *find_option_by_name(argus_option_t *options, const char *name);
argus_option_t       *find_option_by_sname(argus_option_t *options, char sname);
argus_option_t       *find_positional(argus_option_t *options, int position);
argus_option_t       *find_subcommand(argus_option_t *options, const char *name);
argus_option_t       *find_option_by_active_path(argus_t argus, const char *option_path);
const argus_option_t *get_active_options(argus_t *argus);

#endif /* ARGUS_INTERNAL_UTILS_H */
