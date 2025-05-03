#include <criterion/criterion.h>
#include "argus/types.h"
#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/internal/context.h"
#include "argus.h"

// External functions to test
int validate_option(argus_t *argus, argus_option_t *options, argus_option_t *option);
int validate_positional(argus_t *argus, argus_option_t *option);
int validate_structure(argus_t *argus, argus_option_t *options);
int validate_subcommand(argus_t *argus, argus_option_t *option);
int validate_group(argus_t *argus, argus_option_t *option);

// Mock options for tests
ARGUS_OPTIONS(
    valid_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file")),
    POSITIONAL_STRING("input", HELP("Input file"))
)

ARGUS_OPTIONS(
    invalid_options, // Intentionally invalid, no help option
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"))
)

ARGUS_OPTIONS(
    duplicate_options, // Intentionally invalid, duplicate names
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('v', "verbose", HELP("Duplicate option")) // Same name and same short option
)

// Contexte argus pour les tests
static argus_t test_argus;

void setup_validation(void)
{
    test_argus.program_name = "test_program";
    test_argus.error_stack.count = 0;
    context_init(&test_argus);
}

// Test for validating a valid option
Test(validation, validate_valid_option, .init = setup_validation)
{
    argus_option_t option = {
        .type = TYPE_OPTION,
        .name = "test",
        .sname = 't',
        .lname = "test",
        .help = "Test option",
        .value_type = VALUE_TYPE_STRING,
        .handler = string_handler
    };
    
    int result = validate_option(&test_argus, valid_options, &option);
    cr_assert_eq(result, ARGUS_SUCCESS, "Valid option should pass validation");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors should be reported");
}

// Test for validating an invalid option
Test(validation, validate_invalid_option, .init = setup_validation)
{
    // Option without short or long name
    argus_option_t option = {
        .type = TYPE_OPTION,
        .name = "test",
        .sname = 0,
        .lname = NULL,
        .help = "Test option",
        .value_type = VALUE_TYPE_STRING,
        .handler = string_handler
    };
    
    int result = validate_option(&test_argus, valid_options, &option);
    cr_assert_neq(result, ARGUS_SUCCESS, "Option without short or long name should fail validation");
    cr_assert_eq(test_argus.error_stack.count, 1, "An error should be reported");
}

// Test for validating a valid positional option
Test(validation, validate_valid_positional, .init = setup_validation)
{
    argus_option_t option = {
        .type = TYPE_POSITIONAL,
        .name = "test",
        .help = "Test positional option",
        .value_type = VALUE_TYPE_STRING,
        .handler = string_handler,
        .flags = FLAG_REQUIRED
    };
    
    int result = validate_positional(&test_argus, &option);
    cr_assert_eq(result, ARGUS_SUCCESS, "Valid positional should pass validation");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors should be reported");
}

// Test for validating an invalid positional option
Test(validation, validate_invalid_positional, .init = setup_validation)
{
    // Positional option without name
    argus_option_t option = {
        .type = TYPE_POSITIONAL,
        .name = NULL,
        .help = "Test positional option",
        .value_type = VALUE_TYPE_STRING,
        .handler = string_handler,
        .flags = FLAG_REQUIRED
    };
    
    int result = validate_positional(&test_argus, &option);
    cr_assert_neq(result, ARGUS_SUCCESS, "Positional option without name should fail validation");
    cr_assert_eq(test_argus.error_stack.count, 1, "An error should be reported");
}

// Test for validating a valid group
Test(validation, validate_valid_group, .init = setup_validation)
{
    argus_option_t option = {
        .type = TYPE_GROUP,
        .name = "test_group",
        .help = "Test group"
    };
    
    int result = validate_group(&test_argus, &option);
    cr_assert_eq(result, ARGUS_SUCCESS, "Valid group should pass validation");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors should be reported");
}

// Test for validating a valid subcommand
Test(validation, validate_valid_subcommand, .init = setup_validation)
{
    argus_option_t option = {
        .type = TYPE_SUBCOMMAND,
        .name = "test_cmd",
        .help = "Test subcommand",
        .sub_options = valid_options
    };
    
    int result = validate_subcommand(&test_argus, &option);
    cr_assert_eq(result, ARGUS_SUCCESS, "Valid subcommand should pass validation");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors should be reported");
}

// Test for validating an invalid subcommand
Test(validation, validate_invalid_subcommand, .init = setup_validation)
{
    // Subcommand without options
    argus_option_t option = {
        .type = TYPE_SUBCOMMAND,
        .name = "test_cmd",
        .help = "Test subcommand",
        .sub_options = NULL
    };
    
    int result = validate_subcommand(&test_argus, &option);
    cr_assert_neq(result, ARGUS_SUCCESS, "Subcommand without options should fail validation");
    cr_assert_eq(test_argus.error_stack.count, 1, "An error should be reported");
}

// Test for validating a valid structure
Test(validation, validate_valid_structure, .init = setup_validation)
{
    int result = validate_structure(&test_argus, valid_options);
    cr_assert_eq(result, ARGUS_SUCCESS, "Valid structure should pass validation");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors should be reported");
}

// Test for validating an invalid structure
Test(validation, validate_invalid_structure, .init = setup_validation)
{
    int result = validate_structure(&test_argus, invalid_options);
    cr_assert_neq(result, ARGUS_SUCCESS, "Structure without help option should fail validation");
    cr_assert_gt(test_argus.error_stack.count, 0, "Errors should be reported");
}

// Test for validating a structure with duplicate options
Test(validation, validate_duplicate_options, .init = setup_validation)
{
	fprintf(stderr, "validating duplicate options\n");
    int result = validate_structure(&test_argus, duplicate_options);
	fprintf(stderr, "result: %d\n", result);
	fprintf(stderr, "count: %ld\n", test_argus.error_stack.count);
    cr_assert_neq(result, ARGUS_SUCCESS, "Structure with duplicate options should fail validation");
    cr_assert_gt(test_argus.error_stack.count, 0, "Errors should be reported");
}

// Test for validating a option without a helper
Test(validation, validate_option_without_help, .init = setup_validation)
{
    // Option without help message
    argus_option_t option = {
        .type = TYPE_OPTION,
        .name = "test",
        .sname = 't',
        .lname = "test",
        .help = NULL, // No help message
        .value_type = VALUE_TYPE_STRING,
        .handler = string_handler
    };
    
    int result = validate_option(&test_argus, valid_options, &option);
    cr_assert_neq(result, ARGUS_SUCCESS, "Option without help should fail validation");
    cr_assert_gt(test_argus.error_stack.count, 0, "Errors should be reported");

    // positionnal option without help
    argus_option_t pos_option = {
        .type = TYPE_POSITIONAL,
        .name = "test_pos",
        .help = NULL, // No help message
        .value_type = VALUE_TYPE_STRING,
        .handler = string_handler,
        .flags = FLAG_REQUIRED
    };
    result = validate_positional(&test_argus, &pos_option);
    cr_assert_neq(result, ARGUS_SUCCESS, "Positional option without help should fail validation");
    cr_assert_gt(test_argus.error_stack.count, 0, "Errors should be reported");

    // subcommand without help
    argus_option_t sub_option = {
        .type = TYPE_SUBCOMMAND,
        .name = "test_sub",
        .help = NULL, // No help message
        .sub_options = valid_options
    };
    result = validate_subcommand(&test_argus, &sub_option);
    cr_assert_neq(result, ARGUS_SUCCESS, "Subcommand without help should fail validation");
    cr_assert_gt(test_argus.error_stack.count, 0, "Errors should be reported");
}
