/**
 * help_formatter.c - Functions for formatting help text and option descriptions
 *
 * This file implements the formatting logic for help display.
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
#include "argus/internal/help_formatter.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

const char *get_base_type_name(argus_valtype_t type)
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

const char *get_collection_format(argus_valtype_t type)
{
    if (type & VALUE_TYPE_ARRAY) {
        return "%s,...";
    }
    if (type & VALUE_TYPE_MAP)
        return "KEY=%s,...";
    return NULL;
}

char *format_collection_hint(const char *format, const char *type_name)
{
    static char buffer[64];  // Buffer for the formatted string

    snprintf(buffer, sizeof(buffer), format, type_name);
    return buffer;
}

void print_value_hint(const argus_option_t *option)
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

void print_wrapped_text(const char *text, size_t indent, size_t line_width)
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

size_t print_option_name(const argus_option_t *option, size_t indent)
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

char *build_option_description(const argus_option_t *option)
{
    // Start with help text
    char *description = NULL;

    if (option->help) {
        description = safe_strdup(option->help);
        if (!description) {
            return NULL;
        }
    } else {
        description = safe_strdup("");
        if (!description) {
            return NULL;
        }
    }

    // TODO: Future extension point for validator formatters
    // This is where we would add validator information like:
    // - Choices: [opt1, opt2, opt3]
    // - Range: [1 - 125]
    // - Regex: pattern /^[a-z]+$/
    //
    // Example implementation:
    // if (option->validators) {
    //     for (int i = 0; option->validators[i] != NULL; ++i) {
    //         validator_entry_t *validator = option->validators[i];
    //         if (validator->formatter) {
    //             char *validator_desc = validator->formatter(validator->data);
    //             if (validator_desc) {
    //                 // Append to description
    //                 // ...
    //                 free(validator_desc);
    //             }
    //         }
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

    return description;
}

void print_option_description(const argus_option_t *option, size_t padding)
{
    size_t description_indent = padding < 4 ? DESCRIPTION_COLUMN : padding;

    if (padding < 4)
        printf("\n");
    for (size_t i = 0; i < description_indent; ++i)
        printf(" ");
    printf("- ");

    // Build the complete description
    char *description = build_option_description(option);
    if (!description) {
        printf("Error: Memory allocation failed\n");
        return;
    }

    // Print the wrapped description
    if (strlen(description) > 0)
        print_wrapped_text(description, description_indent, MAX_LINE_WIDTH);

    free(description);
    printf("\n");
}
