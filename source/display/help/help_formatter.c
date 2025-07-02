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

#include "argus/internal/cross_platform.h"
#include "argus/internal/display.h"
#include "argus/internal/help.h"
#include "argus/internal/utils.h"
#include "argus/options.h"
#include "argus/types.h"

#include <ctype.h>

argus_helper_config_t get_default_helper_config(void)
{
    return (argus_helper_config_t){.max_line_width          = DEFAULT_MAX_LINE_WIDTH,
                                   .description_column      = DEFAULT_DESCRIPTION_COLUMN,
                                   .option_indent           = DEFAULT_OPTION_INDENT,
                                   .smart_hint_max_length   = DEFAULT_SMART_HINT_MAX_LENGTH,
                                   .smart_hint_allow_spaces = DEFAULT_SMART_HINT_ALLOW_SPACES};
}

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
    if (type & VALUE_TYPE_ARRAY)
        return "%s,...";
    if (type & VALUE_TYPE_MAP)
        return "KEY=%s,...";
    return NULL;
}

char *format_collection_hint(const char *format, const char *type_name)
{
    static char buffer[64];

    snprintf(buffer, sizeof(buffer), format, type_name);
    return buffer;
}

bool is_short_hint(argus_t *argus, const char *hint)
{
    if (!hint)
        return false;

    size_t len       = strlen(hint);
    bool   is_short  = len <= argus->helper.config.smart_hint_max_length;
    bool   is_simple = argus->helper.config.smart_hint_allow_spaces || !strchr(hint, ' ');

    return is_short && is_simple;
}

bool has_single_validator(const argus_option_t *option)
{
    if (!option->validators)
        return false;

    int count = 0;
    for (int i = 0; option->validators[i] != NULL; i++) {
        count++;
        if (count > 1)
            return false;
    }

    return count == 1;
}

char *get_smart_hint(argus_t *argus, const argus_option_t *option)
{
    if (option->hint)
        return safe_strdup(option->hint);

    if (has_single_validator(option) && option->validators[0]->formatter) {
        char *validator_hint = option->validators[0]->formatter(option->validators[0]->data);
        if (validator_hint && is_short_hint(argus, validator_hint))
            return validator_hint;
        free(validator_hint);
    }

    return safe_strdup(get_base_type_name(option->value_type));
}

/**
 * get_env_var_name_for_display - Get environment variable name for display purposes
 *
 * @param argus  Argus context
 * @param option Option to get env var name for
 *
 * @return Environment variable name or NULL if none
 */
static const char *get_env_var_name_for_display(argus_t *argus, const argus_option_t *option)
{
    static char full_name[128];
    const char *prefix           = argus->env_prefix ? argus->env_prefix : "";
    size_t      prefix_len       = strlen(prefix);
    bool        needs_underscore = prefix_len > 0 && prefix[prefix_len - 1] != '_';

    if (option->env_name) {
        if (option->flags & FLAG_NO_ENV_PREFIX)
            return option->env_name;

        if (argus->env_prefix && strncmp(option->env_name, argus->env_prefix, prefix_len) == 0)
            return option->env_name;

        if (needs_underscore) {
            snprintf(full_name, sizeof(full_name), "%s_%s", prefix, option->env_name);
        } else {
            snprintf(full_name, sizeof(full_name), "%s%s", prefix, option->env_name);
        }
        return full_name;
    }

    if (option->flags & FLAG_AUTO_ENV) {
        const char *name = option->name ? option->name : (option->lname ? option->lname : "");

        if (option->flags & FLAG_NO_ENV_PREFIX) {
            snprintf(full_name, sizeof(full_name), "%s", name);
        } else if (needs_underscore) {
            snprintf(full_name, sizeof(full_name), "%s_%s", prefix, name);
        } else {
            snprintf(full_name, sizeof(full_name), "%s%s", prefix, name);
        }

        for (char *p = full_name; *p; ++p)
            *p = *p == '-' ? '_' : (char)toupper(*p);
        return full_name;
    }

    return NULL;
}

void print_wrapped_text(const char *text, size_t indent, size_t line_width)
{
    // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.Branch)
    if (!text || !*text)
        return;

    size_t text_len    = strlen(text);
    size_t line_start  = 0;
    size_t current_pos = 0;
    size_t last_space  = 0;

    while (current_pos < text_len) {
        while (current_pos < text_len &&
               current_pos - line_start < line_width - indent - 2 &&  // Consider "- " prefix
               text[current_pos] != '\n') {
            if (text[current_pos] == ' ')
                last_space = current_pos;
            current_pos++;
        }

        if (current_pos < text_len && text[current_pos] == '\n') {
            printf("%.*s\n", (int)(current_pos - line_start), text + line_start);
            current_pos++;
            line_start = current_pos;

            if (current_pos < text_len)
                putnchar(' ', indent + 2);
        } else if (current_pos - line_start >= line_width - indent - 2) {
            if (last_space > line_start) {
                printf("%.*s\n", (int)(last_space - line_start), text + line_start);
                line_start = last_space + 1;
            } else {
                printf("%.*s\n", (int)(current_pos - line_start), text + line_start);
                line_start = current_pos;
            }

            if (line_start < text_len)
                putnchar(' ', indent + 2);
        } else {
            printf("%s", text + line_start);
            break;
        }

        current_pos = line_start;
        last_space  = line_start;
    }
}

size_t print_option_name(argus_t *argus, const argus_option_t *option, size_t indent)
{
    size_t name_len = putnchar(' ', indent);

    if (option->sname) {
        name_len += printf("-%c", option->sname);
        if (option->lname)
            name_len += printf(", ");
    }

    if (option->lname)
        name_len += printf("--%s", option->lname);

    if (option->value_type != VALUE_TYPE_FLAG) {
        char *smart_hint = get_smart_hint(argus, option);
        if (smart_hint) {
            const char *collection_format = get_collection_format(option->value_type);

            if (collection_format) {
                const char *format_str = format_collection_hint(collection_format, smart_hint);
                name_len += printf(" <%s>", format_str);
            } else
                name_len += printf(" <%s>", smart_hint);
            free(smart_hint);
        }
    }

    return name_len;
}

// Helper function to append text to description
static char *append_to_description(char *description, const char *suffix)
{
    if (!suffix || !*suffix)
        return description;

    size_t desc_len   = strlen(description);
    size_t suffix_len = strlen(suffix);
    size_t desc_size  = desc_len + suffix_len + 1;
    char  *new_desc   = malloc(desc_size);
    if (new_desc) {
        safe_strcpy(new_desc, desc_size, description);
        safe_strcat(new_desc, desc_size, suffix);
        free(description);
        return new_desc;
    }
    return description;
}

// Helper function to append default value information
static char *append_default_info(char *description, const argus_option_t *option)
{
    if (!option->have_default || option->value_type == VALUE_TYPE_FLAG)
        return description;

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
                snprintf(default_buf + default_len, sizeof(default_buf) - default_len, "\"%s\")",
                         option->default_value.as_string);
            } else
                snprintf(default_buf + default_len, sizeof(default_buf) - default_len, "null)");
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

    return append_to_description(description, default_buf);
}

// Helper function to append validator information
static char *append_validator_info(char *description, argus_t *argus, const argus_option_t *option)
{
    if (!option->validators)
        return description;

    bool validator_used_in_hint = false;
    if (has_single_validator(option) && !option->hint && option->validators[0]->formatter) {
        char *validator_hint = option->validators[0]->formatter(option->validators[0]->data);
        if (validator_hint && is_short_hint(argus, validator_hint))
            validator_used_in_hint = true;
        free(validator_hint);
    }

    // Show validators in description if not used as hint or complex case
    if (!validator_used_in_hint || !has_single_validator(option) || option->hint) {
        for (int i = 0; option->validators[i] != NULL; ++i) {
            validator_entry_t *validator = option->validators[i];
            if (validator->formatter) {
                char *validator_desc = validator->formatter(validator->data);
                if (validator_desc) {
                    char validator_info[256] = {0};

                    if (validator->func == regex_validator) {
                        snprintf(validator_info, sizeof(validator_info), " (pattern: %s)",
                                 validator_desc);
                    } else if (validator->func == choices_string_validator ||
                               validator->func == choices_int_validator ||
                               validator->func == choices_float_validator) {
                        snprintf(validator_info, sizeof(validator_info), " [%s]", validator_desc);
                    } else if (validator->func == length_validator) {
                        snprintf(validator_info, sizeof(validator_info), " (%s)", validator_desc);
                    } else if (validator->func == range_validator) {
                        snprintf(validator_info, sizeof(validator_info), " (range: %s)",
                                 validator_desc);
                    } else if (validator->func == count_validator) {
                        snprintf(validator_info, sizeof(validator_info), " (count: %s)",
                                 validator_desc);
                    }

                    if (validator_info[0] != '\0') {
                        description = append_to_description(description, validator_info);
                    }

                    free(validator_desc);
                }
            }
        }
    }
    return description;
}

// Helper function to append environment variable information
static char *append_env_info(char *description, argus_t *argus, const argus_option_t *option)
{
    const char *env_name = get_env_var_name_for_display(argus, option);
    if (!env_name)
        return description;

    char   env_buf[256] = {0};
    size_t desc_len     = strlen(description);

    if (desc_len > 0 && description[desc_len - 1] == ')') {
        snprintf(env_buf, sizeof(env_buf), ", env: %s)", env_name);
        if (desc_len > 0)
            description[desc_len - 1] = '\0';
    } else
        snprintf(env_buf, sizeof(env_buf), " (env: %s)", env_name);

    return append_to_description(description, env_buf);
}

// Helper function to append flag information
static char *append_flag_info(char *description, const argus_option_t *option)
{
    if (!(option->flags & (FLAG_EXIT | FLAG_REQUIRED | FLAG_DEPRECATED | FLAG_EXPERIMENTAL)))
        return description;

    char attrs_buf[32] = {0};

    if (option->flags & FLAG_EXIT)
        safe_strcat(attrs_buf, sizeof(attrs_buf), " (exit)");
    if (option->flags & FLAG_REQUIRED)
        safe_strcat(attrs_buf, sizeof(attrs_buf), " (required)");
    if (option->flags & FLAG_DEPRECATED)
        safe_strcat(attrs_buf, sizeof(attrs_buf), " (deprecated)");
    if (option->flags & FLAG_EXPERIMENTAL)
        safe_strcat(attrs_buf, sizeof(attrs_buf), " (experimental)");

    return append_to_description(description, attrs_buf);
}

char *build_option_description(argus_t *argus, const argus_option_t *option)
{
    char *description = NULL;

    if (option->help) {
        description = safe_strdup(option->help);
        if (!description)
            return NULL;
    } else {
        description = safe_strdup("");
        if (!description)
            return NULL;
    }

    description = append_validator_info(description, argus, option);

    description = append_default_info(description, option);

    description = append_env_info(description, argus, option);

    description = append_flag_info(description, option);

    return description;
}

void print_option_description(argus_t *argus, const argus_option_t *option, size_t padding)
{
    size_t description_indent = argus->helper.config.description_column;

    if (padding < 4)
        printf("\n");
    putnchar(' ', padding < 4 ? description_indent : padding);
    printf("- ");

    char *description = build_option_description(argus, option);
    if (!description) {
        printf("Error: Memory allocation failed\n");
        return;
    }

    if (strlen(description) > 0)
        print_wrapped_text(description, description_indent, argus->helper.config.max_line_width);

    free(description);
    printf("\n");
}
