#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/types.h"
#include <string.h>

// Test argus context
static argus_t test_argus;

void setup_error(void)
{
    test_argus.program_name = "test_program";
    test_argus.error_stack.count = 0;
    for (size_t i = 0; i < ARGUS_MAX_ERRORS_STACK; i++) {
        test_argus.error_stack.errors[i].code = 0;
        test_argus.error_stack.errors[i].message[0] = '\0';
        test_argus.error_stack.errors[i].context = (argus_error_context_t){0};
    }
}

// Test for argus_strerror function
Test(error, argus_strerror)
{
    // Test common error codes
    cr_assert_str_eq(argus_strerror(ARGUS_SUCCESS), "Success");
    cr_assert_str_eq(argus_strerror(ARGUS_ERROR_DUPLICATE_OPTION), "Duplicate option");
    cr_assert_str_eq(argus_strerror(ARGUS_ERROR_INVALID_ARGUMENT), "Invalid argument");
    cr_assert_str_eq(argus_strerror(ARGUS_ERROR_MISSING_REQUIRED), "Missing required option");
    
    // Test invalid error code
    cr_assert_str_eq(argus_strerror(999), "Unknown error");
}

// Test for argus_push_error function
Test(error, argus_push_error, .init = setup_error)
{
    // Create a test error
    argus_error_t error = {
        .code = ARGUS_ERROR_INVALID_ARGUMENT,
        .context = {.option_name = "test_option", .group_name = NULL, .subcommand_name = NULL},
    };
    strcpy(error.message, "Test error message");
    
    // Add error to the stack
    argus_push_error(&test_argus, error);
    
    // Verify if error was added correctly
    cr_assert_eq(test_argus.error_stack.count, 1, "Error count should be 1");
    cr_assert_eq(test_argus.error_stack.errors[0].code, ARGUS_ERROR_INVALID_ARGUMENT, "Error code should match");
    cr_assert_str_eq(test_argus.error_stack.errors[0].message, "Test error message", "Error message should match");
    cr_assert_str_eq(test_argus.error_stack.errors[0].context.option_name, "test_option", "Option name should match");
    cr_assert_null(test_argus.error_stack.errors[0].context.group_name, "Group name should be NULL");
    cr_assert_null(test_argus.error_stack.errors[0].context.subcommand_name, "Subcommand name should be NULL");
}

// Test to verify behavior when error stack overflows
Test(error, argus_push_error_overflow, .init = setup_error)
{
    // Fill the error stack
    argus_error_t error = {
        .code = ARGUS_ERROR_INVALID_ARGUMENT,
        .context = {.option_name = "option", .group_name = NULL, .subcommand_name = NULL},
    };
    strcpy(error.message, "Test error");
    
    // Add errors until stack is full
    for (size_t i = 0; i < ARGUS_MAX_ERRORS_STACK; i++) {
        argus_push_error(&test_argus, error);
    }
    
    cr_assert_eq(test_argus.error_stack.count, ARGUS_MAX_ERRORS_STACK, "Error count should be max");
    
    // Add one more error
    error.code = ARGUS_ERROR_MEMORY;
    strcpy(error.message, "Overflow error");
    argus_push_error(&test_argus, error);
    
    // Verify that the last error was replaced with overflow error
    cr_assert_eq(test_argus.error_stack.count, ARGUS_MAX_ERRORS_STACK, "Error count should remain at max");
    cr_assert_eq(test_argus.error_stack.errors[ARGUS_MAX_ERRORS_STACK - 1].code, ARGUS_ERROR_STACK_OVERFLOW, 
                "Last error should be stack overflow");
}

// Test for ARGUS_COLLECT_ERROR macro
Test(error, argus_collect_error_macro, .init = setup_error)
{
    // Use macro to report an error
    ARGUS_COLLECT_ERROR(&test_argus, ARGUS_ERROR_MISSING_VALUE, "Missing value for option '%s'", "test_option");
    
    // Verify if error was collected correctly
    cr_assert_eq(test_argus.error_stack.count, 1, "Error count should be 1");
    cr_assert_eq(test_argus.error_stack.errors[0].code, ARGUS_ERROR_MISSING_VALUE, "Error code should match");
    cr_assert_str_eq(test_argus.error_stack.errors[0].message, "Missing value for option 'test_option'", 
                    "Error message should be formatted correctly");
}

// Function that uses ARGUS_REPORT_ERROR
int test_function(argus_t *argus, const char *opt_name) {
	ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_ARGUMENT, "Invalid argument: '%s'", opt_name);
}

// Test for ARGUS_REPORT_ERROR macro with stderr redirection
Test(error, argus_report_error_macro, .init = setup_error, .fini = cr_redirect_stderr)
{    
    // Redirect stderr to capture output
    cr_redirect_stderr();
    
    // Call function that reports error
    int result = test_function(&test_argus, "test_option");
    
    // Verify that function returned correct error code
    cr_assert_eq(result, ARGUS_ERROR_INVALID_ARGUMENT, "Should return the error code");
    
    // Verify that error counter was incremented
    cr_assert_eq(test_argus.error_stack.count, 1, "Error count should be incremented");
    
    // Note: Verifying stderr content would require additional configuration
    // and is not included in this basic test
}
