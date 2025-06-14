/**
 * help_organizer.c - Functions for organizing options into display categories
 *
 * This file implements the organization logic for help display.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#define _GNU_SOURCE  // NOLINT

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "argus/internal/help.h"
#include "argus/types.h"

option_entry_t *create_option_entry(const argus_option_t *option)
{
    option_entry_t *entry = malloc(sizeof(option_entry_t));
    if (!entry)
        return NULL;

    entry->option = option;
    entry->next   = NULL;
    return entry;
}

void add_option_to_list(option_entry_t **list, const argus_option_t *option)
{
    option_entry_t *entry = create_option_entry(option);
    if (!entry)
        return;

    if (*list == NULL)
        *list = entry;
    else {
        option_entry_t *current = *list;
        while (current->next != NULL)
            current = current->next;
        current->next = entry;
    }
}

group_info_t *find_or_create_group(help_data_t *data, const char *name, const char *description)
{
    group_info_t *group = data->groups;
    while (group != NULL) {
        if (strcmp(group->name, name) == 0)
            return group;
        group = group->next;
    }

    group = malloc(sizeof(group_info_t));
    if (!group)
        return NULL;

    group->name        = name;
    group->description = description;
    group->options     = NULL;
    group->next        = NULL;

    if (data->groups == NULL)
        data->groups = group;
    else {
        group_info_t *current = data->groups;
        while (current->next != NULL)
            current = current->next;
        current->next = group;
    }

    return group;
}

void organize_options(const argus_option_t *options, help_data_t *data)
{
    const char   *current_group      = NULL;
    const char   *current_group_desc = NULL;
    group_info_t *group              = NULL;

    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        const argus_option_t *option = &options[i];

        switch (option->type) {
            case TYPE_GROUP:
                current_group      = option->name;
                current_group_desc = option->help;
                group              = NULL;
                break;

            case TYPE_OPTION:
                if (option->flags & FLAG_HIDDEN)
                    continue;

                if (current_group != NULL) {
                    if (group == NULL)
                        group = find_or_create_group(data, current_group, current_group_desc);
                    add_option_to_list(&group->options, option);
                } else
                    add_option_to_list(&data->ungrouped, option);
                break;

            case TYPE_POSITIONAL:
                add_option_to_list(&data->positionals, option);
                break;

            case TYPE_SUBCOMMAND:
                add_option_to_list(&data->subcommands, option);
                break;

            default:
                break;
        }
    }
}

void free_help_data(help_data_t *data)
{
    group_info_t *group = data->groups;
    while (group != NULL) {
        option_entry_t *option = group->options;
        while (option != NULL) {
            option_entry_t *next = option->next;
            free(option);
            option = next;
        }

        group_info_t *next_group = group->next;
        free(group);
        group = next_group;
    }

    option_entry_t *option = data->ungrouped;
    while (option != NULL) {
        option_entry_t *next = option->next;
        free(option);
        option = next;
    }

    option = data->positionals;
    while (option != NULL) {
        option_entry_t *next = option->next;
        free(option);
        option = next;
    }

    option = data->subcommands;
    while (option != NULL) {
        option_entry_t *next = option->next;
        free(option);
        option = next;
    }
}

bool has_entries(option_entry_t *list)
{
    return list != NULL;
}

bool has_groups(group_info_t *groups)
{
    return groups != NULL;
}
