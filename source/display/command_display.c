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
#include <string.h>

#include "argus/internal/display.h"
#include "argus/types.h"

#define COMMAND_BUFFER_SIZE   512
#define FULL_PATH_BUFFER_SIZE 1024

static void print_command_line(const char *command_path, const char *description, argus_t *argus)
{
    printf("  %s", command_path);

    size_t padding  = 0;
    size_t name_len = strlen(command_path);
    if (argus->helper.config.description_column > name_len + 2)
        padding = argus->helper.config.description_column - name_len - 2;
    else
        padding = 2;

    for (size_t i = 0; i < padding; ++i)
        printf(" ");

    if (description != NULL)
        printf("- %s", description);

    printf("\n");
}

static void print_subcommand_paths(const argus_option_t *options, const char *base_path,
                                   argus_t *argus, bool *first_in_group)
{
    char path_buffer[COMMAND_BUFFER_SIZE];
    bool current_group_started = false;

    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        const argus_option_t *option = &options[i];

        if (option->type != TYPE_SUBCOMMAND)
            continue;

        if (!current_group_started && !*first_in_group)
            printf("\n");
        current_group_started = true;
        *first_in_group       = false;

        snprintf(path_buffer, sizeof(path_buffer), "%s %s", base_path, option->name);
        print_command_line(path_buffer, option->help, argus);

        if (option->sub_options != NULL) {
            char sub_path_buffer[COMMAND_BUFFER_SIZE];
            snprintf(sub_path_buffer, sizeof(sub_path_buffer), "%s %s", base_path, option->name);
            bool nested_first = true;
            print_subcommand_paths(option->sub_options, sub_path_buffer, argus, &nested_first);
        }
    }
}

void display_available_subcommands(argus_t *argus, const argus_option_t *command)
{
    printf("The '%s' command cannot be executed by itself.\n\n", command->name);
    printf("Available commands:\n");

    bool first_group = true;
    if (command->sub_options != NULL)
        print_subcommand_paths(command->sub_options, command->name, argus, &first_group);

    printf("\nRun '");
    printf("%s", argus->program_name);

    for (size_t i = 0; i < argus->subcommand_depth; ++i)
        printf(" %s", argus->subcommand_stack[i]->name);

    printf(" SUBCOMMAND --help' for more information.\n");
}

static bool has_executable_subcommands(const argus_option_t *option)
{
    if (option->sub_options == NULL)
        return false;

    for (int i = 0; option->sub_options[i].type != TYPE_NONE; ++i) {
        const argus_option_t *sub_option = &option->sub_options[i];

        if (sub_option->type == TYPE_SUBCOMMAND && sub_option->action != NULL)
            return true;
    }

    return false;
}

static bool is_effectively_simple_command(const argus_option_t *option)
{
    return (option->type == TYPE_SUBCOMMAND && option->action != NULL &&
            !has_executable_subcommands(option));
}

static bool print_simple_commands_group(argus_t *argus)
{
    bool printed_any = false;

    for (int i = 0; argus->options[i].type != TYPE_NONE; ++i) {
        const argus_option_t *option = &argus->options[i];

        if (is_effectively_simple_command(option)) {
            char command_path[COMMAND_BUFFER_SIZE];
            snprintf(command_path, sizeof(command_path), "%s %s", argus->program_name,
                     option->name);
            print_command_line(command_path, option->help, argus);
            printed_any = true;
        }
    }

    return printed_any;
}

static void print_command_families(argus_t *argus, bool printed_simple_commands)
{
    bool first_family = true;

    for (int i = 0; argus->options[i].type != TYPE_NONE; ++i) {
        const argus_option_t *option = &argus->options[i];

        if (option->type != TYPE_SUBCOMMAND || !has_executable_subcommands(option))
            continue;

        if (printed_simple_commands || !first_family)
            printf("\n");

        first_family = false;

        char family_base_path[COMMAND_BUFFER_SIZE];
        snprintf(family_base_path, sizeof(family_base_path), "%s %s", argus->program_name,
                 option->name);

        if (option->action != NULL)
            print_command_line(family_base_path, option->help, argus);

        for (int j = 0; option->sub_options[j].type != TYPE_NONE; ++j) {
            const argus_option_t *sub_option = &option->sub_options[j];

            if (sub_option->type != TYPE_SUBCOMMAND || sub_option->action == NULL)
                continue;

            char full_command_path[FULL_PATH_BUFFER_SIZE];
            snprintf(full_command_path, sizeof(full_command_path), "%s %s", family_base_path,
                     sub_option->name);
            print_command_line(full_command_path, sub_option->help, argus);
        }

        printed_simple_commands = false;
    }
}

void display_all_commands(argus_t *argus)
{
    printf("No command specified.\n\n");
    printf("Available commands:\n");

    if (argus->options != NULL) {
        bool printed_simple_commands = print_simple_commands_group(argus);
        print_command_families(argus, printed_simple_commands);
    }

    printf("\nRun '%s COMMAND --help' for more information on a specific command.\n",
           argus->program_name);
}
