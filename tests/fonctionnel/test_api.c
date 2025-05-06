#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "argus.h"
#include <stdlib.h>
#include <string.h>

// Test options
ARGUS_OPTIONS(
    api_test_options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_INT('n', "number", HELP("A number"), DEFAULT(42)),
    OPTION_ARRAY_INT('a', "array", HELP("An array of numbers")),
    OPTION_MAP_INT('m', "map", HELP("A map of numbers")),
    POSITIONAL_STRING("input", HELP("Input file"))
)

// Subcommand options
ARGUS_OPTIONS(
    api_sub_options,
    HELP_OPTION(),
    OPTION_FLAG('d', "debug", HELP("Debug mode"))
)

// Test action handler
int test_action_called = 0;
int test_action(argus_t *argus, void *data) {
    (void)data;
    (void)argus;
    test_action_called = 1;
    return 0;
}

// Options with subcommand
ARGUS_OPTIONS(
    api_cmd_options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    SUBCOMMAND("sub", api_sub_options, HELP("Subcommand"), ACTION(test_action))
)

// Test argus_init
Test(api, argus_init)
{
    argus_t argus = argus_init(api_test_options, "test_program", "1.0.0");

    // Check that argus was initialized correctly
    cr_assert_str_eq(argus.program_name, "test_program", "Program name should be set correctly");
    cr_assert_str_eq(argus.version, "1.0.0", "Version should be set correctly");
    cr_assert_eq(argus.options, api_test_options, "Options should be set correctly");
    cr_assert_eq(argus.error_stack.count, 0, "Error stack should be empty");
    
    // Clean up
    argus_free(&argus);
}

// Test argus_parse with valid arguments
Test(api, argus_parse_valid)
{
    char *argv[] = {"test_program", "-v", "-o", "custom.txt", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(api_test_options, "test_program", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    // Check that parsing succeeded
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing valid arguments should succeed");
    
    // Check that options were set correctly
    cr_assert_eq(argus_get(argus, "verbose").as_bool, true, "Verbose option should be set");
    cr_assert_str_eq(argus_get(argus, "output").as_string, "custom.txt", "Output option should be set correctly");
    cr_assert_str_eq(argus_get(argus, "input").as_string, "input.txt", "Input option should be set correctly");
    
    // Check default values
    cr_assert_eq(argus_get(argus, "number").as_int, 42, "Number option should have default value");
    
    // Clean up
    argus_free(&argus);
}

// Test argus_parse with invalid arguments
Test(api, argus_parse_invalid, .init = cr_redirect_stderr)
{
    // Missing required positional argument
    char *argv[] = {"test_program", "-v"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(api_test_options, "test_program", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    // Check that parsing failed
    cr_assert_neq(status, ARGUS_SUCCESS, "Parsing invalid arguments should fail");
    
    // Clean up
    argus_free(&argus);
}

// Test argus_is_set function
Test(api, argus_is_set)
{
    char *argv[] = {"test_program", "-v", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(api_test_options, "test_program", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Check options that were set
    cr_assert_eq(argus_is_set(argus, "verbose"), true, "Verbose option should be set");
    cr_assert_eq(argus_is_set(argus, "input"), true, "Input option should be set");
    
    // Check options that were not set explicitly
    cr_assert_eq(argus_is_set(argus, "number"), true, "Number option should be set via default");
    
    // Check non-existent option
    cr_assert_eq(argus_is_set(argus, "nonexistent"), false, "Non-existent option should not be set");
    
    // Clean up
    argus_free(&argus);
}

// Test argus_has_command and argus_exec functions
Test(api, argus_has_command_and_exec)
{
    char *argv[] = {"test_program", "sub", "-d"};
    int argc = sizeof(argv) / sizeof(char *);
    
    test_action_called = 0; // Reset action flag
    
    argus_t argus = argus_init(api_cmd_options, "test_program", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    // Check that parsing succeeded
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing valid arguments should succeed");
    
    // Check that we have a command
    cr_assert_eq(argus_has_command(argus), true, "Should have a command");
    
    // Execute the command
    status = argus_exec(&argus, NULL);
    
    // Check that execution succeeded
    cr_assert_eq(status, 0, "Command execution should succeed");
    cr_assert_eq(test_action_called, 1, "Command action should have been called");
    
    // Clean up
    argus_free(&argus);
}

// Test argus_get function with different types
Test(api, argus_get_different_types)
{
    char *argv[] = {"test_program", "-v", "-n", "100", "-o", "file.txt", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(api_test_options, "test_program", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Check different value types
    cr_assert_eq(argus_get(argus, "verbose").as_bool, true, "Boolean value should be correct");
    cr_assert_eq(argus_get(argus, "number").as_int, 100, "Integer value should be correct");
    cr_assert_str_eq(argus_get(argus, "output").as_string, "file.txt", "String value should be correct");
    
    // Check non-existent option (should return empty value)
    cr_assert_eq(argus_get(argus, "nonexistent").raw, 0, "Non-existent option should return empty value");
    
    // Clean up
    argus_free(&argus);
}

// Test argus_count function
Test(api, argus_count)
{
    char *argv[] = {"test_program", "--verbose", "input.txt", "--array=1,2,3,4", \
                    "--map=one=1,two=2,three=3"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(api_test_options, "test_program", "1.0.0");
    argus_parse(&argus, argc, argv);

    // Check count for single-value options
    cr_assert_eq(argus_count(argus, "input"), 1, "Single value should have count 1");
    cr_assert_eq(argus_count(argus, "verbose"), 1, "Single value should have count 1");
    
    // Check count for array option
    cr_assert_eq(argus_count(argus, "array"), 4, "Array option should have count 4");

    // Check count for map option
    cr_assert_eq(argus_count(argus, "map"), 3, "Map option should have count 3");
    

    // Check count for non-existent option
    cr_assert_eq(argus_count(argus, "nonexistent"), 0, "Non-existent option should have count 0");

    // Clean up
    argus_free(&argus);
}

// Test argus_print functions (minimal test to ensure they don't crash)
Test(api, argus_print_functions, .init = cr_redirect_stdout)
{
    argus_t argus = argus_init(api_test_options, "test_program", "1.0.0");
    argus.description = "Test program for argus";
    
    // Test the print functions
    argus_print_help(argus);
    argus_print_usage(argus);
    argus_print_version(argus);
    
    // We don't assert on the output, just make sure they don't crash
    cr_assert(true, "Print functions should not crash");
    
    // Clean up
    argus_free(&argus);
}
