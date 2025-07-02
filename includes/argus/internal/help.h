/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ARGUS_INTERNAL_HELP_H
#define ARGUS_INTERNAL_HELP_H

#include "argus/types.h"

// =============================================================================
// HELP ORGANIZER - Data structures and organization functions
// =============================================================================

/**
 * Option entry structure for linked lists
 */
typedef struct option_entry_s
{
    const argus_option_t  *option;
    struct option_entry_s *next;
} option_entry_t;

/**
 * Group information structure
 */
typedef struct group_info_s
{
    const char          *name;
    const char          *description;
    option_entry_t      *options;
    struct group_info_s *next;
} group_info_t;

/**
 * Help data organization structure
 */
typedef struct help_data_s
{
    group_info_t   *groups;       // Linked list of option groups
    option_entry_t *ungrouped;    // Ungrouped options
    option_entry_t *positionals;  // Positional arguments
    option_entry_t *subcommands;  // Subcommands
} help_data_t;

/**
 * Create a new option entry
 *
 * @param option  Option to create entry for
 * @return        New option entry or NULL on failure
 */
option_entry_t *create_option_entry(const argus_option_t *option);

/**
 * Add an option to a linked list
 *
 * @param list    Pointer to list head
 * @param option  Option to add
 */
void add_option_to_list(option_entry_t **list, const argus_option_t *option);

/**
 * Find or create a group in help data
 *
 * @param data         Help data structure
 * @param name         Group name
 * @param description  Group description
 * @return             Group info structure or NULL on failure
 */
group_info_t *find_or_create_group(help_data_t *data, const char *name, const char *description);

/**
 * Organize options into categories for display
 *
 * @param options  Array of options to organize
 * @param data     Help data structure to populate
 */
void organize_options(const argus_option_t *options, help_data_t *data);

/**
 * Free all help data structures
 *
 * @param data  Help data to free
 */
void free_help_data(help_data_t *data);

/**
 * Check if a list has entries
 *
 * @param list  List to check
 * @return      true if list has entries
 */
bool has_entries(option_entry_t *list);

/**
 * Check if there are groups
 *
 * @param groups  Groups to check
 * @return        true if there are groups
 */
bool has_groups(group_info_t *groups);

// =============================================================================
// HELP FORMATTER - Smart Hint and formatting functions
// =============================================================================

/**
 * Get base type name for value hints
 *
 * @param type  Value type
 * @return      Type name string
 */
const char *get_base_type_name(argus_valtype_t type);

/**
 * Get collection format pattern
 *
 * @param type  Value type
 * @return      Format pattern or NULL if not a collection
 */
const char *get_collection_format(argus_valtype_t type);

/**
 * Format a collection hint
 *
 * @param format     Format pattern
 * @param type_name  Base type name
 * @return           Formatted hint string (static buffer)
 */
char *format_collection_hint(const char *format, const char *type_name);

/**
 * Print wrapped text with proper indentation
 *
 * @param text        Text to print
 * @param indent      Base indentation
 * @param line_width  Maximum line width
 */
void print_wrapped_text(const char *text, size_t indent, size_t line_width);

/**
 * Print option name (short and long forms with hints)
 *
 * @param argus   Argus context
 * @param option  Option to print name for
 * @param indent  Indentation level
 * @return        Total width of printed name
 */
size_t print_option_name(argus_t *argus, const argus_option_t *option, size_t indent);

/**
 * Build description text for an option (including defaults, attributes, etc.)
 *
 * @param argus   Argus context
 * @param option  Option to build description for
 * @return        Allocated description string - caller must free
 */
char *build_option_description(argus_t *argus, const argus_option_t *option);

/**
 * Get smart hint for an option (for testing)
 *
 * @param argus   Argus context
 * @param option  Option to get hint for
 * @return        Allocated hint string - caller must free
 */
char *get_smart_hint(argus_t *argus, const argus_option_t *option);

/**
 * Check if a hint is short (for testing)
 *
 * @param argus  Argus context
 * @param hint   Hint to check
 * @return       true if hint is short and simple
 */
bool is_short_hint(argus_t *argus, const char *hint);

/**
 * Check if option has single validator (for testing)
 *
 * @param option  Option to check
 * @return        true if option has exactly one validator
 */
bool has_single_validator(const argus_option_t *option);

/**
 * Print option description with proper alignment and wrapping
 *
 * @param argus    Argus context
 * @param option   Option to print description for
 * @param padding  Padding before description
 */
void print_option_description(argus_t *argus, const argus_option_t *option, size_t padding);

// =============================================================================
// HELP RENDERER - Rendering and display functions
// =============================================================================

/**
 * Print a single option
 *
 * @param argus   Argus context
 * @param option  Option to print
 * @param indent  Indentation level
 */
void print_option(argus_t *argus, const argus_option_t *option, size_t indent);

/**
 * Print a positional argument
 *
 * @param argus   Argus context
 * @param option  Positional option to print
 * @param indent  Indentation level
 */
void print_positional(argus_t *argus, const argus_option_t *option, size_t indent);

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
 * @param argus   Argus context
 * @param list    List of option entries
 * @param indent  Indentation level
 */
void print_option_list(argus_t *argus, option_entry_t *list, size_t indent);

/**
 * Print a list of positional arguments
 *
 * @param argus   Argus context
 * @param list    List of positional entries
 * @param indent  Indentation level
 */
void print_positional_list(argus_t *argus, option_entry_t *list, size_t indent);

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

#endif /* ARGUS_INTERNAL_HELP_H */
