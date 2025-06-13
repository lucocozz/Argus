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
#include "argus/internal/help.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

void print_option(argus_t *argus, const argus_option_t *option, size_t indent)
{
    size_t name_width = print_option_name(argus, option, indent);

    // Calculate padding for description alignment
    size_t padding = (argus->helper.config.description_column > name_width) ? (argus->helper.config.description_column - name_width) : 2;

    print_option_description(argus, option, padding);
}

void print_positional(argus_t *argus, const argus_option_t *option, size_t indent)
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
    size_t padding = (argus->helper.config.description_column > name_len) ? (argus->helper.config.description_column - name_len) : 2;

    print_option_description(argus, option, padding);
}

void print_subcommand(argus_t *argus, const argus_option_t *option, size_t indent)
{
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
    size_t padding = (argus->helper.config.description_column > name_len) ? (argus->helper.config.description_column - name_len) : 2;

    // Use the common description printing function
    print_option_description(argus, option, padding);
}

void print_option_list(argus_t *argus, option_entry_t *list, size_t indent)
{
    option_entry_t *current = list;
    while (current != NULL) {
        print_option(argus, current->option, indent);
        current = current->next;
    }
}

void print_positional_list(argus_t *argus, option_entry_t *list, size_t indent)
{
    option_entry_t *current = list;
    while (current != NULL) {
        print_positional(argus, current->option, indent);
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
        print_positional_list(argus, data->positionals, argus->helper.config.option_indent);
    }

    // Print groups of options
    if (has_groups(data->groups)) {
        group_info_t *group = data->groups;
        while (group != NULL) {
            // Only print non-empty groups
            if (group->options != NULL) {
                printf("\n%s:\n", group->description ? group->description : group->name);
                print_option_list(argus, group->options, argus->helper.config.option_indent);
            }
            group = group->next;
        }
    }

    // Print ungrouped options
    if (has_entries(data->ungrouped)) {
        printf("\nOptions:\n");
        print_option_list(argus, data->ungrouped, argus->helper.config.option_indent);
    }

    // Print subcommands
    if (has_entries(data->subcommands)) {
        printf("\nCommands:\n");
        print_subcommand_list(argus, data->subcommands, argus->helper.config.option_indent);

        printf("\nRun '%s", argus->program_name);
        for (size_t i = 0; i < argus->subcommand_depth; ++i)
            printf(" %s", argus->subcommand_stack[i]->name);
        printf(" COMMAND --help' for more information on a command.\n");
    }
}