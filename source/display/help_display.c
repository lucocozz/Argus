/**
 * help_display.c - Functions for displaying help information
 *
 * This file implements the help display functionality for the argus library.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#define _GNU_SOURCE  // NOLINT

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argus/internal/cross_platform.h"
#include "argus/internal/display.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

/*
 * Helper data structures
 */

typedef struct option_entry_s
{
    const argus_option_t  *option;
    struct option_entry_s *next;
} option_entry_t;

typedef struct group_info_s
{
    const char          *name;
    const char          *description;
    option_entry_t      *options;
    struct group_info_s *next;
} group_info_t;

typedef struct help_data_s
{
    group_info_t   *groups;       // Linked list of option groups
    option_entry_t *ungrouped;    // Ungrouped options
    option_entry_t *positionals;  // Positional arguments
    option_entry_t *subcommands;  // Subcommands
} help_data_t;

/*
 * Memory management functions
 */

static option_entry_t *create_option_entry(const argus_option_t *option)
{
    option_entry_t *entry = malloc(sizeof(option_entry_t));
    if (!entry)
        return NULL;

    entry->option = option;
    entry->next   = NULL;
    return entry;
}

static void add_option_to_list(option_entry_t **list, const argus_option_t *option)
{
    option_entry_t *entry = create_option_entry(option);
    if (!entry)
        return;

    if (*list == NULL) {
        *list = entry;
    } else {
        option_entry_t *current = *list;
        while (current->next != NULL)
            current = current->next;
        current->next = entry;
    }
}

static group_info_t *find_or_create_group(help_data_t *data, const char *name,
                                          const char *description)
{
    // First, look for existing group
    group_info_t *group = data->groups;
    while (group != NULL) {
        if (strcmp(group->name, name) == 0)
            return group;
        group = group->next;
    }

    // Create new group
    group = malloc(sizeof(group_info_t));
    if (!group)
        return NULL;

    group->name        = name;
    group->description = description;
    group->options     = NULL;
    group->next        = NULL;

    // Add to list
    if (data->groups == NULL) {
        data->groups = group;
    } else {
        group_info_t *current = data->groups;
        while (current->next != NULL)
            current = current->next;
        current->next = group;
    }

    return group;
}

static void free_help_data(help_data_t *data)
{
    // Free groups and their options
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

    // Free ungrouped options
    option_entry_t *option = data->ungrouped;
    while (option != NULL) {
        option_entry_t *next = option->next;
        free(option);
        option = next;
    }

    // Free positionals
    option = data->positionals;
    while (option != NULL) {
        option_entry_t *next = option->next;
        free(option);
        option = next;
    }

    // Free subcommands
    option = data->subcommands;
    while (option != NULL) {
        option_entry_t *next = option->next;
        free(option);
        option = next;
    }
}

/*
 * Option organizing function
 */

static void organize_options(const argus_option_t *options, help_data_t *data)
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
                group              = NULL;  // Will be created on demand when options are added
                break;

            case TYPE_OPTION:
                if (option->flags & FLAG_HIDDEN)
                    continue;

                if (current_group != NULL) {
                    // Ensure group exists
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

/*
 * Helper functions for formatting
 */

// Helper function to get the base type name without collection indicators
static const char *get_base_type_name(argus_valtype_t type)
{
    switch (type) {
        case VALUE_TYPE_INT:
            return "NUM";
        case VALUE_TYPE_STRING:
            return "STR";
        case VALUE_TYPE_FLOAT:
            return "FLOAT";
        case VALUE_TYPE_BOOL:
            return "BOOL";
        case VALUE_TYPE_FLAG:
            return "FLAG";
        default:
            return "VALUE";
    }
}

// Helper function to get the collection format pattern
static const char *get_collection_format(argus_valtype_t type)
{
    if (type & VALUE_TYPE_ARRAY) {
        return "%s,...";
    }
    if (type & VALUE_TYPE_MAP)
        return "KEY=%s,...";
    return NULL;
}

// Helper function to format a collection hint
static char *format_collection_hint(const char *format, const char *type_name)
{
    static char buffer[64];  // Buffer for the formatted string

    snprintf(buffer, sizeof(buffer), format, type_name);
    return buffer;
}

static void print_value_hint(const argus_option_t *option)
{
    if (option->value_type == VALUE_TYPE_FLAG)
        return;  // No hint for boolean flags

    // Get the base type name or hint
    const char *type_name;
    if (option->hint) {
        type_name = option->hint;
    } else {
        type_name = get_base_type_name(option->value_type);
    }

    // Get the collection format if applicable
    const char *collection_format = get_collection_format(option->value_type);

    // Print the formatted hint
    printf(" <%s>",
           collection_format ? format_collection_hint(collection_format, type_name) : type_name);
}

static void print_wrapped_text(const char *text, size_t indent, size_t line_width)
{
    if (!text || !*text)
        return;

    size_t text_len    = strlen(text);
    size_t line_start  = 0;
    size_t current_pos = 0;
    size_t last_space  = 0;

    // Print initial indent for first line (assumed to be already printed by caller)

    while (current_pos < text_len) {
        // Find the next potential line break (space or existing newline)
        while (current_pos < text_len &&
               current_pos - line_start < line_width - indent - 2 &&  // Consider "- " prefix
               text[current_pos] != '\n') {
            if (text[current_pos] == ' ')
                last_space = current_pos;
            current_pos++;
        }

        // Check if we found a natural break or need to force one
        if (current_pos < text_len && text[current_pos] == '\n') {
            // Natural newline in the text
            printf("%.*s\n", (int)(current_pos - line_start), text + line_start);
            current_pos++;  // Skip the newline
            line_start = current_pos;

            // Print indent for continuation line
            if (current_pos < text_len) {
                for (size_t i = 0; i < indent; i++)
                    printf(" ");
                // No marker for continuation lines, just 2 spaces for alignment
                printf("  ");
            }
        } else if (current_pos - line_start >= line_width - indent - 2) {
            // Line too long, break at last space if found
            if (last_space > line_start) {
                printf("%.*s\n", (int)(last_space - line_start), text + line_start);
                line_start = last_space + 1;  // Skip the space
            } else {
                // No space found, forced break in the middle of a word
                printf("%.*s\n", (int)(current_pos - line_start), text + line_start);
                line_start = current_pos;
            }

            // Print indent for continuation line
            if (line_start < text_len) {
                for (size_t i = 0; i < indent; i++)
                    printf(" ");
                // No marker for continuation lines, just 2 spaces for alignment
                printf("  ");
            }
        } else {
            // End of text reached without needing to wrap
            printf("%s", text + line_start);
            break;
        }

        // Reset for next line
        current_pos = line_start;
        last_space  = line_start;
    }
}

/*
 * Print functions for different option types
 */

static void print_option_description(const argus_option_t *option, size_t padding)
{
    // Determine where description starts
    size_t description_indent = DESCRIPTION_COLUMN;

    // If option name is too long, move to next line
    if (padding < 4) {
        printf("\n");
        // Indent to description column
        for (size_t i = 0; i < description_indent; ++i)
            printf(" ");
        // Add visual marker
        printf("- ");
    } else {
        // Otherwise, add calculated padding to align description
        for (size_t i = 0; i < padding; ++i)
            printf(" ");
        // Add visual marker
        printf("- ");
    }

    // Start with help text
    char *description = NULL;

    if (option->help) {
        description = safe_strdup(option->help);
        if (!description) {
            printf("Error: Memory allocation failed\n");
            return;
        }
    } else {
        description = safe_strdup("");
        if (!description) {
            printf("Error: Memory allocation failed\n");
            return;
        }
    }

    // // Append choices if any
    // if (option->choices_count > 0) {
    //     char choices_buf[256] = {0};
    //     snprintf(choices_buf, sizeof(choices_buf), " [");

    //     for (size_t i = 0; i < option->choices_count; ++i) {
    //         char item[64] = {0};

    //         if (i > 0)
    //             safe_strncat(choices_buf, sizeof(choices_buf), ", ",
    //                          sizeof(choices_buf) - strlen(choices_buf) - 1);

    //         switch (option->value_type) {
    //             case VALUE_TYPE_INT:
    //                 snprintf(item, sizeof(item), "%lld", option->choices.as_array_int[i]);
    //                 break;
    //             case VALUE_TYPE_STRING:
    //                 snprintf(item, sizeof(item), "%s", option->choices.as_array_string[i]);
    //                 break;
    //             case VALUE_TYPE_FLOAT:
    //                 snprintf(item, sizeof(item), "%.2f", option->choices.as_array_float[i]);
    //                 break;
    //             default:
    //                 break;
    //         }

    //         safe_strncat(choices_buf, sizeof(choices_buf), item,
    //                      sizeof(choices_buf) - strlen(choices_buf) - 1);
    //     }

    //     safe_strncat(choices_buf, sizeof(choices_buf), "]",
    //                  sizeof(choices_buf) - strlen(choices_buf) - 1);

    //     // Allocate new buffer for combined description
    //     size_t desc_size = strlen(description) + strlen(choices_buf) + 1;
    //     char  *new_desc  = malloc(desc_size);
    //     if (new_desc) {
    //         safe_strcpy(new_desc, desc_size, description);
    //         safe_strcat(new_desc, desc_size, choices_buf);
    //         free(description);
    //         description = new_desc;
    //     }
    // }

    // Append default value if any
    if (option->have_default && option->value_type != VALUE_TYPE_FLAG) {
        char default_buf[128] = {0};
        snprintf(default_buf, sizeof(default_buf), " (default: ");
        size_t default_len = strlen(default_buf);

        switch (option->value_type) {
            case VALUE_TYPE_INT:
                snprintf(default_buf + default_len, sizeof(default_buf) - default_len, "%d)",
                         option->default_value.as_int);
                break;
            case VALUE_TYPE_STRING:
                if (option->default_value.as_string) {
                    snprintf(default_buf + default_len, sizeof(default_buf) - default_len,
                             "\"%s\")", option->default_value.as_string);
                } else {
                    snprintf(default_buf + default_len, sizeof(default_buf) - default_len, "null)");
                }
                break;
            case VALUE_TYPE_FLOAT:
                snprintf(default_buf + default_len, sizeof(default_buf) - default_len, "%.2f)",
                         option->default_value.as_float);
                break;
            case VALUE_TYPE_BOOL:
                snprintf(default_buf + default_len, sizeof(default_buf) - default_len, "%s)",
                         option->default_value.as_bool ? "true" : "false");
                break;
            default:
                safe_strcat(default_buf, sizeof(default_buf), ")");
                break;
        }

        // Allocate new buffer for combined description
        size_t desc_size = strlen(description) + strlen(default_buf) + 1;
        char  *new_desc  = malloc(desc_size);
        if (new_desc) {
            safe_strcpy(new_desc, desc_size, description);
            safe_strcat(new_desc, desc_size, default_buf);
            free(description);
            description = new_desc;
        }
    }

    // Append additional attributes
    if (option->flags & FLAG_EXIT || option->flags & FLAG_REQUIRED ||
        option->flags & FLAG_DEPRECATED || option->flags & FLAG_EXPERIMENTAL) {
        char attrs_buf[32] = {0};

        if (option->flags & FLAG_EXIT)
            safe_strcat(attrs_buf, sizeof(attrs_buf), " (exit)");
        if (option->flags & FLAG_REQUIRED)
            safe_strcat(attrs_buf, sizeof(attrs_buf), " (required)");
        if (option->flags & FLAG_DEPRECATED)
            safe_strcat(attrs_buf, sizeof(attrs_buf), " (deprecated)");
        if (option->flags & FLAG_EXPERIMENTAL)
            safe_strcat(attrs_buf, sizeof(attrs_buf), " (experimental)");

        // Allocate new buffer for combined description
        size_t desc_size = strlen(description) + strlen(attrs_buf) + 1;
        char  *new_desc  = malloc(desc_size);
        if (new_desc) {
            safe_strcpy(new_desc, desc_size, description);
            safe_strcat(new_desc, desc_size, attrs_buf);
            free(description);
            description = new_desc;
        }
    }

    // Print the wrapped description
    if (strlen(description) > 0)
        print_wrapped_text(description, description_indent, MAX_LINE_WIDTH);

    free(description);
    printf("\n");
}

static size_t print_option_name(const argus_option_t *option, size_t indent)
{
    size_t name_len = 0;

    // Print indent
    for (size_t i = 0; i < indent; ++i) {
        printf(" ");
        name_len++;
    }

    // Print short option name if available
    if (option->sname) {
        printf("-%c", option->sname);
        name_len += 2;  // "-a"

        if (option->lname) {
            printf(", ");
            name_len += 2;  // ", "
        }
    }

    // Print long option name if available
    if (option->lname) {
        printf("--%s", option->lname);
        name_len += 2 + strlen(option->lname);  // "--option"
    }

    // Print value hint
    if (option->value_type != VALUE_TYPE_FLAG) {
        print_value_hint(option);

        // Calculate hint length for correct padding
        const char *type_name;
        if (option->hint) {
            type_name = option->hint;
        } else {
            type_name = get_base_type_name(option->value_type);
        }

        // Get the collection format if applicable
        const char *collection_format = get_collection_format(option->value_type);

        // Calculate length based on whether it's a collection or not
        if (collection_format) {
            // Approximate the length for collection format
            // Format is "KEY=%s,..." or "%s,..."
            const char *format_str = format_collection_hint(collection_format, type_name);
            name_len += 3 + strlen(format_str);  // " <hint_format>"
        } else {
            name_len += 3 + strlen(type_name);  // " <hint>"
        }
    }

    return name_len;
}

static void print_option(const argus_option_t *option, size_t indent)
{
    size_t name_width = print_option_name(option, indent);

    // Calculate padding for description alignment
    size_t padding = (DESCRIPTION_COLUMN > name_width) ? (DESCRIPTION_COLUMN - name_width) : 2;

    print_option_description(option, padding);
}

static void print_positional(const argus_option_t *option, size_t indent)
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

static void print_subcommand(argus_t *argus, const argus_option_t *option, size_t indent)
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

/*
 * List printing functions
 */

static void print_option_list(option_entry_t *list, size_t indent)
{
    option_entry_t *current = list;
    while (current != NULL) {
        print_option(current->option, indent);
        current = current->next;
    }
}

static void print_positional_list(option_entry_t *list, size_t indent)
{
    option_entry_t *current = list;
    while (current != NULL) {
        print_positional(current->option, indent);
        current = current->next;
    }
}

static void print_subcommand_list(argus_t *argus, option_entry_t *list, size_t indent)
{
    option_entry_t *current = list;
    while (current != NULL) {
        print_subcommand(argus, current->option, indent);
        current = current->next;
    }
}

static bool has_entries(option_entry_t *list)
{
    return list != NULL;
}

static bool has_groups(group_info_t *groups)
{
    return groups != NULL;
}

static void print_help_sections(argus_t *argus, help_data_t *data)
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
        for (size_t i = 0; i < argus->context.subcommand_depth; ++i)
            printf(" %s", argus->context.subcommand_stack[i]->name);
        printf(" COMMAND --help' for more information on a command.\n");
    }
}

/*
 * Main help display function
 */

void display_help(argus_t *argus, const argus_option_t *command)
{
    if (command == NULL)
        command = get_active_options(argus);

    // Initialize help data
    help_data_t data = {NULL, NULL, NULL, NULL};

    // Organize options into appropriate categories
    organize_options(command, &data);

    // Print the help sections
    print_help_sections(argus, &data);

    // Clean up
    free_help_data(&data);

    printf("\n");
}
