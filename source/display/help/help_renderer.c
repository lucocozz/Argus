/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#define _GNU_SOURCE  // NOLINT

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argus/internal/help.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

void print_option(argus_t *argus, const argus_option_t *option, size_t indent)
{
    size_t name_width = print_option_name(argus, option, indent);

    size_t padding = 0;
    if (argus->helper.config.description_column > name_width)
        padding = argus->helper.config.description_column - name_width;
    else
        padding = 2;

    print_option_description(argus, option, padding);
}

void print_positional(argus_t *argus, const argus_option_t *option, size_t indent)
{
    size_t name_len = putnchar(' ', indent);

    if (option->flags & FLAG_REQUIRED) {
        if (option->value_type & VALUE_TYPE_VARIADIC) {
            printf("<%s...>", option->name);
            name_len += strlen(option->name) + 5;  // <...>
        } else {
            printf("<%s>", option->name);
            name_len += strlen(option->name) + 2;  // <>
        }
    } else {
        if (option->value_type & VALUE_TYPE_VARIADIC) {
            printf("[%s...]", option->name);
            name_len += strlen(option->name) + 5;  // [...]
        } else {
            printf("[%s]", option->name);
            name_len += strlen(option->name) + 2;  // []
        }
    }

    size_t padding = 0;
    if (argus->helper.config.description_column > name_len)
        padding = argus->helper.config.description_column - name_len;
    else
        padding = 2;

    print_option_description(argus, option, padding);
}

void print_subcommand(argus_t *argus, const argus_option_t *option, size_t indent)
{
    size_t name_len = putnchar(' ', indent);

    printf("%s", option->name);
    name_len += strlen(option->name);

    size_t padding = 0;
    if (argus->helper.config.description_column > name_len)
        padding = argus->helper.config.description_column - name_len;
    else
        padding = 2;

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
    if (has_entries(data->positionals)) {
        printf("\nArguments:\n");
        print_positional_list(argus, data->positionals, argus->helper.config.option_indent);
    }

    if (has_groups(data->groups)) {
        group_info_t *group = data->groups;
        while (group != NULL) {
            if (group->options != NULL) {
                printf("\n%s:\n", group->name);
                print_option_list(argus, group->options, argus->helper.config.option_indent);
            }
            group = group->next;
        }
    }

    if (has_entries(data->ungrouped)) {
        printf("\nOptions:\n");
        print_option_list(argus, data->ungrouped, argus->helper.config.option_indent);
    }

    if (has_entries(data->subcommands)) {
        printf("\nCommands:\n");
        print_subcommand_list(argus, data->subcommands, argus->helper.config.option_indent);

        printf("\nRun '%s", argus->program_name);
        for (size_t i = 0; i < argus->subcommand_depth; ++i)
            printf(" %s", argus->subcommand_stack[i]->name);
        printf(" COMMAND --help' for more information on a command.\n");
    }
}
