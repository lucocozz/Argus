/**
 * argus/internal/help_renderer.h - Help rendering functions
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines functions for rendering different types of help elements.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_INTERNAL_HELP_RENDERER_H
#define ARGUS_INTERNAL_HELP_RENDERER_H

#include "argus/internal/help_organizer.h"
#include "argus/types.h"

/**
 * Print a single option
 *
 * @param option  Option to print
 * @param indent  Indentation level
 */
void print_option(const argus_option_t *option, size_t indent);

/**
 * Print a positional argument
 *
 * @param option  Positional option to print
 * @param indent  Indentation level
 */
void print_positional(const argus_option_t *option, size_t indent);

/**
 * Print a subcommand
 *
 * @param argus   Argus context
 * @param option  Subcommand option to print
 * @param indent  Indentation level
 */
void print_subcommand(argus_t *argus, const argus_option_t *option, size_t indent);

/**
 * Print a list of options
 *
 * @param list    List of option entries
 * @param indent  Indentation level
 */
void print_option_list(option_entry_t *list, size_t indent);

/**
 * Print a list of positional arguments
 *
 * @param list    List of positional entries
 * @param indent  Indentation level
 */
void print_positional_list(option_entry_t *list, size_t indent);

/**
 * Print a list of subcommands
 *
 * @param argus   Argus context
 * @param list    List of subcommand entries
 * @param indent  Indentation level
 */
void print_subcommand_list(argus_t *argus, option_entry_t *list, size_t indent);

/**
 * Print all help sections
 *
 * @param argus  Argus context
 * @param data   Organized help data
 */
void print_help_sections(argus_t *argus, help_data_t *data);

#endif /* ARGUS_INTERNAL_HELP_RENDERER_H */