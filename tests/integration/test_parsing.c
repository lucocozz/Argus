#include <criterion/criterion.h>
#include "argus/types.h"
#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/internal/parsing.h"
#include "argus/internal/context.h"
#include "argus.h"

// Options for tests
ARGUS_OPTIONS(
    parse_options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file")),
    OPTION_INT('c', "count", HELP("Count value")),
    POSITIONAL_STRING("input", HELP("Input file")),
)

// Options for testing subcommands
ARGUS_OPTIONS(
    sub_parse_options,
    HELP_OPTION(),
    OPTION_FLAG('d', "debug", HELP("Debug mode")),
)

ARGUS_OPTIONS(
    cmd_options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    SUBCOMMAND("sub", sub_parse_options, 
               HELP("Subcommand")),
)

// Options with ambiguous subcommands for testing
ARGUS_OPTIONS(
    ambiguous_cmd_options,
    HELP_OPTION(),
    SUBCOMMAND("start", sub_parse_options, HELP("Start command")),
    SUBCOMMAND("status", sub_parse_options, HELP("Status command")),
    SUBCOMMAND("stop", sub_parse_options, HELP("Stop command")),
)

// Options with remove/remove-all ambiguity for testing
ARGUS_OPTIONS(
    remove_cmd_options,
    HELP_OPTION(),
    SUBCOMMAND("remove", sub_parse_options, HELP("Remove command")),
    SUBCOMMAND("remove-all", sub_parse_options, HELP("Remove all command")),
)

// Argus context for tests
static argus_t test_argus;

void setup_parsing(void)
{
    test_argus.program_name = "test_program";
    test_argus.options = parse_options;
    test_argus.error_stack.count = 0;
    context_init(&test_argus);
}

void setup_subcmd_parsing(void)
{
    test_argus.program_name = "test_program";
    test_argus.options = cmd_options;
    test_argus.error_stack.count = 0;
    context_init(&test_argus);
}

void setup_ambiguous_parsing(void)
{
    test_argus.program_name = "test_program";
    test_argus.options = ambiguous_cmd_options;
    test_argus.error_stack.count = 0;
    context_init(&test_argus);
}

void setup_remove_parsing(void)
{
    test_argus.program_name = "test_program";
    test_argus.options = remove_cmd_options;
    test_argus.error_stack.count = 0;
    context_init(&test_argus);
}

// Test for handle_long_option
Test(parsing, handle_long_option, .init = setup_parsing)
{
    char *argv[] = {"--output=test.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    int current_index = 0;
    
    // Handle long option with value
    int result = handle_long_option(&test_argus, parse_options, "output=test.txt", argv, argc, &current_index);
    
    cr_assert_eq(result, ARGUS_SUCCESS, "Long option with value should be handled successfully");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors should be reported");
    
    // Verify that option was correctly set
    argus_option_t *option = find_option_by_name(parse_options, "output");
    cr_assert_eq(option->is_set, true, "Option should be marked as set");
    cr_assert_str_eq(option->value.as_string, "test.txt", "Option value should be set correctly");
}

// Test for handle_short_option
Test(parsing, handle_short_option, .init = setup_parsing)
{
    char *argv[] = {"program", "-o", "test.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    int current_index = 1;
    
    // Handle short option with value
    int result = handle_short_option(&test_argus, parse_options, "o", argv, argc, &current_index);
    
    cr_assert_eq(result, ARGUS_SUCCESS, "Short option with value should be handled successfully");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors should be reported");
    cr_assert_eq(current_index, 2, "Index should be advanced to next argument");
    
    // Verify that option was correctly set
    argus_option_t *option = find_option_by_name(parse_options, "output");
    cr_assert_eq(option->is_set, true, "Option should be marked as set");
    cr_assert_str_eq(option->value.as_string, "test.txt", "Option value should be set correctly");
}

// Test for handle_positional
Test(parsing, handle_positional, .init = setup_parsing)
{
    // Handle positional argument
    int result = handle_positional(&test_argus, parse_options, "input.txt", 0);
    
    cr_assert_eq(result, ARGUS_SUCCESS, "Positional argument should be handled successfully");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors should be reported");
    
    // Verify that option was correctly set
    argus_option_t *option = find_option_by_name(parse_options, "input");
    cr_assert_eq(option->is_set, true, "Option should be marked as set");
    cr_assert_str_eq(option->value.as_string, "input.txt", "Option value should be set correctly");
    
    // Test invalid positional index
    result = handle_positional(&test_argus, parse_options, "extra.txt", 1);
    cr_assert_neq(result, ARGUS_SUCCESS, "Invalid positional index should fail");
}

Test(parsing, find_subcommand, .init = setup_subcmd_parsing)
{
    argus_option_t* option = NULL;
    int result = find_subcommand(&test_argus, cmd_options, "sub", &option);
    cr_assert_eq(result, ARGUS_SUCCESS, "Should successfully find subcommand");
    cr_assert_not_null(option, "Should find subcommand");
    cr_assert_str_eq(option->name, "sub", "Should find correct subcommand");
    cr_assert_eq(option->type, TYPE_SUBCOMMAND, "Found option should be subcommand");
    
    option = NULL;
    result = find_subcommand(&test_argus, cmd_options, "nonexistent", &option);
    cr_assert_eq(result, ARGUS_SUCCESS, "Should return success even for nonexistent");
    cr_assert_null(option, "Should return NULL for nonexistent subcommand");
}

// Test for ambiguous subcommands
Test(parsing, find_subcommand_ambiguous, .init = setup_ambiguous_parsing)
{
    // Test exact match - should work fine
    argus_option_t* option = NULL;
    int result = find_subcommand(&test_argus, ambiguous_cmd_options, "start", &option);
    cr_assert_eq(result, ARGUS_SUCCESS, "Should successfully find exact match");
    cr_assert_not_null(option, "Should find exact match subcommand");
    cr_assert_str_eq(option->name, "start", "Should find 'start' subcommand");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors for exact match");
    
    // Reset error stack
    test_argus.error_stack.count = 0;
    
    // Test ambiguous match - "sta" matches both "start" and "status"
    option = NULL;
    result = find_subcommand(&test_argus, ambiguous_cmd_options, "sta", &option);
    cr_assert_eq(result, ARGUS_ERROR_AMBIGUOUS_SUBCOMMAND, "Should return error for ambiguous match");
    cr_assert_eq(test_argus.error_stack.count, 1, "Should report one error for ambiguous match");
    
    // Reset error stack
    test_argus.error_stack.count = 0;
    
    // Test unambiguous partial match - "sto" only matches "stop"
    option = NULL;
    result = find_subcommand(&test_argus, ambiguous_cmd_options, "sto", &option);
    cr_assert_eq(result, ARGUS_SUCCESS, "Should successfully find unambiguous partial match");
    cr_assert_not_null(option, "Should find unambiguous partial match");
    cr_assert_str_eq(option->name, "stop", "Should find 'stop' subcommand");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors for unambiguous partial match");
}

// Test for remove/remove-all ambiguity
Test(parsing, find_subcommand_remove_ambiguity, .init = setup_remove_parsing)
{
    // Test exact match "remove" - should work fine
    argus_option_t* option = NULL;
    int result = find_subcommand(&test_argus, remove_cmd_options, "remove", &option);
    cr_assert_eq(result, ARGUS_SUCCESS, "Should successfully find 'remove'");
    cr_assert_not_null(option, "Should find exact match 'remove' subcommand");
    cr_assert_str_eq(option->name, "remove", "Should find 'remove' subcommand");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors for exact match 'remove'");
    
    // Reset error stack
    test_argus.error_stack.count = 0;
    
    // Test exact match "remove-all" - should work fine
    option = NULL;
    result = find_subcommand(&test_argus, remove_cmd_options, "remove-all", &option);
    cr_assert_eq(result, ARGUS_SUCCESS, "Should successfully find 'remove-all'");
    cr_assert_not_null(option, "Should find exact match 'remove-all' subcommand");
    cr_assert_str_eq(option->name, "remove-all", "Should find 'remove-all' subcommand");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors for exact match 'remove-all'");
    
    // Reset error stack
    test_argus.error_stack.count = 0;
    
    // Test ambiguous match "rem" - should detect ambiguity
    option = NULL;
    result = find_subcommand(&test_argus, remove_cmd_options, "rem", &option);
    cr_assert_eq(result, ARGUS_ERROR_AMBIGUOUS_SUBCOMMAND, "Should return error for ambiguous 'rem'");
    cr_assert_eq(test_argus.error_stack.count, 1, "Should report one error for ambiguous 'rem' match");
}

// Test for handle_subcommand
Test(parsing, handle_subcommand, .init = setup_subcmd_parsing)
{
    char *argv[] = {"-d"};
    int argc = sizeof(argv) / sizeof(char *);
    
    // Find subcommand to use
    argus_option_t *subcmd = NULL;
    int find_result = find_subcommand(&test_argus, cmd_options, "sub", &subcmd);
    cr_assert_eq(find_result, ARGUS_SUCCESS, "Should successfully find subcommand");
    cr_assert_not_null(subcmd, "Subcommand should exist");
    
    // Handle subcommand
    int result = handle_subcommand(&test_argus, subcmd, argc, argv);
    
    cr_assert_eq(result, ARGUS_SUCCESS, "Subcommand should be handled successfully");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors should be reported");
    cr_assert_eq(test_argus.context.subcommand_depth, 1, "Subcommand depth should be 1");
    cr_assert_eq(test_argus.context.subcommand_stack[0], subcmd, "Subcommand should be on the stack");
    
    // Verify that subcommand option was set
    argus_option_t *debug_option = find_option_by_name(sub_parse_options, "debug");
    cr_assert_eq(debug_option->is_set, true, "Subcommand option should be set");
}

// Simple test for parse_args
Test(parsing, parse_args_basic, .init = setup_parsing)
{
    char *argv[] = {"program", "-v", "-o", "output.txt", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    // Parse arguments
    int result = parse_args(&test_argus, parse_options, argc - 1, &argv[1]);
    
    cr_assert_eq(result, ARGUS_SUCCESS, "Arguments should be parsed successfully");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors should be reported");
    
    // Verify that options were correctly set
    argus_option_t *verbose = find_option_by_name(parse_options, "verbose");
    argus_option_t *output = find_option_by_name(parse_options, "output");
    argus_option_t *input = find_option_by_name(parse_options, "input");
    
    cr_assert_eq(verbose->is_set, true, "Verbose option should be set");
    cr_assert_eq(verbose->value.as_bool, true, "Verbose value should be true");
    
    cr_assert_eq(output->is_set, true, "Output option should be set");
    cr_assert_str_eq(output->value.as_string, "output.txt", "Output value should be correct");
    
    cr_assert_eq(input->is_set, true, "Input option should be set");
    cr_assert_str_eq(input->value.as_string, "input.txt", "Input value should be correct");
}

// Test for parse_args with long options
Test(parsing, parse_args_long_options, .init = setup_parsing)
{
    char *argv[] = {"program", "--verbose", "--output=output.txt", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    // Parse arguments
    int result = parse_args(&test_argus, parse_options, argc - 1, &argv[1]);
    
    cr_assert_eq(result, ARGUS_SUCCESS, "Arguments with long options should be parsed successfully");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors should be reported");
    
    // Verify that options were correctly set
    argus_option_t *verbose = find_option_by_name(parse_options, "verbose");
    argus_option_t *output = find_option_by_name(parse_options, "output");
    argus_option_t *input = find_option_by_name(parse_options, "input");
    
    cr_assert_eq(verbose->is_set, true, "Verbose option should be set");
    cr_assert_eq(output->is_set, true, "Output option should be set");
    cr_assert_str_eq(output->value.as_string, "output.txt", "Output value should be correct");
    cr_assert_eq(input->is_set, true, "Input option should be set");
}

// Test for parse_args with -- (end of options)
Test(parsing, parse_args_end_options, .init = setup_parsing)
{
    char *argv[] = {"program", "-v", "--", "-o"};
    int argc = sizeof(argv) / sizeof(char *);
    
    // Parse arguments
    int result = parse_args(&test_argus, parse_options, argc - 1, &argv[1]);

    cr_assert_eq(result, ARGUS_SUCCESS, "Arguments with -- should be parsed successfully");
    cr_assert_eq(test_argus.error_stack.count, 0, "No errors should be reported");
    
    // Verify that options were correctly set
    argus_option_t *verbose = find_option_by_name(parse_options, "verbose");
    argus_option_t *output = find_option_by_name(parse_options, "output");
    argus_option_t *input = find_option_by_name(parse_options, "input");
    
    cr_assert_eq(verbose->is_set, true, "Verbose option should be set");
    cr_assert_eq(output->is_set, false, "Output option should not be set");
    cr_assert_eq(input->is_set, true, "Input option should be set");
    cr_assert_str_eq(input->value.as_string, "-o", "Input value should be -o");
}
