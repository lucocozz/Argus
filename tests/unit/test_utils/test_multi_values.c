#include <criterion/criterion.h>
#include "argus/internal/utils.h"
#include "argus/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declare functions from source/utils/multi_values.c
extern void sort_int_array(argus_value_t *array, size_t count);
extern void sort_string_array(argus_value_t *array, size_t count);
extern size_t make_int_array_unique(argus_value_t *array, size_t count);
extern void sort_map_by_keys(argus_pair_t *map, size_t count);
extern void apply_array_flags(argus_option_t *option);
extern void apply_map_flags(argus_option_t *option);

// Setup function for array tests
void setup_array_option(argus_option_t *option, argus_valtype_t type)
{
    memset(option, 0, sizeof(argus_option_t));
    option->value_type = type;
    option->value.as_array = NULL;
    option->value_count = 0;
    option->value_capacity = 0;
}

// Setup function for map tests
void setup_map_option(argus_option_t *option, argus_valtype_t type)
{
    memset(option, 0, sizeof(argus_option_t));
    option->value_type = type;
    option->value.as_map = NULL;
    option->value_count = 0;
    option->value_capacity = 0;
}

Test(multi_values, adjust_array_size)
{
    argus_option_t option;
    setup_array_option(&option, VALUE_TYPE_ARRAY_INT);
    
    // First call should allocate initial capacity
    adjust_array_size(&option);
    cr_assert_not_null(option.value.as_array, "Array should be allocated");
    cr_assert_eq(option.value_capacity, MULTI_VALUE_INITIAL_CAPACITY, "Initial capacity should be set");
    
    // Fill the array to capacity
    option.value_count = option.value_capacity;
    
    // Next call should double the capacity
    adjust_array_size(&option);
    cr_assert_eq(option.value_capacity, MULTI_VALUE_INITIAL_CAPACITY * 2, "Capacity should double");
    
    // Clean up
    free(option.value.as_array);
}

Test(multi_values, adjust_map_size)
{
    argus_option_t option;
    setup_map_option(&option, VALUE_TYPE_MAP_STRING);
    
    // First call should allocate initial capacity
    adjust_map_size(&option);
    cr_assert_not_null(option.value.as_map, "Map should be allocated");
    cr_assert_eq(option.value_capacity, MULTI_VALUE_INITIAL_CAPACITY, "Initial capacity should be set");
    
    // Fill the map to capacity
    option.value_count = option.value_capacity;
    
    // Next call should double the capacity
    adjust_map_size(&option);
    cr_assert_eq(option.value_capacity, MULTI_VALUE_INITIAL_CAPACITY * 2, "Capacity should double");
    
    // Clean up
    free(option.value.as_map);
}

Test(multi_values, sort_int_array)
{
    // Create an unsorted array
    argus_value_t array[5] = {
        {.as_int = 42},
        {.as_int = 10},
        {.as_int = 30},
        {.as_int = 20},
        {.as_int = 50}
    };
    
    // Sort the array
    sort_int_array(array, 5);
    
    // Check that the array is sorted
    cr_assert_eq(array[0].as_int, 10, "First element should be 10");
    cr_assert_eq(array[1].as_int, 20, "Second element should be 20");
    cr_assert_eq(array[2].as_int, 30, "Third element should be 30");
    cr_assert_eq(array[3].as_int, 42, "Fourth element should be 42");
    cr_assert_eq(array[4].as_int, 50, "Fifth element should be 50");
}

Test(multi_values, sort_string_array)
{
    // Create an unsorted array
    argus_value_t array[4] = {
        {.as_string = "delta"},
        {.as_string = "alpha"},
        {.as_string = "charlie"},
        {.as_string = "bravo"}
    };
    
    // Sort the array
    sort_string_array(array, 4);
    
    // Check that the array is sorted
    cr_assert_str_eq(array[0].as_string, "alpha", "First element should be 'alpha'");
    cr_assert_str_eq(array[1].as_string, "bravo", "Second element should be 'bravo'");
    cr_assert_str_eq(array[2].as_string, "charlie", "Third element should be 'charlie'");
    cr_assert_str_eq(array[3].as_string, "delta", "Fourth element should be 'delta'");
}

Test(multi_values, make_int_array_unique)
{
    // Create an array with duplicates
    argus_value_t array[6] = {
        {.as_int = 10},
        {.as_int = 20},
        {.as_int = 10},  // Duplicate
        {.as_int = 30},
        {.as_int = 20},  // Duplicate
        {.as_int = 40}
    };
    
    // Make the array unique
    size_t new_count = make_int_array_unique(array, 6);
    
    // Check the new count and values
    cr_assert_eq(new_count, 4, "Unique count should be 4");
    cr_assert_eq(array[0].as_int, 10, "First element should be 10");
    cr_assert_eq(array[1].as_int, 20, "Second element should be 20");
    cr_assert_eq(array[2].as_int, 30, "Third element should be 30");
    cr_assert_eq(array[3].as_int, 40, "Fourth element should be 40");
}

Test(multi_values, apply_array_flags)
{
    argus_option_t option;
    setup_array_option(&option, VALUE_TYPE_ARRAY_INT);
    
    // Allocate array with duplicates and unsorted values
    option.value_capacity = 6;
    option.value.as_array = malloc(option.value_capacity * sizeof(argus_value_t));
    
    option.value.as_array[0].as_int = 30;
    option.value.as_array[1].as_int = 10;
    option.value.as_array[2].as_int = 20;
    option.value.as_array[3].as_int = 10;  // Duplicate
    option.value.as_array[4].as_int = 30;  // Duplicate
    option.value.as_array[5].as_int = 20;  // Duplicate
    option.value_count = 6;
    
    // Test with sorting only
    option.flags = FLAG_SORTED;
    apply_array_flags(&option);
    
    cr_assert_eq(option.value_count, 6, "Count should not change with SORTED flag only");
    cr_assert_eq(option.value.as_array[0].as_int, 10, "First element should be 10");
    cr_assert_eq(option.value.as_array[1].as_int, 10, "Second element should be 10 (duplicate)");
    cr_assert_eq(option.value.as_array[2].as_int, 20, "Third element should be 20");
    cr_assert_eq(option.value.as_array[3].as_int, 20, "Fourth element should be 20 (duplicate)");
    cr_assert_eq(option.value.as_array[4].as_int, 30, "Fifth element should be 30");
    cr_assert_eq(option.value.as_array[5].as_int, 30, "Sixth element should be 30 (duplicate)");
    
    // Reset array
    free(option.value.as_array);
    option.value.as_array = malloc(option.value_capacity * sizeof(argus_value_t));
    
    option.value.as_array[0].as_int = 30;
    option.value.as_array[1].as_int = 10;
    option.value.as_array[2].as_int = 20;
    option.value.as_array[3].as_int = 10;
    option.value.as_array[4].as_int = 30;
    option.value.as_array[5].as_int = 20;
    option.value_count = 6;
    
    // Test with unique only
    option.flags = FLAG_UNIQUE;
    apply_array_flags(&option);
    
    cr_assert_eq(option.value_count, 3, "Count should be reduced with UNIQUE flag");
    
    // Note: Original order is maintained but duplicates are removed
    cr_assert_eq(option.value.as_array[0].as_int, 30, "First element should be 30");
    cr_assert_eq(option.value.as_array[1].as_int, 10, "Second element should be 10");
    cr_assert_eq(option.value.as_array[2].as_int, 20, "Third element should be 20");
    
    // Reset array
    free(option.value.as_array);
    option.value.as_array = malloc(option.value_capacity * sizeof(argus_value_t));
    
    option.value.as_array[0].as_int = 30;
    option.value.as_array[1].as_int = 10;
    option.value.as_array[2].as_int = 20;
    option.value.as_array[3].as_int = 10;
    option.value.as_array[4].as_int = 30;
    option.value.as_array[5].as_int = 20;
    option.value_count = 6;
    
    // Test with both sorted and unique
    option.flags = FLAG_SORTED | FLAG_UNIQUE;
    apply_array_flags(&option);
    
    cr_assert_eq(option.value_count, 3, "Count should be reduced with UNIQUE flag");
    cr_assert_eq(option.value.as_array[0].as_int, 10, "First element should be 10");
    cr_assert_eq(option.value.as_array[1].as_int, 20, "Second element should be 20");
    cr_assert_eq(option.value.as_array[2].as_int, 30, "Third element should be 30");
    
    // Clean up
    free(option.value.as_array);
}

Test(multi_values, map_find_key)
{
    argus_option_t option;
    setup_map_option(&option, VALUE_TYPE_MAP_STRING);
    
    // Allocate and populate map
    option.value_capacity = 3;
    option.value.as_map = malloc(option.value_capacity * sizeof(argus_pair_t));
    
    option.value.as_map[0].key = strdup("key1");
    option.value.as_map[0].value.as_string = strdup("value1");
    
    option.value.as_map[1].key = strdup("key2");
    option.value.as_map[1].value.as_string = strdup("value2");
    
    option.value.as_map[2].key = strdup("key3");
    option.value.as_map[2].value.as_string = strdup("value3");
    
    option.value_count = 3;
    
    // Test key lookup
    int index = map_find_key(&option, "key1");
    cr_assert_eq(index, 0, "First key should be at index 0");
    
    index = map_find_key(&option, "key2");
    cr_assert_eq(index, 1, "Second key should be at index 1");
    
    index = map_find_key(&option, "key3");
    cr_assert_eq(index, 2, "Third key should be at index 2");
    
    index = map_find_key(&option, "nonexistent");
    cr_assert_eq(index, -1, "Nonexistent key should return -1");
    
    // Clean up
    for (size_t i = 0; i < option.value_count; i++) {
        free((void*)option.value.as_map[i].key);
        free(option.value.as_map[i].value.as_string);
    }
    free(option.value.as_map);
}

Test(multi_values, sort_map_by_keys)
{
    argus_option_t option;
    setup_map_option(&option, VALUE_TYPE_MAP_STRING);
    
    // Allocate and populate map in unsorted order
    option.value_capacity = 3;
    option.value.as_map = malloc(option.value_capacity * sizeof(argus_pair_t));
    
    option.value.as_map[0].key = strdup("charlie");
    option.value.as_map[0].value.as_string = strdup("value3");
    
    option.value.as_map[1].key = strdup("alpha");
    option.value.as_map[1].value.as_string = strdup("value1");
    
    option.value.as_map[2].key = strdup("bravo");
    option.value.as_map[2].value.as_string = strdup("value2");
    
    option.value_count = 3;
    
    // Sort the map by keys
    sort_map_by_keys(option.value.as_map, option.value_count);
    
    // Check sorted order
    cr_assert_str_eq(option.value.as_map[0].key, "alpha", "First key should be 'alpha'");
    cr_assert_str_eq(option.value.as_map[0].value.as_string, "value1", "First value should be 'value1'");
    
    cr_assert_str_eq(option.value.as_map[1].key, "bravo", "Second key should be 'bravo'");
    cr_assert_str_eq(option.value.as_map[1].value.as_string, "value2", "Second value should be 'value2'");
    
    cr_assert_str_eq(option.value.as_map[2].key, "charlie", "Third key should be 'charlie'");
    cr_assert_str_eq(option.value.as_map[2].value.as_string, "value3", "Third value should be 'value3'");
    
    // Clean up
    for (size_t i = 0; i < option.value_count; i++) {
        free((void*)option.value.as_map[i].key);
        free(option.value.as_map[i].value.as_string);
    }
    free(option.value.as_map);
}
