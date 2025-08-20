/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#include "argus/internal/utils.h"
#include "argus/types.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/*
 * Comparison functions for qsort
 */

static int compare_int_values(const void *a, const void *b)
{
    const argus_value_t *va = (const argus_value_t *)a;
    const argus_value_t *vb = (const argus_value_t *)b;

    if (va->as_int < vb->as_int)
        return -1;
    if (va->as_int > vb->as_int)
        return 1;
    return 0;
}

static int compare_string_values(const void *a, const void *b)
{
    const argus_value_t *va = (const argus_value_t *)a;
    const argus_value_t *vb = (const argus_value_t *)b;

    if (!va->as_string)
        return -1;
    if (!vb->as_string)
        return 1;

    return strcmp(va->as_string, vb->as_string);
}

static int compare_float_values(const void *a, const void *b)
{
    const argus_value_t *va = (const argus_value_t *)a;
    const argus_value_t *vb = (const argus_value_t *)b;

    if (va->as_float < vb->as_float)
        return -1;
    if (va->as_float > vb->as_float)
        return 1;
    return 0;
}

static int compare_map_keys(const void *a, const void *b)
{
    const argus_pair_t *pa = (const argus_pair_t *)a;
    const argus_pair_t *pb = (const argus_pair_t *)b;

    if (!pa->key)
        return -1;
    if (!pb->key)
        return 1;

    return strcmp(pa->key, pb->key);
}

static int compare_map_int_values(const void *a, const void *b)
{
    const argus_pair_t *pa = (const argus_pair_t *)a;
    const argus_pair_t *pb = (const argus_pair_t *)b;

    if (pa->value.as_int < pb->value.as_int)
        return -1;
    if (pa->value.as_int > pb->value.as_int)
        return 1;
    return 0;
}

static int compare_map_string_values(const void *a, const void *b)
{
    const argus_pair_t *pa = (const argus_pair_t *)a;
    const argus_pair_t *pb = (const argus_pair_t *)b;

    if (!pa->value.as_string)
        return -1;
    if (!pb->value.as_string)
        return 1;

    return strcmp(pa->value.as_string, pb->value.as_string);
}

static int compare_map_float_values(const void *a, const void *b)
{
    const argus_pair_t *pa = (const argus_pair_t *)a;
    const argus_pair_t *pb = (const argus_pair_t *)b;

    if (pa->value.as_float < pb->value.as_float)
        return -1;
    if (pa->value.as_float > pb->value.as_float)
        return 1;
    return 0;
}

static int compare_map_bool_values(const void *a, const void *b)
{
    const argus_pair_t *pa = (const argus_pair_t *)a;
    const argus_pair_t *pb = (const argus_pair_t *)b;

    return pa->value.as_bool - pb->value.as_bool;
}

/*
 * Array sorting implementations
 */

void sort_int_array(argus_value_t *array, size_t count)
{
    if (count <= 1)
        return;
    qsort(array, count, sizeof(argus_value_t), compare_int_values);
}

void sort_string_array(argus_value_t *array, size_t count)
{
    if (count <= 1)
        return;
    qsort(array, count, sizeof(argus_value_t), compare_string_values);
}

void sort_float_array(argus_value_t *array, size_t count)
{
    if (count <= 1)
        return;
    qsort(array, count, sizeof(argus_value_t), compare_float_values);
}

/*
 * Array uniqueness implementations
 */

size_t make_int_array_unique(argus_value_t *array, size_t count)
{
    if (count <= 1)
        return count;

    size_t unique_count = 1;

    for (size_t i = 1; i < count; i++) {
        bool is_duplicate = false;

        for (size_t j = 0; j < unique_count; j++) {
            if (array[i].as_int == array[j].as_int) {
                is_duplicate = true;
                break;
            }
        }

        if (!is_duplicate)
            array[unique_count++] = array[i];
    }

    return unique_count;
}

size_t make_string_array_unique(argus_value_t *array, size_t count)
{
    if (count <= 1)
        return count;

    size_t unique_count = 1;

    for (size_t i = 1; i < count; i++) {
        bool is_duplicate = false;

        for (size_t j = 0; j < unique_count; j++) {
            if (array[i].as_string && array[j].as_string &&
                strcmp(array[i].as_string, array[j].as_string) == 0) {
                is_duplicate = true;
                // Free duplicate string
                free(array[i].as_string);
                break;
            }
        }

        if (!is_duplicate)
            array[unique_count++] = array[i];
    }

    return unique_count;
}

size_t make_float_array_unique(argus_value_t *array, size_t count)
{
    if (count <= 1)
        return count;

    size_t unique_count = 1;

    for (size_t i = 1; i < count; i++) {
        bool is_duplicate = false;

        for (size_t j = 0; j < unique_count; j++) {
            // Use small epsilon for float comparison
            if (fabs(array[i].as_float - array[j].as_float) < 0.0000001) {
                is_duplicate = true;
                break;
            }
        }

        if (!is_duplicate) {
            array[unique_count++] = array[i];
        }
    }

    return unique_count;
}

/*
 * Map sorting implementations
 */

void sort_map_by_keys(argus_pair_t *map, size_t count)
{
    if (count <= 1)
        return;
    qsort(map, count, sizeof(argus_pair_t), compare_map_keys);
}

void sort_map_by_int_values(argus_pair_t *map, size_t count)
{
    if (count <= 1)
        return;
    qsort(map, count, sizeof(argus_pair_t), compare_map_int_values);
}

void sort_map_by_string_values(argus_pair_t *map, size_t count)
{
    if (count <= 1)
        return;
    qsort(map, count, sizeof(argus_pair_t), compare_map_string_values);
}

void sort_map_by_float_values(argus_pair_t *map, size_t count)
{
    if (count <= 1)
        return;
    qsort(map, count, sizeof(argus_pair_t), compare_map_float_values);
}

void sort_map_by_bool_values(argus_pair_t *map, size_t count)
{
    if (count <= 1)
        return;
    qsort(map, count, sizeof(argus_pair_t), compare_map_bool_values);
}

/*
 * Map uniqueness implementation
 */

size_t make_map_values_unique(argus_pair_t *map, size_t count, argus_valtype_t type)
{
    if (count <= 1)
        return count;

    // Allocate array to track duplicates
    bool *duplicates = calloc(count, sizeof(bool));
    if (!duplicates)
        return count;  // Failed to allocate, return unchanged

    size_t unique_count = 0;

    // Mark duplicates
    for (size_t i = 0; i < count; i++) {
        if (duplicates[i])
            continue;

        for (size_t j = i + 1; j < count; j++) {
            if (duplicates[j])
                continue;

            bool is_duplicate = false;

            switch (type) {
                case VALUE_TYPE_INT:
                case VALUE_TYPE_MAP_INT:
                    is_duplicate = (map[i].value.as_int == map[j].value.as_int);
                    break;

                case VALUE_TYPE_STRING:
                case VALUE_TYPE_MAP_STRING:
                    if (map[i].value.as_string && map[j].value.as_string) {
                        is_duplicate =
                            (strcmp(map[i].value.as_string, map[j].value.as_string) == 0);
                    }
                    break;

                case VALUE_TYPE_FLOAT:
                case VALUE_TYPE_MAP_FLOAT:
                    is_duplicate =
                        (fabs(map[i].value.as_float - map[j].value.as_float) < 0.0000001);
                    break;

                case VALUE_TYPE_BOOL:
                case VALUE_TYPE_MAP_BOOL:
                    is_duplicate = (map[i].value.as_bool == map[j].value.as_bool);
                    break;

                default:
                    break;
            }

            if (is_duplicate)
                duplicates[j] = true;
        }
    }

    // Rebuild the array without duplicates
    for (size_t i = 0; i < count; i++) {
        if (!duplicates[i]) {
            if (i != unique_count)
                map[unique_count] = map[i];
            unique_count++;
        } else {
            // Free duplicate string keys
            free((void *)map[i].key);

            // Free string values if applicable
            if ((type == VALUE_TYPE_STRING || type == VALUE_TYPE_MAP_STRING) &&
                map[i].value.as_string)
                free(map[i].value.as_string);
        }
    }

    free(duplicates);
    return unique_count;
}

/*
 * Combined operations for arrays
 */

void apply_array_flags(argus_option_t *option)
{
    if (option->value_count <= 1)
        return;

    // First sort if needed
    if (option->flags & FLAG_SORTED) {
        switch (option->value_type) {
            case VALUE_TYPE_ARRAY_INT:
            case VALUE_TYPE_VARIADIC_INT:
                sort_int_array(option->value.as_array, option->value_count);
                break;

            case VALUE_TYPE_ARRAY_STRING:
            case VALUE_TYPE_VARIADIC_STRING:
                sort_string_array(option->value.as_array, option->value_count);
                break;

            case VALUE_TYPE_ARRAY_FLOAT:
            case VALUE_TYPE_VARIADIC_FLOAT:
                sort_float_array(option->value.as_array, option->value_count);
                break;

            default:
                break;
        }
    }

    // Then remove duplicates if needed
    if (option->flags & FLAG_UNIQUE) {
        size_t new_count = 0;

        switch (option->value_type) {
            case VALUE_TYPE_ARRAY_INT:
            case VALUE_TYPE_VARIADIC_INT:
                new_count = make_int_array_unique(option->value.as_array, option->value_count);
                break;

            case VALUE_TYPE_ARRAY_STRING:
            case VALUE_TYPE_VARIADIC_STRING:
                new_count = make_string_array_unique(option->value.as_array, option->value_count);
                break;

            case VALUE_TYPE_ARRAY_FLOAT:
            case VALUE_TYPE_VARIADIC_FLOAT:
                new_count = make_float_array_unique(option->value.as_array, option->value_count);
                break;

            default:
                return;
        }

        option->value_count = new_count;
    }
}

/*
 * Combined operations for maps
 */

void apply_map_flags(argus_option_t *option)
{
    if (option->value_count <= 1)
        return;

    // Handle flag priority: first unique values, then sorting

    // Remove entries with duplicate values if needed
    if (option->flags & FLAG_UNIQUE_VALUE) {
        option->value_count =
            make_map_values_unique(option->value.as_map, option->value_count, option->value_type);
    }

    // Sort by key if needed
    if (option->flags & FLAG_SORTED_KEY) {
        sort_map_by_keys(option->value.as_map, option->value_count);
        // Sort by value if needed (and not already sorted by key)
    } else if (option->flags & FLAG_SORTED_VALUE) {
        switch (option->value_type) {
            case VALUE_TYPE_MAP_INT:
                sort_map_by_int_values(option->value.as_map, option->value_count);
                break;

            case VALUE_TYPE_MAP_STRING:
                sort_map_by_string_values(option->value.as_map, option->value_count);
                break;

            case VALUE_TYPE_MAP_FLOAT:
                sort_map_by_float_values(option->value.as_map, option->value_count);
                break;

            case VALUE_TYPE_MAP_BOOL:
                sort_map_by_bool_values(option->value.as_map, option->value_count);
                break;

            default:
                break;
        }
    }
}

void adjust_array_size(argus_option_t *option)
{
    if (option->value.as_array == NULL) {
        option->value_capacity = MULTI_VALUE_INITIAL_CAPACITY;
        option->value.as_array = malloc(option->value_capacity * sizeof(argus_value_t));
        if (option->value.as_array == NULL) {
            option->value_capacity = 0;
            return;
        }
    } else if (option->value_count >= option->value_capacity) {
        size_t old_capacity = option->value_capacity;
        option->value_capacity *= 2;
        void *new = realloc(option->value.as_array, option->value_capacity * sizeof(argus_value_t));
        if (new == NULL) {
            option->value_capacity = old_capacity;
            return;
        }
        option->value.as_array = new;
    }
}

void adjust_map_size(argus_option_t *option)
{
    if (option->value.as_map == NULL) {
        option->value_capacity = MULTI_VALUE_INITIAL_CAPACITY;
        option->value.as_map   = malloc(option->value_capacity * sizeof(argus_pair_t));
        if (option->value.as_map == NULL) {
            option->value_capacity = 0;
            return;
        }
    } else if (option->value_count >= option->value_capacity) {
        size_t old_capacity = option->value_capacity;
        option->value_capacity *= 2;
        void *new = realloc(option->value.as_map, option->value_capacity * sizeof(argus_pair_t));
        if (new == NULL) {
            option->value_capacity = old_capacity;
            return;
        }
        option->value.as_map = new;
    }
}

int map_find_key(argus_option_t *option, const char *key)
{
    for (size_t i = 0; i < option->value_count; ++i) {
        if (option->value.as_map[i].key && strcmp(option->value.as_map[i].key, key) == 0)
            return ((int)i);
    }
    return (-1);
}
