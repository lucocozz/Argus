#define _GNU_SOURCE  // NOLINT

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/options.h"
#include "argus/types.h"

/**
 * Convert a string to a boolean value
 * Accepts various representations of boolean values
 *
 * True: "true", "yes", "1", "on", "y" (case-insensitive)
 * False: "false", "no", "0", "off", "n" (case-insensitive)
 *
 * @param arg String to convert
 * @return 1 for true, 0 for false, -1 for invalid value
 */
static int string_to_bool(const char *arg)
{
    char  value[6]       = {0};
    char *false_values[] = {"0", "false", "no", "n", "off", "0x0", "0b0"};
    char *true_values[]  = {"1", "true", "yes", "y", "on", "0x1", "0b1"};

    if (strlen(arg) > sizeof(value) - 1)
        return -1;

    size_t arg_len = strlen(arg);
    for (size_t i = 0; i < arg_len && i < sizeof(value) - 1; ++i)
        value[i] = tolower(arg[i]);
    value[arg_len] = '\0';

    for (size_t i = 0; i < ARRAY_SIZE(true_values); ++i) {
        if (strcmp(value, true_values[i]) == 0)
            return true;
    }

    for (size_t i = 0; i < ARRAY_SIZE(false_values); ++i) {
        if (strcmp(value, false_values[i]) == 0)
            return false;
    }

    return -1;  // Invalid boolean value
}

/**
 * Set or update a key-value pair in the map
 */
static int set_kv_pair(argus_t *argus, argus_option_t *option, char *pair)
{
    // Find the separator '='
    char *separator = strchr(pair, '=');
    if (separator == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT,
                           "Invalid key-value format, expected 'key=value': '%s'", pair);
    }

    // Split the string at the separator
    char *key = strndup(pair, separator - pair);
    if (key == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Failed to allocate memory for key '%s'",
                           key);
    }
    char *value = separator + 1;

    // Convert the string value to boolean
    int bool_value = string_to_bool(value);
    if (bool_value == -1) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                           "Invalid boolean value for key '%s': '%s' (expected true/false, yes/no, "
                           "1/0, on/off, y/n)",
                           key, value);
    }

    // Check if the key already exists
    int key_index = map_find_key(option, key);

    // Key exists, update value
    if (key_index >= 0) {
        option->value.as_map[key_index].value.as_bool = (bool)bool_value;
    } else {
        // Key doesn't exist, add new entry
        adjust_map_size(option);

        option->value.as_map[option->value_count].key           = key;
        option->value.as_map[option->value_count].value.as_bool = (bool)bool_value;
        option->value_count++;
    }

    return ARGUS_SUCCESS;
}

/**
 * Handler for boolean map options
 * Format: "key1=true,key2=false,key3=yes,key4=no,..."
 * Accepts various boolean representations: true/false, yes/no, 1/0, on/off, y/n
 * If key exists, replaces the value, otherwise creates a new entry
 */
int map_bool_handler(argus_t *argus, argus_option_t *option, char *value)
{
    // Process comma-separated pairs
    if (strchr(value, ',') != NULL) {
        char **pairs = split(value, ",");
        if (pairs == NULL) {
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Failed to split string '%s'", value);
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
 * Free handler for boolean map options
 */
int free_map_bool_handler(argus_option_t *option)
{
    if (option->value.as_map != NULL) {
        // No need to free boolean values
        for (size_t i = 0; i < option->value_count; ++i)
            free((void *)option->value.as_map[i].key);
        free(option->value.as_map);
    }
    return ARGUS_SUCCESS;
}
