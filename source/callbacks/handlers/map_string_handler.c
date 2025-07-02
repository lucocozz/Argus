/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#define _GNU_SOURCE  // NOLINT

#include <stdlib.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/cross_platform.h"
#include "argus/internal/utils.h"
#include "argus/options.h"
#include "argus/types.h"

/**
 * Set or update a key-value pair in the map
 */
static int set_kv_pair(argus_t *argus, argus_option_t *option, char *pair)
{
    // Find the separator '='
    char *separator = strchr(pair, '=');
    if (separator == NULL) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT,
                            "Invalid key-value format, expected 'key=value': '%s'", pair);
        return ARGUS_ERROR_INVALID_FORMAT;
    }

    // Split the string at the separator
    char *key = safe_strndup(pair, separator - pair);
    if (key == NULL) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_MEMORY, "Failed to allocate memory for key '%s'",
                            key);
        return ARGUS_ERROR_MEMORY;
    }
    char *value = safe_strdup(separator + 1);
    if (value == NULL) {
        free(key);
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_MEMORY, "Failed to allocate memory for value '%s'",
                            value);
        return ARGUS_ERROR_MEMORY;
    }

    // Check if the key already exists
    int key_index = map_find_key(option, key);

    if (key_index >= 0) {
        // Key exists, update value
        free(option->value.as_map[key_index].value.as_string);
        option->value.as_map[key_index].value.as_string = value;
    } else {
        // Key doesn't exist, add new entry

        adjust_map_size(option);

        option->value.as_map[option->value_count].key             = key;
        option->value.as_map[option->value_count].value.as_string = value;
        option->value_count++;
    }

    return ARGUS_SUCCESS;
}

/**
 * Handler for string map options
 * Format: "key1=value1,key2=value2,..."
 * Format: "key1=value1" "key2=value2" ...
 * If key exists, replaces the value, otherwise creates a new entry
 */
int map_string_handler(argus_t *argus, argus_option_t *option, char *value)
{
    // Process comma-separated pairs
    if (strchr(value, ',') != NULL) {
        char **pairs = split(value, ",");
        if (pairs == NULL) {
            ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_MEMORY, "Failed to split string '%s'", value);
            return ARGUS_ERROR_MEMORY;
        }

        for (size_t i = 0; pairs[i] != NULL; ++i) {
            int status = set_kv_pair(argus, option, pairs[i]);
            if (status != ARGUS_SUCCESS) {
                free_split(pairs);
                return status;
            }
        }

        free_split(pairs);
    } else {
        // Single key-value pair
        int status = set_kv_pair(argus, option, value);
        if (status != ARGUS_SUCCESS)
            return status;
    }

    apply_map_flags(option);
    option->is_allocated = true;
    return ARGUS_SUCCESS;
}

/**
 * Free handler for string map options
 */
int free_map_string_handler(argus_option_t *option)
{
    if (option->value.as_map != NULL) {
        for (size_t i = 0; i < option->value_count; ++i) {
            free((void *)option->value.as_map[i].key);
            free((void *)option->value.as_map[i].value.as_string);
        }
        free(option->value.as_map);
    }
    return ARGUS_SUCCESS;
}
