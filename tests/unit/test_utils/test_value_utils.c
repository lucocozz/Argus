#define _GNU_SOURCE // NOLINT

#include <criterion/criterion.h>
#include "argus/internal/utils.h"
#include "argus/types.h"


Test(value_utils, compare_values)
{
    // Test comparing integers
    argus_value_t a = {.as_int = 42};
    argus_value_t b = {.as_int = 42};
    argus_value_t c = {.as_int = 100};
    
    cr_assert_eq(cmp_value(VALUE_TYPE_INT, a, b), 0, "Equal integers should compare as equal");
    cr_assert_lt(cmp_value(VALUE_TYPE_INT, a, c), 0, "42 should be less than 100");
    cr_assert_gt(cmp_value(VALUE_TYPE_INT, c, a), 0, "100 should be greater than 42");
    
    // Test comparing strings
    argus_value_t str1 = {.as_string = "abc"};
    argus_value_t str2 = {.as_string = "abc"};
    argus_value_t str3 = {.as_string = "def"};
    
    cr_assert_eq(cmp_value(VALUE_TYPE_STRING, str1, str2), 0, "Equal strings should compare as equal");
    cr_assert_lt(cmp_value(VALUE_TYPE_STRING, str1, str3), 0, "abc should be less than def");
    cr_assert_gt(cmp_value(VALUE_TYPE_STRING, str3, str1), 0, "def should be greater than abc");
    
    // Test comparing booleans
    argus_value_t true_val = {.as_bool = true};
    argus_value_t false_val = {.as_bool = false};
    
    cr_assert_eq(cmp_value(VALUE_TYPE_BOOL, true_val, true_val), 0, "Equal booleans should compare as equal");
    cr_assert_lt(cmp_value(VALUE_TYPE_BOOL, false_val, true_val), 0, "false should be less than true");
    cr_assert_gt(cmp_value(VALUE_TYPE_BOOL, true_val, false_val), 0, "true should be greater than false");
    
    // Test comparing floats
    argus_value_t float1 = {.as_float = 3.14};
    argus_value_t float2 = {.as_float = 3.14};
    argus_value_t float3 = {.as_float = 2.71};

    cr_assert_eq(cmp_value(VALUE_TYPE_FLOAT, float1, float2), 0, "Equal floats should compare as equal");
    cr_assert_lt(cmp_value(VALUE_TYPE_FLOAT, float3, float1), 0, "2.71 should be less than 3.14");
    cr_assert_gt(cmp_value(VALUE_TYPE_FLOAT, float1, float3), 0, "3.14 should be greater than 2.71");

    // Edge cases for string comparison
    argus_value_t empty_str = {.as_string = ""};
    argus_value_t null_str = {.as_string = NULL};
    
    cr_assert_lt(cmp_value(VALUE_TYPE_STRING, empty_str, str1), 0, "Empty string should be less than non-empty");
    cr_assert_neq(cmp_value(VALUE_TYPE_STRING, null_str, str1), 0, "NULL string should not equal non-NULL string");
}

Test(value_utils, choices_to_value)
{
    // Test integer choices
    argus_value_t int_choices = {.as_array_int = (long long[]){10, 20, 30, 40, 50}};
    
    argus_value_t result1 = choices_to_value(VALUE_TYPE_INT, int_choices, 5, 2);
    cr_assert_eq(result1.as_int, 30, "Third element should be 30");
    
    argus_value_t result2 = choices_to_value(VALUE_TYPE_INT, int_choices, 5, 0);
    cr_assert_eq(result2.as_int, 10, "First element should be 10");
    
    // Test with invalid index
    argus_value_t invalid = choices_to_value(VALUE_TYPE_INT, int_choices, 5, 10);
    cr_assert_eq(invalid.raw, 0, "Invalid index should return empty value");
    
    // Test string choices
    argus_value_t str_choices = {.as_array_string = (char*[]){"one", "two", "three"}};
    
    argus_value_t str_result = choices_to_value(VALUE_TYPE_STRING, str_choices, 3, 1);
    cr_assert_str_eq(str_result.as_string, "two", "Second element should be 'two'");
    
    // Test float choices
    argus_value_t float_choices = {.as_array_float = (double[]){1.1, 2.2, 3.3}};
    
    argus_value_t float_result = choices_to_value(VALUE_TYPE_FLOAT, float_choices, 3, 2);
    cr_assert_float_eq(float_result.as_float, 3.3, 0.0001, "Third element should be 3.3");
    
    // Test with negative index
    argus_value_t neg_index = choices_to_value(VALUE_TYPE_INT, int_choices, 5, -1);
    cr_assert_eq(neg_index.raw, 0, "Negative index should return empty value");
}

int custom_free_handler(argus_option_t *opt) {
    opt->is_allocated = false;
    return 0;
}

Test(value_utils, free_option_value)
{
    // Test freeing non-allocated values
    argus_option_t option1 = {
        .is_allocated = false,
        .value = {.as_int = 42}
    };
    
    free_option_value(&option1);
    cr_assert(true, "free_option_value should not crash with non-allocated values");
    

    argus_option_t option2 = {
        .is_allocated = true,
        .value = {.as_string = strdup("test")},
        .free_handler = custom_free_handler
    };
    
    free_option_value(&option2);
    cr_assert_eq(option2.is_allocated, false, "Custom free handler should be called");
    
    // No need to free option2.value.as_string as it's handled by the custom handler
}

Test(value_utils, print_value_functions)
{
    // Redirect stdout to /dev/null to test print functions
    FILE* null_file = fopen("/dev/null", "w");
    cr_assert_not_null(null_file, "Failed to open /dev/null");
    
    // Test print_value for different types
    argus_value_t int_val = {.as_int = 42};
    print_value(null_file, VALUE_TYPE_INT, int_val);
    
    argus_value_t str_val = {.as_string = "test"};
    print_value(null_file, VALUE_TYPE_STRING, str_val);
    
    argus_value_t float_val = {.as_float = 3.14};
    print_value(null_file, VALUE_TYPE_FLOAT, float_val);
    
    argus_value_t bool_val = {.as_bool = true};
    print_value(null_file, VALUE_TYPE_BOOL, bool_val);
    
    // Test print_value_array
    argus_value_t array[3] = {{.as_int = 1}, {.as_int = 2}, {.as_int = 3}};
    print_value_array(null_file, VALUE_TYPE_INT, array, 3);
    
    argus_value_t str_array[2] = {{.as_string = "hello"}, {.as_string = "world"}};
    print_value_array(null_file, VALUE_TYPE_STRING, str_array, 2);
    
    fclose(null_file);
    cr_assert(true, "All print functions should execute without crashing");
}
