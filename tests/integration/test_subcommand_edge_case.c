#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "argus.h"
#include "argus/errors.h"
#include "argus/internal/parsing.h"

// Forward declaration of action functions
int test_add_action(argus_t *argus, void *data);
int test_remove_action(argus_t *argus, void *data);
int test_nested_action(argus_t *argus, void *data);

// Action function flags for testing
static bool add_action_called = false;
static bool remove_action_called = false;
static bool nested_action_called = false;

// Options for nested subcommand tests
ARGUS_OPTIONS(
    nested_options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output in nested command")),
    POSITIONAL_INT("value", HELP("A numerical value"))
)

// Options for remove subcommand
ARGUS_OPTIONS(
    remove_options,
    HELP_OPTION(),
    OPTION_FLAG('r', "recursive", HELP("Recursive removal")),
    OPTION_FLAG('f', "force", HELP("Force removal")),
    POSITIONAL_STRING("path", HELP("Path to remove"))
)

// Options for add subcommand
ARGUS_OPTIONS(
    add_options,
    HELP_OPTION(),
    OPTION_STRING('m', "message", HELP("Commit message")),
    SUBCOMMAND("nested", nested_options, HELP("Nested subcommand"), ACTION(test_nested_action)),
    POSITIONAL_STRING("path", HELP("Path to add"), FLAGS(FLAG_OPTIONAL))
)

// Root command options
ARGUS_OPTIONS(
    cmd_options,
    HELP_OPTION(),
    VERSION_OPTION(),
    // Global options
    OPTION_FLAG('v', "verbose", HELP("Global verbose output")),
    // Subcommands
    SUBCOMMAND("add", add_options, HELP("Add files"), ACTION(test_add_action)),
    SUBCOMMAND("remove", remove_options, HELP("Remove files"), ACTION(test_remove_action))
)

// Options for option format testing
ARGUS_OPTIONS(
    format_options,
    HELP_OPTION(),
    OPTION_STRING('o', "output", HELP("Output file")),
    OPTION_INT('n', "number", HELP("A number")),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_FLAG('q', "quiet", HELP("Quiet mode"))
)

// Action function implementations
int test_add_action(argus_t *argus, void *data)
{
    (void)argus;
    (void)data;
    add_action_called = true;
    return 0;
}

int test_remove_action(argus_t *argus, void *data)
{
    (void)argus;
    (void)data;
    remove_action_called = true;
    return 0;
}

int test_nested_action(argus_t *argus, void *data)
{
    (void)argus;
    (void)data;
    nested_action_called = true;
    return 0;
}

void setup_subcommand(void)
{
    // Reset action flags before each test
    add_action_called = false;
    remove_action_called = false;
    nested_action_called = false;
}

// Test basic subcommand parsing
Test(subcommand_edge, basic_subcommand, .init = setup_subcommand)
{
    char *argv[] = {"test", "add", "--message=test", "file.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(cmd_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Basic subcommand parsing should succeed");
    cr_assert(argus_has_command(argus), "argus_has_command should return true");
    
    // Execute subcommand
    status = argus_exec(&argus, NULL);
    cr_assert_eq(status, 0, "Subcommand execution should succeed");
    cr_assert(add_action_called, "Add action should be called");
    cr_assert_not(remove_action_called, "Remove action should not be called");
    
    // Verify option values
    cr_assert_str_eq(argus_get(argus, "add.message").as_string, "test", 
                    "Subcommand option should be accessible");
    cr_assert_str_eq(argus_get(argus, "add.path").as_string, "file.txt", 
                    "Subcommand positional should be accessible");
    
    argus_free(&argus);
}

// Test nested subcommand parsing
Test(subcommand_edge, nested_subcommand, .init = setup_subcommand)
{
    char *argv[] = {"test", "add", "nested", "-v", "42"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(cmd_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Nested subcommand parsing should succeed");
    cr_assert(argus_has_command(argus), "argus_has_command should return true");
    
    // Execute subcommand
    status = argus_exec(&argus, NULL);
    cr_assert_eq(status, 0, "Subcommand execution should succeed");
    cr_assert_not(add_action_called, "Add action should not be called");
    cr_assert(nested_action_called, "Nested action should be called");
    
    // Verify option values
    cr_assert_eq(argus_get(argus, "add.nested.verbose").as_bool, true, 
                "Nested subcommand flag should be accessible");
    cr_assert_eq(argus_get(argus, "add.nested.value").as_int, 42, 
                "Nested subcommand positional should be accessible");
    
    argus_free(&argus);
}

// Test subcommand with global options
Test(subcommand_edge, global_options, .init = setup_subcommand)
{
    char *argv[] = {"test", "--verbose", "add", "path"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(cmd_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Subcommand with global options should succeed");
    cr_assert(argus_has_command(argus), "argus_has_command should return true");
    cr_assert_eq(argus_get(argus, "verbose").as_bool, true, 
                "Global option should be accessible");
    
    argus_free(&argus);
}

// Test invalid subcommand
Test(subcommand_edge, invalid_subcommand, .init = setup_subcommand)
{
    char *argv[] = {"test", "invalid", "path"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(cmd_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    // This likely fails with Unknown option or Unknown command
    cr_assert_neq(status, ARGUS_SUCCESS, "Invalid subcommand should fail");
    cr_assert_not(argus_has_command(argus), "argus_has_command should return false");
    
    argus_free(&argus);
}

// Test subcommand abbreviation
Test(subcommand_edge, subcommand_abbreviation, .init = setup_subcommand)
{
    char *argv[] = {"test", "rem", "--force", "path"};  // Short for "remove"
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(cmd_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Abbreviated subcommand should succeed");
    
    argus_free(&argus);
}

// Test negative numbers as positional args for subcommands
Test(subcommand_edge, subcommand_negative_number, .init = setup_subcommand)
{
    char *argv[] = {"test", "add", "nested", "-42"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(cmd_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Subcommand with negative number should succeed");
    cr_assert_eq(argus_get(argus, "add.nested.value").as_int, -42, 
                "Negative number should parse correctly");
    
    argus_free(&argus);
}

// Helper function for format tests
void setup_error_redirect(void)
{
    cr_redirect_stderr();
}

// Test different option formats
Test(option_formats, long_equal)
{
    char *argv[] = {"test", "--output=file.txt", "--number=42"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(format_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Long option with equal sign should succeed");
    cr_assert_str_eq(argus_get(argus, "output").as_string, "file.txt", 
                    "String value should be correctly parsed");
    cr_assert_eq(argus_get(argus, "number").as_int, 42, 
                "Integer value should be correctly parsed");
    
    argus_free(&argus);
}

// Test long option with separate value
Test(option_formats, long_separate)
{
    char *argv[] = {"test", "--output", "file.txt", "--number", "42"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(format_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Long option with separate value should succeed");
    cr_assert_str_eq(argus_get(argus, "output").as_string, "file.txt", 
                    "String value should be correctly parsed");
    cr_assert_eq(argus_get(argus, "number").as_int, 42, 
                "Integer value should be correctly parsed");
    
    argus_free(&argus);
}

// Test short option with attached value
Test(option_formats, short_attached)
{
    char *argv[] = {"test", "-ofile.txt", "-n42"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(format_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Short option with attached value should succeed");
    cr_assert_str_eq(argus_get(argus, "output").as_string, "file.txt", 
                    "String value should be correctly parsed");
    cr_assert_eq(argus_get(argus, "number").as_int, 42, 
                "Integer value should be correctly parsed");
    
    argus_free(&argus);
}

// Test short option with separate value
Test(option_formats, short_separate)
{
    char *argv[] = {"test", "-o", "file.txt", "-n", "42"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(format_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Short option with separate value should succeed");
    cr_assert_str_eq(argus_get(argus, "output").as_string, "file.txt", 
                    "String value should be correctly parsed");
    cr_assert_eq(argus_get(argus, "number").as_int, 42, 
                "Integer value should be correctly parsed");
    
    argus_free(&argus);
}

// Test combining short flags
Test(option_formats, combined_flags)
{
    char *argv[] = {"test", "-vq"};  // Combine verbose and quiet flags
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(format_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Combined short flags should succeed");
    cr_assert_eq(argus_get(argus, "verbose").as_bool, true, 
                "First flag should be set");
    cr_assert_eq(argus_get(argus, "quiet").as_bool, true, 
                "Second flag should be set");
    
    argus_free(&argus);
}

// Test value that looks like an option
Test(option_formats, value_like_option, .init = setup_error_redirect)
{
    char *argv[] = {"test", "--output", "--verbose"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(format_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
	cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed for value-like option");
    cr_assert_str_eq(argus_get(argus, "output").as_string, "--verbose", 
                    "Value should be treated as a string instead of a flag");

    argus_free(&argus);
}

// Test option with empty value
Test(option_formats, empty_value)
{
    char *argv[] = {"test", "--output="};  // Empty value
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(format_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Option with empty value should succeed");
    cr_assert_str_eq(argus_get(argus, "output").as_string, "", 
                    "Empty string value should be correctly parsed");
    
    argus_free(&argus);
}

// Test option with value that starts with a dash but is not an option
Test(option_formats, dash_value)
{
    char *argv[] = {"test", "--output", "-not-an-option"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(format_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Option with dash value should succeed");
    cr_assert_str_eq(argus_get(argus, "output").as_string, "-not-an-option", 
                    "Value starting with dash should be correctly parsed");
    
    argus_free(&argus);
}
