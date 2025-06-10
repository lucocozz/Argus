/**
 * argus/internal/help_organizer.h - Help organization functions
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines functions for organizing options into display categories.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_INTERNAL_HELP_ORGANIZER_H
#define ARGUS_INTERNAL_HELP_ORGANIZER_H

#include "argus/types.h"

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

#endif /* ARGUS_INTERNAL_HELP_ORGANIZER_H */