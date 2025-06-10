/**
 * help_renderer.c - Functions for rendering different types of help elements
 *
 * This file implements the rendering logic for help display.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#define _GNU_SOURCE  // NOLINT

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argus/internal/compiler.h"
#include "argus/internal/cross_platform.h"
#include "argus/internal/display.h"
#include "argus/internal/help_formatter.h"
#include "argus/internal/help_renderer.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

void print_option(const argus_option_t *option, size_t indent)
{
    size_t name_width = print_option_name(option, indent);

    // Calculate padding for description alignment
    size_t padding = (DESCRIPTION_COLUMN > name_width) ? (DESCRIPTION_COLUMN - name_width) : 2;

    print_option_description(option, padding);
}

void print_positional(const argus_option_t *option, size_t indent)
{
    size_t name_len = 0;

    // Print indent
    for (size_t i = 0; i < indent; ++i) {
        printf(" ");
        name_len++;
    }

    // Print name with appropriate brackets based on required status
    if (option->flags & FLAG_REQUIRED) {
        printf("<%s>", option->name);
        name_len += strlen(option->name) + 2;  // < and >
    } else {
        printf("[%s]", option->name);
        name_len += strlen(option->name) + 2;  // [ and ]
    }

    // Calculate padding for description alignment
    size_t padding = (DESCRIPTION_COLUMN > name_len) ? (DESCRIPTION_COLUMN - name_len) : 2;

    print_option_description(option, padding);
}

void print_subcommand(argus_t *argus, const argus_option_t *option, size_t indent)
{
    UNUSED(argus);
    size_t name_len = 0;

    // Print indent
    for (size_t i = 0; i < indent; ++i) {
        printf(" ");
        name_len++;
    }

    // Print subcommand name
    printf("%s", option->name);
    name_len += strlen(option->name);

    // Calculate padding for description alignment
    size_t padding = (DESCRIPTION_COLUMN > name_len) ? (DESCRIPTION_COLUMN - name_len) : 2;

    // Use the common description printing function
    print_option_description(option, padding);
}

void print_option_list(option_entry_t *list, size_t indent)
{
    option_entry_t *current = list;
    while (current != NULL) {
        print_option(current->option, indent);
        current = current->next;
    }
}

void print_positional_list(option_entry_t *list, size_t indent)
{
    option_entry_t *current = list;
    while (current != NULL) {
        print_positional(current->option, indent);
        current = current->next;
    }
}

void print_subcommand_list(argus_t *argus, option_entry_t *list, size_t indent)
{
    option_entry_t *current = list;
    while (current != NULL) {
        print_subcommand(argus, current->option, indent);
        current = current->next;
    }
}

void print_help_sections(argus_t *argus, help_data_t *data)
{
    // Print positional arguments
    if (has_entries(data->positionals)) {
        printf("\nArguments:\n");
        print_positional_list(data->positionals, OPTION_INDENT);
    }

    // Print groups of options
    if (has_groups(data->groups)) {
        group_info_t *group = data->groups;
        while (group != NULL) {
            // Only print non-empty groups
            if (group->options != NULL) {
                printf("\n%s:\n", group->description ? group->description : group->name);
                print_option_list(group->options, OPTION_INDENT);
            }
            group = group->next;
        }
    }

    // Print ungrouped options
    if (has_entries(data->ungrouped)) {
        printf("\nOptions:\n");
        print_option_list(data->ungrouped, OPTION_INDENT);
    }

    // Print subcommands
    if (has_entries(data->subcommands)) {
        printf("\nCommands:\n");
        print_subcommand_list(argus, data->subcommands, OPTION_INDENT);

        printf("\nRun '%s", argus->program_name);
        for (size_t i = 0; i < argus->subcommand_depth; ++i)
            printf(" %s", argus->subcommand_stack[i]->name);
        printf(" COMMAND --help' for more information on a command.\n");
    }
}