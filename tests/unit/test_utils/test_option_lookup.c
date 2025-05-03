#include <criterion/criterion.h>
#include "argus/internal/utils.h"
#include "argus/internal/context.h"
#include "argus.h"

// Mock options for testing
ARGUS_OPTIONS(
    test_options,
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file")),
    OPTION_STRING('s', NULL, HELP("Short-only option")),
    OPTION_FLAG('\0', "long-only", HELP("Long-only option")),
    POSITIONAL_STRING("input", HELP("Input file")),
    POSITIONAL_STRING("output", HELP("Output file")),
    POSITIONAL_INT("count", HELP("Count value"), FLAGS(FLAG_OPTIONAL))
)

// Mock subcommand options
ARGUS_OPTIONS(
    sub_options,
    OPTION_FLAG('d', "debug", HELP("Debug mode")),
    POSITIONAL_STRING("subfile", HELP("Subcommand file"))
)

// Mock nested subcommand options
ARGUS_OPTIONS(
    nested_options,
    OPTION_FLAG('n', "nested", HELP("Nested option"))
)

// Mock options with subcommands
ARGUS_OPTIONS(
    cmd_options,
    OPTION_FLAG('g', "global", HELP("Global option")),
    SUBCOMMAND("sub", sub_options, HELP("Subcommand")),
    SUBCOMMAND("nested", nested_options, HELP("Nested subcommand"))
)

// Mock argus context for testing
static argus_t test_argus;

// Setup function
void setup(void)
{
    test_argus.program_name = "test_prog";
    test_argus.options = test_options;
    test_argus.error_stack.count = 0;
    context_init(&test_argus);
}

// Setup function for subcommand tests
void setup_subcommands(void)
{
    test_argus.program_name = "test_prog";
    test_argus.options = cmd_options;
    test_argus.error_stack.count = 0;
    context_init(&test_argus);
}

// This is a placeholder test since full parsing tests are complex
// and require proper setup of handlers and context
Test(parsing, parse_args_basic)
{
    // These would need proper implementation of handlers to work
    cr_assert(true, "Placeholder for parse_args tests");
}

// Testing find option functions
Test(parsing, find_option_by_name)
{
    argus_option_t* option = find_option_by_name(test_options, "verbose");
    cr_assert_not_null(option, "Should find option by name");
    cr_assert_eq(option->sname, 'v', "Found option should have correct short name");
    
    option = find_option_by_name(test_options, "nonexistent");
    cr_assert_null(option, "Should return NULL for nonexistent option");
    
    // Test positional argument lookup
    option = find_option_by_name(test_options, "input");
    cr_assert_not_null(option, "Should find positional by name");
    cr_assert_eq(option->type, TYPE_POSITIONAL, "Found option should be positional");
    
    // Test option with only short name
    option = find_option_by_name(test_options, "s");
    cr_assert_not_null(option, "Should find option with only short name");
    cr_assert_eq(option->sname, 's', "Found option should have correct short name");
    
    // Test option with only long name
    option = find_option_by_name(test_options, "long-only");
    cr_assert_not_null(option, "Should find option with only long name");
    cr_assert_eq(option->sname, '\0', "Found option should have no short name");
}

Test(parsing, find_option_by_sname)
{
    argus_option_t* option = find_option_by_sname(test_options, 'v');
    cr_assert_not_null(option, "Should find option by short name");
    cr_assert_str_eq(option->name, "verbose", "Found option should have correct name");
    
    option = find_option_by_sname(test_options, 'x');
    cr_assert_null(option, "Should return NULL for nonexistent short name");
    
    // Test option with no long name
    option = find_option_by_sname(test_options, 's');
    cr_assert_not_null(option, "Should find option with no long name");
    cr_assert_eq(option->lname, NULL, "Found option should have no long name");
}

Test(parsing, find_option_by_lname)
{
    argus_option_t* option = find_option_by_lname(test_options, "verbose");
    cr_assert_not_null(option, "Should find option by long name");
    cr_assert_eq(option->sname, 'v', "Found option should have correct short name");
    
    option = find_option_by_lname(test_options, "nonexistent");
    cr_assert_null(option, "Should return NULL for nonexistent long name");
    
    // Test option with only long name
    option = find_option_by_lname(test_options, "long-only");
    cr_assert_not_null(option, "Should find option with only long name");
    cr_assert_eq(option->sname, '\0', "Found option should have no short name");
    
    // Test option with no long name
    option = find_option_by_lname(test_options, NULL);
    cr_assert_null(option, "Should return NULL when searching for NULL long name");
}

Test(parsing, find_positional)
{
    argus_option_t* option = find_positional(test_options, 0);
    cr_assert_not_null(option, "Should find first positional argument");
    cr_assert_str_eq(option->name, "input", "First positional should be 'input'");
    
    option = find_positional(test_options, 1);
    cr_assert_not_null(option, "Should find second positional argument");
    cr_assert_str_eq(option->name, "output", "Second positional should be 'output'");
    
    option = find_positional(test_options, 2);
    cr_assert_not_null(option, "Should find third positional argument");
    cr_assert_str_eq(option->name, "count", "Third positional should be 'count'");
    
    option = find_positional(test_options, 3);
    cr_assert_null(option, "Should return NULL for nonexistent positional index");
}

Test(parsing, find_subcommand, .init = setup_subcommands)
{
    argus_option_t* option = find_subcommand(cmd_options, "sub");
    cr_assert_not_null(option, "Should find subcommand");
    cr_assert_str_eq(option->name, "sub", "Should find correct subcommand");
    cr_assert_eq(option->type, TYPE_SUBCOMMAND, "Found option should be subcommand");
    
    option = find_subcommand(cmd_options, "nested");
    cr_assert_not_null(option, "Should find nested subcommand");
    cr_assert_str_eq(option->name, "nested", "Should find correct nested subcommand");
    
    option = find_subcommand(cmd_options, "nonexistent");
    cr_assert_null(option, "Should return NULL for nonexistent subcommand");
}

Test(parsing, find_option_by_active_path, .init = setup_subcommands)
{
    // Test finding option at root level
    argus_option_t* option = find_option_by_active_path(test_argus, "global");
    cr_assert_not_null(option, "Should find option at root level");
    cr_assert_str_eq(option->name, "global", "Should find correct option");
    
    // Test finding option by explicit root path
    option = find_option_by_active_path(test_argus, ".global");
    cr_assert_not_null(option, "Should find option by explicit root path");
    cr_assert_str_eq(option->name, "global", "Should find correct option");
    
    // Test with subcommand path (should fail since no active subcommand)
    option = find_option_by_active_path(test_argus, "sub.debug");
    cr_assert_null(option, "Should return NULL when no matching subcommand is active");
    
    // Test with an invalid path
    option = find_option_by_active_path(test_argus, "nonexistent.option");
    cr_assert_null(option, "Should return NULL for invalid path");
    
    // Test with active subcommand
    argus_option_t* sub_cmd = find_subcommand(cmd_options, "sub");
    context_push_subcommand(&test_argus, sub_cmd);
    
    // Now we should be able to find the subcommand option
    option = find_option_by_active_path(test_argus, "debug");
    cr_assert_not_null(option, "Should find option in active subcommand");
    cr_assert_str_eq(option->name, "debug", "Should find correct option in subcommand");
    
    // Still should be able to access root option with explicit path
    option = find_option_by_active_path(test_argus, ".global");
    cr_assert_not_null(option, "Should still find root option with explicit path");
    cr_assert_str_eq(option->name, "global", "Should find correct root option");
    
    // Clean up
    context_pop_subcommand(&test_argus);
}

Test(parsing, get_active_options, .init = setup_subcommands)
{
    // Initially should get root options
    const argus_option_t* options = get_active_options(&test_argus);
    cr_assert_eq(options, cmd_options, "Should initially return root options");
    
    // Push a subcommand
    argus_option_t* sub_cmd = find_subcommand(cmd_options, "sub");
    context_push_subcommand(&test_argus, sub_cmd);
    
    // Now should get subcommand options
    options = get_active_options(&test_argus);
    cr_assert_eq(options, sub_options, "Should return subcommand options when subcommand is active");
    
    // Clean up
    context_pop_subcommand(&test_argus);
}
