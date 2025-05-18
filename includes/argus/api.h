/**
 * argus/api.h - Public API functions
 *
 * This header defines all the public functions that users can call
 * to initialize, parse arguments, and access results.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_API_H
#define ARGUS_API_H

#include "argus/internal/compiler.h"
#include "argus/types.h"

ARGUS_API argus_t _argus_init_validate(argus_option_t *options, const char *program_name,
                                       const char *version, bool validate);

/**
 * argus_init - Initialize the argus context
 *
 * @param options      Array of command-line options
 * @param program_name Name of the program
 * @param version      Version string
 * @param description  Program description
 *
 * @return Initialized argus_t context
 *
 * @note
 * Define `ARGUS_RELEASE` when compiling your application to skip
 * options structure validation and improve performance.
 *
 * Example: `gcc -DARGUS_RELEASE my_program.c -o my_program -largus`
 *
 * Note: Only use this in production. During development, leave validation
 * enabled to catch configuration errors early.
 */
ARGUS_API argus_t argus_init(argus_option_t *options, const char *program_name, const char *version)
{
#ifdef ARGUS_RELEASE
    return _argus_init_validate(options, program_name, version, false);
#else
    return _argus_init_validate(options, program_name, version, true);
#endif
}

/**
 * argus_parse - Parse command-line arguments
 *
 * @param argus  Argus context
 * @param argc   Argument count (from main)
 * @param argv   Argument values (from main)
 *
 * @return Status code (0 for success, non-zero for error)
 */
ARGUS_API int argus_parse(argus_t *argus, int argc, char **argv);

/**
 * argus_free - Clean up and free resources
 *
 * @param argus  Argus context
 */
ARGUS_API void argus_free(argus_t *argus);

/**
 * Display functions
 */
ARGUS_API void argus_print_help(argus_t argus);
ARGUS_API void argus_print_usage(argus_t argus);
ARGUS_API void argus_print_version(argus_t argus);

/**
 * argus_is_set - Check if an option was set on the command line
 *
 * @param argus        Argus context
 * @param option_path  Option path (name or subcommand.name format)
 *
 * @return true if the option was set, false otherwise
 */
ARGUS_API bool argus_is_set(argus_t argus, const char *option_path);

/**
 * argus_get - Get the value of an option
 *
 * @param argus        Argus context
 * @param option_path  Option path (name or subcommand.name format)
 *
 * @return Value of the option, or {0} if not found
 */
ARGUS_API argus_value_t argus_get(argus_t argus, const char *option_path);

/**
 * argus_count - Get the number of values for an option
 *
 * @param argus        Argus context
 * @param option_path  Option path (name or subcommand.name format)
 *
 * @return Number of values for the option
 */
ARGUS_API size_t argus_count(argus_t argus, const char *option_path);

/**
 * argus_has_command - Check if a subcommand was parsed
 *
 * @param argus  Argus context
 *
 * @return true if a subcommand was parsed, false otherwise
 */
ARGUS_API bool argus_has_command(argus_t argus);

/**
 * argus_exec - Execute the parsed subcommand
 *
 * @param argus  Argus context
 * @param data   Data to pass to the subcommand action
 *
 * @return Status code (0 for success, non-zero for error)
 */
ARGUS_API int argus_exec(argus_t *argus, void *data);

/**
 * argus_array_get - Get an element from an array option at the specified index
 *
 * @param argus        Argus context
 * @param option_path  Option path (name or subcommand.name format)
 * @param index        Index of the element to retrieve
 *
 * @return Value of the element at the specified index, or {0} if not found or index out of bounds
 */
ARGUS_API argus_value_t argus_array_get(argus_t argus, const char *option_path, size_t index);

/**
 * argus_map_get - Get a value from a map option with the specified key
 *
 * @param argus        Argus context
 * @param option_path  Option path (name or subcommand.name format)
 * @param key          Key to look up in the map
 *
 * @return Value associated with the key, or {0} if not found
 */
ARGUS_API argus_value_t argus_map_get(argus_t argus, const char *option_path, const char *key);

/**
 * argus_array_it - Create an iterator for efficiently traversing an array option
 *
 * @param argus        Argus context
 * @param option_path  Option path (name or subcommand.name format)
 *
 * @return Iterator structure for the array, with count=0 if option not found
 */
ARGUS_API argus_array_it_t argus_array_it(argus_t argus, const char *option_path);

/**
 * argus_array_next - Get the next element from an array iterator
 *
 * @param it      Array iterator
 *
 * @return true if a value was retrieved, false if end of array
 */
ARGUS_API bool argus_array_next(argus_array_it_t *it);

/**
 * argus_array_reset - Reset an array iterator to the beginning
 *
 * @param it  Array iterator to reset
 */
ARGUS_API void argus_array_reset(argus_array_it_t *it);

/**
 * argus_map_it - Create an iterator for efficiently traversing a map option
 *
 * @param argus        Argus context
 * @param option_path  Option path (name or subcommand.name format)
 *
 * @return Iterator structure for the map, with count=0 if option not found
 */
ARGUS_API argus_map_it_t argus_map_it(argus_t argus, const char *option_path);

/**
 * argus_map_next - Get the next key-value pair from a map iterator
 *
 * @param it    Map iterator
 *
 * @return true if a pair was retrieved, false if end of map
 */
ARGUS_API bool argus_map_next(argus_map_it_t *it);

/**
 * argus_map_reset - Reset a map iterator to the beginning
 *
 * @param it  Map iterator to reset
 */
ARGUS_API void argus_map_reset(argus_map_it_t *it);

#endif /* ARGUS_API_H */
