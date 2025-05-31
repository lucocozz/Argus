#define _GNU_SOURCE // NOLINT

#include <criterion/criterion.h>
#include "argus/types.h"
#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/internal/callbacks/handlers.h"
#include <stdlib.h>
#include <string.h>

// Mock argus context for testing
static argus_t test_argus;
static argus_option_t test_option;

void setup_handler(void)
{
    // Initialize argus context
    test_argus.program_name = "test_program";
    test_argus.error_stack.count = 0;
    
    // Initialize test option
    memset(&test_option, 0, sizeof(argus_option_t));
    test_option.name = "test_option";
    test_option.sname = 't';
    test_option.lname = "test";
    test_option.is_allocated = false;
}

// Test for FLAG_handler
Test(handlers, flag_handler, .init = setup_handler)
{
    // Configure option as boolean
    test_option.value_type = VALUE_TYPE_FLAG;
    test_option.value.as_bool = false;
    
    // Call boolean handler which should toggle the value
    int result = flag_handler(&test_argus, &test_option, NULL);
    
    // Verify handler succeeded
    cr_assert_eq(result, ARGUS_SUCCESS, "Bool handler should return success");
    
    // Verify value was toggled
    cr_assert_eq(test_option.value.as_bool, true, "Bool value should be true");
    
    // Call handler again
    result = flag_handler(&test_argus, &test_option, NULL);
    
    // Verify value was toggled again
    cr_assert_eq(test_option.value.as_bool, true, "Bool value should be true after second call");
}


// Test for bool_handler
Test(handlers, bool_handler, .init = setup_handler)
{
    int result;

    // Configure option as boolean
    test_option.value_type = VALUE_TYPE_BOOL;
    
    // Create test value
    char *true_value[] = {"true", "1", "yes", "on"};

    for (int i = 0; i < 4; i++) {
        // Call boolean handler
        result = bool_handler(&test_argus, &test_option, true_value[i]);
        
        // Verify handler succeeded
        cr_assert_eq(result, ARGUS_SUCCESS, "Bool handler should return success");
        
        // Verify value was set correctly
        cr_assert_eq(test_option.value.as_bool, true, "Bool value should be set correctly");
    }


    char *false_value[] = {"false", "0", "no", "off"};
    for (int i = 0; i < 4; i++) {
        // Call boolean handler
        result = bool_handler(&test_argus, &test_option, false_value[i]);
        
        // Verify handler succeeded
        cr_assert_eq(result, ARGUS_SUCCESS, "Bool handler should return success");
        
        // Verify value was set correctly
        cr_assert_eq(test_option.value.as_bool, false, "Bool value should be set correctly");
    }

    // Test with invalid value
    char invalid_value[] = "invalid";
    result = bool_handler(&test_argus, &test_option, invalid_value);
    cr_assert_eq(result, ARGUS_ERROR_INVALID_ARGUMENT, "Bool handler should return error for invalid value");
    cr_assert_eq(test_option.value.as_bool, false, "Bool value should not be set for invalid input");
 
    // Test with NULL value
    result = bool_handler(&test_argus, &test_option, NULL);
    cr_assert_eq(result, ARGUS_ERROR_INVALID_ARGUMENT, "Bool handler should return error for NULL value");
    cr_assert_eq(test_option.value.as_bool, false, "Bool value should not be set for NULL input");

}


// Test for string_handler
Test(handlers, string_handler, .init = setup_handler)
{
    // Configure option as string
    test_option.value_type = VALUE_TYPE_STRING;
    
    // Create test value
    char test_value[] = "test_string";
    
    // Call string handler
    int result = string_handler(&test_argus, &test_option, test_value);
    
    // Verify handler succeeded
    cr_assert_eq(result, ARGUS_SUCCESS, "String handler should return success");
    
    // Verify value was set correctly
    cr_assert_eq(test_option.value.as_string, test_value, "String value should be set correctly");
    cr_assert_str_eq(test_option.value.as_string, "test_string", "String content should match");
}

// Test for int_handler
Test(handlers, int_handler, .init = setup_handler)
{
    // Configure option as integer
    test_option.value_type = VALUE_TYPE_INT;
    
    // Create test value
    char test_value[] = "42";
    
    // Call integer handler
    int result = int_handler(&test_argus, &test_option, test_value);
    
    // Verify handler succeeded
    cr_assert_eq(result, ARGUS_SUCCESS, "Int handler should return success");
    
    // Verify value was set correctly
    cr_assert_eq(test_option.value.as_int, 42, "Int value should be set correctly");
    
    // Test with negative value
    char negative_value[] = "-100";
    result = int_handler(&test_argus, &test_option, negative_value);
    cr_assert_eq(result, ARGUS_SUCCESS, "Int handler should handle negative values");
    cr_assert_eq(test_option.value.as_int, -100, "Negative int value should be set correctly");
}

// Test for float_handler
Test(handlers, float_handler, .init = setup_handler)
{
    // Configure option as float
    test_option.value_type = VALUE_TYPE_FLOAT;
    
    // Create test value
    char test_value[] = "3.14159";
    
    // Call float handler
    int result = float_handler(&test_argus, &test_option, test_value);
    
    // Verify handler succeeded
    cr_assert_eq(result, ARGUS_SUCCESS, "Float handler should return success");
    
    // Verify value was set correctly (approximately)
    cr_assert_float_eq(test_option.value.as_float, 3.14159, 0.00001, "Float value should be set correctly");
    
    // Test with negative value
    char negative_value[] = "-2.718";
    result = float_handler(&test_argus, &test_option, negative_value);
    cr_assert_eq(result, ARGUS_SUCCESS, "Float handler should handle negative values");
    cr_assert_float_eq(test_option.value.as_float, -2.718, 0.00001, "Negative float value should be set correctly");
}

// Test for default_free
Test(handlers, default_free, .init = setup_handler)
{
    // Dynamically allocate a string
    test_option.value.as_string = strdup("test_string");
    
    // Verify string was allocated
    cr_assert_not_null(test_option.value.as_string, "String should be allocated");
    
    // Call default free handler
    int result = default_free(&test_option);
    
    // Verify handler succeeded
    cr_assert_eq(result, ARGUS_SUCCESS, "Default free handler should return success");
    
    // Note: We cannot verify that test_option.value.as_string is NULL
    // because default_free does not set it to NULL, it just frees the memory
}

// Test for array_string_handler (basic test)
Test(handlers, array_string_handler, .init = setup_handler)
{
    // Configure option as array of strings
    test_option.value_type = VALUE_TYPE_ARRAY_STRING;
    test_option.value.as_array = NULL;
    test_option.value_count = 0;
    test_option.value_capacity = 0;
    
    // Create test value with comma-separated elements
    char test_value[] = "one,two,three";
    
    // Call array string handler
    int result = array_string_handler(&test_argus, &test_option, test_value);
    
    // Verify handler succeeded
    cr_assert_eq(result, ARGUS_SUCCESS, "Array string handler should return success");
    
    // Verify array was created correctly
    cr_assert_not_null(test_option.value.as_array, "Array should be allocated");
    cr_assert_eq(test_option.value_count, 3, "Array should have 3 elements");
    cr_assert_str_eq(test_option.value.as_array[0].as_string, "one", "First element should be 'one'");
    cr_assert_str_eq(test_option.value.as_array[1].as_string, "two", "Second element should be 'two'");
    cr_assert_str_eq(test_option.value.as_array[2].as_string, "three", "Third element should be 'three'");
    
    // Free allocated memory
    for (size_t i = 0; i < test_option.value_count; i++) {
        free(test_option.value.as_array[i].as_string);
    }
    free(test_option.value.as_array);
}
