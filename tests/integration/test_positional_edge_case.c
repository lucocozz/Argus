#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "argus.h"
#include "argus/errors.h"
#include "argus/internal/parsing.h"

// Test options with positional arguments of different types
ARGUS_OPTIONS(
    positional_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    POSITIONAL_INT("number", HELP("A numeric value")),
    POSITIONAL_STRING("text", HELP("A text value"), FLAGS(FLAG_OPTIONAL))
)

// Test options for testing the -- separator
ARGUS_OPTIONS(
    separator_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", HELP("Verbose mode")),
    POSITIONAL_STRING("arg1", HELP("First argument")),
    POSITIONAL_STRING("arg2", HELP("Second argument"), FLAGS(FLAG_OPTIONAL))
)

// Test options for integer arrays
ARGUS_OPTIONS(
    array_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_ARRAY_INT('n', "numbers", HELP("List of numbers")),
    POSITIONAL_INT("count", HELP("A count value"))
)

// Helper function for output redirection
void setup_error_redirect(void)
{
    cr_redirect_stderr();
}

// Test handling positive numbers in positional arguments
Test(positional_args, positive_number)
{
    char *argv[] = {"test", "42"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(positional_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with positive number");
    cr_assert_eq(argus_get(argus, "number").as_int, 42, "Integer value should be correctly parsed");
    
    argus_free(&argus);
}

// Test handling negative numbers in positional arguments
Test(positional_args, negative_number, .init = setup_error_redirect)
{
    char *argv[] = {"test", "-42"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(positional_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with negative number");
	cr_assert_eq(argus_get(argus, "number").as_int, -42, "Negative integer value should be correctly parsed");
    
    argus_free(&argus);
}

// Test using -- to force treating negative numbers as positionals
Test(positional_args, negative_with_separator)
{
    char *argv[] = {"test", "--", "-42"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(positional_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with -- separator");
    cr_assert_eq(argus_get(argus, "number").as_int, -42, "Negative integer value should be correctly parsed after --");
    
    argus_free(&argus);
}

// Test using -- to force treating option-like arguments as positionals
Test(positional_args, option_like_with_separator)
{
    char *argv[] = {"test", "--", "--help"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(separator_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with -- separator");
    cr_assert_str_eq(argus_get(argus, "arg1").as_string, "--help", "Option-like string should be treated as positional after --");
    
    argus_free(&argus);
}

// Test using -- with multiple positional arguments
Test(positional_args, multiple_positionals_with_separator)
{
    char *argv[] = {"test", "--", "-42", "--text"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(positional_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with -- separator and multiple positionals");
    cr_assert_eq(argus_get(argus, "number").as_int, -42, "First positional should be parsed as -42");
    cr_assert_str_eq(argus_get(argus, "text").as_string, "--text", "Second positional should be parsed as --text");
    
    argus_free(&argus);
}

// Test mixing options and positionals with --
Test(positional_args, mixed_options_and_separator)
{
    char *argv[] = {"test", "-v", "--", "--arg"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(separator_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with mixed options and separator");
    cr_assert_eq(argus_get(argus, "verbose").as_bool, true, "Option before -- should be recognized");
    cr_assert_str_eq(argus_get(argus, "arg1").as_string, "--arg", "Argument after -- should be treated as positional");
    
    argus_free(&argus);
}

// Test array option with negative numbers
Test(positional_args, array_with_negative_numbers)
{
    char *argv[] = {"test", "--numbers=-1,-2,-3", "5"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(array_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with array of negative numbers");
    cr_assert_eq(argus_count(argus, "numbers"), 3, "Array should expand the range to 3 elements");
    cr_assert_eq(argus_array_get(argus, "numbers", 0).as_int, -1, "First element should be -1");
    cr_assert_eq(argus_array_get(argus, "numbers", 1).as_int, -2, "Second element should be -2");
    cr_assert_eq(argus_array_get(argus, "numbers", 2).as_int, -3, "Third element should be -3");
    cr_assert_eq(argus_get(argus, "count").as_int, 5, "Positional should be parsed correctly");
    
    argus_free(&argus);
}

// Test using negative numbers in ranges for array options
Test(positional_args, array_with_number_ranges)
{
    char *argv[] = {"test", "--numbers=-5--1", "5"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(array_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with number range");
    cr_assert_eq(argus_count(argus, "numbers"), 5, "Array should expand the range to 5 elements");
    cr_assert_eq(argus_array_get(argus, "numbers", 0).as_int, -5, "First element should be -5");
    cr_assert_eq(argus_array_get(argus, "numbers", 4).as_int, -1, "Last element should be -1");
    
    argus_free(&argus);
}
